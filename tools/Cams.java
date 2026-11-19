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
 * Webcams for the demo window, through ffmpeg and v4l2 (the same route as tools/LedCam.java; no extra library). A camera is only opened while its box is
 * ticked: unticking it ends the ffmpeg process and frees the device at once, so the laptop camera can go to Teams or another call. A camera that is not
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

    /** One running camera: an ffmpeg process that writes JPEG frames to a pipe, read on a virtual thread. */
    static final class Cam implements AutoCloseable {
        final String node; volatile BufferedImage frame; volatile String problem;
        private Process proc; private final AtomicBoolean stop = new AtomicBoolean(); private final Runnable onFrame;
        Cam(String node, Runnable onFrame) { this.node = node; this.onFrame = onFrame; start(true); }

        private void start(boolean mjpegIn) {
            try {
                List<String> cmd = new ArrayList<>(List.of("ffmpeg", "-loglevel", "error", "-f", "v4l2"));
                if (mjpegIn) cmd.addAll(List.of("-input_format", "mjpeg"));
                cmd.addAll(List.of("-video_size", "640x360", "-framerate", "15", "-i", node, "-an", "-c:v", mjpegIn ? "copy" : "mjpeg", "-q:v", "6", "-f", "image2pipe", "-"));
                proc = new ProcessBuilder(cmd).redirectError(ProcessBuilder.Redirect.DISCARD).start();
            } catch (IOException e) { problem = "ffmpeg is not available"; return; }
            Process p = proc;
            Thread.ofVirtual().name("cam " + node).start(() -> {
                long got = 0;
                try (InputStream in = new BufferedInputStream(p.getInputStream(), 1 << 16)) {
                    ByteArrayOutputStream buf = new ByteArrayOutputStream(1 << 16);
                    int prev = -1, b;
                    while (!stop.get() && (b = in.read()) >= 0) {
                        if (prev == 0xFF && b == 0xD8) { buf.reset(); buf.write(0xFF); }
                        buf.write(b);
                        if (prev == 0xFF && b == 0xD9 && buf.size() > 4) {
                            BufferedImage im = ImageIO.read(new ByteArrayInputStream(buf.toByteArray()));
                            if (im != null) { frame = im; problem = null; got++; onFrame.run(); }
                            buf.reset();
                        }
                        prev = b;
                    }
                } catch (IOException ignored) { }
                if (!stop.get() && got == 0) {
                    if (mjpegIn) { start(false); return; }          // the camera does not do MJPEG: let ffmpeg convert it
                    problem = "cannot open " + node + " (in use?)";
                    onFrame.run();
                }
            });
        }

        @Override public void close() {
            stop.set(true);
            Process p = proc; if (p != null) p.destroy();          // the device is free again once ffmpeg has gone
            frame = null;
        }
    }

    /** A picture panel for one camera: the latest frame scaled to fit, or a short line of text. */
    static final class CamPanel extends JPanel {
        volatile Cam cam; volatile String text = "off";
        volatile int quarter; volatile boolean mirror;      // quarter turns clockwise (0..3) and a left-right mirror, applied when drawing
        private final AtomicBoolean pending = new AtomicBoolean();
        CamPanel() { setOpaque(false); }
        void refresh() { if (!pending.getAndSet(true)) SwingUtilities.invokeLater(() -> { pending.set(false); repaint(); }); }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            Cam c = cam; BufferedImage im = c == null ? null : c.frame;
            int w = getWidth(), h = getHeight();
            if (im == null) {
                g.setColor(ConsoleTheme.mix(ConsoleTheme.PEACH, ConsoleTheme.BLACK, 0.5)); g.setFont(ConsoleTheme.mono(12f));
                String t = c != null && c.problem != null ? c.problem : c != null ? "waiting for the camera …" : text;
                g.drawString(t, 8, h / 2);
            } else {
                int iw = im.getWidth(), ih = im.getHeight(), q = quarter & 3; boolean odd = (q & 1) == 1;
                int rw = odd ? ih : iw, rh = odd ? iw : ih;                       // size after the turn
                double k = Math.min((double) w / rw, (double) h / rh);
                int pw = (int) (rw * k), ph = (int) (rh * k);
                g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
                g.setClip(new java.awt.geom.RoundRectangle2D.Double((w - pw) / 2.0, (h - ph) / 2.0, pw, ph, 14, 14));
                java.awt.geom.AffineTransform at = new java.awt.geom.AffineTransform();
                at.translate(w / 2.0, h / 2.0); if (mirror) at.scale(-1, 1); at.rotate(q * Math.PI / 2); at.scale(k, k); at.translate(-iw / 2.0, -ih / 2.0);
                g.drawImage(im, at, null);
            }
            g.dispose();
        }
    }
}
