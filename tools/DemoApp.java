///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//DEPS com.formdev:flatlaf:3.5.4
//JAVA 27
//SOURCES Boards.java
//SOURCES Libc.java
//SOURCES Hid.java
//SOURCES HidReader.java
//SOURCES Evdev.java
//SOURCES Ser.java
//SOURCES Proto.java
//SOURCES DiskDrive.java
//SOURCES Canvas.java
//SOURCES BarEffects.java
//SOURCES GridEffects.java
//SOURCES Photos.java
//SOURCES Scenarios.java
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;
import com.formdev.flatlaf.FlatDarkLaf;
import com.formdev.flatlaf.FlatLaf;

import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;
import java.util.List;
import java.util.concurrent.Callable;

/**
 * The Nachtgespenst demo as a window: buttons for the whole cycle and for every part (pages, the Jules Verne file to the SD card and back, the LED bar
 * effects, C64 raster bars and sine scroller, the photo slideshow with prime-modulo dissolves). Everything is painted by the PC through the LampArray of
 * the CoreS3 over USB. Each demo runs on a virtual thread; Stop interrupts it. The ghost is the icon of the project site.
 *
 * <pre>
 * jbang tools/DemoApp.java                 start the window
 * jbang tools/DemoApp.java --shot out.png  render the window to a PNG without a board or a display (for the docs)
 * </pre>
 * Start it as ONE plain command (serial port and /dev/hidraw are needed). It never presses Power Down or Sleep.
 */
public class DemoApp {
    static final Color ACCENT = new Color(0x9D7BFF);

    // ---- state
    final JPanel root = new JPanel(new BorderLayout(12, 12));
    final JTextArea log = new JTextArea();
    final JLabel info = new JLabel("not connected");
    final JLabel status = new JLabel("Ready");
    final JProgressBar busy = new JProgressBar();
    final JSpinner seconds = new JSpinner(new SpinnerNumberModel(8, 2, 60, 1));
    final JComboBox<String> photoGrid = new JComboBox<>(new String[]{"64x38", "160x95", "320x190"});
    final JCheckBox repeat = new JCheckBox("repeat until Stop", false), withFast = new JCheckBox("include the fast disk run", true);
    final JButton stop = new JButton("■  Stop"), reconnect = new JButton("Connect");
    final List<JButton> actions = new ArrayList<>();
    final ScreenPanel screenPanel = new ScreenPanel();
    final LampPanel lampPanel = new LampPanel();
    volatile Scenarios sc; volatile Ser ser; volatile Hid hid;
    volatile Thread worker;

    DemoApp() { build(); }

    // ---- window
    static Image ghost(int size) {
        try {
            Path p = Boards.repoRoot().resolve("site/icon-512.png");
            if (Files.exists(p)) return ImageIO.read(p.toFile()).getScaledInstance(size, size, Image.SCALE_SMOOTH);
        } catch (Exception ignored) { }
        return new BufferedImage(size, size, BufferedImage.TYPE_INT_ARGB);
    }

