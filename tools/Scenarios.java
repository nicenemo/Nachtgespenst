import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

/**
 * Automated demonstration scenarios and diagnostic test routines for Nachtgespenst boards.
 * <p>
 * Coordinates multi-subsystem demonstrations across USB HID, serial command shell,
 * audio tone generation, and hardware display controllers:
 * <ul>
 *     <li><b>Page Walk:</b> Sequences through all hardware display pages without sending keypresses.</li>
 *     <li><b>Disk over Lamps:</b> High-speed file transfer benchmark with SHA-256 validation.</li>
 *     <li><b>Bar Effects:</b> Real-time PC-rendered animations on the 10-LED base bar.</li>
 *     <li><b>Grid Effects:</b> C64-style copper raster bars and sinusoidal scrollers.</li>
 *     <li><b>Photo Dissolve:</b> High-resolution photo transitions using prime-modulus scattering.</li>
 *     <li><b>Microphone Scope &amp; VU:</b> Real-time PCM waveform capture and oscilloscope rendering.</li>
 * </ul>
 * </p>
 */
final class Scenarios {

    /** Log message callback. */
    interface Log {
        void say(String message);
    }

    /** Visual preview callback for GUI applications. */
    interface Preview {
        void screen(BufferedImage deviceScreen);
        void bar(int[] rgb);
        void grid(int[] rgb, int cols, int rows);
    }

    static final String[] BAR_EFFECTS = {"nightrider", "breath", "rainbow", "comet", "sparkle"};
    static final String[] BOARD_EFFECTS = {"nightrider", "breath", "rainbow", "comet", "sparkle"};
    static final String[] GRID_EFFECTS = {"raster bars", "sine scroller"};

    final Ser ser;
    final Hid hid;
    final Log log;
    final String tag;
    final int totalPages;

    volatile double seconds = 8.0;
    volatile String photoGrid = "160x95";
    volatile Preview preview = null;
    private Telemetry telemetry;
    private volatile int[] lastWaveform;
    private volatile long waveformSequence;
    volatile String shotDir = null;

    volatile int pass = 0;
    volatile int fail = 0;

    Scenarios(Ser ser, Hid hid, String tag, Log log) throws Exception {
        this.ser = ser;
        this.hid = hid;
        this.tag = tag;
        this.log = log;
        this.totalPages = Integer.parseInt(ser.kv("info").get("pages"));
    }

    void say(String msg) {
        log.say(msg);
    }

    /**
     * Cycles through every layer/page on the board, pausing ~1 second per page.
     *
     * @return summary string
     * @throws Exception if communication fails
     */
    String walkPages() throws Exception {
        for (int p = 0; p < totalPages; p++) {
            String reply = ser.cmd("layer " + p, 3000);
            Map<String, String> state = ser.kv("state");
            say(String.format("page %2d/%d  %s", p + 1, totalPages, state.getOrDefault("title", "?")));
            grabScreen();

            if (reply == null || !reply.startsWith("OK")) {
                fail++;
                say("LAYER " + p + " failed: " + reply);
            }
            Thread.sleep(p >= totalPages - 2 ? 300 : 1100);
        }
        pass++;
        return totalPages + " pages";
    }

    /**
     * Executes the Disk transfer scenario, sending a compressed book over the lamps
     * and receiving it back over gamepad reports.
     *
     * @param fast {@code true} to request fast uncompressed flash storage
     * @return summary performance string
     * @throws Exception if transfer fails
     */
    String disk(boolean fast) throws Exception {
        ser.cmd("layer " + (totalPages - 3), 3000);
        Thread.sleep(600);
        ser.cmd("disk reset", 2000);

        DiskDrive.Options opts = new DiskDrive.Options();
        opts.via = "usb";
        opts.tag = tag;
        opts.fast = fast;

        int[] counts = {0, 0}; // [pass, fail]
        new DiskDrive(opts, new DiskDrive.Reporter() {
            @Override
            public void check(String name, boolean ok, String detail) {
                if (ok) {
                    pass++;
                    counts[0]++;
                } else {
                    fail++;
                    counts[1]++;
                    say("FAIL " + name + " " + detail);
                }
            }

            @Override
            public void info(String msg) {
                if (!msg.startsWith("  ")) {
                    say(msg);
                }
            }
        }).disk(ser, hid);

        Map<String, String> diskStatus = ser.kv("disk");
        return String.format("save %.1f kB/s, back %.1f kB/s, SHA-256 %s, %d checks ok%s",
                Proto.num(diskStatus, "kbps10") / 10.0,
                Proto.num(diskStatus, "txkbps10") / 10.0,
                "1".equals(diskStatus.get("match")) ? "match" : "DIFFERENT",
                counts[0],
                counts[1] > 0 ? ", " + counts[1] + " FAILED" : "");
    }

