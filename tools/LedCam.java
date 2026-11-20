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
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Automated camera-assisted LED optical calibration and diagnostic test harness.
 * <p>
 * Controls device hardware over serial commands while simultaneously filming the LED bar
 * via Video4Linux2 ({@link V4l2}) using the host laptop webcam.
 * </p>
 * <p>
 * Supported modes:
 * <ul>
 *     <li>{@code pins}: CoreS3 GPIO pin discovery by cycling pin excitation and detecting luminance changes.</li>
 *     <li>{@code colours}: Layer color verification cycling through all keymap layers and compiling a montage.</li>
 *     <li>{@code dark}: Verifies that {@code LEDS OFF} and {@code LEDS LOCKS} fully extinguish the bar.</li>
 *     <li>{@code locks}: Tests additive color mixing for Caps, Num, and Scroll lock states.</li>
 * </ul>
 * </p>
 * <p>
 * Acceleration: Image frame differencing is accelerated using the JDK Vector API
 * ({@code jdk.incubator.vector}) using SIMD lane parallelism.
 * </p>
 */
public class LedCam {

    static String port;
    static String outDir = "ledcam-out";
    static String cameraDevice = "/dev/video0";
    static String resolution = "1280x720";
    static int[] cropArea = null;
    static boolean cleanFrames = false;
    static boolean switchOtherBars = true;

    public static void main(String[] args) throws Exception {
        if (args.length == 0) {
            printUsageAndExit();
        }

        String command = args[0];
        if (!switch (command) {
            case "pins", "colours", "dark", "locks" -> true;
            default -> false;
        }) {
            printUsageAndExit();
        }

        for (int i = 1; i < args.length; i++) {
            switch (args[i]) {
                case "--port" -> port = args[++i];
                case "--out" -> outDir = args[++i];
                case "--camera" -> cameraDevice = args[++i];
                case "--size" -> resolution = args[++i];
                case "--crop" -> cropArea = Arrays.stream(args[++i].split(",")).mapToInt(Integer::parseInt).toArray();
                case "--clean" -> cleanFrames = true;
                case "--keep-others" -> switchOtherBars = false;
                default -> {
                    System.err.println("Unknown option: " + args[i]);
                    System.exit(2);
                }
            }
        }

        if (port == null) {
            System.err.println("--port is required");
            System.exit(2);
        }

        Files.createDirectories(Path.of(outDir));
        if (switchOtherBars) {
            setOtherBarsState(port, false);
        }

        try (SerialLink link = new SerialLink(port)) {
            switch (command) {
                case "pins" -> testPins(link);
                case "colours" -> testColours(link);
                case "dark" -> testDark(link);
                case "locks" -> testLocks(link);
            }
        } finally {
            if (switchOtherBars) {
                setOtherBarsState(port, true);
            }
        }
    }

    private static void printUsageAndExit() {
        System.err.println("usage: LedCam.java pins|colours|dark|locks --port P --out DIR [--camera DEV] [--size WxH] [--crop x0,y0,x1,y1] [--clean]");
        System.exit(2);
    }

    /**
     * Minimal synchronous serial link for diagnostic commands.
     */
    static class SerialLink implements AutoCloseable {
        final SerialPort port;

        SerialLink(String portName) {
            this.port = SerialPort.getCommPort(portName);
            port.setBaudRate(115200);
            port.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
            if (!port.openPort()) {
                throw new IllegalStateException("Cannot open serial port " + portName);
            }
        }

        void send(String cmd) {
            byte[] bytes = (cmd + "\n").getBytes(StandardCharsets.US_ASCII);
            port.writeBytes(bytes, bytes.length);
        }

