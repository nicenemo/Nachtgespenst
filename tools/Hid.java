import com.fazecast.jSerialComm.SerialPort;
import java.io.IOException;
import java.lang.foreign.*;
import java.lang.invoke.MethodHandle;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import static java.lang.foreign.ValueLayout.*;

    /** A /dev/hidrawN node: output reports with write(), feature reports with the HIDIOCSFEATURE and HIDIOCGFEATURE ioctls. */
    final class Hid implements AutoCloseable {
        final int fd; final String path;
        Hid(String path) throws IOException {
            this.path = path;
            try {
                fd = (int) Libc.OPEN.invoke(Libc.CAPTURE, Libc.ARENA.allocateFrom(path), 2);   // O_RDWR
            } catch (Throwable t) { throw new IOException(t); }
            if (fd < 0) throw new IOException("cannot open " + path + " (errno " + Libc.errno() + (Libc.errno() == 13 ? ": permission denied, see docs/wiki/hardware-testing.md for the hidraw access rule" : "") + ")");
        }
        /** Output report: byte 0 is the report ID. Returns bytes written or -errno. */
        int write(byte[] b) {
            try {
                Libc.BUF.copyFrom(MemorySegment.ofArray(b));
                long n = (long) Libc.WRITE.invoke(Libc.CAPTURE, fd, Libc.BUF, (long) b.length);
                return n < 0 ? -Libc.errno() : (int) n;
            } catch (Throwable t) { throw new RuntimeException(t); }
        }
        /** Feature report get: returns the report with the ID in byte 0, or null. */
        byte[] getFeature(int id, int payloadLen) {
            int len = payloadLen + 1;
            try {
                Libc.BUF.fill((byte) 0); Libc.BUF.set(JAVA_BYTE, 0, (byte) id);
                int n = (int) Libc.IOCTL.invoke(Libc.CAPTURE, fd, 0xC0000000L | ((long) len << 16) | (0x48L << 8) | 0x07, Libc.BUF);
                if (n < 0) return null;
                return Libc.BUF.asSlice(0, len).toArray(JAVA_BYTE);
            } catch (Throwable t) { throw new RuntimeException(t); }
        }
        boolean setFeature(byte[] b) {
            try {
                Libc.BUF.copyFrom(MemorySegment.ofArray(b));
                return (int) Libc.IOCTL.invoke(Libc.CAPTURE, fd, 0xC0000000L | ((long) b.length << 16) | (0x48L << 8) | 0x06, Libc.BUF) >= 0;
            } catch (Throwable t) { throw new RuntimeException(t); }
        }
        public void close() { try { int r = (int) Libc.CLOSE.invoke(fd); } catch (Throwable ignored) { } }
    
    /** /dev/hidrawN of this board: bus 0003 = USB, 0005 = Bluetooth, matched on the device name. */
    static String find(String tag, boolean usb) throws IOException {
        String best = null; int bestN = -1;
        try (var ds = Files.newDirectoryStream(Path.of("/sys/class/hidraw"), "hidraw*")) {
            for (Path p : ds) {
                Path ue = p.resolve("device/uevent");
                if (!Files.exists(ue)) continue;
                String id = "", name = "";
                for (String l : Files.readAllLines(ue)) {
                    if (l.startsWith("HID_ID=")) id = l.substring(7);
                    if (l.startsWith("HID_NAME=")) name = l.substring(9);
                }
                boolean isUsb = id.startsWith("0003"), isBle = id.startsWith("0005");
                String token = usb ? "NG-" + tag : (tag.equals("S3") ? "-cs3" : "-c2");   // USB product, or the end of the Bluetooth name
                if (name.contains(token) && (usb ? isUsb : isBle)) {
                    int n = Integer.parseInt(p.getFileName().toString().substring(6));
                    if (n > bestN) { bestN = n; best = "/dev/" + p.getFileName(); }
                }
            }
        }
        return best;
    }
}
