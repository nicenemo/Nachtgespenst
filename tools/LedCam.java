///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//SOURCES Boards.java
//SOURCES V4l2.java
//JAVA 27
//COMPILE_OPTIONS --add-modules=jdk.incubator.vector
//JAVA_OPTIONS --add-modules=jdk.incubator.vector --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;
import jdk.incubator.vector.FloatVector;
import jdk.incubator.vector.VectorOperators;
import jdk.incubator.vector.VectorSpecies;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.List;
import java.util.regex.*;

/**
 * Films the LED bars of the Nachtgespenst boards with the laptop webcam (Video4Linux2 in plain Java, tools/V4l2.java) and drives the boards over serial.
 *
 * <pre>
 * jbang tools/LedCam.java pins    --port /dev/ttyACM0 --out DIR            CoreS3 pin finder (flash tools/ledtest env cores3 first)
 * jbang tools/LedCam.java colours --port /dev/ttyUSB0 --out DIR [--crop x0,y0,x1,y1] [--clean]
 *                                                                          cycle LAYER n, film the bar, write DIR/montage.png
 * common: --camera /dev/video0  --size 1280x720
 * </pre>
 * Run it as a single plain command (outside the Claude Code sandbox it needs the serial port and the camera).
 * Frame differencing uses the Vector API (jdk.incubator.vector, JDK 27 still ships it as an incubator module).
 * The camera is the bottleneck, so the speed-up is small; the numeric colour verdict is weak, judge montage.png by eye.
 */
public class LedCam {
    // ------------------------------------------------------------------ options
    static String port, out = "ledcam-out", camera = "/dev/video0", size = "1280x720";
    static int[] crop = null;
    static boolean clean = false;
    static boolean others = true;              // switch the LED bar of the other board off during the test (LEDS OFF), back on afterwards

    public static void main(String[] args) throws Exception {
        if (args.length == 0 || !(args[0].equals("pins") || args[0].equals("colours") || args[0].equals("dark") || args[0].equals("locks"))) {
            System.err.println("usage: LedCam.java pins|colours|dark|locks --port P --out DIR [--camera DEV] [--size WxH] [--crop x0,y0,x1,y1] [--clean]");
            System.exit(2);
        }
        for (int i = 1; i < args.length; i++) switch (args[i]) {
            case "--port" -> port = args[++i];
            case "--out" -> out = args[++i];
            case "--camera" -> camera = args[++i];
            case "--size" -> size = args[++i];
            case "--crop" -> crop = Arrays.stream(args[++i].split(",")).mapToInt(Integer::parseInt).toArray();
            case "--clean" -> clean = true;
            case "--keep-others" -> others = false;
            default -> { System.err.println("unknown option " + args[i]); System.exit(2); }
        }
        if (port == null) { System.err.println("--port is required"); System.exit(2); }
        Files.createDirectories(Path.of(out));
        if (others) otherBars(port, false);
        try (Link link = new Link(port)) {
            if (args[0].equals("pins")) pins(link); else if (args[0].equals("dark")) dark(link); else if (args[0].equals("locks")) locks(link); else colours(link);
        } finally {
            if (others) otherBars(port, true);
        }
    }

    // ------------------------------------------------------------------ serial
    static class Link implements AutoCloseable {
        final SerialPort p;
        Link(String name) {
            p = SerialPort.getCommPort(name);
            p.setBaudRate(115200);
            p.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
            if (!p.openPort()) throw new IllegalStateException("cannot open " + name);
        }
        void send(String s) { byte[] b = (s + "\n").getBytes(StandardCharsets.US_ASCII); p.writeBytes(b, b.length); }
        /** Collect lines until one starts with prefix (or the timeout passes). */
        List<String> readUntil(String prefix, long timeoutMs) throws InterruptedException {
            List<String> lines = new ArrayList<>();
            StringBuilder sb = new StringBuilder();
            long end = System.currentTimeMillis() + timeoutMs;
            byte[] one = new byte[256];
            while (System.currentTimeMillis() < end) {
                int n = p.bytesAvailable() > 0 ? p.readBytes(one, Math.min(one.length, p.bytesAvailable())) : 0;
                if (n <= 0) { Thread.sleep(10); continue; }
                for (int i = 0; i < n; i++) {
                    char c = (char) (one[i] & 0xff);
                    if (c == '\n') {
                        String l = sb.toString().trim(); sb.setLength(0);
                        lines.add(l);
                        if (l.startsWith(prefix)) return lines;
                    } else if (c != '\r') sb.append(c);
                }
            }
            return lines;
        }
        public void close() { p.closePort(); }
    }

