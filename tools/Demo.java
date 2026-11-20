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
//SOURCES Tones.java
//SOURCES Telemetry.java
//SOURCES Scope.java
//SOURCES Scenarios.java
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;

/**
 * Headless CLI demonstration runner executing Nachtgespenst CoreS3 scenarios in sequence.
 * <pre>
 *   jbang tools/Demo.java                 # Run full cycle once
 *   jbang tools/Demo.java --cycles 0      # Repeat indefinitely until interrupted
 *   jbang tools/Demo.java --only pages,disk,bar,grid,photos
 * </pre>
 */
public class Demo {

    public static void main(String[] args) throws Exception {
        String onlyFilter = "pages,disk,bar,grid,photos";
        String photoGridResolution = "160x95";
        int cycleCount = 1;
        double effectDurationSeconds = 8.0;
        boolean fastDisk = true;
        String screenshotsDir = null;

        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--cycles" -> cycleCount = Integer.parseInt(args[++i]);
                case "--only" -> onlyFilter = args[++i];
                case "--seconds" -> effectDurationSeconds = Double.parseDouble(args[++i]);
                case "--photo-grid" -> photoGridResolution = args[++i];
                case "--no-fast" -> fastDisk = false;
                case "--shots" -> screenshotsDir = args[++i];
                default -> {
                    System.err.println("Unknown option: " + args[i]);
                    System.exit(2);
                }
            }
        }

        Boards.Board board = Boards.parse("S3");
        Optional<SerialPort> portOpt = Boards.find(board);
        if (portOpt.isEmpty()) {
            System.err.println("No S3 board found on USB");
            System.exit(2);
        }

        SerialPort serialPort = portOpt.get();
        if (!Boards.usable(serialPort)) {
            System.err.println(Boards.SANDBOX_HINT);
            System.exit(2);
        }

        String hidNode = Hid.find("S3", true);
        if (hidNode == null) {
            System.err.println("No USB HID device node found for the CoreS3");
            System.exit(2);
        }

        Set<String> selectedScenarios = new LinkedHashSet<>(Arrays.asList(onlyFilter.split(",")));

        try (Ser ser = new Ser(serialPort); Hid hid = new Hid(hidNode)) {
            Scenarios scenarios = new Scenarios(ser, hid, "S3", System.out::println);
            scenarios.seconds = effectDurationSeconds;
            scenarios.photoGrid = photoGridResolution;
            scenarios.shotDir = screenshotsDir;

            final String shotsFolder = screenshotsDir;
            if (shotsFolder != null) {
                Files.createDirectories(Path.of(shotsFolder));
                int[] shotCounter = {0};
                scenarios.preview = new Scenarios.Preview() {
                    @Override
                    public void screen(BufferedImage img) {
                        try {
                            Path targetPath = Path.of(shotsFolder, String.format("screen-%03d.png", shotCounter[0]++));
                            ImageIO.write(img, "png", targetPath.toFile());
                        } catch (IOException ignored) {}
                    }

                    @Override public void bar(int[] rgb) {}
                    @Override public void grid(int[] rgb, int cols, int rows) {}
                };
            }

            System.out.println("Demo runner connected to: " + ser.kv("info"));
            List<String[]> executionLog = new ArrayList<>();

            try {
                for (int c = 1; cycleCount == 0 || c <= cycleCount; c++) {
                    System.out.println("=== cycle " + c + " ===");
                    if (selectedScenarios.size() == 5 && selectedScenarios.contains("pages")) {
                        executionLog.addAll(scenarios.cycle(fastDisk));
                    } else {
                        for (String scenarioKey : selectedScenarios) {
                            long startNs = System.nanoTime();
                            String resultNote = switch (scenarioKey) {
                                case "pages" -> scenarios.walkPages();
                                case "disk" -> scenarios.disk(false);
                                case "bar" -> scenarios.bar();
                                case "grid" -> scenarios.grid();
                                case "photos" -> scenarios.photos();
                                case "mic" -> scenarios.micDemo();
                                case "scope" -> scenarios.micScope();
                                case "board" -> {
                                    StringBuilder sb = new StringBuilder();
                                    for (String eff : Scenarios.BOARD_EFFECTS) {
                                        sb.append(scenarios.boardEffect(eff)).append("; ");
                                    }
                                    yield sb.toString();
                                }
                                default -> "unknown scenario: " + scenarioKey;
                            };
                            executionLog.add(new String[]{
                                    scenarioKey,
                                    String.format("%.1f s", (System.nanoTime() - startNs) / 1e9),
                                    resultNote
                            });
                        }
                    }
                }
            } finally {
                scenarios.release();
            }

            System.out.println("\nSUMMARY");
            for (String[] entry : executionLog) {
                System.out.printf("  %-14s %8s  %s%n", entry[0], entry[1], entry[2]);
            }
            System.out.printf("checks: pass=%d fail=%d -> %s%n",
                    scenarios.pass, scenarios.fail, (scenarios.fail == 0) ? "PASS" : "FAIL");

            System.exit(scenarios.fail == 0 ? 0 : 1);
        }
    }
}
