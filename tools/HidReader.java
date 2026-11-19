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

    /** Reads the input reports of the board's HID node (the gamepad, report ID 11) without blocking the writer. */
    final class HidReader implements AutoCloseable {
        final int fd; final java.util.concurrent.ConcurrentLinkedQueue<byte[]> q = new java.util.concurrent.ConcurrentLinkedQueue<>();
        volatile boolean run = true; final Thread t; volatile long reports = 0;
        HidReader(String path) throws IOException {
            int f;
            try { f = (int) Libc.OPEN.invoke(Libc.CAPTURE, Libc.ARENA.allocateFrom(path), 2048); } catch (Throwable e) { throw new IOException(e); }   // O_RDONLY | O_NONBLOCK
            if (f < 0) throw new IOException("cannot open " + path + " for reading (errno " + Libc.errno() + ")");
            fd = f;
            Arena a = Arena.ofShared(); MemorySegment buf = a.allocate(256);
            t = Thread.ofVirtual().name("hid-reader").unstarted(() -> {
                try {
                    while (run) {
                        long n = (long) Libc.READ.invoke(fd, buf, 256L);
                        if (n <= 0) { java.util.concurrent.locks.LockSupport.parkNanos(40_000); continue; }
                        if ((buf.get(JAVA_BYTE, 0) & 255) == Proto.GAMEPAD_ID && n >= 49) { q.add(buf.asSlice(1, 48).toArray(JAVA_BYTE)); reports++; }
                    }
                } catch (Throwable e) { }
            });
            t.start();
        }
        public void close() { run = false; try { t.join(200); } catch (InterruptedException ignored) { } try { int r = (int) Libc.CLOSE.invoke(fd); } catch (Throwable ignored) { } }
    }