        List<String> readUntil(String prefix, long timeoutMs) throws InterruptedException {
            List<String> lines = new ArrayList<>();
            StringBuilder sb = new StringBuilder();
            long deadline = System.currentTimeMillis() + timeoutMs;
            byte[] buf = new byte[256];

            while (System.currentTimeMillis() < deadline) {
                int count = port.bytesAvailable() > 0
                        ? port.readBytes(buf, Math.min(buf.length, port.bytesAvailable()))
                        : 0;
                if (count <= 0) {
                    Thread.sleep(10);
                    continue;
                }

                for (int i = 0; i < count; i++) {
                    char c = (char) (buf[i] & 0xFF);
                    if (c == '\n') {
                        String line = sb.toString().trim();
                        sb.setLength(0);
                        lines.add(line);
                        if (line.startsWith(prefix)) {
                            return lines;
                        }
                    } else if (c != '\r') {
                        sb.append(c);
                    }
                }
            }
            return lines;
        }

        @Override
        public void close() {
            port.closePort();
        }
    }

    private static void setOtherBarsState(String activePort, boolean enable) {
        String cmd = enable ? "leds on" : "leds off";
        for (SerialPort p : SerialPort.getCommPorts()) {
            if (p.getSystemPortPath().equals(activePort)) continue;
            try {
                if (p.openPort()) {
                    p.setBaudRate(115200);
                    p.writeBytes((cmd + "\n").getBytes(StandardCharsets.US_ASCII), cmd.length() + 1);
                    Thread.sleep(100);
                    p.closePort();
                }
            } catch (Exception ignored) {
                // Ignore ports that cannot be opened
            }
        }
    }

    // --- Frame Data Structures & Vector API SIMD Kernels ---

    /**
     * Planar float image representation separating R, G, and B into contiguous flat arrays.
     * Enables zero-overhead contiguous Vector API SIMD lane loading.
     */
    record Planes(int width, int height, float[] r, float[] g, float[] b) {
        static Planes of(BufferedImage img) {
            int w = img.getWidth();
            int h = img.getHeight();
            int[] pixels = img.getRGB(0, 0, w, h, null, 0, w);
            float[] r = new float[pixels.length];
            float[] g = new float[pixels.length];
            float[] b = new float[pixels.length];

            for (int i = 0; i < pixels.length; i++) {
                r[i] = (pixels[i] >> 16) & 0xFF;
                g[i] = (pixels[i] >> 8) & 0xFF;
                b[i] = pixels[i] & 0xFF;
            }
            return new Planes(w, h, r, g, b);
        }
    }

    record CapturedFrame(Path path, BufferedImage img, Planes planes) {}

    static CapturedFrame captureFrame(String name) throws Exception {
        Path filePath = Path.of(outDir, name);
        String[] dims = resolution.split("x");
        int targetW = Integer.parseInt(dims[0]);
        int targetH = Integer.parseInt(dims[1]);

        BufferedImage img = null;
        try (V4l2 cam = new V4l2(cameraDevice, targetW, targetH)) {
            // Read 25 frames to allow hardware auto-exposure and white balance to converge
            for (int count = 0, tries = 0; count < 25 && tries < 200; tries++) {
                BufferedImage next = cam.next(500, 0);
                if (next != null) {
                    img = next;
                    count++;
                }
            }
        }

        if (img == null) {
            throw new IOException("No picture captured from " + cameraDevice);
        }

        ImageIO.write(img, "png", filePath.toFile());
        return new CapturedFrame(filePath, img, Planes.of(img));
    }

    private static final VectorSpecies<Float> VECTOR_SPECIES = FloatVector.SPECIES_PREFERRED;