    /**
     * The LED bar of the other board lights the white sheet and the camera sees it: switch it off for the length of the test
     * (LEDS OFF over its serial port) and back on afterwards (LEDS ON). The setting is stored on that board, so a run that is
     * killed halfway leaves it dark: send LEDS ON to it. Opening the port can reset the board, hence the wait.
     */
    static void otherBars(String ownPort, boolean on) {
        for (Boards.Board b : List.of(Boards.C2, Boards.S3)) {
            Optional<SerialPort> sp = Boards.find(b);
            if (sp.isEmpty() || Boards.path(sp.get()).equals(ownPort)) continue;
            try (Link l = new Link(Boards.path(sp.get()))) {
                for (int attempt = 0; attempt < 4; attempt++) {
                    Thread.sleep(attempt == 0 ? 2500 : 1000);
                    l.send(on ? "LEDS ON" : "LEDS OFF");
                    if (l.readUntil("OK LEDS", 1500).stream().anyMatch(x -> x.startsWith("OK LEDS"))) { System.out.println(b.tag() + " bar " + (on ? "on" : "off")); break; }
                }
            } catch (Exception e) { System.err.println("could not switch the " + b.tag() + " bar " + (on ? "on" : "off") + ": " + e.getMessage()); }
        }
    }

    // ------------------------------------------------------------------ camera frames
    /** Planar float copy of a frame, so the Vector API kernels can run over contiguous channel arrays. */
    record Planes(int w, int h, float[] r, float[] g, float[] b) {
        static Planes of(BufferedImage img) {
            int w = img.getWidth(), h = img.getHeight();
            int[] px = img.getRGB(0, 0, w, h, null, 0, w);
            float[] r = new float[px.length], g = new float[px.length], b = new float[px.length];
            for (int i = 0; i < px.length; i++) { r[i] = (px[i] >> 16) & 255; g[i] = (px[i] >> 8) & 255; b[i] = px[i] & 255; }
            return new Planes(w, h, r, g, b);
        }
    }

    record Frame(Path path, BufferedImage img, Planes planes) {}

    /** One webcam frame, through V4l2.java (plain Java, no ffmpeg); 25 frames are read and the last kept, so the auto exposure can settle. */
    static Frame frame(String name) throws Exception {
        Path path = Path.of(out, name);
        String[] wh = size.split("x");
        V4l2 cam = new V4l2(camera, Integer.parseInt(wh[0]), Integer.parseInt(wh[1]));
        BufferedImage img = null;
        try {
            for (int n = 0, tries = 0; n < 25 && tries < 200; tries++) { BufferedImage im = cam.next(500, 0); if (im != null) { img = im; n++; } }
        } finally { cam.close(); }
        if (img == null) throw new IOException("no picture from " + camera);
        ImageIO.write(img, "png", path.toFile());
        return new Frame(path, img, Planes.of(img));
    }

    // ------------------------------------------------------------------ Vector API kernels
    static final VectorSpecies<Float> SP = FloatVector.SPECIES_PREFERRED;

    /** out[i] = |a.r-b.r| + |a.g-b.g| + |a.b-b.b| for every pixel. */
    static float[] absDiffSum(Planes a, Planes b) {
        int n = a.r.length; float[] out = new float[n];
        int i = 0, bound = SP.loopBound(n);
        for (; i < bound; i += SP.length()) {
            FloatVector dr = FloatVector.fromArray(SP, a.r, i).sub(FloatVector.fromArray(SP, b.r, i)).abs();
            FloatVector dg = FloatVector.fromArray(SP, a.g, i).sub(FloatVector.fromArray(SP, b.g, i)).abs();
            FloatVector db = FloatVector.fromArray(SP, a.b, i).sub(FloatVector.fromArray(SP, b.b, i)).abs();
            dr.add(dg).add(db).intoArray(out, i);
        }
        for (; i < n; i++) out[i] = Math.abs(a.r[i] - b.r[i]) + Math.abs(a.g[i] - b.g[i]) + Math.abs(a.b[i] - b.b[i]);
        return out;
    }

