import java.io.ByteArrayOutputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HexFormat;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.locks.LockSupport;
import java.util.zip.CRC32;
import java.util.zip.Deflater;

/**
 * Protocol encoders, decoders, and constants for HID LampArray and Disk transfer protocols.
 * <p>
 * Implements report formatting for standard and proprietary USB HID usage pages:
 * <ul>
 *     <li><b>LampArray (Usage Page 0x59):</b> Control reports (ID 10), Range updates (ID 9),
 *         and Multi-lamp updates (ID 8).</li>
 *     <li><b>Disk over Lamps protocol:</b> Blocks encoded as lamp RGBI values, carrying file
 *         data, CRC-32 integrity checks, credits, resend requests, and SHA-256 verification.</li>
 * </ul>
 * </p>
 */
final class Proto {

    // --- Disk protocol flags ---
    public static final int F_CREDIT = 8;
    public static final int F_REQ = 16;
    public static final int F_HASH = 32;
    public static final int F_RESEND = 64;

    /** HID Report ID for gamepad input reports carrying return packets and telemetry. */
    public static final int GAMEPAD_ID = 11;

    /** HID Report ID for LampArray Multi-Update reports. */
    public static final int REPORT_MULTI_UPDATE = 8;

    /** HID Report ID for LampArray Range-Update reports. */
    public static final int REPORT_RANGE_UPDATE = 9;

    /** HID Report ID for LampArray Control reports. */
    public static final int REPORT_CONTROL = 10;

    /** First lamp ID reserved for the Disk data area (lamps 0..9 are physical base bar lamps). */
    public static final int Disk_FIRST = 10;

    /** Shared PRNG for error injection and session ID generation. */
    static final Random RNG = new Random(7);

    private static final HexFormat HEX = HexFormat.of();

    private Proto() {
        // Non-instantiable utility class
    }

    /**
     * Constructs a LampArray Control report (Report ID 10).
     *
     * @param autonomous {@code true} to let firmware run autonomous lighting,
     *                   {@code false} to allow host control
     * @return 2-byte HID feature/output report
     */
    static byte[] control(boolean autonomous) {
        return new byte[]{
                (byte) REPORT_CONTROL,
                (byte) (autonomous ? 1 : 0)
        };
    }

    /**
     * Constructs a LampArray Range Update report (Report ID 9).
     * <p>
     * Applies a single RGBI colour to all lamps from {@code first} to {@code last} (inclusive).
     * </p>
     *
     * @param first    starting lamp index
     * @param last     ending lamp index (inclusive)
     * @param r        red intensity (0..255)
     * @param g        green intensity (0..255)
     * @param b        blue intensity (0..255)
     * @param i        intensity scaling factor (0..255)
     * @param complete {@code true} if this report completes the frame update
     * @return 10-byte HID output report
     */
    static byte[] range(int first, int last, int r, int g, int b, int i, boolean complete) {
        return new byte[]{
                (byte) REPORT_RANGE_UPDATE,
                (byte) (complete ? 1 : 0),
                (byte) first,
                (byte) (first >> 8),
                (byte) last,
                (byte) (last >> 8),
                (byte) r,
                (byte) g,
                (byte) b,
                (byte) i
        };
    }

    /**
     * Constructs a LampArray Multi Update report (Report ID 8) for up to 8 lamps.
     * <p>
     * Layout (51 bytes total):
     * <ul>
     *     <li>Byte 0: Report ID (8)</li>
     *     <li>Byte 1: Lamp count (1..8)</li>
     *     <li>Byte 2: Update complete flag (0 or 1)</li>
     *     <li>Bytes 3..18: 8 &times; 16-bit little-endian lamp IDs</li>
     *     <li>Bytes 19..50: 8 &times; 4-byte RGBA/RGBI colour values</li>
     * </ul>
     * </p>
     *
     * @param ids      array of up to 8 lamp IDs
     * @param rgbi     array of [8][4] RGBI values for each lamp
     * @param count    number of valid lamp entries in the report (1..8)
     * @param complete {@code true} if this report signals the end of the update
     * @return 51-byte HID output report
     */
    static byte[] multi(int[] ids, int[][] rgbi, int count, boolean complete) {
        byte[] packet = new byte[51];
        packet[0] = (byte) REPORT_MULTI_UPDATE;
        packet[1] = (byte) count;
        packet[2] = (byte) (complete ? 1 : 0);

        for (int k = 0; k < count; k++) {
            // Little-endian lamp ID
            packet[3 + 2 * k] = (byte) ids[k];
            packet[4 + 2 * k] = (byte) (ids[k] >> 8);

            // RGBA / RGBI colour components
            for (int c = 0; c < 4; c++) {
                packet[19 + 4 * k + c] = (byte) rgbi[k][c];
            }
        }
        return packet;
    }

