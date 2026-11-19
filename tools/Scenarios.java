import java.util.*;

/**
 * The demo scenarios for the CoreS3, shared by the command line runner (Demo) and the window (DemoApp). Everything is painted by the PC through
 * the LampArray, nothing is uploaded. Each method returns a one-line result; cancelling interrupts the thread that runs it.
 */
final class Scenarios {
    interface Log { void say(String s); }

    /** What the window shows next to the log: the screen of the board, and the lamps as the PC has worked them out. */
    interface Preview {
        void screen(java.awt.image.BufferedImage deviceScreen);
        void bar(int[] rgb);
        void grid(int[] rgb, int cols, int rows);
    }

    static final String[] BAR_EFFECTS = {"nightrider", "breath", "rainbow", "comet", "sparkle"};
    static final String[] BOARD_EFFECTS = {"nightrider", "breath", "rainbow", "comet", "sparkle"};
    static final String[] GRID_EFFECTS = {"raster bars", "sine scroller"};

    final Ser s; final Hid h; final Log log; final String tag; final int pages;
    volatile double seconds = 8;
    volatile String photoGrid = "160x95";
    volatile Preview preview = null;
    private Telemetry telemetry;                          // the bar colours and the microphone waveform come back over the game pad, not over serial
    private volatile int[] lastWave; private volatile long waveSeq;
    volatile String shotDir = null;                  // when set, the screen of the board is saved here after every photo
    volatile int pass = 0, fail = 0;

    Scenarios(Ser s, Hid h, String tag, Log log) throws Exception {
        this.s = s; this.h = h; this.tag = tag; this.log = log;
        this.pages = Integer.parseInt(s.kv("INFO").get("pages"));
    }

    void say(String m) { log.say(m); }

    /** Every page of the board, about a second each; no key or tap is ever sent (Power Down and Sleep are tiles on some pages). */
    String walkPages() throws Exception {
        for (int p = 0; p < pages; p++) {
            String r = s.cmd("LAYER " + p, 3000);
            Map<String, String> st = s.kv("STATE?");
            say(String.format("page %2d/%d  %s", p + 1, pages, st.getOrDefault("title", "?")));
            grabScreen();
            if (r == null || !r.startsWith("OK")) { fail++; say("LAYER " + p + " failed: " + r); }
            Thread.sleep(p >= pages - 2 ? 300 : 1100);
        }
        pass++;
        return pages + " pages";
    }

    /** The Jules Verne file to the board (and the SD card) and back over the game pad buttons, with SHA-256 on both ends. */
    String disk(boolean fast) throws Exception {
        s.cmd("LAYER " + (pages - 3), 3000); Thread.sleep(600);
        s.cmd("DISK RESET", 2000);
        DiskDrive.Options o = new DiskDrive.Options();
        o.via = "usb"; o.tag = tag; o.fast = fast;
        int[] f = {0, 0};
        new DiskDrive(o, new DiskDrive.Reporter() {
            public void check(String name, boolean ok, String detail) { if (ok) { pass++; f[0]++; } else { fail++; f[1]++; say("FAIL " + name + " " + detail); } }
            public void info(String msg) { if (!msg.startsWith("  ")) say(msg); }
        }).disk(s, h);
        Map<String, String> st = s.kv("DISK?");
        return String.format("save %.1f kB/s, back %.1f kB/s, SHA-256 %s, %d checks ok%s", Proto.num(st, "kbps10") / 10.0, Proto.num(st, "txkbps10") / 10.0,
                "1".equals(st.get("match")) ? "match" : "DIFFERENT", f[0], f[1] > 0 ? ", " + f[1] + " FAILED" : "");
    }

    void goBarPage() throws Exception {
        s.cmd("LAYER " + (pages - 1), 3000); Thread.sleep(400); h.write(Proto.control(false));
        Preview pv = preview; BarEffects.listener = pv == null ? null : pv::bar;
    }

