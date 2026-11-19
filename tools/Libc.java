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

/** libc through the foreign function API: open, read, write, close and ioctl for the HID and input nodes. */
final class Libc {
    static final Linker LINKER = Linker.nativeLinker();
    static final SymbolLookup STD = LINKER.defaultLookup();
    static final StructLayout CAP = Linker.Option.captureStateLayout();
    static final long ERRNO_AT = CAP.byteOffset(MemoryLayout.PathElement.groupElement("errno"));
    static final Arena ARENA = Arena.global();
    static final MemorySegment CAPTURE = ARENA.allocate(CAP);
    static final MemorySegment BUF = ARENA.allocate(1024);
    static final MethodHandle OPEN = LINKER.downcallHandle(STD.find("open").orElseThrow(), FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_INT), Linker.Option.captureCallState("errno"));
    static final MethodHandle WRITE = LINKER.downcallHandle(STD.find("write").orElseThrow(), FunctionDescriptor.of(JAVA_LONG, JAVA_INT, ADDRESS, JAVA_LONG), Linker.Option.captureCallState("errno"));
    static final MethodHandle READ = LINKER.downcallHandle(STD.find("read").orElseThrow(), FunctionDescriptor.of(JAVA_LONG, JAVA_INT, ADDRESS, JAVA_LONG));
    static final MethodHandle CLOSE = LINKER.downcallHandle(STD.find("close").orElseThrow(), FunctionDescriptor.of(JAVA_INT, JAVA_INT));
    static final MethodHandle IOCTL = LINKER.downcallHandle(STD.find("ioctl").orElseThrow(), FunctionDescriptor.of(JAVA_INT, JAVA_INT, JAVA_LONG, ADDRESS),
            Linker.Option.firstVariadicArg(2), Linker.Option.captureCallState("errno"));

    static int errno() { return CAPTURE.get(JAVA_INT, ERRNO_AT); }

}
