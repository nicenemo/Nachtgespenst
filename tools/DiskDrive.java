import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.zip.CRC32;

/**
 * The Disk demo from the PC side: sends a book to the Disk page over the lamps (SAVE: blocks, acknowledged by F13..F24) and receives it
 * back over the game pad reports (LOAD: credits through the lamps), with variable bit rate, SHA-256 on both ends and checks. Used by
 * LampTest (disk) and by the Demo runner.
 */
final class DiskDrive {
    /** Where the checks and the messages go (LampTest counts them, the Demo runner prints them). */
    interface Reporter { void check(String name, boolean ok, String detail); void info(String msg); }

    static final class Options {
        String via = "usb", tag = "S3", name = "pg164.txt", file = null;
        boolean fast = false, load = true, grab = true;
        int inject = 0, min = 16, max = 1024, paceUs = 0;
    }

    final Options o; final Reporter rep;
    DiskDrive(Options o, Reporter rep) { this.o = o; this.rep = rep; }

    static final String GUTENBERG = "https://gutenberg.pglaf.org/1/6/164/164-0.txt";   // Twenty Thousand Leagues under the Sea (Project Gutenberg #164, public domain)

    byte[] diskBook() throws Exception {
        Path f = Path.of(o.file != null ? o.file : "tools/data/pg164.txt");
        if (!Files.exists(f)) {
            if (o.file != null) throw new IOException("no such file " + f);
            rep.info("downloading " + GUTENBERG);
            var rsp = java.net.http.HttpClient.newBuilder().followRedirects(java.net.http.HttpClient.Redirect.NORMAL).build()
                    .send(java.net.http.HttpRequest.newBuilder(java.net.URI.create(GUTENBERG)).build(), java.net.http.HttpResponse.BodyHandlers.ofByteArray());
            if (rsp.statusCode() != 200) throw new IOException("download failed: HTTP " + rsp.statusCode());
            Files.createDirectories(f.getParent()); Files.write(f, rsp.body());
        }
        return Files.readAllBytes(f);
    }


    /** One block in the lamp format of Disk.h: header, payload, CRC-32, padded to whole lamps. */

    static final int KEY_F13 = 183, KEY_NAK = 191, KEY_DONE_OK = 193, KEY_DONE_BAD = 194;   // F13..F20 ack (block number mod 8), F21 nak, F23 ok, F24 bad

    

    /**
     * Sends a book to the Disk page and checks that it arrives. Variable bit rate: the block size (lamps per block), the window (blocks
     * per acknowledgement) and the gap between reports grow after clean windows and are cut back after every error (a NAK or a missing
     * acknowledgement), so a bad link (Bluetooth, --inject) settles on what it can carry.
     */
    void disk(Ser s, Hid h) throws Exception {
        try { diskRun(s, h); } finally { if (!o.via.equals("usb")) s.cmd("prefer usb", 2000); }
    }

