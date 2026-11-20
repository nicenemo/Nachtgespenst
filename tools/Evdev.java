import java.io.IOException;
import java.lang.foreign.Arena;
import java.lang.foreign.MemorySegment;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import static java.lang.foreign.ValueLayout.JAVA_INT_UNALIGNED;
import static java.lang.foreign.ValueLayout.JAVA_SHORT_UNALIGNED;

/**
 * Interface to a Linux event device ({@code /dev/input/eventN}).
 * <p>
 * Reads kernel input events asynchronously from the host's evdev subsystem using
 * a dedicated virtual thread. Supports optional exclusive grabbing ({@code EVIOCGRAB})
 * so keystrokes sent by the device (such as F13..F24 acknowledgements) are consumed
 * exclusively by the test suite rather than propagating to the window manager or desktop.
 * </p>
 * <p>
 * On 64-bit Linux systems, {@code struct input_event} is 24 bytes:
 * <ul>
 *     <li>Bytes 0..15: {@code struct timeval} (8-byte tv_sec, 8-byte tv_usec)</li>
 *     <li>Bytes 16..17: {@code __u16 type} (e.g. {@link #EV_KEY})</li>
 *     <li>Bytes 18..19: {@code __u16 code} (Linux key code)</li>
 *     <li>Bytes 20..23: {@code __s32 value} (0 = release, 1 = press, 2 = autorepeat)</li>
 * </ul>
 * </p>
 */
final class Evdev implements AutoCloseable {

    /** Event type for key press / release events. */
    public static final int EV_KEY = 1;

    /** Value indicating a key press down event. */
    public static final int KEY_PRESS = 1;

    /** ioctl request code for {@code EVIOCGRAB}: grabs/releases exclusive access to the device. */
    public static final long EVIOCGRAB = 0x40044590L;

    /** Size in bytes of {@code struct input_event} on 64-bit Linux architectures. */
    private static final int EVENT_SIZE = 24;
    private static final int EVENT_BATCH = 32;

    final int fd;
    final boolean grabbed;
    final BlockingQueue<Integer> keys = new LinkedBlockingQueue<>();
    private final Arena arena = Arena.ofShared();
    private final Thread readerThread;
    private volatile boolean running = true;

    /**
     * Opens an evdev event node in non-blocking mode and launches an event listener.
     *
     * @param path device node path (e.g. {@code /dev/input/event4})
     * @param grab if {@code true}, exclusively grabs the device using {@link #EVIOCGRAB}
     * @throws IOException if opening the device node fails
     */
    Evdev(String path, boolean grab) throws IOException {
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
            throw new IOException("Cannot open " + path + " (errno " + Libc.errno() + ")");
        }
        this.fd = openedFd;

        boolean grabSuccess = false;
        if (grab) {
            try {
                int res = (int) Libc.IOCTL.invoke(
                        Libc.CAPTURE,
                        fd,
                        EVIOCGRAB,
                        MemorySegment.ofAddress(1)
                );
                grabSuccess = (res == 0);
            } catch (Throwable ignored) {
                grabSuccess = false;
            }
        }
        this.grabbed = grabSuccess;

        MemorySegment buf = arena.allocate((long) EVENT_SIZE * EVENT_BATCH);
        this.readerThread = Thread.ofVirtual().name("evdev-reader-" + openedFd).start(() -> {
            try {
                while (running) {
                    long bytesRead = (long) Libc.READ.invoke(fd, buf, (long) EVENT_SIZE * EVENT_BATCH);
                    if (bytesRead <= 0) {
                        Thread.sleep(1);
                        continue;
                    }

                    for (int offset = 0; offset + EVENT_SIZE <= bytesRead; offset += EVENT_SIZE) {
                        int type = buf.get(JAVA_SHORT_UNALIGNED, offset + 16) & 0xFFFF;
                        int code = buf.get(JAVA_SHORT_UNALIGNED, offset + 18) & 0xFFFF;
                        int val = buf.get(JAVA_INT_UNALIGNED, offset + 20);

                        if (type == EV_KEY && val == KEY_PRESS) {
                            keys.add(code);
                        }
                    }
                }
            } catch (InterruptedException ignored) {
                Thread.currentThread().interrupt();
            } catch (Throwable ignored) {
                // Terminate gracefully on device close
            }
        });
    }

    /**
     * Terminates the listener thread, releases any exclusive grab, and closes the device node.
     */
    @Override
    public void close() {
        running = false;
        try {
            readerThread.interrupt();
            readerThread.join(200);
        } catch (InterruptedException ignored) {
            Thread.currentThread().interrupt();
        }
        Libc.close(fd);
        arena.close();
    }

    /**
     * Locates the {@code /dev/input/eventN} device node for the Nachtgespenst keyboard endpoint.
     * <p>
     * Parses {@code /proc/bus/input/devices} to find the matching device name and physical
     * connection bus (USB vs Bluetooth).
     * </p>
     *
     * @param tag board identifier tag (e.g. {@code "S3"} or {@code "C2"})
     * @param usb {@code true} for USB connection, {@code false} for Bluetooth
     * @return path to the matching input event node (e.g. {@code "/dev/input/event7"}), or {@code null} if not found
     */
    static String findKeyboard(String tag, boolean usb) {
        String token = usb
                ? "NG-" + tag + " Keyboard"
                : (tag.equals("S3") ? "-cs3 Keyboard" : "-c2 Keyboard");
        Path devicesFile = Path.of("/proc/bus/input/devices");
        if (!Files.exists(devicesFile)) {
            return null;
        }

        try {
            String currentName = null;
            boolean matchesConnection = false;

            for (String line : Files.readAllLines(devicesFile)) {
                if (line.startsWith("N: Name=")) {
                    currentName = line;
                    matchesConnection = false;
                } else if (line.startsWith("P: Phys=") && currentName != null && currentName.contains(token)) {
                    matchesConnection = (usb == line.substring(8).startsWith("usb-"));
                } else if (line.startsWith("H: Handlers=") && matchesConnection) {
                    for (String word : line.split("\\s+")) {
                        if (word.startsWith("event")) {
                            return "/dev/input/" + word;
                        }
                    }
                } else if (line.isBlank()) {
                    currentName = null;
                    matchesConnection = false;
                }
            }
        } catch (IOException ignored) {
            // Return null if reading /proc/bus/input/devices fails
        }
        return null;
    }
}
