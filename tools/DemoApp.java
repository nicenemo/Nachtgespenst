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
//SOURCES Tones.java
//SOURCES Telemetry.java
//SOURCES Scope.java
//SOURCES ConsoleTheme.java
//SOURCES ConsoleUi.java
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
    // ---- state
    final Root root = new Root();
    final JTextArea log = new JTextArea();
    final ConsoleUi.PillBar infoBar = new ConsoleUi.PillBar(ConsoleTheme.PEACH), statusBar = new ConsoleUi.PillBar(ConsoleTheme.LAVENDER);
    final JSpinner seconds = new JSpinner(new SpinnerNumberModel(8, 2, 60, 1));
    final JComboBox<String> photoGrid = new JComboBox<>(new String[]{"64x38", "160x95", "320x190"});
    final JCheckBox repeat = check("Repeat until stopped", false), withFast = check("Include the fast disk run", true), withMic = check("Include the microphone parts", false);
    final ConsoleUi.BlockButton stop = new ConsoleUi.BlockButton("Stop", ConsoleTheme.CORAL), reconnect = new ConsoleUi.BlockButton("Connect", ConsoleTheme.LAVENDER);
    final List<JButton> actions = new ArrayList<>();
    final ScreenPanel screenPanel = new ScreenPanel();
    final LampPanel lampPanel = new LampPanel();
    volatile Scenarios sc; volatile Ser ser; volatile Hid hid;
    volatile Thread worker;

    DemoApp() { build(); }

    /** The window: a black ground, the elbow at the top left with the title, the sidebar of blocks under it, the log and the previews, and a status bar. */
    static final class Root extends JPanel {
        Root() { super(new BorderLayout(ConsoleTheme.GAP, ConsoleTheme.GAP)); setOpaque(true); setBackground(ConsoleTheme.BLACK); setBorder(BorderFactory.createEmptyBorder(0, 0, ConsoleTheme.GAP + 4, ConsoleTheme.PAD)); }
        @Override protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            int gap = ConsoleTheme.GAP, w = getWidth(), h = getHeight();
            // the banner: black, with the swirling waves of the site behind the ghost and the title
            Shape band = new java.awt.geom.RoundRectangle2D.Double(gap, gap, w - gap - ConsoleTheme.PAD, ConsoleTheme.BAR_H - gap, 36, 36);
            Graphics2D gb = (Graphics2D) g.create(); gb.clip(band);
            ConsoleUi.waves(gb, gap, gap, w - gap - ConsoleTheme.PAD, ConsoleTheme.BAR_H - gap); gb.dispose();
            ConsoleUi.ghost(g, gap + 12, gap, ConsoleTheme.BAR_H - gap - 2);
            g.setColor(new Color(0xF6E7A6)); g.setFont(ConsoleTheme.sansBold(26f));
            g.drawString("Nachtgespenst", gap + 12 + ConsoleTheme.BAR_H + 4, gap + (ConsoleTheme.BAR_H - gap) / 2 + 10);
            // the elbow frames the sidebar: a thin bar over it and a leg beside it, a large outer radius and a tight inner radius
            g.setColor(ConsoleTheme.AMBER);
            int y0 = ConsoleTheme.BAR_H + gap;
            g.fill(ConsoleUi.elbow(0, y0, ConsoleTheme.LEG_W + ConsoleTheme.ELBOW_W, h - y0 - gap - 4, ConsoleTheme.ELBOW_W, ConsoleTheme.ELBOW_W, 28, 8));
            g.dispose();
        }
    }

    static Image ghost(int size) {
        try {
            Path p = Boards.repoRoot().resolve("site/icon-512.png");
            if (Files.exists(p)) return ImageIO.read(p.toFile()).getScaledInstance(size, size, Image.SCALE_SMOOTH);
        } catch (Exception ignored) { }
        return new BufferedImage(size, size, BufferedImage.TYPE_INT_ARGB);
    }

    static JCheckBox check(String text, boolean on) {
        JCheckBox c = new JCheckBox(text, on);
        c.setOpaque(false); c.setFont(ConsoleTheme.sansRegular(13f)); c.setForeground(ConsoleTheme.PEACH); c.setAlignmentX(Component.LEFT_ALIGNMENT); c.setFocusPainted(false);
        return c;
    }

    void build() {
        int gap = ConsoleTheme.GAP;
        // header: the info bar next to the elbow, the connect block at the right
        JPanel head = new JPanel(new BorderLayout(gap, 0)); head.setOpaque(false);
        head.setPreferredSize(new Dimension(100, ConsoleTheme.BAR_H));
        JPanel infoWrap = new JPanel(new BorderLayout()); infoWrap.setOpaque(false);
        infoWrap.setBorder(BorderFactory.createEmptyBorder(ConsoleTheme.BAR_H / 2 - 12, 0, ConsoleTheme.BAR_H / 2 - 12, 0)); infoWrap.add(infoBar);
        JPanel west = new JPanel(); west.setOpaque(false); west.setPreferredSize(new Dimension(ConsoleTheme.BAR_H + 12 + 4 + 215, ConsoleTheme.BAR_H));
        reconnect.setPreferredSize(new Dimension(130, ConsoleTheme.BLOCK_H)); reconnect.setMaximumSize(new Dimension(130, ConsoleTheme.BLOCK_H));
        JPanel conn = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, ConsoleTheme.BAR_H / 2 - ConsoleTheme.BLOCK_H / 2)); conn.setOpaque(false); conn.add(reconnect);
        head.add(west, BorderLayout.WEST); head.add(infoWrap, BorderLayout.CENTER); head.add(conn, BorderLayout.EAST);
        infoBar.setText("not connected"); infoBar.setPreferredSize(new Dimension(100, 28));
        root.add(head, BorderLayout.NORTH);

        // sidebar: blocks in the colours of the console, the text at the bottom right of each block
        JPanel side = new JPanel(); side.setLayout(new BoxLayout(side, BoxLayout.Y_AXIS)); side.setOpaque(false);
        side.setBorder(BorderFactory.createEmptyBorder(ConsoleTheme.ELBOW_W + 2 * gap, ConsoleTheme.ELBOW_W + gap, gap, gap));
        ConsoleUi.BlockButton start = new ConsoleUi.BlockButton("Start the cycle", ConsoleTheme.AMBER);
        start.setFont(ConsoleTheme.sansBold(16f)); start.setPreferredSize(new Dimension(100, 52)); start.setMaximumSize(new Dimension(Integer.MAX_VALUE, 52));
        start.addActionListener(e -> cycleRun()); actions.add(start);
        stop.setEnabled(false); stop.addActionListener(e -> stopRun());
        side.add(start); side.add(stop); side.add(Box.createVerticalStrut(gap));
        side.add(repeat); side.add(withFast); side.add(withMic);

        side.add(group("Parts", ConsoleTheme.PEACH));
        side.add(action("Walk through all pages", () -> sc.walkPages(), ConsoleTheme.PEACH));
        side.add(action("Jules Verne to the SD card", () -> sc.disk(false), ConsoleTheme.PEACH));
        side.add(action("The same, fast mode", () -> sc.disk(true), ConsoleTheme.PEACH));

        side.add(group("LED bar, painted by the PC", ConsoleTheme.LAVENDER));
        for (String n : Scenarios.BAR_EFFECTS) side.add(action(cap(n), () -> sc.barEffect(n), ConsoleTheme.LAVENDER));
        side.add(action("All of them", () -> sc.bar(), ConsoleTheme.LAVENDER));

        side.add(group("On the board itself", ConsoleTheme.AMBER));
        for (String n : Scenarios.BOARD_EFFECTS) side.add(action(cap(n), () -> sc.boardEffect(n), ConsoleTheme.AMBER));

        side.add(group("Microphone, the PC plays tones", ConsoleTheme.CORAL));
        side.add(action("Oscilloscope on the lamps", () -> sc.micScope(), ConsoleTheme.CORAL));
        side.add(action("Vu meter on the LED bar", () -> sc.micDemo(), ConsoleTheme.CORAL));
        side.add(action("Microphone check", () -> sc.micCheck(), ConsoleTheme.CORAL));

        side.add(group("Lamp grid, in the style of a home computer", ConsoleTheme.PEACH));
        for (String n : Scenarios.GRID_EFFECTS) side.add(action(cap(n), () -> sc.gridEffect(n), ConsoleTheme.PEACH));

        side.add(group("Photo slideshow", ConsoleTheme.LAVENDER));
        photoGrid.setSelectedItem("160x95"); photoGrid.setFont(ConsoleTheme.mono(13f)); photoGrid.setAlignmentX(Component.LEFT_ALIGNMENT);
        photoGrid.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        side.add(photoGrid); side.add(Box.createVerticalStrut(gap));
        side.add(action("Slideshow of five photos", () -> { sc.photoGrid = (String) photoGrid.getSelectedItem(); return sc.photos(); }, ConsoleTheme.LAVENDER));

        side.add(group("Seconds per effect", ConsoleTheme.PEACH));
        seconds.setFont(ConsoleTheme.mono(13f)); seconds.setAlignmentX(Component.LEFT_ALIGNMENT); seconds.setMaximumSize(new Dimension(120, 30));
        side.add(seconds);
        JScrollPane lp = new JScrollPane(side, ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        lp.getVerticalScrollBar().putClientProperty("FlatLaf.style", "width: 8; thumbArc: 999; trackArc: 999; thumb: #9999FF; track: #000000; hoverThumbColor: #FFCC99; pressedThumbColor: #FF9900");
        lp.setBorder(null); lp.setOpaque(false); lp.getViewport().setOpaque(false); lp.getVerticalScrollBar().setUnitIncrement(18);
        lp.setPreferredSize(new Dimension(ConsoleTheme.LEG_W + ConsoleTheme.ELBOW_W + gap, 100));
        root.add(lp, BorderLayout.WEST);

        // centre: the log, and the board and the lamps on the right
        log.setEditable(false); log.setFont(ConsoleTheme.mono(13f)); log.setLineWrap(true); log.setWrapStyleWord(true);
        log.setBackground(ConsoleTheme.BLACK); log.setForeground(ConsoleTheme.TEXT); log.setCaretColor(ConsoleTheme.AMBER); log.setBorder(BorderFactory.createEmptyBorder(4, 6, 4, 6));
        JScrollPane logPane = new JScrollPane(log); logPane.setBorder(null); logPane.getViewport().setBackground(ConsoleTheme.BLACK);
        ConsoleUi.Frame logFrame = new ConsoleUi.Frame("What is happening", ConsoleTheme.PEACH); logFrame.setLayout(new BorderLayout()); logFrame.add(logPane);
        ConsoleUi.Frame boardFrame = new ConsoleUi.Frame("The board", ConsoleTheme.LAVENDER); boardFrame.setLayout(new BorderLayout()); boardFrame.add(screenPanel);
        ConsoleUi.Frame lampFrame = new ConsoleUi.Frame("The lamps, worked out on the PC", ConsoleTheme.AMBER); lampFrame.setLayout(new BorderLayout()); lampFrame.add(lampPanel);
        // the centre grows and shrinks with the window: the log takes two fifths, the previews three fifths, board and lamps share the height
        for (JComponent c : new JComponent[]{logFrame, boardFrame, lampFrame, screenPanel, lampPanel, logPane}) c.setPreferredSize(new Dimension(10, 10));
        JPanel right = new JPanel(new GridBagLayout()); right.setOpaque(false);
        GridBagConstraints rc = new GridBagConstraints(); rc.fill = GridBagConstraints.BOTH; rc.weightx = 1; rc.gridx = 0; rc.insets = new Insets(0, 0, gap, 0);
        rc.gridy = 0; rc.weighty = 0.5; right.add(boardFrame, rc);
        rc.gridy = 1; rc.weighty = 0.5; rc.insets = new Insets(0, 0, 0, 0); right.add(lampFrame, rc);
        JPanel centre = new JPanel(new GridBagLayout()); centre.setOpaque(false);
        GridBagConstraints cc = new GridBagConstraints(); cc.fill = GridBagConstraints.BOTH; cc.weighty = 1; cc.gridy = 0;
        cc.gridx = 0; cc.weightx = 0.4; cc.insets = new Insets(0, 0, 0, gap); centre.add(logFrame, cc);
        cc.gridx = 1; cc.weightx = 0.6; cc.insets = new Insets(0, 0, 0, 0); centre.add(right, cc);
        root.add(centre, BorderLayout.CENTER);

        statusBar.setText("Ready"); statusBar.setPreferredSize(new Dimension(100, 30));
        JPanel south = new JPanel(new BorderLayout()); south.setOpaque(false); south.setBorder(BorderFactory.createEmptyBorder(0, ConsoleTheme.LEG_W + ConsoleTheme.ELBOW_W + gap, 0, 0)); south.add(statusBar);
        root.add(south, BorderLayout.SOUTH);

        screenPanel.setOpaque(false); lampPanel.setOpaque(false);
        reconnect.addActionListener(e -> connect());
        setEnabledActions(false);
    }

    static String cap(String n) { return Character.toUpperCase(n.charAt(0)) + n.substring(1); }
    static Component group(String name, Color c) {
        JPanel p = new JPanel(); p.setOpaque(false); p.setLayout(new BoxLayout(p, BoxLayout.Y_AXIS)); p.setAlignmentX(Component.LEFT_ALIGNMENT);
        p.add(Box.createVerticalStrut(ConsoleTheme.GAP + 4)); p.add(new ConsoleUi.Band(name, c)); p.add(Box.createVerticalStrut(4));
        return p;
    }

    JButton action(String label, Callable<String> body, Color colour) {
        ConsoleUi.BlockButton b = new ConsoleUi.BlockButton(label, colour);
        b.addActionListener(e -> run(label, body));
        actions.add(b);
        return b;
    }
    void setEnabledActions(boolean on) { for (JButton b : actions) b.setEnabled(on); }

    // ---- log and status (called from any thread)
    void say(String s) {
        SwingUtilities.invokeLater(() -> { log.append(s + "\n"); log.setCaretPosition(log.getDocument().getLength()); });
    }
    void state(String s, boolean running) {
        SwingUtilities.invokeLater(() -> { statusBar.setText(s); statusBar.setBusy(running); statusBar.setFill(running ? ConsoleTheme.AMBER : ConsoleTheme.LAVENDER); stop.setEnabled(running); setEnabledActions(!running && sc != null); reconnect.setEnabled(!running); });
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
                Map<String, String> i = ser.kv("info");
                SwingUtilities.invokeLater(() -> infoBar.setText("Connected: " + i.get("name") + ", firmware " + i.get("fw") + ", " + i.get("pages") + " pages, USB " + node));
                say("Connected. Pick a demo, or start the cycle.");
                state("Ready", false);
            } catch (Exception e) {
                say("Cannot connect: " + e.getMessage());
                SwingUtilities.invokeLater(() -> infoBar.setText("Not connected"));
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
        boolean again = repeat.isSelected(), fast = withFast.isSelected(), mic = withMic.isSelected();
        state("Cycle running", true);
        lampPanel.clear();
        worker = Thread.ofVirtual().name("cycle").start(() -> {
            try {
                int n = 0;
                do {
                    say("=== cycle " + (++n));
                    for (String[] r : sc.cycle(fast, mic)) say(String.format("%-12s %8s  %s", r[0], r[1], r[2]));
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
        String shot = null, scopeShot = null;
        for (int i = 0; i < args.length; i++) { if (args[i].equals("--shot")) shot = args[++i]; if (args[i].equals("--scope-shot")) scopeShot = args[++i]; }
        if (scopeShot != null) {                               // the oscilloscope for a synthetic tone, without a board
            System.setProperty("java.awt.headless", "true");
            Scope sc = new Scope(160, 95); java.util.Random rnd = new java.util.Random(1);
            for (int f = 0; f < 6; f++) {
                int[] v = new int[512]; double ph = rnd.nextDouble() * 6.28;
                for (int i = 0; i < 512; i++) v[i] = (int) (3000 * Math.sin(2 * Math.PI * 880 * i / 16000.0 + ph) + rnd.nextGaussian() * 120);
                sc.frame(null, v);
            }
            BufferedImage im = new BufferedImage(160 * 4, 95 * 4, BufferedImage.TYPE_INT_RGB);
            for (int y = 0; y < 95; y++) for (int x = 0; x < 160; x++) for (int dy = 0; dy < 4; dy++) for (int dx = 0; dx < 4; dx++) im.setRGB(x * 4 + dx, y * 4 + dy, sc.pix[y * 160 + x]);
            ImageIO.write(im, "png", new java.io.File(scopeShot)); System.out.println("wrote " + scopeShot); return;
        }
        FlatLaf.setGlobalExtraDefaults(Map.of("@accentColor", "#9D7BFF"));
        if (shot != null) System.setProperty("java.awt.headless", "true");
        FlatDarkLaf.setup();
        if (shot != null) {
            DemoApp app = new DemoApp();
            app.infoBar.setText("Connected: NG-S3, firmware Nov 19 2026, 30 pages, USB /dev/hidraw0");
            app.log.setText("Connected. Pick a demo, or start the cycle.\n=== Jules Verne to the SD card and back\nbook 618734 bytes, raw deflate 232700 bytes (38%)\nsent 232700 compressed bytes in 7,95 s = 28,6 kB/s\nloaded back 618734 bytes in 16,37 s = 36,9 kB/s over the gamepad\nSHA-256 of the received file equals the original\n");
            app.setEnabledActions(true);
            try {
                BufferedImage sample = ImageIO.read(Boards.repoRoot().resolve("site/img/lamps/scope-880.png").toFile());
                app.screenPanel.set(sample);                       // the board shows the oscilloscope the PC paints on its lamps
                Scope sc = new Scope(80, 47); java.util.Random rnd = new java.util.Random(1);
                for (int f = 0; f < 5; f++) {
                    int[] v = new int[512]; double ph = rnd.nextDouble() * 6.28;
                    for (int i = 0; i < 512; i++) v[i] = (int) (3000 * Math.sin(2 * Math.PI * 880 * i / 16000.0 + ph) + rnd.nextGaussian() * 120);
                    sc.frame(null, v);
                }
                app.lampPanel.setGrid(sc.pix.clone(), 80, 47);
                app.lampPanel.setBar(new int[]{0x220000, 0x660500, 0xCC1000, 0xFF2000, 0xFF1000, 0x880800, 0x330300, 0x110100, 0, 0});
            } catch (Exception ignored) { }
            app.root.setSize(1280, 820);
            layout(app.root);
            BufferedImage img = new BufferedImage(1280, 820, BufferedImage.TYPE_INT_RGB);
            Graphics2D g2 = img.createGraphics();
            g2.setColor(UIManager.getColor("Panel.background")); g2.fillRect(0, 0, 1280, 820);
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
            f.setMinimumSize(new Dimension(980, 640)); f.setSize(1280, 820); f.setLocationByPlatform(true);
            // F11 toggles full screen, Escape leaves it; the window can also be maximised and resized freely, the layout follows
            app.root.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke("F11"), "full");
            app.root.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke("ESCAPE"), "unfull");
            app.root.getActionMap().put("full", new AbstractAction() { public void actionPerformed(java.awt.event.ActionEvent e) { fullScreen(f, null); } });
            app.root.getActionMap().put("unfull", new AbstractAction() { public void actionPerformed(java.awt.event.ActionEvent e) { fullScreen(f, false); } });
            f.setVisible(true);
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
        /** Two vertical bars of five lamps left and right of the picture: lamps 0 to 4 run down the right side, lamps 5 to 9 run up the left side (webcam measurement). */
        @Override protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            Graphics2D g = (Graphics2D) g0;
            Insets in = getInsets();
            int barW = Math.max(26, Math.min(46, getWidth() / 14)), gap = 10, x0 = in.left + 4, y0 = in.top + 6;
            int w = getWidth() - in.left - in.right - 8, h = getHeight() - in.top - in.bottom - 34;
            int availW = w - 2 * (barW + gap);
            int[] gr = grid; int c = cols, r = rows;
            double aspect = gr == null ? 320.0 / 190.0 : (double) c / r;                 // the lamp area of the device is 320 x 190
            int pw = availW, ph = (int) (pw / aspect);
            if (ph > h) { ph = h; pw = (int) (ph * aspect); }
            int px = x0 + barW + gap + (availW - pw) / 2, py = y0 + (h - ph) / 2;
            int[] b = bar;
            for (int side = 0; side < 2; side++) {
                int bx = side == 0 ? px - gap - barW : px + pw + gap;
                for (int i = 0; i < 5; i++) {
                    int cell = ph / 5, ly = py + i * cell + 3;
                    int lamp = side == 0 ? 9 - i : i;                          // the left side runs up (lamp 9 at the top), the right side runs down
                    ConsoleUi.ledCell(g, bx, ly, barW, cell - 6, b[lamp] & 0xFFFFFF);
                }
            }
            if (gr == null) {
                g.setColor(new Color(0x101010)); g.fillRect(px, py, pw, ph); g.setColor(Color.GRAY);
                g.drawRect(px, py, pw, ph); g.drawString("the lamp grid appears here", px + 12, py + ph / 2);
                return;
            }
            BufferedImage im = new BufferedImage(c, r, BufferedImage.TYPE_INT_RGB);
            im.setRGB(0, 0, c, r, gr, 0, c);
            g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR);
            g.drawImage(im, px, py, pw, ph, null);
            g.setColor(Color.GRAY); g.setFont(ConsoleTheme.mono(11f));
            String cap = c + " x " + r + " lamps, bar 0-4 down the right, 5-9 up the left";
            while (cap.length() > 12 && g.getFontMetrics().stringWidth(cap) > w) cap = cap.substring(0, cap.length() - 1);   // never wider than the panel
            g.drawString(cap, x0, Math.min(getHeight() - 6, py + ph + 18));
        }
    }

    /** Full screen on or off (null toggles). The frame is undecorated in full screen, which needs a dispose first. */
    static void fullScreen(JFrame f, Boolean want) {
        GraphicsDevice d = GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();
        boolean now = d.getFullScreenWindow() == f, to = want == null ? !now : want;
        if (to == now) return;
        f.dispose(); f.setUndecorated(to); f.setVisible(true); d.setFullScreenWindow(to ? f : null);
        if (!to) { f.setSize(1280, 820); f.setLocationByPlatform(true); }
        f.revalidate(); f.repaint();
    }

    static void layout(Container c) { c.doLayout(); for (Component k : c.getComponents()) if (k instanceof Container cc) layout(cc); }
}