    void diskRun(Ser s, Hid h) throws Exception {
        boolean usb = o.via.equals("usb");
        byte[] book = diskBook(); byte[] comp = Proto.deflateRaw(book);
        CRC32 whole = new CRC32(); whole.update(comp);
        rep.info(String.format("book %d bytes, raw deflate %d bytes (%.0f%%)", book.length, comp.length, 100.0 * comp.length / book.length));
        int pages = Integer.parseInt(s.kv("info").get("pages")), diskPage = pages - 3;
        s.cmd("layer " + diskPage, 3000); Thread.sleep(600);
        rep.check("on the Disk page", s.kv("state").getOrDefault("title", "").equals("Disk"), s.kv("state").toString());
        s.cmd("disk reset", 2000);
        if (!usb) s.cmd("prefer ble", 2000);                      // the answers (keys and game pad reports) must leave over the link under test, not over USB
        String ev = Evdev.findKeyboard(o.tag, usb);
        if (ev == null) { rep.check("keyboard device of the board found", false, "no evdev node"); return; }
        try (Evdev ed = new Evdev(ev, o.grab)) {
            rep.info("acknowledgements read from " + ev + (ed.grabbed ? " (grabbed: the F keys do not reach the desktop)" : " (NOT grabbed: the desktop sees the F keys)"));
            h.write(Proto.control(false)); Thread.sleep(300);
            int sid = 1 + Proto.RNG.nextInt(250);
            byte[] nm = o.name.getBytes(StandardCharsets.US_ASCII);
            byte[] hdr = new byte[19 + nm.length];
            System.arraycopy("NGD1".getBytes(StandardCharsets.US_ASCII), 0, hdr, 0, 4);
            hdr[17] = (byte) (o.fast ? 1 : 0); hdr[18] = (byte) nm.length; System.arraycopy(nm, 0, hdr, 19, nm.length);
            for (int k = 0; k < 4; k++) { hdr[4 + k] = (byte) (comp.length >> (8 * k)); hdr[8 + k] = (byte) (book.length >> (8 * k)); hdr[12 + k] = (byte) (whole.getValue() >> (8 * k)); }
            hdr[16] = 1;                                              // method: raw deflate

            int gap = o.paceUs > 0 ? o.paceUs : 0;                        // microseconds between reports (USB: none)
            int blockLamps = usb ? 256 : 64, window = usb ? 4 : 2, clean = 0;
            List<int[]> sent = new ArrayList<>();                     // per sequence number: {offset, length}; entry 0 is the header block
            int errStreak = 0, acked = -1, errors = 0, timeouts = 0, naks = 0, windows = 0, reportsSent = 0, resends = 0, maxBlock = 0;
            long t0 = System.nanoTime(); boolean done = false; Integer finalKey = null;
            int off = 0, seq = 0;
            // block 0: header, acknowledge wanted
            for (int attempt = 0; attempt < 5 && acked < 0; attempt++) {
                ed.keys.clear();
                reportsSent += Proto.sendBlock(h, Proto.diskBlock(0, 4 | 2, sid, hdr, 0, hdr.length), gap, 0);
                Integer k = ed.keys.poll(usb ? 1500 : 6000, TimeUnit.MILLISECONDS);
                if (k != null && k == KEY_F13) acked = 0;
            }
            rep.check("the board answers the first block (Disk page open?)", acked == 0, "");
            if (acked != 0) return;
            sent.add(new int[]{0, 0});
            seq = 1;
            while (!done) {
                if (Thread.currentThread().isInterrupted()) throw new InterruptedException();
                // one window: blocks seq .. seq+window-1, acknowledgement wanted on the last
                int first = seq, nBlocks = 0, lastSeq = -1; boolean lastFlag = false;
                ed.keys.clear();                                           // an old acknowledgement with the same number modulo 8 must not count for this window
                long wt0 = System.nanoTime();
                for (int w = 0; w < window && off < comp.length; w++) {
                    int payload = Math.min(blockLamps * 4 - 16, comp.length - off);       // 16 = header, offset, CRC
                    boolean isLast = off + payload >= comp.length;
                    boolean wantAck = isLast || w == window - 1;
                    int flags = (wantAck ? 2 : 0) | (isLast ? 1 : 0);
                    byte[] pl = new byte[4 + payload];                         // every data block starts with the byte offset of its data
                    for (int k = 0; k < 4; k++) pl[k] = (byte) (off >> (8 * k));
                    System.arraycopy(comp, off, pl, 4, payload);
                    reportsSent += Proto.sendBlock(h, Proto.diskBlock(seq, flags, sid, pl, 0, pl.length), gap, o.inject);
                    while (sent.size() <= seq) sent.add(null);
                    sent.set(seq, new int[]{off, payload});
                    maxBlock = Math.max(maxBlock, payload);
                    off += payload; lastSeq = seq; seq++; nBlocks++; lastFlag = isLast;
                }
                windows++;
                // wait for this window's acknowledgement (generous on a slow link)
                long waitMs = (usb ? 400 : 4000) + (long) (nBlocks * blockLamps / 8) * (gap / 1000 + (usb ? 1 : 40));
                if (lastFlag) waitMs = Math.max(waitMs, 20000);          // after the last block the board unpacks the file and writes the SD card before it answers
                int want = KEY_F13 + (lastSeq & 7);
                Integer got = null; long end = System.currentTimeMillis() + waitMs;
                while (System.currentTimeMillis() < end) {
                    Integer k = ed.keys.poll(20, TimeUnit.MILLISECONDS);
                    if (k == null) continue;
                    if (k == want || k == KEY_NAK || k == KEY_DONE_OK || k == KEY_DONE_BAD) { got = k; break; }
                }
                if (got != null && (got == KEY_DONE_OK || got == KEY_DONE_BAD)) { finalKey = got; acked = lastSeq; break; }
                if (got != null && got == want) {
                    acked = lastSeq; clean++; errStreak = 0;
                    if (gap > (usb ? 0 : 5000)) gap = Math.max(usb ? 0 : 5000, gap * 3 / 4);      // every clean window shortens the pause again
                    if (lastFlag) { Integer k = ed.keys.poll(20000, TimeUnit.MILLISECONDS); finalKey = k; break; }   // the board decodes, then F23 or F24
                    if (clean % 3 == 0) {                              // three clean windows: faster
                        if (blockLamps < o.max) blockLamps = Math.min(o.max, blockLamps * 2);
                        else if (window < 16) window *= 2;

                    }
                } else {
                    errors++; clean = 0;
                    if (got == null) timeouts++; else naks++;
                    // cut back: smaller blocks, smaller window, longer gap; resend from the first block that was not acknowledged
                    blockLamps = Math.max(o.min, blockLamps / 2); window = Math.max(1, window / 2);
                    if (++errStreak >= 2) gap = gap == 0 ? 200 : Math.min(3000, gap * 2);   // a pause only after errors in a row (a lost report on its own is just a smaller block)
                    int resumeSeq = acked + 1;
                    off = sent.get(acked)[0] + sent.get(acked)[1];     // the data after the last acknowledged block
                    while (sent.size() > resumeSeq) sent.remove(sent.size() - 1);
                    seq = resumeSeq; resends++;
                    if (errors > 400) { rep.check("the transfer completes", false, "too many errors"); return; }
                }
                if (windows % 20 == 0) rep.info(String.format("  %d%%  block %d lamps, window %d, gap %d us, errors %d", 100L * off / comp.length, blockLamps, window, gap, errors));
            }
            double sec = (System.nanoTime() - t0) / 1e9;
            Thread.sleep(500);
            Map<String, String> st = s.kv("disk");
            rep.info(String.format("sent %d compressed bytes in %.2f s = %.1f kB/s (%.1f kB/s of text), %d reports, %d windows, %d errors (%d timeouts, %d naks), largest block %d bytes, last block %d lamps, window %d, gap %d us",
                    comp.length, sec, comp.length / 1024.0 / sec, book.length / 1024.0 / sec, reportsSent, windows, errors, timeouts, naks, maxBlock, blockLamps, window, gap));
            rep.info("board: " + st);
            rep.check("the board reports the transfer finished", finalKey != null && finalKey == KEY_DONE_OK, String.valueOf(finalKey));
            rep.check("the board decoded the book (crc ok, original size)", "done".equals(st.get("state")) && "1".equals(st.get("crc")) && Proto.num(st, "original") == book.length, st.toString());
            if ("1".equals(st.get("sd"))) rep.check("the file was written to the SD card", "1".equals(st.get("saved")), st.toString());
            else rep.info("no SD card in the board: the file stays in RAM");
            rep.check("the board counted the lines of the book", Proto.num(st, "lines") == book.length - new String(book, StandardCharsets.ISO_8859_1).replace("\n", "").length(), "lines=" + st.get("lines"));
            // The PC's hash of the original goes to the board for the demo screen; the board shows its own next to it.
            byte[] orig = Proto.sha256(book);
            rep.check("the SHA-256 the board computed equals the original's", Proto.hex(orig).equals(st.get("sha")), "board " + st.get("sha") + " pc " + Proto.hex(orig));
            Proto.sendHash(h, sid, orig, (long) (comp.length / sec), 0);
            Thread.sleep(300);
            st = s.kv("disk");
            rep.check("the board compares the PC's hash with its own (match)", "1".equals(st.get("match")), st.toString());
            if (o.load) {
                String devPort = Hid.find(o.tag, usb);
                try (HidReader rd = new HidReader(devPort)) {
                    double[] ls = new double[9];
                    byte[] back = diskLoadBack(s, h, rd, o.name, sid, ls);
                    rep.check("the book came back over the gamepad buttons", back != null, "");
                    if (back != null) {
                        String got = Proto.hex(Proto.sha256(back));
                        rep.check("SHA-256 of the received file equals the original", got.equals(Proto.hex(orig)) && Arrays.equals(back, book), got + " vs " + Proto.hex(orig));
                        rep.check("the board's own hash (in its header report) equals the received file's", ls[8] == 1, "");
                        Proto.sendHash(h, sid, Proto.sha256(back), (long) (ls[0] * 1024), 1);
                        Thread.sleep(500);
                        Map<String, String> st2 = s.kv("disk");
                        rep.info("board after the load: " + st2);
                        rep.check("the board shows a match with the hash of what the PC received", "1".equals(st2.get("match")), st2.toString());
                        rep.check("the board measured its own sending speed", Proto.num(st2, "txkbps10") > 0, "txkbps10=" + st2.get("txkbps10"));
                    }
                }
            }
        }
    }


