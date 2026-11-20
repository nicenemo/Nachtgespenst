import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.LockSupport;
import java.util.zip.CRC32;

/**
 * Host-side implementation of the virtual Disk protocol over HID LampArray.
 * <p>
 * Demonstrates high-throughput bidirectional data transfer across standard USB HID endpoints:
 * <ul>
 *     <li><b>SAVE (Host &rarr; Device):</b> Sends uncompressed or compressed file blocks encoded
 *         into the RGBI values of the lamp matrix. Acknowledgements and flow-control signals
 *         return via keyboard scan codes (F13..F24) read through Linux {@link Evdev}.</li>
 *     <li><b>LOAD (Device &rarr; Host):</b> Streams data packets from the device back to the host
 *         inside 48-byte HID gamepad input reports. Pacing credits and retransmission requests
 *         flow backwards from host to device over the lamps.</li>
 *     <li><b>Adaptive Rate Control (VBR):</b> Dynamically expands payload size and credit window
 *         during error-free transmission, and performs exponential backoff upon packet drop or NAK.</li>
 *     <li><b>End-to-end verification:</b> Verifies per-block CRC-32 and compares 256-bit SHA-256
 *         cryptographic hashes computed independently by both host and device.</li>
 * </ul>
 * </p>
 */
final class DiskDrive {

    /** Callback interface for streaming status logs and assertion checks. */
    interface Reporter {
        void check(String name, boolean ok, String detail);
        void info(String msg);
    }

    /** Configuration parameters controlling the Disk transfer session. */
    static final class Options {
        String via = "usb";
        String tag = "S3";
        String name = "pg164.txt";
        String file = null;
        boolean fast = false;
        boolean load = true;
        boolean grab = true;
        int inject = 0;
        int min = 16;
        int max = 1024;
        int paceUs = 0;

        Options() {}
    }

    // Keyboard acknowledgement scan codes emitted by device firmware
    static final int KEY_F13 = 183;      // Base acknowledgement (sequence mod 8)
    static final int KEY_NAK = 191;      // Negative acknowledgement (checksum / sequence failure)
    static final int KEY_DONE_OK = 193;  // Transfer decoded and saved successfully (F23)
    static final int KEY_DONE_BAD = 194; // Verification failed (F24)

    /** URL for test payload: Jules Verne's "Twenty Thousand Leagues under the Sea" (Project Gutenberg #164). */
    static final String GUTENBERG_URL = "https://gutenberg.pglaf.org/1/6/164/164-0.txt";

    final Options options;
    final Reporter reporter;

    DiskDrive(Options options, Reporter reporter) {
        this.options = options;
        this.reporter = reporter;
    }

    /**
     * Loads the test payload book into memory, downloading it if not present on disk.
     */
    byte[] diskBook() throws Exception {
        Path filePath = Path.of(options.file != null ? options.file : "tools/data/pg164.txt");
        if (!Files.exists(filePath)) {
            if (options.file != null) {
                throw new IOException("No such file: " + filePath);
            }
            reporter.info("Downloading test payload from " + GUTENBERG_URL);
            HttpClient client = HttpClient.newBuilder()
                    .followRedirects(HttpClient.Redirect.NORMAL)
                    .build();
            HttpRequest request = HttpRequest.newBuilder(URI.create(GUTENBERG_URL)).build();
            HttpResponse<byte[]> response = client.send(request, HttpResponse.BodyHandlers.ofByteArray());
            if (response.statusCode() != 200) {
                throw new IOException("Download failed with HTTP " + response.statusCode());
            }
            Files.createDirectories(filePath.getParent());
            Files.write(filePath, response.body());
        }
        return Files.readAllBytes(filePath);
    }

    /**
     * Executes the end-to-end disk transfer test with automatic link preference restoration.
     */
    void disk(Ser ser, Hid hid) throws Exception {
        try {
            diskRun(ser, hid);
        } finally {
            if (!options.via.equals("usb")) {
                ser.cmd("prefer usb", 2000);
            }
        }
    }