    /** Mean signed (a - b) per channel over a rectangle. */
    static double[] roiMeanDiff(Planes a, Planes b, int x0, int y0, int x1, int y1) {
        x0 = Math.max(0, x0); y0 = Math.max(0, y0); x1 = Math.min(a.w, x1); y1 = Math.min(a.h, y1);
        double[] sum = new double[3];
        for (int y = y0; y < y1; y++) {
            int row = y * a.w, i = row + x0, end = row + x1, bound = i + SP.loopBound(x1 - x0);
            FloatVector r = FloatVector.zero(SP), g = FloatVector.zero(SP), bl = FloatVector.zero(SP);
            for (; i < bound; i += SP.length()) {
                r = r.add(FloatVector.fromArray(SP, a.r, i).sub(FloatVector.fromArray(SP, b.r, i)));
                g = g.add(FloatVector.fromArray(SP, a.g, i).sub(FloatVector.fromArray(SP, b.g, i)));
                bl = bl.add(FloatVector.fromArray(SP, a.b, i).sub(FloatVector.fromArray(SP, b.b, i)));
            }
            sum[0] += r.reduceLanes(VectorOperators.ADD); sum[1] += g.reduceLanes(VectorOperators.ADD); sum[2] += bl.reduceLanes(VectorOperators.ADD);
            for (; i < end; i++) { sum[0] += a.r[i] - b.r[i]; sum[1] += a.g[i] - b.g[i]; sum[2] += a.b[i] - b.b[i]; }
        }
        double n = Math.max(1, (double) (x1 - x0) * (y1 - y0));
        for (int c = 0; c < 3; c++) sum[c] /= n;
        return sum;
    }

    /** Mean of the k largest values: a small lit area (an LED bar) still stands out, unlike with a percentile. */
    static double topMean(float[] v, int k) {
        float[] s = v.clone(); Arrays.sort(s);
        double sum = 0;
        for (int i = s.length - k; i < s.length; i++) sum += s[i];
        return sum / k;
    }

    // ------------------------------------------------------------------ pins: CoreS3 pin finder
    record PinResult(int gpio, String label, double score, double[] rgb) {}

    static void pins(Link l) throws Exception {
        List<int[]> gpios = new ArrayList<>(); List<String> labels = new ArrayList<>();
        for (int attempt = 0; attempt < 3 && gpios.isEmpty(); attempt++) {   // opening the port can reset the board: retry LIST
            Thread.sleep(1500);
            l.send("LIST");
            for (String line : l.readUntil("OK LIST", 4000))
                if (line.startsWith("CAND ")) { String[] t = line.split(" "); gpios.add(new int[]{Integer.parseInt(t[1])}); labels.add(t[2]); }
        }
        if (gpios.isEmpty()) throw new IllegalStateException("no candidates: is the tools/ledtest cores3 pin finder flashed?");
        System.out.println("candidates: " + gpios.size());
        Frame base = frame("base.jpg");
        List<PinResult> results = new ArrayList<>();
        for (int k = 0; k < gpios.size(); k++) {
            int gpio = gpios.get(k)[0];
            l.send("PIN " + gpio); Thread.sleep(150);
            double best = 0; double[] bestRgb = {0, 0, 0};
            for (int phase = 0; phase < 3; phase++) {          // the sketch shows R, G, B for 0.6 s each
                Thread.sleep(450);
                Frame f = frame(String.format("pin%02d_%d.jpg", gpio, phase));
                float[] d = absDiffSum(f.planes, base.planes);
                double score = topMean(d, 200);
                if (score > best) { best = score; bestRgb = litMean(f.planes, d); }
            }
            l.readUntil("OK PIN", 4000); Thread.sleep(300);
            results.add(new PinResult(gpio, labels.get(k), best, bestRgb));
            System.out.printf("GPIO %2d %-8s score=%6.1f rgb=%s%n", gpio, labels.get(k), best, Arrays.toString(round(bestRgb)));
        }
        results.sort(Comparator.comparingDouble(PinResult::score).reversed());
        System.out.print("TOP:");
        for (int i = 0; i < Math.min(3, results.size()); i++) System.out.printf(" GPIO %d (%s) %.1f;", results.get(i).gpio, results.get(i).label, results.get(i).score);
        System.out.println();
        if (clean) deleteFrames();
    }

