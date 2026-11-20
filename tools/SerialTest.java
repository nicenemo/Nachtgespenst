///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 27
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;

import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.imageio.ImageIO;

/**
 * Serial test harness for Nachtgespenst boards (M5Stack Core2 and CoreS3).
 *
 * <p>Validates firmware functionality and communication over UART/serial link:
 * <ul>
 *   <li>Verifies board identity and capabilities via {@code INFO}.</li>
 *   <li>Exercises shell commands (syntax error checking, help index, uppercase rejection).</li>
 *   <li>Validates UI layer navigation, touch input (TAP, DOWN, UP), and hardware buttons.</li>
 *   <li>Tests HID key injection on host via evdev (Scroll Lock, F13-F24, System Control Wakeup).</li>
 *   <li>Verifies EEPROM/NVS persistent settings, LED indicators, and backlight control.</li>
 *   <li>Verifies BLE and USB connection status and routing modes.</li>
 *   <li>Captures screenshots via framebuffer readout and runs optical/pixel sanity assertions.</li>
 *   <li>Measures UI layer switch and rendering latency via {@code --latency}.</li>
 * </ul>
 *
 * <h2>Usage:</h2>
 * <pre>{@code
 *   jbang tools/SerialTest.java [--board S3|C2] [--shot DIR] [--port /dev/ttyX] [--latency]
 * }</pre>
 *
 * <p>Exit code 0 indicates all tests passed; exit code 1 indicates one or more failures or
 * that no compatible board was detected.
 */
public class SerialTest {

    private static int passCount = 0;
    private static int failCount = 0;
    private static final List<String> report = new ArrayList<>();

    /**
     * Record representing an evdev input event from {@code /dev/input/eventN}.
     *
     * @param code Linux key code (e.g., 70 for KEY_SCROLLLOCK, 143 for KEY_WAKEUP)
     * @param value Event value (1 for press, 0 for release, 2 for autorepeat)
     */
    public record KeyEvent(int code, int value) {}

    /**
     * Records an assertion result and appends it to the test report log.
     *
     * @param board board identifier (e.g. "C2" or "S3")
     * @param name name of the test check
     * @param ok true if assertion passed
     * @param detail optional diagnostic information
     */
    static void check(String board, String name, boolean ok, String detail) {
        if (ok) {
            passCount++;
        } else {
            failCount++;
        }
        String suffix = detail.isEmpty() ? "" : "  (" + detail + ")";
        report.add(String.format("%s [%s] %s%s", ok ? "PASS" : "FAIL", board, name, suffix));
    }

    /**
     * Adds an informational message to the test report log.
     *
     * @param message informational text
     */
    static void info(String message) {
        report.add("INFO " + message);
    }

    /**
     * Serial communication link wrapping a {@link SerialPort} with non-blocking timeouts,
     * line buffering, and command round-trip helpers.
     */
    static class Link implements AutoCloseable {
        private final SerialPort port;
        private final ByteArrayOutputStream pending = new ByteArrayOutputStream();

        /**
         * Creates a link for the given serial port.
         *
         * @param port the underlying serial port
         */
        Link(SerialPort port) {
            this.port = port;
        }

        /**
         * Opens the port at 115200 baud with non-blocking timeout configuration.
         *
         * @return true if successfully opened
         */
        boolean open() {
            port.setBaudRate(115200);
            port.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
            return port.openPort();
        }

        /**
         * Drains any pending input bytes in the serial receive FIFO.
         *
         * @throws InterruptedException if interrupted while sleeping
         */
        void drain() throws InterruptedException {
            Thread.sleep(150);
            byte[] buffer = new byte[4096];
            while (port.bytesAvailable() > 0) {
                int available = port.bytesAvailable();
                port.readBytes(buffer, Math.min(buffer.length, available));
            }
        }

        /**
         * Sends an ASCII command string terminated by newline.
         *
         * @param s command string to transmit
         */
        void send(String s) {
            byte[] bytes = (s + "\n").getBytes(StandardCharsets.US_ASCII);
            port.writeBytes(bytes, bytes.length);
        }

        /**
         * Reads exactly {@code n} bytes from the serial port within the specified timeout.
         *
         * @param n number of bytes to read
         * @param timeoutMs maximum time to wait in milliseconds
         * @return byte array of length {@code n}, or null if timed out
         * @throws InterruptedException if interrupted during sleep
         */
        byte[] readBytes(int n, long timeoutMs) throws InterruptedException {
            byte[] out = new byte[n];
            int received = 0;
            long deadline = System.currentTimeMillis() + timeoutMs;

            while (received < n && System.currentTimeMillis() < deadline) {
                int available = port.bytesAvailable();
                if (available > 0) {
                    int read = port.readBytes(out, Math.min(available, n - received), received);
                    if (read > 0) {
                        received += read;
                    }
                } else {
                    Thread.sleep(2);
                }
            }
            return received == n ? out : null;
        }

        /**
         * Reads a single line terminated by newline within the specified timeout.
         *
         * @param timeoutMs maximum time to wait in milliseconds
         * @return trimmed non-empty line string, or null on timeout
         * @throws InterruptedException if interrupted during sleep
         */
        String line(long timeoutMs) throws InterruptedException {
            long deadline = System.currentTimeMillis() + timeoutMs;
            StringBuilder sb = new StringBuilder();
            byte[] one = new byte[1];

            while (System.currentTimeMillis() < deadline) {
                if (port.bytesAvailable() > 0 && port.readBytes(one, 1) == 1) {
                    if (one[0] == '\n') {
                        String s = sb.toString().trim();
                        if (!s.isEmpty()) {
                            return s;
                        }
                        sb.setLength(0);
                    } else if (one[0] != '\r') {
                        sb.append((char) one[0]);
                    }
                } else {
                    Thread.sleep(2);
                }
            }
            return null;
        }

        /**
         * Sends a command and waits for the first reply line starting with {@code OK} or {@code ERR}.
         *
         * @param command command string to send
         * @param timeoutMs maximum timeout in milliseconds
         * @return reply line string starting with OK or ERR, or null if timed out
         * @throws InterruptedException if interrupted during sleep
         */
        String cmd(String command, long timeoutMs) throws InterruptedException {
            send(command);
            long deadline = System.currentTimeMillis() + timeoutMs;
            while (System.currentTimeMillis() < deadline) {
                long remaining = Math.max(1, deadline - System.currentTimeMillis());
                String reply = line(remaining);
                if (reply == null) {
                    return null;
                }
                if (reply.startsWith("OK") || reply.startsWith("ERR")) {
                    return reply;
                }
            }
            return null;
        }

        @Override
        public void close() {
            port.closePort();
        }
    }