    void enterBarPage() throws Exception {
        ser.cmd("layer " + (totalPages - 1), 3000);
        Thread.sleep(400);
        hid.write(Proto.control(false));
        Preview pv = preview;
        BarEffects.listener = (pv == null) ? null : pv::bar;
    }

    void grabScreen() throws Exception {
        Preview pv = preview;
        if (pv == null) return;
        Ser.Shot screenshot = ser.shot();
        if (screenshot == null) return;

        BufferedImage img = new BufferedImage(screenshot.width(), screenshot.height(), BufferedImage.TYPE_INT_RGB);
        for (int y = 0; y < screenshot.height(); y++) {
            for (int x = 0; x < screenshot.width(); x++) {
                int[] rgb = screenshot.at(x, y);
                img.setRGB(x, y, (rgb[0] << 16) | (rgb[1] << 8) | rgb[2]);
            }
        }
        pv.screen(img);
    }

    Canvas createCanvas(int cols, int rows) throws Exception {
        Canvas canvas = new Canvas(ser, hid, cols, rows);
        Preview pv = preview;
        if (pv != null) {
            canvas.listener = px -> pv.grid(px, cols, rows);
        }
        return canvas;
    }

    void enterGridPage() throws Exception {
        ser.cmd("layer " + (totalPages - 2), 3000);
        Thread.sleep(400);
    }

    String barEffect(String name) throws Exception {
        enterBarPage();
        say("bar: " + name);
        switch (name) {
            case "nightrider" -> BarEffects.nightrider(hid, seconds);
            case "breath" -> BarEffects.breath(hid, seconds);
            case "rainbow" -> BarEffects.rainbow(hid, seconds);
            case "comet" -> BarEffects.comet(hid, seconds);
            default -> BarEffects.sparkle(hid, seconds);
        }
        BarEffects.listener = null;
        grabScreen();
        pass++;
        return name;
    }

    String bar() throws Exception {
        for (String name : BAR_EFFECTS) {
            barEffect(name);
        }
        return BAR_EFFECTS.length + " effects";
    }

    String gridEffect(String name) throws Exception {
        enterGridPage();
        Canvas canvas = createCanvas(40, 25);
        say("grid 40 x 25, the 16 C64 colours: " + name);

        long startTime = System.nanoTime();
        if (name.startsWith("raster")) {
            GridEffects.rasterBars(canvas, seconds);
        } else {
            canvas.fill(GridEffects.C64[0]);
            canvas.present();
            GridEffects.sineScroller(
                    canvas,
                    "NACHTGESPENST - ONE HID LAMPARRAY, TWO BOARDS - TWENTY THOUSAND LEAGUES UNDER THE SEA - GREETINGS FROM THE LAMPS",
                    seconds * 1.5
            );
        }

        double duration = (System.nanoTime() - startTime) / 1e9;
        say(String.format("%.0f reports per second, %d lamps painted", canvas.reports / duration, canvas.lamps));
        grabScreen();
        pass++;
        return String.format("%s: %d reports", name, canvas.reports);
    }

    synchronized void telemetryOn(boolean bar, boolean wave) throws Exception {
        if (telemetry == null) {
            telemetry = new Telemetry(hid.path, new Telemetry.Sink() {
                @Override
                public void bar(int[] rgb) {
                    Preview pv = preview;
                    if (pv != null) pv.bar(rgb);
                }

                @Override
                public void wave(int[] samples) {
                    lastWaveform = samples;
                    waveformSequence++;
                }
            });
        }
        ser.cmd("telem " + (bar && wave ? "BOTH" : wave ? "WAVE" : bar ? "BAR" : "OFF"), 1500);
    }