    /**
     * Compresses data using raw DEFLATE (no zlib headers or Adler-32 trailer) at maximum compression.
     *
     * @param input uncompressed data bytes
     * @return raw DEFLATE byte array
     */
    static byte[] deflateRaw(byte[] input) {
        Deflater deflater = new Deflater(Deflater.BEST_COMPRESSION, true); // true = raw deflate
        deflater.setInput(input);
        deflater.finish();

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        byte[] buffer = new byte[65536];
        while (!deflater.finished()) {
            int count = deflater.deflate(buffer);
            out.write(buffer, 0, count);
        }
        deflater.end();
        return out.toByteArray();
    }

    /**
     * Transmits a block over the LampArray as multi-updates of up to 8 lamps per report.
     *
     * @param hid       HID device interface
     * @param block     block data (must be a multiple of 4 bytes)
     * @param gapUs     microsecond pause between reports
     * @param injectPct simulated packet drop probability (0..100)
     * @return total number of reports sent
     */
    static int sendBlock(Hid hid, byte[] block, int gapUs, int injectPct) {
        int lampCount = block.length / 4;
        int reports = 0;

        for (int base = 0; base < lampCount; base += 8) {
            int count = Math.min(8, lampCount - base);
            int[] ids = new int[8];
            int[][] colours = new int[8][4];

            for (int k = 0; k < count; k++) {
                ids[k] = Disk_FIRST + base + k;
                for (int j = 0; j < 4; j++) {
                    colours[k][j] = block[(base + k) * 4 + j] & 0xFF;
                }
            }

            boolean isLast = (base + count >= lampCount);
            if (injectPct > 0 && RNG.nextInt(100) < injectPct) {
                // Drop this report to test CRC / retransmission logic
                reports++;
                park(gapUs);
                continue;
            }

            hid.write(multi(ids, colours, count, isLast));
            reports++;
            park(gapUs);
        }
        return reports;
    }

    /**
     * Transmits a SHA-256 digest and speed measurement to the board for on-screen comparison.
     *
     * @param hid         HID interface
     * @param sid         session ID
     * @param sha         32-byte SHA-256 digest
     * @param bytesPerSec measured transfer rate in bytes per second
     * @param which       0 for original file hash, 1 for received file hash
     */
    static void sendHash(Hid hid, int sid, byte[] sha, long bytesPerSec, int which) {
        byte[] payload = new byte[37];
        System.arraycopy(sha, 0, payload, 0, 32);

        // 32-bit little-endian speed
        for (int k = 0; k < 4; k++) {
            payload[32 + k] = (byte) (bytesPerSec >> (8 * k));
        }
        payload[36] = (byte) which;

        byte[] block = diskBlock(0, F_HASH, sid, payload, 0, payload.length);
        sendBlock(hid, block, 0, 0);
    }

    /**
     * Transmits a credit frame instructing the board to stream data packets.
     *
     * @param hid     HID interface
     * @param sid     session ID
     * @param wanted  next wanted byte offset
     * @param payload requested payload bytes per report
     * @param window  number of reports per credit window
     * @param gap10us inter-report pacing delay in 10-microsecond units
     * @param resend  {@code true} if this credit requests retransmission
     */
    static void sendCredit(Hid hid, int sid, long wanted, int payload, int window, int gap10us, boolean resend) {
        byte[] creditPayload = new byte[6];
        for (int k = 0; k < 4; k++) {
            creditPayload[k] = (byte) (wanted >> (8 * k));
        }
        creditPayload[4] = (byte) gap10us;
        creditPayload[5] = (byte) (gap10us >> 8);

        int flags = F_CREDIT | (resend ? F_RESEND : 0);
        byte[] block = diskBlock(0, flags, sid, payload, window, creditPayload, 0, creditPayload.length);
        sendBlock(hid, block, 0, 0);
    }

