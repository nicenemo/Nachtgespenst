import java.util.ArrayList;
import java.util.List;

/**
 * Virtual oscilloscope rendered onto the 2D LED matrix display.
 * <p>
 * Processes raw PCM microphone audio frames (512 samples at 16 kHz) acquired from the board,
 * computes triggering zero-crossings, applies dynamic gain scaling, renders phosphor decay persistence,
 * superimposes an 8&times;8 graticule, and stamps live frequency/timebase metrics using a 3&times;5 bitmapped font.
 * </p>
 */
final class Scope {

    /** Audio sampling rate of the board's microphone in Hz. */
    public static final int SAMPLE_RATE = 16000;

    final int cols;
    final int rows;

    /** Phosphor intensity persistence map [0.0f, 1.0f] per pixel. */
    final float[] glow;

    /** Current RGB pixel framebuffer (length {@code cols * rows}). */
    final int[] pix;

    double lastFreq = 0.0;
    double lastSpanMs = 0.0;
    int lastPeak = 0;

    /**
     * Initializes an oscilloscope canvas with the specified matrix dimensions.
     *
     * @param cols matrix columns
     * @param rows matrix rows
     */
    Scope(int cols, int rows) {
        this.cols = cols;
        this.rows = rows;
        this.glow = new float[cols * rows];
        this.pix = new int[cols * rows];
    }