    // ------------------------------------------------------------------ disk: LOAD, the file comes back over the gamepad buttons




    /**
     * Asks the board for the file back and receives it as gamepad input reports: 48 bytes per report (offset, flags, length, up to 38
     * bytes of data, CRC-32). The lamps are the way back: every credit (the next offset wanted, the payload size, the window and a
     * gap) is a lamp frame, and a resend request makes the board go back. Variable bit rate: after clean stretches the payload,
     * the window and the gap are opened up, after every error they are cut back.
     */
    byte[] diskLoadBack(Ser s, Hid h, HidReader rd, String name, int sid, double[] stats) throws Exception {
        boolean usb = o.via.equals("usb");
        int payload = usb ? 32 : 16, window = usb ? 32 : 8, gap = 0, clean = 0, errors = 0, credits = 0, resends = 0, dups = 0, aheads = 0, headers = 0; long dupBytes = 0;
        byte[] nm = name.getBytes(StandardCharsets.US_ASCII);
        rd.q.clear();
        h.write(Proto.control(false)); Thread.sleep(100);
        Proto.sendBlock(h, Proto.diskBlock(0, Proto.F_REQ, sid, payload, window, nm, 0, nm.length), 0, 0);
        byte[] got = null, devSha = null; int total = -1; long wanted = 0, t0 = 0, lastCredit = 0, lastProgress = System.nanoTime(), lastResend = 0; int sinceCredit = 0; boolean needResend = false;
        long deadline = System.nanoTime() + 40_000_000_000L; long nextLog = System.nanoTime() + 2_000_000_000L;
        while (System.nanoTime() < deadline) {
            if (Thread.currentThread().isInterrupted()) throw new InterruptedException();
            byte[] r; int batch = 0;
            while ((r = rd.q.poll()) != null && batch++ < 400) {
                CRC32 c = new CRC32(); c.update(r, 0, 44);
                if (c.getValue() != Proto.le32(r, 44)) { errors++; clean = 0; needResend = true; continue; }
                long off = Proto.le32(r, 0); int flags = r[4] & 255, len = r[5] & 255;
                if ((flags & 2) != 0) {                                   // the header report: size and the board's SHA-256
                    headers++;
                    if (got == null) { total = (int) Proto.le32(r, 6); devSha = Arrays.copyOfRange(r, 10, 42); got = new byte[total]; t0 = System.nanoTime(); Proto.sendCredit(h, sid, 0, payload, window, gap, false); lastCredit = System.nanoTime(); credits++; }
                    continue;
                }
                if (got == null) continue;
                if (off == wanted && off + len <= total) { System.arraycopy(r, 6, got, (int) off, len); wanted += len; sinceCredit++; clean++; lastProgress = System.nanoTime(); }
                else if (off > wanted) { needResend = true; errors++; aheads++; clean = 0; }   // a report went missing
                else { dups++; dupBytes += len; }                                    // a report we already have
            }
            long now = System.nanoTime();
            if (now > nextLog) { nextLog = now + 2_000_000_000L; rep.info(String.format("  load: wanted %d of %d, errors %d, resends %d, payload %d, window %d, gap %d, reports read %d, duplicates %d, headers %d", wanted, total, errors, resends, payload, window, gap * 10, rd.reports, dups, headers)); }
            if (got != null && wanted >= total) break;
            if (got != null && (needResend || now - lastProgress > 400_000_000L) && now - lastResend > 40_000_000L) {
                payload = Math.max(4, payload / 2); window = Math.max(2, window / 2); gap = gap == 0 ? 20 : Math.min(60000, gap * 2);   // cut back
                Proto.sendCredit(h, sid, wanted, payload, window, gap, true); credits++; resends++;
                lastResend = now; lastCredit = now; needResend = false; sinceCredit = 0; lastProgress = now; rd.q.clear();
                continue;
            }
            if (got != null && (sinceCredit >= Math.max(1, window / 2) || now - lastCredit > 15_000_000L)) {
                if (clean >= 300) { clean = 0; if (payload < 38) payload = Math.min(38, payload * 2); else if (window < 200) window = Math.min(200, window * 2); else if (gap > 0) gap = gap * 3 / 4; }
                Proto.sendCredit(h, sid, wanted, payload, window, gap, false); credits++;
                lastCredit = now; sinceCredit = 0;
            }
            if (rd.q.isEmpty()) java.util.concurrent.locks.LockSupport.parkNanos(100_000);
        }
        if (got == null || wanted < total) {
            rep.info(String.format("load stalled: header %s, wanted %d of %d, %d report errors, %d resend requests, %d credits, payload %d, window %d, gap %d, %d reports read",
                    got != null, wanted, total, errors, resends, credits, payload, window, gap * 10, rd.reports));
            return null;
        }
        double sec = (System.nanoTime() - t0) / 1e9;
        Proto.sendCredit(h, sid, total, payload, window, gap, false);          // the last credit tells the board that everything arrived
        stats[0] = total / 1024.0 / sec; stats[1] = errors; stats[2] = resends; stats[3] = credits; stats[4] = payload; stats[5] = window; stats[6] = gap; stats[7] = sec;
        byte[] sha = Proto.sha256(got);
        rep.info(String.format("loaded back %d bytes in %.2f s = %.1f kB/s over the gamepad (%d report errors, %d resend requests, %d credits, %d duplicate reports, %d header reports, %d reports read; last payload %d bytes, window %d, gap %d us)",
                total, sec, stats[0], errors, resends, credits, dups, headers, rd.reports, payload, window, gap * 10));
        stats[8] = Arrays.equals(sha, devSha) ? 1 : 0;
        return got;
    }

    int barPage(Ser s) throws Exception { return Integer.parseInt(s.kv("info").get("pages")) - 1; }
}
