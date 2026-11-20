import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.LayoutManager;
import java.awt.RenderingHints;
import java.awt.geom.AffineTransform;
import java.awt.geom.RoundRectangle2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * High-level webcam device manager and Swing presentation components.
 * <p>
 * Manages video capture sessions powered by {@link V4l2}. Features:
 * <ul>
 *     <li>Auto-discovery of Video4Linux2 capture nodes via sysfs (filtering out metadata streams).</li>
 *     <li>Shared multi-subscriber capture per physical device (enabling simultaneous previews
 *         without reopening device nodes).</li>
 *     <li>On-demand activation: unticking a camera stops capture and releases the kernel device node
 *         instantly so other host applications (e.g. video conferencing) can use it.</li>
 *     <li>Dynamic aspect-aware layout and hardware transformation (rotation, mirroring, flipping).</li>
 * </ul>
 * </p>
 */
final class Cams {

    private Cams() {
        // Non-instantiable utility class
    }

    /**
     * Information record describing an enumerated video capture device.
     *
     * @param node filesystem path to the device node (e.g. {@code "/dev/video0"})
     * @param name human-readable device model name reported by the driver
     */
    record Device(String node, String name) {
        @Override
        public String toString() {
            return name + " (" + node + ")";
        }
    }

    /**
     * Discovers all primary Video4Linux2 capture devices present on the system.
     * <p>
     * Scans {@code /sys/class/video4linux/}, selecting index 0 nodes and filtering out
     * secondary metadata nodes created by modern UVC drivers.
     * </p>
     *
     * @return list of enumerated capture devices
     */
    static List<Device> list() {
        List<Device> devices = new ArrayList<>();
        Path basePath = Path.of("/sys/class/video4linux");
        if (!Files.isDirectory(basePath)) {
            return devices;
        }

        try (var stream = Files.list(basePath)) {
            stream.sorted().forEach(dir -> {
                try {
                    String index = Files.readString(dir.resolve("index")).trim();
                    if (!index.equals("0")) {
                        return; // Skip secondary/metadata streams
                    }
                    String name = Files.readString(dir.resolve("name")).trim();
                    devices.add(new Device("/dev/" + dir.getFileName(), name));
                } catch (IOException ignored) {
                    // Skip nodes that cannot be read
                }
            });
        } catch (IOException ignored) {
            // Sysfs access failed
        }
        return devices;
    }

    /** Registry of currently open hardware capture pipelines, keyed by device node path. */
    private static final Map<String, SharedCapture> OPEN_CAPTURES = new HashMap<>();

    /**
     * Returns the number of currently active video streaming pipelines.
     * Test suites verify that only necessary streams run concurrently.
     *
     * @return active capture stream count
     */
    static int openCount() {
        synchronized (OPEN_CAPTURES) {
            return OPEN_CAPTURES.size();
        }
    }

    /**
     * Internal reference-counted background streaming pipeline for a single hardware device.
     */
    private static final class SharedCapture {
        final String node;
        volatile BufferedImage latestFrame;
        volatile String errorProblem;
        final List<Runnable> frameListeners = new CopyOnWriteArrayList<>();
        private final AtomicBoolean stopRequested = new AtomicBoolean(false);
        private final Thread captureThread;

        SharedCapture(String node, int width, int height) {
            this.node = node;
            // Platform daemon thread: V4L2 streaming blocks inside native poll/ioctl
            this.captureThread = Thread.ofPlatform().daemon().name("v4l2-cam-" + node).start(() -> {
                try (V4l2 v4l2 = new V4l2(node, width, height)) {
                    while (!stopRequested.get()) {
                        BufferedImage frame = v4l2.next(300, 60);
                        if (frame != null) {
                            latestFrame = frame;
                            errorProblem = null;
                            notifyListeners();
                        }
                    }
                } catch (IOException e) {
                    if (!stopRequested.get()) {
                        errorProblem = e.getMessage();
                        notifyListeners();
                    }
                }
            });
        }

        private void notifyListeners() {
            for (Runnable listener : frameListeners) {
                try {
                    listener.run();
                } catch (RuntimeException ignored) {
                    // Suppress exceptions from listener callbacks
                }
            }
        }

        void terminate() {
            stopRequested.set(true);
            latestFrame = null;
        }
    }

    /**
     * Subscriber handle to an active camera capture session.
     */
    static final class Cam implements AutoCloseable {
        final String node;
        private final SharedCapture shared;
        private final Runnable onFrame;
        private boolean closed = false;

        Cam(String node, Runnable onFrame) {
            this(node, 640, 360, onFrame);
        }

        Cam(String node, int width, int height, Runnable onFrame) {
            this.node = node;
            this.onFrame = onFrame;
            synchronized (OPEN_CAPTURES) {
                this.shared = OPEN_CAPTURES.computeIfAbsent(
                        node,
                        n -> new SharedCapture(n, width, height)
                );
                this.shared.frameListeners.add(onFrame);
            }
        }

        /** Returns the most recently decoded video frame, or {@code null} if none is ready. */
        BufferedImage frame() {
            return shared.latestFrame;
        }

        /** Returns error or diagnostic message if the camera cannot be captured. */
        String problem() {
            return shared.errorProblem;
        }

        @Override
        public void close() {
            synchronized (OPEN_CAPTURES) {
                if (closed) return;
                closed = true;
                shared.frameListeners.remove(onFrame);
                if (shared.frameListeners.isEmpty()) {
                    OPEN_CAPTURES.remove(node, shared);
                    shared.terminate();
                }
            }
        }
    }

