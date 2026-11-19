/** Effects for the ten bar lamps (lamps 0 to 9), painted by the PC while the bar lamp page is shown. */
final class BarEffects {
    static void frame(Hid h, int[] rgb) {
        int[] ids = new int[8]; int[][] c = new int[8][4];
        for (int k = 0; k < 8; k++) { ids[k] = k; c[k] = new int[]{(rgb[k] >> 16) & 255, (rgb[k] >> 8) & 255, rgb[k] & 255, 255}; }
        h.write(Proto.multi(ids, c, 8, false));
        int[] ids2 = {8, 9, 0, 0, 0, 0, 0, 0}; int[][] c2 = new int[8][4];
        for (int k = 0; k < 2; k++) c2[k] = new int[]{(rgb[8 + k] >> 16) & 255, (rgb[8 + k] >> 8) & 255, rgb[8 + k] & 255, 255};
        h.write(Proto.multi(ids2, c2, 2, true));
    }
    static int scale(int c, double f) { f = Math.max(0, Math.min(1, f)); return ((int) (((c >> 16) & 255) * f) << 16) | ((int) (((c >> 8) & 255) * f) << 8) | (int) ((c & 255) * f); }
    static int hsv(double h, double s, double v) { return java.awt.Color.HSBtoRGB((float) (h - Math.floor(h)), (float) s, (float) v) & 0xFFFFFF; }

    /** Knight Rider: a red scanner that sweeps to and fro with a fading tail. */
    static void nightrider(Hid h, double seconds) throws InterruptedException {
        long t0 = System.nanoTime(); int[] px = new int[10];
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            double t = (System.nanoTime() - t0) / 1e9, p = 4.5 + 4.5 * Math.sin(t * 3.2);       // 0..9
            for (int i = 0; i < 10; i++) px[i] = scale(0xFF1000, Math.exp(-Math.abs(i - p) * 0.9));
            frame(h, px); Thread.sleep(16);
        }
    }
    /** Breath: all lamps swell and fade slowly, white-blue. */
    static void breath(Hid h, double seconds) throws InterruptedException {
        long t0 = System.nanoTime(); int[] px = new int[10];
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            double t = (System.nanoTime() - t0) / 1e9, f = (Math.sin(t * 1.6 - Math.PI / 2) + 1) / 2;
            java.util.Arrays.fill(px, scale(0x6090FF, 0.05 + 0.95 * f * f));
            frame(h, px); Thread.sleep(16);
        }
    }
    /** A rainbow that moves along the bar. */
    static void rainbow(Hid h, double seconds) throws InterruptedException {
        long t0 = System.nanoTime(); int[] px = new int[10];
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            double t = (System.nanoTime() - t0) / 1e9;
            for (int i = 0; i < 10; i++) px[i] = hsv(i / 10.0 + t * 0.3, 1, 1);
            frame(h, px); Thread.sleep(16);
        }
    }
    /** A comet with a long tail that runs from the left to the right and starts over. */
    static void comet(Hid h, double seconds) throws InterruptedException {
        long t0 = System.nanoTime(); int[] px = new int[10];
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            double t = (System.nanoTime() - t0) / 1e9, p = (t * 7) % 14 - 2;                    // the head runs from -2 to 12
            for (int i = 0; i < 10; i++) { double d = p - i; px[i] = d >= 0 ? scale(0x30FFD0, Math.exp(-d * 0.7)) : 0; }
            frame(h, px); Thread.sleep(16);
        }
    }
    /** Sparkle: random lamps flash and fade. */
    static void sparkle(Hid h, double seconds) throws InterruptedException {
        long t0 = System.nanoTime(); double[] v = new double[10]; java.util.Random r = new java.util.Random(3); int[] px = new int[10];
        while ((System.nanoTime() - t0) / 1e9 < seconds) {
            for (int i = 0; i < 10; i++) { v[i] *= 0.9; if (r.nextInt(14) == 0) v[i] = 1; px[i] = scale(0xFFD070, v[i]); }
            frame(h, px); Thread.sleep(16);
        }
    }
}