    /**
     * Runs the forward SAVE transfer pipeline followed optionally by the reverse LOAD verification.
     */
    void diskRun(Ser ser, Hid hid) throws Exception {
        boolean isUsb = options.via.equals("usb");
        byte[] originalBook = diskBook();
        byte[] compressedData = Proto.deflateRaw(originalBook);

        CRC32 wholeChecksum = new CRC32();
        wholeChecksum.update(compressedData);

        reporter.info(String.format("Book %d bytes, raw deflate %d bytes (%.0f%%)",
                originalBook.length, compressedData.length, 100.0 * compressedData.length / originalBook.length));

        int totalPages = Integer.parseInt(ser.kv("info").get("pages"));
        int diskPage = totalPages - 3;
        ser.cmd("layer " + diskPage, 3000);
        Thread.sleep(600);

        reporter.check("on the Disk page", "Disk".equals(ser.kv("state").getOrDefault("title", "")), ser.kv("state").toString());
        ser.cmd("disk reset", 2000);

        if (!isUsb) {
            ser.cmd("prefer ble", 2000);
        }

        String evdevNode = Evdev.findKeyboard(options.tag, isUsb);
        if (evdevNode == null) {
            reporter.check("keyboard device of the board found", false, "no evdev node found");
            return;
        }

        try (Evdev keyboard = new Evdev(evdevNode, options.grab)) {
            reporter.info("Acknowledgements read from " + evdevNode
                    + (keyboard.grabbed ? " (grabbed: keys do not reach desktop)" : " (NOT grabbed: desktop sees keys)"));

            hid.write(Proto.control(false));
            Thread.sleep(300);

            int sessionId = 1 + Proto.RNG.nextInt(250);
            byte[] nameBytes = options.name.getBytes(StandardCharsets.US_ASCII);
            byte[] headerPayload = new byte[19 + nameBytes.length];

            // Magic header "NGD1"
            System.arraycopy("NGD1".getBytes(StandardCharsets.US_ASCII), 0, headerPayload, 0, 4);

            // Lengths: compressed, uncompressed, CRC-32
            for (int k = 0; k < 4; k++) {
                headerPayload[4 + k] = (byte) (compressedData.length >> (8 * k));
                headerPayload[8 + k] = (byte) (originalBook.length >> (8 * k));
                headerPayload[12 + k] = (byte) (wholeChecksum.getValue() >> (8 * k));
            }
            headerPayload[16] = 1; // Compression method: 1 = raw DEFLATE
            headerPayload[17] = (byte) (options.fast ? 1 : 0);
            headerPayload[18] = (byte) nameBytes.length;
            System.arraycopy(nameBytes, 0, headerPayload, 19, nameBytes.length);

            int gapDelayUs = Math.max(0, options.paceUs);
            int blockLamps = isUsb ? 256 : 64;
            int windowSize = isUsb ? 4 : 2;
            int consecutiveCleanWindows = 0;

            List<int[]> sentBlocks = new ArrayList<>(); // seq -> [offset, payloadLength]
            int errorStreak = 0;
            int lastAckedSeq = -1;
            int totalErrors = 0;
            int timeouts = 0;
            int naks = 0;
            int windowCount = 0;
            int reportsSent = 0;
            int resends = 0;
            int maxBlockBytes = 0;

            long startTime = System.nanoTime();
            boolean transferDone = false;
            Integer finalResponseKey = null;

            // Transmit block 0 (File metadata header)
            for (int attempt = 0; attempt < 5 && lastAckedSeq < 0; attempt++) {
                keyboard.keys.clear();
                reportsSent += Proto.sendBlock(
                        hid,
                        Proto.diskBlock(0, 4 | 2, sessionId, headerPayload, 0, headerPayload.length),
                        gapDelayUs,
                        0
                );
                Integer ackKey = keyboard.keys.poll(isUsb ? 1500 : 6000, TimeUnit.MILLISECONDS);
                if (ackKey != null && ackKey == KEY_F13) {
                    lastAckedSeq = 0;
                }
            }

            reporter.check("the board answers the first block (Disk page open?)", lastAckedSeq == 0, "");
            if (lastAckedSeq != 0) {
                return;
            }

            sentBlocks.add(new int[]{0, 0}); // Block 0 record
            int currentSeq = 1;
            int currentOffset = 0;

            while (!transferDone) {
                if (Thread.currentThread().isInterrupted()) {
                    throw new InterruptedException();
                }

                keyboard.keys.clear();
                int blocksInWindow = 0;
                int lastWindowSeq = -1;
                boolean isFinalBlock = false;

                for (int w = 0; w < windowSize && currentOffset < compressedData.length; w++) {
                    int payloadLen = Math.min(blockLamps * 4 - 16, compressedData.length - currentOffset);
                    isFinalBlock = (currentOffset + payloadLen >= compressedData.length);
                    boolean wantAck = isFinalBlock || (w == windowSize - 1);
                    int flags = (wantAck ? 2 : 0) | (isFinalBlock ? 1 : 0);

                    byte[] packetPayload = new byte[4 + payloadLen];
                    for (int k = 0; k < 4; k++) {
                        packetPayload[k] = (byte) (currentOffset >> (8 * k));
                    }
                    System.arraycopy(compressedData, currentOffset, packetPayload, 4, payloadLen);

                    reportsSent += Proto.sendBlock(
                            hid,
                            Proto.diskBlock(currentSeq, flags, sessionId, packetPayload, 0, packetPayload.length),
                            gapDelayUs,
                            options.inject
                    );

                    while (sentBlocks.size() <= currentSeq) {
                        sentBlocks.add(null);
                    }
                    sentBlocks.set(currentSeq, new int[]{currentOffset, payloadLen});
                    maxBlockBytes = Math.max(maxBlockBytes, payloadLen);

                    currentOffset += payloadLen;
                    lastWindowSeq = currentSeq;
                    currentSeq++;
                    blocksInWindow++;
                    isFinalBlock = (currentOffset >= compressedData.length);
                }

                windowCount++;

                // Await window acknowledgement
                long waitTimeoutMs = (isUsb ? 400 : 4000)
                        + (long) (blocksInWindow * blockLamps / 8) * (gapDelayUs / 1000 + (isUsb ? 1 : 40));
                if (isFinalBlock) {
                    waitTimeoutMs = Math.max(waitTimeoutMs, 20000);
                }

                int expectedKey = KEY_F13 + (lastWindowSeq & 7);
                Integer receivedKey = null;
                long deadline = System.currentTimeMillis() + waitTimeoutMs;

                while (System.currentTimeMillis() < deadline) {
                    Integer polled = keyboard.keys.poll(20, TimeUnit.MILLISECONDS);
                    if (polled == null) continue;
                    if (polled == expectedKey || polled == KEY_NAK || polled == KEY_DONE_OK || polled == KEY_DONE_BAD) {
                        receivedKey = polled;
                        break;
                    }
                }

                if (receivedKey != null && (receivedKey == KEY_DONE_OK || receivedKey == KEY_DONE_BAD)) {
                    finalResponseKey = receivedKey;
                    lastAckedSeq = lastWindowSeq;
                    break;
                }

                if (receivedKey != null && receivedKey == expectedKey) {
                    lastAckedSeq = lastWindowSeq;
                    consecutiveCleanWindows++;
                    errorStreak = 0;

                    if (gapDelayUs > (isUsb ? 0 : 5000)) {
                        gapDelayUs = Math.max(isUsb ? 0 : 5000, gapDelayUs * 3 / 4);
                    }

                    if (isFinalBlock) {
                        finalResponseKey = keyboard.keys.poll(20000, TimeUnit.MILLISECONDS);
                        break;
                    }

                    if (consecutiveCleanWindows % 3 == 0) {
                        if (blockLamps < options.max) {
                            blockLamps = Math.min(options.max, blockLamps * 2);
                        } else if (windowSize < 16) {
                            windowSize *= 2;
                        }
                    }
                } else {
                    totalErrors++;
                    consecutiveCleanWindows = 0;
                    if (receivedKey == null) timeouts++; else naks++;

                    // Backoff on failure
                    blockLamps = Math.max(options.min, blockLamps / 2);
                    windowSize = Math.max(1, windowSize / 2);
                    if (++errorStreak >= 2) {
                        gapDelayUs = (gapDelayUs == 0) ? 200 : Math.min(3000, gapDelayUs * 2);
                    }

                    int resumeSeq = lastAckedSeq + 1;
                    currentOffset = sentBlocks.get(lastAckedSeq)[0] + sentBlocks.get(lastAckedSeq)[1];
                    while (sentBlocks.size() > resumeSeq) {
                        sentBlocks.removeLast();
                    }
                    currentSeq = resumeSeq;
                    resends++;

                    if (totalErrors > 400) {
                        reporter.check("the transfer completes", false, "too many errors (>400)");
                        return;
                    }
                }

                if (windowCount % 20 == 0) {
                    reporter.info(String.format("  %d%%  block %d lamps, window %d, gap %d us, errors %d",
                            100L * currentOffset / compressedData.length, blockLamps, windowSize, gapDelayUs, totalErrors));
                }
            }

            double elapsedSec = (System.nanoTime() - startTime) / 1e9;
            Thread.sleep(500);

            Map<String, String> status = ser.kv("disk");
            reporter.info(String.format(
                    "Sent %d compressed bytes in %.2f s = %.1f kB/s (%.1f kB/s text), %d reports, %d windows, %d errors (%d timeouts, %d naks), max block %d bytes, window %d, gap %d us",
                    compressedData.length, elapsedSec, compressedData.length / 1024.0 / elapsedSec,
                    originalBook.length / 1024.0 / elapsedSec, reportsSent, windowCount,
                    totalErrors, timeouts, naks, maxBlockBytes, windowSize, gapDelayUs));
            reporter.info("Board status: " + status);

            reporter.check("the board reports the transfer finished", finalResponseKey != null && finalResponseKey == KEY_DONE_OK, String.valueOf(finalResponseKey));
            reporter.check("the board decoded the book (crc ok, original size)",
                    "done".equals(status.get("state")) && "1".equals(status.get("crc")) && Proto.num(status, "original") == originalBook.length,
                    status.toString());

            if ("1".equals(status.get("sd"))) {
                reporter.check("the file was written to the SD card", "1".equals(status.get("saved")), status.toString());
            } else {
                reporter.info("No SD card in the board: file retained in RAM");
            }

            // Verify lines counted
            long expectedLines = originalBook.length - new String(originalBook, StandardCharsets.ISO_8859_1).replace("\n", "").length();
            reporter.check("the board counted the lines of the book", Proto.num(status, "lines") == expectedLines, "lines=" + status.get("lines"));

            // Cross-verify SHA-256 hash
            byte[] originalSha = Proto.sha256(originalBook);
            reporter.check("the SHA-256 the board computed equals the original's",
                    Proto.hex(originalSha).equals(status.get("sha")),
                    "board " + status.get("sha") + " pc " + Proto.hex(originalSha));

            Proto.sendHash(hid, sessionId, originalSha, (long) (compressedData.length / elapsedSec), 0);
            Thread.sleep(300);
            status = ser.kv("disk");
            reporter.check("the board compares the PC's hash with its own (match)", "1".equals(status.get("match")), status.toString());

            // LOAD verification (receive file back over gamepad buttons)
            if (options.load) {
                String devPort = Hid.find(options.tag, isUsb);
                try (HidReader hidReader = new HidReader(devPort)) {
                    double[] statsArray = new double[9];
                    byte[] returnedBook = diskLoadBack(ser, hid, hidReader, options.name, sessionId, statsArray);
                    reporter.check("the book came back over the gamepad buttons", returnedBook != null, "");

                    if (returnedBook != null) {
                        String receivedHex = Proto.hex(Proto.sha256(returnedBook));
                        reporter.check("SHA-256 of the received file equals the original",
                                receivedHex.equals(Proto.hex(originalSha)) && Arrays.equals(returnedBook, originalBook),
                                receivedHex + " vs " + Proto.hex(originalSha));
                        reporter.check("the board's own hash (in its header report) equals the received file's", statsArray[8] == 1.0, "");

                        Proto.sendHash(hid, sessionId, Proto.sha256(returnedBook), (long) (statsArray[0] * 1024), 1);
                        Thread.sleep(500);

                        Map<String, String> statusAfterLoad = ser.kv("disk");
                        reporter.info("Board status after load: " + statusAfterLoad);
                        reporter.check("the board shows a match with the hash of what the PC received", "1".equals(statusAfterLoad.get("match")), statusAfterLoad.toString());
                        reporter.check("the board measured its own sending speed", Proto.num(statusAfterLoad, "txkbps10") > 0, "txkbps10=" + statusAfterLoad.get("txkbps10"));
                    }
                }
            }
        }
    }

