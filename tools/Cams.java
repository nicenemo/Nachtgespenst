import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Webcams for the demo window, through Video4Linux2 in plain Java (V4l2.java, the foreign function API; no ffmpeg and no native library). A camera is only opened while its box is
 * ticked: unticking it stops the capture and frees the device at once, so the laptop camera can go to Teams or another call. A camera that is not
 * there is not an error: the panel just says so.
 */
final class Cams {
    record Device(String node, String name) {
        @Override public String toString() { return name + " (" + node + ")"; }
    }

    /** The capture devices of the machine (index 0 of every camera; the other nodes of a camera carry metadata). */
    static List<Device> list() {
        List<Device> out = new ArrayList<>();
        Path base = Path.of("/sys/class/video4linux");
        if (!Files.isDirectory(base)) return out;
        try (var s = Files.list(base)) {
            s.sorted().forEach(d -> {
                try {
                    String idx = Files.readString(d.resolve("index")).trim();
                    if (!idx.equals("0")) return;
                    String name = Files.readString(d.resolve("name")).trim();
                    out.add(new Device("/dev/" + d.getFileName(), name));
                } catch (IOException ignored) { }
            });
        } catch (IOException ignored) { }
        return out;
    }

    /** One capture per device, shared by everything that has the device open: the same camera in two panels is no problem. */
    private static final Map<String, Shared> OPEN = new HashMap<>();
    /** How many cameras are being read at this moment (one capture per device): the tests check that only one video stream runs. */
    static int openCount() { synchronized (OPEN) { return OPEN.size(); } }

    private static final class Shared {
        final String node; volatile BufferedImage frame; volatile String problem;
        final List<Runnable> listeners = new java.util.concurrent.CopyOnWriteArrayList<>();
        private final AtomicBoolean stop = new AtomicBoolean(); private Thread thread;
        Shared(String node) {
            this.node = node;
            thread = Thread.ofPlatform().daemon().name("cam " + node).start(() -> {          // a platform thread: the capture sits in native calls
                try {
                    V4l2 v = new V4l2(node, 640, 360);
                    try {
                        while (!stop.get()) {
                            BufferedImage im = v.next(300, 60);
                            if (im != null) { frame = im; problem = null; notifyAll_(); }
                        }
                    } finally { v.close(); }                                                   // the device is free again
                } catch (IOException e) { if (!stop.get()) { problem = e.getMessage(); notifyAll_(); } }
            });
        }
        private void notifyAll_() { for (Runnable r : listeners) { try { r.run(); } catch (RuntimeException ignored) { } } }
        void end() { stop.set(true); frame = null; }
    }

    /** A handle on a camera. Opening never throws: a camera that is missing or busy only has a problem text. */
    static final class Cam implements AutoCloseable {
        final String node; private final Shared sh; private final Runnable onFrame; private boolean closed;
        Cam(String node, Runnable onFrame) {
            this.node = node; this.onFrame = onFrame;
            synchronized (OPEN) { sh = OPEN.computeIfAbsent(node, Shared::new); sh.listeners.add(onFrame); }
        }
        BufferedImage frame() { return sh.frame; }
        String problem() { return sh.problem; }
        @Override public void close() {
            synchronized (OPEN) {
                if (closed) return; closed = true;
                sh.listeners.remove(onFrame);
                if (sh.listeners.isEmpty()) { OPEN.remove(node, sh); sh.end(); }
            }
        }
    }

    /** A picture panel for one camera: the latest frame scaled to fit, or a short line of text. */
    static final class CamPanel extends JPanel {
        volatile Cam cam; volatile String text = "off";
        volatile int quarter; volatile boolean mirror, flip;      // quarter turns clockwise (0..3) and a left-right mirror, applied when drawing
        private final AtomicBoolean pending = new AtomicBoolean();
        CamPanel() { setOpaque(false); }
        /** Width over height of the picture as it is shown (after the turn); 16:9 until a frame has arrived. */
        double aspect() {
            Cam c = cam; BufferedImage im = c == null ? null : c.frame();
            double a = im == null ? 16.0 / 9 : (double) im.getWidth() / im.getHeight();
            return (quarter & 1) == 1 ? 1 / a : a;
        }
        void refresh() { if (!pending.getAndSet(true)) SwingUtilities.invokeLater(() -> { pending.set(false); repaint(); }); }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            Cam c = cam; BufferedImage im = c == null ? null : c.frame();
            int w = getWidth(), h = getHeight();
            if (im == null) {
                g.setColor(ConsoleTheme.mix(ConsoleTheme.PEACH, ConsoleTheme.BLACK, 0.5)); g.setFont(ConsoleTheme.mono(12f));
                String t = c != null && c.problem() != null ? c.problem() : c != null ? "waiting for the camera …" : text;
                g.drawString(t, 8, h / 2);
            } else {
                int iw = im.getWidth(), ih = im.getHeight(), q = quarter & 3; boolean odd = (q & 1) == 1;
                int rw = odd ? ih : iw, rh = odd ? iw : ih;                       // size after the turn
                double k = Math.min((double) w / rw, (double) h / rh);
                int pw = (int) (rw * k), ph = (int) (rh * k);
                g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
                g.setClip(new java.awt.geom.RoundRectangle2D.Double((w - pw) / 2.0, (h - ph) / 2.0, pw, ph, 14, 14));
                java.awt.geom.AffineTransform at = new java.awt.geom.AffineTransform();
                at.translate(w / 2.0, h / 2.0); if (mirror) at.scale(-1, 1); if (flip) at.scale(1, -1); at.rotate(q * Math.PI / 2); at.scale(k, k); at.translate(-iw / 2.0, -ih / 2.0);
                g.drawImage(im, at, null);
            }
            g.dispose();
        }
    }

    /** Lays the visible camera panels side by side or one above the other, whichever shows the pictures larger (a turned camera is tall, not wide). */
    static final class CamLayout implements LayoutManager {
        private final int gap;
        CamLayout(int gap) { this.gap = gap; }
        public void addLayoutComponent(String n, Component c) { }
        public void removeLayoutComponent(Component c) { }
        public Dimension preferredLayoutSize(Container c) { return new Dimension(10, 10); }
        public Dimension minimumLayoutSize(Container c) { return new Dimension(10, 10); }
        private static double area(double cw, double ch, double aspect) { double w = Math.min(cw, ch * aspect); return w * (w / aspect); }
        public void layoutContainer(Container c) {
            List<CamPanel> v = new ArrayList<>(); for (Component k : c.getComponents()) if (k.isVisible() && k instanceof CamPanel p) v.add(p);
            Insets in = c.getInsets(); int w = c.getWidth() - in.left - in.right, h = c.getHeight() - in.top - in.bottom;
            if (v.isEmpty() || w <= 0 || h <= 0) return;
            if (v.size() == 1) { v.get(0).setBounds(in.left, in.top, w, h); return; }
            double cw = (w - gap) / 2.0, ch = (h - gap) / 2.0, row = 0, col = 0;
            for (CamPanel p : v) { row += area(cw, h, p.aspect()); col += area(w, ch, p.aspect()); }
            if (row >= col) { int x = in.left; int cwi = (int) cw; for (CamPanel p : v) { p.setBounds(x, in.top, cwi, h); x += cwi + gap; } }
            else { int y = in.top; int chi = (int) ch; for (CamPanel p : v) { p.setBounds(in.left, y, w, chi); y += chi + gap; } }
        }
    }
}