    /**
     * SIMD-accelerated absolute difference sum across R, G, B planes:
     * {@code out[i] = |a.r[i] - b.r[i]| + |a.g[i] - b.g[i]| + |a.b[i] - b.b[i]|}.
     */
    static float[] absDiffSum(Planes a, Planes b) {
        int length = a.r.length;
        float[] out = new float[length];
        int i = 0;
        int bound = VECTOR_SPECIES.loopBound(length);

        // Vectorized SIMD loop
        for (; i < bound; i += VECTOR_SPECIES.length()) {
            FloatVector diffR = FloatVector.fromArray(VECTOR_SPECIES, a.r, i)
                    .sub(FloatVector.fromArray(VECTOR_SPECIES, b.r, i)).abs();
            FloatVector diffG = FloatVector.fromArray(VECTOR_SPECIES, a.g, i)
                    .sub(FloatVector.fromArray(VECTOR_SPECIES, b.g, i)).abs();
            FloatVector diffB = FloatVector.fromArray(VECTOR_SPECIES, a.b, i)
                    .sub(FloatVector.fromArray(VECTOR_SPECIES, b.b, i)).abs();

            diffR.add(diffG).add(diffB).intoArray(out, i);
        }

        // Scalar tail loop
        for (; i < length; i++) {
            out[i] = Math.abs(a.r[i] - b.r[i])
                    + Math.abs(a.g[i] - b.g[i])
                    + Math.abs(a.b[i] - b.b[i]);
        }
        return out;
    }

    /**
     * Computes the mean signed difference {@code (a - b)} for each channel within a bounding ROI.
     */
    static double[] roiMeanDiff(Planes a, Planes b, int x0, int y0, int x1, int y1) {
        int minX = Math.clamp(x0, 0, a.width);
        int minY = Math.clamp(y0, 0, a.height);
        int maxX = Math.clamp(x1, 0, a.width);
        int maxY = Math.clamp(y1, 0, a.height);

        double[] channelSums = new double[3];

        for (int y = minY; y < maxY; y++) {
            int rowOffset = y * a.width;
            int idx = rowOffset + minX;
            int endIdx = rowOffset + maxX;
            int bound = idx + VECTOR_SPECIES.loopBound(maxX - minX);

            FloatVector sumR = FloatVector.zero(VECTOR_SPECIES);
            FloatVector sumG = FloatVector.zero(VECTOR_SPECIES);
            FloatVector sumB = FloatVector.zero(VECTOR_SPECIES);

            for (; idx < bound; idx += VECTOR_SPECIES.length()) {
                sumR = sumR.add(FloatVector.fromArray(VECTOR_SPECIES, a.r, idx)
                        .sub(FloatVector.fromArray(VECTOR_SPECIES, b.r, idx)));
                sumG = sumG.add(FloatVector.fromArray(VECTOR_SPECIES, a.g, idx)
                        .sub(FloatVector.fromArray(VECTOR_SPECIES, b.g, idx)));
                sumB = sumB.add(FloatVector.fromArray(VECTOR_SPECIES, a.b, idx)
                        .sub(FloatVector.fromArray(VECTOR_SPECIES, b.b, idx)));
            }

            channelSums[0] += sumR.reduceLanes(VectorOperators.ADD);
            channelSums[1] += sumG.reduceLanes(VectorOperators.ADD);
            channelSums[2] += sumB.reduceLanes(VectorOperators.ADD);

            for (; idx < endIdx; idx++) {
                channelSums[0] += a.r[idx] - b.r[idx];
                channelSums[1] += a.g[idx] - b.g[idx];
                channelSums[2] += a.b[idx] - b.b[idx];
            }
        }

        double pixelCount = Math.max(1, (double) (maxX - minX) * (maxY - minY));
        for (int c = 0; c < 3; c++) {
            channelSums[c] /= pixelCount;
        }
        return channelSums;
    }

    /** Mean of the {@code k} largest pixel differences. */
    static double topMean(float[] values, int k) {
        float[] sorted = values.clone();
        Arrays.sort(sorted);
        double sum = 0.0;
        for (int i = sorted.length - k; i < sorted.length; i++) {
            sum += sorted[i];
        }
        return sum / k;
    }

    record PinCandidate(int gpio, String label, double score, double[] rgb) {}

