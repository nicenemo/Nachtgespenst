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
import java.util.zip.CRC32;
import java.util.zip.Deflater;

/** Report builders and helpers of the LampArray and the Disk protocol (report byte 0 is the report ID). */
final class Proto {
    static final int F_CREDIT = 8, F_REQ = 16, F_HASH = 32, F_RESEND = 64, GAMEPAD_ID = 11;
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



    static byte[] deflateRaw(byte[] in) {
        Deflater d = new Deflater(9, true); d.setInput(in); d.finish();
        java.io.ByteArrayOutputStream o = new java.io.ByteArrayOutputStream();
        byte[] b = new byte[65536];
        while (!d.finished()) o.write(b, 0, d.deflate(b));
        d.end(); return o.toByteArray();
    }

    /** Writes one block as multi updates of eight lamps; the last report carries "update complete" (the clock edge). inject drops reports at random. */
    static int sendBlock(Hid h, byte[] blk, int gapUs, int injectPct) {
        int lamps = blk.length / 4, reports = 0;
        for (int base = 0; base < lamps; base += 8) {
            int cnt = Math.min(8, lamps - base); int[] ids = new int[8]; int[][] c = new int[8][4];
            for (int k = 0; k < cnt; k++) { ids[k] = Disk_FIRST + base + k; for (int j = 0; j < 4; j++) c[k][j] = blk[(base + k) * 4 + j] & 255; }
            boolean last = base + cnt >= lamps;
            if (injectPct > 0 && RNG.nextInt(100) < injectPct) { reports++; park(gapUs); continue; }   // a lost report: CRC or timeout catches it
            h.write(multi(ids, c, cnt, last)); reports++;
            park(gapUs);
        }
        return reports;
    }

    /** The PC's SHA-256 and speed go to the board (over the lamps) so that it can show them next to its own. which: 0 = the original, 1 = what came back. */
    static void sendHash(Hid h, int sid, byte[] sha, long bytesPerSec, int which) {
        byte[] pl = new byte[37];
        System.arraycopy(sha, 0, pl, 0, 32);
        for (int k = 0; k < 4; k++) pl[32 + k] = (byte) (bytesPerSec >> (8 * k));
        pl[36] = (byte) which;
        sendBlock(h, diskBlock(0, F_HASH, sid, pl, 0, pl.length), 0, 0);
    }

    static void sendCredit(Hid h, int sid, long wanted, int payload, int window, int gap10us, boolean resend) {
        byte[] pl = new byte[6];
        for (int k = 0; k < 4; k++) pl[k] = (byte) (wanted >> (8 * k));
        pl[4] = (byte) gap10us; pl[5] = (byte) (gap10us >> 8);
        sendBlock(h, diskBlock(0, F_CREDIT | (resend ? F_RESEND : 0), sid, payload, window, pl, 0, pl.length), 0, 0);
    }

    static final int Disk_FIRST = 10;                       // the first lamp of the data area
    static final java.util.Random RNG = new java.util.Random(7);
    static void park(int us) { if (us > 0) java.util.concurrent.locks.LockSupport.parkNanos(us * 1000L); }


    static byte[] diskBlock(int seq, int flags, int sid, byte[] data, int off, int len) { return diskBlock(seq, flags, sid, 0, 0, data, off, len); }

    /** p6 and p7 are the two parameter bytes of the header: for a request or a credit the payload bytes per report and the window. */
    static byte[] diskBlock(int seq, int flags, int sid, int p6, int p7, byte[] data, int off, int len) {
        int lamps = (8 + len + 4 + 3) / 4;
        byte[] b = new byte[lamps * 4];
        b[0] = (byte) seq; b[1] = (byte) (seq >> 8); b[2] = (byte) flags; b[3] = (byte) sid; b[4] = (byte) len; b[5] = (byte) (len >> 8); b[6] = (byte) p6; b[7] = (byte) p7;
        System.arraycopy(data, off, b, 8, len);
        CRC32 c = new CRC32(); c.update(b, 0, 8 + len); long v = c.getValue();
        for (int k = 0; k < 4; k++) b[8 + len + k] = (byte) (v >> (8 * k));
        return b;
    }

    static byte[] sha256(byte[] b) throws Exception { return java.security.MessageDigest.getInstance("SHA-256").digest(b); }
    static String hex(byte[] b) { StringBuilder sb = new StringBuilder(); for (byte x : b) sb.append(String.format("%02x", x & 255)); return sb.toString(); }
    static long le32(byte[] b, int o) { return (b[o] & 255L) | ((b[o + 1] & 255L) << 8) | ((b[o + 2] & 255L) << 16) | ((b[o + 3] & 255L) << 24); }


    static int u16(byte[] b, int at) { return (b[at] & 255) | ((b[at + 1] & 255) << 8); }
    static long u32(byte[] b, int at) { return (u16(b, at) | ((long) u16(b, at + 2) << 16)); }
    static long num(Map<String, String> m, String k) { try { return Long.parseLong(m.get(k)); } catch (Exception e) { return -1; } }
}