    /** Mean colour of the pixels that changed noticeably (summed |diff| above 60). */
    static double[] litMean(Planes f, float[] d) {
        double[] s = new double[3]; int n = 0;
        for (int i = 0; i < d.length; i++) if (d[i] > 60) { s[0] += f.r[i]; s[1] += f.g[i]; s[2] += f.b[i]; n++; }
        if (n > 0) for (int c = 0; c < 3; c++) s[c] /= n;
        return s;
    }

    // ------------------------------------------------------------------ colours: layer colour check
    static final Map<String, int[]> CSS = new HashMap<>();
    static {
        String[] t = {"Blue 0 0 255", "Red 255 0 0", "Orange 255 165 0", "Green 0 128 0", "Purple 128 0 128", "Cyan 0 255 255",
            "Yellow 255 255 0", "White 255 255 255", "Magenta 255 0 255", "Lime 0 255 0", "Maroon 128 0 0", "Navy 0 0 128",
            "Olive 128 128 0", "Teal 0 128 128", "DeepPink 255 20 147", "Gold 255 215 0", "Indigo 75 0 130", "DarkGreen 0 100 0",
            "DarkOrange 255 140 0", "DarkViolet 148 0 211", "SkyBlue 135 206 235", "LightCoral 240 128 128", "PaleGreen 152 251 152",
            "Khaki 240 230 140", "Lavender 230 230 250", "MistyRose 255 228 225"};
        for (String s : t) { String[] p = s.split(" "); CSS.put(p[0], new int[]{Integer.parseInt(p[1]), Integer.parseInt(p[2]), Integer.parseInt(p[3])}); }
    }

    record Expect(String name, int[] rgb) {}

    /** Layer order in Keymap.h -> LAYER_COLORS index -> colour name (FastLED names equal the CSS ones used here). */
    static List<Expect> expectations() throws IOException {
        Path root = Files.exists(Path.of("include/Keymap.h")) ? Path.of(".") : Path.of("..");
        String colours = Files.readString(root.resolve("include/LedColors.h"));
        colours = colours.substring(colours.indexOf("LAYER_COLORS[]"));
        colours = colours.substring(0, colours.indexOf("};"));
        List<String> names = new ArrayList<>();
        Matcher m = Pattern.compile("CRGB::(\\w+)").matcher(colours);
        while (m.find()) names.add(m.group(1));
        List<Expect> out = new ArrayList<>();
        Matcher k = Pattern.compile("LAYER_COLORS\\[(\\d+)\\]").matcher(Files.readString(root.resolve("include/Keymap.h")));
        while (k.find()) { String n = names.get(Integer.parseInt(k.group(1))); out.add(new Expect(n, CSS.get(n))); }
        return out;
    }

