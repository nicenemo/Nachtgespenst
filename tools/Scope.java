import java.util.*;

/**
 * An oscilloscope on the lamp grid. It gets the last microphone buffer of the board (512 samples at 16 kHz), finds a rising zero crossing to trigger
 * on so that the trace stands still, picks the time base so that about three periods fit, scales the amplitude automatically, lets the trace
 * glow and fade like phosphor, and draws a graticule of 8 x 8 divisions with the frequency and the time base in a tiny 3 x 5 font.
 */
final class Scope {
    static final int RATE = 16000;
    final int cols, rows;
    final float[] glow;
    final int[] pix;
    double lastFreq = 0, lastSpanMs = 0; int lastPeak = 0;

    Scope(int cols, int rows) { this.cols = cols; this.rows = rows; glow = new float[cols * rows]; pix = new int[cols * rows]; }

    /** Draws one frame for the samples into the canvas picture (call Canvas.present() afterwards). */
    void frame(Canvas c, int[] v) {
        int n = v.length, peak = 1;
        for (int s : v) peak = Math.max(peak, Math.abs(s));
        // rising zero crossings that lead to a real swing (not the noise around zero)
        List<Integer> cross = new ArrayList<>();
        for (int i = 1; i < n - 8; i++) {
            if (v[i - 1] < 0 && v[i] >= 0) {
                int hi = 0; for (int k = i; k < Math.min(n, i + 10); k++) hi = Math.max(hi, v[k]);
                if (hi > Math.max(40, peak / 4)) cross.add(i);
            }
        }
        double freq = 0;
        if (cross.size() >= 2) freq = (double) RATE * (cross.size() - 1) / (cross.get(cross.size() - 1) - cross.get(0));
        int span = freq > 0 ? (int) Math.max(48, Math.min(300, 3.0 * RATE / freq)) : 200;
        int trig = 0;
        for (int t : cross) if (t + span < n) { trig = t; break; }
        if (trig + span >= n) span = n - trig - 1;
        lastFreq = freq; lastSpanMs = span * 1000.0 / RATE; lastPeak = peak;
        int seen = 1; for (int i = trig; i <= trig + span && i < n; i++) seen = Math.max(seen, Math.abs(v[i]));
        double gain = rows * 0.40 / Math.max(80, seen);                            // quiet noise stays small, a tone fills the screen

        for (int i = 0; i < glow.length; i++) glow[i] *= 0.62f;                     // the phosphor fades
        int prev = -1;
        for (int x = 0; x < cols; x++) {
            double pos = trig + x * (double) (span - 1) / (cols - 1);
            int i0 = (int) pos; double f = pos - i0; int i1 = Math.min(n - 1, i0 + 1);
            double val = v[i0] * (1 - f) + v[i1] * f;
            int y = (int) Math.round(rows / 2.0 - val * gain);
            y = Math.max(0, Math.min(rows - 1, y));
            int a = prev < 0 ? y : prev;
            for (int yy = Math.min(a, y); yy <= Math.max(a, y); yy++) glow[yy * cols + x] = 1f;   // join the points with a vertical run
            prev = y;
        }
        for (int y = 0; y < rows; y++) for (int x = 0; x < cols; x++) {
            int bg = graticule(x, y), g = (int) (Math.min(1f, glow[y * cols + x]) * 255);
            pix[y * cols + x] = g > 8 ? trace(glow[y * cols + x]) : bg;
        }
        String top = freq > 0 ? String.format("%dHZ", Math.round(freq)) : "NO TONE";
        text(0, 1, top, 0x66FF88);
        String right = String.format("%.1fMS", lastSpanMs);
        text(cols - 4 * right.length() - 1, 1, right, 0x4DAA66);
        if (c != null) c.setAll(pix);
    }

    /** 8 x 8 divisions: dotted lines, brighter axes with a tick every four lamps. */
    int graticule(int x, int y) {
        int cx = cols / 2, cy = rows / 2;
        if (y == cy) return (x % 4 == 0) ? 0x3C6B3C : 0x2A4A2A;
        if (x == cx) return (y % 4 == 0) ? 0x3C6B3C : 0x2A4A2A;
        boolean vline = x % (cols / 8) == 0, hline = y % Math.max(1, rows / 8) == 0;
        if ((vline && y % 2 == 0) || (hline && x % 2 == 0)) return 0x142414;
        return 0x020502;
    }

    static int trace(float g) {
        g = Math.min(1f, g);
        int r = (int) (255 * Math.max(0, g - 0.75f) * 4), gr = (int) (255 * Math.min(1f, 0.25f + g * 0.9f)), b = (int) (255 * Math.max(0, g - 0.7f) * 2.5f);
        return (Math.min(255, r) << 16) | (Math.min(255, gr) << 8) | Math.min(255, b);
    }

    // ---- a 3 x 5 font for the readout
    static final Map<Character, String> FONT = new HashMap<>();
    static {
        String[] f = {"0:111101101101111", "1:010110010010111", "2:111001111100111", "3:111001111001111", "4:101101111001001", "5:111100111001111",
                      "6:111100111101111", "7:111001001001001", "8:111101111101111", "9:111101111001111", "H:101101111101101", "Z:111001010100111",
                      "M:101111111101101", "S:111100111001111", "N:111101101101101", "O:111101101101111", "T:111010010010010", "E:111100110100111",
                      "A:010101111101101", ".:000000000000010", "-:000000111000000", "K:101101110101101", "V:101101101101010", "P:111101111100100", " :000000000000000"};
        for (String e : f) FONT.put(e.charAt(0), e.substring(2));
    }
    void text(int x0, int y0, String s, int rgb) {
        for (int ci = 0; ci < s.length(); ci++) {
            String g = FONT.get(s.charAt(ci)); if (g == null) continue;
            for (int k = 0; k < 15; k++) if (g.charAt(k) == '1') {
                int x = x0 + ci * 4 + k % 3, y = y0 + k / 3;
                if (x >= 0 && x < cols && y >= 0 && y < rows) pix[y * cols + x] = rgb;
            }
        }
    }
}
