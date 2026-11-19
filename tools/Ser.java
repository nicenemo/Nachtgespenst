import com.fazecast.jSerialComm.SerialPort;
import java.io.IOException;
import java.lang.foreign.*;
import java.lang.invoke.MethodHandle;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import static java.lang.foreign.ValueLayout.*;

/** The serial protocol of a board (PING, INFO, LAYER, LAMPS ...). */
    final class Ser implements AutoCloseable {
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