    static void colours(Link l) throws Exception {
        l.send("info");
        int locks = -1;
        for (String line : l.readUntil("OK INFO", 3000)) { Matcher m = Pattern.compile("locks=(\\d+)").matcher(line); if (m.find()) locks = Integer.parseInt(m.group(1)); }
        if (locks < 0) throw new IllegalStateException("board did not answer INFO");
        List<Expect> exp = expectations();
        layer(l, locks);                                          // Locks page, nothing active = faint white = dark baseline
        Frame base = frame("base.jpg");
        int w = base.planes.w, h = base.planes.h;
        int[][] rois = {{w / 5, h * 3 / 5, w * 3 / 5, h}, {w * 3 / 5, h * 3 / 5, w, h}};   // table glow below the left and right device
        String[] sides = {"left", "right"};
        List<Frame> frames = new ArrayList<>();
        int bad = 0;
        for (int n = 0; n < exp.size(); n++) {
            layer(l, n);
            Frame f = frame(String.format("L%02d.jpg", n)); frames.add(f);
            double[][] m = new double[2][];
            for (int s = 0; s < 2; s++) m[s] = roiMeanDiff(f.planes, base.planes, rois[s][0], rois[s][1], rois[s][2], rois[s][3]);
            int side = mag(m[0]) >= mag(m[1]) ? 0 : 1;
            String verdict = verdict(exp.get(n).rgb, m[side]);
            if (verdict.startsWith("CHECK")) bad++;
            System.out.printf("L%02d %-12s exp=%s side=%-5s glow=%s mag=%5.1f %s%n", n, exp.get(n).name, Arrays.toString(exp.get(n).rgb),
                    sides[side], Arrays.toString(round(m[side])), mag(m[side]), verdict);
        }
        layer(l, 0);
        Path png = Path.of(out, "montage.png");
        montage(frames, exp, png);
        System.out.println("numeric CHECKs: " + bad + " (weak signal, the glow is only a few brightness levels; judge " + png + " by eye)");
        if (clean) deleteFrames();
    }

    // ------------------------------------------------------------------ dark: LEDS OFF really switches the bar off
    /** Films the bar with LEDS ON (layer 0, blue), LEDS OFF twice and LEDS LOCKS on a layer page, and compares. Restores LEDS ON. */
    static void dark(Link l) throws Exception {
        int fails = 0;
        l.send("set defaults 1"); l.readUntil("OK", 1500);
        layer(l, 0);
        Frame on = shot(l, "LEDS ON", "on.jpg");
        Frame off1 = shot(l, "LEDS OFF", "off1.jpg");
        Frame off2 = shot(l, "LEDS OFF", "off2.jpg");
        Frame locks = shot(l, "LEDS LOCKS", "locks.jpg");
        Frame back = shot(l, "LEDS ON", "back.jpg");
        int[] c = crop != null ? crop : new int[]{0, 0, on.planes.w, on.planes.h};
        double onOff = mag(roiMeanDiff(on.planes, off1.planes, c[0], c[1], c[2], c[3]));
        double noise = mag(roiMeanDiff(off1.planes, off2.planes, c[0], c[1], c[2], c[3]));
        double locksVsOff = mag(roiMeanDiff(locks.planes, off1.planes, c[0], c[1], c[2], c[3]));
        double backVsOn = mag(roiMeanDiff(back.planes, on.planes, c[0], c[1], c[2], c[3]));
        System.out.printf("glow LEDS ON vs OFF: %.1f   OFF vs OFF (camera noise): %.1f   LEDS LOCKS vs OFF: %.1f   back ON vs first ON: %.1f%n", onOff, noise, locksVsOff, backVsOn);
        fails += verdictLine("LEDS OFF is clearly darker than LEDS ON", onOff > 6 && onOff > 4 * noise);
        fails += verdictLine("two LEDS OFF frames look the same (the bar is really dark)", noise < 3);
        // "Locks only" shows the host lock state on every page: dark when no lock is on, lit when one is.
        l.send("state");
        boolean anyLock = false;
        for (String line : l.readUntil("OK STATE", 1500)) { Matcher m = Pattern.compile("caps=(\\d) num=(\\d) scroll=(\\d)").matcher(line); if (m.find()) anyLock = !(m.group(1) + m.group(2) + m.group(3)).equals("000"); }
        fails += verdictLine("LEDS LOCKS on a layer page " + (anyLock ? "shows the lock that is on (host lock active)" : "is as dark as OFF (no lock is on)"),
                anyLock ? locksVsOff > Math.max(6, noise * 4) : locksVsOff < Math.max(3, noise * 2));
        fails += verdictLine("LEDS ON brings the colours back", backVsOn < onOff * 0.35);
        l.send("leds on"); l.readUntil("OK", 1500);
        if (clean) deleteFrames();
        System.out.println(fails == 0 ? "dark: PASS" : "dark: " + fails + " FAIL");
        if (fails > 0) System.exit(1);
    }