    /** One screenshot of the board for the window (the picture the lamps or the page make on the device). */
    void grabScreen() throws Exception {
        Preview pv = preview; if (pv == null) return;
        Ser.Shot sh = s.shot(); if (sh == null) return;
        java.awt.image.BufferedImage img = new java.awt.image.BufferedImage(sh.w(), sh.h(), java.awt.image.BufferedImage.TYPE_INT_RGB);
        for (int y = 0; y < sh.h(); y++) for (int x = 0; x < sh.w(); x++) { int[] c = sh.at(x, y); img.setRGB(x, y, (c[0] << 16) | (c[1] << 8) | c[2]); }
        pv.screen(img);
    }

    Canvas canvas(int cols, int rows) throws Exception {
        Canvas c = new Canvas(s, h, cols, rows);
        Preview pv = preview; if (pv != null) c.listener = px -> pv.grid(px, cols, rows);
        return c;
    }
    void goGridPage() throws Exception { s.cmd("LAYER " + (pages - 2), 3000); Thread.sleep(400); }

    String barEffect(String name) throws Exception {
        goBarPage();
        say("bar: " + name);
        switch (name) {
            case "nightrider" -> BarEffects.nightrider(h, seconds);
            case "breath" -> BarEffects.breath(h, seconds);
            case "rainbow" -> BarEffects.rainbow(h, seconds);
            case "comet" -> BarEffects.comet(h, seconds);
            default -> BarEffects.sparkle(h, seconds);
        }
        BarEffects.listener = null; grabScreen();
        pass++;
        return name;
    }

    String bar() throws Exception { for (String n : BAR_EFFECTS) barEffect(n); return BAR_EFFECTS.length + " effects"; }

    String gridEffect(String name) throws Exception {
        goGridPage();
        Canvas c = canvas(40, 25);
        say("grid 40 x 25, the 16 C64 colours: " + name);
        long t0 = System.nanoTime();
        if (name.startsWith("raster")) GridEffects.rasterBars(c, seconds);
        else {
            c.fill(GridEffects.C64[0]); c.present();
            GridEffects.sineScroller(c, "NACHTGESPENST - ONE HID LAMPARRAY, TWO BOARDS - TWENTY THOUSAND LEAGUES UNDER THE SEA - GREETINGS FROM THE LAMPS", seconds * 1.5);
        }
        double sec = (System.nanoTime() - t0) / 1e9;
        say(String.format("%.0f reports per second, %d lamps painted", c.reports / sec, c.lamps));
        grabScreen();
        pass++;
        return String.format("%s: %d reports", name, c.reports);
    }

    /** Starts reading the telemetry of the board (the game pad reports of the HID node) for the window. */
    synchronized void telemetryOn(boolean bar, boolean wave) throws Exception {
        if (telemetry == null) {
            telemetry = new Telemetry(h.path, new Telemetry.Sink() {
                public void bar(int[] rgb) { Preview pv = preview; if (pv != null) pv.bar(rgb); }
                public void wave(int[] samples) { lastWave = samples; waveSeq++; }
            });
        }
        s.cmd("TELEM " + (bar && wave ? "BOTH" : wave ? "WAVE" : bar ? "BAR" : "OFF"), 1500);
    }
    synchronized void telemetryOff() {
        try { s.cmd("TELEM OFF", 1500); } catch (Exception ignored) { }
        if (telemetry != null) { telemetry.close(); telemetry = null; }
    }

    /** Mirrors the bar of the board in the window while an effect that runs on the board goes on: the board sends the colours over the game pad. */
    void mirrorBar(long ms) throws Exception {
        Thread.sleep(ms);
    }