    static void testPins(SerialLink link) throws Exception {
        link.send("RESET");
        Thread.sleep(1500);

        List<Integer> gpios = new ArrayList<>();
        List<String> labels = new ArrayList<>();

        link.send("LIST");
        for (String line : link.readUntil("OK LIST", 4000)) {
            if (line.startsWith("CAND ")) {
                String[] parts = line.split(" ");
                gpios.add(Integer.parseInt(parts[1]));
                labels.add(parts[2]);
            }
        }

        if (gpios.isEmpty()) {
            throw new IllegalStateException("No pin candidates found: flash tools/ledtest cores3 pin finder first");
        }

        System.out.println("candidates: " + gpios.size());
        CapturedFrame baseline = captureFrame("base.jpg");
        List<PinCandidate> results = new ArrayList<>();

        for (int k = 0; k < gpios.size(); k++) {
            int gpio = gpios.get(k);
            link.send("PIN " + gpio);
            Thread.sleep(150);

            double bestScore = 0;
            double[] bestRgb = {0, 0, 0};

            for (int phase = 0; phase < 3; phase++) {
                Thread.sleep(450);
                CapturedFrame f = captureFrame(String.format("pin%02d_%d.jpg", gpio, phase));
                float[] diffs = absDiffSum(f.planes, baseline.planes);
                double score = topMean(diffs, 200);

                if (score > bestScore) {
                    bestScore = score;
                    bestRgb = litMean(f.planes, diffs);
                }
            }
            link.readUntil("OK PIN", 4000);
            Thread.sleep(300);

            results.add(new PinCandidate(gpio, labels.get(k), bestScore, bestRgb));
            System.out.printf("GPIO %2d %-8s score=%6.1f rgb=%s%n",
                    gpio, labels.get(k), bestScore, Arrays.toString(round(bestRgb)));
        }

        results.sort(Comparator.comparingDouble(PinCandidate::score).reversed());
        System.out.print("TOP:");
        for (int i = 0; i < Math.min(3, results.size()); i++) {
            PinCandidate top = results.get(i);
            System.out.printf(" GPIO %d (%s) %.1f;", top.gpio(), top.label(), top.score());
        }
        System.out.println();

        if (cleanFrames) {
            deleteFrames();
        }
    }

    static double[] litMean(Planes planes, float[] diffs) {
        double[] sums = new double[3];
        int count = 0;
        for (int i = 0; i < diffs.length; i++) {
            if (diffs[i] > 60) {
                sums[0] += planes.r[i];
                sums[1] += planes.g[i];
                sums[2] += planes.b[i];
                count++;
            }
        }
        if (count > 0) {
            for (int c = 0; c < 3; c++) {
                sums[c] /= count;
            }
        }
        return sums;
    }

    record LayerExpectation(String name, int[] rgb) {}

    static final Map<String, int[]> CSS_COLORS = new HashMap<>();
    static {
        String[] definitions = {
                "Blue 0 0 255", "Red 255 0 0", "Orange 255 165 0", "Green 0 128 0", "Purple 128 0 128", "Cyan 0 255 255",
                "Yellow 255 255 0", "White 255 255 255", "Magenta 255 0 255", "Lime 0 255 0", "Maroon 128 0 0", "Navy 0 0 128",
                "Olive 128 128 0", "Teal 0 128 128", "DeepPink 255 20 147", "Gold 255 215 0", "Indigo 75 0 130", "DarkGreen 0 100 0",
                "DarkOrange 255 140 0", "DarkViolet 148 0 211", "SkyBlue 135 206 235", "LightCoral 240 128 128", "PaleGreen 152 251 152",
                "Khaki 240 230 140", "Lavender 230 230 250", "MistyRose 255 228 225"
        };
        for (String def : definitions) {
            String[] p = def.split(" ");
            CSS_COLORS.put(p[0], new int[]{Integer.parseInt(p[1]), Integer.parseInt(p[2]), Integer.parseInt(p[3])});
        }
    }

