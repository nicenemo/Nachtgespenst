///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 17+
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;
import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;

/**
 * Serial test harness for Nachtgespenst boards (Core2 + CoreS3 only).
 * Usage: jbang tools/SerialTest.java [--board S3|C2] [--shot dir] [--port /dev/ttyX] [--latency]
 * --latency: instead of the functional tests, measure how long a page takes to reach the display (see latency()).
 * Only ports with USB VID:PID 10c4:ea60 (Core2 CP2104) or 303a:1001 (CoreS3) are opened,
 * and a board is accepted only if INFO answers with board=C2|S3.
 * Uses the Locks page + Scroll Lock only (harmless on the host).
 * Exit code 0 = all PASS, 1 = failures / no board found.
 */
public class SerialTest {
    static int pass = 0, fail = 0;
    static final List<String> report = new ArrayList<>();

    static void check(String board, String name, boolean ok, String detail) {
        if (ok) pass++; else fail++;
        report.add(String.format("%s [%s] %s%s", ok ? "PASS" : "FAIL", board, name, detail.isEmpty() ? "" : "  (" + detail + ")"));
    }

    static class Link implements AutoCloseable {
        final SerialPort p;
        final ByteArrayOutputStream pending = new ByteArrayOutputStream();
        Link(SerialPort p) { this.p = p; }
        boolean open() {
            p.setBaudRate(115200);
            p.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
            return p.openPort();
        }
        void drain() throws InterruptedException {
            Thread.sleep(150);
            byte[] b = new byte[4096];
            while (p.bytesAvailable() > 0) p.readBytes(b, Math.min(b.length, p.bytesAvailable()));
        }
        void send(String s) { byte[] b = (s + "\n").getBytes(StandardCharsets.US_ASCII); p.writeBytes(b, b.length); }
        byte[] readBytes(int n, long timeoutMs) throws InterruptedException {
            byte[] out = new byte[n]; int got = 0; long end = System.currentTimeMillis() + timeoutMs;
            while (got < n && System.currentTimeMillis() < end) {
                int av = p.bytesAvailable();
                if (av > 0) got += p.readBytes(out, Math.min(av, n - got), got);
                else Thread.sleep(2);
            }
            return got == n ? out : null;
        }
        String line(long timeoutMs) throws InterruptedException {
            long end = System.currentTimeMillis() + timeoutMs;
            StringBuilder sb = new StringBuilder();
            byte[] one = new byte[1];
            while (System.currentTimeMillis() < end) {
                if (p.bytesAvailable() > 0 && p.readBytes(one, 1) == 1) {
                    if (one[0] == '\n') { String s = sb.toString().trim(); if (!s.isEmpty()) return s; sb.setLength(0); }
                    else sb.append((char) one[0]);
                } else Thread.sleep(2);
            }
            return null;
        }
        /** Send a command and return the first reply line starting with OK/ERR (banner/noise skipped). */
        String cmd(String c, long timeoutMs) throws InterruptedException {
            send(c);
            long end = System.currentTimeMillis() + timeoutMs;
            while (System.currentTimeMillis() < end) {
                String l = line(Math.max(1, end - System.currentTimeMillis()));
                if (l == null) return null;
                if (l.startsWith("OK") || l.startsWith("ERR")) return l;
            }
            return null;
        }
        public void close() { p.closePort(); }
    }

    static Map<String, String> kv(String s) {
        Map<String, String> m = new HashMap<>();
        if (s == null) return m;
        for (String t : s.split("\\s+")) { int i = t.indexOf('='); if (i > 0) m.put(t.substring(0, i), t.substring(i + 1)); }
        return m;
    }

    static boolean candidate(SerialPort p) {
        int v = p.getVendorID(), id = p.getProductID();
        return (v == 0x10c4 && id == 0xea60) || (v == 0x303a && id == 0x1001);
    }

    /** Find /dev/input/eventN whose name matches (from /proc/bus/input/devices). */
    /** Find the evdev node of a host input device; usb=true picks the USB one, false the Bluetooth (non-usb Phys) one. */
    static String findEvent(String devName, boolean usb) {
        try {
            String cur = null; boolean match = false;
            for (String l : Files.readAllLines(Path.of("/proc/bus/input/devices"))) {
                if (l.startsWith("N: Name=")) { cur = l; match = false; }
                if (l.startsWith("P: Phys=") && cur != null && cur.contains(devName))
                    match = usb == l.substring(8).startsWith("usb-");
                if (l.startsWith("H: Handlers=") && match) {
                    for (String t : l.split("\\s+")) if (t.startsWith("event")) return "/dev/input/" + t;
                }
                if (l.isBlank()) { cur = null; match = false; }
            }
        } catch (IOException e) { }
        return null;
    }