    /**
     * Parses a space-delimited string of {@code key=value} pairs into a Map.
     *
     * @param s string containing key=value tokens
     * @return map of key-value associations
     */
    static Map<String, String> parseKeyValue(String s) {
        Map<String, String> map = new HashMap<>();
        if (s == null) {
            return map;
        }
        for (String token : s.split("\\s+")) {
            int eq = token.indexOf('=');
            if (eq > 0) {
                map.put(token.substring(0, eq), token.substring(eq + 1));
            }
        }
        return map;
    }

    /**
     * Determines whether a serial port matches known Nachtgespenst hardware VID:PID pairs.
     *
     * @param port the serial port to examine
     * @return true if candidate VID:PID matches CP2104 or ESP32-S3 USB CDC
     */
    static boolean isCandidate(SerialPort port) {
        int vid = port.getVendorID();
        int pid = port.getProductID();
        return (vid == 0x10C4 && pid == 0xEA60)   // Core2 CP2104 USB-UART
            || (vid == 0x1209 && pid == 0x0001)   // CoreS3 Nachtgespenst USB CDC
            || (vid == 0x303A && pid == 0x1001);  // ESP32-S3 ROM bootloader
    }

    /**
     * Finds the Linux evdev device node in {@code /proc/bus/input/devices} matching a name pattern.
     *
     * @param deviceName pattern to match in device name
     * @param usb true to match USB devices (Phys starting with "usb-"), false for Bluetooth
     * @return path to device node (e.g. "/dev/input/event3"), or null if not found
     */
    static String findEventNode(String deviceName, boolean usb) {
        try {
            String currentName = null;
            boolean match = false;
            Path path = Path.of("/proc/bus/input/devices");
            if (!Files.exists(path)) {
                return null;
            }

            for (String line : Files.readAllLines(path)) {
                if (line.startsWith("N: Name=")) {
                    currentName = line;
                    match = false;
                }
                if (line.startsWith("P: Phys=") && currentName != null && currentName.contains(deviceName)) {
                    match = usb == line.substring(8).startsWith("usb-");
                }
                if (line.startsWith("H: Handlers=") && match) {
                    for (String token : line.split("\\s+")) {
                        if (token.startsWith("event")) {
                            return "/dev/input/" + token;
                        }
                    }
                }
                if (line.isBlank()) {
                    currentName = null;
                    match = false;
                }
            }
        } catch (IOException ignored) {}
        return null;
    }

    /**
     * Captures Linux evdev key events (type == EV_KEY) on a virtual thread while executing an action.
     *
     * @param evPath path to the evdev node
     * @param action action to trigger the event (e.g. serial key command)
     * @param waitMs time to collect events after triggering
     * @return list of captured key events
     * @throws Exception if opening or reading the evdev node fails
     */
    static List<KeyEvent> captureKeys(String evPath, Runnable action, long waitMs) throws Exception {
        List<KeyEvent> events = Collections.synchronizedList(new ArrayList<>());
        try (FileInputStream in = new FileInputStream(evPath)) {
            Thread captureThread = Thread.ofVirtual().name("evdev-capture").start(() -> {
                byte[] rawEvent = new byte[24];
                try {
                    while (true) {
                        int readTotal = 0;
                        while (readTotal < 24) {
                            int r = in.read(rawEvent, readTotal, 24 - readTotal);
                            if (r < 0) return;
                            readTotal += r;
                        }
                        ByteBuffer bb = ByteBuffer.wrap(rawEvent).order(ByteOrder.LITTLE_ENDIAN);
                        bb.position(16);
                        int type = bb.getShort() & 0xFFFF;
                        int code = bb.getShort() & 0xFFFF;
                        int value = bb.getInt();
                        if (type == 1) { // EV_KEY
                            events.add(new KeyEvent(code, value));
                        }
                    }
                } catch (IOException ignored) {}
            });

            Thread.sleep(100);
            action.run();
            Thread.sleep(waitMs);
            captureThread.interrupt();
        }
        return events;
    }