    static List<LayerExpectation> loadExpectations() throws IOException {
        Path root = Files.exists(Path.of("include/Keymap.h")) ? Path.of(".") : Path.of("..");
        String coloursText = Files.readString(root.resolve("include/LedColors.h"));
        coloursText = coloursText.substring(coloursText.indexOf("LAYER_COLORS[]"));
        coloursText = coloursText.substring(0, coloursText.indexOf("};"));

        List<String> colorNames = new ArrayList<>();
        Matcher m = Pattern.compile("CRGB::(\\w+)").matcher(coloursText);
        while (m.find()) {
            colorNames.add(m.group(1));
        }

        List<LayerExpectation> expectations = new ArrayList<>();
        Matcher k = Pattern.compile("LAYER_COLORS\\[(\\d+)\\]").matcher(Files.readString(root.resolve("include/Keymap.h")));
        while (k.find()) {
            String name = colorNames.get(Integer.parseInt(k.group(1)));
            expectations.add(new LayerExpectation(name, CSS_COLORS.get(name)));
        }
        return expectations;
    }

    static void testColours(SerialLink link) throws Exception {
        link.send("info");
        int locksPage = -1;
        for (String line : link.readUntil("OK INFO", 3000)) {
            Matcher m = Pattern.compile("locks=(\\d+)").matcher(line);
            if (m.find()) {
                locksPage = Integer.parseInt(m.group(1));
            }
        }
        if (locksPage < 0) {
            throw new IllegalStateException("Board did not reply to INFO command");
        }

        List<LayerExpectation> expectations = loadExpectations();
        setLayer(link, locksPage);

        CapturedFrame baseline = captureFrame("base.jpg");
        int w = baseline.planes.width;
        int h = baseline.planes.height;
        int[][] rois = {
                {w / 5, h * 3 / 5, w * 3 / 5, h},     // Left device table glow
                {w * 3 / 5, h * 3 / 5, w, h}          // Right device table glow
        };
        String[] sideNames = {"left", "right"};

        List<CapturedFrame> frames = new ArrayList<>();
        int anomalies = 0;

        for (int layerIdx = 0; layerIdx < expectations.size(); layerIdx++) {
            setLayer(link, layerIdx);
            CapturedFrame frame = captureFrame(String.format("L%02d.jpg", layerIdx));
            frames.add(frame);

            double[][] diffs = new double[2][];
            for (int s = 0; s < 2; s++) {
                diffs[s] = roiMeanDiff(frame.planes, baseline.planes, rois[s][0], rois[s][1], rois[s][2], rois[s][3]);
            }

            int dominantSide = magnitude(diffs[0]) >= magnitude(diffs[1]) ? 0 : 1;
            LayerExpectation exp = expectations.get(layerIdx);
            String verdictText = evaluateColorMatch(exp.rgb(), diffs[dominantSide]);
            if (verdictText.startsWith("CHECK")) {
                anomalies++;
            }

            System.out.printf("L%02d %-12s exp=%s side=%-5s glow=%s mag=%5.1f %s%n",
                    layerIdx, exp.name(), Arrays.toString(exp.rgb()),
                    sideNames[dominantSide], Arrays.toString(round(diffs[dominantSide])),
                    magnitude(diffs[dominantSide]), verdictText);
        }

        setLayer(link, 0);
        Path montagePath = Path.of(outDir, "montage.png");
        renderMontage(frames, expectations, montagePath);
        System.out.println("numeric CHECKs: " + anomalies + " (weak signal; inspect " + montagePath + " by eye)");

        if (cleanFrames) {
            deleteFrames();
        }
    }