    /**
     * Computes an oscilloscope frame from 16 kHz audio samples and updates the canvas.
     *
     * @param canvas the target canvas to update (or {@code null} to update internal buffer only)
     * @param samples array of raw 16-bit signed PCM audio samples
     */
    void frame(Canvas canvas, int[] samples) {
        int sampleCount = samples.length;
        int peak = 1;
        for (int s : samples) {
            peak = Math.max(peak, Math.abs(s));
        }

        // Locate rising zero crossings that exceed the noise threshold
        List<Integer> crossings = new ArrayList<>();
        int noiseThreshold = Math.max(40, peak / 4);

        for (int i = 1; i < sampleCount - 8; i++) {
            if (samples[i - 1] < 0 && samples[i] >= 0) {
                int localMax = 0;
                for (int k = i; k < Math.min(sampleCount, i + 10); k++) {
                    localMax = Math.max(localMax, samples[k]);
                }
                if (localMax > noiseThreshold) {
                    crossings.add(i);
                }
            }
        }

        // Calculate fundamental frequency from crossing intervals
        double freq = 0.0;
        if (crossings.size() >= 2) {
            int firstCross = crossings.getFirst();
            int lastCross = crossings.getLast();
            freq = (double) SAMPLE_RATE * (crossings.size() - 1) / (lastCross - firstCross);
        }

        // Determine span (display timebase) to fit approximately 3 full cycles
        int span = (freq > 0)
                ? (int) Math.clamp(3.0 * SAMPLE_RATE / freq, 48, 300)
                : 200;

        int triggerIndex = 0;
        for (int t : crossings) {
            if (t + span < sampleCount) {
                triggerIndex = t;
                break;
            }
        }
        if (triggerIndex + span >= sampleCount) {
            span = sampleCount - triggerIndex - 1;
        }

        lastFreq = freq;
        lastSpanMs = span * 1000.0 / SAMPLE_RATE;
        lastPeak = peak;

        // Auto-gain scaling
        int maxSeen = 1;
        for (int i = triggerIndex; i <= triggerIndex + span && i < sampleCount; i++) {
            maxSeen = Math.max(maxSeen, Math.abs(samples[i]));
        }
        double gain = (rows * 0.40) / Math.max(80, maxSeen);

        // Phosphor decay: fade existing trace
        for (int i = 0; i < glow.length; i++) {
            glow[i] *= 0.62f;
        }

        // Plot new trace with linear interpolation and vertical continuity
        int previousY = -1;
        for (int x = 0; x < cols; x++) {
            double samplePos = triggerIndex + x * (double) (span - 1) / (cols - 1);
            int idx0 = (int) samplePos;
            double frac = samplePos - idx0;
            int idx1 = Math.min(sampleCount - 1, idx0 + 1);

            double interpolatedVal = samples[idx0] * (1.0 - frac) + samples[idx1] * frac;
            int y = (int) Math.round(rows / 2.0 - interpolatedVal * gain);
            y = Math.clamp(y, 0, rows - 1);

            int startY = (previousY < 0) ? y : previousY;
            int minY = Math.min(startY, y);
            int maxY = Math.max(startY, y);
            for (int cy = minY; cy <= maxY; cy++) {
                glow[cy * cols + x] = 1.0f;
            }
            previousY = y;
        }

        // Composite phosphor glow over graticule background
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                int index = y * cols + x;
                float intensity = Math.min(1.0f, glow[index]);
                int bg = graticule(x, y);
                pix[index] = (intensity * 255.0f > 8) ? traceColor(intensity) : bg;
            }
        }

        // On-screen telemetry text overlay
        String topText = (freq > 0) ? String.format("%dHZ", Math.round(freq)) : "NO TONE";
        drawText(0, 1, topText, 0xFFCC99);

        String rightText = String.format("%.1fMS", lastSpanMs);
        drawText(cols - 4 * rightText.length() - 1, 1, rightText, 0x9999FF);

        if (canvas != null) {
            canvas.setAll(pix);
        }
    }

    /**
     * Computes the graticule background colour for coordinate (x, y).
     *
     * @param x column index
     * @param y row index
     * @return 24-bit 0xRRGGBB colour
     */
    int graticule(int x, int y) {
        int centreX = cols / 2;
        int centreY = rows / 2;

        if (y == centreY) {
            return (x % 4 == 0) ? 0x4A4A8C : 0x2E2E58; // Major horizontal axis
        }
        if (x == centreX) {
            return (y % 4 == 0) ? 0x4A4A8C : 0x2E2E58; // Major vertical axis
        }

        boolean vGrid = (x % Math.max(1, cols / 8) == 0);
        boolean hGrid = (y % Math.max(1, rows / 8) == 0);
        if ((vGrid && y % 2 == 0) || (hGrid && x % 2 == 0)) {
            return 0x16162C; // Minor dotted division lines
        }
        return 0x000000;
    }

    /**
     * Converts phosphor glow intensity into a multi-stage thermal/phosphor color ramp.
     * Transitions from dark amber through peach to an intense white-hot core.
     *
     * @param g normalized glow intensity [0.0, 1.0]
     * @return 24-bit 0xRRGGBB colour
     */
    static int traceColor(float g) {
        float intensity = Math.clamp(g, 0.0f, 1.0f);
        float core = Math.max(0.0f, (intensity - 0.55f) / 0.45f);
        float decay = (float) Math.sqrt(intensity);

        int r = (int) (255 * decay);
        int gr = (int) (153 * decay + (240 - 153) * core * decay);
        int b = (int) (232 * core * core * decay);

        return (r << 16) | (Math.min(255, gr) << 8) | Math.min(255, b);
    }

    // --- 3x5 font table encoded as 15-bit integers ---
    private static final short[] FONT_BITS = new short[128];
    static {
        String[] fontDefs = {
                "0:111101101101111", "1:010110010010111", "2:111001111100111", "3:111001111001111",
                "4:101101111001001", "5:111100111001111", "6:111100111101111", "7:111001001001001",
                "8:111101111101111", "9:111101111001111", "H:101101111101101", "Z:111001010100111",
                "M:101111111101101", "S:111100111001111", "N:111101101101101", "O:111101101101111",
                "T:111010010010010", "E:111100110100111", "A:010101111101101", ".:000000000000010",
                "-:000000111000000", "K:101101110101101", "V:101101101101010", "P:111101111100100",
                " :000000000000000"
        };
        for (String def : fontDefs) {
            char ch = def.charAt(0);
            String bits = def.substring(2);
            short val = 0;
            for (int k = 0; k < 15; k++) {
                if (bits.charAt(k) == '1') {
                    val |= (short) (1 << (14 - k));
                }
            }
            if (ch < FONT_BITS.length) {
                FONT_BITS[ch] = val;
            }
        }
    }

    /**
     * Renders a text string into the internal pixel framebuffer using the compact 3&times;5 font.
     *
     * @param x0  origin column
     * @param y0  origin row
     * @param str text message to render
     * @param rgb 24-bit 0xRRGGBB font colour
     */
    void drawText(int x0, int y0, String str, int rgb) {
        for (int ci = 0; ci < str.length(); ci++) {
            char c = str.charAt(ci);
            if (c >= FONT_BITS.length) continue;
            short glyph = FONT_BITS[c];
            if (glyph == 0 && c != ' ') continue;

            for (int k = 0; k < 15; k++) {
                if (((glyph >> (14 - k)) & 1) == 1) {
                    int x = x0 + ci * 4 + (k % 3);
                    int y = y0 + (k / 3);
                    if (x >= 0 && x < cols && y >= 0 && y < rows) {
                        pix[y * cols + x] = rgb;
                    }
                }
            }
        }
    }

    /** Compatibility alias for {@link #drawText(int, int, String, int)}. */
    void text(int x0, int y0, String s, int rgb) {
        drawText(x0, y0, s, rgb);
    }
}
