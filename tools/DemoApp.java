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
//SOURCES Cams.java
//SOURCES V4l2.java
//SOURCES Talk.java
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
import javax.sound.sampled.*;
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
    final ConsoleUi.PillBar infoBar = new ConsoleUi.PillBar(ConsoleTheme.PEACH);
    volatile String connText = "not connected", busyText;                     // the info bar: the connection, or what is running
    final SlideTitle slideTitle = new SlideTitle();
    final JSpinner seconds = new JSpinner(new SpinnerNumberModel(8, 2, 60, 1));
    final JComboBox<String> photoGrid = new JComboBox<>(new String[]{"64x38", "160x95", "320x190"});
    final JCheckBox repeat = check("Repeat until stopped", false), withFast = check("Include the fast disk run", true), withMic = check("Include the microphone parts", false);
    final ConsoleUi.BlockButton stop = new ConsoleUi.IconButton(ConsoleUi.IconButton.Kind.CANCEL, "Stop the running demo", ConsoleTheme.CORAL), reconnect = new ConsoleUi.BlockButton("Connect", ConsoleTheme.LAVENDER);
    final List<JButton> actions = new ArrayList<>();
    final Talk.SlidePanel slidePanel = new Talk.SlidePanel();
    Talk.Player player;
    final Cams.CamPanel[] camPanel = {new Cams.CamPanel(), new Cams.CamPanel()};            // 0 = the camera on the tripod above the board, 1 = the laptop camera
    final JCheckBox[] camBox = {check("Tripod camera above the board", false), check("Laptop camera", false)};
    final JComboBox<Cams.Device> camPick0 = new JComboBox<>(), camPick1 = new JComboBox<>();
    final JComboBox<?>[] camPick = {camPick0, camPick1};
    ConsoleUi.Frame camsFrame; JComponent sideScroll; JPanel southPane;
    final JPanel demoPane = new JPanel(), camPane = new JPanel(), logPaneOuter = new JPanel();
    final ConsoleUi.Lcd lcd = new ConsoleUi.Lcd();
    JDialog openDialog;
    final ScreenPanel screenPanel = new ScreenPanel();
    final LampPanel lampPanel = new LampPanel();
    volatile Scenarios sc; volatile Ser ser; volatile Hid hid;
    volatile Thread worker;

    DemoApp() { build(); }

    /** The window: a black ground, the title and a bar of icon buttons on top, the slide with the cameras and previews under it, and a status bar. */
    static final class Root extends JPanel {
        Root() { super(new BorderLayout(ConsoleTheme.GAP, ConsoleTheme.GAP)); setOpaque(true); setBackground(ConsoleTheme.BLACK); setBorder(BorderFactory.createEmptyBorder(0, ConsoleTheme.PAD, ConsoleTheme.GAP + 4, ConsoleTheme.PAD)); }
        @Override protected void paintComponent(Graphics g0) {
            super.paintComponent(g0);
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            int gap = ConsoleTheme.GAP, w = getWidth(), h = getHeight();
            ConsoleUi.ghost(g, gap + 12, gap, ConsoleTheme.BAR_H - gap - 2);
            g.setColor(new Color(0xD8D8E8)); g.setFont(ConsoleTheme.sansBold(17f));
            g.drawString("Nachtgespenst", gap + 12 + ConsoleTheme.BAR_H + 2, gap + (ConsoleTheme.BAR_H - gap) / 2 + 6);
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
        JPanel west = new JPanel(); west.setOpaque(false); west.setPreferredSize(new Dimension(ConsoleTheme.BAR_H + 12 + 2 + 140, ConsoleTheme.BAR_H));
        reconnect.setPreferredSize(new Dimension(130, ConsoleTheme.BLOCK_H)); reconnect.setMaximumSize(new Dimension(130, ConsoleTheme.BLOCK_H));
        JPanel conn = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, ConsoleTheme.BAR_H / 2 - ConsoleTheme.BLOCK_H / 2)); conn.setOpaque(false); conn.add(reconnect);
        head.add(west, BorderLayout.WEST); head.add(infoWrap, BorderLayout.CENTER); head.add(conn, BorderLayout.EAST);
        infoBar.setText("not connected"); infoBar.setPreferredSize(new Dimension(100, 28));
        root.add(head, BorderLayout.NORTH);

        // toolbar: icon buttons for the talk and the popups (demos, cameras, microphone, speaker, log); every button has a tooltip
        stop.setEnabled(false); stop.addActionListener(e -> stopRun());
        JPanel tools = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 7)); tools.setOpaque(false);
        tools.add(tb(ConsoleUi.IconButton.Kind.PLAY, "Start the talk (F5)", ConsoleTheme.AMBER, this::talkStart));
        tools.add(tb(ConsoleUi.IconButton.Kind.PAUSE, "Pause or resume the talk (P)", ConsoleTheme.PEACH, () -> { if (player != null) player.pause(); }));
        tools.add(tb(ConsoleUi.IconButton.Kind.PREV, "Previous slide (left, up, Page Up)", ConsoleTheme.LAVENDER, () -> { if (player != null) player.prev(); }));
        tools.add(tb(ConsoleUi.IconButton.Kind.NEXT, "Next slide (right, down, space, Page Down)", ConsoleTheme.LAVENDER, () -> { if (player != null) player.next(); }));
        tools.add(tb(ConsoleUi.IconButton.Kind.STOP, "Stop the talk (Esc)", ConsoleTheme.CORAL, this::talkStop));
        tools.add(Box.createHorizontalStrut(10));
        stop.setPreferredSize(new Dimension(44, 34)); stop.setMaximumSize(new Dimension(44, 34)); tools.add(stop);
        tools.add(tb(ConsoleUi.IconButton.Kind.DEMOS, "Demos: the cycle and every part on its own", ConsoleTheme.LAVENDER, () -> popup("Demos", demoPane)));
        tools.add(tb(ConsoleUi.IconButton.Kind.CAMERA, "Cameras: choose, turn and mirror", ConsoleTheme.LAVENDER, () -> popup("Cameras", camPane)));
        tools.add(tb(ConsoleUi.IconButton.Kind.MIC, "Microphone: device, gain and level", ConsoleTheme.LAVENDER, this::micPopup));
        tools.add(tb(ConsoleUi.IconButton.Kind.SPEAKER, "Speaker: device and volume", ConsoleTheme.LAVENDER, () -> popup("Speaker", speakerPane)));
        tools.add(tb(ConsoleUi.IconButton.Kind.LOG, "What is happening: the log", ConsoleTheme.LAVENDER, () -> popup("What is happening", logPaneOuter)));
        // the header: the title of the slide in the middle, the info bar (connection, or what is running) at the right; the toolbar sits at the bottom
        infoWrap.setPreferredSize(new Dimension(300, ConsoleTheme.BAR_H));
        JPanel mid = new JPanel(new BorderLayout(gap, 0)); mid.setOpaque(false); mid.add(slideTitle, BorderLayout.CENTER); mid.add(infoWrap, BorderLayout.EAST);
        head.remove(infoWrap); head.add(mid, BorderLayout.CENTER);
        slidePanel.onTitle = slideTitle::set;
        sideScroll = tools;

        // popup: the demos, in three columns
        ConsoleUi.BlockButton start = new ConsoleUi.BlockButton("Start the cycle", ConsoleTheme.AMBER);
        start.setFont(ConsoleTheme.sansBold(16f)); start.setPreferredSize(new Dimension(100, 52)); start.setMaximumSize(new Dimension(Integer.MAX_VALUE, 52));
        start.addActionListener(e -> cycleRun()); actions.add(start);
        JPanel c1 = column(), c2 = column(), c3 = column();
        c1.add(start); c1.add(Box.createVerticalStrut(gap)); c1.add(repeat); c1.add(withFast); c1.add(withMic);
        c1.add(group("Parts", ConsoleTheme.PEACH));
        c1.add(action("Walk through all pages", () -> sc.walkPages(), ConsoleTheme.PEACH));
        c1.add(action("Jules Verne to the SD card", () -> sc.disk(false), ConsoleTheme.PEACH));
        c1.add(action("The same, fast mode", () -> sc.disk(true), ConsoleTheme.PEACH));
        c1.add(group("Microphone, the PC plays tones", ConsoleTheme.CORAL));
        c1.add(action("Oscilloscope on the lamps", () -> sc.micScope(), ConsoleTheme.CORAL));
        c1.add(action("Vu meter on the LED bar", () -> sc.micDemo(), ConsoleTheme.CORAL));
        c1.add(action("Microphone check", () -> sc.micCheck(), ConsoleTheme.CORAL));
        c2.add(group("LED bar, painted by the PC", ConsoleTheme.LAVENDER));
        for (String n : Scenarios.BAR_EFFECTS) c2.add(action(cap(n), () -> sc.barEffect(n), ConsoleTheme.LAVENDER));
        c2.add(action("All of them", () -> sc.bar(), ConsoleTheme.LAVENDER));
        c2.add(group("On the board itself", ConsoleTheme.AMBER));
        for (String n : Scenarios.BOARD_EFFECTS) c2.add(action(cap(n), () -> sc.boardEffect(n), ConsoleTheme.AMBER));
        c3.add(group("Lamp grid, in the style of a home computer", ConsoleTheme.PEACH));
        for (String n : Scenarios.GRID_EFFECTS) c3.add(action(cap(n), () -> sc.gridEffect(n), ConsoleTheme.PEACH));
        c3.add(group("Photo slideshow", ConsoleTheme.LAVENDER));
        photoGrid.setSelectedItem("160x95"); photoGrid.setFont(ConsoleTheme.mono(13f)); photoGrid.setAlignmentX(Component.LEFT_ALIGNMENT);
        photoGrid.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        c3.add(photoGrid); c3.add(Box.createVerticalStrut(gap));
        c3.add(action("Slideshow of five photos", () -> { sc.photoGrid = (String) photoGrid.getSelectedItem(); return sc.photos(); }, ConsoleTheme.LAVENDER));
        c3.add(group("Seconds per effect", ConsoleTheme.PEACH));
        seconds.setFont(ConsoleTheme.mono(13f)); seconds.setAlignmentX(Component.LEFT_ALIGNMENT); seconds.setMaximumSize(new Dimension(120, 30));
        c3.add(seconds);
        demoPane.setLayout(new GridLayout(1, 3, 2 * gap, 0)); demoPane.setOpaque(false);
        demoPane.add(c1); demoPane.add(c2); demoPane.add(c3);

        // popup: the cameras
        camPane.setLayout(new BoxLayout(camPane, BoxLayout.Y_AXIS)); camPane.setOpaque(false);
        for (Cams.Device d : Cams.list()) { camPick0.addItem(d); camPick1.addItem(d); }
        if (camPick1.getItemCount() > 1) camPick1.setSelectedIndex(0);
        if (camPick0.getItemCount() > 1) camPick0.setSelectedIndex(camPick0.getItemCount() - 1);
        java.util.prefs.Preferences pr = java.util.prefs.Preferences.userNodeForPackage(DemoApp.class);
        for (int i = 0; i < 2; i++) {
            final int k = i;
            camPick[i].setFont(ConsoleTheme.mono(12f)); camPick[i].setAlignmentX(Component.LEFT_ALIGNMENT); camPick[i].setMaximumSize(new Dimension(Integer.MAX_VALUE, 28));
            String want = pr.get("cam" + i, null);
            if (want != null) for (int j = 0; j < camPick[i].getItemCount(); j++) if (camPick[i].getItemAt(j).toString().equals(want)) camPick[i].setSelectedIndex(j);
            camBox[i].addActionListener(e -> camToggle(k));
            camPick[i].addActionListener(e -> { if (camBox[k].isSelected()) camToggle(k); });
            camPanel[i].quarter = pr.getInt("rot" + i, 0); camPanel[i].mirror = pr.getBoolean("mir" + i, false);
            camPanel[i].flip = pr.getBoolean("flip" + i, false);
            JPanel row = new JPanel(new GridLayout(1, 5, 4, 0)); row.setOpaque(false); row.setAlignmentX(Component.LEFT_ALIGNMENT); row.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
            row.add(iconButton(ConsoleUi.IconButton.Kind.TURN_LEFT, "Turn a quarter anticlockwise", () -> { camPanel[k].quarter = (camPanel[k].quarter + 3) & 3; camSaved(k); }));
            row.add(iconButton(ConsoleUi.IconButton.Kind.TURN_RIGHT, "Turn a quarter clockwise", () -> { camPanel[k].quarter = (camPanel[k].quarter + 1) & 3; camSaved(k); }));
            row.add(iconButton(ConsoleUi.IconButton.Kind.TURN_HALF, "Turn half a circle", () -> { camPanel[k].quarter = (camPanel[k].quarter + 2) & 3; camSaved(k); }));
            row.add(iconButton(ConsoleUi.IconButton.Kind.MIRROR_H, "Mirror left to right", () -> { camPanel[k].mirror = !camPanel[k].mirror; camSaved(k); }));
            row.add(iconButton(ConsoleUi.IconButton.Kind.MIRROR_V, "Mirror top to bottom", () -> { camPanel[k].flip = !camPanel[k].flip; camSaved(k); }));
            camPane.add(camBox[i]); camPane.add(camPick[i]); camPane.add(Box.createVerticalStrut(4)); camPane.add(row); camPane.add(Box.createVerticalStrut(2 * gap));
        }
        camPane.setPreferredSize(new Dimension(380, 220));
        buildAudioPanes(pr);

        // centre: the slide and the log on the left, the cameras, the board and the lamps on the right
        log.setEditable(false); log.setFont(ConsoleTheme.mono(12f)); log.setLineWrap(true); log.setWrapStyleWord(true);
        log.setBackground(ConsoleTheme.BLACK); log.setForeground(ConsoleTheme.TEXT); log.setCaretColor(ConsoleTheme.AMBER); log.setBorder(BorderFactory.createEmptyBorder(4, 6, 4, 6));
        JScrollPane logPane = new JScrollPane(log); logPane.setBorder(null); logPane.getViewport().setBackground(ConsoleTheme.BLACK);
        logPaneOuter.setLayout(new BorderLayout()); logPaneOuter.setOpaque(false); logPaneOuter.add(logPane); logPaneOuter.setPreferredSize(new Dimension(640, 360));
        ConsoleUi.Frame slideFrame = new ConsoleUi.Frame("The talk", ConsoleTheme.AMBER); slideFrame.setLayout(new BorderLayout()); slideFrame.add(slidePanel);
                camsFrame = new ConsoleUi.Frame("The cameras", ConsoleTheme.PEACH); camsFrame.setLayout(new Cams.CamLayout(gap)); camsFrame.setVisible(false);
        for (int i = 0; i < 2; i++) { camPanel[i].setPreferredSize(new Dimension(10, 10)); camPanel[i].setVisible(false); camsFrame.add(camPanel[i]); }
        ConsoleUi.Frame boardFrame = new ConsoleUi.Frame("Board screenshot", ConsoleTheme.LAVENDER); boardFrame.setLayout(new BorderLayout()); boardFrame.add(screenPanel);
        ConsoleUi.Frame lampFrame = new ConsoleUi.Frame("Lamps on PC", ConsoleTheme.AMBER); lampFrame.setLayout(new BorderLayout()); lampFrame.add(lampPanel);
        // the centre grows and shrinks with the window: two fifths for the slide and the log, three fifths for the pictures
        for (JComponent c : new JComponent[]{slideFrame, camsFrame, boardFrame, lampFrame, screenPanel, lampPanel, slidePanel}) c.setPreferredSize(new Dimension(10, 10));
        JPanel left = new JPanel(new BorderLayout()); left.setOpaque(false); left.add(slideFrame);
        JPanel right = new JPanel(new GridBagLayout()); right.setOpaque(false);
        GridBagConstraints rc = new GridBagConstraints(); rc.fill = GridBagConstraints.BOTH; rc.weightx = 1; rc.gridx = 0; rc.insets = new Insets(0, 0, gap, 0);
        rc.gridy = 0; rc.weighty = 0.56; right.add(camsFrame, rc);
        rc.gridy = 1; rc.weighty = 0.18; right.add(boardFrame, rc);
        rc.gridy = 2; rc.weighty = 0.26; rc.insets = new Insets(0, 0, 0, 0); right.add(lampFrame, rc);
        JPanel centre = new JPanel(new GridBagLayout()); centre.setOpaque(false);
        GridBagConstraints cc = new GridBagConstraints(); cc.fill = GridBagConstraints.BOTH; cc.weighty = 1; cc.gridy = 0;
        cc.gridx = 0; cc.weightx = 0.70; cc.insets = new Insets(0, 0, 0, gap); centre.add(left, cc);
        cc.gridx = 1; cc.weightx = 0.30; cc.insets = new Insets(0, 0, 0, 0); centre.add(right, cc);
        root.add(centre, BorderLayout.CENTER);

        JPanel south = southPane = new JPanel(new GridBagLayout()); south.setOpaque(false);
        GridBagConstraints sc2 = new GridBagConstraints(); sc2.fill = GridBagConstraints.NONE; sc2.anchor = GridBagConstraints.WEST; sc2.weightx = 0; sc2.gridx = 0; south.add(tools, sc2);
        sc2.fill = GridBagConstraints.HORIZONTAL; sc2.weightx = 1; sc2.gridx = 1; sc2.insets = new Insets(0, gap, 0, 0); south.add(lcd, sc2);
        root.add(south, BorderLayout.SOUTH);

        screenPanel.setOpaque(false); lampPanel.setOpaque(false);
        reconnect.addActionListener(e -> connect());
        setEnabledActions(false);
        loadSlides();
        // keys for the talk: space or right = next, left = previous, P = pause, H = hide the sidebar (a clean window to share), F11 = full screen
        JComponent r = root; var im = r.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW); var am = r.getActionMap();
        // A presentation clicker types these keys (the Core2 as a clicker does too): PowerPoint and every third-party clicker use the same ones.
        Runnable next = () -> { if (player != null) player.next(); }, prev = () -> { if (player != null) player.prev(); };
        for (String k : new String[]{"SPACE", "RIGHT", "DOWN", "PAGE_DOWN", "ENTER"}) bindKey(im, am, k, next);
        for (String k : new String[]{"LEFT", "UP", "PAGE_UP", "BACK_SPACE"}) bindKey(im, am, k, prev);
        bindKey(im, am, "P", () -> { if (player != null) player.pause(); });
        bindKey(im, am, "B", this::toggleBlank); bindKey(im, am, "PERIOD", this::toggleBlank);     // black screen, as in PowerPoint
        bindKey(im, am, "F5", this::talkStart);
        bindKey(im, am, "H", this::toggleSidebar);
    }

    /** A column inside a scroll pane that always has the width of the viewport, so wide children (the camera lists) are cut off instead of widening the sidebar. */
    static final class ColumnPanel extends JPanel implements Scrollable {
        public Dimension getPreferredScrollableViewportSize() { return getPreferredSize(); }
        public int getScrollableUnitIncrement(Rectangle r, int o, int d) { return 18; }
        public int getScrollableBlockIncrement(Rectangle r, int o, int d) { return 120; }
        public boolean getScrollableTracksViewportWidth() { return true; }
        public boolean getScrollableTracksViewportHeight() { return false; }
    }

    // ---- sound: the speaker the tones of the demos play on, and the microphone of the PC (a level check for the online talk)
    final JPanel speakerPane = new JPanel(), micPane = new JPanel();
    final JComboBox<Mixer.Info> speakerPick = new JComboBox<>(), micPick = new JComboBox<>();
    final JSlider speakerVol = new JSlider(0, 100, 100), micGain = new JSlider(0, 200, 100);
    final Meter meter = new Meter();
    volatile TargetDataLine micLine; volatile boolean micRun;

    void buildAudioPanes(java.util.prefs.Preferences pr) {
        for (Mixer.Info mi : Tones.devices(true)) speakerPick.addItem(mi);
        for (Mixer.Info mi : Tones.devices(false)) micPick.addItem(mi);
        speakerPick.setRenderer(new MixerRenderer()); micPick.setRenderer(new MixerRenderer());
        pickByName(speakerPick, pr.get("speaker", null)); pickByName(micPick, pr.get("mic", null));
        speakerVol.setValue(pr.getInt("speakerVol", 100)); micGain.setValue(pr.getInt("micGain", 100));
        Tones.volume = speakerVol.getValue() / 100.0; Tones.output = (Mixer.Info) speakerPick.getSelectedItem();
        speakerPick.addActionListener(e -> { Tones.output = (Mixer.Info) speakerPick.getSelectedItem(); if (Tones.output != null) pr.put("speaker", Tones.output.getName()); });
        speakerVol.addChangeListener(e -> { Tones.volume = speakerVol.getValue() / 100.0; pr.putInt("speakerVol", speakerVol.getValue()); });
        micGain.addChangeListener(e -> pr.putInt("micGain", micGain.getValue()));
        micPick.addActionListener(e -> { Mixer.Info mi = (Mixer.Info) micPick.getSelectedItem(); if (mi != null) pr.put("mic", mi.getName()); if (micRun) { stopMic(); startMic(); } });
        speakerPane.setLayout(new BoxLayout(speakerPane, BoxLayout.Y_AXIS)); speakerPane.setOpaque(false);
        speakerPane.add(label("Device")); speakerPane.add(fit(speakerPick)); speakerPane.add(Box.createVerticalStrut(8));
        speakerPane.add(label("Volume of the tones")); speakerPane.add(fit(speakerVol)); speakerPane.add(Box.createVerticalStrut(8));
        ConsoleUi.BlockButton test = new ConsoleUi.BlockButton("Play a test tone", ConsoleTheme.LAVENDER);
        test.addActionListener(e -> Tones.playAsync(440, 1.0, 0.6)); speakerPane.add(test);
        speakerPane.setPreferredSize(new Dimension(440, 190));
        micPane.setLayout(new BoxLayout(micPane, BoxLayout.Y_AXIS)); micPane.setOpaque(false);
        micPane.add(label("Device")); micPane.add(fit(micPick)); micPane.add(Box.createVerticalStrut(8));
        micPane.add(label("Gain")); micPane.add(fit(micGain)); micPane.add(Box.createVerticalStrut(8));
        micPane.add(label("Level")); meter.setAlignmentX(Component.LEFT_ALIGNMENT); meter.setMaximumSize(new Dimension(Integer.MAX_VALUE, 26)); meter.setPreferredSize(new Dimension(100, 26)); micPane.add(meter);
        micPane.add(Box.createVerticalStrut(8)); micPane.add(label("The microphone is opened only while this window is open. The demos listen with the microphone of the board, not with this one."));
        micPane.setPreferredSize(new Dimension(440, 230));
    }
    static void pickByName(JComboBox<Mixer.Info> box, String name) { if (name != null) for (int i = 0; i < box.getItemCount(); i++) if (box.getItemAt(i).getName().equals(name)) box.setSelectedIndex(i); }
    static JComponent fit(JComponent c) { c.setAlignmentX(Component.LEFT_ALIGNMENT); c.setMaximumSize(new Dimension(Integer.MAX_VALUE, c.getPreferredSize().height)); return c; }
    static JLabel label(String t) {
        JLabel l = new JLabel("<html><body style='width:400px'>" + t + "</body></html>"); l.setFont(ConsoleTheme.sansRegular(12f)); l.setForeground(ConsoleTheme.PEACH); l.setAlignmentX(Component.LEFT_ALIGNMENT); return l;
    }
    static final class MixerRenderer extends DefaultListCellRenderer {
        @Override public Component getListCellRendererComponent(JList<?> l, Object v, int i, boolean sel, boolean foc) {
            super.getListCellRendererComponent(l, v, i, sel, foc); if (v instanceof Mixer.Info mi) setText(mi.getName()); return this;
        }
    }
    void micPopup() { startMic(); try { popup("Microphone", micPane); } finally { stopMic(); } }
    void startMic() {
        Mixer.Info mi = (Mixer.Info) micPick.getSelectedItem();
        if (mi == null) { meter.set(0, "no microphone found"); return; }
        micRun = true;
        Thread.ofVirtual().name("mic").start(() -> {
            AudioFormat fmt = new AudioFormat(16000f, 16, 1, true, false);
            try (TargetDataLine line = AudioSystem.getTargetDataLine(fmt, mi)) {
                line.open(fmt); line.start(); micLine = line; byte[] b = new byte[1600];
                while (micRun) {
                    int n = line.read(b, 0, b.length); if (n <= 0) break;
                    double sum = 0; for (int i = 0; i + 1 < n; i += 2) { short v = (short) ((b[i + 1] << 8) | (b[i] & 255)); sum += (double) v * v; }
                    double rms = Math.sqrt(sum / (n / 2)) / 32768.0 * (micGain.getValue() / 100.0);
                    meter.set(Math.min(1.0, Math.sqrt(rms * 6)), null);                       // a square root so that quiet speech still moves the meter
                }
            } catch (Exception e) { meter.set(0, "cannot open the microphone: " + e.getMessage()); }
            micLine = null;
        });
    }
    void stopMic() { micRun = false; TargetDataLine l = micLine; if (l != null) { try { l.stop(); l.close(); } catch (RuntimeException ignored) { } } meter.set(0, null); }

    /** A level meter in the style of the LED bar: a row of small blocks that light up from the left. */
    static final class Meter extends JComponent {
        private volatile double level; private volatile String note; private double peak;
        void set(double l, String n) { level = l; note = n; SwingUtilities.invokeLater(this::repaint); }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            int n = 30, w = getWidth(), h = getHeight(); double cw = (double) w / n; peak = Math.max(level, peak - 0.02);
            for (int i = 0; i < n; i++) {
                boolean on = i < level * n; Color c = i > n * 0.85 ? ConsoleTheme.CORAL : i > n * 0.6 ? ConsoleTheme.AMBER : ConsoleTheme.PEACH;
                g.setColor(on ? c : ConsoleTheme.mix(c, ConsoleTheme.BLACK, 0.85)); g.fill(new java.awt.geom.RoundRectangle2D.Double(i * cw + 1, 2, cw - 3, h - 4, 4, 4));
            }
            if (note != null) { g.setColor(ConsoleTheme.CORAL); g.setFont(ConsoleTheme.sansRegular(12f)); g.drawString(note, 6, h / 2 + 4); }
            g.dispose();
        }
    }

    /** A toolbar button: a drawn icon in a block, with a tooltip. */
    ConsoleUi.IconButton tb(ConsoleUi.IconButton.Kind kind, String tip, Color colour, Runnable r) {
        ConsoleUi.IconButton b = new ConsoleUi.IconButton(kind, tip, colour);
        b.setPreferredSize(new Dimension(44, 34)); b.setMaximumSize(new Dimension(44, 34));
        b.addActionListener(e -> r.run());
        return b;
    }

    static JPanel column() { JPanel c = new JPanel(); c.setLayout(new BoxLayout(c, BoxLayout.Y_AXIS)); c.setOpaque(false); return c; }

    /** A modal popup with the content on black; Escape closes it. Starting a demo from it closes it too. */
    void popup(String title, JComponent content) {
        Window owner = SwingUtilities.getWindowAncestor(root);
        JDialog d = new JDialog(owner, title, Dialog.ModalityType.APPLICATION_MODAL);
        JPanel p = new JPanel(new BorderLayout()); p.setBackground(ConsoleTheme.BLACK); p.setBorder(BorderFactory.createEmptyBorder(14, 16, 16, 16)); p.add(content);
        d.setContentPane(p); d.pack(); d.setLocationRelativeTo(owner);
        p.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke("ESCAPE"), "close");
        p.getActionMap().put("close", new AbstractAction() { public void actionPerformed(java.awt.event.ActionEvent e) { d.dispose(); } });
        openDialog = d; d.setVisible(true); openDialog = null;
    }
    void closePopup() { JDialog d = openDialog; if (d != null) SwingUtilities.invokeLater(d::dispose); }

    static void bindKey(InputMap im, ActionMap am, String key, Runnable r) {
        im.put(KeyStroke.getKeyStroke(key), "k-" + key);
        am.put("k-" + key, new AbstractAction() { public void actionPerformed(java.awt.event.ActionEvent e) { r.run(); } });
    }

    JButton talkButton(String label, Color colour, Runnable r) {
        ConsoleUi.BlockButton b = new ConsoleUi.BlockButton(label, colour);
        b.addActionListener(e -> r.run());
        return b;
    }

    // ---- the black screen (B or full stop) and the mouse pointer that hides itself while the talk runs and you do not move it
    boolean blank; JFrame frame;
    final JPanel blankPane = new JPanel() { @Override protected void paintComponent(Graphics g) { g.setColor(Color.BLACK); g.fillRect(0, 0, getWidth(), getHeight()); } };
    void toggleBlank() {
        blank = !blank;
        if (frame != null) { blankPane.setOpaque(true); frame.setGlassPane(blankPane); blankPane.setVisible(blank); blankPane.setCursor(blank ? hiddenCursor() : Cursor.getDefaultCursor()); frame.repaint(); }
    }
    static Cursor hiddenCursor() { return Toolkit.getDefaultToolkit().createCustomCursor(new BufferedImage(16, 16, BufferedImage.TYPE_INT_ARGB), new Point(0, 0), "none"); }
    void autoHideCursor(JFrame f) {
        final Cursor hidden = hiddenCursor();
        javax.swing.Timer idle = new javax.swing.Timer(2500, e -> { if (player != null && player.running) f.setCursor(hidden); });
        idle.setRepeats(false);
        Toolkit.getDefaultToolkit().addAWTEventListener(ev -> { f.setCursor(Cursor.getDefaultCursor()); idle.restart(); }, AWTEvent.MOUSE_MOTION_EVENT_MASK | AWTEvent.MOUSE_EVENT_MASK);
    }

    void toggleSidebar() {
        boolean show = !sideScroll.isVisible();
        sideScroll.setVisible(show);
        root.revalidate(); root.repaint();
    }

    ConsoleUi.IconButton iconButton(ConsoleUi.IconButton.Kind kind, String tip, Runnable r) {
        ConsoleUi.IconButton b = new ConsoleUi.IconButton(kind, tip, ConsoleTheme.LAVENDER);
        b.addActionListener(e -> r.run());
        return b;
    }
    void camSaved(int i) {
        var pr = java.util.prefs.Preferences.userNodeForPackage(DemoApp.class);
        pr.putInt("rot" + i, camPanel[i].quarter); pr.putBoolean("mir" + i, camPanel[i].mirror); pr.putBoolean("flip" + i, camPanel[i].flip);
        camPanel[i].refresh(); camsFrame.revalidate();
    }

    // ---- cameras: a camera is only open while its box is ticked
    void camToggle(int i) {
        Cams.CamPanel p = camPanel[i];
        Cams.Cam old = p.cam; p.cam = null; if (old != null) old.close();
        if (camBox[i].isSelected()) {
            Cams.Device d = (Cams.Device) camPick[i].getSelectedItem();
            if (d == null) p.text = "no camera found"; else { p.cam = new Cams.Cam(d.node(), p::refresh); java.util.prefs.Preferences.userNodeForPackage(DemoApp.class).put("cam" + i, d.toString()); }
        } else p.text = "off";
        p.setVisible(camBox[i].isSelected());
        camsFrame.setVisible(camBox[0].isSelected() || camBox[1].isSelected());
        p.refresh(); root.revalidate(); root.repaint();
    }
    /** Switch a camera on from the command line: by number (the order of --list in CamTool) or by device node, which need not be in the list. The same camera may serve both panels. */
    void camStart(int i, String which) {
        Cams.Device pick = null; List<Cams.Device> all = Cams.list();
        if (which.matches("\\d+") && Integer.parseInt(which) < all.size()) pick = all.get(Integer.parseInt(which));
        else if (which.startsWith("/dev/")) { for (Cams.Device d : all) if (d.node().equals(which)) pick = d; if (pick == null) pick = new Cams.Device(which, which); }
        if (pick == null) { camPanel[i].text = "no camera " + which; camPanel[i].setVisible(true); camsFrame.setVisible(true); return; }
        JComboBox<Cams.Device> box = camPick(i); boolean known = false;
        for (int j = 0; j < box.getItemCount(); j++) if (box.getItemAt(j).node().equals(pick.node())) { box.setSelectedIndex(j); known = true; }
        if (!known) { box.addItem(pick); box.setSelectedItem(pick); }
        camBox[i].setSelected(true); camToggle(i);
    }
    @SuppressWarnings("unchecked") JComboBox<Cams.Device> camPick(int i) { return (JComboBox<Cams.Device>) camPick[i]; }
    void closeCams() { for (int i = 0; i < 2; i++) { Cams.Cam c = camPanel[i].cam; camPanel[i].cam = null; if (c != null) c.close(); } }

    // ---- the talk
    static final Path TALK_DIR = Boards.repoRoot().resolve("tools/talk");
    void loadSlides() {
        List<Talk.Slide> slides = Talk.load(TALK_DIR);
        player = new Talk.Player(slides, this::slideShown, this::showProgress, () -> worker != null && worker.isAlive());
        if (!slides.isEmpty()) slidePanel.show(slides.get(0), 0, slides.size());
    }
    void talkStart() {
        if (player != null && player.running) return;
        loadSlides();                                                                // the files are read again, so edits show without a restart
        if (player.slides.isEmpty()) { say("No slides found in " + TALK_DIR); return; }
        say("=== the talk starts, " + player.slides.size() + " slides, about " + player.totalMs() / 60000 + " minutes " + (player.totalMs() / 1000 % 60) + " s");
        player.start();
    }
    void talkStop() { if (player != null && player.running) { player.stop(); stopRun(); say("The talk is stopped."); } }
    void slideShown(Talk.Slide s) {
        slidePanel.show(s, player.index, player.slides.size());
        for (String n : s.notes()) say("note: " + n);
        Thread w = worker;
        if (s.demo() == null) { if (w != null && w.isAlive()) w.interrupt(); return; }
        Thread.ofVirtual().start(() -> {
            try { if (w != null && w.isAlive()) { w.interrupt(); w.join(4000); } } catch (InterruptedException ignored) { }
            SwingUtilities.invokeLater(() -> { Callable<String> d = demoFor(s.demo()); if (d != null && sc != null) run(s.title(), d); else if (sc == null) say("(no board: the demo of this slide is skipped)"); else say("unknown demo " + s.demo()); });
        });
    }
    void showProgress() {
        if (player == null || !player.running) { SwingUtilities.invokeLater(() -> lcd.talk(-1, "")); return; }
        long e = player.elapsedMs() / 1000, t = player.totalMs() / 1000;
        final String tt = e / 60 + ":" + String.format("%02d", e % 60) + " / " + t / 60 + ":" + String.format("%02d", t % 60) + (player.paused ? " ||" : "");
        final double fr = t == 0 ? 0 : (double) e / t;
        SwingUtilities.invokeLater(() -> lcd.talk(fr, tt));
        if (player.index >= 0) slidePanel.show(player.slides.get(player.index), player.index, player.slides.size());
    }
    void setInfo() {
        SwingUtilities.invokeLater(() -> { String b = busyText; infoBar.setText(b != null ? b : connText); infoBar.setBusy(b != null); infoBar.setFill(b != null ? ConsoleTheme.AMBER : ConsoleTheme.PEACH); });
    }

    /** The title of the slide, large, in the header. */
    static final class SlideTitle extends JComponent {
        private volatile String text = "";
        SlideTitle() { setOpaque(false); }
        void set(String t) { text = t; repaint(); }
        @Override protected void paintComponent(Graphics g0) {
            Graphics2D g = (Graphics2D) g0.create(); ConsoleUi.quality(g);
            g.setFont(ConsoleTheme.sansBold(30f)); g.setColor(ConsoleTheme.SLIDE_TITLE);
            FontMetrics fm = g.getFontMetrics(); String t = text;
            while (t.length() > 1 && fm.stringWidth(t) > getWidth()) t = t.substring(0, t.length() - 2) + "…";
            g.drawString(t, 0, (getHeight() + fm.getAscent() - fm.getDescent()) / 2);
            g.dispose();
        }
    }

    /** The demos a slide can start: pages, disk, diskfast, bars, bar:name, board:name, grid:name, scope, vu, miccheck, photos. */
    Callable<String> demoFor(String id) {
        String k = id.contains(":") ? id.substring(0, id.indexOf(':')) : id, v = id.contains(":") ? id.substring(id.indexOf(':') + 1) : "";
        return switch (k) {
            case "pages" -> () -> sc.walkPages();
            case "disk" -> () -> sc.disk(false);
            case "diskfast" -> () -> sc.disk(true);
            case "bars" -> () -> sc.bar();
            case "bar" -> () -> sc.barEffect(v);
            case "board" -> () -> sc.boardEffect(v);
            case "grid" -> () -> sc.gridEffect(v);
            case "scope" -> () -> sc.micScope();
            case "vu" -> () -> sc.micDemo();
            case "miccheck" -> () -> sc.micCheck();
            case "photos" -> () -> { sc.photoGrid = (String) photoGrid.getSelectedItem(); return sc.photos(); };
            default -> null;
        };
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
        SwingUtilities.invokeLater(() -> { log.append(s + "\n"); log.setCaretPosition(log.getDocument().getLength()); lcdFeed(s); });
    }
    /** Reads the numbers of a log line into the readouts of the bottom bar: speeds, tones, rates and the hash check. */
    void lcdFeed(String s) {
        java.util.regex.Matcher m;
        if ((m = java.util.regex.Pattern.compile("sent .*= ([0-9.,]+) kB/s").matcher(s)).find()) lcd.set("send kB/s", num(m.group(1)), 60, fmt(m.group(1)), ConsoleTheme.AMBER);
        if ((m = java.util.regex.Pattern.compile("loaded back .*= ([0-9.,]+) kB/s").matcher(s)).find()) lcd.set("load kB/s", num(m.group(1)), 60, fmt(m.group(1)), ConsoleTheme.AMBER);
        if ((m = java.util.regex.Pattern.compile("([0-9.,]+) reports per second").matcher(s)).find()) lcd.set("reports/s", num(m.group(1)), 250, fmt(m.group(1)), ConsoleTheme.PEACH);
        if ((m = java.util.regex.Pattern.compile("hears (?:about )?([0-9.,]+) Hz|measured: ([0-9.,]+) Hz").matcher(s)).find()) { String v = m.group(1) != null ? m.group(1) : m.group(2); lcd.set("tone Hz", num(v), 2000, fmt(v), ConsoleTheme.LAVENDER); }
        if ((m = java.util.regex.Pattern.compile("\\(([0-9.,]+) lamps/s\\)").matcher(s)).find()) lcd.set("lamps/s", num(m.group(1)), 20000, fmt(m.group(1)), ConsoleTheme.PEACH);
        if (s.contains("SHA-256") && s.contains("equals")) lcd.set("SHA-256", 1, 1, "match", ConsoleTheme.PEACH);
        else if (s.contains("SHA-256") && (s.contains("differ") || s.contains("not equal"))) lcd.set("SHA-256", 1, 1, "differs", ConsoleTheme.CORAL);
    }
    static double num(String v) { try { return Double.parseDouble(v.replace(',', '.')); } catch (NumberFormatException e) { return 0; } }
    static String fmt(String v) { double d = num(v); return d >= 100 ? String.valueOf(Math.round(d)) : String.format(java.util.Locale.ROOT, "%.1f", d); }

    void state(String s, boolean running) {
        busyText = running ? s : null; setInfo(); SwingUtilities.invokeLater(() -> { stop.setEnabled(running); setEnabledActions(!running && sc != null); reconnect.setEnabled(!running); });
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
                connText = "Connected: " + i.get("name") + ", " + i.get("pages") + " pages"; setInfo();
                say("Connected. Pick a demo, or start the cycle.");
                state("Ready", false);
            } catch (Exception e) {
                say("Cannot connect: " + e.getMessage());
                connText = "Not connected"; setInfo();
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
        closePopup();
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
        closePopup();
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
        String shot = null, scopeShot = null; final String[] camArg = new String[2];             // --tripod CAM and --laptop CAM: a camera number or /dev/videoN, switched on at the start
        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("--shot")) shot = args[++i];
            else if (args[i].equals("--scope-shot")) scopeShot = args[++i];
            else if (args[i].equals("--tripod")) camArg[0] = args[++i];
            else if (args[i].equals("--laptop")) camArg[1] = args[++i];
        }
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
            app.infoBar.setText("Connected: NG-S3, 30 pages");
            app.log.setText("Connected. Pick a demo, or start the cycle.\n=== Jules Verne to the SD card and back\nbook 618734 bytes, raw deflate 232700 bytes (38%)\nsent 232700 compressed bytes in 7,95 s = 28,6 kB/s\nloaded back 618734 bytes in 16,37 s = 36,9 kB/s over the gamepad\nSHA-256 of the received file equals the original\n");
            for (int i = 0; i < 2; i++) { app.camPanel[i].setVisible(true); app.camPanel[i].text = i == 0 ? "tripod camera" : "laptop camera"; }  // the layout with both camera panels
            app.camsFrame.setVisible(true);
            app.setEnabledActions(true);
            app.lcd.talk(0.42, "4:12 / 10:10"); app.lcd.set("send kB/s", 28.6, 60, "28.6", ConsoleTheme.AMBER); app.lcd.set("load kB/s", 36.9, 60, "36.9", ConsoleTheme.AMBER); app.lcd.set("SHA-256", 1, 1, "match", ConsoleTheme.PEACH);
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
                public void windowClosing(java.awt.event.WindowEvent e) { app.stopRun(); app.closeCams(); if (app.sc != null) app.sc.release(); app.close(); f.dispose(); System.exit(0); }
            });
            f.setMinimumSize(new Dimension(980, 640)); f.setSize(1280, 820); f.setLocationByPlatform(true);
            // F11 toggles full screen, Escape leaves it; the window can also be maximised and resized freely, the layout follows
            app.root.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke("F11"), "full");
            app.root.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(KeyStroke.getKeyStroke("ESCAPE"), "unfull");        // also what a clicker's end key types
            app.root.getActionMap().put("full", new AbstractAction() { public void actionPerformed(java.awt.event.ActionEvent e) { fullScreen(f, null); } });
            app.root.getActionMap().put("unfull", new AbstractAction() { public void actionPerformed(java.awt.event.ActionEvent e) { if (app.blank) app.toggleBlank(); else fullScreen(f, false); } });
            app.frame = f; app.autoHideCursor(f);
            for (int i = 0; i < 2; i++) if (camArg[i] != null) app.camStart(i, camArg[i]);
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
            g.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);         // smooth, not blocks: the lamps are a picture here
            g.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
            g.drawImage(im, px, py, pw, ph, null);
            g.setColor(Color.GRAY); g.setFont(ConsoleTheme.mono(11f));
            String cap = c + " x " + r + " lamps";
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
