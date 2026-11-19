///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 27
//SOURCES Boards.java
//SOURCES Libc.java
//SOURCES Hid.java
//SOURCES HidReader.java
//SOURCES Evdev.java
//SOURCES Ser.java
//SOURCES Proto.java
//SOURCES DiskDrive.java
//SOURCES Canvas.java
//SOURCES BarEffects.java
//SOURCES GridEffects.java
//SOURCES Photos.java
//SOURCES Scenarios.java
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;
import java.util.*;

/**
 * The demo without a window: runs the scenarios of the CoreS3 one after the other (see DemoApp for the window with buttons).
 * <pre>
 * jbang tools/Demo.java                 one round of everything
 * jbang tools/Demo.java --cycles 0      until Ctrl-C
 * jbang tools/Demo.java --only pages,disk,bar,grid,photos
 * options: --seconds 8 (per effect)  --photo-grid 160x95  --no-fast
 * </pre>
 * Run it as ONE plain command (serial port and /dev/hidraw are needed).
 */
public class Demo {
    public static void main(String[] args) throws Exception {
        String only = "pages,disk,bar,grid,photos", photoGrid = "160x95"; int cycles = 1; double seconds = 8; boolean fast = true; String shots = null;
        for (int i = 0; i < args.length; i++) switch (args[i]) {
            case "--cycles" -> cycles = Integer.parseInt(args[++i]);
            case "--only" -> only = args[++i];
            case "--seconds" -> seconds = Double.parseDouble(args[++i]);
            case "--photo-grid" -> photoGrid = args[++i];
            case "--no-fast" -> fast = false;
            case "--shots" -> shots = args[++i];
            default -> { System.err.println("unknown option " + args[i]); System.exit(2); }
        }
        Boards.Board board = Boards.parse("S3");
        Optional<SerialPort> sp = Boards.find(board);
        if (sp.isEmpty()) { System.err.println("no S3 board found on USB"); System.exit(2); }
        if (!Boards.usable(sp.get())) { System.err.println(Boards.SANDBOX_HINT); System.exit(2); }
        String node = Hid.find("S3", true);
        if (node == null) { System.err.println("no USB HID node for the CoreS3"); System.exit(2); }
        Set<String> want = new LinkedHashSet<>(Arrays.asList(only.split(",")));
        try (Ser s = new Ser(sp.get()); Hid h = new Hid(node)) {
            Scenarios sc = new Scenarios(s, h, "S3", System.out::println);
            sc.seconds = seconds; sc.photoGrid = photoGrid; sc.shotDir = shots;
            final String shotsDir = shots;
            if (shots != null) {                                    // the screen of the board after every page and part, as files
                java.nio.file.Files.createDirectories(java.nio.file.Path.of(shots));
                int[] n = {0};
                sc.preview = new Scenarios.Preview() {
                    public void screen(java.awt.image.BufferedImage img) {
                        try { javax.imageio.ImageIO.write(img, "png", java.nio.file.Path.of(shotsDir, String.format("screen-%03d.png", n[0]++)).toFile()); } catch (java.io.IOException e) { }
                    }
                    public void bar(int[] rgb) { }
                    public void grid(int[] rgb, int cols, int rows) { }
                };
            }
            System.out.println("Demo runner on " + s.kv("INFO"));
            List<String[]> all = new ArrayList<>();
            try {
                for (int c = 1; cycles == 0 || c <= cycles; c++) {
                    System.out.println("=== cycle " + c + " ===");
                    if (want.size() == 5) all.addAll(sc.cycle(fast));
                    else for (String w : want) {
                        long t0 = System.nanoTime(); String note = switch (w) {
                            case "pages" -> sc.walkPages(); case "disk" -> sc.disk(false); case "bar" -> sc.bar(); case "grid" -> sc.grid(); case "photos" -> sc.photos();
                            default -> "unknown scenario"; };
                        all.add(new String[]{w, String.format("%.1f s", (System.nanoTime() - t0) / 1e9), note});
                    }
                }
            } finally { sc.release(); }
            System.out.println("\nSUMMARY");
            for (String[] r : all) System.out.printf("  %-14s %8s  %s%n", r[0], r[1], r[2]);
            System.out.printf("checks: pass=%d fail=%d -> %s%n", sc.pass, sc.fail, sc.fail == 0 ? "PASS" : "FAIL");
            System.exit(sc.fail == 0 ? 0 : 1);
        }
    }
}
