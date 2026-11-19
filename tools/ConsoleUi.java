import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

/** The building blocks of the console style as small Swing components, drawn with Graphics2D (no images). */
final class ConsoleUi {
    static void quality(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
        g.setRenderingHint(RenderingHints.KEY_STROKE_CONTROL, RenderingHints.VALUE_STROKE_PURE);
    }

    /**
     * The L-shaped elbow: a bar along the top and a leg down the left side that meet in a corner with a large outer radius and a tight inner radius.
     * Drawn into the rectangle (x, y, w, h): barH is the height of the bar, legW the width of the leg.
     */
    static Path2D elbow(double x, double y, double w, double h, double barH, double legW, double outerR, double innerR) {
        Path2D.Double p = new Path2D.Double();
        p.moveTo(x, y + outerR);
        p.append(new Arc2D.Double(x, y, 2 * outerR, 2 * outerR, 180, -90, Arc2D.OPEN), true);     // the outer corner, from the left to the top
        p.lineTo(x + w, y);
        p.lineTo(x + w, y + barH);
        p.lineTo(x + legW + innerR, y + barH);
        p.append(new Arc2D.Double(x + legW, y + barH, 2 * innerR, 2 * innerR, 90, 90, Arc2D.OPEN), true);   // the tight inner corner, from the top to the left
        p.lineTo(x + legW, y + h);
        p.lineTo(x, y + h);
        p.closePath();
        return p;
    }

    /** A rounded block with its text at the bottom right, like the keys of a modular console. Lighter when hovered, darker and nudged when pressed. */
    static class BlockButton extends JButton {
        Color base;
        BlockButton(String text, Color base) {
            super(text);
            this.base = base;
            setContentAreaFilled(false); setBorderPainted(false); setFocusPainted(false); setOpaque(false); setRolloverEnabled(true);
            setFont(ConsoleTheme.sansBold(14f));
            setForeground(ConsoleTheme.TEXT_ON_BLOCK);
            setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
            setMargin(new Insets(0, 0, 0, 0));
            setPreferredSize(new Dimension(100, ConsoleTheme.BLOCK_H + ConsoleTheme.GAP));      // the block is drawn GAP/2 smaller at the top and the bottom: the gap between blocks
            setMaximumSize(new Dimension(Integer.MAX_VALUE, ConsoleTheme.BLOCK_H + ConsoleTheme.GAP));
            setAlignmentX(Component.LEFT_ALIGNMENT);
        }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); quality(g);
            boolean on = isEnabled(), down = getModel().isPressed(), over = getModel().isRollover();
            Color fill = !on ? ConsoleTheme.mix(base, ConsoleTheme.BLACK, 0.78) : down ? ConsoleTheme.darker(base) : over ? ConsoleTheme.lighter(base) : base;
            g.setColor(fill);
            int gp = ConsoleTheme.GAP / 2, w = getWidth(), h = getHeight() - 2 * gp, r = ConsoleTheme.BLOCK_R;
            g.translate(0, gp);
            g.fill(new RoundRectangle2D.Double(0, 0, w, h, r * 2, r * 2));
            if (hasFocus() && on) { g.setColor(ConsoleTheme.PEACH); g.setStroke(new BasicStroke(2f)); g.draw(new RoundRectangle2D.Double(1.5, 1.5, w - 3, h - 3, r * 2, r * 2)); }
            g.setFont(getFont());
            g.setColor(on ? ConsoleTheme.TEXT_ON_BLOCK : ConsoleTheme.mix(base, ConsoleTheme.BLACK, 0.45));
            FontMetrics fm = g.getFontMetrics();
            int tw = fm.stringWidth(getText());
            g.drawString(getText(), w - ConsoleTheme.PAD - tw + (down ? 1 : 0), h - 8 + (down ? 1 : 0));        // bottom right
            g.dispose();
        }
    }

    /** A rounded bar with a line of text (Geist Mono) and, optionally, a row of small moving segments while something is going on. */
    static class PillBar extends JComponent {
        private String text = ""; private Color fill; private boolean busy; private int tick; private final javax.swing.Timer timer;
        PillBar(Color fill) {
            this.fill = fill; setOpaque(false); setPreferredSize(new Dimension(100, 30));
            timer = new javax.swing.Timer(120, e -> { tick++; repaint(); });
        }
        void setText(String t) { text = t; repaint(); }
        void setBusy(boolean b) { busy = b; if (b) timer.start(); else timer.stop(); repaint(); }
        void setFill(Color c) { fill = c; repaint(); }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); quality(g);
            int w = getWidth(), h = getHeight();
            g.setColor(fill); g.fill(new RoundRectangle2D.Double(0, 0, w, h, h, h));
            g.setFont(ConsoleTheme.mono(13f)); g.setColor(ConsoleTheme.BLACK);
            g.drawString(text, h / 2 + 2, (h + g.getFontMetrics().getAscent() - g.getFontMetrics().getDescent()) / 2);
            if (busy) {
                for (int i = 0; i < 6; i++) {
                    g.setColor(ConsoleTheme.mix(fill, ConsoleTheme.BLACK, i == tick % 6 ? 0.0 : 0.55));
                    g.fill(new RoundRectangle2D.Double(w - h / 2 - 6 * 16 + i * 16 - 6, h / 2 - 5, 12, 10, 8, 8));
                }
            }
            g.dispose();
        }
    }

    /** A flat band with a heading in Geist Sans, for a group of blocks. */
    static class Band extends JComponent {
        private final String text; private final Color color;
        Band(String text, Color color) {
            this.text = text; this.color = color; setOpaque(false);
            setPreferredSize(new Dimension(100, 22)); setMaximumSize(new Dimension(Integer.MAX_VALUE, 22)); setAlignmentX(Component.LEFT_ALIGNMENT);
        }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); quality(g);
            g.setFont(ConsoleTheme.sansBold(13f)); g.setColor(color);
            g.drawString(text, 4, getHeight() - 6);
            g.setColor(ConsoleTheme.mix(color, ConsoleTheme.BLACK, 0.7));
            g.fillRect(g.getFontMetrics().stringWidth(text) + 12, getHeight() - 10, Math.max(0, getWidth() - g.getFontMetrics().stringWidth(text) - 12), 2);
            g.dispose();
        }
    }

    /** A panel with a thin rounded outline and a title, for the previews. */
    static class Frame extends JPanel {
        private final String title; private final Color color;
        Frame(String title, Color color) { this.title = title; this.color = color; setOpaque(false); setBorder(BorderFactory.createEmptyBorder(26, 8, 8, 8)); }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); quality(g);
            g.setColor(ConsoleTheme.mix(color, ConsoleTheme.BLACK, 0.55)); g.setStroke(new BasicStroke(1.5f));
            g.draw(new RoundRectangle2D.Double(1, 9, getWidth() - 3, getHeight() - 11, 16, 16));
            g.setFont(ConsoleTheme.sansBold(12f));
            int tw = g.getFontMetrics().stringWidth(title);
            g.setColor(ConsoleTheme.BLACK); g.fillRect(14, 0, tw + 12, 20);
            g.setColor(color); g.drawString(title, 20, 14);
            g.dispose();
            super.paintComponent(g0);
        }
    }
}
