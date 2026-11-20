import java.io.IOException;
import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.locks.LockSupport;

import static java.lang.foreign.ValueLayout.JAVA_BYTE;

/**
 * Asynchronous background reader for board HID input reports.
 * <p>
 * Reads input reports from the board's raw HID interface in a non-blocking loop
 * executed on a virtual thread. Filters for gamepad input reports
 * (Report ID {@value Proto#GAMEPAD_ID}) carrying telemetry and disk load packets,
 * and queues 48-byte payload slices for consumption without stalling the main thread.
 * </p>
 */
final class HidReader implements AutoCloseable {

    private static final int BUFFER_SIZE = 256;
    private static final int REQUIRED_REPORT_SIZE = 49;
    private static final int PAYLOAD_SIZE = 48;

    final int fd;
    final ConcurrentLinkedQueue<byte[]> q = new ConcurrentLinkedQueue<>();
    private final Arena arena = Arena.ofShared();
    private final Thread readerThread;
    private volatile boolean running = true;
    volatile long reports = 0;

    /**
     * Opens the HID device node in non-blocking mode and launches the reader thread.
     *
     * @param path device node path (e.g. {@code /dev/hidraw1})
     * @throws IOException if opening the device fails
     */
    HidReader(String path) throws IOException {
        int openedFd;
        try {
            openedFd = (int) Libc.OPEN.invoke(
                    Libc.CAPTURE,
                    Libc.ARENA.allocateFrom(path),
                    Libc.O_RDONLY | Libc.O_NONBLOCK
            );
        } catch (Throwable e) {
            throw new IOException("Failed to invoke open on " + path, e);
        }

        if (openedFd < 0) {
            throw new IOException("Cannot open " + path + " for reading (errno " + Libc.errno() + ")");
        }
        this.fd = openedFd;

        MemorySegment buf = arena.allocate(BUFFER_SIZE);
        this.readerThread = Thread.ofVirtual().name("hid-reader-" + openedFd).start(() -> {
            try {
                while (running) {
                    long bytesRead = (long) Libc.READ.invoke(fd, buf, (long) BUFFER_SIZE);
                    if (bytesRead <= 0) {
                        LockSupport.parkNanos(40_000); // 40 microseconds backoff
                        continue;
                    }

                    int reportId = buf.get(JAVA_BYTE, 0) & 0xFF;
                    if (reportId == Proto.GAMEPAD_ID && bytesRead >= REQUIRED_REPORT_SIZE) {
                        q.add(buf.asSlice(1, PAYLOAD_SIZE).toArray(JAVA_BYTE));
                        reports++;
                    }
                }
            } catch (Throwable ignored) {
                // Thread terminates on device closure or error
            }
        });
    }

    /**
     * Stops the reader thread, waits for termination, and closes the native file descriptor.
     */
    @Override
    public void close() {
        running = false;
        try {
            readerThread.join(200);
        } catch (InterruptedException ignored) {
            Thread.currentThread().interrupt();
        }
        Libc.close(fd);
        arena.close();
    }
}
