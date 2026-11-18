///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
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
 * Films the LED bars of the Nachtgespenst boards with the laptop webcam (ffmpeg + v4l2) and drives the boards over serial.
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

    public static void main(String[] args) throws Exception {
        if (args.length == 0 || !(args[0].equals("pins") || args[0].equals("colours"))) {
            System.err.println("usage: LedCam.java pins|colours --port P --out DIR [--camera DEV] [--size WxH] [--crop x0,y0,x1,y1] [--clean]");
            System.exit(2);
        }
        for (int i = 1; i < args.length; i++) switch (args[i]) {
            case "--port" -> port = args[++i];
            case "--out" -> out = args[++i];
            case "--camera" -> camera = args[++i];
            case "--size" -> size = args[++i];
            case "--crop" -> crop = Arrays.stream(args[++i].split(",")).mapToInt(Integer::parseInt).toArray();
            case "--clean" -> clean = true;
            default -> { System.err.println("unknown option " + args[i]); System.exit(2); }
        }
        if (port == null) { System.err.println("--port is required"); System.exit(2); }
        Files.createDirectories(Path.of(out));
        try (Link link = new Link(port)) {
            if (args[0].equals("pins")) pins(link); else colours(link);
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

    /** One webcam frame; 25 frames are read and the last kept, so the auto exposure can settle. */
    static Frame frame(String name) throws Exception {
        Path path = Path.of(out, name);
        Process pr = new ProcessBuilder("ffmpeg", "-loglevel", "error", "-y", "-f", "v4l2", "-video_size", size, "-i", camera,
                "-frames:v", "25", "-update", "1", path.toString()).inheritIO().start();
        if (pr.waitFor() != 0) throw new IOException("ffmpeg failed");
        BufferedImage img = ImageIO.read(path.toFile());
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
        l.send("INFO");
        int pages = 0;
        for (String line : l.readUntil("OK INFO", 3000)) { Matcher m = Pattern.compile("pages=(\\d+)").matcher(line); if (m.find()) pages = Integer.parseInt(m.group(1)); }
        if (pages == 0) throw new IllegalStateException("board did not answer INFO");
        List<Expect> exp = expectations();
        layer(l, pages - 1);                                      // Locks page, nothing active = faint white = dark baseline
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

    static void layer(Link l, int n) throws Exception { l.send("LAYER " + n); Thread.sleep(1200); }

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