    // ------------------------------------------------------------------ locks: the Locks page colours
    /** Reads caps/num/scroll from STATE?, or null. */
    static int[] lockState(Link l) throws Exception {
        l.send("state");
        for (String line : l.readUntil("OK STATE", 1500)) {
            Matcher m = Pattern.compile("caps=(\\d) num=(\\d) scroll=(\\d)").matcher(line);
            if (m.find()) return new int[]{Integer.parseInt(m.group(1)), Integer.parseInt(m.group(2)), Integer.parseInt(m.group(3))};
        }
        return null;
    }

    /**
     * Films the bar on the Locks page for none, each single lock and some mixes, and compares the hue with the colours the
     * firmware uses (caps orange, num green, scroll blue, additive mix). The locks are toggled with KEY on the Locks page,
     * which types the real keys on the host and, on a Bluetooth-only board, toggles the shown state at once (the Core2); the
     * original lock state is restored. A board with a USB host (CoreS3) follows the host's own LED report instead.
     */
    static void locks(Link l) throws Exception {
        l.send("info");
        int lockPage = -1;
        for (String line : l.readUntil("OK INFO", 3000)) { Matcher m = Pattern.compile("locks=(\\d+)").matcher(line); if (m.find()) lockPage = Integer.parseInt(m.group(1)); }
        if (lockPage < 0) throw new IllegalStateException("board did not answer INFO");
        layer(l, lockPage);
        int[] start = lockState(l);
        if (start == null) throw new IllegalStateException("no STATE? answer");
        int[][] combos = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 0}, {0, 1, 1}, {1, 1, 1}};
        String[] names = {"none", "caps", "num", "scroll", "caps+num", "num+scroll", "all three"};
        int[][] rgb = {{255, 80, 0}, {0, 220, 60}, {40, 80, 255}};
        List<Frame> frames = new ArrayList<>(); List<Expect> exp = new ArrayList<>();
        Frame base = null; int bad = 0, unreachable = 0;
        for (int k = 0; k < combos.length; k++) {
            int[] want = combos[k];
            for (int attempt = 0; attempt < 3; attempt++) {
                int[] st = lockState(l);
                if (st == null || Arrays.equals(st, want)) break;
                for (int b = 0; b < 3; b++) if (st[b] != want[b]) { l.send("key " + lockPage + " " + b); l.readUntil("OK", 1500); Thread.sleep(500); }
                Thread.sleep(600);
            }
            int[] got = lockState(l);
            if (got == null || !Arrays.equals(got, want)) { System.out.printf("%-10s state %s could not be reached (board shows %s): skipped%n", names[k], Arrays.toString(want), Arrays.toString(got)); unreachable++; continue; }
            Thread.sleep(700);
            Frame f = frame("lock" + k + ".jpg");
            int[] e = {0, 0, 0}; int n = 0;
            for (int b = 0; b < 3; b++) if (want[b] == 1) { for (int c = 0; c < 3; c++) e[c] = Math.min(255, e[c] + rgb[b][c]); n++; }
            if (k == 0) { base = f; System.out.println("baseline (no lock on): captured"); frames.add(f); exp.add(new Expect(names[k], new int[]{128, 128, 128})); continue; }
            int w = f.planes.w, h = f.planes.h;
            int[][] rois = {{w / 5, h * 3 / 5, w * 3 / 5, h}, {w * 3 / 5, h * 3 / 5, w, h}};
            double[] m0 = roiMeanDiff(f.planes, base.planes, rois[0][0], rois[0][1], rois[0][2], rois[0][3]);
            double[] m1 = roiMeanDiff(f.planes, base.planes, rois[1][0], rois[1][1], rois[1][2], rois[1][3]);
            double[] m = mag(m0) >= mag(m1) ? m0 : m1;
            String v = verdict(e, m);
            if (v.startsWith("CHECK")) bad++;
            System.out.printf("%-10s exp=%s glow=%s mag=%5.1f %s%n", names[k], Arrays.toString(e), Arrays.toString(round(m)), mag(m), v);
            frames.add(f); exp.add(new Expect(names[k], e));
        }
        ensureState(l, lockPage, start);
        Path png = Path.of(out, "locks.png");
        montage(frames, exp, png);
        System.out.println("locks: " + bad + " numeric CHECKs, " + unreachable + " states not reachable; judge " + png + " by eye");
        if (clean) deleteFrames();
    }

    /** Toggles the locks back to the state they had at the start. */
    static void ensureState(Link l, int lockPage, int[] want) throws Exception {
        for (int attempt = 0; attempt < 3; attempt++) {
            int[] st = lockState(l);
            if (st == null || Arrays.equals(st, want)) return;
            for (int b = 0; b < 3; b++) if (st[b] != want[b]) { l.send("key " + lockPage + " " + b); l.readUntil("OK", 1500); Thread.sleep(500); }
            Thread.sleep(500);
        }
    }

    static Frame shot(Link l, String cmd, String name) throws Exception {
        l.send(cmd); l.readUntil("OK", 1500);
        Thread.sleep(1500);
        return frame(name);
    }

    static int verdictLine(String what, boolean ok) { System.out.println((ok ? "PASS " : "FAIL ") + what); return ok ? 0 : 1; }

    static void layer(Link l, int n) throws Exception { l.send("layer " + n); Thread.sleep(1200); }

    static double mag(double[] v) { return Math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

    /** Chromaticity comparison: remove the grey component of both colours and compare directions. */
    static String verdict(int[] exp, double[] glow) {
        if (exp == null) return "?";
        double em = (exp[0] + exp[1] + exp[2]) / 3.0, gm = (glow[0] + glow[1] + glow[2]) / 3.0;
        double[] e = {exp[0] - em, exp[1] - em, exp[2] - em}, g = {glow[0] - gm, glow[1] - gm, glow[2] - gm};
        if (mag(e) < 25 || mag(glow) < 6) return "dim/neutral";
        double cos = (e[0] * g[0] + e[1] * g[1] + e[2] * g[2]) / (mag(e) * mag(g) + 1e-9);
        return cos > 0.7 ? "ok" : String.format("CHECK cos=%.2f", cos);
    }

    /** Contact sheet: every layer's frame (cropped) with a swatch of the expected colour. */
    static void montage(List<Frame> frames, List<Expect> exp, Path png) throws IOException {
        int tw = 240, th = 150, cols = 6, rows = (frames.size() + cols - 1) / cols, lab = 18;
        BufferedImage m = new BufferedImage(cols * tw, rows * (th + lab), BufferedImage.TYPE_INT_RGB);
        Graphics2D g = m.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
        g.setColor(new Color(20, 20, 20)); g.fillRect(0, 0, m.getWidth(), m.getHeight());
        for (int i = 0; i < frames.size(); i++) {
            BufferedImage im = frames.get(i).img;
            int[] c = crop != null ? crop : new int[]{0, 0, im.getWidth(), im.getHeight()};
            BufferedImage sub = im.getSubimage(c[0], c[1], c[2] - c[0], c[3] - c[1]);
            int x = (i % cols) * tw, y = (i / cols) * (th + lab);
            g.drawImage(sub, x, y + lab, tw, th, null);
            int[] rgb = exp.get(i).rgb != null ? exp.get(i).rgb : new int[]{128, 128, 128};
            g.setColor(new Color(rgb[0], rgb[1], rgb[2])); g.fillRect(x + 2, y + 2, 14, 14);
            g.setColor(Color.WHITE); g.drawString("L" + i + " " + exp.get(i).name, x + 22, y + 14);
        }
        g.dispose();
        ImageIO.write(m, "png", png.toFile());
    }

    // ------------------------------------------------------------------ helpers
    static int[] round(double[] v) { return Arrays.stream(v).mapToInt(x -> (int) Math.round(x)).toArray(); }

    static void deleteFrames() throws IOException {
        try (var s = Files.list(Path.of(out))) {
            for (Path p : (Iterable<Path>) s.filter(p -> p.toString().endsWith(".jpg"))::iterator) Files.delete(p);
        }
    }
}
