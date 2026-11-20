///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//SOURCES Boards.java
//JAVA 27
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Stream;

/**
 * Automated firmware flashing utility for Nachtgespenst boards (Core2 and CoreS3).
 * <p>
 * Invokes {@code esptool} to flash built binary firmware images directly to target hardware:
 * <ul>
 *     <li><b>Core2:</b> CP2104 USB-to-UART bridge automatically resets the ESP32 into ROM bootloader via DTR/RTS.</li>
 *     <li><b>CoreS3:</b> Native USB OTG peripheral requires a 1200-baud touch reset sequence to initiate ROM download mode.</li>
 * </ul>
 * </p>
 * <pre>
 *   jbang tools/Flash.java core2|cores3 [--image FILE] [--dry-run]
 * </pre>
 */
public class Flash {

    private static void fail(String message, int exitCode) {
        System.err.println("error: " + message);
        System.exit(exitCode);
    }

    public static void main(String[] args) throws Exception {
        if (args.length == 0 || args[0].startsWith("-")) {
            fail("usage: jbang tools/Flash.java core2|cores3 [--image FILE] [--dry-run]", 2);
        }

        Boards.Board board = null;
        try {
            board = Boards.parse(args[0]);
        } catch (IllegalArgumentException e) {
            fail(e.getMessage(), 2);
        }

        String customImage = null;
        boolean dryRun = false;

        for (int i = 1; i < args.length; i++) {
            switch (args[i]) {
                case "--image" -> {
                    if (++i >= args.length) {
                        fail("--image requires a file path argument", 2);
                    }
                    customImage = args[i];
                }
                case "--dry-run" -> dryRun = true;
                default -> fail("unknown option " + args[i], 2);
            }
        }

        Path repoRoot = Boards.repoRoot();
        Path imagePath = (customImage != null)
                ? Path.of(customImage).toAbsolutePath()
                : repoRoot.resolve(".pio/build/" + board.env() + "/firmware.bin");

        if (!Files.isRegularFile(imagePath)) {
            fail("No image at " + imagePath + " (build it first: .pio/venv/bin/pio run -e " + board.env() + ")", 1);
        }

        System.out.println("branch : " + queryGit(repoRoot, "branch", "--show-current"));
        System.out.printf("image  : %s (%d bytes, %s)%n",
                imagePath, Files.size(imagePath), Files.getLastModifiedTime(imagePath).toInstant());

        Optional<Path> staleFile = findSourceNewerThan(repoRoot, imagePath);
        staleFile.ifPresent(p -> System.out.println(
                "WARNING: " + repoRoot.relativize(p) + " is newer than the image: rebuild before flashing unless intended"
        ));

        Path bundledEsptool = repoRoot.resolve(".pio/core/packages/tool-esptool/esptool");
        String esptoolBinary = Files.isExecutable(bundledEsptool) ? bundledEsptool.toString() : "esptool";

        Optional<SerialPort> detectedPort = Boards.find(board);
        if (detectedPort.isEmpty()) {
            fail("No " + board.env() + " board on USB (looked for " + board.usbId() + "): plugged in? data cable?", 1);
        }

        SerialPort port = detectedPort.get();
        System.out.println("port   : " + Boards.path(port));

        if (dryRun) {
            System.out.printf("dry run: would flash %s to %s with %s --chip %s --baud %d write-flash 0x10000%n",
                    imagePath, Boards.path(port), esptoolBinary, board.chip(), board.baud());
            return;
        }

        if (!Boards.usable(port)) {
            fail(Boards.SANDBOX_HINT, 3);
        }

        // CoreS3 native USB 1200-baud touch resets chip into Espressif ROM bootloader
        if (board == Boards.S3) {
            System.out.println("step   : 1200-baud touch (triggering ROM bootloader)");
            port.setBaudRate(1200);
            if (!port.openPort()) {
                fail(Boards.SANDBOX_HINT, 3);
            }
            Thread.sleep(300);
            port.closePort(); // Dropping DTR at 1200 baud initiates reset into ROM bootloader
            Thread.sleep(3000);

            port = Boards.waitBoot(board, 15).orElse(null);
            if (port == null) {
                port = Boards.waitFor(board, 10).orElse(null);
            }
            if (port == null) {
                fail("CoreS3 did not reappear after 1200-baud touch (hold side button ~3 s for download mode)", 1);
            }
        }

        System.out.println("step   : writing flash");
        int exitCode = new ProcessBuilder(
                esptoolBinary,
                "--chip", board.chip(),
                "--port", Boards.path(port),
                "--baud", String.valueOf(board.baud()),
                "write-flash",
                "--no-progress",
                "0x10000",
                imagePath.toString()
        ).inheritIO().start().waitFor();

        if (exitCode != 0) {
            fail("esptool failed with exit code " + exitCode, 1);
        }

        System.out.println("step   : waiting for board to boot application");
        Thread.sleep(3000);

        port = Boards.waitApp(board, 45).orElse(null);
        if (port == null) {
            port = Boards.waitFor(board, 5).orElse(null);
            if (port == null) {
                fail("Board did not reappear after flashing", 1);
            }
            System.out.printf("WARNING: board enumerated as %04x:%04x (old firmware or still in bootloader)%n",
                    port.getVendorID(), port.getProductID());
        }

        Thread.sleep(4000); // Allow USB CDC enumeration to stabilize
        System.out.println("done   : " + board.env() + " is up on " + Boards.path(port));
    }

    /**
     * Checks if any source file in {@code src/}, {@code include/}, or {@code platformio.ini}
     * was modified more recently than the firmware binary image.
     */
    static Optional<Path> findSourceNewerThan(Path root, Path image) throws IOException {
        Instant buildTime = Files.getLastModifiedTime(image).toInstant();
        List<Path> watchRoots = List.of(
                root.resolve("src"),
                root.resolve("include"),
                root.resolve("platformio.ini")
        );

        for (Path watchRoot : watchRoots) {
            if (!Files.exists(watchRoot)) continue;
            try (Stream<Path> stream = Files.walk(watchRoot)) {
                Optional<Path> modified = stream
                        .filter(Files::isRegularFile)
                        .filter(p -> !p.toString().contains("/hid_")) // Exclude auto-generated headers
                        .filter(p -> {
                            try {
                                return Files.getLastModifiedTime(p).toInstant().isAfter(buildTime);
                            } catch (IOException e) {
                                return false;
                            }
                        })
                        .findFirst();

                if (modified.isPresent()) {
                    return modified;
                }
            }
        }
        return Optional.empty();
    }

    /**
     * Executes a git command in the repository root and captures the trimmed output.
     */
    static String queryGit(Path repoRoot, String... gitArgs) {
        try {
            List<String> command = new ArrayList<>();
            command.add("git");
            command.add("-C");
            command.add(repoRoot.toString());
            command.addAll(List.of(gitArgs));

            Process process = new ProcessBuilder(command)
                    .redirectErrorStream(true)
                    .start();
            String output = new String(process.getInputStream().readAllBytes(), StandardCharsets.UTF_8).trim();
            return (process.waitFor() == 0 && !output.isEmpty()) ? output : "?";
        } catch (Exception e) {
            return "?";
        }
    }
}