    /**
     * Swing panel displaying a scaled, rotated, and mirrored camera preview feed.
     */
    static final class CamPanel extends JPanel {
        volatile Cam cam;
        volatile String text = "off";
        volatile int quarter = 0; // Quarter turns clockwise (0..3)
        volatile boolean mirror = false;
        volatile boolean flip = false;

        private final AtomicBoolean repaintPending = new AtomicBoolean(false);

        CamPanel() {
            setOpaque(false);
        }

        /**
         * Calculates the effective width-to-height aspect ratio considering 90&deg; rotations.
         *
         * @return effective aspect ratio (defaults to 16:9 until a frame arrives)
         */
        double aspect() {
            Cam c = cam;
            BufferedImage img = (c == null) ? null : c.frame();
            double nominalAspect = (img == null) ? 16.0 / 9.0 : (double) img.getWidth() / img.getHeight();
            return ((quarter & 1) == 1) ? 1.0 / nominalAspect : nominalAspect;
        }

        /** Schedules a coalesced UI repaint on the Swing Event Dispatch Thread. */
        void refresh() {
            if (!repaintPending.getAndSet(true)) {
                SwingUtilities.invokeLater(() -> {
                    repaintPending.set(false);
                    repaint();
                });
            }
        }

        @Override
        protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create();
            ConsoleUi.quality(g);

            Cam c = cam;
            BufferedImage img = (c == null) ? null : c.frame();
            int panelW = getWidth();
            int panelH = getHeight();

            if (img == null) {
                g.setColor(ConsoleTheme.mix(ConsoleTheme.PEACH, ConsoleTheme.BLACK, 0.5));
                g.setFont(ConsoleTheme.mono(12f));
                String message = (c != null && c.problem() != null)
                        ? c.problem()
                        : (c != null ? "waiting for the camera …" : text);
                g.drawString(message, 8, panelH / 2);
            } else {
                int imgW = img.getWidth();
                int imgH = img.getHeight();
                int turns = quarter & 3;
                boolean isTransposed = ((turns & 1) == 1);

                int rotatedW = isTransposed ? imgH : imgW;
                int rotatedH = isTransposed ? imgW : imgH;

                double scaleFactor = Math.min((double) panelW / rotatedW, (double) panelH / rotatedH);
                int previewW = (int) (rotatedW * scaleFactor);
                int previewH = (int) (rotatedH * scaleFactor);

                g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
                g.setClip(new RoundRectangle2D.Double(
                        (panelW - previewW) / 2.0,
                        (panelH - previewH) / 2.0,
                        previewW,
                        previewH,
                        14,
                        14
                ));

                AffineTransform transform = new AffineTransform();
                transform.translate(panelW / 2.0, panelH / 2.0);
                if (mirror) transform.scale(-1, 1);
                if (flip) transform.scale(1, -1);
                transform.rotate(turns * Math.PI / 2.0);
                transform.scale(scaleFactor, scaleFactor);
                transform.translate(-imgW / 2.0, -imgH / 2.0);

                g.drawImage(img, transform, null);
            }
            g.dispose();
        }
    }

    /**
     * Custom LayoutManager arranging multiple camera preview panels to maximize screen area.
     * Evaluates whether horizontal or vertical tiling yields the largest total preview size.
     */
    static final class CamLayout implements LayoutManager {
        private final int gap;

        CamLayout(int gap) {
            this.gap = gap;
        }

        @Override public void addLayoutComponent(String name, Component comp) {}
        @Override public void removeLayoutComponent(Component comp) {}
        @Override public Dimension preferredLayoutSize(Container parent) { return new Dimension(10, 10); }
        @Override public Dimension minimumLayoutSize(Container parent) { return new Dimension(10, 10); }

        private static double effectiveArea(double maxW, double maxH, double aspect) {
            double w = Math.min(maxW, maxH * aspect);
            return w * (w / aspect);
        }

        @Override
        public void layoutContainer(Container parent) {
            List<CamPanel> visiblePanels = new ArrayList<>();
            for (Component comp : parent.getComponents()) {
                if (comp.isVisible() && comp instanceof CamPanel panel) {
                    visiblePanels.add(panel);
                }
            }

            Insets insets = parent.getInsets();
            int availW = parent.getWidth() - insets.left - insets.right;
            int availH = parent.getHeight() - insets.top - insets.bottom;

            if (visiblePanels.isEmpty() || availW <= 0 || availH <= 0) {
                return;
            }

            if (visiblePanels.size() == 1) {
                visiblePanels.getFirst().setBounds(insets.left, insets.top, availW, availH);
                return;
            }

            // Compare horizontal split vs vertical split total visible area
            double cellW = (availW - gap) / 2.0;
            double cellH = (availH - gap) / 2.0;
            double rowScore = 0.0;
            double colScore = 0.0;

            for (CamPanel panel : visiblePanels) {
                rowScore += effectiveArea(cellW, availH, panel.aspect());
                colScore += effectiveArea(availW, cellH, panel.aspect());
            }

            if (rowScore >= colScore) {
                // Side-by-side horizontal tiling
                int currentX = insets.left;
                int intCellW = (int) cellW;
                for (CamPanel panel : visiblePanels) {
                    panel.setBounds(currentX, insets.top, intCellW, availH);
                    currentX += intCellW + gap;
                }
            } else {
                // Stacked vertical tiling
                int currentY = insets.top;
                int intCellH = (int) cellH;
                for (CamPanel panel : visiblePanels) {
                    panel.setBounds(insets.left, currentY, availW, intCellH);
                    currentY += intCellH + gap;
                }
            }
        }
    }
}
