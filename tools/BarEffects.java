import java.awt.Color;
import java.util.Arrays;
import java.util.Random;
import java.util.function.Consumer;

/**
 * Visual lighting animation effects for the 10-LED physical base bar (lamps 0 to 9).
 * <p>
 * Driven in real-time from the host over the USB/Bluetooth LampArray interface
 * (Report ID 8) while the board's bar lamp page is visible.
 * </p>
 * <p>
 * <b>Hardware Geometry:</b><br>
 * The ten LEDs form a U-shape around the base of the device:
 * <ul>
 *     <li>Lamps 0..4 run down the <b>right side</b> (0 at the top, 4 at the bottom).</li>
 *     <li>Lamps 5..9 run up the <b>left side</b> (5 at the bottom, 9 at the top).</li>
 * </ul>
 * For any given row index {@code r} (0..4 from top to bottom):
 * <ul>
 *     <li>Right side: {@code lamp = r}</li>
 *     <li>Left side: {@code lamp = 9 - r}</li>
 * </ul>
 * </p>
 */
final class BarEffects {

    public static final int BAR_LAMP_COUNT = 10;
    private static final int FRAME_DELAY_MS = 16; // ~60 frames per second

    /**
     * Optional listener invoked with the current 10-lamp RGB buffer for each rendered frame.
     * Used by the GUI preview panel to mirror the device display.
     */
    static volatile Consumer<int[]> listener;

    private BarEffects() {
        // Non-instantiable utility class
    }

    /**
     * Sends a complete 10-lamp frame to the hardware.
     * <p>
     * Emits two {@link Proto#REPORT_MULTI_UPDATE} packets:
     * <ol>
     *     <li>Report 1: Lamps 0..7 (partial frame)</li>
     *     <li>Report 2: Lamps 8..9 with the update complete flag set</li>
     * </ol>
     * </p>
     *
     * @param hid the target HID device
     * @param rgb 10-element array of 24-bit 0xRRGGBB color values
     */
    static void frame(Hid hid, int[] rgb) {
        Consumer<int[]> l = listener;
        if (l != null) {
            l.accept(rgb.clone());
        }

        // First 8 lamps (0..7)
        int[] ids1 = new int[8];
        int[][] colours1 = new int[8][4];
        for (int k = 0; k < 8; k++) {
            ids1[k] = k;
            colours1[k][0] = (rgb[k] >> 16) & 0xFF; // Red
            colours1[k][1] = (rgb[k] >> 8) & 0xFF;  // Green
            colours1[k][2] = rgb[k] & 0xFF;         // Blue
            colours1[k][3] = 0xFF;                  // Intensity
        }
        hid.write(Proto.multi(ids1, colours1, 8, false));

        // Remaining 2 lamps (8..9) with update complete set to true
        int[] ids2 = {8, 9, 0, 0, 0, 0, 0, 0};
        int[][] colours2 = new int[8][4];
        for (int k = 0; k < 2; k++) {
            colours2[k][0] = (rgb[8 + k] >> 16) & 0xFF;
            colours2[k][1] = (rgb[8 + k] >> 8) & 0xFF;
            colours2[k][2] = rgb[8 + k] & 0xFF;
            colours2[k][3] = 0xFF;
        }
        hid.write(Proto.multi(ids2, colours2, 2, true));
    }

    /**
     * Scales an RGB colour by an intensity factor in range [0.0, 1.0].
     *
     * @param color  24-bit 0xRRGGBB colour
     * @param factor brightness factor
     * @return scaled 24-bit colour
     */
    static int scale(int color, double factor) {
        double f = Math.clamp(factor, 0.0, 1.0);
        int r = (int) (((color >> 16) & 0xFF) * f);
        int g = (int) (((color >> 8) & 0xFF) * f);
        int b = (int) ((color & 0xFF) * f);
        return (r << 16) | (g << 8) | b;
    }

    /**
     * Converts HSV components to a 24-bit 0xRRGGBB colour.
     *
     * @param hue        hue angle [0.0, 1.0)
     * @param saturation saturation [0.0, 1.0]
     * @param value      brightness value [0.0, 1.0]
     * @return 24-bit 0xRRGGBB colour
     */
    static int hsv(double hue, double saturation, double value) {
        float h = (float) (hue - Math.floor(hue));
        return Color.HSBtoRGB(h, (float) saturation, (float) value) & 0xFFFFFF;
    }

    /** Returns the lamp ID for row {@code r} (0..4 from top) on the right side. */
    static int right(int row) {
        return row;
    }