    /** Collect key press events (code,value) from the evdev node for ms milliseconds while action runs. */
    static List<int[]> captureKeys(String evPath, Runnable action, long ms) throws Exception {
        List<int[]> ev = Collections.synchronizedList(new ArrayList<>());
        FileInputStream in = new FileInputStream(evPath);
        Thread t = new Thread(() -> {
            byte[] b = new byte[24];
            try {
                while (true) {
                    int n = 0;
                    while (n < 24) { int r = in.read(b, n, 24 - n); if (r < 0) return; n += r; }
                    ByteBuffer bb = ByteBuffer.wrap(b).order(ByteOrder.LITTLE_ENDIAN);
                    bb.position(16);
                    int type = bb.getShort() & 0xffff, code = bb.getShort() & 0xffff, val = bb.getInt();
                    if (type == 1) ev.add(new int[]{code, val});
                }
            } catch (IOException e) { }
        });
        t.setDaemon(true); t.start();
        Thread.sleep(100);
        action.run();
        Thread.sleep(ms);
        in.close();
        return ev;
    }

    static void testBoard(String tag, Link l, Map<String, String> info, Path shotDir) throws Exception {
        String r = l.cmd("PING", 1500);
        check(tag, "PING", "OK PONG".equals(r), String.valueOf(r));
        String bad = l.cmd("BOGUS 1 2 3", 1500);
        check(tag, "unknown cmd -> ERR", bad != null && bad.startsWith("ERR"), String.valueOf(bad));
        String bad2 = l.cmd("TAP x y", 1500);
        check(tag, "garbage args -> ERR", bad2 != null && bad2.startsWith("ERR"), String.valueOf(bad2));

        int pages = Integer.parseInt(info.get("pages")), locks = Integer.parseInt(info.get("locks"));
        int W = Integer.parseInt(info.get("w")), H = Integer.parseInt(info.get("h"));

        r = l.cmd("LAYER 0", 2000);
        check(tag, "LAYER 0", r != null && r.startsWith("OK LAYER 0"), String.valueOf(r));
        r = l.cmd("STATE?", 1500);
        check(tag, "STATE? page=0", r != null && kv(r).getOrDefault("page", "").startsWith("0/"), String.valueOf(r));
        r = l.cmd("LAYER " + pages, 1500);
        check(tag, "LAYER out of range -> ERR", r != null && r.startsWith("ERR"), String.valueOf(r));
        r = l.cmd("LAYER " + locks, 2000);
        r = l.cmd("STATE?", 1500);
        check(tag, "STATE? title=Locks", r != null && kv(r).getOrDefault("title", "").equals("Locks"), String.valueOf(r));

        if (shotDir != null) {
            saveShot(tag, l, shotDir);
            // every page: <shotDir>/<board>/pageNN.png (slow over the Core2 UART: ~20 s per page)
            for (int p = 0; p < pages; p++) {
                l.cmd("LAYER " + p, 2000);
                Thread.sleep(300);
                saveShot(tag, l, shotDir.resolve(tag), String.format("page%02d.png", p));
            }
            l.cmd("LAYER " + locks, 2000);
        }

        // KEY on Locks page: Scroll Lock (idx 2) -> host LED must toggle
        String ev = "S3".equals(tag) ? findEvent(info.get("name") + " Keyboard", true) : null;
        Map<String, String> s0 = kv(l.cmd("STATE?", 1500));
        int scroll0 = Integer.parseInt(s0.get("scroll")), presses0 = Integer.parseInt(s0.get("presses"));
        final Link lk = l;
        List<int[]> keys = null;
        if (ev != null) {
            try { keys = captureKeys(ev, () -> { try { lk.cmd("KEY " + locks + " 2", 1500); } catch (Exception e) { } }, 600); }
            catch (IOException e) { report.add("INFO [" + tag + "] cannot read " + ev + ": " + e.getMessage()); ev = null; }
        }
        if (ev == null) l.cmd("KEY " + locks + " 2", 1500);
        Thread.sleep(500);
        Map<String, String> s1 = kv(l.cmd("STATE?", 1500));
        check(tag, "KEY locks 2: presses++", Integer.parseInt(s1.get("presses")) == presses0 + 1, s0.get("presses") + "->" + s1.get("presses"));
        // Host LED feedback depends on the desktop (Wayland/X often does not toggle LEDs): informational only.
        report.add("INFO [" + tag + "] host scroll LED " + scroll0 + "->" + s1.get("scroll") + " (not asserted)");
        if (keys != null) {
            boolean down = false, up = false;
            for (int[] k : keys) if (k[0] == 70) { if (k[1] == 1) down = true; if (k[1] == 0) up = true; }
            check(tag, "host evdev sees KEY_SCROLLLOCK down+up", down && up, ev + " events=" + keys.size());
        } else if ("S3".equals(tag)) {
            report.add("SKIP [" + tag + "] evdev check (device node not found/readable)");
        }
        // restore scroll LED state
        l.cmd("KEY " + locks + " 2", 1500); Thread.sleep(400);

        // TAP on the Locks tile 2 (Scroll). Layout from ui.cpp: bar 24 px on top (C2) / 38 px at the bottom (S3), GAP=3, 3 tiles.
        int gap = 3, bar = "C2".equals(tag) ? 24 : 38, w = (W - 4 * gap) / 3;
        int tx = gap + 2 * (w + gap) + w / 2, ty = ("C2".equals(tag) ? bar : 0) + (H - bar) / 2;
        int p0 = Integer.parseInt(kv(l.cmd("STATE?", 1500)).get("presses"));
        r = l.cmd("TAP " + tx + " " + ty, 1500);
        check(tag, "TAP accepted", "OK TAP".equals(r), String.valueOf(r));
        Thread.sleep(600);
        Map<String, String> s2 = kv(l.cmd("STATE?", 1500));
        check(tag, "TAP on Scroll tile counted", Integer.parseInt(s2.get("presses")) == p0 + 1 && "Scroll".equalsIgnoreCase(s2.get("last").replace("_", " ").split(" ")[0]) ,
              "presses " + p0 + "->" + s2.get("presses") + " last=" + s2.get("last"));
        l.cmd("KEY " + locks + " 2", 1500); // restore state again
        if ("S3".equals(tag)) testBle(tag, l, info, locks);
        l.cmd("LAYER 0", 2000);
    }

