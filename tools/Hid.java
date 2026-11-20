import java.io.IOException;
import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.nio.file.Files;
import java.nio.file.Path;

import static java.lang.foreign.ValueLayout.JAVA_BYTE;

/**
 * Interface to a Linux raw HID device node ({@code /dev/hidrawN}).
 * <p>
 * Handles low-level communication with the USB/Bluetooth HID endpoint of Nachtgespenst boards:
 * <ul>
 *     <li>Output reports via standard {@code write()} system calls (report ID in byte 0)</li>
 *     <li>Feature reports via {@code HIDIOCSFEATURE} and {@code HIDIOCGFEATURE} ioctls</li>
 *     <li>Sysfs auto-discovery matching board hardware tags and transport bus types</li>
 * </ul>
 * </p>
 */
final class Hid implements AutoCloseable {

    // Linux _IOC macro encoding:
    // _IOC(dir, type, nr, size) = (dir << 30) | (size << 16) | (type << 8) | nr
    // _IOC_READ | _IOC_WRITE = 3 (0xC0000000L), type = 'H' (0x48)
    private static final long IOC_DIR_RW = 0xC0000000L;
    private static final long IOC_TYPE_HID = 0x48L << 8;
    private static final int IOC_NR_SET_FEATURE = 0x06;
    private static final int IOC_NR_GET_FEATURE = 0x07;

    final int fd;
    final String path;
    private final Arena arena = Arena.ofShared();
    private final MemorySegment ioBuffer = arena.allocate(1024);

    /**
     * Constructs an ioctl request code for HID feature reports.
     *
     * @param nr   the ioctl command number (0x06 for set, 0x07 for get)
     * @param size size in bytes of the report transfer
     * @return the encoded 64-bit ioctl request code
     */
    private static long hidIoctl(int nr, int size) {
        return IOC_DIR_RW | ((long) size << 16) | IOC_TYPE_HID | nr;
    }

    /**
     * Opens a raw HID device node for reading and writing.
     *
     * @param path path to the device node (e.g. {@code /dev/hidraw0})
     * @throws IOException if the device cannot be opened or permission is denied
     */
    Hid(String path) throws IOException {
        this.path = path;
        try {
            this.fd = (int) Libc.OPEN.invoke(Libc.CAPTURE, Libc.ARENA.allocateFrom(path), Libc.O_RDWR);
        } catch (Throwable t) {
            throw new IOException("Failed to invoke open on " + path, t);
        }

        if (fd < 0) {
            int err = Libc.errno();
            String hint = (err == 13)
                    ? ": permission denied, see docs/wiki/hardware-testing.md for the hidraw access rule"
                    : "";
            throw new IOException("Cannot open " + path + " (errno " + err + hint + ")");
        }
    }

    /**
     * Sends an HID output report to the device.
     * <p>
     * Byte 0 of the buffer must be the HID Report ID.
     * </p>
     *
     * @param data the report bytes to transmit
     * @return the number of bytes written, or negative {@code -errno} on failure
     */
    synchronized int write(byte[] data) {
        try {
            MemorySegment buf = (data.length <= ioBuffer.byteSize())
                    ? ioBuffer
                    : arena.allocate(data.length);
            buf.copyFrom(MemorySegment.ofArray(data));

            long written = (long) Libc.WRITE.invoke(Libc.CAPTURE, fd, buf, (long) data.length);
            return written < 0 ? -Libc.errno() : (int) written;
        } catch (Throwable t) {
            throw new RuntimeException("Error writing output report to " + path, t);
        }
    }

    /**
     * Retrieves an HID feature report from the device.
     *
     * @param id         the HID report ID to request
     * @param payloadLen expected length of the report payload (excluding report ID)
     * @return the report data including report ID at index 0, or {@code null} on failure
     */
    synchronized byte[] getFeature(int id, int payloadLen) {
        int totalLen = payloadLen + 1;
        try {
            MemorySegment buf = (totalLen <= ioBuffer.byteSize())
                    ? ioBuffer
                    : arena.allocate(totalLen);
            buf.fill((byte) 0);
            buf.set(JAVA_BYTE, 0, (byte) id);

            long req = hidIoctl(IOC_NR_GET_FEATURE, totalLen);
            int res = (int) Libc.IOCTL.invoke(Libc.CAPTURE, fd, req, buf);
            if (res < 0) {
                return null;
            }
            return buf.asSlice(0, totalLen).toArray(JAVA_BYTE);
        } catch (Throwable t) {
            throw new RuntimeException("Error getting feature report id=" + id, t);
        }
    }

    /**
     * Sends an HID feature report to the device.
     *
     * @param data the feature report bytes (byte 0 is the report ID)
     * @return {@code true} if accepted by the device, {@code false} otherwise
     */
    synchronized boolean setFeature(byte[] data) {
        try {
            MemorySegment buf = (data.length <= ioBuffer.byteSize())
                    ? ioBuffer
                    : arena.allocate(data.length);
            buf.copyFrom(MemorySegment.ofArray(data));

            long req = hidIoctl(IOC_NR_SET_FEATURE, data.length);
            int res = (int) Libc.IOCTL.invoke(Libc.CAPTURE, fd, req, buf);
            return res >= 0;
        } catch (Throwable t) {
            throw new RuntimeException("Error setting feature report", t);
        }
    }

    /**
     * Closes the raw HID device node and releases allocated memory segments.
     */
    @Override
    public void close() {
        try {
            Libc.close(fd);
        } finally {
            arena.close();
        }
    }

    /**
     * Discovers the {@code /dev/hidrawN} node corresponding to a Nachtgespenst board.
     * <p>
     * Inspects sysfs properties ({@code /sys/class/hidraw/hidraw*}) to locate a device
     * matching the vendor/product token and connection type:
     * <ul>
     *     <li>Bus {@code 0003}: USB connection</li>
     *     <li>Bus {@code 0005}: Bluetooth Low Energy connection</li>
     * </ul>
     * </p>
     *
     * @param tag the board identifier (e.g. {@code "S3"} or {@code "C2"})
     * @param usb {@code true} for USB connection, {@code false} for Bluetooth
     * @return the device node path (e.g. {@code "/dev/hidraw2"}), or {@code null} if not found
     * @throws IOException if sysfs traversal fails
     */
    static String find(String tag, boolean usb) throws IOException {
        String best = null;
        int bestN = -1;
        Path hidrawDir = Path.of("/sys/class/hidraw");
        if (!Files.isDirectory(hidrawDir)) {
            return null;
        }

        try (var ds = Files.newDirectoryStream(hidrawDir, "hidraw*")) {
            for (Path p : ds) {
                Path ueventFile = p.resolve("device/uevent");
                if (!Files.exists(ueventFile)) {
                    continue;
                }

                String hidId = "";
                String hidName = "";
                for (String line : Files.readAllLines(ueventFile)) {
                    if (line.startsWith("HID_ID=")) {
                        hidId = line.substring(7);
                    } else if (line.startsWith("HID_NAME=")) {
                        hidName = line.substring(9);
                    }
                }

                boolean isUsb = hidId.startsWith("0003");
                boolean isBle = hidId.startsWith("0005");
                String token = usb
                        ? "NG-" + tag
                        : (tag.equals("S3") ? "-cs3" : "-c2");

                if (hidName.contains(token) && (usb ? isUsb : isBle)) {
                    String fileName = p.getFileName().toString();
                    int num = Integer.parseInt(fileName.substring(6));
                    if (num > bestN) {
                        bestN = num;
                        best = "/dev/" + fileName;
                    }
                }
            }
        }
        return best;
    }
}
