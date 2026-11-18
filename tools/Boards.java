//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 17+

import com.fazecast.jSerialComm.SerialPort;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Optional;

/**
 * Shared board lookup for Flash.java and Serial.java (pulled in with //SOURCES, not run on its own).
 *
 * Boards are found by USB vendor:product ID, so it does not matter whether the CoreS3 is /dev/ttyACM0 or
 * /dev/ttyACM1 today. Inside the Claude Code sandbox the port is listed but cannot be opened; that is how
 * {@link #usable} tells a sandboxed call from a real fault.
 */
final class Boards {
    private Boards() {}

    /** A supported board: PlatformIO environment, USB ID, esptool chip name and flash baud rate. */
    record Board(String tag, String env, int vid, int pid, String chip, int baud) {
        String usbId() { return String.format("%04x:%04x", vid, pid); }
    }

    static final Board C2 = new Board("C2", "core2", 0x10c4, 0xea60, "esp32", 460800);     // CP2104 UART bridge
    static final Board S3 = new Board("S3", "cores3", 0x303a, 0x1001, "esp32s3", 921600);  // native USB (app and ROM bootloader)

    /** Accepts C2, S3, core2 and cores3 in any case. */
    static Board parse(String name) {
        switch (name.toLowerCase()) {
            case "c2": case "core2": return C2;
            case "s3": case "cores3": return S3;
            default: throw new IllegalArgumentException("unknown board '" + name + "' (use core2|cores3 or C2|S3)");
        }
    }

    static Optional<SerialPort> find(Board b) {
        for (SerialPort p : SerialPort.getCommPorts())
            if (p.getVendorID() == b.vid() && p.getProductID() == b.pid()) return Optional.of(p);
        return Optional.empty();
    }

    /** Waits until the board shows up as a serial port again (after a flash or a reset). */
    static Optional<SerialPort> waitFor(Board b, int seconds) throws InterruptedException {
        for (int i = 0; i < seconds * 2; i++) {
            Optional<SerialPort> p = find(b);
            if (p.isPresent()) return p;
            Thread.sleep(500);
        }
        return Optional.empty();
    }

    /** True when the port can really be opened: false inside the sandbox, or without the uucp/dialout group. */
    static boolean usable(SerialPort p) {
        boolean ok = p.openPort();
        if (ok) p.closePort();
        return ok;
    }

    static String path(SerialPort p) { return p.getSystemPortPath(); }

    /** The repository root: the nearest parent of the working directory that has a platformio.ini. */
    static Path repoRoot() {
        for (Path d = Path.of("").toAbsolutePath(); d != null; d = d.getParent())
            if (Files.exists(d.resolve("platformio.ini"))) return d;
        throw new IllegalStateException("no platformio.ini above " + Path.of("").toAbsolutePath() + ": run from inside the repository");
    }

    static final String SANDBOX_HINT =
            "cannot open the port: this call is running inside the Claude Code sandbox (or your user is not in the uucp group).\n"
          + "       Run the tool as ONE plain command that matches an entry in sandbox.excludedCommands\n"
          + "       (see docs/wiki/claude-code-workflow.md), or disable the sandbox for that single call.";
}
