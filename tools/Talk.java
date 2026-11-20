import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.Timer;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.RenderingHints;
import java.awt.font.LineBreakMeasurer;
import java.awt.font.TextAttribute;
import java.awt.font.TextLayout;
import java.awt.geom.Ellipse2D;
import java.awt.geom.RoundRectangle2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.text.AttributedString;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;

/**
 * Slide presentation parser, layout engine, and playback sequencer for technical talks.
 * <p>
 * Reads Markdown slide documents from {@code tools/talk/*.md} ordered alphabetically.
 * Format specification:
 * <pre>
 * ---
 * seconds: 45          duration to display slide before advancing
 * demo: disk           optional scenario trigger (e.g. 'disk', 'bar:nightrider')
 * ---
 * # Slide Title
 * - Bullet point 1
 * - Bullet point 2
 * &gt; Speaker note line (logged to console, omitted from visual slide)
 * ![Photo Caption](img/cores3.jpg)
 * </pre>
 * </p>
 */
final class Talk {

    private Talk() {
        // Non-instantiable utility class
    }

    /**
     * Data record representing a parsed presentation slide.
     *
     * @param file    filename on disk
     * @param title   slide heading
     * @param bullets body bullet points and paragraphs
     * @param notes   speaker notes
     * @param seconds nominal display duration
     * @param demo    optional automated scenario key to run during this slide
     * @param images  list of referenced image paths
     */
    record Slide(
            String file,
            String title,
            List<String> bullets,
            List<String> notes,
            int seconds,
            String demo,
            List<Path> images
    ) {}

    /**
     * Loads and parses all {@code .md} slide files in the specified directory.
     *
     * @param dir path to directory containing slide files
     * @return sorted list of parsed {@link Slide} instances
     */
    static List<Slide> load(Path dir) {
        List<Slide> slides = new ArrayList<>();
        if (!Files.isDirectory(dir)) {
            return slides;
        }

        try (var s = Files.list(dir)) {
            List<Path> paths = s.filter(p -> p.getFileName().toString().endsWith(".md"))
                    .sorted()
                    .toList();
            for (Path path : paths) {
                slides.add(parse(path));
            }
        } catch (IOException ignored) {
            // Return empty or partial list
        }
        return slides;
    }

    /**
     * Parses a single Markdown slide file.
     *
     * @param path slide file path
     * @return parsed {@link Slide}
     * @throws IOException if reading the file fails
     */
    static Slide parse(Path path) throws IOException {
        List<String> lines = Files.readAllLines(path);
        int seconds = 30;
        String demo = null;
        String title = path.getFileName().toString();
        int lineIdx = 0;

        List<String> bullets = new ArrayList<>();
        List<String> notes = new ArrayList<>();
        List<Path> images = new ArrayList<>();

        // Parse optional YAML-style frontmatter delimiter "---"
        if (!lines.isEmpty() && lines.getFirst().trim().equals("---")) {
            for (lineIdx = 1; lineIdx < lines.size() && !lines.get(lineIdx).trim().equals("---"); lineIdx++) {
                String line = lines.get(lineIdx);
                int colon = line.indexOf(':');
                if (colon < 0) continue;

                String key = line.substring(0, colon).trim();
                String val = line.substring(colon + 1).trim();

                switch (key) {
                    case "seconds" -> {
                        try {
                            seconds = Integer.parseInt(val);
                        } catch (NumberFormatException ignored) {}
                    }
                    case "demo" -> demo = val.isEmpty() ? null : val;
                }
            }
            lineIdx++; // Skip closing "---"
        }

        // Parse Markdown body
        for (; lineIdx < lines.size(); lineIdx++) {
            String l = lines.get(lineIdx).strip();
            if (l.isEmpty()) continue;

            if (l.startsWith("# ")) {
                title = l.substring(2).trim();
            } else if (l.startsWith("- ") || l.startsWith("* ")) {
                bullets.add("• " + l.substring(2).trim());
            } else if (l.startsWith(">")) {
                notes.add(l.substring(1).trim());
            } else if (l.matches("!\\[.*\\]\\(.+\\)")) {
                String sub = l.substring(l.indexOf("](") + 2, l.lastIndexOf(")"));
                Path imgPath = path.toAbsolutePath().getParent().resolve(sub).normalize();
                images.add(imgPath);
            } else {
                bullets.add(l);
            }
        }

        return new Slide(path.getFileName().toString(), title, bullets, notes, seconds, demo, images);
    }

