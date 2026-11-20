import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.lang.foreign.Arena;
import java.lang.foreign.FunctionDescriptor;
import java.lang.foreign.Linker;
import java.lang.foreign.MemoryLayout;
import java.lang.foreign.MemorySegment;
import java.lang.foreign.StructLayout;
import java.lang.foreign.SymbolLookup;
import java.lang.invoke.MethodHandle;

import static java.lang.foreign.ValueLayout.ADDRESS;
import static java.lang.foreign.ValueLayout.JAVA_BYTE;
import static java.lang.foreign.ValueLayout.JAVA_INT;
import static java.lang.foreign.ValueLayout.JAVA_LONG;
import static java.lang.foreign.ValueLayout.JAVA_SHORT;

/**
 * Pure Java Video4Linux2 (V4L2) camera driver using the Foreign Function &amp; Memory (FFM) API.
 * <p>
 * Directly accesses Linux kernel V4L2 device nodes ({@code /dev/videoN}) using native system calls
 * ({@code open}, {@code ioctl}, {@code mmap}, {@code munmap}, {@code poll}, {@code close}) without
 * requiring native C helper libraries, FFmpeg binaries, or OpenCV bindings.
 * </p>
 * <p>
 * <b>Streaming architecture:</b>
 * <ul>
 *     <li>Requests memory-mapped kernel streaming buffers ({@code V4L2_MEMORY_MMAP}).</li>
 *     <li>Prefers uncompressed 16-bit YUYV (YUV 4:2:2) packed frames, converted directly to RGB.</li>
 *     <li>Falls back to Motion-JPEG (MJPEG), automatically injecting standard DHT Huffman tables
 *         if omitted by webcam firmware before handing to {@link ImageIO}.</li>
 *     <li>Cleanly stops streaming and unmaps buffers on {@link #close()} to release device locks immediately.</li>
 * </ul>
 * </p>
 */
final class V4l2 implements AutoCloseable {

    private static final Linker LINKER = Linker.nativeLinker();
    private static final SymbolLookup STD = LINKER.defaultLookup();
    private static final StructLayout CAP = Linker.Option.captureStateLayout();
    private static final long ERRNO_AT = CAP.byteOffset(MemoryLayout.PathElement.groupElement("errno"));

    private static final Linker.Option ERRNO_OPTION = Linker.Option.captureCallState("errno");

    private static MethodHandle downcall(String name, FunctionDescriptor fd, Linker.Option... opts) {
        return LINKER.downcallHandle(STD.find(name).orElseThrow(), fd, opts);
    }

