import java.awt.*;
import java.io.File;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;

/**
 * The shared visual language of the desktop demo and the device (docs/wiki/design-system.md): a black ground, blocks of amber, peach and lavender with
 * rounded ends and elbows, a coral for alerts, Geist Sans for names and buttons and Geist Mono for live data. Sentence case everywhere, no all caps.
 * The fonts are loaded from tools/fonts (SIL Open Font License 1.1, see OFL.txt there) with a fallback to the system fonts.
 */
final class ConsoleTheme {
    static final Color BLACK = new Color(0x000000);
    static final Color AMBER = new Color(0xFF9900);        // primary accent: elbows and active blocks
    static final Color PEACH = new Color(0xFFCC99);        // secondary accent
    static final Color LAVENDER = new Color(0x9999FF);     // tertiary: navigation
    static final Color CORAL = new Color(0xCC6666);        // alerts and status
    static final Color DIM = new Color(0x1B1208);          // a block that is switched off
    static final Color TEXT_ON_BLOCK = BLACK;
    static final Color TEXT = PEACH;                       // live data on black

    // spacing and shapes for the desktop (the device uses half of these, see the design system)
    static final int GAP = 6, PAD = 10, BAR_H = 56, LEG_W = 250, OUTER_R = 56, INNER_R = 18, BLOCK_R = 14, BLOCK_H = 38;

    private static final Map<String, Font> CACHE = new HashMap<>();

    private static Font load(String file, String fallback, int style) {
        return CACHE.computeIfAbsent(file + style, k -> {
            try {
                Path p = Boards.repoRoot().resolve("tools/fonts").resolve(file);
                if (Files.exists(p)) return Font.createFont(Font.TRUETYPE_FONT, new File(p.toString()));
            } catch (Exception ignored) { }
            return new Font(fallback, style, 12);
        });
    }

    static Font sansRegular(float size) { return load("Geist-Regular.ttf", Font.SANS_SERIF, Font.PLAIN).deriveFont(size); }
    static Font sansBold(float size) { return load("Geist-Bold.ttf", Font.SANS_SERIF, Font.BOLD).deriveFont(size); }
    static Font mono(float size) { return load("GeistMono-Regular.ttf", Font.MONOSPACED, Font.PLAIN).deriveFont(size); }

    static Color mix(Color a, Color b, double t) {
        return new Color((int) (a.getRed() * (1 - t) + b.getRed() * t), (int) (a.getGreen() * (1 - t) + b.getGreen() * t), (int) (a.getBlue() * (1 - t) + b.getBlue() * t));
    }
    static Color lighter(Color c) { return mix(c, Color.WHITE, 0.22); }
    static Color darker(Color c) { return mix(c, BLACK, 0.30); }
}
