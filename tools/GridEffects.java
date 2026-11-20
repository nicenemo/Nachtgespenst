import java.awt.Color;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.util.Arrays;

/**
 * Commodore 64-inspired retro visual effects for the 2D lamp matrix display.
 * <p>
 * Includes:
 * <ul>
 *     <li><b>Copper/Raster bars:</b> Four undulating gradient bars sweeping the display with sines.</li>
 *     <li><b>Sine wave scroller:</b> Monospaced text riding a sine wave, shaded in cycling C64 colours.</li>
 *     <li><b>Prime modulus dissolve:</b> Pseudo-random transition between two images using coprime
 *         congruence ranking, creating moir&eacute; or evenly scattered pixel dissolution without
 *         a precomputed lookup table.</li>
 * </ul>
 * </p>
 */
final class GridEffects {

    /** Standard 16 colours of the Commodore 64 palette (Pepto's palette). */
    public static final int[] C64 = {
            0x000000, 0xFFFFFF, 0x68372B, 0x70A4B2,
            0x6F3D86, 0x588D43, 0x352879, 0xB8C76F,
            0x6F4F25, 0x433900, 0x9A6759, 0x444444,
            0x6C6C6C, 0x9AD284, 0x6C5EB5, 0x959595
    };

    /** Four symmetric colour gradient ramps referencing palette indices in {@link #C64}. */
    private static final int[][] RAMPS = {
            {6, 14, 3, 1, 3, 14, 6},
            {2, 10, 7, 1, 7, 10, 2},
            {5, 13, 7, 1, 7, 13, 5},
            {4, 14, 15, 1, 15, 14, 4}
    };

    private GridEffects() {
        // Non-instantiable utility class
    }