    private static final MethodHandle OPEN = downcall("open", FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_INT), ERRNO_OPTION);
    private static final MethodHandle CLOSE = downcall("close", FunctionDescriptor.of(JAVA_INT, JAVA_INT));
    private static final MethodHandle IOCTL = downcall("ioctl", FunctionDescriptor.of(JAVA_INT, JAVA_INT, JAVA_LONG, ADDRESS), Linker.Option.firstVariadicArg(2), ERRNO_OPTION);
    private static final MethodHandle MMAP = downcall("mmap", FunctionDescriptor.of(ADDRESS, ADDRESS, JAVA_LONG, JAVA_INT, JAVA_INT, JAVA_INT, JAVA_LONG), ERRNO_OPTION);
    private static final MethodHandle MUNMAP = downcall("munmap", FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_LONG));
    private static final MethodHandle POLL = downcall("poll", FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_LONG, JAVA_INT), ERRNO_OPTION);
    private static final MethodHandle STRERROR = downcall("strerror", FunctionDescriptor.of(ADDRESS, JAVA_INT));

    // --- V4L2 ioctl command codes (64-bit Linux) ---
    private static final long VIDIOC_QUERYBUF = 0xC0585609L;
    private static final long VIDIOC_REQBUFS = 0xC0145608L;
    private static final long VIDIOC_S_FMT = 0xC0D05605L;
    private static final long VIDIOC_QBUF = 0xC058560FL;
    private static final long VIDIOC_DQBUF = 0xC0585611L;
    private static final long VIDIOC_STREAMON = 0x40045612L;
    private static final long VIDIOC_STREAMOFF = 0x40045613L;

    private static final int O_RDWR = 2;
    private static final int V4L2_BUF_TYPE_VIDEO_CAPTURE = 1;
    private static final int V4L2_MEMORY_MMAP = 1;

    // FourCC codes as 32-bit little-endian integers
    public static final int V4L2_PIX_FMT_YUYV = 0x56595559; // 'YUYV'
    public static final int V4L2_PIX_FMT_MJPEG = 0x47504A4D; // 'MJPG'

    private final Arena arena = Arena.ofShared();
    private final MemorySegment captureState = arena.allocate(CAP);
    private final MemorySegment v4l2Buffer = arena.allocate(88); // sizeof(struct v4l2_buffer)

    private final int fd;
    private final int pixelFormat;
    final int width;
    final int height;
    private final int stride;
    private final MemorySegment[] mappedBuffers;

    private boolean streaming = false;
    private boolean closed = false;
    private long lastFrameTimestamp = 0;

    private int errno() {
        return captureState.get(JAVA_INT, ERRNO_AT);
    }

    private String getErrorString(int err) {
        try {
            MemorySegment ptr = (MemorySegment) STRERROR.invoke(err);
            String nativeStr = ptr.reinterpret(200).getString(0);
            return switch (err) {
                case 16 -> "the camera is in use by another program (EBUSY)";
                case 13 -> "no permission to access the camera (EACCES; is the user in the 'video' group?)";
                case 2, 19 -> "no such camera device (ENOENT/ENODEV)";
                default -> nativeStr;
            };
        } catch (Throwable ignored) {
            return "error " + err;
        }
    }

    private int ioctl(long request, MemorySegment arg) throws IOException {
        try {
            return (int) IOCTL.invoke(captureState, fd, request, arg);
        } catch (Throwable t) {
            throw new IOException("ioctl failed for request 0x" + Long.toHexString(request), t);
        }
    }

    /**
     * Connects to a Video4Linux2 device node, negotiates pixel format, memory-maps buffers,
     * and starts the capture stream.
     *
     * @param deviceNode filesystem path to the device (e.g. {@code /dev/video0})
     * @param targetW    desired horizontal resolution in pixels
     * @param targetH    desired vertical resolution in pixels
     * @throws IOException if device access or configuration fails
     */
    V4l2(String deviceNode, int targetW, int targetH) throws IOException {
        int openedFd;
        try {
            openedFd = (int) OPEN.invoke(captureState, arena.allocateFrom(deviceNode), O_RDWR);
        } catch (Throwable t) {
            throw new IOException("Failed to invoke open on " + deviceNode, t);
        }

        if (openedFd < 0) {
            int err = errno();
            arena.close();
            throw new IOException("Cannot open " + deviceNode + ": " + getErrorString(err));
        }
        this.fd = openedFd;

        try {
            // Negotiation: struct v4l2_format (208 bytes)
            MemorySegment formatSegment = arena.allocate(208);
            int[][] formatCandidates = {
                    {targetW, targetH, V4L2_PIX_FMT_YUYV},
                    {640, 480, V4L2_PIX_FMT_YUYV},
                    {targetW, targetH, V4L2_PIX_FMT_MJPEG},
                    {640, 480, V4L2_PIX_FMT_MJPEG},
                    {320, 240, V4L2_PIX_FMT_YUYV}
            };

            int lastError = 0;
            boolean formatNegotiated = false;
            for (int[] candidate : formatCandidates) {
                formatSegment.fill((byte) 0);
                formatSegment.set(JAVA_INT, 0, V4L2_BUF_TYPE_VIDEO_CAPTURE);
                formatSegment.set(JAVA_INT, 8, candidate[0]); // width
                formatSegment.set(JAVA_INT, 12, candidate[1]); // height
                formatSegment.set(JAVA_INT, 16, candidate[2]); // pixelformat
                formatSegment.set(JAVA_INT, 20, 0); // field = V4L2_FIELD_ANY

                if (ioctl(VIDIOC_S_FMT, formatSegment) >= 0) {
                    formatNegotiated = true;
                    break;
                }
                lastError = errno();
            }

            if (!formatNegotiated) {
                throw new IOException("Cannot set picture format: " + getErrorString(lastError));
            }

            this.width = formatSegment.get(JAVA_INT, 8);
            this.height = formatSegment.get(JAVA_INT, 12);
            this.pixelFormat = formatSegment.get(JAVA_INT, 16);
            int bytesPerLine = formatSegment.get(JAVA_INT, 24);

            if (pixelFormat != V4L2_PIX_FMT_YUYV && pixelFormat != V4L2_PIX_FMT_MJPEG) {
                throw new IOException("Camera supports neither YUYV nor MJPEG (fourcc 0x" + Integer.toHexString(pixelFormat) + ")");
            }
            this.stride = (bytesPerLine > 0) ? bytesPerLine : width * 2;

            // Request 4 memory-mapped streaming buffers: struct v4l2_requestbuffers (20 bytes)
            MemorySegment reqBufs = arena.allocate(20);
            reqBufs.set(JAVA_INT, 0, 4); // count
            reqBufs.set(JAVA_INT, 4, V4L2_BUF_TYPE_VIDEO_CAPTURE);
            reqBufs.set(JAVA_INT, 8, V4L2_MEMORY_MMAP);

            if (ioctl(VIDIOC_REQBUFS, reqBufs) < 0) {
                throw new IOException("Cannot request buffers: " + getErrorString(errno()));
            }

            int bufferCount = reqBufs.get(JAVA_INT, 0);
            this.mappedBuffers = new MemorySegment[bufferCount];

            for (int i = 0; i < bufferCount; i++) {
                v4l2Buffer.fill((byte) 0);
                v4l2Buffer.set(JAVA_INT, 0, i); // index
                v4l2Buffer.set(JAVA_INT, 4, V4L2_BUF_TYPE_VIDEO_CAPTURE);
                v4l2Buffer.set(JAVA_INT, 60, V4L2_MEMORY_MMAP);

                if (ioctl(VIDIOC_QUERYBUF, v4l2Buffer) < 0) {
                    throw new IOException("Cannot query buffer " + i + ": " + getErrorString(errno()));
                }

                long length = v4l2Buffer.get(JAVA_INT, 72) & 0xFFFFFFFFL;
                long offset = v4l2Buffer.get(JAVA_INT, 64) & 0xFFFFFFFFL;

                MemorySegment mapping = (MemorySegment) MMAP.invoke(
                        captureState,
                        MemorySegment.NULL,
                        length,
                        3 /* PROT_READ | PROT_WRITE */,
                        1 /* MAP_SHARED */,
                        fd,
                        offset
                );

                if (mapping.address() == -1L) {
                    throw new IOException("Cannot mmap buffer " + i + ": " + getErrorString(errno()));
                }

                mappedBuffers[i] = mapping.reinterpret(length);

                if (ioctl(VIDIOC_QBUF, v4l2Buffer) < 0) {
                    throw new IOException("Cannot queue buffer " + i + ": " + getErrorString(errno()));
                }
            }

            // Start streaming: struct v4l2_buf_type
            MemorySegment streamType = arena.allocate(4);
            streamType.set(JAVA_INT, 0, V4L2_BUF_TYPE_VIDEO_CAPTURE);
            if (ioctl(VIDIOC_STREAMON, streamType) < 0) {
                throw new IOException("Cannot start video stream: " + getErrorString(errno()));
            }
            this.streaming = true;

        } catch (IOException e) {
            close();
            throw e;
        } catch (Throwable t) {
            close();
            throw new IOException("Initialization error", t);
        }
    }

    /**
     * Fetches the next available video frame, waiting up to {@code timeoutMs}.
     *
     * @param timeoutMs maximum time to wait for a frame in milliseconds
     * @param minGapMs  minimum interval between decoded frames (subsequent frames dropped cheaply)
     * @return the decoded {@link BufferedImage}, or {@code null} on timeout / dropped frame
     * @throws IOException if a device communication error occurs
     */
    BufferedImage next(int timeoutMs, int minGapMs) throws IOException {
        try {
            // struct pollfd (8 bytes: int fd, short events, short revents)
            MemorySegment pollFd = arena.allocate(8);
            pollFd.set(JAVA_INT, 0, fd);
            pollFd.set(JAVA_SHORT, 4, (short) 1 /* POLLIN */);

            int pollResult = (int) POLL.invoke(captureState, pollFd, 1L, timeoutMs);
            if (pollResult <= 0) {
                return null;
            }

            v4l2Buffer.fill((byte) 0);
            v4l2Buffer.set(JAVA_INT, 4, V4L2_BUF_TYPE_VIDEO_CAPTURE);
            v4l2Buffer.set(JAVA_INT, 60, V4L2_MEMORY_MMAP);

            if (ioctl(VIDIOC_DQBUF, v4l2Buffer) < 0) {
                int err = errno();
                if (err == 11 /* EAGAIN */) {
                    return null;
                }
                throw new IOException("Lost camera stream (DQBUF failed): " + getErrorString(err));
            }

            int index = v4l2Buffer.get(JAVA_INT, 0);
            int bytesUsed = v4l2Buffer.get(JAVA_INT, 8);

            BufferedImage frame = null;
            long now = System.currentTimeMillis();
            if (now - lastFrameTimestamp >= minGapMs && index >= 0 && index < mappedBuffers.length) {
                frame = decodeFrame(mappedBuffers[index], bytesUsed);
                lastFrameTimestamp = now;
            }

            // Re-queue buffer for subsequent capture
            if (ioctl(VIDIOC_QBUF, v4l2Buffer) < 0) {
                throw new IOException("Lost camera stream (QBUF failed): " + getErrorString(errno()));
            }

            return frame;
        } catch (IOException e) {
            throw e;
        } catch (Throwable t) {
            throw new IOException("Error capturing frame", t);
        }
    }

    /**
     * Decodes a captured raw frame into an RGB {@link BufferedImage}.
     */
    private BufferedImage decodeFrame(MemorySegment segment, int bytesUsed) throws IOException {
        if (pixelFormat == V4L2_PIX_FMT_MJPEG) {
            byte[] jpegBytes = segment.asSlice(0, bytesUsed).toArray(JAVA_BYTE);
            try {
                return ImageIO.read(new ByteArrayInputStream(Mjpeg.withTables(jpegBytes)));
            } catch (IOException | RuntimeException ignored) {
                return null; // Corrupted frame
            }
        }

        // Direct high-performance YUYV to RGB conversion
        BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        int[] rowPixels = new int[width];

        for (int y = 0; y < height; y++) {
            long rowOffset = (long) y * stride;
            for (int x = 0; x < width; x += 2) {
                int y0 = segment.get(JAVA_BYTE, rowOffset + x * 2L) & 0xFF;
                int u = (segment.get(JAVA_BYTE, rowOffset + x * 2L + 1) & 0xFF) - 128;
                int y1 = segment.get(JAVA_BYTE, rowOffset + x * 2L + 2) & 0xFF;
                int v = (segment.get(JAVA_BYTE, rowOffset + x * 2L + 3) & 0xFF) - 128;

                rowPixels[x] = yuvToRgb(y0, u, v);
                if (x + 1 < width) {
                    rowPixels[x + 1] = yuvToRgb(y1, u, v);
                }
            }
            img.setRGB(0, y, width, 1, rowPixels, 0, width);
        }

        return img;
    }

    /**
     * Converts YUV color components to a 24-bit 0xRRGGBB integer.
     */
    private static int yuvToRgb(int y, int u, int v) {
        int c = (y - 16) * 298 + 128;
        int r = Math.clamp((c + 409 * v) >> 8, 0, 255);
        int g = Math.clamp((c - 100 * u - 208 * v) >> 8, 0, 255);
        int b = Math.clamp((c + 516 * u) >> 8, 0, 255);
        return (r << 16) | (g << 8) | b;
    }

    /**
     * Stops the capture stream, unmaps kernel buffers, closes the file descriptor,
     * and releases all allocated memory.
     */
    @Override
    public void close() {
        if (closed) return;
        closed = true;

        try {
            if (streaming) {
                MemorySegment streamType = arena.allocate(4);
                streamType.set(JAVA_INT, 0, V4L2_BUF_TYPE_VIDEO_CAPTURE);
                ioctl(VIDIOC_STREAMOFF, streamType);
            }

            if (mappedBuffers != null) {
                for (MemorySegment m : mappedBuffers) {
                    if (m != null) {
                        MUNMAP.invoke(m, m.byteSize());
                    }
                }
            }
            CLOSE.invoke(fd);
        } catch (Throwable ignored) {
            // Suppress errors during cleanup
        } finally {
            arena.close();
        }
    }

    /**
     * Helper injecting missing standard Huffman tables (DHT) into MJPEG frames.
     */
    static final class Mjpeg {
        private static final int[] DC_LUM = {0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        private static final int[] DC_CHR = {0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        private static final int[] AC_LUM = {
                0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D,
                0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
                0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
                0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
                0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
                0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
                0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
                0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
                0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
                0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
                0xF9, 0xFA
        };
        private static final int[] AC_CHR = {
                0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77,
                0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
                0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
                0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
                0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
                0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
                0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
                0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
                0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
                0xF9, 0xFA
        };

        private static final byte[] DHT_DATA = buildDht();

        private static byte[] buildDht() {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            int[][] tables = {DC_LUM, AC_LUM, DC_CHR, AC_CHR};
            int[] tableIds = {0x00, 0x10, 0x01, 0x11};

            for (int k = 0; k < 4; k++) {
                out.write(0xFF);
                out.write(0xC4);
                int length = 2 + 1 + tables[k].length;
                out.write(length >> 8);
                out.write(length & 0xFF);
                out.write(tableIds[k]);
                for (int val : tables[k]) {
                    out.write(val);
                }
            }
            return out.toByteArray();
        }

        static byte[] withTables(byte[] jpegBytes) {
            // Check if DHT marker (0xFF, 0xC4) is already present
            for (int i = 2; i + 3 < jpegBytes.length; ) {
                if ((jpegBytes[i] & 0xFF) != 0xFF) break;
                int marker = jpegBytes[i + 1] & 0xFF;
                if (marker == 0xC4) return jpegBytes; // Already has DHT
                if (marker == 0xDA) break; // Start of scan (SOS)
                int segmentLength = ((jpegBytes[i + 2] & 0xFF) << 8) | (jpegBytes[i + 3] & 0xFF);
                i += 2 + segmentLength;
            }

            // Insert standard DHT immediately after SOI (0xFF, 0xD8)
            int insertOffset = 2;
            byte[] output = new byte[jpegBytes.length + DHT_DATA.length];
            System.arraycopy(jpegBytes, 0, output, 0, insertOffset);
            System.arraycopy(DHT_DATA, 0, output, insertOffset, DHT_DATA.length);
            System.arraycopy(jpegBytes, insertOffset, output, insertOffset + DHT_DATA.length, jpegBytes.length - insertOffset);
            return output;
        }
    }
}
