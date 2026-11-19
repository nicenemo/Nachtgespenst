import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.lang.foreign.*;
import java.lang.invoke.MethodHandle;
import static java.lang.foreign.ValueLayout.*;

/**
 * A webcam through Video4Linux2, in plain Java: the foreign function API calls open, ioctl, mmap and poll of libc (no ffmpeg, no native library).
 * The camera is asked for YUYV (raw, no decoder needed); a camera that only has MJPEG gets that, decoded by ImageIO. Closing the capture stops the
 * stream and closes the device at once, so another program (Teams) can have the camera again.
 */
final class V4l2 {
    private static final Linker L = Linker.nativeLinker();
    private static final SymbolLookup STD = L.defaultLookup();
    private static final StructLayout CAP = Linker.Option.captureStateLayout();
    private static final long ERRNO_AT = CAP.byteOffset(MemoryLayout.PathElement.groupElement("errno"));
    private static MethodHandle h(String name, FunctionDescriptor fd, Linker.Option... o) { return L.downcallHandle(STD.find(name).orElseThrow(), fd, o); }
    private static final Linker.Option ERRNO = Linker.Option.captureCallState("errno");
    private static final MethodHandle OPEN = h("open", FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_INT), ERRNO);
    private static final MethodHandle CLOSE = h("close", FunctionDescriptor.of(JAVA_INT, JAVA_INT));
    private static final MethodHandle IOCTL = h("ioctl", FunctionDescriptor.of(JAVA_INT, JAVA_INT, JAVA_LONG, ADDRESS), Linker.Option.firstVariadicArg(2), ERRNO);
    private static final MethodHandle MMAP = h("mmap", FunctionDescriptor.of(ADDRESS, ADDRESS, JAVA_LONG, JAVA_INT, JAVA_INT, JAVA_INT, JAVA_LONG), ERRNO);
    private static final MethodHandle MUNMAP = h("munmap", FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_LONG));
    private static final MethodHandle POLL = h("poll", FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_LONG, JAVA_INT), ERRNO);
    private static final MethodHandle STRERROR = h("strerror", FunctionDescriptor.of(ADDRESS, JAVA_INT));

    // ioctl numbers of Video4Linux2 on 64-bit Linux: direction, size of the structure, 'V', number
    private static final long QUERYBUF = 0xC0585609L, REQBUFS = 0xC0145608L, S_FMT = 0xC0D05605L, QBUF = 0xC058560FL, DQBUF = 0xC0585611L, STREAMON = 0x40045612L, STREAMOFF = 0x40045613L;
    private static final int BUF_TYPE_CAPTURE = 1, MEMORY_MMAP = 1;
    private static final int YUYV = 0x56595559, MJPG = 0x47504A4D;       // 'YUYV' and 'MJPG' as little-endian four-character codes

    private final Arena arena = Arena.ofShared();
    private final MemorySegment cap = arena.allocate(CAP), buf = arena.allocate(88);
    private final int fd; private final int fmt; final int width, height; private final int stride;
    private final MemorySegment[] maps; private boolean streaming, closed;
    private long last;

    private int call(MethodHandle hnd, Object... args) throws IOException {
        try {
            return switch (args.length) {
                case 2 -> (int) hnd.invoke(cap, args[0], args[1]);
                default -> throw new IllegalStateException();
            };
        } catch (Throwable t) { throw new IOException(t); }
    }

    private String errText(int e) {
        try { MemorySegment s = (MemorySegment) STRERROR.invoke(e); String t = s.reinterpret(200).getString(0);
            return switch (e) { case 16 -> "the camera is in use by another program"; case 13 -> "no permission for the camera (is the user in the video group?)"; case 2, 19 -> "no such camera"; default -> t; };
        } catch (Throwable t) { return "error " + e; }
    }
    private int errno() { return cap.get(JAVA_INT, ERRNO_AT); }

    private int ioctl(long req, MemorySegment arg) throws IOException {
        try { return (int) IOCTL.invoke(cap, fd, req, arg); } catch (Throwable t) { throw new IOException(t); }
    }

    /** Opens the camera, sets the picture format (about 640 x 360), maps four buffers and starts the stream. Throws an IOException with a readable reason. */
    V4l2(String node, int wantW, int wantH) throws IOException {
        int f;
        try { f = (int) OPEN.invoke(cap, arena.allocateFrom(node), 2 /* O_RDWR */); } catch (Throwable t) { throw new IOException(t); }
        if (f < 0) { int e = errno(); arena.close(); throw new IOException("cannot open " + node + ": " + errText(e)); }
        fd = f;
        try {
            MemorySegment fm = arena.allocate(208);                                  // struct v4l2_format
            int[][] tries = {{wantW, wantH, YUYV}, {640, 480, YUYV}, {wantW, wantH, MJPG}, {640, 480, MJPG}, {320, 240, YUYV}};        // some cameras refuse the first size or format
            int lastErr = 0; boolean set = false;
            for (int[] t : tries) {
                fm.fill((byte) 0); fm.set(JAVA_INT, 0, BUF_TYPE_CAPTURE); fm.set(JAVA_INT, 8, t[0]); fm.set(JAVA_INT, 12, t[1]); fm.set(JAVA_INT, 16, t[2]); fm.set(JAVA_INT, 20, 0);
                if (ioctl(S_FMT, fm) >= 0) { set = true; break; }
                lastErr = errno();
            }
            if (!set) throw new IOException("cannot set the picture format: " + errText(lastErr));
            width = fm.get(JAVA_INT, 8); height = fm.get(JAVA_INT, 12); fmt = fm.get(JAVA_INT, 16); int bpl = fm.get(JAVA_INT, 24);
            if (fmt != YUYV && fmt != MJPG) throw new IOException("the camera has neither YUYV nor MJPEG (format " + Integer.toHexString(fmt) + ")");
            stride = bpl > 0 ? bpl : width * 2;
            MemorySegment rb = arena.allocate(20);                                   // struct v4l2_requestbuffers
            rb.set(JAVA_INT, 0, 4); rb.set(JAVA_INT, 4, BUF_TYPE_CAPTURE); rb.set(JAVA_INT, 8, MEMORY_MMAP);
            if (ioctl(REQBUFS, rb) < 0) throw new IOException("cannot get buffers: " + errText(errno()));
            int n = rb.get(JAVA_INT, 0); maps = new MemorySegment[n];
            for (int i = 0; i < n; i++) {
                buf.fill((byte) 0); buf.set(JAVA_INT, 0, i); buf.set(JAVA_INT, 4, BUF_TYPE_CAPTURE); buf.set(JAVA_INT, 60, MEMORY_MMAP);
                if (ioctl(QUERYBUF, buf) < 0) throw new IOException("cannot query a buffer: " + errText(errno()));
                long len = buf.get(JAVA_INT, 72) & 0xFFFFFFFFL, off = buf.get(JAVA_INT, 64) & 0xFFFFFFFFL;
                MemorySegment m = (MemorySegment) MMAP.invoke(cap, MemorySegment.NULL, len, 3 /* read and write */, 1 /* shared */, fd, off);
                if (m.address() == -1L) throw new IOException("cannot map a buffer: " + errText(errno()));
                maps[i] = m.reinterpret(len);
                if (ioctl(QBUF, buf) < 0) throw new IOException("cannot queue a buffer: " + errText(errno()));
            }
            MemorySegment type = arena.allocate(4); type.set(JAVA_INT, 0, BUF_TYPE_CAPTURE);
            if (ioctl(STREAMON, type) < 0) throw new IOException("cannot start the stream: " + errText(errno()));
            streaming = true;
        } catch (IOException e) { close(); throw e;
        } catch (Throwable t) { close(); throw new IOException(t); }
    }

    /** The next picture, or null when none came within timeoutMs. At most one picture every minGapMs is decoded; the others are dropped (cheap). */
    BufferedImage next(int timeoutMs, int minGapMs) throws IOException {
        try {
            MemorySegment pfd = arena.allocate(8); pfd.set(JAVA_INT, 0, fd); pfd.set(JAVA_SHORT, 4, (short) 1 /* POLLIN */);
            int r = (int) POLL.invoke(cap, pfd, 1L, timeoutMs);
            if (r <= 0) return null;
            buf.fill((byte) 0); buf.set(JAVA_INT, 4, BUF_TYPE_CAPTURE); buf.set(JAVA_INT, 60, MEMORY_MMAP);
            if (ioctl(DQBUF, buf) < 0) { int e = errno(); if (e == 11 /* EAGAIN */) return null; throw new IOException("lost the camera: " + errText(e)); }
            int i = buf.get(JAVA_INT, 0), used = buf.get(JAVA_INT, 8);
            BufferedImage img = null;
            long now = System.currentTimeMillis();
            if (now - last >= minGapMs && i >= 0 && i < maps.length) { img = decode(maps[i], used); last = now; }
            if (ioctl(QBUF, buf) < 0) throw new IOException("lost the camera: " + errText(errno()));
            return img;
        } catch (IOException e) { throw e; } catch (Throwable t) { throw new IOException(t); }
    }

    private BufferedImage decode(MemorySegment m, int used) throws IOException {
        if (fmt == MJPG) {
            byte[] jpg = m.asSlice(0, used).toArray(JAVA_BYTE);
            try { return ImageIO.read(new ByteArrayInputStream(Mjpeg.withTables(jpg))); } catch (IOException | RuntimeException e) { return null; }     // a damaged picture: skip it
        }
        BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        int[] px = new int[width];
        for (int y = 0; y < height; y++) {
            long row = (long) y * stride;
            for (int x = 0; x < width; x += 2) {
                int y0 = m.get(JAVA_BYTE, row + x * 2L) & 255, u = (m.get(JAVA_BYTE, row + x * 2L + 1) & 255) - 128, y1 = m.get(JAVA_BYTE, row + x * 2L + 2) & 255, v = (m.get(JAVA_BYTE, row + x * 2L + 3) & 255) - 128;
                px[x] = rgb(y0, u, v); if (x + 1 < width) px[x + 1] = rgb(y1, u, v);
            }
            img.setRGB(0, y, width, 1, px, 0, width);
        }
        return img;
    }
    private static int rgb(int y, int u, int v) {
        int c = (y - 16) * 298 + 128;
        int r = clamp((c + 409 * v) >> 8), g = clamp((c - 100 * u - 208 * v) >> 8), b = clamp((c + 516 * u) >> 8);
        return (r << 16) | (g << 8) | b;
    }
    private static int clamp(int v) { return v < 0 ? 0 : Math.min(255, v); }

    /** Stops the stream, unmaps the buffers and closes the device: the camera is free again. */
    void close() {
        if (closed) return; closed = true;
        try {
            if (streaming) { MemorySegment type = arena.allocate(4); type.set(JAVA_INT, 0, BUF_TYPE_CAPTURE); ioctl(STREAMOFF, type); }
            if (maps != null) for (MemorySegment m : maps) if (m != null) MUNMAP.invoke(m, m.byteSize());
            CLOSE.invoke(fd);
        } catch (Throwable ignored) { }
        arena.close();
    }

    /** Many webcams leave the Huffman tables out of their MJPEG pictures (they are implied); ImageIO needs them, so the standard tables are put in when they are missing. */
    static final class Mjpeg {
        private static final int[] DC_LUM = {0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0, 0,1,2,3,4,5,6,7,8,9,10,11};
        private static final int[] DC_CHR = {0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0, 0,1,2,3,4,5,6,7,8,9,10,11};
        private static final int[] AC_LUM = {0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,0x7D,
            0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,
            0x24,0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,
            0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,
            0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,
            0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA};
        private static final int[] AC_CHR = {0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,0x77,
            0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,0x23,0x33,0x52,0xF0,
            0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,0x1A,0x26,0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,
            0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,
            0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,
            0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA};
        private static final byte[] DHT = build();
        private static byte[] build() {
            java.io.ByteArrayOutputStream o = new java.io.ByteArrayOutputStream();
            int[][] t = {DC_LUM, AC_LUM, DC_CHR, AC_CHR}; int[] id = {0x00, 0x10, 0x01, 0x11};
            for (int k = 0; k < 4; k++) { o.write(0xFF); o.write(0xC4); int len = 2 + 1 + t[k].length; o.write(len >> 8); o.write(len & 255); o.write(id[k]); for (int v : t[k]) o.write(v); }
            return o.toByteArray();
        }
        static byte[] withTables(byte[] jpg) {
            for (int i = 2; i + 3 < jpg.length; ) {                                 // walk the segments up to the scan: is there a Huffman table (DHT)?
                if ((jpg[i] & 255) != 0xFF) break;
                int m = jpg[i + 1] & 255; if (m == 0xC4) return jpg; if (m == 0xDA) break;
                i += 2 + (((jpg[i + 2] & 255) << 8) | (jpg[i + 3] & 255));
            }
            int at = 2;                                                              // after the SOI marker (and before everything else is fine)
            byte[] out = new byte[jpg.length + DHT.length];
            System.arraycopy(jpg, 0, out, 0, at); System.arraycopy(DHT, 0, out, at, DHT.length); System.arraycopy(jpg, at, out, at + DHT.length, jpg.length - at);
            return out;
        }
    }
}
