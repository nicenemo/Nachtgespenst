//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 27

import com.fazecast.jSerialComm.SerialPort;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Optional;

/**
 * Shared board definitions and USB serial discovery for Nachtgespenst tools.
 * <p>
 * Boards are identified by USB Vendor and Product IDs (VID:PID), ensuring reliable
 * device location regardless of whether the operating system assigns {@code /dev/ttyACM0},
 * {@code /dev/ttyACM1}, or {@code /dev/ttyUSB0}.
 * </p>
 * <p>
 * In restricted sandbox environments, serial ports may be enumerated by the kernel
 * but fail to open; {@link #usable(SerialPort)} detects this condition cleanly.
 * </p>
 */
final class Boards {

    private Boards() {
        // Non-instantiable utility class
    }

    /**
     * Hardware board profile.
     *
     * @param tag      short tag (e.g. {@code "C2"} or {@code "S3"})
     * @param env      PlatformIO build environment name (e.g. {@code "core2"} or {@code "cores3"})
     * @param vid      USB Vendor ID of the running application
     * @param pid      USB Product ID of the running application
     * @param bootVid  USB Vendor ID of the ROM bootloader
     * @param bootPid  USB Product ID of the ROM bootloader
     * @param chip     Espressif chip target name for esptool (e.g. {@code "esp32"}, {@code "esp32s3"})
     * @param baud     recommended flashing baud rate in bps
     */
    record Board(
            String tag,
            String env,
            int vid,
            int pid,
            int bootVid,
            int bootPid,
            String chip,
            int baud
    ) {
        /** Returns the USB VID:PID formatted as a lowercase hex pair, e.g. {@code "1209:0001"}. */
        String usbId() {
            return String.format("%04x:%04x", vid, pid);
        }
    }

    /** M5Stack Core2 (ESP32-D0WDQ6-V3 with CP2104 USB-UART bridge at 10c4:ea60). */
    static final Board C2 = new Board(
            "C2", "core2",
            0x10c4, 0xea60,
            0x10c4, 0xea60,
            "esp32", 230400
    );

    /**
     * M5Stack CoreS3 (ESP32-S3 with native USB OTG).
     * Application uses pid.codes ID 1209:0001 (ADR 0015); ROM bootloader uses Espressif 303a:1001.
     */
    static final Board S3 = new Board(
            "S3", "cores3",
            0x1209, 0x0001,
            0x303a, 0x1001,
            "esp32s3", 921600
    );

    /**
     * Resolves a board identifier string into a known {@link Board} instance.
     *
     * @param name case-insensitive name ({@code "c2"}, {@code "core2"}, {@code "s3"}, {@code "cores3"})
     * @return the matching {@link Board} instance
     * @throws IllegalArgumentException if the board identifier is unrecognised
     */
    static Board parse(String name) {
        return switch (name.toLowerCase()) {
            case "c2", "core2" -> C2;
            case "s3", "cores3" -> S3;
            default -> throw new IllegalArgumentException(
                    "Unknown board '" + name + "' (use core2|cores3 or C2|S3)"
            );
        };
    }

    /**
     * Searches for a board in either normal application mode or bootloader mode.
     *
     * @param board the target board definition
     * @return optional containing the matching serial port if found
     */
    static Optional<SerialPort> find(Board board) {
        return Arrays.stream(SerialPort.getCommPorts())
                .filter(p -> (p.getVendorID() == board.vid() && p.getProductID() == board.pid())
                        || (p.getVendorID() == board.bootVid() && p.getProductID() == board.bootPid()))
                .findFirst();
    }

    /**
     * Searches for a board strictly in application mode (matching application VID:PID only).
     *
     * @param board the target board definition
     * @return optional containing the serial port if found in application mode
     */
    static Optional<SerialPort> findApp(Board board) {
        return Arrays.stream(SerialPort.getCommPorts())
                .filter(p -> p.getVendorID() == board.vid() && p.getProductID() == board.pid())
                .findFirst();
    }