    /** An effect that runs on the board itself (SET effect ...), in the colour of the layer; no lamp updates are sent. */
    String boardEffect(String name) throws Exception {
        s.cmd("LAYER 0", 3000); Thread.sleep(300);
        say("the board runs " + name + " on its own (the lamps are not used)");
        if (preview != null) telemetryOn(true, false);
        s.cmd("SET effect " + name, 2000);
        try { mirrorBar((long) (seconds * 1000)); } finally { s.cmd("SET effect none", 2000); telemetryOff(); }
        pass++;
        return name + " on the board";
    }

    /** The vu effect: the board listens with its microphone while the PC plays tones, and the bar follows the level. */
    String micDemo() throws Exception {
        s.cmd("LAYER 0", 3000); Thread.sleep(300);
        say("the vu meter on the bar follows the microphone; the PC plays tones of 440, 1000 and 2000 Hz");
        telemetryOn(true, true);
        s.cmd("SET effect vu", 2000); mirrorBar(1500);
        StringBuilder note = new StringBuilder();
        Scope probe = new Scope(160, 95);
        try {
            for (int hz : new int[]{440, 1000, 2000}) {
                Thread t = Tones.playAsync(hz, 2.2, 0.6);
                mirrorBar(1000);
                int[] w = lastWave;
                if (w != null) { probe.frame(null, w); say(String.format("%d Hz: the board hears about %.0f Hz (from its waveform, peak %d)", hz, probe.lastFreq, probe.lastPeak)); note.append(hz).append("->").append(Math.round(probe.lastFreq)).append(" "); }
                mirrorBar(1100);
                t.join();
                mirrorBar(600);
            }
        } finally { s.cmd("SET effect none", 2000); telemetryOff(); }
        pass++;
        return "microphone: " + note.toString().trim();
    }

    /**
     * The microphone as an oscilloscope on the lamps: the PC asks the board for the waveform it hears (WAVE?) and paints it on the 160 x 95 lamp
     * grid, so it shows on the screen of the board and in the window. The PC plays tones of 440, 880 and 1500 Hz through its speaker.
     */
    String micScope() throws Exception {
        goGridPage();
        Canvas c = canvas(80, 47);                         // 4 x 4 pixels per lamp: a scope that the lamp updates can keep up with
        Scope scope = new Scope(80, 47);
        s.cmd("MIC ON", 2000);
        telemetryOn(preview != null, true);
        mirrorWait(1200);
        Thread tones = Thread.ofVirtual().name("tones").start(() -> {
            try { Tones.play(440, 3.0, 0.5); Tones.play(880, 3.0, 0.5); Tones.play(1500, 3.0, 0.5); } catch (Exception e) { say("cannot play tones: " + e); }
        });
        say("the board listens and sends the waveform over the game pad; the oscilloscope is painted on the lamps (the PC plays 440, 880 and 1500 Hz)");
        long end = System.currentTimeMillis() + (long) Math.max(10000, seconds * 1000);
        int frames = 0; double lastFreq = 0; long seen = waveSeq; long t0 = System.currentTimeMillis(); boolean shotA = false, shotB = false;
        try {
            while (System.currentTimeMillis() < end) {
                if (waveSeq == seen || lastWave == null) { Thread.sleep(5); continue; }
                seen = waveSeq;
                scope.frame(c, lastWave);
                c.present(); frames++; if (scope.lastFreq > 0) lastFreq = scope.lastFreq;
                long el = System.currentTimeMillis() - t0;
                if (!shotA && el > 4500) { shotA = true; grabScreen(); } else if (!shotB && el > 7800) { shotB = true; grabScreen(); }
            }
        } finally { s.cmd("MIC OFF", 2000); telemetryOff(); }
        say(frames + " frames; the last frequency measured: " + Math.round(lastFreq) + " Hz");
        grabScreen();
        pass++;
        return frames + " scope frames from the microphone, painted on the lamps";
    }

    void mirrorWait(long ms) throws InterruptedException { Thread.sleep(ms); }

    String grid() throws Exception { for (String n : GRID_EFFECTS) gridEffect(n); return "raster bars and a sine scroller"; }

