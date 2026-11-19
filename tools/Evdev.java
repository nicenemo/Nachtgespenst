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

    /** A host input device (evdev). With grab the kernel delivers its events to this reader only, so the F keys do not reach the desktop. */
    final class Evdev implements AutoCloseable {
        final int fd; final boolean grabbed; final BlockingQueue<Integer> keys = new LinkedBlockingQueue<>();
        volatile boolean run = true; final Thread t;
        Evdev(String path, boolean grab) throws IOException {
            int f;
            try { f = (int) Libc.OPEN.invoke(Libc.CAPTURE, Libc.ARENA.allocateFrom(path), 2048); } catch (Throwable e) { throw new IOException(e); }   // O_RDONLY | O_NONBLOCK
            if (f < 0) throw new IOException("cannot open " + path + " (errno " + Libc.errno() + ")");
            fd = f;
            boolean g = false;
            if (grab) { try { g = (int) Libc.IOCTL.invoke(Libc.CAPTURE, fd, 0x40044590L, MemorySegment.ofAddress(1)) == 0; } catch (Throwable e) { g = false; } }   // EVIOCGRAB
            grabbed = g;
            Arena a = Arena.ofShared(); MemorySegment buf = a.allocate(24 * 32);
            t = Thread.ofVirtual().name("evdev").unstarted(() -> {
                try {
                    while (run) {
                        long n = (long) Libc.READ.invoke(fd, buf, 24L * 32);
                        if (n <= 0) { Thread.sleep(1); continue; }
                        for (int o = 0; o + 24 <= n; o += 24) {
                            int type = buf.get(JAVA_SHORT_UNALIGNED, o + 16) & 0xffff, code = buf.get(JAVA_SHORT_UNALIGNED, o + 18) & 0xffff, val = buf.get(JAVA_INT_UNALIGNED, o + 20);
                            if (type == 1 && val == 1) keys.add(code);
                        }
                    }
                } catch (Throwable e) { }
            });
            t.start();
        }
        public void close() { run = false; try { t.join(200); } catch (InterruptedException ignored) { } try { int r = (int) Libc.CLOSE.invoke(fd); } catch (Throwable ignored) { } }
    
    /** The keyboard evdev node of the board on the chosen link (same lookup as SerialTest). */
    static String findKeyboard(String tag, boolean usb) {
        String token = usb ? "NG-" + tag + " Keyboard" : (tag.equals("S3") ? "-cs3 Keyboard" : "-c2 Keyboard");
        try {
            String cur = null; boolean match = false;
            for (String l : Files.readAllLines(Path.of("/proc/bus/input/devices"))) {
                if (l.startsWith("N: Name=")) { cur = l; match = false; }
                if (l.startsWith("P: Phys=") && cur != null && cur.contains(token)) match = usb == l.substring(8).startsWith("usb-");
                if (l.startsWith("H: Handlers=") && match) for (String w : l.split("\\s+")) if (w.startsWith("event")) return "/dev/input/" + w;
                if (l.isBlank()) { cur = null; match = false; }
            }
        } catch (IOException e) { }
        return null;
    }
}
