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
import static java.lang.foreign.ValueLayout.JAVA_INT;
import static java.lang.foreign.ValueLayout.JAVA_LONG;

/**
 * Foreign Function & Memory (FFM) bindings to standard C library ({@code libc}) primitives.
 * <p>
 * Provides direct access to essential POSIX system calls needed for raw HID and Linux
 * input subsystem access: {@code open}, {@code read}, {@code write}, {@code close},
 * and {@code ioctl}.
 * </p>
 * <p>
 * Implemented using modern Java Foreign Function &amp; Memory APIs (JEP 454), eliminating
 * the need for native JNI glue libraries.
 * </p>
 */
final class Libc {

    // --- POSIX file open flags ---
    public static final int O_RDONLY = 0;
    public static final int O_WRONLY = 1;
    public static final int O_RDWR = 2;
    public static final int O_NONBLOCK = 0x800; // 2048

    // --- Native Linker & Lookup ---
    static final Linker LINKER = Linker.nativeLinker();
    static final SymbolLookup STD = LINKER.defaultLookup();

    // Layout for capturing errno from downcalls
    static final StructLayout CAP = Linker.Option.captureStateLayout();
    static final long ERRNO_AT = CAP.byteOffset(MemoryLayout.PathElement.groupElement("errno"));

    // Global arena for long-lived native allocations
    static final Arena ARENA = Arena.global();

    // Default capture segment and buffer maintained for backwards compatibility
    static final MemorySegment CAPTURE = ARENA.allocate(CAP);
    static final MemorySegment BUF = ARENA.allocate(1024);

    // Downcall MethodHandles
    static final MethodHandle OPEN = LINKER.downcallHandle(
            STD.find("open").orElseThrow(),
            FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_INT),
            Linker.Option.captureCallState("errno")
    );

    static final MethodHandle WRITE = LINKER.downcallHandle(
            STD.find("write").orElseThrow(),
            FunctionDescriptor.of(JAVA_LONG, JAVA_INT, ADDRESS, JAVA_LONG),
            Linker.Option.captureCallState("errno")
    );

    static final MethodHandle READ = LINKER.downcallHandle(
            STD.find("read").orElseThrow(),
            FunctionDescriptor.of(JAVA_LONG, JAVA_INT, ADDRESS, JAVA_LONG)
    );

    static final MethodHandle CLOSE = LINKER.downcallHandle(
            STD.find("close").orElseThrow(),
            FunctionDescriptor.of(JAVA_INT, JAVA_INT)
    );

    static final MethodHandle IOCTL = LINKER.downcallHandle(
            STD.find("ioctl").orElseThrow(),
            FunctionDescriptor.of(JAVA_INT, JAVA_INT, JAVA_LONG, ADDRESS),
            Linker.Option.firstVariadicArg(2),
            Linker.Option.captureCallState("errno")
    );

    private Libc() {
        // Non-instantiable utility class
    }

    /**
     * Retrieves the most recently captured POSIX {@code errno} value.
     *
     * @return the error number from the last intercepted downcall
     */
    static int errno() {
        return CAPTURE.get(JAVA_INT, ERRNO_AT);
    }

    /**
     * Opens a file descriptor at the specified filesystem path.
     *
     * @param path  the null-terminated pathname
     * @param flags the open flags (e.g. {@link #O_RDWR}, {@link #O_NONBLOCK})
     * @return the opened file descriptor, or negative on failure
     * @throws IOException if the native invocation fails
     */
    static int open(String path, int flags) throws IOException {
        try (var arena = Arena.ofConfined()) {
            MemorySegment nativePath = arena.allocateFrom(path);
            return (int) OPEN.invoke(CAPTURE, nativePath, flags);
        } catch (Throwable t) {
            throw new IOException("Failed to open " + path, t);
        }
    }

    /**
     * Writes bytes from a native memory segment to an open file descriptor.
     *
     * @param fd    the open file descriptor
     * @param buf   the native memory segment containing data
     * @param count number of bytes to write
     * @return number of bytes written, or negative on failure
     * @throws IOException if the downcall invocation fails
     */
    static long write(int fd, MemorySegment buf, long count) throws IOException {
        try {
            return (long) WRITE.invoke(CAPTURE, fd, buf, count);
        } catch (Throwable t) {
            throw new IOException("Failed to write to fd=" + fd, t);
        }
    }

    /**
     * Reads bytes from an open file descriptor into a native memory segment.
     *
     * @param fd    the open file descriptor
     * @param buf   the target memory segment
     * @param count maximum number of bytes to read
     * @return number of bytes read, or non-positive on EOF/error
     * @throws IOException if the downcall invocation fails
     */
    static long read(int fd, MemorySegment buf, long count) throws IOException {
        try {
            return (long) READ.invoke(fd, buf, count);
        } catch (Throwable t) {
            throw new IOException("Failed to read from fd=" + fd, t);
        }
    }

    /**
     * Closes an open file descriptor.
     *
     * @param fd the file descriptor to close
     * @return 0 on success, or -1 on error
     */
    static int close(int fd) {
        try {
            return (int) CLOSE.invoke(fd);
        } catch (Throwable ignored) {
            return -1;
        }
    }
}
