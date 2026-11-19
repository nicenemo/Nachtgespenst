import javax.swing.*;
import java.awt.*;
import java.awt.font.*;
import java.awt.geom.*;
import java.io.IOException;
import java.nio.file.*;
import java.text.AttributedString;
import java.util.*;
import java.util.List;
import java.util.function.Consumer;

/**
 * The slides of the talk. One Markdown file per slide in tools/talk/ (sorted by name), so every slide can be edited, reordered or dropped with a text editor.
 *
 * <pre>
 * ---
 * seconds: 45          how long the slide stays (the talk adds up to about ten minutes)
 * demo: disk           optional: a demo that starts with the slide (see DemoApp.demoFor); the talk waits for it to end
 * ---
 * # The title
 * - a bullet point
 * - another one
 * &gt; a line for the speaker (shown in the log, never on the slide)
 * </pre>
 * Plain lines are paragraphs. Only this small subset of Markdown is read, so the files stay readable everywhere.
 */
final class Talk {
    record Slide(String file, String title, List<String> bullets, List<String> notes, int seconds, String demo) { }

    static List<Slide> load(Path dir) {
        List<Slide> out = new ArrayList<>();
        if (!Files.isDirectory(dir)) return out;
        try (var s = Files.list(dir)) {
            for (Path p : s.filter(f -> f.getFileName().toString().endsWith(".md")).sorted().toList()) out.add(parse(p));
        } catch (IOException ignored) { }
        return out;
    }

    static Slide parse(Path p) throws IOException {
        List<String> lines = Files.readAllLines(p);
        int seconds = 30; String demo = null, title = p.getFileName().toString(); int i = 0;
        List<String> bullets = new ArrayList<>(), notes = new ArrayList<>();
        if (!lines.isEmpty() && lines.get(0).trim().equals("---")) {
            for (i = 1; i < lines.size() && !lines.get(i).trim().equals("---"); i++) {
                String l = lines.get(i); int c = l.indexOf(':'); if (c < 0) continue;
                String k = l.substring(0, c).trim(), v = l.substring(c + 1).trim();
                if (k.equals("seconds")) { try { seconds = Integer.parseInt(v); } catch (NumberFormatException ignored) { } }
                else if (k.equals("demo")) demo = v.isEmpty() ? null : v;
            }
            i++;
        }
        for (; i < lines.size(); i++) {
            String l = lines.get(i).strip();
            if (l.isEmpty()) continue;
            if (l.startsWith("# ")) title = l.substring(2).trim();
            else if (l.startsWith("- ") || l.startsWith("* ")) bullets.add("• " + l.substring(2).trim());
            else if (l.startsWith(">")) notes.add(l.substring(1).trim());
            else bullets.add(l);
        }
        return new Slide(p.getFileName().toString(), title, bullets, notes, seconds, demo);
    }

    /** Draws a slide: the title in amber and the bullets in peach, in the largest size that fits the panel. */
    static final class SlidePanel extends JPanel {
        private volatile Slide slide; private volatile int index, count;
        java.util.function.Consumer<String> onTitle;                  // the title of the slide is shown by the window (in its header), not on the slide
        SlidePanel() { setOpaque(false); setBorder(BorderFactory.createEmptyBorder(8, 12, 8, 12)); }
        void show(Slide s, int i, int n) { slide = s; index = i; count = n; if (onTitle != null) onTitle.accept(s.title()); repaint(); }

        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            Insets in = getInsets(); int x = in.left, y = in.top, w = getWidth() - in.left - in.right, h = getHeight() - in.top - in.bottom;
            Slide s = slide;
            if (s == null) {
                g.setColor(ConsoleTheme.mix(ConsoleTheme.PEACH, ConsoleTheme.BLACK, 0.5)); g.setFont(ConsoleTheme.sansBold(20f));
                g.drawString("Start the talk, or pick a demo.", x, y + 30); g.dispose(); return;
            }
            for (float size = 44f; size >= 14f; size -= 2f) {
                if (draw(g, s, x, y, w, h, size, false)) break;
                if (size <= 14f) draw(g, s, x, y, w, h, 14f, true);
            }
            g.dispose();
        }