    /**
     * Searches for a board strictly in ROM bootloader mode (matching bootloader VID:PID only).
     *
     * @param board the target board definition
     * @return optional containing the serial port if found in bootloader mode
     */
    static Optional<SerialPort> findBoot(Board board) {
        return Arrays.stream(SerialPort.getCommPorts())
                .filter(p -> p.getVendorID() == board.bootVid() && p.getProductID() == board.bootPid())
                .findFirst();
    }

    /**
     * Polls until the board appears as a serial port in either application or bootloader mode.
     *
     * @param board          the target board
     * @param timeoutSeconds maximum polling duration in seconds
     * @return optional containing the port, or empty on timeout
     * @throws InterruptedException if interrupted while polling
     */
    static Optional<SerialPort> waitFor(Board board, int timeoutSeconds) throws InterruptedException {
        int attempts = timeoutSeconds * 2;
        for (int i = 0; i < attempts; i++) {
            Optional<SerialPort> port = find(board);
            if (port.isPresent()) {
                return port;
            }
            Thread.sleep(500);
        }
        return Optional.empty();
    }

    /**
     * Polls until the board appears in application mode.
     *
     * @param board          the target board
     * @param timeoutSeconds maximum duration in seconds
     * @return optional containing the port, or empty on timeout
     * @throws InterruptedException if interrupted while polling
     */
    static Optional<SerialPort> waitApp(Board board, int timeoutSeconds) throws InterruptedException {
        int attempts = timeoutSeconds * 2;
        for (int i = 0; i < attempts; i++) {
            Optional<SerialPort> port = findApp(board);
            if (port.isPresent()) {
                return port;
            }
            Thread.sleep(500);
        }
        return Optional.empty();
    }

    /**
     * Polls until the board appears in bootloader mode.
     *
     * @param board          the target board
     * @param timeoutSeconds maximum duration in seconds
     * @return optional containing the port, or empty on timeout
     * @throws InterruptedException if interrupted while polling
     */
    static Optional<SerialPort> waitBoot(Board board, int timeoutSeconds) throws InterruptedException {
        int attempts = timeoutSeconds * 2;
        for (int i = 0; i < attempts; i++) {
            Optional<SerialPort> port = findBoot(board);
            if (port.isPresent()) {
                return port;
            }
            Thread.sleep(500);
        }
        return Optional.empty();
    }

    /**
     * Tests whether the serial port can genuinely be opened for communication.
     *
     * @param port the candidate serial port
     * @return {@code true} if opened successfully, {@code false} if blocked by permissions or sandbox
     */
    static boolean usable(SerialPort port) {
        boolean ok = port.openPort();
        if (ok) {
            port.closePort();
        }
        return ok;
    }

    /**
     * Returns the operating system device path for a serial port (e.g. {@code "/dev/ttyACM0"}).
     *
     * @param port the serial port
     * @return the device filesystem path
     */
    static String path(SerialPort port) {
        return port.getSystemPortPath();
    }

    /**
     * Locates the root directory of the repository by walking upwards to locate {@code platformio.ini}.
     *
     * @return the absolute path to the repository root
     * @throws IllegalStateException if {@code platformio.ini} cannot be located in any ancestor directory
     */
    static Path repoRoot() {
        for (Path d = Path.of("").toAbsolutePath(); d != null; d = d.getParent()) {
            if (Files.exists(d.resolve("platformio.ini"))) {
                return d;
            }
        }
        throw new IllegalStateException(
                "No platformio.ini above " + Path.of("").toAbsolutePath() + ": run from inside the repository"
        );
    }

    /** Helpful diagnostic message printed when serial port access is blocked by sandbox or permissions. */
    static final String SANDBOX_HINT = """
            cannot open the port: this call is running inside the Claude Code sandbox (or your user is not in the uucp group).
                   Run the tool as ONE plain command that matches an entry in sandbox.excludedCommands
            """;
}
