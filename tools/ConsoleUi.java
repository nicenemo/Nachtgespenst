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


    /** The ghost and the crescent moon of the site icon, drawn as paths without the square background (viewBox 128 x 128). */
    static void ghost(Graphics2D g0, double x, double y, double size) {
        Graphics2D g = (Graphics2D) g0.create(); quality(g);
        g.translate(x, y); g.scale(size / 128.0, size / 128.0);
        Area moon = new Area(new Ellipse2D.Double(81, 13, 34, 34)); moon.subtract(new Area(new Ellipse2D.Double(93, 12, 26, 26)));
        g.setColor(new Color(0xF6E7A6)); g.fill(moon);
        Path2D.Double body = new Path2D.Double();
        body.moveTo(60, 20); body.curveTo(80, 20, 90, 38, 88, 60); body.curveTo(87, 74, 92, 90, 95, 108);
        body.quadTo(88, 100, 82, 109); body.quadTo(76, 118, 70, 109); body.quadTo(64, 100, 58, 109); body.quadTo(52, 118, 46, 109); body.quadTo(40, 100, 33, 108);
        body.curveTo(36, 90, 40, 74, 36, 60); body.curveTo(34, 38, 42, 20, 60, 20); body.closePath();
        Color ink = new Color(0x1A1626);
        g.setColor(new Color(0xECE8F6)); g.fill(body);
        g.setColor(ink); g.setStroke(new BasicStroke(2.2f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND)); g.draw(body);
        g.fill(new Ellipse2D.Double(45.6, 39, 10.8, 20)); g.fill(new Ellipse2D.Double(63.6, 39, 10.8, 20)); g.fill(new Ellipse2D.Double(54, 66, 12, 30));
        g.setStroke(new BasicStroke(1.6f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        Path2D.Double nose = new Path2D.Double(); nose.moveTo(59, 63); nose.quadTo(60, 66.5, 61, 63); nose.moveTo(63, 63); nose.quadTo(64, 66.5, 65, 63); g.draw(nose);
        for (int side = 0; side < 2; side++) {
            AffineTransform keep = g.getTransform();
            g.rotate(Math.toRadians(side == 0 ? 12 : -12), side == 0 ? 38 : 82, 66);
            Ellipse2D hand = new Ellipse2D.Double((side == 0 ? 38 : 82) - 7.5, 54, 15, 24);
            g.setColor(new Color(0xD9D3EE)); g.fill(hand); g.setColor(ink); g.setStroke(new BasicStroke(2f)); g.draw(hand);
            g.setTransform(keep);
        }
        g.dispose();
    }

    /** The swirling night sky of the site (hero-bg.svg): four waves in dark red and orange, scaled to a band of any size. Drawn inside the clip of the caller. */
    static void waves(Graphics2D g0, double x, double y, double w, double h) {
        Graphics2D g = (Graphics2D) g0.create(); quality(g);
        double k = h / 150.0, sx = w / 1660.0;
        String[] d = {"M-20 300 C160 250 300 350 480 300 S800 240 1000 310 S1360 260 1640 300", "M-20 370 C180 320 320 420 520 370 S860 310 1060 380 S1380 330 1640 372",
                      "M-20 440 C170 400 330 480 540 440 S880 390 1080 450 S1400 410 1640 442", "M-20 495 C190 462 340 520 560 490 S900 455 1100 500 S1420 470 1640 492"};
        int[] col = {0x3A1A34, 0x7A2A1C, 0xB5481F, 0x7A2A1C}; float[] wd = {46, 40, 30, 26};
        double[][][] pts = {
            {{-20, 300}, {160, 250, 300, 350, 480, 300}, {800, 240, 1000, 310}, {1360, 260, 1640, 300}},
            {{-20, 370}, {180, 320, 320, 420, 520, 370}, {860, 310, 1060, 380}, {1380, 330, 1640, 372}},
            {{-20, 440}, {170, 400, 330, 480, 540, 440}, {880, 390, 1080, 450}, {1400, 410, 1640, 442}},
            {{-20, 495}, {190, 462, 340, 520, 560, 490}, {900, 455, 1100, 500}, {1420, 470, 1640, 492}}};
        g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 0.75f));
        for (int i = 0; i < 4; i++) {
            double[][] q = pts[i];
            Path2D.Double p = new Path2D.Double(); p.moveTo(q[0][0], q[0][1]);
            p.curveTo(q[1][0], q[1][1], q[1][2], q[1][3], q[1][4], q[1][5]);
            double cx = q[1][4] * 2 - q[1][2], cy = q[1][5] * 2 - q[1][3];                  // the smooth continuation (SVG "S")
            p.curveTo(cx, cy, q[2][0], q[2][1], q[2][2], q[2][3]);
            double cx2 = q[2][2] * 2 - q[2][0], cy2 = q[2][3] * 2 - q[2][1];
            p.curveTo(cx2, cy2, q[3][0], q[3][1], q[3][2], q[3][3]);
            AffineTransform at = new AffineTransform(); at.translate(x, y); at.scale(sx, k); at.translate(20, -285);
            Shape sh = at.createTransformedShape(p);
            g.setColor(new Color(col[i])); g.setStroke(new BasicStroke((float) (wd[i] * k), BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND)); g.draw(sh);
        }
        g.dispose();
    }

    /**
     * One lamp of the LED bar as a vertical rectangle behind a fine lattice, like the diffuser over the LEDs in the keys of a keyboard: the lamp lights the whole
     * rectangle in its colour, the lattice lines stay dark, and a soft glow spills over the edge. A lamp that is off shows the bare lattice.
     */
    static void ledCell(Graphics2D g, int x, int y, int w, int h, int rgb) {
        int r = (rgb >> 16) & 255, gr = (rgb >> 8) & 255, b = rgb & 255, peak = Math.max(r, Math.max(gr, b));
        double luma = peak / 255.0;
        Color on = peak == 0 ? Color.BLACK : new Color(r * 255 / peak, gr * 255 / peak, b * 255 / peak);        // the hue at full strength; the brightness is applied below
        RoundRectangle2D.Double box = new RoundRectangle2D.Double(x, y, w, h, 6, 6);
        if (luma > 0.03) {                                                                                      // glow
            for (int i = 3; i >= 1; i--) {
                g.setColor(new Color(on.getRed(), on.getGreen(), on.getBlue(), (int) (28 * luma)));
                g.fill(new RoundRectangle2D.Double(x - i * 2, y - i * 2, w + i * 4, h + i * 4, 6 + i * 3, 6 + i * 3));
            }
        }
        g.setColor(new Color(0x0C0C10)); g.fill(box);
        Shape old = g.getClip(); g.clip(box);
        g.setColor(ConsoleTheme.mix(new Color(0x16161C), on, Math.pow(luma, 0.7)));                              // gamma: a dim lamp still shows its colour
        g.fill(box);
        g.setColor(new Color(0, 0, 0, 215)); g.setStroke(new BasicStroke(1.2f));
        int step = Math.max(4, w / 6);
        for (int gx = x + step; gx < x + w; gx += step) g.draw(new java.awt.geom.Line2D.Double(gx, y, gx, y + h));        // the lattice
        for (int gy = y + step; gy < y + h; gy += step) g.draw(new java.awt.geom.Line2D.Double(x, gy, x + w, gy));
        g.setClip(old);
        g.setColor(new Color(0x3A3A44)); g.setStroke(new BasicStroke(1f)); g.draw(box);
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

    /** A small block with a drawn symbol instead of text: a turn clockwise or anticlockwise, a half turn, a mirror left to right or top to bottom. */
    static class IconButton extends BlockButton {
        enum Kind { TURN_RIGHT, TURN_LEFT, TURN_HALF, MIRROR_H, MIRROR_V }
        private final Kind kind;
        IconButton(Kind kind, String tip, Color base) {
            super("", base); this.kind = kind; setToolTipText(tip); getAccessibleContext().setAccessibleName(tip);
            setPreferredSize(new Dimension(40, 30)); setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        }
        @Override protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            Graphics2D g = (Graphics2D) g0.create(); quality(g);
            double cx = getWidth() / 2.0, cy = getHeight() / 2.0; boolean on = isEnabled(), down = getModel().isPressed();
            g.setColor(on ? ConsoleTheme.TEXT_ON_BLOCK : ConsoleTheme.mix(base, ConsoleTheme.BLACK, 0.45));
            g.setStroke(new BasicStroke(2f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
            g.translate(cx + (down ? 1 : 0), cy + (down ? 1 : 0));
            switch (kind) {
                case TURN_RIGHT, TURN_LEFT, TURN_HALF -> {
                    double r = kind == Kind.TURN_HALF ? 9 : 7; boolean left = kind == Kind.TURN_LEFT;
                    if (left) g.scale(-1, 1);
                    g.draw(new Arc2D.Double(-r, -r, 2 * r, 2 * r, 100, -300, Arc2D.OPEN));           // a clockwise arc with a gap at the top left
                    double th = Math.toRadians(160), ex = r * Math.cos(th), ey = -r * Math.sin(th);   // where the arc ends
                    double dx = Math.sin(th), dy = Math.cos(th);                                      // the direction of travel there (clockwise on the screen)
                    double tx = ex + dx * 3, ty = ey + dy * 3, nx = -dy, ny = dx;
                    Path2D.Double head = new Path2D.Double();
                    head.moveTo(tx - dx * 5 + nx * 4, ty - dy * 5 + ny * 4); head.lineTo(tx, ty); head.lineTo(tx - dx * 5 - nx * 4, ty - dy * 5 - ny * 4);
                    g.draw(head);
                    if (kind == Kind.TURN_HALF) { g.setFont(ConsoleTheme.sansBold(8f)); g.drawString("180", -g.getFontMetrics().stringWidth("180") / 2f, 3); }
                }
                case MIRROR_H, MIRROR_V -> {
                    if (kind == Kind.MIRROR_V) g.rotate(Math.PI / 2);
                    Path2D.Double l = new Path2D.Double(); l.moveTo(-3, -6); l.lineTo(-9, 6); l.lineTo(-3, 6); l.closePath();
                    Path2D.Double r2 = new Path2D.Double(); r2.moveTo(3, -6); r2.lineTo(9, 6); r2.lineTo(3, 6); r2.closePath();
                    g.fill(r2); g.draw(l);                                                           // one solid and one outlined triangle either side of the axis
                    g.setStroke(new BasicStroke(1.5f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER, 10f, new float[]{2.5f, 2.5f}, 0f));
                    g.draw(new java.awt.geom.Line2D.Double(0, -8, 0, 8));
                }
            }
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
            FontMetrics fm = g.getFontMetrics(); String t = text; int room = w - h - (busy ? 6 * 16 + h / 2 : 0);
            while (t.length() > 1 && fm.stringWidth(t) > room) t = t.substring(0, t.length() - 2) + "…";          // never wider than the bar
            g.drawString(t, h / 2 + 2, (h + fm.getAscent() - fm.getDescent()) / 2);
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
        Frame(String title, Color color) { this.title = title; this.color = color; setOpaque(false); setBorder(BorderFactory.createEmptyBorder(28, 12, 12, 12)); }
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