    static void testDark(SerialLink link) throws Exception {
        int failures = 0;
        link.send("set defaults 1");
        link.readUntil("OK", 1500);

        setLayer(link, 0);
        CapturedFrame on = captureCommandShot(link, "LEDS ON", "on.jpg");
        CapturedFrame off1 = captureCommandShot(link, "LEDS OFF", "off1.jpg");
        CapturedFrame off2 = captureCommandShot(link, "LEDS OFF", "off2.jpg");
        CapturedFrame locks = captureCommandShot(link, "LEDS LOCKS", "locks.jpg");
        CapturedFrame back = captureCommandShot(link, "LEDS ON", "back.jpg");

        int[] c = cropArea != null ? cropArea : new int[]{0, 0, on.planes.width, on.planes.height};
        double onOffMag = magnitude(roiMeanDiff(on.planes, off1.planes, c[0], c[1], c[2], c[3]));
        double noiseMag = magnitude(roiMeanDiff(off1.planes, off2.planes, c[0], c[1], c[2], c[3]));
        double locksVsOffMag = magnitude(roiMeanDiff(locks.planes, off1.planes, c[0], c[1], c[2], c[3]));
        double backVsOnMag = magnitude(roiMeanDiff(back.planes, on.planes, c[0], c[1], c[2], c[3]));

        System.out.printf("glow LEDS ON vs OFF: %.1f   OFF vs OFF (noise): %.1f   LEDS LOCKS vs OFF: %.1f   back ON vs ON: %.1f%n",
                onOffMag, noiseMag, locksVsOffMag, backVsOnMag);

        failures += printVerdict("LEDS OFF is clearly darker than LEDS ON", onOffMag > 6 && onOffMag > 4 * noiseMag);
        failures += printVerdict("two LEDS OFF frames look the same (the bar is really dark)", noiseMag < 3);

        link.send("state");
        boolean anyLockActive = false;
        for (String line : link.readUntil("OK STATE", 1500)) {
            Matcher m = Pattern.compile("caps=(\\d) num=(\\d) scroll=(\\d)").matcher(line);
            if (m.find()) {
                anyLockActive = !(m.group(1) + m.group(2) + m.group(3)).equals("000");
            }
        }

        failures += printVerdict(
                "LEDS LOCKS on a layer page " + (anyLockActive ? "shows active lock" : "is as dark as OFF"),
                anyLockActive ? locksVsOffMag > Math.max(6, noiseMag * 4) : locksVsOffMag < Math.max(3, noiseMag * 2)
        );
        failures += printVerdict("LEDS ON restores colors", backVsOnMag < onOffMag * 0.35);

        link.send("leds on");
        link.readUntil("OK", 1500);

        if (cleanFrames) {
            deleteFrames();
        }
        System.out.println(failures == 0 ? "dark: PASS" : "dark: " + failures + " FAIL");
        if (failures > 0) {
            System.exit(1);
        }
    }

    static int[] queryLockState(SerialLink link) throws Exception {
        link.send("state");
        for (String line : link.readUntil("OK STATE", 1500)) {
            Matcher m = Pattern.compile("caps=(\\d) num=(\\d) scroll=(\\d)").matcher(line);
            if (m.find()) {
                return new int[]{
                        Integer.parseInt(m.group(1)),
                        Integer.parseInt(m.group(2)),
                        Integer.parseInt(m.group(3))
                };
            }
        }
        return null;
    }