    synchronized void telemetryOff() {
        try {
            ser.cmd("telem off", 1500);
        } catch (Exception ignored) {}
        if (telemetry != null) {
            telemetry.close();
            telemetry = null;
        }
    }

    String boardEffect(String name) throws Exception {
        ser.cmd("layer 0", 3000);
        Thread.sleep(300);
        say("the board runs " + name + " on its own (the lamps are not used)");

        if (preview != null) {
            telemetryOn(true, false);
        }
        ser.cmd("set effect " + name, 2000);
        try {
            Thread.sleep((long) (seconds * 1000));
        } finally {
            ser.cmd("set effect none", 2000);
            telemetryOff();
        }
        pass++;
        return name + " on the board";
    }

    String micDemo() throws Exception {
        ser.cmd("layer 0", 3000);
        Thread.sleep(300);
        say("the vu meter on the bar follows the microphone; the PC plays tones of 440, 1000 and 2000 Hz");

        telemetryOn(true, true);
        ser.cmd("set effect vu", 2000);
        Thread.sleep(1500);

        StringBuilder note = new StringBuilder();
        Scope probe = new Scope(160, 95);

        try {
            for (int hz : new int[]{440, 1000, 2000}) {
                Thread toneThread = Tones.playAsync(hz, 2.2, 0.6);
                Thread.sleep(1000);
                int[] w = lastWaveform;
                if (w != null) {
                    probe.frame(null, w);
                    say(String.format("%d Hz: board hears about %.0f Hz (from waveform, peak %d)", hz, probe.lastFreq, probe.lastPeak));
                    note.append(hz).append("->").append(Math.round(probe.lastFreq)).append(" ");
                }
                Thread.sleep(1100);
                toneThread.join();
                Thread.sleep(600);
            }
        } finally {
            ser.cmd("set effect none", 2000);
            telemetryOff();
        }
        pass++;
        return "microphone: " + note.toString().trim();
    }

    String micScope() throws Exception {
        enterGridPage();
        Canvas canvas = createCanvas(80, 47);
        Scope scope = new Scope(80, 47);

        ser.cmd("mic on", 2000);
        telemetryOn(preview != null, true);
        Thread.sleep(1200);

        Thread tonePlayer = Thread.ofVirtual().name("tones-player").start(() -> {
            try {
                Tones.play(440, 3.0, 0.5);
                Tones.play(880, 3.0, 0.5);
                Tones.play(1500, 3.0, 0.5);
            } catch (Exception e) {
                say("cannot play tones: " + e.getMessage());
            }
        });

        say("the board listens and sends the waveform over the game pad; the oscilloscope is painted on the lamps");
        long deadline = System.currentTimeMillis() + (long) Math.max(10000, seconds * 1000);
        int frameCount = 0;
        double finalFreq = 0;
        long seenSeq = waveformSequence;
        long startMs = System.currentTimeMillis();
        boolean shot1Taken = false;
        boolean shot2Taken = false;

        try {
            while (System.currentTimeMillis() < deadline) {
                if (waveformSequence == seenSeq || lastWaveform == null) {
                    Thread.sleep(5);
                    continue;
                }
                seenSeq = waveformSequence;
                scope.frame(canvas, lastWaveform);
                canvas.present();
                frameCount++;
                if (scope.lastFreq > 0) {
                    finalFreq = scope.lastFreq;
                }

                long elapsed = System.currentTimeMillis() - startMs;
                if (!shot1Taken && elapsed > 4500) {
                    shot1Taken = true;
                    grabScreen();
                } else if (!shot2Taken && elapsed > 7800) {
                    shot2Taken = true;
                    grabScreen();
                }
            }
        } finally {
            ser.cmd("mic off", 2000);
            telemetryOff();
        }

        say(frameCount + " frames; last frequency measured: " + Math.round(finalFreq) + " Hz");
        grabScreen();
        pass++;
        return frameCount + " scope frames from the microphone, painted on the lamps";
    }

    String grid() throws Exception {
        for (String n : GRID_EFFECTS) {
            gridEffect(n);
        }
        return "raster bars and a sine scroller";
    }

