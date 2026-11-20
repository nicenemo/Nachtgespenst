import java.awt.Color;
import java.awt.Font;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

/**
 * Shared visual palette, typography, and geometry metrics for desktop tools and GUI demos.
 * <p>
 * Implements the Nachtgespenst design system (detailed in {@code docs/wiki/design-system.md}):
 * <ul>
 *     <li><b>Ground:</b> Deep black ground ({@code #000000}) with high-contrast text.</li>
 *     <li><b>Accents:</b> Primary amber ({@code #FF9900}), secondary peach ({@code #FFCC99}),
 *         tertiary lavender ({@code #9999FF}), and alert coral ({@code #CC6666}).</li>
 *     <li><b>Typography:</b> Geist Sans for labels and buttons, Geist Mono for live telemetry data
 *         (SIL Open Font License 1.1, loaded from {@code tools/fonts/}).</li>
 * </ul>
 * </p>
 */
final class ConsoleTheme {

    // --- Base Color Palette ---
    public static final Color BLACK = new Color(0x000000);
    public static final Color AMBER = new Color(0xFF9900);        // Primary accent: elbows and active blocks
    public static final Color PEACH = new Color(0xFFCC99);        // Secondary accent: highlights and labels
    public static final Color LAVENDER = new Color(0x9999FF);     // Tertiary accent: navigation
    public static final Color CORAL = new Color(0xCC6666);        // Status warnings and alerts
    public static final Color DIM = new Color(0x1B1208);          // Inactive / unpowered block fill
    public static final Color TEXT_ON_BLOCK = BLACK;
    public static final Color TEXT = PEACH;                       // Live telemetry text on black ground

    // --- Slide Presentation Palette ---
    public static final Color SLIDE_TITLE = new Color(0xB8B8FF);
    public static final Color SLIDE_TEXT = new Color(0xECECF6);
    public static final Color SLIDE_BULLET = AMBER;
    public static final Color SLIDE_NOTE = new Color(0xB4B4CC);

    // --- Desktop Layout Metrics ---
    public static final int GAP = 6;
    public static final int PAD = 10;
    public static final int BAR_H = 64;
    public static final int LEG_W = 250;
    public static final int OUTER_R = 56;
    public static final int INNER_R = 18;
    public static final int BLOCK_R = 14;
    public static final int BLOCK_H = 38;
    public static final int ELBOW_W = 12;

    private static final Map<String, Font> FONT_CACHE = new HashMap<>();

    private ConsoleTheme() {
        // Non-instantiable utility class
    }

    /**
     * Loads a TrueType font from the repository's {@code tools/fonts/} directory,
     * falling back to standard logical system fonts if absent.
     *
     * @param fileName font filename within {@code tools/fonts/}
     * @param fallback logical font family fallback
     * @param style    font style flags (e.g. {@link Font#PLAIN}, {@link Font#BOLD})
     * @return the loaded or fallback {@link Font}
     */
    private static Font loadFont(String fileName, String fallback, int style) {
        String cacheKey = fileName + ":" + style;
        return FONT_CACHE.computeIfAbsent(cacheKey, _ -> {
            try {
                Path fontPath = Boards.repoRoot().resolve("tools/fonts").resolve(fileName);
                if (Files.exists(fontPath)) {
                    return Font.createFont(Font.TRUETYPE_FONT, fontPath.toFile());
                }
            } catch (Exception ignored) {
                // Fall back to system logical font
            }
            return new Font(fallback, style, 12);
        });
    }

    /** Returns Geist Sans Regular scaled to the requested point size. */
    static Font sansRegular(float size) {
        return loadFont("Geist-Regular.ttf", Font.SANS_SERIF, Font.PLAIN).deriveFont(size);
    }

    /** Returns Geist Sans Bold scaled to the requested point size. */
    static Font sansBold(float size) {
        return loadFont("Geist-Bold.ttf", Font.SANS_SERIF, Font.BOLD).deriveFont(size);
    }

    /** Returns Geist Mono Regular scaled to the requested point size. */
    static Font mono(float size) {
        return loadFont("GeistMono-Regular.ttf", Font.MONOSPACED, Font.PLAIN).deriveFont(size);
    }

    /**
     * Linearly interpolates between two colours.
     *
     * @param a first colour (t = 0.0)
     * @param b second colour (t = 1.0)
     * @param t interpolation weight [0.0, 1.0]
     * @return blended {@link Color}
     */
    static Color mix(Color a, Color b, double t) {
        double factor = Math.clamp(t, 0.0, 1.0);
        int r = (int) (a.getRed() * (1.0 - factor) + b.getRed() * factor);
        int g = (int) (a.getGreen() * (1.0 - factor) + b.getGreen() * factor);
        int bl = (int) (a.getBlue() * (1.0 - factor) + b.getBlue() * factor);
        return new Color(r, g, bl);
    }

    /** Returns a lightened tint of the specified colour (mixed with 22% white). */
    static Color lighter(Color c) {
        return mix(c, Color.WHITE, 0.22);
    }

    /** Returns a darkened shade of the specified colour (mixed with 30% black). */
    static Color darker(Color c) {
        return mix(c, BLACK, 0.30);
    }
}