    /** Five photos, dissolved into each other with prime moduli. */
    String photos() throws Exception {
        goGridPage();
        String[] gp = photoGrid.split("x"); int cols = Integer.parseInt(gp[0]), rows = Integer.parseInt(gp[1]);
        Canvas c = canvas(cols, rows);
        int n = Photos.LIST.length; int[][] pic = new int[n][];
        for (int i = 0; i < n; i++) { pic[i] = Photos.toGrid(Photos.load(i), cols, rows); say("photo " + (i + 1) + ": " + Photos.LIST[i][1]); }
        long lamps = (long) cols * rows, m = GridEffects.nextPrime(lamps + 1);
        long[] primes = {7, 31, GridEffects.nearestPrime(Math.round(m * 0.618)), 97, 211};
        say(String.format("%d x %d lamps (%d), modulus m = %d", cols, rows, lamps, m));
        c.setAll(pic[0]); c.present();
        Thread.sleep(1500);
        StringBuilder note = new StringBuilder();
        for (int i = 1; i <= n; i++) {
            long p = primes[(i - 1) % primes.length];
            double sec = GridEffects.dissolve(c, pic[(i - 1) % n], pic[i % n], p, 400);
            say(String.format("photo %d -> %d, p = %d: %.2f s (%.0f lamps/s)", ((i - 1) % n) + 1, (i % n) + 1, p, sec, lamps / sec));
            note.append(String.format("p=%d %.1fs ", p, sec));
            if (shotDir != null) shot("photo" + (i % n + 1));
            grabScreen();
            Thread.sleep(1800);
        }
        pass++;
        return note.toString().trim();
    }

    /** The screen of the board as a PNG (the picture the lamps make), for the documentation. */
    void shot(String name) throws Exception {
        Ser.Shot sh = s.shot();
        if (sh == null) { say("no screenshot"); return; }
        java.awt.image.BufferedImage img = new java.awt.image.BufferedImage(sh.w(), sh.h(), java.awt.image.BufferedImage.TYPE_INT_RGB);
        for (int y = 0; y < sh.h(); y++) for (int x = 0; x < sh.w(); x++) { int[] c = sh.at(x, y); img.setRGB(x, y, (c[0] << 16) | (c[1] << 8) | c[2]); }
        java.nio.file.Path f = java.nio.file.Path.of(shotDir, name + ".png");
        java.nio.file.Files.createDirectories(f.getParent());
        javax.imageio.ImageIO.write(img, "png", f.toFile());
        say("saved " + f);
    }

    /** One round of everything. */
    List<String[]> cycle(boolean withFast) throws Exception {
        List<String[]> out = new ArrayList<>();
        record Step(String name, java.util.concurrent.Callable<String> run) { }
        List<Step> steps = new ArrayList<>(List.of(new Step("pages", this::walkPages), new Step("disk", () -> disk(false))));
        if (withFast) steps.add(new Step("disk (fast)", () -> disk(true)));
        steps.addAll(List.of(new Step("bar", this::bar), new Step("grid", this::grid), new Step("photos", this::photos)));
        for (Step st : steps) {
            say("--- " + st.name());
            long t0 = System.nanoTime();
            String note;
            try { note = st.run().call(); } catch (InterruptedException e) { throw e; } catch (Exception e) { note = "FAILED: " + e; fail++; say(note); }
            out.add(new String[]{st.name(), String.format("%.1f s", (System.nanoTime() - t0) / 1e9), note});
        }
        return out;
    }

    /** Gives the lamps back to the board and goes to the first page. */
    void release() {
        try { telemetryOff(); s.cmd("SET effect none", 1500); s.cmd("MIC OFF", 1500); h.write(Proto.control(true)); s.cmd("LAMPS RESET", 2000); s.cmd("LAYER 0", 3000); s.cmd("WAKE", 1000); } catch (Exception ignored) { }
    }
}