    /** Down+up of Scroll Lock (KEY_SCROLLLOCK=70) seen in an evdev capture. */
    static boolean scrollDownUp(List<int[]> keys) {
        boolean down = false, up = false;
        if (keys != null) for (int[] k : keys) if (k[0] == 70) { if (k[1] == 1) down = true; if (k[1] == 0) up = true; }
        return down && up;
    }

    /** Press Scroll Lock on the Locks page and capture the events on one host evdev node. */
    static List<int[]> pressAndCapture(Link l, String ev, int locks) throws Exception {
        try { return captureKeys(ev, () -> { try { l.cmd("KEY " + locks + " 2", 1500); } catch (Exception e) { } }, 800); }
        catch (IOException e) { report.add("INFO cannot read " + ev + ": " + e.getMessage()); return null; }
    }

    /** Send one mode command, press Scroll Lock, and check which of the two host devices saw it (expUsb / expBle). */
    static void routeCheck(String tag, Link l, String modeCmd, String okReply, String usbEv, String bleEv, int locks,
                           boolean expUsb, boolean expBle) throws Exception {
        String r = l.cmd(modeCmd, 1500);
        check(tag, modeCmd, okReply.equals(r), String.valueOf(r));
        List<int[]> usb = pressAndCapture(l, usbEv, locks), ble = pressAndCapture(l, bleEv, locks);
        // pressAndCapture presses once per device, so the host scroll LED is toggled twice per mode = balanced
        check(tag, modeCmd + ": USB device " + (expUsb ? "sees" : "stays silent"),
              usb != null && (expUsb ? scrollDownUp(usb) : usb.isEmpty()), usbEv + " events=" + (usb == null ? -1 : usb.size()));
        check(tag, modeCmd + ": BLE device " + (expBle ? "sees" : "stays silent"),
              ble != null && (expBle ? scrollDownUp(ble) : ble.isEmpty()), bleEv + " events=" + (ble == null ? -1 : ble.size()));
    }