    /**
     * Swing component rendering an individual slide with dynamic responsive text fitting.
     */
    static final class SlidePanel extends JPanel {
        private volatile Slide slide;
        private volatile int index;
        private volatile int count;
        Consumer<String> onTitle;

        SlidePanel() {
            setOpaque(false);
            setBorder(BorderFactory.createEmptyBorder(8, 12, 8, 12));
        }

        int shownIndex() { return index; }
        int shownCount() { return count; }
        String shownTitle() { return slide == null ? "" : slide.title(); }

        void show(Slide s, int idx, int total) {
            this.slide = s;
            this.index = idx;
            this.count = total;
            if (onTitle != null && s != null) {
                onTitle.accept(s.title());
            }
            repaint();
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            ConsoleUi.quality(g);

            Insets insets = getInsets();
            int x = insets.left;
            int y = insets.top;
            int w = getWidth() - insets.left - insets.right;
            int h = getHeight() - insets.top - insets.bottom;

            Slide s = slide;
            if (s == null) {
                g.setColor(ConsoleTheme.mix(ConsoleTheme.PEACH, ConsoleTheme.BLACK, 0.5));
                g.setFont(ConsoleTheme.sansBold(20f));
                g.drawString("Start the talk, or pick a demo.", x, y + 30);
                g.dispose();
                return;
            }

            int textW = w;
            int imageW = 0;

            if (!s.images().isEmpty()) {
                boolean hasText = !s.bullets().isEmpty();
                imageW = hasText ? (int) (w * 0.6) : w;
                textW = hasText ? w - imageW - 20 : 0;
                drawImages(g, s.images(), x + w - imageW, y, imageW, h);
            }

            if (textW > 0) {
                // Iteratively scale down font size until all text fits inside height bounds
                for (float size = 44f; size >= 14f; size -= 2f) {
                    if (layoutAndRenderText(g, s, x, y, textW, h, size, false)) {
                        break;
                    }
                    if (size <= 14f) {
                        layoutAndRenderText(g, s, x, y, textW, h, 14f, true);
                    }
                }
            }
            g.dispose();
        }

        private static final Map<Path, BufferedImage> IMAGE_CACHE = new HashMap<>();

        private static BufferedImage loadCachedImage(Path p) {
            return IMAGE_CACHE.computeIfAbsent(p, k -> {
                try {
                    return ImageIO.read(k.toFile());
                } catch (IOException | RuntimeException e) {
                    return null;
                }
            });
        }

        private void drawImages(Graphics2D g, List<Path> images, int x, int y, int w, int h) {
            int count = images.size();
            int gap = 12;
            int cellH = (h - (count - 1) * gap) / count;

            for (int i = 0; i < count; i++) {
                int cellY = y + i * (cellH + gap);
                Path imagePath = images.get(i);
                BufferedImage img = loadCachedImage(imagePath);

                if (img == null) {
                    g.setColor(ConsoleTheme.SLIDE_NOTE);
                    g.setFont(ConsoleTheme.mono(13f));
                    g.drawString("picture not found: " + imagePath.getFileName(), x, cellY + 20);
                    continue;
                }

                double scale = Math.min((double) w / img.getWidth(), (double) cellH / img.getHeight());
                int imgW = (int) (img.getWidth() * scale);
                int imgH = (int) (img.getHeight() * scale);
                int posX = x + (w - imgW) / 2;
                int posY = cellY + (cellH - imgH) / 2;

                Graphics2D gi = (Graphics2D) g.create();
                gi.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
                gi.setClip(new RoundRectangle2D.Double(posX, posY, imgW, imgH, 18, 18));
                gi.drawImage(img, posX, posY, imgW, imgH, null);
                gi.dispose();
            }
        }