    String photos() throws Exception {
        enterGridPage();
        String[] dims = photoGrid.split("x");
        int cols = Integer.parseInt(dims[0]);
        int rows = Integer.parseInt(dims[1]);
        Canvas canvas = createCanvas(cols, rows);

        int count = Photos.LIST.size();
        int[][] photoBuffers = new int[count][];
        for (int i = 0; i < count; i++) {
            photoBuffers[i] = Photos.toGrid(Photos.load(i), cols, rows);
            say("photo " + (i + 1) + ": " + Photos.LIST.get(i).credit());
        }

        long totalLamps = (long) cols * rows;
        long modulus = GridEffects.nextPrime(totalLamps + 1);
        long[] coprimeSteps = {7, 31, GridEffects.nearestPrime(Math.round(modulus * 0.618)), 97, 211};

        say(String.format("%d x %d lamps (%d), modulus m = %d", cols, rows, totalLamps, modulus));
        canvas.setAll(photoBuffers[0]);
        canvas.present();
        Thread.sleep(1500);

        StringBuilder notes = new StringBuilder();
        for (int i = 1; i <= count; i++) {
            long p = coprimeSteps[(i - 1) % coprimeSteps.length];
            double sec = GridEffects.dissolve(canvas, photoBuffers[(i - 1) % count], photoBuffers[i % count], p, 400);
            say(String.format("photo %d -> %d, p = %d: %.2f s (%.0f lamps/s)",
                    ((i - 1) % count) + 1, (i % count) + 1, p, sec, totalLamps / sec));
            notes.append(String.format("p=%d %.1fs ", p, sec));

            if (shotDir != null) {
                saveShot("photo" + (i % count + 1));
            }
            grabScreen();
            Thread.sleep(1800);
        }
        pass++;
        return notes.toString().trim();
    }

    void saveShot(String name) throws Exception {
        Ser.Shot screenshot = ser.shot();
        if (screenshot == null) {
            say("no screenshot captured");
            return;
        }

        BufferedImage img = new BufferedImage(screenshot.width(), screenshot.height(), BufferedImage.TYPE_INT_RGB);
        for (int y = 0; y < screenshot.height(); y++) {
            for (int x = 0; x < screenshot.width(); x++) {
                int[] rgb = screenshot.at(x, y);
                img.setRGB(x, y, (rgb[0] << 16) | (rgb[1] << 8) | rgb[2]);
            }
        }

        Path file = Path.of(shotDir, name + ".png");
        Files.createDirectories(file.getParent());
        javax.imageio.ImageIO.write(img, "png", file.toFile());
        say("saved " + file);
    }

    List<String[]> cycle(boolean withFast) throws Exception {
        return cycle(withFast, false);
    }

    List<String[]> cycle(boolean withFast, boolean withMic) throws Exception {
        List<String[]> results = new ArrayList<>();
        record ScenarioStep(String name, Callable<String> action) {}

        List<ScenarioStep> steps = new ArrayList<>(List.of(
                new ScenarioStep("pages", this::walkPages),
                new ScenarioStep("disk", () -> disk(false))
        ));

        if (withFast) {
            steps.add(new ScenarioStep("disk (fast)", () -> disk(true)));
        }
        steps.addAll(List.of(
                new ScenarioStep("bar", this::bar),
                new ScenarioStep("grid", this::grid),
                new ScenarioStep("photos", this::photos)
        ));

        if (withMic) {
            steps.add(new ScenarioStep("scope", this::micScope));
        }

        for (ScenarioStep step : steps) {
            say("--- " + step.name());
            long startNs = System.nanoTime();
            String note;
            try {
                note = step.action().call();
            } catch (InterruptedException e) {
                throw e;
            } catch (Exception e) {
                note = "FAILED: " + e.getMessage();
                fail++;
                say(note);
            }
            results.add(new String[]{
                    step.name(),
                    String.format("%.1f s", (System.nanoTime() - startNs) / 1e9),
                    note
            });
        }
        return results;
    }

    void release() {
        try {
            telemetryOff();
            ser.cmd("set effect none", 1500);
            ser.cmd("mic off", 1500);
            hid.write(Proto.control(true));
            ser.cmd("lamps reset", 2000);
            ser.cmd("layer 0", 3000);
            ser.cmd("wake", 1000);
        } catch (Exception ignored) {}
    }
}
