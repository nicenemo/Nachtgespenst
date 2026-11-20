import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.Timer;
import java.awt.AlphaComposite;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.Arc2D;
import java.awt.geom.Area;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.geom.Path2D;
import java.awt.geom.RoundRectangle2D;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * Custom vector graphics Swing UI components for the Nachtgespenst desktop console design system.
 * <p>
 * Provides reusable components drawn entirely with resolution-independent {@link Graphics2D} paths:
 * <ul>
 *     <li>L-shaped elbow bars and frames.</li>
 *     <li>Vector site mascot and background wave patterns.</li>
 *     <li>Diffuser lattice LED cell visualization.</li>
 *     <li>Modular console block buttons and icon buttons.</li>
 *     <li>Retro instrument-style digital LCD readouts with dot-matrix bar meters.</li>
 *     <li>Pill status bars with animated activity progress indicators.</li>
 * </ul>
 * </p>
 */
final class ConsoleUi {

    private ConsoleUi() {
        // Non-instantiable utility class
    }

    /**
     * Configures a Graphics2D context with high-quality rendering hints.
     *
     * @param g the graphics context to configure
     */
    static void quality(Graphics2D g) {
        g.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
        g.setRenderingHint(RenderingHints.KEY_STROKE_CONTROL, RenderingHints.VALUE_STROKE_PURE);
    }

    /**
     * Constructs the standard L-shaped elbow path: a horizontal top bar transitioning
     * smoothly via an outer and inner curve into a vertical left leg.
     */
    static Path2D elbow(double x, double y, double w, double h, double barH, double legW, double outerR, double innerR) {
        Path2D.Double p = new Path2D.Double();
        p.moveTo(x, y + outerR);
        p.append(new Arc2D.Double(x, y, 2 * outerR, 2 * outerR, 180, -90, Arc2D.OPEN), true);
        p.lineTo(x + w, y);
        p.lineTo(x + w, y + barH);
        p.lineTo(x + legW + innerR, y + barH);
        p.append(new Arc2D.Double(x + legW, y + barH, 2 * innerR, 2 * innerR, 90, 90, Arc2D.OPEN), true);
        p.lineTo(x + legW, y + h);
        p.lineTo(x, y + h);
        p.closePath();
        return p;
    }

