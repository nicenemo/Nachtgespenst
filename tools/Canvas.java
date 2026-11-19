import java.util.*;

/**
 * A grid of lamps on the Lamps: screen page of a board, painted over the LampArray. set() changes the picture, present() sends only the
 * lamps that differ from what the board shows: runs of one colour as range updates (one report for many lamps), the rest as multi
 * updates (eight lamps per report), the last report carrying "update complete".
 */
final class Canvas {
    final Hid h; final int cols, rows;
    final int[] cur, next;                     // 0xRRGGBB per lamp; cur is what the board shows (-1 = not known yet)
    long reports = 0, lamps = 0;
    volatile java.util.function.Consumer<int[]> listener;    // gets a copy of the picture (0xRRGGBB per lamp) after every present()

    Canvas(Ser s, Hid h, int cols, int rows) throws Exception {
        this.h = h; this.cols = cols; this.rows = rows;
        String r = s.cmd("lamps grid " + cols + " " + rows, 3000);
        if (r == null || !r.startsWith("OK")) throw new IllegalStateException("the board refused a grid of " + cols + " x " + rows + ": " + r);
        h.write(Proto.control(false));
        cur = new int[cols * rows]; next = new int[cols * rows];
        Arrays.fill(cur, -1);
    }

    void set(int x, int y, int rgb) { if (x >= 0 && y >= 0 && x < cols && y < rows) next[y * cols + x] = rgb; }
    void fill(int rgb) { Arrays.fill(next, rgb); }
    void setAll(int[] pic) { System.arraycopy(pic, 0, next, 0, next.length); }

    /** Sends what changed; returns the number of reports. */
    int present() {
        List<byte[]> out = new ArrayList<>();
        List<int[]> singles = new ArrayList<>();                 // {id, rgb}
        int n = next.length;
        for (int i = 0; i < n; ) {
            if (next[i] == cur[i]) { i++; continue; }
            int j = i + 1;
            while (j < n && next[j] == next[i] && next[j] != cur[j]) j++;   // a run of lamps that all change to the same colour
            if (j - i >= 4) {
                int c = next[i];
                out.add(Proto.range(Proto.Disk_FIRST + i, Proto.Disk_FIRST + j - 1, (c >> 16) & 255, (c >> 8) & 255, c & 255, 255, false));
                lamps += j - i;
            } else for (int k = i; k < j; k++) singles.add(new int[]{Proto.Disk_FIRST + k, next[k]});
            i = j;
        }
        lamps += singles.size();
        int total = out.size() + (singles.size() + 7) / 8;
        if (total == 0) return 0;
        List<byte[]> multis = new ArrayList<>();
        for (int base = 0; base < singles.size(); base += 8) {
            int cnt = Math.min(8, singles.size() - base); int[] ids = new int[8]; int[][] c = new int[8][4];
            for (int k = 0; k < cnt; k++) { int[] s = singles.get(base + k); ids[k] = s[0]; c[k] = new int[]{(s[1] >> 16) & 255, (s[1] >> 8) & 255, s[1] & 255, 255}; }
            multis.add(Proto.multi(ids, c, cnt, false));
        }
        out.addAll(multis);
        byte[] last = out.get(out.size() - 1);
        if ((last[0] & 255) == 9) last[1] = 1; else last[2] = 1;          // update complete on the last report
        for (byte[] r : out) h.write(r);
        System.arraycopy(next, 0, cur, 0, n);
        reports += out.size();
        java.util.function.Consumer<int[]> l = listener; if (l != null) l.accept(next.clone());
        return out.size();
    }
}
