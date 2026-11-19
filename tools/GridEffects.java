import java.awt.*;
import java.awt.image.BufferedImage;
import java.util.*;

/** C64-style effects on the lamp grid: raster bars, a sine scroller and a dissolve between two pictures with a prime modulus. */
final class GridEffects {
    /** The 16 colours of the C64 (Pepto's palette). */
    static final int[] C64 = {0x000000, 0xFFFFFF, 0x68372B, 0x70A4B2, 0x6F3D86, 0x588D43, 0x352879, 0xB8C76F,
                              0x6F4F25, 0x433900, 0x9A6759, 0x444444, 0x6C6C6C, 0x9AD284, 0x6C5EB5, 0x959595};
    static final int[][] RAMPS = {{6, 14, 3, 1, 3, 14, 6}, {2, 10, 7, 1, 7, 10, 2}, {5, 13, 7, 1, 7, 13, 5}, {4, 14, 15, 1, 15, 14, 4}};

    /** Copper bars: four coloured bars of seven rows that move over the screen with sines. A row is one colour, so a bar is one range update per row. */
    static void rasterBars(Canvas c, double seconds) throws InterruptedException {
        long t0 = System.nanoTime();
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            double t = (System.nanoTime() - t0) / 1e9;
            int[] row = new int[c.rows]; Arrays.fill(row, C64[0]);
            for (int k = 0; k < RAMPS.length; k++) {
                double centre = c.rows / 2.0 + (c.rows / 2.0 - 4) * Math.sin(t * (0.8 + 0.37 * k) + k * 1.9);
                int[] ramp = RAMPS[k];
                for (int j = 0; j < ramp.length; j++) { int y = (int) Math.round(centre) - ramp.length / 2 + j; if (y >= 0 && y < c.rows) row[y] = C64[ramp[j]]; }
            }
            for (int y = 0; y < c.rows; y++) for (int x = 0; x < c.cols; x++) c.set(x, y, row[y]);
            c.present(); Thread.sleep(8);
        }
    }

    /** The text as a bit mask (one row of booleans per pixel row), rendered with a bold monospaced font of 9 pixels. */
    static boolean[][] textMask(String text) {
        Font f = new Font(Font.MONOSPACED, Font.BOLD, 11);
        BufferedImage probe = new BufferedImage(1, 1, BufferedImage.TYPE_INT_RGB);
        FontMetrics fm = probe.createGraphics().getFontMetrics(f);
        int w = fm.stringWidth(text) + 2, h = fm.getHeight() + 2;
        BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);
        Graphics2D g = img.createGraphics();
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);
        g.setColor(Color.WHITE); g.setFont(f); g.drawString(text, 1, fm.getAscent());
        boolean[][] m = new boolean[h][w];
        for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) m[y][x] = (img.getRGB(x, y) & 0xFF) > 128;
        return m;
    }

    /** A scrolling text whose columns ride on a sine, in C64 colours that cycle. */
    static void sineScroller(Canvas c, String text, double seconds) throws InterruptedException {
        boolean[][] m = textMask("      " + text + "      ");
        int h = m.length, w = m[0].length;
        long t0 = System.nanoTime();
        int[] cycle = {6, 14, 3, 1, 3, 14};
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            double t = (System.nanoTime() - t0) / 1e9;
            c.fill(C64[0]);
            int x0 = (int) (t * 22) % w;
            for (int x = 0; x < c.cols; x++) {
                int sx = (x0 + x) % w;
                int dy = (int) Math.round(Math.sin(x * 0.28 + t * 3.1) * (c.rows / 2.0 - h / 2.0 - 1));
                int base = c.rows / 2 - h / 2 + dy;
                for (int j = 0; j < h; j++) if (m[j][sx]) c.set(x, base + j, C64[cycle[((x / 3) + (int) (t * 9) + j / 3) % cycle.length]]);
            }
            c.present(); Thread.sleep(8);
        }
    }

    static boolean isPrime(long n) { if (n < 2) return false; for (long d = 2; d * d <= n; d++) if (n % d == 0) return false; return true; }
    static long nextPrime(long n) { while (!isPrime(n)) n++; return n; }
    static long nearestPrime(long n) { for (long d = 0; ; d++) { if (isPrime(n + d)) return n + d; if (n - d > 1 && isPrime(n - d)) return n - d; } }

    /**
     * Dissolve from picture a to picture b. Lamp i gets the rank (i * p) mod m, with m the first prime above the number of lamps and p another
     * prime: because p and m have no common divisor the ranks are all different, a permutation of the lamps without a table. A threshold
     * climbs from 0 to m and a lamp switches when its rank falls below it. A small p switches neighbours together (stripes and a moire), a p
     * near 0.618 m scatters evenly. Returns the seconds the dissolve took on the link.
     */
    static double dissolve(Canvas c, int[] a, int[] b, long p, int lampsPerStep) throws InterruptedException {
        int n = c.cols * c.rows; long m = nextPrime(n + 1);
        Integer[] order = new Integer[n]; long[] rank = new long[n];
        for (int i = 0; i < n; i++) { order[i] = i; rank[i] = ((long) i * p) % m; }
        Arrays.sort(order, Comparator.comparingLong(i -> rank[i]));
        c.setAll(a); c.present();
        long t0 = System.nanoTime(); int done = 0;
        while (done < n) {
            if (Thread.currentThread().isInterrupted()) throw new InterruptedException();
            int upto = Math.min(n, done + lampsPerStep);
            for (int k = done; k < upto; k++) { int i = order[k]; c.next[i] = b[i]; }
            done = upto;
            c.present();
        }
        return (System.nanoTime() - t0) / 1e9;
    }
}