    void build() {
        root.setBorder(new EmptyBorder(14, 14, 14, 14));
        // header: the ghost of the site, the title and the board
        JPanel head = new JPanel(new BorderLayout(14, 0));
        JLabel g = new JLabel(new ImageIcon(ghost(84)));
        JLabel title = new JLabel("Nachtgespenst");
        title.setFont(title.getFont().deriveFont(Font.BOLD, 30f));
        title.setForeground(ACCENT);
        JLabel sub = new JLabel("a HID LampArray demo: the PC paints the lamps, the CoreS3 shows them");
        sub.setForeground(Color.GRAY);
        JPanel t = new JPanel(); t.setLayout(new BoxLayout(t, BoxLayout.Y_AXIS)); t.setOpaque(false);
        t.add(title); t.add(sub); t.add(Box.createVerticalStrut(6)); t.add(info);
        head.add(g, BorderLayout.WEST); head.add(t, BorderLayout.CENTER);
        JPanel conn = new JPanel(new FlowLayout(FlowLayout.RIGHT)); conn.setOpaque(false); conn.add(reconnect);
        head.add(conn, BorderLayout.EAST);
        root.add(head, BorderLayout.NORTH);

        // controls
        JPanel left = new JPanel(); left.setLayout(new BoxLayout(left, BoxLayout.Y_AXIS));
        JButton start = new JButton("▶  Start the cycle");
        start.addActionListener(e -> cycleRun());
        actions.add(start);
        start.setFont(start.getFont().deriveFont(Font.BOLD, 15f));
        stop.addActionListener(e -> stopRun());
        stop.setEnabled(false);
        JPanel cyc = section("Cycle");
        cyc.add(row(start, stop)); cyc.add(repeat); cyc.add(withFast);
        left.add(cyc);

        JPanel one = section("Parts");
        one.add(action("Walk through all pages", () -> sc.walkPages(), false));
        one.add(action("Jules Verne to the SD card and back", () -> sc.disk(false), false));
        one.add(action("… the same in fast mode (no display, no sound)", () -> sc.disk(true), false));
        left.add(one);

        JPanel bar = section("LED bar");
        JPanel br = new JPanel(new GridLayout(0, 2, 6, 6));
        for (String n : Scenarios.BAR_EFFECTS) br.add(action(Character.toUpperCase(n.charAt(0)) + n.substring(1), () -> sc.barEffect(n), false));
        br.add(action("All of them", () -> sc.bar(), false));
        bar.add(br); left.add(bar);

        JPanel grid = section("Lamp grid (C64 style)");
        for (String n : Scenarios.GRID_EFFECTS) grid.add(action(Character.toUpperCase(n.charAt(0)) + n.substring(1), () -> sc.gridEffect(n), false));
        left.add(grid);

        JPanel ph = section("Photo slideshow (prime-modulo dissolve)");
        photoGrid.setSelectedItem("160x95");
        ph.add(row(new JLabel("Lamps"), photoGrid));
        ph.add(action("Slideshow of five photos", () -> { sc.photoGrid = (String) photoGrid.getSelectedItem(); return sc.photos(); }, false));
        left.add(ph);

        JPanel set = section("Settings");
        set.add(row(new JLabel("Seconds per effect"), seconds));
        left.add(set);
        for (Component c : left.getComponents()) if (c instanceof JPanel p) p.setMaximumSize(new Dimension(Integer.MAX_VALUE, p.getPreferredSize().height));
        left.add(Box.createVerticalGlue());
        JScrollPane lp = new JScrollPane(left, ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        lp.setBorder(null); lp.getVerticalScrollBar().setUnitIncrement(16);
        lp.setPreferredSize(new Dimension(380, 100));
        root.add(lp, BorderLayout.WEST);

        // log and status
        log.setEditable(false); log.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12)); log.setLineWrap(true); log.setWrapStyleWord(true);
        JScrollPane logPane = new JScrollPane(log);
        logPane.setBorder(BorderFactory.createTitledBorder("What is happening"));
        root.add(logPane, BorderLayout.CENTER);
        // right: the screen of the board and the lamps as the PC has worked them out
        JPanel right = new JPanel(new GridLayout(2, 1, 8, 8));
        right.setPreferredSize(new Dimension(420, 100));
        screenPanel.setBorder(BorderFactory.createTitledBorder("The board (screenshot)"));
        lampPanel.setBorder(BorderFactory.createTitledBorder("The lamps, worked out on the PC"));
        right.add(screenPanel); right.add(lampPanel);
        root.add(right, BorderLayout.EAST);
        JPanel south = new JPanel(new BorderLayout(10, 0));
        busy.setPreferredSize(new Dimension(160, 8)); busy.setIndeterminate(false);
        south.add(status, BorderLayout.CENTER); south.add(busy, BorderLayout.EAST);
        root.add(south, BorderLayout.SOUTH);