    /**
     * Draws the Nachtgespenst ghost and crescent moon mascot vector emblem.
     */
    static void ghost(Graphics2D g0, double x, double y, double size) {
        Graphics2D g = (Graphics2D) g0.create();
        quality(g);
        g.translate(x, y);
        g.scale(size / 128.0, size / 128.0);

        // Crescent moon
        Area moon = new Area(new Ellipse2D.Double(81, 13, 34, 34));
        moon.subtract(new Area(new Ellipse2D.Double(93, 12, 26, 26)));
        g.setColor(new Color(0xF6E7A6));
        g.fill(moon);

        // Ghost body silhouette
        Path2D.Double body = new Path2D.Double();
        body.moveTo(60, 20);
        body.curveTo(80, 20, 90, 38, 88, 60);
        body.curveTo(87, 74, 92, 90, 95, 108);
        body.quadTo(88, 100, 82, 109);
        body.quadTo(76, 118, 70, 109);
        body.quadTo(64, 100, 58, 109);
        body.quadTo(52, 118, 46, 109);
        body.quadTo(40, 100, 33, 108);
        body.curveTo(36, 90, 40, 74, 36, 60);
        body.curveTo(34, 38, 42, 20, 60, 20);
        body.closePath();

        Color ink = new Color(0x1A1626);
        g.setColor(new Color(0xECE8F6));
        g.fill(body);
        g.setColor(ink);
        g.setStroke(new BasicStroke(2.2f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        g.draw(body);

        // Eyes and mouth
        g.fill(new Ellipse2D.Double(45.6, 39, 10.8, 20));
        g.fill(new Ellipse2D.Double(63.6, 39, 10.8, 20));
        g.fill(new Ellipse2D.Double(54, 66, 12, 30));

        // Nose
        g.setStroke(new BasicStroke(1.6f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        Path2D.Double nose = new Path2D.Double();
        nose.moveTo(59, 63);
        nose.quadTo(60, 66.5, 61, 63);
        nose.moveTo(63, 63);
        nose.quadTo(64, 66.5, 65, 63);
        g.draw(nose);

        // Hands
        for (int side = 0; side < 2; side++) {
            AffineTransform keep = g.getTransform();
            g.rotate(Math.toRadians(side == 0 ? 12 : -12), side == 0 ? 38 : 82, 66);
            Ellipse2D hand = new Ellipse2D.Double((side == 0 ? 38 : 82) - 7.5, 54, 15, 24);
            g.setColor(new Color(0xD9D3EE));
            g.fill(hand);
            g.setColor(ink);
            g.setStroke(new BasicStroke(2f));
            g.draw(hand);
            g.setTransform(keep);
        }
        g.dispose();
    }

    /**
     * Draws the swirling night-sky wave backdrop.
     */
    static void waves(Graphics2D g0, double x, double y, double w, double h) {
        Graphics2D g = (Graphics2D) g0.create();
        quality(g);
        double k = h / 150.0;
        double sx = w / 1660.0;

        int[] colors = {0x3A1A34, 0x7A2A1C, 0xB5481F, 0x7A2A1C};
        float[] strokeWidths = {46, 40, 30, 26};
        double[][][] wavePoints = {
                {{-20, 300}, {160, 250, 300, 350, 480, 300}, {800, 240, 1000, 310}, {1360, 260, 1640, 300}},
                {{-20, 370}, {180, 320, 320, 420, 520, 370}, {860, 310, 1060, 380}, {1380, 330, 1640, 372}},
                {{-20, 440}, {170, 400, 330, 480, 540, 440}, {880, 390, 1080, 450}, {1400, 410, 1640, 442}},
                {{-20, 495}, {190, 462, 340, 520, 560, 490}, {900, 455, 1100, 500}, {1420, 470, 1640, 492}}
        };

        g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 0.75f));
        for (int i = 0; i < 4; i++) {
            double[][] q = wavePoints[i];
            Path2D.Double p = new Path2D.Double();
            p.moveTo(q[0][0], q[0][1]);
            p.curveTo(q[1][0], q[1][1], q[1][2], q[1][3], q[1][4], q[1][5]);

            double cx = q[1][4] * 2 - q[1][2];
            double cy = q[1][5] * 2 - q[1][3];
            p.curveTo(cx, cy, q[2][0], q[2][1], q[2][2], q[2][3]);

            double cx2 = q[2][2] * 2 - q[2][0];
            double cy2 = q[2][3] * 2 - q[2][1];
            p.curveTo(cx2, cy2, q[3][0], q[3][1], q[3][2], q[3][3]);

            AffineTransform at = new AffineTransform();
            at.translate(x, y);
            at.scale(sx, k);
            at.translate(20, -285);

            Shape sh = at.createTransformedShape(p);
            g.setColor(new Color(colors[i]));
            g.setStroke(new BasicStroke((float) (strokeWidths[i] * k), BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
            g.draw(sh);
        }
        g.dispose();
    }

    /**
     * Renders a simulated single LED diffuser cell with fine lattice mesh and surrounding glow.
     */
    static void ledCell(Graphics2D g, int x, int y, int w, int h, int rgb) {
        int r = (rgb >> 16) & 0xFF;
        int gr = (rgb >> 8) & 0xFF;
        int b = rgb & 0xFF;
        int peak = Math.max(r, Math.max(gr, b));
        double luma = peak / 255.0;

        Color fullHue = (peak == 0)
                ? Color.BLACK
                : new Color(r * 255 / peak, gr * 255 / peak, b * 255 / peak);

        RoundRectangle2D.Double box = new RoundRectangle2D.Double(x, y, w, h, 6, 6);

        if (luma > 0.03) {
            // Multi-pass soft ambient glow
            for (int i = 3; i >= 1; i--) {
                g.setColor(new Color(fullHue.getRed(), fullHue.getGreen(), fullHue.getBlue(), (int) (28 * luma)));
                g.fill(new RoundRectangle2D.Double(x - i * 2, y - i * 2, w + i * 4, h + i * 4, 6 + i * 3, 6 + i * 3));
            }
        }

        g.setColor(new Color(0x0C0C10));
        g.fill(box);

        Shape oldClip = g.getClip();
        g.clip(box);

        g.setColor(ConsoleTheme.mix(new Color(0x16161C), fullHue, Math.pow(luma, 0.7)));
        g.fill(box);

        // Fine diffuser lattice grid lines
        g.setColor(new Color(0, 0, 0, 215));
        g.setStroke(new BasicStroke(1.2f));
        int step = Math.max(4, w / 6);
        for (int gx = x + step; gx < x + w; gx += step) {
            g.draw(new Line2D.Double(gx, y, gx, y + h));
        }
        for (int gy = y + step; gy < y + h; gy += step) {
            g.draw(new Line2D.Double(x, gy, x + w, gy));
        }

        g.setClip(oldClip);
        g.setColor(new Color(0x3A3A44));
        g.setStroke(new BasicStroke(1.0f));
        g.draw(box);
    }

    /**
     * Rounded console push-button with high-contrast text and hover/press tactile animations.
     */
    static class BlockButton extends JButton {
        Color base;

        BlockButton(String text, Color base) {
            super(text);
            this.base = base;
            setContentAreaFilled(false);
            setBorderPainted(false);
            setFocusPainted(false);
            setOpaque(false);
            setRolloverEnabled(true);
            setFont(ConsoleTheme.sansBold(14f));
            setForeground(ConsoleTheme.TEXT_ON_BLOCK);
            setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
            setMargin(new Insets(0, 0, 0, 0));
            setPreferredSize(new Dimension(100, ConsoleTheme.BLOCK_H + ConsoleTheme.GAP));
            setMaximumSize(new Dimension(Integer.MAX_VALUE, ConsoleTheme.BLOCK_H + ConsoleTheme.GAP));
            setAlignmentX(Component.LEFT_ALIGNMENT);
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            quality(g);

            boolean enabled = isEnabled();
            boolean pressed = getModel().isPressed();
            boolean rollover = getModel().isRollover();

            Color fill = !enabled
                    ? ConsoleTheme.mix(base, ConsoleTheme.BLACK, 0.78)
                    : pressed
                    ? ConsoleTheme.darker(base)
                    : rollover
                    ? ConsoleTheme.lighter(base)
                    : base;

            g.setColor(fill);
            int gp = ConsoleTheme.GAP / 2;
            int w = getWidth();
            int h = getHeight() - 2 * gp;
            int r = ConsoleTheme.BLOCK_R;

            g.translate(0, gp);
            g.fill(new RoundRectangle2D.Double(0, 0, w, h, r * 2, r * 2));

            if (hasFocus() && enabled) {
                g.setColor(ConsoleTheme.PEACH);
                g.setStroke(new BasicStroke(2.0f));
                g.draw(new RoundRectangle2D.Double(1.5, 1.5, w - 3, h - 3, r * 2, r * 2));
            }

            g.setFont(getFont());
            g.setColor(enabled ? ConsoleTheme.TEXT_ON_BLOCK : ConsoleTheme.mix(base, ConsoleTheme.BLACK, 0.45));
            FontMetrics fm = g.getFontMetrics();
            int tw = fm.stringWidth(getText());
            int nudge = pressed ? 1 : 0;
            g.drawString(getText(), w - ConsoleTheme.PAD - tw + nudge, h - 8 + nudge);
            g.dispose();
        }
    }

    /**
     * Compact console button displaying a vector symbol icon.
     */
    static class IconButton extends BlockButton {
        enum Kind {
            TURN_RIGHT, TURN_LEFT, TURN_HALF, MIRROR_H, MIRROR_V,
            CAMERA, MIC, SPEAKER, PLAY, PAUSE, PREV, NEXT,
            STOP, CANCEL, DEMOS, LOG, LAPTOP, GEAR
        }

        private final Kind kind;
        private final Color normalColor;

        IconButton(Kind kind, String tooltip, Color base) {
            super("", base);
            this.kind = kind;
            this.normalColor = base;
            setToolTipText(tooltip);
            getAccessibleContext().setAccessibleName(tooltip);
            setPreferredSize(new Dimension(40, 30));
            setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        }

        void setActive(boolean on) {
            this.base = on ? ConsoleTheme.AMBER : normalColor;
            repaint();
        }

        @Override
        protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            Graphics2D g = (Graphics2D) g0.create();
            quality(g);

            double cx = getWidth() / 2.0;
            double cy = getHeight() / 2.0;
            boolean enabled = isEnabled();
            boolean pressed = getModel().isPressed();

            g.setColor(enabled ? ConsoleTheme.TEXT_ON_BLOCK : ConsoleTheme.mix(base, ConsoleTheme.BLACK, 0.45));
            g.setStroke(new BasicStroke(2.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
            g.translate(cx + (pressed ? 1 : 0), cy + (pressed ? 1 : 0));

            switch (kind) {
                case TURN_RIGHT, TURN_LEFT, TURN_HALF -> {
                    double r = (kind == Kind.TURN_HALF) ? 9 : 7;
                    if (kind == Kind.TURN_LEFT) {
                        g.scale(-1, 1);
                    }
                    g.draw(new Arc2D.Double(-r, -r, 2 * r, 2 * r, 100, -300, Arc2D.OPEN));

                    double th = Math.toRadians(160);
                    double ex = r * Math.cos(th);
                    double ey = -r * Math.sin(th);
                    double dx = Math.sin(th);
                    double dy = Math.cos(th);
                    double tx = ex + dx * 3;
                    double ty = ey + dy * 3;
                    double nx = -dy;
                    double ny = dx;

                    Path2D.Double arrow = new Path2D.Double();
                    arrow.moveTo(tx - dx * 5 + nx * 4, ty - dy * 5 + ny * 4);
                    arrow.lineTo(tx, ty);
                    arrow.lineTo(tx - dx * 5 - nx * 4, ty - dy * 5 - ny * 4);
                    g.draw(arrow);

                    if (kind == Kind.TURN_HALF) {
                        g.setFont(ConsoleTheme.sansBold(8f));
                        g.drawString("180", -g.getFontMetrics().stringWidth("180") / 2f, 3);
                    }
                }
                case PLAY -> {
                    Path2D.Double t = new Path2D.Double();
                    t.moveTo(-4, -7);
                    t.lineTo(-4, 7);
                    t.lineTo(7, 0);
                    t.closePath();
                    g.fill(t);
                }
                case PAUSE -> {
                    g.fill(new RoundRectangle2D.Double(-6, -7, 4, 14, 2, 2));
                    g.fill(new RoundRectangle2D.Double(2, -7, 4, 14, 2, 2));
                }
                case PREV -> {
                    g.fill(new RoundRectangle2D.Double(-8, -7, 2.5, 14, 1, 1));
                    Path2D.Double t = new Path2D.Double();
                    t.moveTo(7, -7);
                    t.lineTo(7, 7);
                    t.lineTo(-4, 0);
                    t.closePath();
                    g.fill(t);
                }
                case NEXT -> {
                    g.fill(new RoundRectangle2D.Double(5.5, -7, 2.5, 14, 1, 1));
                    Path2D.Double t = new Path2D.Double();
                    t.moveTo(-7, -7);
                    t.lineTo(-7, 7);
                    t.lineTo(4, 0);
                    t.closePath();
                    g.fill(t);
                }
                case STOP -> g.fill(new RoundRectangle2D.Double(-6, -6, 12, 12, 3, 3));
                case CANCEL -> {
                    g.draw(new Line2D.Double(-6, -6, 6, 6));
                    g.draw(new Line2D.Double(6, -6, -6, 6));
                }
                case DEMOS -> {
                    for (int i = 0; i < 4; i++) {
                        g.fill(new RoundRectangle2D.Double(i % 2 == 0 ? -8 : 1, i < 2 ? -8 : 1, 7, 7, 2, 2));
                    }
                }
                case LOG -> {
                    for (int i = 0; i < 3; i++) {
                        g.fill(new Ellipse2D.Double(-9, i * 6 - 8, 3, 3));
                        g.draw(new Line2D.Double(-3.5, i * 6 - 6.5, 8, i * 6 - 6.5));
                    }
                }
                case LAPTOP -> {
                    g.draw(new RoundRectangle2D.Double(-7, -7, 14, 9, 2, 2));
                    g.draw(new Line2D.Double(-10, 5, 10, 5));
                }
                case GEAR -> {
                    g.draw(new Ellipse2D.Double(-4, -4, 8, 8));
                    for (int k = 0; k < 8; k++) {
                        double a = k * Math.PI / 4;
                        g.draw(new Line2D.Double(5.5 * Math.cos(a), 5.5 * Math.sin(a), 8 * Math.cos(a), 8 * Math.sin(a)));
                    }
                }
                case CAMERA -> {
                    g.draw(new RoundRectangle2D.Double(-9, -5, 18, 12, 4, 4));
                    g.draw(new Ellipse2D.Double(-3.5, -2.5, 7, 7));
                    g.fill(new RoundRectangle2D.Double(-4, -8, 8, 3, 2, 2));
                }
                case MIC -> {
                    g.draw(new RoundRectangle2D.Double(-3, -9, 6, 11, 6, 6));
                    g.draw(new Arc2D.Double(-6.5, -6, 13, 13, 180, 180, Arc2D.OPEN));
                    g.draw(new Line2D.Double(0, 7, 0, 10));
                    g.draw(new Line2D.Double(-4, 10, 4, 10));
                }
                case SPEAKER -> {
                    Path2D.Double cone = new Path2D.Double();
                    cone.moveTo(-9, -3);
                    cone.lineTo(-5, -3);
                    cone.lineTo(0, -7);
                    cone.lineTo(0, 7);
                    cone.lineTo(-5, 3);
                    cone.lineTo(-9, 3);
                    cone.closePath();
                    g.fill(cone);
                    g.draw(new Arc2D.Double(-3, -4, 8, 8, -60, 120, Arc2D.OPEN));
                    g.draw(new Arc2D.Double(-3, -8, 16, 16, -60, 120, Arc2D.OPEN));
                }
                case MIRROR_H, MIRROR_V -> {
                    if (kind == Kind.MIRROR_V) {
                        g.rotate(Math.PI / 2);
                    }
                    Path2D.Double l = new Path2D.Double();
                    l.moveTo(-3, -6);
                    l.lineTo(-9, 6);
                    l.lineTo(-3, 6);
                    l.closePath();

                    Path2D.Double r2 = new Path2D.Double();
                    r2.moveTo(3, -6);
                    r2.lineTo(9, 6);
                    r2.lineTo(3, 6);
                    r2.closePath();

                    g.fill(r2);
                    g.draw(l);
                    g.setStroke(new BasicStroke(1.5f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER, 10f, new float[]{2.5f, 2.5f}, 0f));
                    g.draw(new Line2D.Double(0, -8, 0, 8));
                }
            }
            g.dispose();
        }
    }

    /**
     * Retro scientific instrument LCD display component showing digital readouts and LED dot bar meters.
     */
    static class Lcd extends JComponent {
        record Reading(double value, double max, String text, Color color) {}

        private final Map<String, Reading> readings = new LinkedHashMap<>();
        private double talkFraction = -1.0;
        private double slideFraction = 0.0;
        private String talkText = "";
        private String talkLabel = "talk";

        Lcd() {
            setOpaque(false);
            setPreferredSize(new Dimension(100, 48));
        }

        void set(String label, double value, double max, String text, Color colour) {
            readings.remove(label);
            readings.put(label, new Reading(value, max, text, colour));
            while (readings.size() > 3) {
                readings.remove(readings.keySet().iterator().next());
            }
            repaint();
        }

        void talk(double fraction, String text, double slideFrac, String label) {
            this.talkFraction = fraction;
            this.talkText = text;
            this.slideFraction = slideFrac;
            this.talkLabel = label.isEmpty() ? "talk" : label;
            repaint();
        }

        void clearAll() {
            readings.clear();
            talkFraction = -1;
            repaint();
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            quality(g);
            int w = getWidth();
            int h = getHeight();

            g.setColor(new Color(0x0C0C14));
            g.fill(new RoundRectangle2D.Double(0, 0, w, h, 16, 16));
            g.setColor(ConsoleTheme.mix(ConsoleTheme.LAVENDER, ConsoleTheme.BLACK, 0.75));
            g.setStroke(new BasicStroke(1.2f));
            g.draw(new RoundRectangle2D.Double(0.6, 0.6, w - 1.2, h - 1.2, 16, 16));

            List<Reading> cells = new ArrayList<>();
            List<String> names = new ArrayList<>();

            if (talkFraction >= 0) {
                cells.add(new Reading(talkFraction, 1.0, talkText, ConsoleTheme.LAVENDER));
                names.add(talkLabel);
            }
            for (var entry : readings.entrySet()) {
                cells.add(entry.getValue());
                names.add(entry.getKey());
            }

            if (cells.isEmpty()) {
                drawMeter(g, 14, h - 20, w - 28, 3, 0, ConsoleTheme.PEACH);
                g.dispose();
                return;
            }

            int n = cells.size();
            int pad = 14;
            double unit = (w - pad) / (n + (talkFraction >= 0 ? 0.6 : 0));
            double curX = pad;

            for (int i = 0; i < n; i++) {
                Reading reading = cells.get(i);
                double span = (i == 0 && talkFraction >= 0) ? unit * 1.6 : unit;
                int x = (int) curX;
                int cellW = (int) span;
                int meterW = cellW - pad;
                curX += span;

                double frac = Math.clamp(reading.value() / reading.max(), 0.0, 1.0);
                Color col = reading.color();
                String text = reading.text();

                g.setFont(ConsoleTheme.mono(15f));
                FontMetrics fm = g.getFontMetrics();
                int tx = x + meterW - fm.stringWidth(text);

                g.setFont(ConsoleTheme.sansRegular(11f));
                FontMetrics labelFm = g.getFontMetrics();
                int availableLabelRoom = tx - x - 8;

                List<String> forms = new ArrayList<>(Arrays.asList(names.get(i).split("\\|")));
                if (forms.size() == 1 && forms.getFirst().contains(" ")) {
                    forms.add(forms.getFirst().substring(0, forms.getFirst().indexOf(' ')));
                }

                for (String labelCandidate : forms) {
                    if (labelFm.stringWidth(labelCandidate) <= availableLabelRoom) {
                        g.setColor(ConsoleTheme.mix(ConsoleTheme.PEACH, ConsoleTheme.BLACK, 0.35));
                        g.drawString(labelCandidate, x, 15);
                        break;
                    }
                }

                // Render faint unlit 8-digit ghost segments followed by lit digits
                g.setFont(ConsoleTheme.mono(15f));
                g.setColor(new Color(col.getRed(), col.getGreen(), col.getBlue(), 28));
                g.drawString(text.replaceAll("[0-9A-Za-z]", "8"), tx, 17);
                g.setColor(col);
                g.drawString(text, tx, 17);

                if (i == 0 && talkFraction >= 0) {
                    drawMeter(g, x, 24, meterW, 1, slideFraction, ConsoleTheme.PEACH);
                    drawMeter(g, x, 32, meterW, 2, frac, col);
                } else {
                    drawMeter(g, x, h - 19, meterW, 3, frac, col);
                }
            }
            g.dispose();
        }

        private static void drawMeter(Graphics2D g, int x, int y, int w, int rows, double frac, Color col) {
            int cellSize = 4;
            int step = 5;
            int cols = Math.max(1, w / step);
            int litCols = (int) Math.round(frac * cols);

            for (int c = 0; c < cols; c++) {
                for (int r = 0; r < rows; r++) {
                    Color fill = (c < litCols)
                            ? ConsoleTheme.mix(col, ConsoleTheme.BLACK, 0.12)
                            : ConsoleTheme.mix(col, ConsoleTheme.BLACK, 0.90);
                    g.setColor(fill);
                    g.fillRect(x + c * step, y + r * step, cellSize, cellSize);
                }
            }
        }
    }

    /**
     * Pill-shaped status bar with label and animated pulse dots.
     */
    static class PillBar extends JComponent {
        private String text = "";
        private Color fill;
        private boolean busy = false;
        private int tickCount = 0;
        private final Timer animationTimer;

        PillBar(Color fill) {
            this.fill = fill;
            setOpaque(false);
            setPreferredSize(new Dimension(100, 30));
            this.animationTimer = new Timer(120, _ -> {
                tickCount++;
                repaint();
            });
        }

        void setText(String t) {
            this.text = t;
            repaint();
        }

        void setBusy(boolean isBusy) {
            this.busy = isBusy;
            if (isBusy) {
                animationTimer.start();
            } else {
                animationTimer.stop();
            }
            repaint();
        }

        void setFill(Color c) {
            this.fill = c;
            repaint();
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            quality(g);
            int w = getWidth();
            int h = getHeight();

            g.setColor(fill);
            g.fill(new RoundRectangle2D.Double(0, 0, w, h, h, h));

            g.setFont(ConsoleTheme.mono(13f));
            g.setColor(ConsoleTheme.BLACK);
            FontMetrics fm = g.getFontMetrics();

            String displayStr = text;
            int room = w - h - (busy ? 6 * 16 + h / 2 : 0);
            while (displayStr.length() > 1 && fm.stringWidth(displayStr) > room) {
                displayStr = displayStr.substring(0, displayStr.length() - 2) + "…";
            }

            g.drawString(displayStr, h / 2 + 2, (h + fm.getAscent() - fm.getDescent()) / 2);

            if (busy) {
                for (int i = 0; i < 6; i++) {
                    double darkness = (i == (tickCount % 6)) ? 0.0 : 0.55;
                    g.setColor(ConsoleTheme.mix(fill, ConsoleTheme.BLACK, darkness));
                    g.fill(new RoundRectangle2D.Double(w - h / 2 - 6 * 16 + i * 16 - 6, h / 2 - 5, 12, 10, 8, 8));
                }
            }
            g.dispose();
        }
    }

    /**
     * Group header band with trailing accent rule.
     */
    static class Band extends JComponent {
        private final String text;
        private final Color color;

        Band(String text, Color color) {
            this.text = text;
            this.color = color;
            setOpaque(false);
            setPreferredSize(new Dimension(100, 22));
            setMaximumSize(new Dimension(Integer.MAX_VALUE, 22));
            setAlignmentX(Component.LEFT_ALIGNMENT);
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            quality(g);

            g.setFont(ConsoleTheme.sansBold(13f));
            g.setColor(color);
            g.drawString(text, 4, getHeight() - 6);

            int textWidth = g.getFontMetrics().stringWidth(text);
            g.setColor(ConsoleTheme.mix(color, ConsoleTheme.BLACK, 0.7));
            int ruleStart = textWidth + 12;
            g.fillRect(ruleStart, getHeight() - 10, Math.max(0, getWidth() - ruleStart), 2);
            g.dispose();
        }
    }

    /**
     * Rounded outline container frame with integrated title badge.
     */
    static class Frame extends JPanel {
        private final String title;
        private final Color color;

        Frame(String title, Color color) {
            this.title = title;
            this.color = color;
            setOpaque(false);
            setBorder(BorderFactory.createEmptyBorder(28, 12, 12, 12));
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            quality(g);

            g.setColor(ConsoleTheme.mix(color, ConsoleTheme.BLACK, 0.55));
            g.setStroke(new BasicStroke(1.5f));
            g.draw(new RoundRectangle2D.Double(1, 9, getWidth() - 3, getHeight() - 11, 16, 16));

            g.setFont(ConsoleTheme.sansBold(12f));
            int textW = g.getFontMetrics().stringWidth(title);
            g.setColor(ConsoleTheme.BLACK);
            g.fillRect(14, 0, textW + 12, 20);

            g.setColor(color);
            g.drawString(title, 20, 14);
            g.dispose();

            super.paintComponent(g0);
        }
    }
}