    static void testLocks(SerialLink link) throws Exception {
        link.send("info");
        int lockPage = -1;
        for (String line : link.readUntil("OK INFO", 3000)) {
            Matcher m = Pattern.compile("locks=(\\d+)").matcher(line);
            if (m.find()) {
                lockPage = Integer.parseInt(m.group(1));
            }
        }
        if (lockPage < 0) {
            throw new IllegalStateException("Board did not answer INFO command");
        }

        setLayer(link, lockPage);
        int[] initialLocks = queryLockState(link);
        if (initialLocks == null) {
            throw new IllegalStateException("Failed to query initial lock state");
        }

        int[][] combinations = {
                {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1},
                {1, 1, 0}, {0, 1, 1}, {1, 1, 1}
        };
        String[] comboNames = {"none", "caps", "num", "scroll", "caps+num", "num+scroll", "all three"};
        int[][] lockColorValues = {{255, 80, 0}, {0, 220, 60}, {40, 80, 255}};

        List<CapturedFrame> frames = new ArrayList<>();
        List<LayerExpectation> expectations = new ArrayList<>();
        CapturedFrame baseline = null;
        int checkFailures = 0;
        int unreachableCount = 0;

        for (int k = 0; k < combinations.length; k++) {
            int[] targetState = combinations[k];
            for (int attempt = 0; attempt < 3; attempt++) {
                int[] current = queryLockState(link);
                if (current == null || Arrays.equals(current, targetState)) break;
                for (int b = 0; b < 3; b++) {
                    if (current[b] != targetState[b]) {
                        link.send("key " + lockPage + " " + b);
                        link.readUntil("OK", 1500);
                        Thread.sleep(500);
                    }
                }
                Thread.sleep(600);
            }

            int[] achieved = queryLockState(link);
            if (achieved == null || !Arrays.equals(achieved, targetState)) {
                System.out.printf("%-10s state %s could not be reached (board shows %s): skipped%n",
                        comboNames[k], Arrays.toString(targetState), Arrays.toString(achieved));
                unreachableCount++;
                continue;
            }

            Thread.sleep(700);
            CapturedFrame frame = captureFrame("lock" + k + ".jpg");

            int[] expectedRgb = {0, 0, 0};
            for (int b = 0; b < 3; b++) {
                if (targetState[b] == 1) {
                    for (int c = 0; c < 3; c++) {
                        expectedRgb[c] = Math.min(255, expectedRgb[c] + lockColorValues[b][c]);
                    }
                }
            }

            if (k == 0) {
                baseline = frame;
                System.out.println("baseline (no lock active): captured");
                frames.add(frame);
                expectations.add(new LayerExpectation(comboNames[k], new int[]{128, 128, 128}));
                continue;
            }

            int w = frame.planes.width;
            int h = frame.planes.height;
            int[][] rois = {{w / 5, h * 3 / 5, w * 3 / 5, h}, {w * 3 / 5, h * 3 / 5, w, h}};
            double[] m0 = roiMeanDiff(frame.planes, baseline.planes, rois[0][0], rois[0][1], rois[0][2], rois[0][3]);
            double[] m1 = roiMeanDiff(frame.planes, baseline.planes, rois[1][0], rois[1][1], rois[1][2], rois[1][3]);
            double[] m = magnitude(m0) >= magnitude(m1) ? m0 : m1;

            String verdict = evaluateColorMatch(expectedRgb, m);
            if (verdict.startsWith("CHECK")) {
                checkFailures++;
            }
            System.out.printf("%-10s exp=%s glow=%s mag=%5.1f %s%n",
                    comboNames[k], Arrays.toString(expectedRgb), Arrays.toString(round(m)), magnitude(m), verdict);
            frames.add(frame);
            expectations.add(new LayerExpectation(comboNames[k], expectedRgb));
        }

        restoreLockState(link, lockPage, initialLocks);
        Path pngPath = Path.of(outDir, "locks.png");
        renderMontage(frames, expectations, pngPath);
        System.out.println("locks: " + checkFailures + " numeric CHECKs, " + unreachableCount + " unreachable; inspect " + pngPath + " by eye");

        if (cleanFrames) {
            deleteFrames();
        }
    }

    static void restoreLockState(SerialLink link, int lockPage, int[] target) throws Exception {
        for (int attempt = 0; attempt < 3; attempt++) {
            int[] current = queryLockState(link);
            if (current == null || Arrays.equals(current, target)) return;
            for (int b = 0; b < 3; b++) {
                if (current[b] != target[b]) {
                    link.send("key " + lockPage + " " + b);
                    link.readUntil("OK", 1500);
                    Thread.sleep(500);
                }
            }
            Thread.sleep(500);
        }
    }

