///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 27
//SOURCES Boards.java
//SOURCES LedCam.java
//COMPILE_OPTIONS --add-modules=jdk.incubator.vector
//JAVA_OPTIONS --add-modules=jdk.incubator.vector --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.lang.foreign.*;
import java.lang.invoke.MethodHandle;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.IntFunction;

import static java.lang.foreign.ValueLayout.*;

/**
 * HID LampArray test bench for the Nachtgespenst boards (ADR 0010): drives the LampArray of a board through its HID
 * interface (/dev/hidrawN, over USB or Bluetooth), reads the result back over the serial port (LAMPS?, SHOT) and,
 * for the LED bar, with the webcam. Compare transports with --via usb|ble.
 *
 * <pre>
 * jbang tools/LampTest.java info                        list the HID nodes and the board state
 * jbang tools/LampTest.java enum                        LampArray attributes and per-lamp enumeration speed
 * jbang tools/LampTest.java bar    [--crop x0,y0,x1,y1] colours and intensity ramp of the LED bar (camera optional)
 * jbang tools/LampTest.java screen [--tiers 8x6,16x12,...] [--shot DIR]   lamp grids on the screen: speed, loss, picture
 * jbang tools/LampTest.java all
 * options: --board S3|C2  --via usb|ble  --camera /dev/video0  --out DIR (camera frames, deleted afterwards)
 * </pre>
 * Run it as ONE plain command outside the Claude Code sandbox (it needs /dev/hidrawN, the serial port and the camera).
 * Reading the lamp state over serial is the oracle; the screen picture (SHOT) and the camera close the loop.
 * Exit code 0 = all PASS, 1 = failures, 2 = usage or sandbox problem.
 */
public class LampTest {
    static String boardName = "S3", via = null, shotDir = null, out = "lamptest-out", camera = "/dev/video0", tiers = "8x6,16x12,32x24,64x48,128x96,320x190";
    static int[] crop = null;
    static int frames = 0, enumMax = 200;
    static String ownPort;                     // serial port of the board under test (the other board's LED bar is switched off during camera tests)
    static int paceUs = -1;                    // microseconds between two output reports; -1 = automatic (0 for USB, 15000 for Bluetooth)
    static int pass = 0, fail = 0;
    static String tag;
    static Boards.Board board;

    static void check(String name, boolean ok, String detail) {
        if (ok) pass++; else fail++;
        System.out.printf("%s [%s] %s%s%n", ok ? "PASS" : "FAIL", tag, name, detail.isEmpty() ? "" : "  (" + detail + ")");
    }
    static void info(String s) { System.out.printf("INFO [%s] %s%n", tag, s); }

    public static void main(String[] args) throws Exception {
        List<String> cmds = new ArrayList<>();
        for (int i = 0; i < args.length; i++) switch (args[i]) {
            case "--board" -> boardName = args[++i];
            case "--via" -> via = args[++i];
            case "--shot" -> shotDir = args[++i];
            case "--camera" -> camera = args[++i];
            case "--out" -> out = args[++i];
            case "--tiers" -> tiers = args[++i];
            case "--frames" -> frames = Integer.parseInt(args[++i]);
            case "--enum-max" -> enumMax = Integer.parseInt(args[++i]);
            case "--pace-us" -> paceUs = Integer.parseInt(args[++i]);
            case "--crop" -> crop = Arrays.stream(args[++i].split(",")).mapToInt(Integer::parseInt).toArray();
            default -> { if (args[i].startsWith("--")) usage("unknown option " + args[i]); cmds.add(args[i]); }
        }
        if (cmds.isEmpty()) usage("no command");
        board = Boards.parse(boardName);
        tag = board.tag();
        if (via == null) via = tag.equals("C2") ? "ble" : "usb";
        boolean usb = via.equals("usb");
        if (paceUs < 0) paceUs = usb ? 0 : 15000;
        if (usb && tag.equals("C2")) usage("the Core2 has no USB HID, use --via ble");

        Optional<SerialPort> sp = Boards.find(board);
        if (sp.isEmpty()) { System.err.println("no " + tag + " board found on USB"); System.exit(2); }
        if (!Boards.usable(sp.get())) { System.err.println(Boards.SANDBOX_HINT); System.exit(2); }
        ownPort = Boards.path(sp.get());
        String node = findHid(usb);
        if (node == null && !cmds.contains("info")) { System.err.println("no " + (usb ? "USB" : "Bluetooth") + " HID node for " + tag + " (connected and paired? after a descriptor change remove and pair again)"); System.exit(2); }

        try (Ser s = new Ser(sp.get())) {
            Hid h = null;
            if (node != null) {
                try { h = new Hid(node); } catch (IOException e) { System.err.println(e.getMessage()); System.exit(2); }
            }
            Map<String, String> st = s.kv("INFO");
            info("board " + st + " via " + via + " node " + node);
            for (String c : cmds) {
                switch (c) {
                    case "info" -> info(s.cmd("LAMPS?", 2000));
                    case "enum" -> enumerate(s, h);
                    case "bar" -> bar(s, h);
                    case "screen" -> screen(s, h);
                    case "rate" -> rate(s, h);
                    case "speed" -> speed(s, h);
                    case "interference" -> interference(s, h);
                    case "latency" -> latency(s, h);
                    case "ingest" -> ingest(s, h);
                    case "all" -> { enumerate(s, h); bar(s, h); rate(s, h); screen(s, h); }
                    default -> usage("unknown command " + c);
                }
            }
            if (h != null) { try { h.write(new byte[]{10, 1}); } catch (Exception ignored) { } h.close(); }  // give the lamps back (autonomous)
        }
        System.out.printf("SUMMARY pass=%d fail=%d -> %s%n", pass, fail, fail == 0 ? "PASS" : "FAIL");
        System.exit(fail == 0 ? 0 : 1);
    }