        reconnect.addActionListener(e -> connect());
        setEnabledActions(false);
    }

    static JPanel section(String name) {
        JPanel p = new JPanel(); p.setLayout(new BoxLayout(p, BoxLayout.Y_AXIS));
        TitledBorder b = BorderFactory.createTitledBorder(name); b.setTitleColor(ACCENT);
        p.setBorder(BorderFactory.createCompoundBorder(b, new EmptyBorder(4, 8, 8, 8)));
        p.setAlignmentX(Component.LEFT_ALIGNMENT);
        return p;
    }
    static JPanel row(Component... cs) { JPanel r = new JPanel(new FlowLayout(FlowLayout.LEFT, 6, 2)); for (Component c : cs) r.add(c); r.setAlignmentX(Component.LEFT_ALIGNMENT); return r; }

    JButton action(String label, Callable<String> body, boolean unused) {
        JButton b = new JButton(label);
        b.setAlignmentX(Component.LEFT_ALIGNMENT);
        b.setMaximumSize(new Dimension(Integer.MAX_VALUE, b.getPreferredSize().height));
        b.addActionListener(e -> run(label.replaceAll("[^\\p{L}\\p{N} ()-]", "").trim(), body));
        actions.add(b);
        return b;
    }
    void setEnabledActions(boolean on) { for (JButton b : actions) b.setEnabled(on); }

    // ---- log and status (called from any thread)
    void say(String s) {
        SwingUtilities.invokeLater(() -> { log.append(s + "\n"); log.setCaretPosition(log.getDocument().getLength()); });
    }
    void state(String s, boolean running) {
        SwingUtilities.invokeLater(() -> { status.setText(s); busy.setIndeterminate(running); stop.setEnabled(running); setEnabledActions(!running && sc != null); reconnect.setEnabled(!running); });
    }

    // ---- the board
    void connect() {
        Thread.ofVirtual().name("connect").start(() -> {
            try {
                state("Connecting to the CoreS3 …", true);
                close();
                Boards.Board board = Boards.parse("S3");
                Optional<SerialPort> sp = Boards.find(board);
                if (sp.isEmpty()) throw new IllegalStateException("no CoreS3 found on USB (is it plugged in and flashed?)");
                if (!Boards.usable(sp.get())) throw new IllegalStateException(Boards.SANDBOX_HINT);
                String node = Hid.find("S3", true);
                if (node == null) throw new IllegalStateException("no USB HID node for the CoreS3 (udev rule installed? see docs/wiki/hardware-testing.md)");
                ser = new Ser(sp.get()); hid = new Hid(node);
                sc = new Scenarios(ser, hid, "S3", this::say);
                sc.preview = new Scenarios.Preview() {
                    public void screen(BufferedImage img) { screenPanel.set(img); }
                    public void bar(int[] rgb) { lampPanel.setBar(rgb); }
                    public void grid(int[] rgb, int cols, int rows) { lampPanel.setGrid(rgb, cols, rows); }
                };
                Map<String, String> i = ser.kv("INFO");
                SwingUtilities.invokeLater(() -> info.setText("connected: " + i.get("name") + ", firmware " + i.get("fw") + ", " + i.get("pages") + " pages, USB " + node));
                say("Connected. Pick a demo, or start the cycle.");
                state("Ready", false);
            } catch (Exception e) {
                say("Cannot connect: " + e.getMessage());
                SwingUtilities.invokeLater(() -> info.setText("not connected"));
                state("Not connected", false);
            }
        });
    }

    void close() {
        try { if (hid != null) hid.close(); } catch (Exception ignored) { }
        try { if (ser != null) ser.close(); } catch (Exception ignored) { }
        hid = null; ser = null; sc = null;
    }

    // ---- running demos on virtual threads
    void run(String name, Callable<String> body) {
        if (sc == null || (worker != null && worker.isAlive())) return;
        sc.seconds = ((Number) seconds.getValue()).doubleValue();
        state("Running: " + name, true);
        say("=== " + name);
        lampPanel.clear();
        worker = Thread.ofVirtual().name("demo").start(() -> {
            String note;
            try { note = body.call(); }
            catch (InterruptedException e) { note = "stopped"; }
            catch (Exception e) { note = "failed: " + e; }
            sc.release();
            say("→ " + note);
            state("Ready", false);
        });
    }

    void cycleRun() {
        if (sc == null || (worker != null && worker.isAlive())) return;
        sc.seconds = ((Number) seconds.getValue()).doubleValue();
        sc.photoGrid = (String) photoGrid.getSelectedItem();
        boolean again = repeat.isSelected(), fast = withFast.isSelected();
        state("Cycle running", true);
        lampPanel.clear();
        worker = Thread.ofVirtual().name("cycle").start(() -> {
            try {
                int n = 0;
                do {
                    say("=== cycle " + (++n));
                    for (String[] r : sc.cycle(fast)) say(String.format("%-12s %8s  %s", r[0], r[1], r[2]));
                } while (again && !Thread.currentThread().isInterrupted());
                say("→ cycle done, checks pass=" + sc.pass + " fail=" + sc.fail);
            } catch (InterruptedException e) { say("→ stopped");
            } catch (Exception e) { say("→ failed: " + e); }
            sc.release();
            state("Ready", false);
        });
    }

    void stopRun() { Thread w = worker; if (w != null) { w.interrupt(); say("Stop requested …"); } }

    // ---- main
    public static void main(String[] args) throws Exception {
        String shot = null;
        for (int i = 0; i < args.length; i++) if (args[i].equals("--shot")) shot = args[++i];
        FlatLaf.setGlobalExtraDefaults(Map.of("@accentColor", "#9D7BFF"));
        if (shot != null) System.setProperty("java.awt.headless", "true");
        FlatDarkLaf.setup();
        if (shot != null) {
            DemoApp app = new DemoApp();
            app.info.setText("connected: NG-S3, firmware Nov 19 2026, 30 pages, USB /dev/hidraw0 (preview)");
            app.log.setText("Connected. Pick a demo, or start the cycle.\n=== Jules Verne to the SD card and back\nbook 618734 bytes, raw deflate 232700 bytes (38%)\nsent 232700 compressed bytes in 7,95 s = 28,6 kB/s\nloaded back 618734 bytes in 16,37 s = 36,9 kB/s over the gamepad\nSHA-256 of the received file equals the original\n");
            app.setEnabledActions(true);
            try {
                BufferedImage sample = ImageIO.read(Boards.repoRoot().resolve("site/img/lamps/deventer-lamps.png").toFile());
                app.screenPanel.set(sample);
                int cols = 160, rows = 95; int[] px = new int[cols * rows];
                for (int y = 0; y < rows; y++) for (int x = 0; x < cols; x++) px[y * cols + x] = sample.getRGB(x * 2, y * 2) & 0xFFFFFF;
                app.lampPanel.setGrid(px, cols, rows);
                app.lampPanel.setBar(new int[]{0x220000, 0x660500, 0xCC1000, 0xFF2000, 0xFF1000, 0x880800, 0x330300, 0x110100, 0, 0});
            } catch (Exception ignored) { }
            app.root.setSize(1450, 780);
            layout(app.root);
            BufferedImage img = new BufferedImage(1450, 780, BufferedImage.TYPE_INT_RGB);
            Graphics2D g2 = img.createGraphics();
            g2.setColor(UIManager.getColor("Panel.background")); g2.fillRect(0, 0, 1450, 780);
            app.root.print(g2); g2.dispose();
            ImageIO.write(img, "png", new java.io.File(shot));
            System.out.println("wrote " + shot);
            return;
        }
        SwingUtilities.invokeAndWait(() -> {
            DemoApp app = new DemoApp();
            JFrame f = new JFrame("Nachtgespenst demo");
            f.setIconImage(ghost(64));
            f.setContentPane(app.root);
            f.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
            f.addWindowListener(new java.awt.event.WindowAdapter() {
                public void windowClosing(java.awt.event.WindowEvent e) { app.stopRun(); if (app.sc != null) app.sc.release(); app.close(); f.dispose(); System.exit(0); }
            });
            f.setSize(1450, 780); f.setLocationByPlatform(true); f.setVisible(true);
            app.connect();
        });
    }

    /** The screenshot of the board, scaled to fit. */
    static final class ScreenPanel extends JPanel {
        private volatile BufferedImage img; private final java.util.concurrent.atomic.AtomicBoolean pending = new java.util.concurrent.atomic.AtomicBoolean();
        void set(BufferedImage i) { img = i; if (!pending.getAndSet(true)) SwingUtilities.invokeLater(() -> { pending.set(false); repaint(); }); }
        @Override protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            BufferedImage i = img; if (i == null) { g0.setColor(Color.GRAY); g0.drawString("no screenshot yet", 20, 40); return; }
            Graphics2D g = (Graphics2D) g0;
            Insets in = getInsets(); int w = getWidth() - in.left - in.right - 8, h = getHeight() - in.top - in.bottom - 8;
            double k = Math.min((double) w / i.getWidth(), (double) h / i.getHeight());
            int pw = (int) (i.getWidth() * k), ph = (int) (i.getHeight() * k);
            g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
            g.drawImage(i, in.left + (w - pw) / 2 + 4, in.top + 4, pw, ph, null);
        }
    }

    /** The lamps the way the device draws them: the ten bar lamps in a row and the grid scaled to the lamp area, with hard pixel edges. */
    static final class LampPanel extends JPanel {
        private volatile int[] bar = new int[10]; private volatile int[] grid; private volatile int cols, rows;
        private final java.util.concurrent.atomic.AtomicBoolean pending = new java.util.concurrent.atomic.AtomicBoolean();
        void touch() { if (!pending.getAndSet(true)) SwingUtilities.invokeLater(() -> { pending.set(false); repaint(); }); }
        void setBar(int[] rgb) { bar = rgb; touch(); }
        void setGrid(int[] rgb, int c, int r) { grid = rgb; cols = c; rows = r; touch(); }
        void clear() { bar = new int[10]; grid = null; touch(); }
        @Override protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            Graphics2D g = (Graphics2D) g0;
            Insets in = getInsets(); int w = getWidth() - in.left - in.right - 8, x0 = in.left + 4, y = in.top + 6;
            int[] b = bar; int cell = Math.min(34, w / 10);
            for (int i = 0; i < 10; i++) {
                g.setColor(new Color(b[i] & 0xFFFFFF)); g.fillRoundRect(x0 + i * (w / 10), y, cell - 4, 22, 8, 8);
                g.setColor(new Color(0x555555)); g.drawRoundRect(x0 + i * (w / 10), y, cell - 4, 22, 8, 8);
            }
            int[] gr = grid; int top = y + 30;
            if (gr == null) { g.setColor(Color.GRAY); g.drawString("bar lamps 0-9; the lamp grid appears here", x0, top + 16); return; }
            int ch = getHeight() - in.bottom - top - 22; int c = cols, r = rows;
            BufferedImage im = new BufferedImage(c, r, BufferedImage.TYPE_INT_RGB);
            im.setRGB(0, 0, c, r, gr, 0, c);
            double k = Math.min((double) w / c, (double) ch / r); int pw = (int) (c * k), ph = (int) (r * k);
            g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR);
            g.drawImage(im, x0 + (w - pw) / 2, top, pw, ph, null);
            g.setColor(Color.GRAY); g.drawString(c + " x " + r + " lamps", x0, top + ph + 14);
        }
    }

    static void layout(Container c) { c.doLayout(); for (Component k : c.getComponents()) if (k instanceof Container cc) layout(cc); }
}