    static CapturedFrame captureCommandShot(SerialLink link, String cmd, String filename) throws Exception {
        link.send(cmd);
        link.readUntil("OK", 1500);
        Thread.sleep(1500);
        return captureFrame(filename);
    }

    static int printVerdict(String description, boolean conditionMet) {
        System.out.println((conditionMet ? "PASS " : "FAIL ") + description);
        return conditionMet ? 0 : 1;
    }

    static void setLayer(SerialLink link, int layerIndex) throws Exception {
        link.send("layer " + layerIndex);
        Thread.sleep(1200);
    }

    static double magnitude(double[] vector) {
        return Math.sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    }

    /**
     * Compares chromaticity direction between expected colour and camera-captured glow.
     */
    static String evaluateColorMatch(int[] expectedRgb, double[] capturedGlow) {
        if (expectedRgb == null) return "?";

        double expectedMean = (expectedRgb[0] + expectedRgb[1] + expectedRgb[2]) / 3.0;
        double glowMean = (capturedGlow[0] + capturedGlow[1] + capturedGlow[2]) / 3.0;

        double[] e = {expectedRgb[0] - expectedMean, expectedRgb[1] - expectedMean, expectedRgb[2] - expectedMean};
        double[] g = {capturedGlow[0] - glowMean, capturedGlow[1] - glowMean, capturedGlow[2] - glowMean};

        if (magnitude(e) < 25.0 || magnitude(capturedGlow) < 6.0) {
            return "dim/neutral";
        }

        double cosTheta = (e[0] * g[0] + e[1] * g[1] + e[2] * g[2]) / (magnitude(e) * magnitude(g) + 1e-9);
        return (cosTheta > 0.7) ? "ok" : String.format("CHECK cos=%.2f", cosTheta);
    }

    /**
     * Compiles a tiled contact sheet thumbnail montage of captured test frames.
     */
    static void renderMontage(List<CapturedFrame> frames, List<LayerExpectation> expectations, Path outputPng) throws IOException {
        int thumbW = 240;
        int thumbH = 150;
        int cols = 6;
        int rows = (frames.size() + cols - 1) / cols;
        int labelH = 18;

        BufferedImage sheet = new BufferedImage(cols * thumbW, rows * (thumbH + labelH), BufferedImage.TYPE_INT_RGB);
        Graphics2D g = sheet.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
        g.setColor(new Color(20, 20, 20));
        g.fillRect(0, 0, sheet.getWidth(), sheet.getHeight());

        for (int i = 0; i < frames.size(); i++) {
            BufferedImage img = frames.get(i).img();
            int[] crop = (cropArea != null) ? cropArea : new int[]{0, 0, img.getWidth(), img.getHeight()};
            BufferedImage cropped = img.getSubimage(crop[0], crop[1], crop[2] - crop[0], crop[3] - crop[1]);

            int cellX = (i % cols) * thumbW;
            int cellY = (i / cols) * (thumbH + labelH);

            g.drawImage(cropped, cellX, cellY + labelH, thumbW, thumbH, null);

            int[] swatchRgb = (expectations.get(i).rgb() != null) ? expectations.get(i).rgb() : new int[]{128, 128, 128};
            g.setColor(new Color(swatchRgb[0], swatchRgb[1], swatchRgb[2]));
            g.fillRect(cellX + 2, cellY + 2, 14, 14);

            g.setColor(Color.WHITE);
            g.drawString("L" + i + " " + expectations.get(i).name(), cellX + 22, cellY + 14);
        }

        g.dispose();
        ImageIO.write(sheet, "png", outputPng.toFile());
    }

    static int[] round(double[] vals) {
        return Arrays.stream(vals).mapToInt(x -> (int) Math.round(x)).toArray();
    }

    static void deleteFrames() throws IOException {
        try (var s = Files.list(Path.of(outDir))) {
            for (Path p : (Iterable<Path>) s.filter(p -> p.toString().endsWith(".jpg"))::iterator) {
                Files.delete(p);
            }
        }
    }
}
