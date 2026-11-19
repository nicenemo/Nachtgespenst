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
    static final String[] GRID_EFFECTS = {"raster bars", "sine scroller"};

    final Ser s; final Hid h; final Log log; final String tag; final int pages;
    volatile double seconds = 8;
    volatile String photoGrid = "160x95";
    volatile Preview preview = null;
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
        try { h.write(Proto.control(true)); s.cmd("LAMPS RESET", 2000); s.cmd("LAYER 0", 3000); s.cmd("WAKE", 1000); } catch (Exception ignored) { }
    }
}