    /**
     * High-precision thread parking in microseconds.
     *
     * @param microseconds duration to pause
     */
    static void park(int microseconds) {
        if (microseconds > 0) {
            LockSupport.parkNanos(microseconds * 1_000L);
        }
    }

    /**
     * Constructs a Disk protocol block with default parameter bytes.
     */
    static byte[] diskBlock(int seq, int flags, int sid, byte[] data, int off, int len) {
        return diskBlock(seq, flags, sid, 0, 0, data, off, len);
    }

    /**
     * Constructs a formatted Disk protocol block: 8-byte header, data payload, and 4-byte CRC-32,
     * padded up to 4-byte whole lamp boundaries.
     *
     * @param seq   16-bit sequence number
     * @param flags 8-bit protocol flags
     * @param sid   8-bit session identifier
     * @param p6    parameter byte 6 (e.g. payload length per report)
     * @param p7    parameter byte 7 (e.g. window size)
     * @param data  payload byte array
     * @param off   offset within data
     * @param len   length of data to include
     * @return padded block byte array
     */
    static byte[] diskBlock(int seq, int flags, int sid, int p6, int p7, byte[] data, int off, int len) {
        int lampCount = (8 + len + 4 + 3) / 4;
        byte[] block = new byte[lampCount * 4];

        // 8-byte header
        block[0] = (byte) seq;
        block[1] = (byte) (seq >> 8);
        block[2] = (byte) flags;
        block[3] = (byte) sid;
        block[4] = (byte) len;
        block[5] = (byte) (len >> 8);
        block[6] = (byte) p6;
        block[7] = (byte) p7;

        // Payload
        System.arraycopy(data, off, block, 8, len);

        // CRC-32 over header + payload
        CRC32 crc = new CRC32();
        crc.update(block, 0, 8 + len);
        long crcValue = crc.getValue();

        for (int k = 0; k < 4; k++) {
            block[8 + len + k] = (byte) (crcValue >> (8 * k));
        }
        return block;
    }

    /**
     * Computes the SHA-256 digest of an input byte array.
     */
    static byte[] sha256(byte[] data) {
        try {
            return MessageDigest.getInstance("SHA-256").digest(data);
        } catch (NoSuchAlgorithmException e) {
            throw new IllegalStateException("SHA-256 not available", e);
        }
    }

    /**
     * Formats a byte array as a lower-case hexadecimal string.
     */
    static String hex(byte[] data) {
        return HEX.formatHex(data);
    }

    /**
     * Reads a 32-bit unsigned little-endian integer from a byte array.
     */
    static long le32(byte[] data, int offset) {
        return (data[offset] & 0xFFL)
                | ((data[offset + 1] & 0xFFL) << 8)
                | ((data[offset + 2] & 0xFFL) << 16)
                | ((data[offset + 3] & 0xFFL) << 24);
    }

    /**
     * Reads a 16-bit unsigned little-endian integer from a byte array.
     */
    static int u16(byte[] data, int offset) {
        return (data[offset] & 0xFF) | ((data[offset + 1] & 0xFF) << 8);
    }

    /**
     * Reads a 32-bit unsigned little-endian integer from a byte array using 16-bit halves.
     */
    static long u32(byte[] data, int offset) {
        return u16(data, offset) | ((long) u16(data, offset + 2) << 16);
    }

    /**
     * Safely parses a long value from a key-value dictionary.
     *
     * @param map the key-value map
     * @param key the property key to look up
     * @return the parsed long, or -1 if absent or malformed
     */
    static long num(Map<String, String> map, String key) {
        try {
            String val = map.get(key);
            return val != null ? Long.parseLong(val) : -1;
        } catch (NumberFormatException e) {
            return -1;
        }
    }
}