    /**
     * Renders four horizontal copper bars undulating vertically along independent sine waves.
     *
     * @param canvas  the target canvas
     * @param seconds total playback duration in seconds
     * @throws InterruptedException if playback is cancelled
     */
    static void rasterBars(Canvas canvas, double seconds) throws InterruptedException {
        long startTime = System.nanoTime();
        int rows = canvas.rows;
        int cols = canvas.cols;
        int[] rowColors = new int[rows];

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            double elapsed = (System.nanoTime() - startTime) / 1e9;
            Arrays.fill(rowColors, C64[0]);

            for (int k = 0; k < RAMPS.length; k++) {
                double centre = rows / 2.0 + (rows / 2.0 - 4) * Math.sin(elapsed * (0.8 + 0.37 * k) + k * 1.9);
                int[] ramp = RAMPS[k];
                int rampRadius = ramp.length / 2;

                for (int j = 0; j < ramp.length; j++) {
                    int y = (int) Math.round(centre) - rampRadius + j;
                    if (y >= 0 && y < rows) {
                        rowColors[y] = C64[ramp[j]];
                    }
                }
            }

            for (int y = 0; y < rows; y++) {
                int c = rowColors[y];
                for (int x = 0; x < cols; x++) {
                    canvas.set(x, y, c);
                }
            }

            canvas.present();
            Thread.sleep(8);
        }
    }

    /**
     * Renders an ASCII text string into a 2D boolean pixel mask using a bold monospaced font.
     *
     * @param text the text string to render
     * @return 2D boolean array of [height][width] where {@code true} represents lit pixels
     */
    static boolean[][] textMask(String text) {
        Font font = new Font(Font.MONOSPACED, Font.BOLD, 11);
        BufferedImage probe = new BufferedImage(1, 1, BufferedImage.TYPE_INT_RGB);
        FontMetrics fm = probe.createGraphics().getFontMetrics(font);

        int width = fm.stringWidth(text) + 2;
        int height = fm.getHeight() + 2;
        BufferedImage img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);

        Graphics2D g = img.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);
        g.setColor(Color.WHITE);
        g.setFont(font);
        g.drawString(text, 1, fm.getAscent());
        g.dispose();

        boolean[][] mask = new boolean[height][width];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                mask[y][x] = (img.getRGB(x, y) & 0xFF) > 128;
            }
        }
        return mask;
    }

    /**
     * Renders a smoothly scrolling banner whose columns ride along a sinusoidal curve.
     *
     * @param canvas  the target canvas
     * @param text    the text message to scroll
     * @param seconds total playback duration in seconds
     * @throws InterruptedException if playback is cancelled
     */
    static void sineScroller(Canvas canvas, String text, double seconds) throws InterruptedException {
        boolean[][] mask = textMask("      " + text + "      ");
        int maskH = mask.length;
        int maskW = mask[0].length;
        long startTime = System.nanoTime();
        int[] colorCycle = {6, 14, 3, 1, 3, 14};

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            double elapsed = (System.nanoTime() - startTime) / 1e9;
            canvas.fill(C64[0]);

            int x0 = (int) (elapsed * 22) % maskW;
            double verticalAmplitude = canvas.rows / 2.0 - maskH / 2.0 - 1;

            for (int x = 0; x < canvas.cols; x++) {
                int sourceX = (x0 + x) % maskW;
                int dy = (int) Math.round(Math.sin(x * 0.28 + elapsed * 3.1) * verticalAmplitude);
                int baseRow = canvas.rows / 2 - maskH / 2 + dy;

                for (int j = 0; j < maskH; j++) {
                    if (mask[j][sourceX]) {
                        int colorIndex = colorCycle[((x / 3) + (int) (elapsed * 9) + j / 3) % colorCycle.length];
                        canvas.set(x, baseRow + j, C64[colorIndex]);
                    }
                }
            }

            canvas.present();
            Thread.sleep(8);
        }
    }

    /** Tests whether {@code n} is prime. */
    static boolean isPrime(long n) {
        if (n < 2) return false;
        for (long d = 2; d * d <= n; d++) {
            if (n % d == 0) return false;
        }
        return true;
    }

    /** Finds the smallest prime &ge; {@code n}. */
    static long nextPrime(long n) {
        long p = n;
        while (!isPrime(p)) p++;
        return p;
    }

    /** Finds the prime closest to {@code n}. */
    static long nearestPrime(long n) {
        for (long d = 0; ; d++) {
            if (isPrime(n + d)) return n + d;
            if (n - d > 1 && isPrime(n - d)) return n - d;
        }
    }

    /**
     * Dissolves image {@code a} into image {@code b} using a mathematical pseudo-random permutation.
     * <p>
     * Each lamp index {@code i} is assigned a rank {@code (i * p) % m}, where {@code m} is the first
     * prime above the total lamp count, and {@code p} is a chosen coprime factor. Because {@code gcd(p, m) == 1},
     * the transformation produces a full permutation without duplication. A threshold rises from 0 to {@code m},
     * switching pixels as their rank is surpassed.
     * </p>
     *
     * @param canvas       the target canvas
     * @param a            initial pixel array (length {@code cols * rows})
     * @param b            target pixel array (length {@code cols * rows})
     * @param p            coprime factor (values near {@code 0.618 * m} yield optimal scattering)
     * @param lampsPerStep number of lamps transitioned per frame
     * @return duration in seconds taken by the transition
     * @throws InterruptedException if cancelled
     */
    static double dissolve(Canvas canvas, int[] a, int[] b, long p, int lampsPerStep) throws InterruptedException {
        int n = canvas.cols * canvas.rows;
        long m = nextPrime(n + 1);

        int[] order = new int[n];
        long[] rank = new long[n];
        for (int i = 0; i < n; i++) {
            order[i] = i;
            rank[i] = ((long) i * p) % m;
        }

        // Fast zero-allocation index sort based on primitive rank array
        sortIndices(order, rank, 0, n - 1);

        canvas.setAll(a);
        canvas.present();

        long startTime = System.nanoTime();
        int done = 0;

        while (done < n) {
            if (Thread.currentThread().isInterrupted()) {
                throw new InterruptedException();
            }

            int upto = Math.min(n, done + lampsPerStep);
            for (int k = done; k < upto; k++) {
                int lampIdx = order[k];
                canvas.next[lampIdx] = b[lampIdx];
            }
            done = upto;
            canvas.present();
        }

        return (System.nanoTime() - startTime) / 1e9;
    }

    /** In-place dual-pivot or quicksort for primitive integer indices ordered by a secondary rank array. */
    private static void sortIndices(int[] order, long[] rank, int low, int high) {
        if (low < high) {
            int pIndex = partition(order, rank, low, high);
            sortIndices(order, rank, low, pIndex - 1);
            sortIndices(order, rank, pIndex + 1, high);
        }
    }

    private static int partition(int[] order, long[] rank, int low, int high) {
        long pivot = rank[order[high]];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (rank[order[j]] <= pivot) {
                i++;
                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
        int temp = order[i + 1];
        order[i + 1] = order[high];
        order[high] = temp;
        return i + 1;
    }
}