        /** Lays the slide out at one size; returns whether it fits. Nothing is drawn unless it fits (or force is set). */
        private boolean draw(Graphics2D g, Slide s, int x, int y, int w, int h, float size, boolean force) {
            Font tf = ConsoleTheme.sansBold(size * 1.2f), bf = ConsoleTheme.sansRegular(size);
            int indent = (int) (size * 0.95);
            record Item(Font font, Color colour, List<TextLayout> lines, boolean bullet, double gap) { }
            List<Item> items = new ArrayList<>();
            for (String b : s.bullets()) {
                boolean bullet = b.startsWith("• "); String t = bullet ? b.substring(2) : b;
                items.add(new Item(bf, bullet ? ConsoleTheme.SLIDE_TEXT : ConsoleTheme.SLIDE_NOTE, wrap(t, bf, w - (bullet ? indent : 0), g), bullet, size * 0.5));
            }
            double total = 0;
            for (Item it : items) total += it.lines().stream().mapToDouble(t -> t.getAscent() + t.getDescent() + t.getLeading() * 1.25).sum() + it.gap();
            if (total > h && !force) return false;
            double yy = y;
            for (Item it : items) {
                g.setColor(it.colour());
                boolean first = true;
                for (TextLayout t : it.lines()) {
                    yy += t.getAscent();
                    if (it.bullet() && first) { g.setColor(ConsoleTheme.SLIDE_BULLET); double d = size * 0.3; g.fill(new java.awt.geom.Ellipse2D.Double(x + size * 0.12, yy - size * 0.36 - d / 2, d, d)); g.setColor(it.colour()); }
                    t.draw(g, (float) (x + (it.bullet() ? indent : 0)), (float) yy);
                    yy += t.getDescent() + t.getLeading() * 1.25; first = false;
                }
                yy += it.gap();
            }
            return true;
        }

        private static List<TextLayout> wrap(String text, Font f, int width, Graphics2D g) {
            List<TextLayout> out = new ArrayList<>();
            AttributedString as = new AttributedString(text.isEmpty() ? " " : text); as.addAttribute(TextAttribute.FONT, f);
            LineBreakMeasurer m = new LineBreakMeasurer(as.getIterator(), g.getFontRenderContext());
            while (m.getPosition() < text.length()) out.add(m.nextLayout(Math.max(40, width)));
            return out;
        }
    }

    /** Runs the slides on a timer: each slide stays for its seconds, and when it has a demo it also waits for the demo. */
    static final class Player {
        final List<Slide> slides; final Consumer<Slide> onSlide; final Runnable onTick; final java.util.function.BooleanSupplier demoRunning;
        int index = -1; long slideStart, pausedAt; boolean running, paused; private final javax.swing.Timer timer;
        /** Time on the current slide, in milliseconds (frozen while paused). */
        long slideElapsedMs() { return index < 0 ? 0 : (paused ? pausedAt : System.currentTimeMillis()) - slideStart; }
        long totalMs() { return slides.stream().mapToLong(s -> s.seconds() * 1000L).sum(); }

        Player(List<Slide> slides, Consumer<Slide> onSlide, Runnable onTick, java.util.function.BooleanSupplier demoRunning) {
            this.slides = slides; this.onSlide = onSlide; this.onTick = onTick; this.demoRunning = demoRunning;
            timer = new javax.swing.Timer(250, e -> tick());
        }
        void start() { running = true; paused = false; go(0); timer.start(); }
        void stop() { running = false; timer.stop(); index = -1; onTick.run(); }
        void pause() { if (!running) return; if (paused) { slideStart += System.currentTimeMillis() - pausedAt; paused = false; } else { paused = true; pausedAt = System.currentTimeMillis(); } onTick.run(); }
        void go(int i) {
            if (i < 0) i = 0;
            if (i >= slides.size()) { stop(); return; }
            index = i; slideStart = System.currentTimeMillis(); paused = false; onSlide.accept(slides.get(i)); onTick.run();
        }
        void next() { if (running) go(index + 1); }
        void prev() { if (running) go(index - 1); }
        /** Elapsed time of the talk: the seconds of the finished slides plus the time on this one. */
        long elapsedMs() {
            if (index < 0) return 0;
            long done = 0; for (int i = 0; i < index; i++) done += slides.get(i).seconds() * 1000L;
            return done + Math.min(slides.get(index).seconds() * 1000L, (paused ? pausedAt : System.currentTimeMillis()) - slideStart);
        }
        private void tick() {
            if (!running || paused) return;
            Slide s = slides.get(index);
            if (System.currentTimeMillis() - slideStart >= s.seconds() * 1000L && !(s.demo() != null && demoRunning.getAsBoolean())) go(index + 1);
            else onTick.run();
        }
    }
}