    static void usage(String why) {
        System.err.println(why + "\nusage: LampTest.java info|enum|bar|screen|rate|speed|ingest|interference|latency|all [--board S3|C2] [--via usb|ble] [--tiers 8x6,16x12] [--frames N] [--shot DIR] [--crop x0,y0,x1,y1] [--camera DEV]");
        System.exit(2);
    }

    // ------------------------------------------------------------------ HID nodes
    /** /dev/hidrawN of this board: bus 0003 = USB, 0005 = Bluetooth, matched on the device name. */
    static String findHid(boolean usb) throws IOException {
        String best = null; int bestN = -1;
        try (var ds = Files.newDirectoryStream(Path.of("/sys/class/hidraw"), "hidraw*")) {
            for (Path p : ds) {
                Path ue = p.resolve("device/uevent");
                if (!Files.exists(ue)) continue;
                String id = "", name = "";
                for (String l : Files.readAllLines(ue)) {
                    if (l.startsWith("HID_ID=")) id = l.substring(7);
                    if (l.startsWith("HID_NAME=")) name = l.substring(9);
                }
                boolean isUsb = id.startsWith("0003"), isBle = id.startsWith("0005");
                String token = usb ? "NG-" + tag : (tag.equals("S3") ? "-cs3" : "-c2");   // USB product, or the end of the Bluetooth name
                if (name.contains(token) && (usb ? isUsb : isBle)) {
                    int n = Integer.parseInt(p.getFileName().toString().substring(6));
                    if (n > bestN) { bestN = n; best = "/dev/" + p.getFileName(); }
                }
            }
        }
        return best;
    }

    // ------------------------------------------------------------------ libc through the foreign function API
    static final Linker LINKER = Linker.nativeLinker();
    static final SymbolLookup STD = LINKER.defaultLookup();
    static final StructLayout CAP = Linker.Option.captureStateLayout();
    static final long ERRNO_AT = CAP.byteOffset(MemoryLayout.PathElement.groupElement("errno"));
    static final Arena ARENA = Arena.global();
    static final MemorySegment CAPTURE = ARENA.allocate(CAP);
    static final MemorySegment BUF = ARENA.allocate(1024);
    static final MethodHandle OPEN = LINKER.downcallHandle(STD.find("open").orElseThrow(), FunctionDescriptor.of(JAVA_INT, ADDRESS, JAVA_INT), Linker.Option.captureCallState("errno"));
    static final MethodHandle WRITE = LINKER.downcallHandle(STD.find("write").orElseThrow(), FunctionDescriptor.of(JAVA_LONG, JAVA_INT, ADDRESS, JAVA_LONG), Linker.Option.captureCallState("errno"));
    static final MethodHandle CLOSE = LINKER.downcallHandle(STD.find("close").orElseThrow(), FunctionDescriptor.of(JAVA_INT, JAVA_INT));
    static final MethodHandle IOCTL = LINKER.downcallHandle(STD.find("ioctl").orElseThrow(), FunctionDescriptor.of(JAVA_INT, JAVA_INT, JAVA_LONG, ADDRESS),
            Linker.Option.firstVariadicArg(2), Linker.Option.captureCallState("errno"));

    static int errno() { return CAPTURE.get(JAVA_INT, ERRNO_AT); }

    /** A /dev/hidrawN node: output reports with write(), feature reports with the HIDIOCSFEATURE and HIDIOCGFEATURE ioctls. */
    static final class Hid implements AutoCloseable {
        final int fd;
        Hid(String path) throws IOException {
            try {
                fd = (int) OPEN.invoke(CAPTURE, ARENA.allocateFrom(path), 2);   // O_RDWR
            } catch (Throwable t) { throw new IOException(t); }
            if (fd < 0) throw new IOException("cannot open " + path + " (errno " + errno() + (errno() == 13 ? ": permission denied, see docs/wiki/hardware-testing.md for the hidraw access rule" : "") + ")");
        }
        /** Output report: byte 0 is the report ID. Returns bytes written or -errno. */
        int write(byte[] b) {
            try {
                BUF.copyFrom(MemorySegment.ofArray(b));
                long n = (long) WRITE.invoke(CAPTURE, fd, BUF, (long) b.length);
                return n < 0 ? -errno() : (int) n;
            } catch (Throwable t) { throw new RuntimeException(t); }
        }
        /** Feature report get: returns the report with the ID in byte 0, or null. */
        byte[] getFeature(int id, int payloadLen) {
            int len = payloadLen + 1;
            try {
                BUF.fill((byte) 0); BUF.set(JAVA_BYTE, 0, (byte) id);
                int n = (int) IOCTL.invoke(CAPTURE, fd, 0xC0000000L | ((long) len << 16) | (0x48L << 8) | 0x07, BUF);
                if (n < 0) return null;
                return BUF.asSlice(0, len).toArray(JAVA_BYTE);
            } catch (Throwable t) { throw new RuntimeException(t); }
        }
        boolean setFeature(byte[] b) {
            try {
                BUF.copyFrom(MemorySegment.ofArray(b));
                return (int) IOCTL.invoke(CAPTURE, fd, 0xC0000000L | ((long) b.length << 16) | (0x48L << 8) | 0x06, BUF) >= 0;
            } catch (Throwable t) { throw new RuntimeException(t); }
        }
        public void close() { try { int r = (int) CLOSE.invoke(fd); } catch (Throwable ignored) { } }
    }