        private boolean layoutAndRenderText(Graphics2D g, Slide s, int x, int y, int w, int h, float size, boolean force) {
            Font bodyFont = ConsoleTheme.sansRegular(size);
            int indent = (int) (size * 0.95);

            record FormattedItem(Font font, Color colour, List<TextLayout> lines, boolean isBullet, double bottomGap) {}
            List<FormattedItem> items = new ArrayList<>();

            for (String b : s.bullets()) {
                boolean isBullet = b.startsWith("• ");
                String text = isBullet ? b.substring(2) : b;
                Color col = isBullet ? ConsoleTheme.SLIDE_TEXT : ConsoleTheme.SLIDE_NOTE;
                int availW = w - (isBullet ? indent : 0);

                items.add(new FormattedItem(
                        bodyFont,
                        col,
                        wrapText(text, bodyFont, availW, g),
                        isBullet,
                        size * 0.5
                ));
            }

            double totalHeight = 0;
            for (FormattedItem item : items) {
                totalHeight += item.lines().stream()
                        .mapToDouble(t -> t.getAscent() + t.getDescent() + t.getLeading() * 1.25)
                        .sum() + item.bottomGap();
            }

            if (totalHeight > h && !force) {
                return false;
            }

            double currentY = y;
            for (FormattedItem item : items) {
                g.setColor(item.colour());
                boolean firstLine = true;

                for (TextLayout layout : item.lines()) {
                    currentY += layout.getAscent();
                    if (item.isBullet() && firstLine) {
                        g.setColor(ConsoleTheme.SLIDE_BULLET);
                        double dotSize = size * 0.3;
                        g.fill(new Ellipse2D.Double(
                                x + size * 0.12,
                                currentY - size * 0.36 - dotSize / 2,
                                dotSize,
                                dotSize
                        ));
                        g.setColor(item.colour());
                    }

                    float drawX = (float) (x + (item.isBullet() ? indent : 0));
                    layout.draw(g, drawX, (float) currentY);
                    currentY += layout.getDescent() + layout.getLeading() * 1.25;
                    firstLine = false;
                }
                currentY += item.bottomGap();
            }
            return true;
        }

        private static List<TextLayout> wrapText(String text, Font font, int width, Graphics2D g) {
            List<TextLayout> layouts = new ArrayList<>();
            AttributedString attrStr = new AttributedString(text.isEmpty() ? " " : text);
            attrStr.addAttribute(TextAttribute.FONT, font);

            LineBreakMeasurer measurer = new LineBreakMeasurer(attrStr.getIterator(), g.getFontRenderContext());
            int targetWidth = Math.max(40, width);
            while (measurer.getPosition() < text.length()) {
                layouts.add(measurer.nextLayout(targetWidth));
            }
            return layouts;
        }
    }

    /**
     * Presentation timeline sequencer automatically advancing slides according to their durations.
     */
    static final class Player {
        final List<Slide> slides;
        final Consumer<Slide> onSlide;
        final Runnable onTick;
        final BooleanSupplier demoRunning;

        int index = -1;
        long slideStartTime;
        long pausedTimestamp;
        boolean running = false;
        boolean paused = false;
        private final Timer tickTimer;

        Player(List<Slide> slides, Consumer<Slide> onSlide, Runnable onTick, BooleanSupplier demoRunning) {
            this.slides = slides;
            this.onSlide = onSlide;
            this.onTick = onTick;
            this.demoRunning = demoRunning;
            this.tickTimer = new Timer(250, _ -> tick());
        }

        long slideElapsedMs() {
            return index < 0 ? 0 : (paused ? pausedTimestamp : System.currentTimeMillis()) - slideStartTime;
        }

        long totalMs() {
            return slides.stream().mapToLong(s -> s.seconds() * 1000L).sum();
        }

        void start() {
            running = true;
            paused = false;
            jumpTo(0);
            tickTimer.start();
        }

        void stop() {
            running = false;
            tickTimer.stop();
            index = -1;
            onTick.run();
        }

        void pause() {
            if (!running) return;
            if (paused) {
                slideStartTime += System.currentTimeMillis() - pausedTimestamp;
                paused = false;
            } else {
                paused = true;
                pausedTimestamp = System.currentTimeMillis();
            }
            onTick.run();
        }

        void jumpTo(int idx) {
            int target = Math.max(0, idx);
            if (target >= slides.size()) {
                stop();
                return;
            }
            index = target;
            slideStartTime = System.currentTimeMillis();
            paused = false;
            onSlide.accept(slides.get(target));
            onTick.run();
        }

        void next() {
            if (running) jumpTo(index + 1);
        }

        void prev() {
            if (running) jumpTo(index - 1);
        }

        long elapsedMs() {
            if (index < 0) return 0;
            long completedMs = 0;
            for (int i = 0; i < index; i++) {
                completedMs += slides.get(i).seconds() * 1000L;
            }
            long curElapsed = (paused ? pausedTimestamp : System.currentTimeMillis()) - slideStartTime;
            return completedMs + Math.min(slides.get(index).seconds() * 1000L, curElapsed);
        }

        private void tick() {
            if (!running || paused) return;
            Slide curSlide = slides.get(index);
            long elapsed = System.currentTimeMillis() - slideStartTime;
            long requiredMs = curSlide.seconds() * 1000L;

            if (elapsed >= requiredMs && !(curSlide.demo() != null && demoRunning.getAsBoolean())) {
                jumpTo(index + 1);
            } else {
                onTick.run();
            }
        }
    }
}