    /**
     * S3 keeps USB preferred by default. With both links up: PREFER BLE / ONLY BLE route over BLE only, ONLY USB over
     * USB only, PREFER BOTH over both; each mode is checked against the USB and BLE host input devices.
     */
    static void testBle(String tag, Link l, Map<String, String> info, int locks) throws Exception {
        String st = l.cmd("PREFER?", 1500);
        Map<String, String> m = st != null && st.startsWith("OK") ? kv(st) : Map.of();
        check(tag, "BLE connected (board reports ble=1)", "1".equals(m.get("ble")), String.valueOf(st));
        String bleEv = findEvent(info.get("name") + " Keyboard", false), usbEv = findEvent(info.get("name") + " Keyboard", true);
        if (bleEv == null || usbEv == null || !"1".equals(m.get("ble")) || !"1".equals(m.get("usb"))) {
            report.add("SKIP [" + tag + "] BLE/USB routing checks (need both host devices and both links up)"); return;
        }
        routeCheck(tag, l, "PREFER USB",  "OK PREFER USB",  usbEv, bleEv, locks, true,  false);
        routeCheck(tag, l, "PREFER BLE",  "OK PREFER BLE",  usbEv, bleEv, locks, false, true);
        routeCheck(tag, l, "ONLY USB",    "OK ONLY USB",    usbEv, bleEv, locks, true,  false);
        routeCheck(tag, l, "ONLY BLE",    "OK ONLY BLE",    usbEv, bleEv, locks, false, true);
        routeCheck(tag, l, "PREFER BOTH", "OK PREFER BOTH", usbEv, bleEv, locks, true,  true);
        String r = l.cmd("PREFER USB", 1500);
        check(tag, "PREFER USB restored", "OK PREFER USB".equals(r), String.valueOf(r));
    }

    static void saveShot(String tag, Link l, Path dir) throws Exception {
        saveShot(tag, l, dir, "shot-" + tag + ".png");
    }