    // ------------------------------------------------------------------ serial
    static final class Ser implements AutoCloseable {
        final SerialPort p;
        Ser(SerialPort p) { this.p = p; p.setBaudRate(115200); p.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0); if (!p.openPort()) throw new IllegalStateException("cannot open " + p.getSystemPortPath()); }
        void send(String s) { byte[] b = (s + "\n").getBytes(StandardCharsets.US_ASCII); p.writeBytes(b, b.length); }
        void drain() throws InterruptedException { Thread.sleep(150); byte[] b = new byte[4096]; while (p.bytesAvailable() > 0) p.readBytes(b, Math.min(b.length, p.bytesAvailable())); }
        String line(long ms) throws InterruptedException {
            long end = System.currentTimeMillis() + ms; StringBuilder sb = new StringBuilder(); byte[] one = new byte[1];
            while (System.currentTimeMillis() < end) {
                if (p.bytesAvailable() > 0 && p.readBytes(one, 1) == 1) {
                    if (one[0] == '\n') { String s = sb.toString().trim(); if (!s.isEmpty()) return s; sb.setLength(0); } else sb.append((char) one[0]);
                } else Thread.sleep(1);
            }
            return null;
        }
        String cmd(String c, long ms) throws InterruptedException {
            send(c); long end = System.currentTimeMillis() + ms;
            while (System.currentTimeMillis() < end) {
                String l = line(Math.max(1, end - System.currentTimeMillis()));
                if (l == null) return null;
                if (l.startsWith("OK") || l.startsWith("ERR")) return l;
            }
            return null;
        }
        byte[] bytes(int n, long ms) throws InterruptedException {
            byte[] o = new byte[n]; int got = 0; long end = System.currentTimeMillis() + ms;
            while (got < n && System.currentTimeMillis() < end) { int av = p.bytesAvailable(); if (av > 0) got += p.readBytes(o, Math.min(av, n - got), got); else Thread.sleep(2); }
            return got == n ? o : null;
        }
        Map<String, String> kv(String c) throws InterruptedException {
            Map<String, String> m = new LinkedHashMap<>(); String s = cmd(c, 3000);
            if (s != null) for (String t : s.split("\\s+")) { int i = t.indexOf('='); if (i > 0) m.put(t.substring(0, i), t.substring(i + 1)); }
            return m;
        }
        /** Screenshot as RGB888 w*h*3, or null. */
        record Shot(int w, int h, byte[] px) {
            int[] at(int x, int y) { int i = (y * w + x) * 3; return new int[]{px[i] & 255, px[i + 1] & 255, px[i + 2] & 255}; }
        }
        Shot shot() throws Exception {
            drain(); send("SHOT"); String hdr = null; long end = System.currentTimeMillis() + 5000;
            while (System.currentTimeMillis() < end) { String s = line(1000); if (s != null && s.startsWith("OK SHOT")) { hdr = s; break; } }
            if (hdr == null) return null;
            String[] t = hdr.split("\\s+"); int w = Integer.parseInt(t[2]), h = Integer.parseInt(t[3]);
            byte[] px = bytes(w * h * 3, 90000); drain();
            return px == null ? null : new Shot(w, h, px);
        }
        public void close() { p.closePort(); }
    }

    // ------------------------------------------------------------------ report builders (byte 0 = report ID)
    static byte[] control(boolean autonomous) { return new byte[]{10, (byte) (autonomous ? 1 : 0)}; }

    /** Range update: lamps first..last (inclusive) get one colour. */
    static byte[] range(int first, int last, int r, int g, int b, int i, boolean complete) {
        return new byte[]{9, (byte) (complete ? 1 : 0), (byte) first, (byte) (first >> 8), (byte) last, (byte) (last >> 8), (byte) r, (byte) g, (byte) b, (byte) i};
    }

    /** Multi update of up to 8 lamps: ids[k] gets rgbi[k]. */
    static byte[] multi(int[] ids, int[][] rgbi, int count, boolean complete) {
        byte[] p = new byte[51];
        p[0] = 8; p[1] = (byte) count; p[2] = (byte) (complete ? 1 : 0);
        for (int k = 0; k < count; k++) {
            p[3 + 2 * k] = (byte) ids[k]; p[4 + 2 * k] = (byte) (ids[k] >> 8);
            for (int c = 0; c < 4; c++) p[19 + 4 * k + c] = (byte) rgbi[k][c];
        }
        return p;
    }

    /** Waits until nothing more arrives from an earlier test: BlueZ queues output reports and drains them at about 35 per second. */
    static void drain(Ser s) throws Exception {
        long end = System.currentTimeMillis() + 240000;
        while (System.currentTimeMillis() < end) {
            s.cmd("LAMPS RESET", 3000); Thread.sleep(1500);
            if (num(s.kv("LAMPS?"), "reports") == 0) return;
        }
        info("the board kept receiving old reports for 4 minutes");
    }

    /** Waits until the report counter has not moved for 1.5 s (everything sent so far has arrived). */
    static void settle(Ser s, long maxMs) throws Exception {
        long end = System.currentTimeMillis() + maxMs, last = -1; int calm = 0;
        while (System.currentTimeMillis() < end && calm < 3) {
            Thread.sleep(500);
            long r = num(s.kv("LAMPS?"), "reports");
            if (r == last) calm++; else calm = 0;
            last = r;
        }
    }

    /** Sleeps between two output reports so a Bluetooth link is not flooded (BlueZ drops what it cannot queue). */
    static void pace() { if (paceUs > 0) java.util.concurrent.locks.LockSupport.parkNanos(paceUs * 1000L); }
    static void pace(int us) { if (us > 0) java.util.concurrent.locks.LockSupport.parkNanos(us * 1000L); }

    static int u16(byte[] b, int at) { return (b[at] & 255) | ((b[at + 1] & 255) << 8); }
    static long u32(byte[] b, int at) { return (u16(b, at) | ((long) u16(b, at + 2) << 16)); }

    static long num(Map<String, String> m, String k) { try { return Long.parseLong(m.get(k)); } catch (Exception e) { return -1; } }

    // ------------------------------------------------------------------ enum
    static void enumerate(Ser s, Hid h) throws Exception {
        s.cmd("LAMPS RESET", 2000);
        Map<String, String> st = s.kv("LAMPS?");
        long count = num(st, "lamps");
        byte[] a = h.getFeature(5, 22);
        check("attributes report readable (feature 5)", a != null, a == null ? "GET_FEATURE failed" : "");
        if (a == null) return;
        int lamps = u16(a, 1);
        info(String.format("LampArray: %d lamps, box %d x %d x %d um, kind %d, min update interval %d us", lamps, u32(a, 3), u32(a, 7), u32(a, 11), u32(a, 15), u32(a, 19)));
        check("attributes lamp count equals the board's", lamps == count, lamps + " vs " + count);
        int n = (int) Math.min(count, enumMax);
        long t0 = System.nanoTime(); int ok = 0;
        for (int id = 0; id < n; id++) {
            int lid = n == count ? id : (int) ((long) id * (count - 1) / (n - 1));
            if (!h.setFeature(new byte[]{6, (byte) lid, (byte) (lid >> 8)})) continue;
            byte[] r = h.getFeature(7, 28);
            if (r != null && u16(r, 1) == lid) ok++;
        }
        double ms = (System.nanoTime() - t0) / 1e6;
        check("lamp attribute request/response (feature 6 and 7), " + n + " lamps", ok == n, ok + " ok");
        info(String.format("enumeration: %.1f ms per lamp, so %d lamps take about %.1f s", ms / n, count, ms / n * count / 1000));
        byte[] last = null;
        h.setFeature(new byte[]{6, 10, 0}); last = h.getFeature(7, 28);
        if (last != null) info(String.format("lamp 10: position %d,%d,%d um, purposes 0x%x, latency %d us, levels %d/%d/%d/%d", u32(last, 3), u32(last, 7), u32(last, 11), u32(last, 15), u32(last, 19),
                last[23] & 255, last[24] & 255, last[25] & 255, last[26] & 255));
    }

    // ------------------------------------------------------------------ bar
    static void bar(Ser s, Hid h) throws Exception {
        s.cmd("LAMPS RESET", 2000);
        s.cmd("LAMPS BRIGHT 40", 1000);
        s.cmd("LAYER " + barPage(s), 3000);                             // the bar lamp page is the last page
        check("control report: host takes over", h.write(control(false)) > 0, "");
        Thread.sleep(300);
        check("LAMPS? shows host mode", "host".equals(s.kv("LAMPS?").get("mode")), "");

        // A. every lamp its own colour and intensity, read back lamp by lamp
        int[][] pal = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {0, 255, 255}, {255, 0, 255}, {255, 255, 255}, {255, 128, 0}, {128, 0, 255}, {64, 64, 64}};
        for (int k = 0; k < 10; k += 8) {
            int cnt = Math.min(8, 10 - k); int[] ids = new int[8]; int[][] c = new int[8][4];
            for (int j = 0; j < cnt; j++) { ids[j] = k + j; c[j] = new int[]{pal[k + j][0], pal[k + j][1], pal[k + j][2], 255 - 20 * (k + j)}; }
            h.write(multi(ids, c, cnt, k + 8 >= 10));
        }
        Thread.sleep(300);
        int bad = 0;
        for (int i = 0; i < 10; i++) {
            String r = s.cmd("LAMPS GET " + i, 1500);
            String want = String.format("OK LAMP %d %d %d %d %d", i, pal[i][0], pal[i][1], pal[i][2], 255 - 20 * i);
            if (!want.equals(r)) { bad++; info("lamp " + i + ": " + r + " expected " + want); }
        }
        check("multi update: 10 lamps with colour and intensity read back", bad == 0, bad + " wrong");
        screenshot(s, "bar-palette");

        // B. intensity ramp (white, all ten lamps by one range update) with the webcam
        int[] levels = {0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 255};
        double[] lum = new double[levels.length];
        boolean cam = crop != null;
        if (cam) LedCam.otherBars(ownPort, false);
        List<BufferedImage> crops = new ArrayList<>();
        LedCam.out = out; LedCam.camera = camera;
        LedCam.Frame base = null;
        if (cam) { Files.createDirectories(Path.of(out)); h.write(range(0, 9, 0, 0, 0, 0, true)); Thread.sleep(700); base = LedCam.frame("base.jpg"); }
        int wrong = 0;
        for (int k = 0; k < levels.length; k++) {
            h.write(range(0, 9, 255, 255, 255, levels[k], true));
            Thread.sleep(cam ? 700 : 120);
            String r = s.cmd("LAMPS GET 9", 1500);
            if (!("OK LAMP 9 255 255 255 " + levels[k]).equals(r)) wrong++;
            if (cam) {
                LedCam.Frame f = LedCam.frame(String.format("i%03d.jpg", levels[k]));
                double[] d = LedCam.roiMeanDiff(f.planes(), base.planes(), crop[0], crop[1], crop[2], crop[3]);
                lum[k] = (d[0] + d[1] + d[2]) / 3;
                crops.add(f.img().getSubimage(crop[0], crop[1], crop[2] - crop[0], crop[3] - crop[1]));
            }
        }
        check("range update: intensity 0..255 read back", wrong == 0, wrong + " wrong");
        if (cam) {
            StringBuilder t = new StringBuilder("camera brightness per intensity:");
            for (int k = 0; k < levels.length; k++) t.append(String.format(" %d=%.1f", levels[k], lum[k]));
            info(t.toString());
            int steps = 0; boolean mono = true;
            for (int k = 1; k < levels.length; k++) { if (lum[k] > lum[k - 1] + 0.3) steps++; if (lum[k] < lum[k - 1] - 1.5) mono = false; }
            check("camera: brightness never falls as the intensity rises", mono, "");
            check("camera: full intensity is clearly brighter than off", lum[levels.length - 1] - lum[0] > 3, String.format("%.1f vs %.1f", lum[levels.length - 1], lum[0]));
            info(steps + " of " + (levels.length - 1) + " intensity steps are visible to the camera (global brightness " + 40 + "/255)");
            sheet(crops, levels, Path.of(out, "intensity.png"));
            info("contact sheet " + Path.of(out, "intensity.png"));
        }

        // C. the three colour channels
        String[] names = {"red", "green", "blue"};
        for (int c = 0; c < 3; c++) {
            h.write(range(0, 9, c == 0 ? 255 : 0, c == 1 ? 255 : 0, c == 2 ? 255 : 0, 255, true));
            Thread.sleep(150);
            String r = s.cmd("LAMPS GET 5", 1500);
            check("colour " + names[c], ("OK LAMP 5 " + (c == 0 ? 255 : 0) + " " + (c == 1 ? 255 : 0) + " " + (c == 2 ? 255 : 0) + " 255").equals(r), String.valueOf(r));
        }

        // D. update speed of the bar: range updates back to back
        drain(s); h.write(control(false));
        int n = paceUs > 0 ? 150 : 2000; long t0 = System.nanoTime(); int errs = 0;
        for (int k = 0; k < n; k++) { if (h.write(range(0, 9, k & 255, 255 - (k & 255), 128, 255, true)) < 0) errs++; pace(); }
        double ms = (System.nanoTime() - t0) / 1e6;
        settle(s, 120000);
        Map<String, String> st = s.kv("LAMPS?");
        long got = num(st, "reports"), span = num(st, "span_us");
        info(String.format("bar updates: %d range updates in %.0f ms on the host (%.0f/s), the board counted %d reports (%d expected with the control report), within %.0f ms (%.0f/s), %d write errors",
                n, ms, n / (ms / 1000), got, n + 1, span / 1000.0, got / (span / 1e6), errs));
        check("bar update burst: no write errors", errs == 0, errs + " errors");
        check("bar update burst: every report arrived", got == n + 1, got + " of " + n + " range updates plus the control report");
        h.write(control(true));
        s.cmd("LAMPS RESET", 2000);
        if (cam) { deleteFrames(); LedCam.otherBars(ownPort, true); }
    }

    /** How fast does a transport deliver output reports? Multi updates (51 bytes) and range updates (10 bytes) at several send intervals. */
    static void rate(Ser s, Hid h) throws Exception {
        s.cmd("LAYER " + barPage(s), 3000);
        int[] paces = paceUs == 0 ? new int[]{0} : new int[]{0, 15000, 30000, 40000};
        int n = paceUs == 0 ? 500 : 80;
        for (String kind : new String[]{"multi", "range"}) {
            StringBuilder tbl = new StringBuilder(); boolean lossless = true; double bestRate = 0;
            for (int us : paces) {
                drain(s); h.write(control(false)); Thread.sleep(200);
                int errs = 0; long t0 = System.nanoTime();
                int[] ids = new int[8]; int[][] c = new int[8][4];
                for (int k = 0; k < n; k++) {
                    byte[] p;
                    if (kind.equals("multi")) { for (int j = 0; j < 8; j++) { ids[j] = j; c[j] = new int[]{k & 255, j * 30, 255 - (k & 255), 255}; } p = multi(ids, c, 8, true); }
                    else p = range(0, 9, k & 255, 128, 64, 255, true);
                    if (h.write(p) < 0) errs++;
                    pace(us);
                }
                double hostMs = (System.nanoTime() - t0) / 1e6;
                settle(s, 120000);
                Map<String, String> st = s.kv("LAMPS?");
                long got = num(st, "reports") - 1, span = num(st, "span_us");
                double rateDev = got > 1 ? (got - 1) / (span / 1e6) : 0;
                tbl.append(String.format("%n   send interval %5d us (host %6.0f/s): %3d of %d delivered, the board saw them over %6.0f ms = %5.1f reports/s%s",
                        us, n / (hostMs / 1000), got, n, span / 1000.0, rateDev, errs > 0 ? ", write errors " + errs : ""));
                if (got != n && (us >= 15000 || paceUs == 0)) lossless = false;      // flooding a Bluetooth link without any pause loses reports; that is a finding, not a failure
                bestRate = Math.max(bestRate, rateDev);
            }
            info(kind + " updates (" + (kind.equals("multi") ? "51" : "10") + " bytes each), " + via + ":" + tbl);
            check(kind + " updates: every report is delivered at a sensible send rate" + (paceUs == 0 ? "" : " (15 ms or slower)"), lossless, String.format("best delivery rate %.0f reports/s = %.0f lamps/s", bestRate, bestRate * (kind.equals("multi") ? 8 : 10)));
        }
        h.write(control(true)); s.cmd("LAMPS RESET", 2000);
    }

    // ------------------------------------------------------------------ speed and interference
    record Burst(double hostPerSec, long reports, double boardPerSec, double lampsPerSec, double lampsSetPerSec, long drawn, long drawUs, boolean lossless) {}

    /**
     * Sends n output reports back to back (paced when a pace is set) and reads what the board counted: reports per second on the
     * host and on the board, lamps per second by the sender's count (lamps per report) and by the board's own counter, and how
     * often the board drew. With noise the serial port carries a PING every 10 ms during the flood.
     */
    static Burst burst(Ser s, Hid h, int n, IntFunction<byte[]> gen, int lampsPerReport, boolean noise) throws Exception {
        drain(s); h.write(control(false)); Thread.sleep(150);
        AtomicBoolean stop = new AtomicBoolean(false);
        Thread t = null;
        if (noise) { t = new Thread(() -> { while (!stop.get()) { s.send("PING"); try { Thread.sleep(10); } catch (InterruptedException e) { return; } } }); t.start(); }
        long t0 = System.nanoTime();
        for (int k = 0; k < n; k++) { h.write(gen.apply(k)); pace(); }
        double hostMs = (System.nanoTime() - t0) / 1e6;
        if (t != null) { stop.set(true); t.join(); }
        s.drain();
        settle(s, 180000);
        Map<String, String> st = s.kv("LAMPS?");
        long got = num(st, "reports") - 1, span = num(st, "span_us");
        double sec = Math.max(span / 1e6, 1e-6);
        return new Burst(n / (hostMs / 1000), got, (got - 1) / sec, lampsPerReport * (got - 1) / sec, num(st, "lamps_set") / sec, num(st, "drawn"), num(st, "draw_us"), got == n);
    }

    static String fmt(Burst b, int n) {
        return String.format("%5d reports: host %6.0f/s, board %6.0f/s, %9.0f lamps/s (board counter %10.0f/s), drew %d times (%d us), %s",
                n, b.hostPerSec, b.boardPerSec, b.lampsPerSec, b.lampsSetPerSec, b.drawn, b.drawUs, b.lossless ? "all arrived" : (b.reports + " of " + n + " arrived"));
    }

    static byte[] multiFrame(int k) {
        int[] ids = new int[8]; int[][] c = new int[8][4];
        for (int j = 0; j < 8; j++) { ids[j] = 10 + j; c[j] = new int[]{k & 255, j * 30, 255 - (k & 255), 255}; }
        return multi(ids, c, 8, true);
    }

    /** Lamps switched per second: multi updates (8 lamps) and range updates of growing size, on a lamp page (the board draws). */
    static void speed(Ser s, Hid h) throws Exception { speedOn(s, h, barPage(s) - 1, "screen lamp page"); }

    /**
     * The same on an ordinary layer page: nothing is drawn and the bar does not follow the lamps, so this is the pure intake
     * of the board (the HID task stores the lamps, the main loop and the other core do nothing for them).
     */
    static void ingest(Ser s, Hid h) throws Exception { speedOn(s, h, 0, "layer page, no drawing, bar untouched"); }

    static void speedOn(Ser s, Hid h, int screenPage, String what) throws Exception {
        info("page: " + what);
        s.cmd("LAYER " + screenPage, 3000);
        int[][] grids = {{16, 12}, {16, 12}, {32, 24}, {64, 48}, {128, 96}, {320, 190}};
        int[] sizes = {1, 192, 768, 3072, 12288, 60800};
        s.cmd("LAMPS GRID 16 12", 3000);
        int nm = paceUs > 0 ? 80 : 4000;
        Burst m = burst(s, h, nm, Lamp_multi, 8, false);
        info("multi update, 8 lamps per report, " + via + ": " + fmt(m, nm));
        check("multi updates: every report arrived", m.lossless, m.reports + " of " + nm);
        for (int i = 0; i < sizes.length; i++) {
            s.cmd("LAMPS GRID " + grids[i][0] + " " + grids[i][1], 3000);
            final int r = sizes[i];
            int n = paceUs > 0 ? 60 : (r >= 12288 ? 200 : 1500);
            Burst b = burst(s, h, n, k -> range(10, 10 + r - 1, k & 255, 255 - (k & 255), 128, 255, true), r, false);
            info("range update of " + r + " lamp" + (r > 1 ? "s" : "") + ", " + via + ": " + fmt(b, n));
            check("range of " + r + " lamps: every report arrived", b.lossless, b.reports + " of " + n);
        }
        s.cmd("LAMPS GRID 16 12", 3000); h.write(control(true)); s.cmd("LAMPS RESET", 2000);
    }

    static final IntFunction<byte[]> Lamp_multi = LampTest::multiFrame;

    /** Does anything else on the link change the delivery rate: serial traffic, the board drawing, the page that is shown? */
    static void interference(Ser s, Hid h) throws Exception {
        int screenPage = barPage(s) - 1, barPg = barPage(s);
        s.cmd("LAMPS GRID 16 12", 3000);
        int n = paceUs > 0 ? 80 : 3000;
        if (paceUs > 0) paceUs = 15000;                     // Bluetooth: queue a little faster than it drains; the board's span is the drain rate
        String[] names = {"bar page (the board draws ten swatches), no serial traffic", "bar page, PING on the serial port every 10 ms",
                          "screen lamp page (the board redraws changed rows), no serial traffic", "layer page (no lamp drawing at all), no serial traffic",
                          "layer page, PING on the serial port every 10 ms"};
        int[] pages = {barPg, barPg, screenPage, 0, 0};
        boolean[] noise = {false, true, false, false, true};
        double[] rate = new double[names.length];
        for (int i = 0; i < names.length; i++) {
            s.cmd("LAYER " + pages[i], 3000); Thread.sleep(800);
            Burst b = burst(s, h, n, Lamp_multi, 8, noise[i]);
            rate[i] = b.boardPerSec;
            info(names[i] + ": " + fmt(b, n));
        }
        double base = rate[0];
        for (int i = 1; i < names.length; i++) info(String.format("  %s: %+.1f%% against the baseline", names[i], 100 * (rate[i] - base) / base));
        check("serial traffic changes the rate by less than 5%", Math.abs(rate[1] - base) / base < 0.05 && Math.abs(rate[4] - rate[3]) / rate[3] < 0.05, String.format("%.1f vs %.1f and %.1f vs %.1f reports/s", rate[1], base, rate[4], rate[3]));
        check("drawing changes the rate by less than 5%", Math.abs(rate[2] - base) / base < 0.05 && Math.abs(rate[3] - base) / base < 0.05, String.format("screen page %.1f, layer page %.1f, bar page %.1f reports/s", rate[2], rate[3], base));
        s.cmd("LAYER 0", 3000); h.write(control(true)); s.cmd("LAMPS RESET", 2000);
    }

    /**
     * How long does a lamp frame take to become visible? The board stamps the report that completes a frame and measures until the bar
     * has been shown (bar_lat_us) and until the screen has been drawn (scr_lat_us). Frames are sent 150 ms apart (a slow, steady
     * stream: no queueing on the host), once with the bar worker on the other core and once with the bar shown by the main loop.
     */
    static void latency(Ser s, Hid h) throws Exception {
        int screenPage = barPage(s) - 1, barPg = barPage(s);
        s.cmd("LAMPS GRID 16 12", 3000);
        int n = frames > 0 ? frames : 40;
        int[] pages = {barPg, screenPage};
        String[] names = {"bar page", "screen lamp page (16 x 12)"};
        for (int w = 1; w >= 0; w--) {
            s.cmd("LAMPS WORKER " + (w == 1 ? "ON" : "OFF"), 2000);
            for (int i = 0; i < pages.length; i++) {
                s.cmd("LAYER " + pages[i], 3000); Thread.sleep(800);
                drain(s); h.write(control(false)); Thread.sleep(300);
                s.cmd("LAMPS RESET", 2000);
                for (int k = 0; k < n; k++) {
                    h.write(range(0, 9, k % 2 == 0 ? 255 : 0, k % 2 == 0 ? 0 : 255, 64, 255, false));
                    pace(15000);
                    h.write(range(10, 201, k % 2 == 0 ? 0 : 255, 128, k % 2 == 0 ? 255 : 0, 255, true));
                    Thread.sleep(150);
                }
                Thread.sleep(1500);                                      // BLE: the last reports may still be queued on the host
                settle(s, 20000);
                Map<String, String> st = s.kv("LAMPS?");
                for (int tries = 0; tries < 4 && st.get("bar_shows") == null; tries++) { Thread.sleep(500); st = s.kv("LAMPS?"); }
                info(String.format("%s, worker %s: bar shown %s times, latency avg/max %s us; screen drawn %s times, latency avg/max %s us",
                        names[i], w == 1 ? "on " : "off", st.get("bar_shows"), st.get("bar_lat_us"), st.get("drawn"), st.get("scr_lat_us")));
                check(names[i] + ", worker " + (w == 1 ? "on" : "off") + ": the bar was shown for the frames", num(st, "bar_shows") > 0, st.get("bar_shows"));
            }
        }
        s.cmd("LAMPS WORKER ON", 2000); s.cmd("LAYER 0", 3000); h.write(control(true)); s.cmd("LAMPS RESET", 2000);
    }

    static int barPage(Ser s) throws Exception { return Integer.parseInt(s.kv("INFO").get("pages")) - 1; }

    static void deleteFrames() throws IOException {
        try (var ds = Files.list(Path.of(out))) { for (Path p : (Iterable<Path>) ds.filter(p -> p.toString().endsWith(".jpg"))::iterator) Files.delete(p); }
    }

    static void sheet(List<BufferedImage> crops, int[] labels, Path png) throws IOException {
        int tw = 160, th = 100, cols = 6, rows = (crops.size() + cols - 1) / cols;
        BufferedImage m = new BufferedImage(cols * tw, rows * (th + 16), BufferedImage.TYPE_INT_RGB);
        Graphics2D g = m.createGraphics(); g.setColor(Color.DARK_GRAY); g.fillRect(0, 0, m.getWidth(), m.getHeight());
        for (int i = 0; i < crops.size(); i++) {
            int x = (i % cols) * tw, y = (i / cols) * (th + 16);
            g.drawImage(crops.get(i), x, y + 16, tw, th, null);
            g.setColor(Color.WHITE); g.drawString("intensity " + labels[i], x + 4, y + 12);
        }
        g.dispose(); ImageIO.write(m, "png", png.toFile());
    }

    static void screenshot(Ser s, String name) throws Exception {
        if (shotDir == null) return;
        Ser.Shot sh = s.shot();
        if (sh == null) { info("screenshot " + name + " failed"); return; }
        savePng(sh, Path.of(shotDir, tag + "-" + via + "-" + name + ".png"));
    }

    static void savePng(Ser.Shot sh, Path f) throws IOException {
        BufferedImage img = new BufferedImage(sh.w, sh.h, BufferedImage.TYPE_INT_RGB);
        for (int y = 0; y < sh.h; y++) for (int x = 0; x < sh.w; x++) { int[] c = sh.at(x, y); img.setRGB(x, y, (c[0] << 16) | (c[1] << 8) | c[2]); }
        Files.createDirectories(f.getParent()); ImageIO.write(img, "png", f.toFile());
        info("screenshot " + f);
    }

    // ------------------------------------------------------------------ screen
    /** Layout of the grid on the panel, mirroring Lamps.cpp (body rectangle, 12 px status strip, integer cell size). */
    record Grid(int cols, int rows, int cw, int ch, int ox, int oy, int bodyY) {
        static Grid of(int cols, int rows) {
            int bodyY = tag.equals("C2") ? 24 : 0, bodyH = tag.equals("C2") ? 216 : 202, gh = bodyH - 12;
            int cw = Math.max(1, 320 / cols), ch = Math.max(1, gh / rows);
            return new Grid(cols, rows, cw, ch, (320 - cw * cols) / 2, (gh - ch * rows) / 2, bodyY);
        }
        int[] colour(int c, int r, int shift) {
            int cc = (c + shift) % cols;
            return new int[]{cols > 1 ? cc * 255 / (cols - 1) : 255, rows > 1 ? r * 255 / (rows - 1) : 255, 128, 255};
        }
    }

    static void screen(Ser s, Hid h) throws Exception {
        s.cmd("LAYER " + (barPage(s) - 1), 3000);                        // the screen lamp page
        check("on the screen lamp page", s.kv("STATE?").getOrDefault("title", "").startsWith("Lamps-screen"), "");
        for (String t : tiers.split(",")) {
            String[] cr = t.split("x"); int cols = Integer.parseInt(cr[0]), rows = Integer.parseInt(cr[1]);
            String r = s.cmd("LAMPS GRID " + cols + " " + rows, 3000);
            if (r == null || !r.startsWith("OK")) { check("grid " + t, false, String.valueOf(r)); continue; }
            drain(s);
            Grid g = Grid.of(cols, rows);
            int n = cols * rows, reportsPerFrame = (n + 7) / 8;
            int nf = frames > 0 ? frames : paceUs > 0 ? (reportsPerFrame <= 30 ? 15 : reportsPerFrame <= 100 ? 4 : reportsPerFrame <= 400 ? 2 : 1)
                    : (reportsPerFrame <= 30 ? 60 : reportsPerFrame <= 100 ? 20 : reportsPerFrame <= 400 ? 6 : 2);
            check("control report: host takes over (" + t + ")", h.write(control(false)) > 0, "");
            long sent = 0, errs = 0; long t0 = System.nanoTime();
            byte[] pkt;
            for (int f = 0; f < nf; f++) {
                int shift = f == nf - 1 ? 0 : f % cols;                  // the last frame is the reference picture
                for (int base = 0; base < n; base += 8) {
                    int cnt = Math.min(8, n - base); int[] ids = new int[8]; int[][] c = new int[8][];
                    for (int k = 0; k < cnt; k++) { ids[k] = 10 + base + k; c[k] = g.colour((base + k) % cols, (base + k) / cols, shift); }
                    pkt = multi(ids, c, cnt, base + cnt >= n);
                    if (h.write(pkt) < 0) errs++; sent++; pace();
                }
            }
            double ms = (System.nanoTime() - t0) / 1e6;
            Map<String, String> st;
            settle(s, 180000); st = s.kv("LAMPS?");
            long got = num(st, "reports"), span = num(st, "span_us"), drawn = num(st, "drawn"), fr = num(st, "frames");
            info(String.format("%s: %d lamps, %d frames of %d reports: host %.0f ms = %.1f frames/s, %.0f lamps/s, %.0f reports/s; board counted %d for %d sent plus one control report (%.1f%%) in %.0f ms (%.1f frames/s), drew %d times (%d us each)",
                    t, n, nf, reportsPerFrame, ms, nf / (ms / 1000), (double) n * nf / (ms / 1000), sent / (ms / 1000), got, sent, 100.0 * (got - 1) / sent,
                    span / 1000.0, fr / (span / 1e6), drawn, num(st, "draw_us")));
            check("tier " + t + ": no write errors", errs == 0, errs + " errors");
            check("tier " + t + ": every report arrived", got == sent + 1, got + " counted, " + sent + " sent plus the control report");
            Thread.sleep(300);
            Ser.Shot sh = s.shot();
            if (sh == null) { check("tier " + t + ": screenshot", false, "no data"); continue; }
            int stride = Math.max(1, n / 1500), bad = 0, seen = 0;
            for (int k = 0; k < n; k += stride) {
                int c = k % cols, rw = k / cols; int[] e = g.colour(c, rw, 0);
                int[] p = sh.at(g.ox + c * g.cw + g.cw / 2, g.bodyY + g.oy + rw * g.ch + g.ch / 2);
                seen++;
                if (Math.abs(p[0] - e[0]) > 10 || Math.abs(p[1] - e[1]) > 10 || Math.abs(p[2] - e[2]) > 10) bad++;
            }
            check("tier " + t + ": screenshot matches the lamps", bad * 200 <= seen, bad + " of " + seen + " sampled lamps differ");
            if (shotDir != null) savePng(sh, Path.of(shotDir, tag + "-" + via + "-screen-" + t + ".png"));
        }
        h.write(control(true));
        s.cmd("LAMPS GRID 16 12", 2000);
        s.cmd("LAMPS RESET", 2000);
    }
}
