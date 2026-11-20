import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.function.Consumer;

/**
 * 2D virtual canvas mapped onto the board's on-screen LampArray grid.
 * <p>
 * Optimizes network/USB bandwidth during {@link #present()} by transmitting only
 * lamps that changed since the last frame:
 * <ul>
 *     <li>Contiguous runs of &ge; 4 identical color changes are collapsed into a single
 *         {@link Proto#REPORT_RANGE_UPDATE} report (Report ID 9).</li>
 *     <li>Remaining changes are batched into {@link Proto#REPORT_MULTI_UPDATE} reports
 *         (Report ID 8), up to 8 lamps per packet.</li>
 *     <li>The final report of the frame asserts the "update complete" clock edge flag.</li>
 * </ul>
 * </p>
 */
final class Canvas {

    private final Hid hid;
    final int cols;
    final int rows;

    /** Framebuffer currently shown on the device (0xRRGGBB per pixel; -1 indicates unknown state). */
    final int[] cur;

    /** Next framebuffer to be presented (0xRRGGBB per pixel). */
    final int[] next;

    long reports = 0;
    long lamps = 0;

    /** Optional listener receiving a defensive copy of the presented 0xRRGGBB pixels after each frame. */
    volatile Consumer<int[]> listener;

    /**
     * Initializes the grid dimensions on the device and configures host control.
     *
     * @param ser  serial interface to configure display mode
     * @param hid  HID interface to transmit lamp updates
     * @param cols number of horizontal columns
     * @param rows number of vertical rows
     * @throws Exception if the board rejects the grid configuration
     */
    Canvas(Ser ser, Hid hid, int cols, int rows) throws Exception {
        this.hid = hid;
        this.cols = cols;
        this.rows = rows;

        String reply = ser.cmd("lamps grid " + cols + " " + rows, 3000);
        if (reply == null || !reply.startsWith("OK")) {
            throw new IllegalStateException("The board refused a grid of " + cols + " x " + rows + ": " + reply);
        }

        hid.write(Proto.control(false));

        int totalPixels = cols * rows;
        this.cur = new int[totalPixels];
        this.next = new int[totalPixels];
        Arrays.fill(cur, -1);
    }

    /**
     * Sets the colour of a single lamp at (x, y). Out-of-bounds coordinates are ignored.
     *
     * @param x   column index (0..cols-1)
     * @param y   row index (0..rows-1)
     * @param rgb 24-bit 0xRRGGBB colour
     */
    void set(int x, int y, int rgb) {
        if (x >= 0 && y >= 0 && x < cols && y < rows) {
            next[y * cols + x] = rgb;
        }
    }

    /**
     * Fills the entire canvas with a single colour.
     *
     * @param rgb 24-bit 0xRRGGBB colour
     */
    void fill(int rgb) {
        Arrays.fill(next, rgb);
    }

    /**
     * Copies an entire pixel array into the next framebuffer.
     *
     * @param pic array of length {@code cols * rows} in 0xRRGGBB format
     */
    void setAll(int[] pic) {
        System.arraycopy(pic, 0, next, 0, next.length);
    }

    /**
     * Computes the difference between {@code cur} and {@code next}, compiles optimal
     * range and multi-lamp HID reports, transmits them to the device, and marks
     * the frame complete.
     *
     * @return total number of HID reports sent
     */
    int present() {
        List<byte[]> reportsToSend = new ArrayList<>();
        List<int[]> singleUpdates = new ArrayList<>(); // Pairs of [lampId, rgb]
        int totalLamps = next.length;

        for (int i = 0; i < totalLamps; ) {
            if (next[i] == cur[i]) {
                i++;
                continue;
            }

            int j = i + 1;
            while (j < totalLamps && next[j] == next[i] && next[j] != cur[j]) {
                j++;
            }

            int runLength = j - i;
            if (runLength >= 4) {
                // Range update: 1 report updates many lamps
                int color = next[i];
                int firstId = Proto.Disk_FIRST + i;
                int lastId = Proto.Disk_FIRST + j - 1;
                reportsToSend.add(Proto.range(
                        firstId,
                        lastId,
                        (color >> 16) & 0xFF,
                        (color >> 8) & 0xFF,
                        color & 0xFF,
                        0xFF,
                        false
                ));
                lamps += runLength;
            } else {
                for (int k = i; k < j; k++) {
                    singleUpdates.add(new int[]{Proto.Disk_FIRST + k, next[k]});
                }
            }
            i = j;
        }

        lamps += singleUpdates.size();
        int totalReports = reportsToSend.size() + (singleUpdates.size() + 7) / 8;
        if (totalReports == 0) {
            return 0;
        }

        // Batch individual changes into 8-lamp multi updates
        for (int base = 0; base < singleUpdates.size(); base += 8) {
            int count = Math.min(8, singleUpdates.size() - base);
            int[] ids = new int[8];
            int[][] rgbi = new int[8][4];

            for (int k = 0; k < count; k++) {
                int[] entry = singleUpdates.get(base + k);
                ids[k] = entry[0];
                int color = entry[1];
                rgbi[k][0] = (color >> 16) & 0xFF;
                rgbi[k][1] = (color >> 8) & 0xFF;
                rgbi[k][2] = color & 0xFF;
                rgbi[k][3] = 0xFF;
            }
            reportsToSend.add(Proto.multi(ids, rgbi, count, false));
        }

        // Set "update complete" flag on the very last report to latch changes
        byte[] lastReport = reportsToSend.getLast();
        if ((lastReport[0] & 0xFF) == Proto.REPORT_RANGE_UPDATE) {
            lastReport[1] = 1; // Range update 'complete' offset is byte 1
        } else {
            lastReport[2] = 1; // Multi update 'complete' offset is byte 2
        }

        for (byte[] report : reportsToSend) {
            hid.write(report);
        }

        System.arraycopy(next, 0, cur, 0, totalLamps);
        reports += reportsToSend.size();

        Consumer<int[]> l = listener;
        if (l != null) {
            l.accept(next.clone());
        }

        return reportsToSend.size();
    }
}