    /**
     * Executes functional test suite against a Nachtgespenst keyboard board.
     *
     * @param tag board tag ("C2" or "S3")
     * @param link serial link to the board
     * @param info board identity and configuration key-values
     * @param shotDir directory for screenshots, or null
     * @throws Exception on communication or I/O failure
     */
    static void testBoard(String tag, Link link, Map<String, String> info, Path shotDir) throws Exception {
        String reply = link.cmd("ping", 1500);
        check(tag, "PING", "OK PONG".equals(reply), String.valueOf(reply));

        String badCmd = link.cmd("BOGUS 1 2 3", 1500);
        check(tag, "unknown cmd -> ERR", badCmd != null && badCmd.startsWith("ERR"), String.valueOf(badCmd));

        String badArgs = link.cmd("tap x y", 1500);
        check(tag, "garbage args -> ERR", badArgs != null && badArgs.startsWith("ERR"), String.valueOf(badArgs));

        // Shell help tests
        link.drain();
        link.send("help");
        List<String> helpLines = new ArrayList<>();
        for (String line; (line = link.line(400)) != null;) {
            helpLines.add(line);
        }
        check(tag, "help lists the commands",
                !helpLines.isEmpty() && helpLines.get(0).startsWith("OK HELP")
                        && helpLines.stream().anyMatch(l -> l.startsWith("layer "))
                        && helpLines.stream().anyMatch(l -> l.startsWith("telem ")),
                helpLines.size() + " lines");

        link.drain();
        link.send("help layer");
        List<String> layerHelp = new ArrayList<>();
        for (String line; (line = link.line(400)) != null;) {
            layerHelp.add(line);
        }
        check(tag, "help layer explains the command",
                layerHelp.size() >= 2 && "OK HELP layer".equals(layerHelp.get(0))
                        && layerHelp.stream().anyMatch(l -> l.contains("layer <n>")),
                layerHelp.size() + " lines");

        String unknownHelp = link.cmd("help nonsense", 1500);
        check(tag, "help of an unknown command -> ERR",
                unknownHelp != null && unknownHelp.startsWith("ERR"), String.valueOf(unknownHelp));

        String upperCase = link.cmd("PING", 1500);
        check(tag, "upper case commands are not accepted any more",
                upperCase != null && upperCase.startsWith("ERR"), String.valueOf(upperCase));

        int pages = Integer.parseInt(info.get("pages"));
        int locks = Integer.parseInt(info.get("locks"));
        int width = Integer.parseInt(info.get("w"));
        int height = Integer.parseInt(info.get("h"));

        if ("1".equals(info.get("presenter"))) {
            presenterChecks(tag, link, info, shotDir);
            return;
        }

        reply = link.cmd("layer 0", 2000);
        check(tag, "LAYER 0", reply != null && reply.startsWith("OK LAYER 0"), String.valueOf(reply));

        reply = link.cmd("state", 1500);
        check(tag, "STATE? page=0",
                reply != null && parseKeyValue(reply).getOrDefault("page", "").startsWith("0/"),
                String.valueOf(reply));

        // TAP outside Locks page must be refused for safety
        reply = link.cmd("tap 10 10", 1500);
        check(tag, "TAP outside the Locks page is refused",
                reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        reply = link.cmd("layer " + pages, 1500);
        check(tag, "LAYER out of range -> ERR",
                reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        link.cmd("layer " + locks, 2000);
        reply = link.cmd("state", 1500);
        check(tag, "STATE? title=Locks",
                reply != null && "Locks".equals(parseKeyValue(reply).getOrDefault("title", "")),
                String.valueOf(reply));

        if (shotDir != null) {
            saveShot(tag, link, shotDir);
            for (int p = 0; p < pages; p++) {
                link.cmd("layer " + p, 2000);
                Thread.sleep(300);
                saveShot(tag, link, shotDir.resolve(tag), String.format("page%02d.png", p));
            }
            link.cmd("layer " + locks, 2000);
        }

        // Host evdev key capture on Locks page (Scroll Lock, index 2)
        String evdevNode = "S3".equals(tag) ? findEventNode(info.get("name") + " Keyboard", true) : null;
        Map<String, String> state0 = parseKeyValue(link.cmd("state", 1500));
        int scroll0 = Integer.parseInt(state0.get("scroll"));
        int presses0 = Integer.parseInt(state0.get("presses"));

        List<KeyEvent> keys = null;
        if (evdevNode != null) {
            try {
                keys = captureKeys(evdevNode, () -> {
                    try {
                        link.cmd("key " + locks + " 2", 1500);
                    } catch (Exception ignored) {}
                }, 600);
            } catch (IOException e) {
                report.add("INFO [" + tag + "] cannot read " + evdevNode + ": " + e.getMessage());
                evdevNode = null;
            }
        }

        if (evdevNode == null) {
            link.cmd("key " + locks + " 2", 1500);
        }
        Thread.sleep(500);

        Map<String, String> state1 = parseKeyValue(link.cmd("state", 1500));
        int presses1 = Integer.parseInt(state1.get("presses"));
        check(tag, "KEY locks 2: presses++", presses1 == presses0 + 1,
                state0.get("presses") + "->" + state1.get("presses"));

        report.add("INFO [" + tag + "] host scroll LED " + scroll0 + "->" + state1.get("scroll") + " (not asserted)");

        if (keys != null) {
            boolean down = keys.stream().anyMatch(k -> k.code() == 70 && k.value() == 1);
            boolean up = keys.stream().anyMatch(k -> k.code() == 70 && k.value() == 0);
            check(tag, "host evdev sees KEY_SCROLLLOCK down+up", down && up,
                    evdevNode + " events=" + keys.size());
        } else if ("S3".equals(tag)) {
            report.add("SKIP [" + tag + "] evdev check (device node not found/readable)");
        }

        // Restore scroll lock state
        link.cmd("key " + locks + " 2", 1500);
        Thread.sleep(400);

        // TAP on Locks tile 2 (Scroll Lock)
        int gap = 3;
        int barHeight = "C2".equals(tag) ? 24 : 38;
        int tileW = (width - 4 * gap) / 3;
        int rowH = (height - barHeight - 3 * gap) / 2;
        int touchX = gap + 2 * (tileW + gap) + tileW / 2;
        int touchY = ("C2".equals(tag) ? barHeight : 0) + gap + rowH / 2;

        int p0 = Integer.parseInt(parseKeyValue(link.cmd("state", 1500)).get("presses"));
        reply = link.cmd("tap " + touchX + " " + touchY, 1500);
        check(tag, "TAP accepted", "OK TAP".equals(reply), String.valueOf(reply));
        Thread.sleep(600);

        Map<String, String> state2 = parseKeyValue(link.cmd("state", 1500));
        int p1 = Integer.parseInt(state2.get("presses"));
        String lastKey = state2.getOrDefault("last", "").replace("_", " ");
        check(tag, "TAP on Scroll tile counted",
                p1 == p0 + 1 && lastKey.toLowerCase().startsWith("scroll"),
                "presses " + p0 + "->" + p1 + " last=" + state2.get("last"));

        link.cmd("key " + locks + " 2", 1500);
        check(tag, "STATE? reports usb= and ble= next to conn=",
                state2.containsKey("usb") && state2.containsKey("ble") && state2.containsKey("conn"),
                String.valueOf(state2.keySet()));

        pressChecks(tag, link, width, height, locks);
        settingsChecks(tag, link);
        systemControlCheck(tag, link, info);
        functionKeysCheck(tag, link, info);
        if ("S3".equals(tag)) {
            testBle(tag, link, info, locks);
        }
        link.cmd("layer 0", 2000);
    }

    /**
     * Presentation clicker and air-mouse functional checks (Core2 presenter firmware).
     *
     * @param tag board tag ("C2")
     * @param link serial link to the board
     * @param info board identity key-values
     * @param shotDir directory for screenshots, or null
     * @throws Exception on communication error
     */
    static void presenterChecks(String tag, Link link, Map<String, String> info, Path shotDir) throws Exception {
        final int AMBER = 0xFF9900;
        final int PEACH = 0xFFCC99;
        int width = Integer.parseInt(info.get("w"));
        int height = Integer.parseInt(info.get("h"));

        check(tag, "one page, no Locks page",
                "1".equals(info.get("pages")) && "255".equals(info.get("locks")),
                info.get("pages") + " pages, locks=" + info.get("locks"));

        String reply = link.cmd("layer 0", 2000);
        check(tag, "LAYER 0", reply != null && reply.startsWith("OK LAYER 0"), String.valueOf(reply));

        reply = link.cmd("layer 1", 1500);
        check(tag, "LAYER 1 -> ERR (one page)", reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        reply = link.cmd("key 0 0", 1500);
        check(tag, "key by number is refused", reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        reply = link.cmd("clicker bogus", 1500);
        check(tag, "clicker with an unknown action -> ERR", reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        link.cmd("pointer off", 1500);
        Map<String, String> pointerState = parseKeyValue(link.cmd("pointer", 1500));
        check(tag, "pointer is off", "off".equals(pointerState.get("pointer")), String.valueOf(pointerState));

        if (shotDir != null) {
            saveShot(tag, link, shotDir, "presenter-" + tag + ".png");
        }

        BufferedImage big = grab(link);
        boolean bigOk = big != null
                && nearRgb(big.getRGB(240, 130) & 0xFFFFFF, AMBER, 48)
                && nearRgb(big.getRGB(60, 50) & 0xFFFFFF, 0x9999FF, 48);
        check(tag, "Next is the big amber block at the right, Previous at the left",
                bigOk, big == null ? "no image" : String.format("right %06x left %06x",
                        big.getRGB(240, 130) & 0xFFFFFF, big.getRGB(60, 50) & 0xFFFFFF));

        int gap = 3;
        int bar = 24;
        int ah = height - bar - 2 * gap;
        int wl = (width - 3 * gap) / 2;
        int ws = (wl - gap) / 2;
        int h1 = ah * 40 / 100;
        int hs = (ah - h1 - 2 * gap) / 2;
        int ys2 = 2 * gap + h1 + hs + gap;
        int tx = 2 * gap + ws + ws / 2;
        int ty = bar + ys2 + hs / 2;
        int sx = 2 * gap + ws + 14;
        int sy = bar + ys2 + 14;

        BufferedImage idle = grab(link);
        int c0 = idle == null ? 0 : idle.getRGB(sx, sy) & 0xFFFFFF;
        int p0 = Integer.parseInt(parseKeyValue(link.cmd("state", 1500)).get("presses"));

        reply = link.cmd("down " + tx + " " + ty, 1500);
        check(tag, "down on the Pointer mode key accepted", "OK DOWN".equals(reply), String.valueOf(reply));
        Thread.sleep(300);

        BufferedImage held = grab(link);
        int c1 = held == null ? 0 : held.getRGB(sx, sy) & 0xFFFFFF;
        check(tag, "tile turns amber while the touch is held",
                nearRgb(c1, AMBER, 48) && !nearRgb(c0, AMBER, 48),
                String.format("idle %06x held %06x", c0, c1));

        link.cmd("up", 1500);
        Thread.sleep(400);

        Map<String, String> stateAfterUp = parseKeyValue(link.cmd("state", 1500));
        boolean clicked = Integer.parseInt(stateAfterUp.get("presses")) != p0;
        check(tag, "up releases without a click",
                !clicked && "off".equals(stateAfterUp.get("ptr")),
                "presses " + p0 + "->" + stateAfterUp.get("presses") + " ptr=" + stateAfterUp.get("ptr"));

        if (clicked) {
            link.cmd("pointer off", 1500);
        }

        // Button B down/up
        reply = link.cmd("btn b down", 1500);
        check(tag, "btn b down accepted", "OK BTN DOWN".equals(reply), String.valueOf(reply));
        Thread.sleep(300);

        int bx = 222 + 32 + 3;
        int by = 12;
        BufferedImage bh = grab(link);
        Map<String, String> sb = parseKeyValue(link.cmd("state", 1500));
        check(tag, "B lights its indicator and switches the pointer on (still)",
                bh != null && nearRgb(bh.getRGB(bx, by) & 0xFFFFFF, AMBER, 48)
                        && "still".equals(sb.get("ptr")) && "2".equals(sb.get("btn")),
                String.format("indicator %06x ptr=%s btn=%s",
                        bh == null ? 0 : bh.getRGB(bx, by) & 0xFFFFFF, sb.get("ptr"), sb.get("btn")));

        link.cmd("btn b up", 1500);
        Thread.sleep(300);
        BufferedImage bu = grab(link);
        check(tag, "B up puts the indicator out",
                bu != null && nearRgb(bu.getRGB(bx, by) & 0xFFFFFF, PEACH, 24), "");

        Thread.sleep(400);
        if (shotDir != null) {
            saveShot(tag, link, shotDir, "presenter-on-" + tag + ".png");
        }

        BufferedImage pm = grab(link);
        check(tag, "pointer mode is a different page: a big peach Click block",
                pm != null && nearRgb(pm.getRGB(60, 50) & 0xFFFFFF, PEACH, 48),
                pm == null ? "no image" : String.format("%06x", pm.getRGB(60, 50) & 0xFFFFFF));

        // Pointer movement and sleep checks
        link.cmd("pointer on", 1500);
        pointerState = parseKeyValue(link.cmd("pointer", 1500));
        long rep0 = Long.parseLong(pointerState.get("reports"));
        check(tag, "pointer on: waiting (still)", "still".equals(pointerState.get("pointer")),
                String.valueOf(pointerState));

        link.cmd("pointer feed 60 20", 1500);
        Thread.sleep(350);
        pointerState = parseKeyValue(link.cmd("pointer", 1500));
        check(tag, "moving the board wakes the pointer",
                "moving".equals(pointerState.get("pointer")) && Long.parseLong(pointerState.get("reports")) > rep0,
                String.valueOf(pointerState));

        Thread.sleep(3800);
        pointerState = parseKeyValue(link.cmd("pointer", 1500));
        check(tag, "held still it falls asleep by itself", "still".equals(pointerState.get("pointer")),
                String.valueOf(pointerState));

        long rep1 = Long.parseLong(pointerState.get("reports"));
        Thread.sleep(600);
        check(tag, "asleep it sends nothing",
                Long.parseLong(parseKeyValue(link.cmd("pointer", 1500)).get("reports")) == rep1, "");

        link.cmd("pointer feed 60 0", 1500);
        Thread.sleep(350);
        check(tag, "and wakes up when it is moved again",
                "moving".equals(parseKeyValue(link.cmd("pointer", 1500)).get("pointer")), "");

        reply = link.cmd("pointer off", 1500);
        check(tag, "pointer off",
                reply != null && reply.contains("pointer=off")
                        && "off".equals(parseKeyValue(link.cmd("state", 1500)).get("ptr")),
                String.valueOf(reply));

        Thread.sleep(300);
        check(tag, "off it sends nothing, even when moved", hasNoMovement(link), "");

        // Settings adjustments
        check(tag, "pointer gain 30", "OK POINTER SET".equals(link.cmd("pointer gain 30", 1500)), "");
        check(tag, "pointer gain 0 -> ERR", String.valueOf(link.cmd("pointer gain 0", 1500)).startsWith("ERR"), "");
        check(tag, "pointer idle 2", "OK POINTER SET".equals(link.cmd("pointer idle 2", 1500)), "");
        check(tag, "pointer upright on/off",
                "OK POINTER SET".equals(link.cmd("pointer upright on", 1500))
                        && "OK POINTER SET".equals(link.cmd("pointer upright off", 1500)), "");
        check(tag, "pointer flipx and flipy",
                "OK POINTER SET".equals(link.cmd("pointer flipx on", 1500))
                        && "OK POINTER SET".equals(link.cmd("pointer flipx off", 1500))
                        && "OK POINTER SET".equals(link.cmd("pointer flipy off", 1500)), "");

        link.cmd("pointer gain 20", 1500);
        settingsChecks(tag, link);
    }

    /**
     * Checks if pointer reports counter remains unchanged after feeding movement.
     *
     * @param link serial link
     * @return true if no movement reports occurred
     * @throws Exception on communication error
     */
    static boolean hasNoMovement(Link link) throws Exception {
        long reportsBefore = Long.parseLong(parseKeyValue(link.cmd("pointer", 1500)).get("reports"));
        link.cmd("pointer feed 90 90", 1500);
        Thread.sleep(800);
        long reportsAfter = Long.parseLong(parseKeyValue(link.cmd("pointer", 1500)).get("reports"));
        return reportsAfter == reportsBefore;
    }

    /**
     * Reads a screenshot directly from the board via the {@code shot} command.
     *
     * @param link serial link
     * @return BufferedImage of board framebuffer, or null if timed out or failed
     * @throws Exception on communication error
     */
    static BufferedImage grab(Link link) throws Exception {
        link.drain();
        link.send("shot");
        String header = null;
        long deadline = System.currentTimeMillis() + 5000;
        while (System.currentTimeMillis() < deadline) {
            String s = link.line(1000);
            if (s != null && s.startsWith("OK SHOT")) {
                header = s;
                break;
            }
        }
        if (header == null) return null;

        String[] tokens = header.split("\\s+");
        int width = Integer.parseInt(tokens[2]);
        int height = Integer.parseInt(tokens[3]);
        byte[] pixels = link.readBytes(width * height * 3, 60000);
        link.drain();

        if (pixels == null) return null;

        BufferedImage image = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        for (int i = 0; i < width * height; i++) {
            int r = pixels[i * 3] & 0xFF;
            int g = pixels[i * 3 + 1] & 0xFF;
            int b = pixels[i * 3 + 2] & 0xFF;
            image.setRGB(i % width, i / width, (r << 16) | (g << 8) | b);
        }
        return image;
    }

    /**
     * Checks if two 24-bit RGB values are within per-channel tolerance.
     *
     * @param rgb actual RGB value
     * @param want expected RGB value
     * @param tolerance maximum channel difference
     * @return true if red, green, and blue differences are all <= tolerance
     */
    static boolean nearRgb(int rgb, int want, int tolerance) {
        int rDiff = Math.abs(((rgb >> 16) & 0xFF) - ((want >> 16) & 0xFF));
        int gDiff = Math.abs(((rgb >> 8) & 0xFF) - ((want >> 8) & 0xFF));
        int bDiff = Math.abs((rgb & 0xFF) - (want & 0xFF));
        return rDiff <= tolerance && gDiff <= tolerance && bDiff <= tolerance;
    }

    /**
     * Validates touch press and hold mechanics on the Locks page.
     *
     * @param tag board tag ("C2" or "S3")
     * @param link serial link
     * @param width board display width
     * @param height board display height
     * @param locks Locks layer index
     * @throws Exception on communication error
     */
    static void pressChecks(String tag, Link link, int width, int height, int locks) throws Exception {
        final int AMBER = 0xFF9900;
        final int PEACH = 0xFFCC99;
        link.cmd("layer " + locks, 2000);

        int gap = 3;
        int barHeight = "C2".equals(tag) ? 24 : 38;
        int tileW = (width - 4 * gap) / 3;
        int tileH = (height - barHeight - 3 * gap) / 2;
        int topOffset = "C2".equals(tag) ? barHeight : 0;
        int x2 = gap + 2 * (tileW + gap);
        int tx = x2 + tileW / 2;
        int ty = topOffset + gap + tileH / 2;
        int sx = x2 + 16;
        int sy = topOffset + gap + 16;

        BufferedImage idle = grab(link);
        if (idle == null) {
            check(tag, "press: screenshot", false, "no image");
            return;
        }

        int c0 = idle.getRGB(sx, sy) & 0xFFFFFF;
        int p0 = Integer.parseInt(parseKeyValue(link.cmd("state", 1500)).get("presses"));

        String reply = link.cmd("down " + tx + " " + ty, 1500);
        check(tag, "down accepted", "OK DOWN".equals(reply), String.valueOf(reply));
        Thread.sleep(300);

        BufferedImage held = grab(link);
        int c1 = held == null ? 0 : held.getRGB(sx, sy) & 0xFFFFFF;
        check(tag, "tile turns amber while the touch is held",
                nearRgb(c1, AMBER, 48) && !nearRgb(c0, AMBER, 48),
                String.format("idle %06x held %06x", c0, c1));

        check(tag, "state shows touch=1", "1".equals(parseKeyValue(link.cmd("state", 1500)).get("touch")), "");

        reply = link.cmd("up", 1500);
        check(tag, "up accepted", "OK UP".equals(reply), String.valueOf(reply));
        Thread.sleep(500);

        BufferedImage after = grab(link);
        int c2 = after == null ? 0 : after.getRGB(sx, sy) & 0xFFFFFF;
        Map<String, String> stateAfter = parseKeyValue(link.cmd("state", 1500));
        boolean clicked = Integer.parseInt(stateAfter.get("presses")) != p0;
        check(tag, "tile returns to its colour after up, no key sent",
                nearRgb(c2, c0, 8) && !clicked && "0".equals(stateAfter.get("touch")),
                String.format("after %06x presses %d->%s touch=%s", c2, p0, stateAfter.get("presses"), stateAfter.get("touch")));

        if (clicked) {
            link.cmd("key " + locks + " 2", 1500);
            Thread.sleep(300);
        }

        reply = link.cmd("up", 1500);
        check(tag, "up without down -> ERR", reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        link.cmd("down 10 " + (topOffset == 0 ? 10 : height - 10), 1500);
        link.cmd("up", 1500);
        Thread.sleep(300);

        if ("C2".equals(tag)) {
            int bx = 222 + 32 + 3;
            int by = 12;
            BufferedImage a0 = grab(link);
            reply = link.cmd("btn b down", 1500);
            check(tag, "btn b down accepted", "OK BTN DOWN".equals(reply), String.valueOf(reply));
            Thread.sleep(300);

            BufferedImage a1 = grab(link);
            Map<String, String> sb = parseKeyValue(link.cmd("state", 1500));
            check(tag, "indicator B turns amber while held, btn=2",
                    a0 != null && a1 != null
                            && nearRgb(a0.getRGB(bx, by) & 0xFFFFFF, PEACH, 24)
                            && nearRgb(a1.getRGB(bx, by) & 0xFFFFFF, AMBER, 48)
                            && "2".equals(sb.get("btn")),
                    String.format("idle %06x held %06x btn=%s",
                            a0 == null ? 0 : a0.getRGB(bx, by) & 0xFFFFFF,
                            a1 == null ? 0 : a1.getRGB(bx, by) & 0xFFFFFF, sb.get("btn")));

            reply = link.cmd("btn b up", 1500);
            Thread.sleep(300);
            BufferedImage a2 = grab(link);
            check(tag, "btn b up puts the indicator out",
                    "OK BTN UP".equals(reply) && a2 != null
                            && nearRgb(a2.getRGB(bx, by) & 0xFFFFFF, PEACH, 24)
                            && "0".equals(parseKeyValue(link.cmd("state", 1500)).get("btn")),
                    String.valueOf(reply));

            reply = link.cmd("btn d down", 1500);
            check(tag, "btn d -> ERR", reply != null && reply.startsWith("ERR"), String.valueOf(reply));
        } else {
            reply = link.cmd("btn a down", 1500);
            check(tag, "btn on the CoreS3 -> ERR (no buttons)",
                    reply != null && reply.startsWith("ERR"), String.valueOf(reply));
        }

        link.cmd("layer " + locks, 2000);
    }

    /**
     * Tests EEPROM/NVS persistent settings configuration commands.
     *
     * @param tag board tag ("C2" or "S3")
     * @param link serial link
     * @throws Exception on communication error
     */
    static void settingsChecks(String tag, Link link) throws Exception {
        String reply = link.cmd("set", 1500);
        check(tag, "SET? lists the settings", reply != null && reply.startsWith("OK SET led="), String.valueOf(reply));

        for (String mode : new String[]{"OFF", "LOCKS", "ON"}) {
            reply = link.cmd("leds " + mode, 1500);
            String q = link.cmd("leds", 1500);
            check(tag, "LEDS " + mode,
                    ("OK LEDS " + mode).equals(reply) && q != null && q.contains("led=" + mode.toLowerCase()),
                    reply + " / " + q);
        }

        reply = link.cmd("set bright 500", 1500);
        check(tag, "SET bright out of range -> ERR", reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        reply = link.cmd("set nonsense 1", 1500);
        check(tag, "SET unknown key -> ERR", reply != null && reply.startsWith("ERR"), String.valueOf(reply));

        reply = link.cmd("set bright 60", 1500);
        check(tag, "SET bright 60", reply != null && reply.contains("bright=60"), String.valueOf(reply));

        reply = link.cmd("bl 100", 1500);
        check(tag, "BL 100", "OK BL 100".equals(reply), String.valueOf(reply));

        reply = link.cmd("bl auto", 1500);
        check(tag, "BL AUTO", "OK BL AUTO".equals(reply), String.valueOf(reply));

        reply = link.cmd("wake", 1500);
        check(tag, "WAKE", "OK WAKE".equals(reply), String.valueOf(reply));

        reply = link.cmd("set defaults 1", 1500);
        check(tag, "SET defaults restores the stored settings",
                reply != null && reply.contains("led=on bright=40 ledidle=0 bl=128 dim=60 off=300 prefer=usb"),
                String.valueOf(reply));
    }

    /**
     * Validates System Control HID reports (power down, sleep, wake up) via KEY_WAKEUP (143).
     *
     * @param tag board tag ("C2" or "S3")
     * @param link serial link
     * @param info board identity map
     * @throws Exception on communication error
     */
    static void systemControlCheck(String tag, Link link, Map<String, String> info) throws Exception {
        boolean s3 = "S3".equals(tag);
        String evdevNode = findEventNode(s3 ? info.get("name") + " Keyboard" : "-c2 Keyboard", s3);
        if (evdevNode == null) {
            report.add("SKIP [" + tag + "] System Control check (no evdev device: board not connected)");
            return;
        }

        int mediaLayer = 18;
        int wakeTile = 11;
        List<KeyEvent> keys;
        try {
            keys = captureKeys(evdevNode, () -> {
                try {
                    link.cmd("key " + mediaLayer + " " + wakeTile, 1500);
                } catch (Exception ignored) {}
            }, 800);
        } catch (IOException e) {
            report.add("INFO [" + tag + "] cannot read " + evdevNode + ": " + e.getMessage());
            return;
        }

        boolean down = keys.stream().anyMatch(k -> k.code() == 143 && k.value() == 1);
        boolean up = keys.stream().anyMatch(k -> k.code() == 143 && k.value() == 0);
        check(tag, "System Control: Wake Up tile reaches the host (KEY_WAKEUP down+up)",
                down && up, evdevNode + " events=" + keys.size());
    }

    /**
     * Validates function keys F13-F24 (Linux codes 183-194) used for block transfer acknowledgements.
     *
     * @param tag board tag ("C2" or "S3")
     * @param link serial link
     * @param info board identity map
     * @throws Exception on communication error
     */
    static void functionKeysCheck(String tag, Link link, Map<String, String> info) throws Exception {
        boolean s3 = "S3".equals(tag);
        String evdevNode = findEventNode(s3 ? info.get("name") + " Keyboard" : "-c2 Keyboard", s3);
        if (evdevNode == null) {
            report.add("SKIP [" + tag + "] function key check (no evdev device)");
            return;
        }

        int[][] tiles = {
            {7, 8, 13}, {7, 3, 14}, {7, 4, 15}, {7, 5, 16}, {7, 0, 17}, {7, 1, 18},
            {7, 2, 19}, {7, 10, 20}, {10, 2, 21}, {10, 3, 22}, {10, 0, 23}, {10, 1, 24}
        };

        StringBuilder failures = new StringBuilder();
        for (int[] tile : tiles) {
            int expectedCode = tile[2] <= 12 ? 58 + tile[2] : 170 + tile[2];
            List<KeyEvent> keys = captureKeys(evdevNode, () -> {
                try {
                    link.cmd("key " + tile[0] + " " + tile[1], 1500);
                } catch (Exception ignored) {}
            }, 500);

            boolean down = keys.stream().anyMatch(k -> k.code() == expectedCode && k.value() == 1);
            boolean up = keys.stream().anyMatch(k -> k.code() == expectedCode && k.value() == 0);

            if (!(down && up)) {
                failures.append(" F").append(tile[2]);
            } else {
                info("F" + tile[2] + " arrives as evdev code " + expectedCode);
            }
        }

        check(tag, "F13 to F24 each reach the host as their own key",
                failures.isEmpty(), failures.toString().trim());
    }

    /**
     * Validates whether a captured event list contains down+up of Scroll Lock (code 70).
     *
     * @param keys list of key events
     * @return true if both press (1) and release (0) were captured
     */
    static boolean hasScrollDownUp(List<KeyEvent> keys) {
        if (keys == null) return false;
        boolean down = keys.stream().anyMatch(k -> k.code() == 70 && k.value() == 1);
        boolean up = keys.stream().anyMatch(k -> k.code() == 70 && k.value() == 0);
        return down && up;
    }

    /**
     * Presses Scroll Lock on the Locks page and captures evdev events on the target node.
     *
     * @param link serial link
     * @param evdevNode target evdev node path
     * @param locks Locks layer index
     * @return list of captured events, or null on error
     * @throws Exception on communication error
     */
    static List<KeyEvent> pressAndCapture(Link link, String evdevNode, int locks) throws Exception {
        try {
            return captureKeys(evdevNode, () -> {
                try {
                    link.cmd("key " + locks + " 2", 1500);
                } catch (Exception ignored) {}
            }, 800);
        } catch (IOException e) {
            report.add("INFO cannot read " + evdevNode + ": " + e.getMessage());
            return null;
        }
    }

    /**
     * Tests transport routing between USB and BLE interfaces.
     *
     * @param tag board tag
     * @param link serial link
     * @param modeCmd mode switch command
     * @param okReply expected command response
     * @param usbEv USB evdev node
     * @param bleEv BLE evdev node
     * @param locks Locks layer index
     * @param expectUsb true if USB device should receive key event
     * @param expectBle true if BLE device should receive key event
     * @throws Exception on communication error
     */
    static void routeCheck(String tag, Link link, String modeCmd, String okReply,
                           String usbEv, String bleEv, int locks,
                           boolean expectUsb, boolean expectBle) throws Exception {
        String reply = link.cmd(modeCmd.toLowerCase(), 1500);
        check(tag, modeCmd, okReply.equals(reply), String.valueOf(reply));

        List<KeyEvent> usbEvents = pressAndCapture(link, usbEv, locks);
        List<KeyEvent> bleEvents = pressAndCapture(link, bleEv, locks);

        check(tag, modeCmd + ": USB device " + (expectUsb ? "sees" : "stays silent"),
                usbEvents != null && (expectUsb ? hasScrollDownUp(usbEvents) : usbEvents.isEmpty()),
                usbEv + " events=" + (usbEvents == null ? -1 : usbEvents.size()));

        check(tag, modeCmd + ": BLE device " + (expectBle ? "sees" : "stays silent"),
                bleEvents != null && (expectBle ? hasScrollDownUp(bleEvents) : bleEvents.isEmpty()),
                bleEv + " events=" + (bleEvents == null ? -1 : bleEvents.size()));
    }

    /**
     * Verifies Bluetooth LE and USB routing behavior on CoreS3.
     *
     * @param tag board tag ("S3")
     * @param link serial link
     * @param info board identity map
     * @param locks Locks layer index
     * @throws Exception on communication error
     */
    static void testBle(String tag, Link link, Map<String, String> info, int locks) throws Exception {
        String status = link.cmd("prefer", 1500);
        Map<String, String> map = (status != null && status.startsWith("OK"))
                ? parseKeyValue(status) : Map.of();
        check(tag, "BLE connected (board reports ble=1)", "1".equals(map.get("ble")), String.valueOf(status));

        String bleEv = findEventNode(("S3".equals(tag) ? "-cs3" : "-c2") + " Keyboard", false);
        String usbEv = findEventNode(info.get("name") + " Keyboard", true);

        if (bleEv == null || usbEv == null || !"1".equals(map.get("ble")) || !"1".equals(map.get("usb"))) {
            report.add("SKIP [" + tag + "] BLE/USB routing checks (need both host devices and both links up)");
            return;
        }

        routeCheck(tag, link, "PREFER USB",  "OK PREFER USB",  usbEv, bleEv, locks, true,  false);
        routeCheck(tag, link, "PREFER BLE",  "OK PREFER BLE",  usbEv, bleEv, locks, false, true);
        routeCheck(tag, link, "ONLY USB",    "OK ONLY USB",    usbEv, bleEv, locks, true,  false);
        routeCheck(tag, link, "ONLY BLE",    "OK ONLY BLE",    usbEv, bleEv, locks, false, true);
        routeCheck(tag, link, "PREFER BOTH", "OK PREFER BOTH", usbEv, bleEv, locks, true,  true);

        String reply = link.cmd("prefer usb", 1500);
        check(tag, "PREFER USB restored", "OK PREFER USB".equals(reply), String.valueOf(reply));
    }

    /**
     * Captures and saves a PNG screenshot under the default filename.
     *
     * @param tag board tag
     * @param link serial link
     * @param dir destination directory
     * @throws Exception on error
     */
    static void saveShot(String tag, Link link, Path dir) throws Exception {
        saveShot(tag, link, dir, "shot-" + tag + ".png");
    }

    /**
     * Captures and saves a PNG screenshot under the specified filename.
     *
     * @param tag board tag
     * @param link serial link
     * @param dir destination directory
     * @param filename output file name
     * @throws Exception on error
     */
    static void saveShot(String tag, Link link, Path dir, String filename) throws Exception {
        BufferedImage image = grab(link);
        if (image == null) {
            check(tag, "SHOT", false, "no image captured");
            return;
        }
        Files.createDirectories(dir);
        Path target = dir.resolve(filename);
        ImageIO.write(image, "png", target.toFile());
        check(tag, "SHOT saved", Files.size(target) > 500, target.toString());
    }

    /**
     * Times round-trip readout of a framebuffer screenshot in milliseconds.
     *
     * @param link serial link
     * @return duration in milliseconds, or -1 on error
     * @throws Exception on error
     */
    static double timeShot(Link link) throws Exception {
        link.drain();
        long start = System.nanoTime();
        link.send("shot");
        String header = null;
        long deadline = System.currentTimeMillis() + 5000;
        while (System.currentTimeMillis() < deadline) {
            String s = link.line(1000);
            if (s != null && s.startsWith("OK SHOT")) {
                header = s;
                break;
            }
        }
        if (header == null) return -1;

        String[] tokens = header.split("\\s+");
        int width = Integer.parseInt(tokens[2]);
        int height = Integer.parseInt(tokens[3]);
        byte[] pixels = link.readBytes(width * height * 3, 60000);
        double ms = (System.nanoTime() - start) / 1e6;
        link.drain();
        return pixels == null ? -1 : ms;
    }

    /**
     * Calculates median value of a list of floating-point numbers.
     *
     * @param values input values
     * @return median value or NaN if empty
     */
    static double calculateMedian(List<Double> values) {
        if (values.isEmpty()) return Double.NaN;
        List<Double> sorted = new ArrayList<>(values);
        Collections.sort(sorted);
        return sorted.get(sorted.size() / 2);
    }

    /**
     * Benchmarks UI layer switch, frame building, rendering, and panel flush latency.
     *
     * @param tag board tag
     * @param link serial link
     * @param info board identity map
     * @throws Exception on communication error
     */
    static void latency(String tag, Link link, Map<String, String> info) throws Exception {
        int locks = Integer.parseInt(info.get("locks"));
        int reps = "C2".equals(tag) ? 1 : 3;
        int[] samplePages = {1, 13, 14, locks};

        List<Double> pingTimes = new ArrayList<>();
        List<Double> readoutTimes = new ArrayList<>();
        List<Double> ackTimes = new ArrayList<>();
        List<Double> totalTimes = new ArrayList<>();
        List<Double> buildTimes = new ArrayList<>();
        List<Double> renderTimes = new ArrayList<>();
        List<Double> flushTimes = new ArrayList<>();

        link.cmd("layer 0", 3000);
        Thread.sleep(600);

        for (int i = 0; i < 20; i++) {
            long t0 = System.nanoTime();
            link.cmd("ping", 1000);
            pingTimes.add((System.nanoTime() - t0) / 1e6);
        }

        for (int i = 0; i < 2; i++) {
            readoutTimes.add(timeShot(link));
        }

        for (int r = 0; r < reps; r++) {
            for (int page : samplePages) {
                link.drain();
                long t0 = System.nanoTime();
                String ack = link.cmd("layer " + page, 5000);
                double tAck = (System.nanoTime() - t0) / 1e6;
                double shot = timeShot(link);

                if (ack == null || shot < 0) {
                    report.add("FAIL [" + tag + "] latency sample for page " + page);
                    failCount++;
                    continue;
                }

                ackTimes.add(tAck);
                totalTimes.add((System.nanoTime() - t0) / 1e6);

                String perf = link.cmd("perf", 1500);
                if (perf != null && perf.startsWith("OK PERF")) {
                    Map<String, String> kv = parseKeyValue(perf);
                    buildTimes.add(Double.parseDouble(kv.get("build_ms")));
                    renderTimes.add(Double.parseDouble(kv.get("render_ms")));
                    flushTimes.add(Double.parseDouble(kv.get("flush_ms")));
                }
            }
        }

        link.cmd("layer 0", 3000);
        Thread.sleep(800);
        link.drain();

        // Fast burst paging test
        long burstStart = System.nanoTime();
        for (int i = 1; i <= 10; i++) {
            link.send("layer " + i);
            Thread.sleep(150);
        }

        String lastReply = null;
        long burstDeadline = System.currentTimeMillis() + 90000;
        while (System.currentTimeMillis() < burstDeadline) {
            String line = link.line(1000);
            if (line != null && line.startsWith("OK LAYER 10")) {
                lastReply = line;
                break;
            }
        }

        double burstShot = timeShot(link);
        double burstMs = (System.nanoTime() - burstStart) / 1e6 - calculateMedian(readoutTimes);
        link.cmd("layer 0", 5000);

        if (lastReply == null || burstShot < 0) {
            report.add("FAIL [" + tag + "] fast paging burst did not complete");
            failCount++;
        } else {
            report.add(String.format("INFO [%s] fast paging: 10 page changes 150 ms apart, last page on screen %.0f ms after the first press (%.0f ms after the last press)",
                    tag, burstMs, burstMs - 1500));
        }

        report.add(String.format("INFO [%s] latency: PING %.0f ms, SHOT read-out %.0f ms, LAYER ack %.0f ms, LAYER+SHOT %.0f ms",
                tag, calculateMedian(pingTimes), calculateMedian(readoutTimes), calculateMedian(ackTimes), calculateMedian(totalTimes)));
        report.add(String.format("INFO [%s] page on screen: %.0f ms (median of %d samples, pages %s)",
                tag, calculateMedian(totalTimes) - calculateMedian(readoutTimes), totalTimes.size(), Arrays.toString(samplePages)));

        if (!buildTimes.isEmpty()) {
            report.add(String.format("INFO [%s] firmware split (PERF?, medians): build %.0f ms, render %.0f ms of which flush to the panel %.0f ms",
                    tag, calculateMedian(buildTimes), calculateMedian(renderTimes), calculateMedian(flushTimes)));
        } else {
            report.add("INFO [" + tag + "] PERF? not available on this firmware");
        }
    }

    /**
     * Program entry point. Parses command-line arguments, connects to matching boards,
     * and runs functional tests or latency benchmarks.
     *
     * @param args command-line arguments
     * @throws Exception on execution failure
     */
    public static void main(String[] args) throws Exception {
        String targetBoard = null;
        String forcePort = null;
        Path shotDirectory = null;
        boolean measureLatency = false;

        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--board" -> targetBoard = args[++i].toUpperCase();
                case "--shot" -> shotDirectory = Path.of(args[++i]);
                case "--port" -> forcePort = args[++i];
                case "--latency" -> measureLatency = true;
                default -> {
                    System.err.println("unknown arg: " + args[i]);
                    System.exit(2);
                }
            }
        }

        int boardsFound = 0;
        for (SerialPort port : SerialPort.getCommPorts()) {
            if (!isCandidate(port)) continue;
            if (forcePort != null && !port.getSystemPortPath().equals(forcePort)) continue;

            try (Link link = new Link(port)) {
                if (!link.open()) {
                    report.add("SKIP " + port.getSystemPortPath() + ": cannot open");
                    continue;
                }
                link.drain();
                String infoReply = null;
                for (int attempt = 0; attempt < 3 && (infoReply == null || !infoReply.startsWith("OK INFO")); attempt++) {
                    infoReply = link.cmd("info", 2000);
                }
                if (infoReply == null || !infoReply.startsWith("OK INFO")) {
                    report.add("SKIP " + port.getSystemPortPath() + ": no INFO reply (not a Nachtgespenst)");
                    continue;
                }

                Map<String, String> info = parseKeyValue(infoReply);
                String tag = info.getOrDefault("board", "?");
                if (!"C2".equals(tag) && !"S3".equals(tag)) {
                    report.add("SKIP " + port.getSystemPortPath() + ": board=" + tag);
                    continue;
                }
                if (targetBoard != null && !targetBoard.equals(tag)) {
                    continue;
                }

                boardsFound++;
                String expectedName = "S3".equals(tag) ? "NG-S3" : "NG-C2";
                check(tag, "INFO identity " + port.getSystemPortPath(), expectedName.equals(info.get("name")), infoReply);

                if (measureLatency) {
                    latency(tag, link, info);
                } else {
                    testBoard(tag, link, info, shotDirectory);
                }
            } catch (Exception e) {
                failCount++;
                report.add("FAIL exception on " + port.getSystemPortPath() + ": " + e);
            }
        }

        if (boardsFound == 0) {
            failCount++;
            report.add("FAIL no Nachtgespenst board found");
        }

        report.forEach(System.out::println);
        System.out.printf("SUMMARY pass=%d fail=%d -> %s%n",
                passCount, failCount, failCount == 0 ? "PASS" : "FAIL");
        System.exit(failCount == 0 ? 0 : 1);
    }
}