    static void saveShot(String tag, Link l, Path dir, String name) throws Exception {
        l.drain();
        l.send("SHOT");
        String hdr = null; long end = System.currentTimeMillis() + 5000;
        while (System.currentTimeMillis() < end) {
            String s = l.line(1000);
            if (s != null && s.startsWith("OK SHOT")) { hdr = s; break; }
        }
        if (hdr == null) { check(tag, "SHOT", false, "no header"); return; }
        String[] t = hdr.split("\\s+");
        int w = Integer.parseInt(t[2]), h = Integer.parseInt(t[3]);
        byte[] px = l.readBytes(w * h * 3, 60000);
        if (px == null) { l.drain(); check(tag, "SHOT", false, "short data"); return; }
        BufferedImage img = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);
        for (int i = 0; i < w * h; i++)
            img.setRGB(i % w, i / w, ((px[i * 3] & 255) << 16) | ((px[i * 3 + 1] & 255) << 8) | (px[i * 3 + 2] & 255));
        Files.createDirectories(dir);
        Path f = dir.resolve(name);
        ImageIO.write(img, "png", f.toFile());
        l.drain();
        check(tag, "SHOT saved", Files.size(f) > 500, f.toString());
    }

    /** Milliseconds for one SHOT round trip (header + w*h*3 bytes + END), or -1 on failure. */
    static double timeShot(Link l) throws Exception {
        l.drain();
        long t0 = System.nanoTime();
        l.send("SHOT");
        String hdr = null; long end = System.currentTimeMillis() + 5000;
        while (System.currentTimeMillis() < end) {
            String s = l.line(1000);
            if (s != null && s.startsWith("OK SHOT")) { hdr = s; break; }
        }
        if (hdr == null) return -1;
        String[] t = hdr.split("\\s+");
        byte[] px = l.readBytes(Integer.parseInt(t[2]) * Integer.parseInt(t[3]) * 3, 60000);
        double ms = (System.nanoTime() - t0) / 1e6;
        l.drain();
        return px == null ? -1 : ms;
    }

    static double median(List<Double> v) {
        List<Double> s = new ArrayList<>(v); Collections.sort(s);
        return s.isEmpty() ? Double.NaN : s.get(s.size() / 2);
    }

    /**
     * Page-switch latency. A page is "on the display" when the framebuffer shows it, which LVGL only guarantees after
     * a render: it answers LAYER before it has drawn anything. So the time is (LAYER + SHOT) minus a plain SHOT
     * (the SHOT also forces the render). Samples a sparse, two dense and the Locks page; one repetition on the slow
     * Core2 UART (a SHOT takes about 20 s there), three on the S3.
     */
    static void latency(String tag, Link l, Map<String, String> info) throws Exception {
        int pages = Integer.parseInt(info.get("pages")), reps = "C2".equals(tag) ? 1 : 3;
        int[] sample = {1, 13, 14, pages - 1};
        List<Double> ping = new ArrayList<>(), readout = new ArrayList<>(), ack = new ArrayList<>(), total = new ArrayList<>();
        l.cmd("LAYER 0", 3000); Thread.sleep(600);
        for (int i = 0; i < 20; i++) { long t0 = System.nanoTime(); l.cmd("PING", 1000); ping.add((System.nanoTime() - t0) / 1e6); }
        for (int i = 0; i < 2; i++) readout.add(timeShot(l));
        for (int r = 0; r < reps; r++) for (int n : sample) {
            l.drain();
            long t0 = System.nanoTime();
            String a = l.cmd("LAYER " + n, 5000);
            double tAck = (System.nanoTime() - t0) / 1e6;
            double shot = timeShot(l);
            if (a == null || shot < 0) { report.add("FAIL [" + tag + "] latency sample for page " + n); fail++; continue; }
            ack.add(tAck); total.add((System.nanoTime() - t0) / 1e6);
        }
        l.cmd("LAYER 0", 3000);
        report.add(String.format("INFO [%s] latency: PING %.0f ms, SHOT read-out %.0f ms, LAYER ack %.0f ms, LAYER+SHOT %.0f ms",
                tag, median(ping), median(readout), median(ack), median(total)));
        report.add(String.format("INFO [%s] page on screen: %.0f ms (median of %d samples, pages %s)",
                tag, median(total) - median(readout), total.size(), Arrays.toString(sample)));
    }

    public static void main(String[] args) throws Exception {
        String only = null, forcePort = null; Path shot = null; boolean latency = false;
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--board": only = args[++i].toUpperCase(); break;
                case "--shot": shot = Path.of(args[++i]); break;
                case "--port": forcePort = args[++i]; break;
                case "--latency": latency = true; break;
                default: System.err.println("unknown arg " + args[i]); System.exit(2);
            }
        }
        int found = 0;
        for (SerialPort p : SerialPort.getCommPorts()) {
            if (!candidate(p)) continue;
            if (forcePort != null && !p.getSystemPortPath().equals(forcePort)) continue;
            try (Link l = new Link(p)) {
                if (!l.open()) { report.add("SKIP " + p.getSystemPortPath() + ": cannot open"); continue; }
                l.drain();
                String r = null;
                for (int i = 0; i < 3 && (r == null || !r.startsWith("OK INFO")); i++) r = l.cmd("INFO", 2000);
                if (r == null || !r.startsWith("OK INFO")) { report.add("SKIP " + p.getSystemPortPath() + ": no INFO reply (not a Nachtgespenst)"); continue; }
                Map<String, String> info = kv(r);
                String tag = info.getOrDefault("board", "?");
                if (!tag.equals("C2") && !tag.equals("S3")) { report.add("SKIP " + p.getSystemPortPath() + ": board=" + tag); continue; }
                if (only != null && !only.equals(tag)) continue;
                found++;
                String exp = tag.equals("S3") ? "Nachtgespenst-S3" : "Nachtgespenst-C2";
                check(tag, "INFO identity " + p.getSystemPortPath(), exp.equals(info.get("name")), r);
                if (latency) latency(tag, l, info); else testBoard(tag, l, info, shot);
            } catch (Exception e) {
                fail++; report.add("FAIL exception on " + p.getSystemPortPath() + ": " + e);
            }
        }
        if (found == 0) { fail++; report.add("FAIL no Nachtgespenst board found"); }
        report.forEach(System.out::println);
        System.out.printf("SUMMARY pass=%d fail=%d -> %s%n", pass, fail, fail == 0 ? "PASS" : "FAIL");
        System.exit(fail == 0 ? 0 : 1);
    }
}
