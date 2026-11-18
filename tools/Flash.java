///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//SOURCES Boards.java
//JAVA 17+
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.List;
import java.util.Optional;
import java.util.stream.Stream;

/**
 * Flash an already built Nachtgespenst firmware image to a board and wait until it is back.
 *
 * <pre>
 * jbang tools/Flash.java core2|cores3 [--image FILE] [--dry-run]
 * </pre>
 *
 * Build first (about 12 minutes per environment): {@code .pio/venv/bin/pio run -e cores3}.
 * <ul>
 * <li>core2: the CP2104 bridge resets the chip into the bootloader by itself.</li>
 * <li>cores3: native USB (TinyUSB) cannot do that, so a 1200-baud touch puts the ROM bootloader on the port first.</li>
 * </ul>
 * Only the application is written (0x10000); bootloader, partition table and Bluetooth bonds stay as they are.
 *
 * Run it as ONE plain command (no pipe, cd, &amp;&amp;, ;, VAR=x prefix) so that it matches "*jbang*Flash.java*" in
 * sandbox.excludedCommands and runs outside the Claude Code sandbox. Exit codes: 0 done, 1 failed, 2 usage,
 * 3 port cannot be opened (sandbox or permissions).
 */
public class Flash {
    static void die(String msg, int code) { System.err.println("error: " + msg); System.exit(code); }

    public static void main(String[] args) throws Exception {
        if (args.length == 0 || args[0].startsWith("-")) die("usage: jbang tools/Flash.java core2|cores3 [--image FILE] [--dry-run]", 2);
        Boards.Board b = null;
        try { b = Boards.parse(args[0]); } catch (IllegalArgumentException e) { die(e.getMessage(), 2); }
        String image = null; boolean dry = false;
        for (int i = 1; i < args.length; i++) switch (args[i]) {
            case "--image" -> { if (++i >= args.length) die("--image needs a file", 2); image = args[i]; }
            case "--dry-run" -> dry = true;
            default -> die("unknown option " + args[i], 2);
        }

        Path root = Boards.repoRoot();
        Path img = image != null ? Path.of(image).toAbsolutePath() : root.resolve(".pio/build/" + b.env() + "/firmware.bin");
        if (!Files.isRegularFile(img)) die("no image at " + img + " (build it first: .pio/venv/bin/pio run -e " + b.env() + ")", 1);

        // A stale image is how the wrong UI got flashed once: say clearly what is about to happen.
        System.out.println("branch : " + git(root, "branch", "--show-current"));
        System.out.printf("image  : %s (%d bytes, %s)%n", img, Files.size(img), Files.getLastModifiedTime(img).toInstant());
        Optional<Path> stale = newerThan(root, img);
        stale.ifPresent(p -> System.out.println("WARNING: " + root.relativize(p) + " is newer than the image: rebuild before flashing unless that is intended"));

        Path bundled = root.resolve(".pio/core/packages/tool-esptool/esptool");
        String esptool = Files.isExecutable(bundled) ? bundled.toString() : "esptool";

        Optional<SerialPort> found = Boards.find(b);
        if (found.isEmpty()) die("no " + b.env() + " board on USB (looked for " + b.usbId() + "): plugged in? cable carries data? drivers loaded (cp210x, cdc_acm)?", 1);
        SerialPort port = found.get();
        System.out.println("port   : " + Boards.path(port));
        if (dry) {
            System.out.printf("dry run: would flash %s to %s with %s --chip %s --baud %d write-flash 0x10000%n",
                    img, Boards.path(port), esptool, b.chip(), b.baud());
            return;
        }
        if (!Boards.usable(port)) die(Boards.SANDBOX_HINT, 3);

        if (b == Boards.S3) {
            System.out.println("step   : 1200-baud touch (ROM bootloader)");
            port.setBaudRate(1200);
            if (!port.openPort()) die(Boards.SANDBOX_HINT, 3);
            Thread.sleep(300);
            port.closePort();                       // dropping DTR at 1200 baud makes the firmware reboot into the bootloader
            Thread.sleep(3000);
            port = Boards.waitBoot(b, 15).orElse(null);
            if (port == null) die("the CoreS3 did not come back after the 1200-baud touch (hold the side button ~3 s for download mode)", 1);
        }

        System.out.println("step   : writing");
        int rc = new ProcessBuilder(esptool, "--chip", b.chip(), "--port", Boards.path(port), "--baud", String.valueOf(b.baud()),
                "write-flash", "--no-progress", "0x10000", img.toString()).inheritIO().start().waitFor();
        if (rc != 0) die("esptool failed with exit code " + rc, 1);

        System.out.println("step   : waiting for the board to boot");
        Thread.sleep(3000);
        port = Boards.waitApp(b, 45).orElse(null);          // the application ID: not the ROM bootloader's (USB enumeration can take 30 s)
        if (port == null) {
            port = Boards.waitFor(b, 5).orElse(null);       // any ID: an image with the old USB ID 303a:1001 shows up like that
            if (port == null) die("the board did not reappear after flashing", 1);
            System.out.printf("WARNING: the board is up on %04x:%04x, not %s (old firmware, or still in the bootloader)%n", port.getVendorID(), port.getProductID(), b.usbId());
        }
        Thread.sleep(4000);                         // firmware start + CDC enumeration; a test started earlier loses its first reply
        System.out.println("done   : " + b.env() + " is up on " + Boards.path(port));
    }

    /** First source file (outside the generated hid_* headers) that is newer than the image, if any. */
    static Optional<Path> newerThan(Path root, Path img) throws IOException {
        Instant built = Files.getLastModifiedTime(img).toInstant();
        List<Path> roots = List.of(root.resolve("src"), root.resolve("include"), root.resolve("platformio.ini"));
        for (Path r : roots) {
            if (!Files.exists(r)) continue;
            try (Stream<Path> s = Files.walk(r)) {
                Optional<Path> hit = s.filter(Files::isRegularFile).filter(p -> !p.toString().contains("/hid_"))
                        .filter(p -> { try { return Files.getLastModifiedTime(p).toInstant().isAfter(built); } catch (IOException e) { return false; } })
                        .findFirst();
                if (hit.isPresent()) return hit;
            }
        }
        return Optional.empty();
    }

    static String git(Path root, String... args) {
        try {
            String[] cmd = new String[args.length + 3];
            cmd[0] = "git"; cmd[1] = "-C"; cmd[2] = root.toString();
            System.arraycopy(args, 0, cmd, 3, args.length);
            Process p = new ProcessBuilder(cmd).redirectErrorStream(true).start();
            String out = new String(p.getInputStream().readAllBytes()).trim();
            return p.waitFor() == 0 && !out.isEmpty() ? out : "?";
        } catch (Exception e) { return "?"; }
    }
}