    /** Returns the lamp ID for row {@code r} (0..4 from top) on the left side. */
    static int left(int row) {
        return 9 - row;
    }

    /**
     * Knight Rider scanner: red scanner bouncing up and down both sides symmetrically with an exponential fading tail.
     *
     * @param hid     the HID device
     * @param seconds total playback duration
     * @throws InterruptedException if animation is cancelled
     */
    static void nightrider(Hid hid, double seconds) throws InterruptedException {
        long startTime = System.nanoTime();
        int[] pixels = new int[BAR_LAMP_COUNT];

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            double elapsed = (System.nanoTime() - startTime) / 1e9;
            double pos = 2.0 + 2.0 * Math.sin(elapsed * 3.2); // Row 0..4 oscillation

            for (int r = 0; r < 5; r++) {
                int c = scale(0xFF1000, Math.exp(-Math.abs(r - pos) * 1.1));
                pixels[right(r)] = c;
                pixels[left(r)] = c;
            }
            frame(hid, pixels);
            Thread.sleep(FRAME_DELAY_MS);
        }
    }

    /**
     * Breathing effect: slow sinusoidal pulsation of soft cyan/blue glow across all lamps.
     *
     * @param hid     the HID device
     * @param seconds total playback duration
     * @throws InterruptedException if animation is cancelled
     */
    static void breath(Hid hid, double seconds) throws InterruptedException {
        long startTime = System.nanoTime();
        int[] pixels = new int[BAR_LAMP_COUNT];

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            double elapsed = (System.nanoTime() - startTime) / 1e9;
            double factor = (Math.sin(elapsed * 1.6 - Math.PI / 2) + 1.0) / 2.0;
            int c = scale(0x6090FF, 0.05 + 0.95 * factor * factor);

            Arrays.fill(pixels, c);
            frame(hid, pixels);
            Thread.sleep(FRAME_DELAY_MS);
        }
    }

    /**
     * Rainbow cascade: full-spectrum rainbow flowing downwards on both sides.
     *
     * @param hid     the HID device
     * @param seconds total playback duration
     * @throws InterruptedException if animation is cancelled
     */
    static void rainbow(Hid hid, double seconds) throws InterruptedException {
        long startTime = System.nanoTime();
        int[] pixels = new int[BAR_LAMP_COUNT];

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            double elapsed = (System.nanoTime() - startTime) / 1e9;
            for (int r = 0; r < 5; r++) {
                int c = hsv(r / 5.0 - elapsed * 0.3, 1.0, 1.0);
                pixels[right(r)] = c;
                pixels[left(r)] = c;
            }
            frame(hid, pixels);
            Thread.sleep(FRAME_DELAY_MS);
        }
    }

    /**
     * Comet effect: single bright turquoise head with long fading tail travelling along the entire U-chain.
     *
     * @param hid     the HID device
     * @param seconds total playback duration
     * @throws InterruptedException if animation is cancelled
     */
    static void comet(Hid hid, double seconds) throws InterruptedException {
        long startTime = System.nanoTime();
        int[] pixels = new int[BAR_LAMP_COUNT];

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            double elapsed = (System.nanoTime() - startTime) / 1e9;
            double head = (elapsed * 7.0) % 14.0 - 2.0;

            for (int i = 0; i < BAR_LAMP_COUNT; i++) {
                double dist = head - i;
                pixels[i] = (dist >= 0) ? scale(0x30FFD0, Math.exp(-dist * 0.7)) : 0;
            }
            frame(hid, pixels);
            Thread.sleep(FRAME_DELAY_MS);
        }
    }

    /**
     * Sparkle effect: random lamps flare in warm amber and gently decay.
     *
     * @param hid     the HID device
     * @param seconds total playback duration
     * @throws InterruptedException if animation is cancelled
     */
    static void sparkle(Hid hid, double seconds) throws InterruptedException {
        long startTime = System.nanoTime();
        double[] values = new double[BAR_LAMP_COUNT];
        Random random = new Random(3);
        int[] pixels = new int[BAR_LAMP_COUNT];

        while ((System.nanoTime() - startTime) / 1e9 < seconds) {
            for (int i = 0; i < BAR_LAMP_COUNT; i++) {
                values[i] *= 0.9;
                if (random.nextInt(14) == 0) {
                    values[i] = 1.0;
                }
                pixels[i] = scale(0xFFD070, values[i]);
            }
            frame(hid, pixels);
            Thread.sleep(FRAME_DELAY_MS);
        }
    }
}