    /**
     * Reads a file streamed back from the board across gamepad reports.
     */
    byte[] diskLoadBack(Ser ser, Hid hid, HidReader reader, String fileName, int sessionId, double[] stats) throws Exception {
        boolean isUsb = options.via.equals("usb");
        int payloadSize = isUsb ? 32 : 16;
        int windowSize = isUsb ? 32 : 8;
        int pacingDelay10us = 0;
        int consecutiveClean = 0;
        int errorCount = 0;
        int creditCount = 0;
        int resendCount = 0;
        int duplicateCount = 0;
        int headersReceived = 0;

        byte[] nameBytes = fileName.getBytes(StandardCharsets.US_ASCII);
        reader.q.clear();
        hid.write(Proto.control(false));
        Thread.sleep(100);

        // Send initial request block
        Proto.sendBlock(
                hid,
                Proto.diskBlock(0, Proto.F_REQ, sessionId, payloadSize, windowSize, nameBytes, 0, nameBytes.length),
                0,
                0
        );

        byte[] receivedFile = null;
        byte[] deviceSha = null;
        int totalExpectedBytes = -1;
        long nextWantedOffset = 0;
        long startTime = 0;
        long lastCreditTime = 0;
        long lastProgressTime = System.nanoTime();
        long lastResendTime = 0;
        int packetsSinceLastCredit = 0;
        boolean resendNeeded = false;

        long deadline = System.nanoTime() + 40_000_000_000L; // 40 seconds
        long nextLogTime = System.nanoTime() + 2_000_000_000L;
        CRC32 packetCrc = new CRC32();

        while (System.nanoTime() < deadline) {
            if (Thread.currentThread().isInterrupted()) {
                throw new InterruptedException();
            }

            byte[] report;
            int batchCount = 0;
            while ((report = reader.q.poll()) != null && batchCount++ < 400) {
                packetCrc.reset();
                packetCrc.update(report, 0, 44);
                if (packetCrc.getValue() != Proto.le32(report, 44)) {
                    errorCount++;
                    consecutiveClean = 0;
                    resendNeeded = true;
                    continue;
                }

                long packetOffset = Proto.le32(report, 0);
                int flags = report[4] & 0xFF;
                int packetLen = report[5] & 0xFF;

                if ((flags & 2) != 0) {
                    // Header report: file size + device SHA-256
                    headersReceived++;
                    if (receivedFile == null) {
                        totalExpectedBytes = (int) Proto.le32(report, 6);
                        deviceSha = Arrays.copyOfRange(report, 10, 42);
                        receivedFile = new byte[totalExpectedBytes];
                        startTime = System.nanoTime();

                        Proto.sendCredit(hid, sessionId, 0, payloadSize, windowSize, pacingDelay10us, false);
                        lastCreditTime = System.nanoTime();
                        creditCount++;
                    }
                    continue;
                }

                if (receivedFile == null) continue;

                if (packetOffset == nextWantedOffset && packetOffset + packetLen <= totalExpectedBytes) {
                    System.arraycopy(report, 6, receivedFile, (int) packetOffset, packetLen);
                    nextWantedOffset += packetLen;
                    packetsSinceLastCredit++;
                    consecutiveClean++;
                    lastProgressTime = System.nanoTime();
                } else if (packetOffset > nextWantedOffset) {
                    resendNeeded = true;
                    errorCount++;
                    consecutiveClean = 0;
                } else {
                    duplicateCount++;
                }
            }

            long now = System.nanoTime();
            if (now > nextLogTime) {
                nextLogTime = now + 2_000_000_000L;
                reporter.info(String.format("  Load: wanted %d of %d, errors %d, resends %d, payload %d, window %d, reports read %d",
                        nextWantedOffset, totalExpectedBytes, errorCount, resendCount, payloadSize, windowSize, reader.reports));
            }

            if (receivedFile != null && nextWantedOffset >= totalExpectedBytes) {
                break;
            }

            if (receivedFile != null && (resendNeeded || now - lastProgressTime > 400_000_000L) && now - lastResendTime > 40_000_000L) {
                payloadSize = Math.max(4, payloadSize / 2);
                windowSize = Math.max(2, windowSize / 2);
                pacingDelay10us = (pacingDelay10us == 0) ? 20 : Math.min(60000, pacingDelay10us * 2);

                Proto.sendCredit(hid, sessionId, nextWantedOffset, payloadSize, windowSize, pacingDelay10us, true);
                creditCount++;
                resendCount++;
                lastResendTime = now;
                lastCreditTime = now;
                resendNeeded = false;
                packetsSinceLastCredit = 0;
                lastProgressTime = now;
                reader.q.clear();
                continue;
            }

            if (receivedFile != null && (packetsSinceLastCredit >= Math.max(1, windowSize / 2) || now - lastCreditTime > 15_000_000L)) {
                if (consecutiveClean >= 300) {
                    consecutiveClean = 0;
                    if (payloadSize < 38) payloadSize = Math.min(38, payloadSize * 2);
                    else if (windowSize < 200) windowSize = Math.min(200, windowSize * 2);
                    else if (pacingDelay10us > 0) pacingDelay10us = pacingDelay10us * 3 / 4;
                }
                Proto.sendCredit(hid, sessionId, nextWantedOffset, payloadSize, windowSize, pacingDelay10us, false);
                creditCount++;
                lastCreditTime = now;
                packetsSinceLastCredit = 0;
            }

            if (reader.q.isEmpty()) {
                LockSupport.parkNanos(100_000);
            }
        }

        if (receivedFile == null || nextWantedOffset < totalExpectedBytes) {
            reporter.info(String.format("Load stalled: received %d of %d bytes, errors %d, resends %d",
                    nextWantedOffset, totalExpectedBytes, errorCount, resendCount));
            return null;
        }

        double durationSec = (System.nanoTime() - startTime) / 1e9;
        Proto.sendCredit(hid, sessionId, totalExpectedBytes, payloadSize, windowSize, pacingDelay10us, false);

        stats[0] = totalExpectedBytes / 1024.0 / durationSec;
        stats[1] = errorCount;
        stats[2] = resendCount;
        stats[3] = creditCount;
        stats[4] = payloadSize;
        stats[5] = windowSize;
        stats[6] = pacingDelay10us;
        stats[7] = durationSec;

        byte[] computedSha = Proto.sha256(receivedFile);
        reporter.info(String.format("Loaded back %d bytes in %.2f s = %.1f kB/s over gamepad buttons (%d errors, %d resends)",
                totalExpectedBytes, durationSec, stats[0], errorCount, resendCount));
        stats[8] = Arrays.equals(computedSha, deviceSha) ? 1.0 : 0.0;
        return receivedFile;
    }

    int barPage(Ser ser) throws Exception {
        return Integer.parseInt(ser.kv("info").get("pages")) - 1;
    }
}
