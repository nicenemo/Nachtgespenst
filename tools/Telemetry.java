import java.util.concurrent.locks.LockSupport;
import java.util.zip.CRC32;

/**
 * Asynchronous decoder for live device telemetry received over USB/Bluetooth HID reports.
 * <p>
 * Decodes telemetry packets routed from the firmware over HID Gamepad input reports
 * (Report ID {@value Proto#GAMEPAD_ID}) without consuming serial bandwidth. Supported streams:
 * <ul>
 *     <li><b>Bar LED telemetry:</b> Live 10-lamp physical RGB colours.</li>
 *     <li><b>Microphone waveform:</b> 512-sample 16 kHz audio buffers, transmitted as chunked
 *         delta/divisor packets and reconstructed in memory.</li>
 * </ul>
 * </p>
 */
final class Telemetry implements AutoCloseable {

    /** Telemetry packet flag indicating bar LED RGB data. */
    public static final int FLAG_BAR = 0x04;

    /** Telemetry packet flag indicating microphone audio chunk. */
    public static final int FLAG_WAVE = 0x08;

    /** Telemetry packet flag indicating completion of the current multi-chunk frame. */
    public static final int FLAG_FRAME_COMPLETE = 0x01;

    /** Callback sink receiving decoded device telemetry. */
    interface Sink {
        /** Called with the 10-lamp RGB colours (0xRRGGBB). */
        void bar(int[] rgb);

        /** Called with 512 audio samples at 16 kHz. */
        void wave(int[] samples);
    }

    private final HidReader reader;
    private final Thread telemetryThread;
    private volatile boolean running = true;

    volatile long barFrames = 0;
    volatile long waveFrames = 0;

    /**
     * Connects to the device's HID node and begins telemetry packet decoding.
     *
     * @param hidNode path to the {@code /dev/hidrawN} node
     * @param sink    consumer for parsed telemetry updates
     * @throws Exception if opening the HID reader fails
     */
    Telemetry(String hidNode, Sink sink) throws Exception {
        this.reader = new HidReader(hidNode);

        this.telemetryThread = Thread.ofVirtual().name("telemetry-decoder").start(() -> {
            byte[] waveBuffer = new byte[2 + 512]; // 2-byte divisor followed by 512 8-bit samples
            int waveBytesReceived = 0;
            CRC32 crc = new CRC32();

            while (running) {
                byte[] report = reader.q.poll();
                if (report == null) {
                    LockSupport.parkNanos(1_000_000); // 1 ms sleep
                    continue;
                }

                // Verify 4-byte CRC-32 over bytes 0..43
                crc.reset();
                crc.update(report, 0, 44);
                long expectedCrc = Proto.le32(report, 44);
                if (crc.getValue() != expectedCrc) {
                    continue; // Corrupted packet, discard
                }

                long offset = Proto.le32(report, 0);
                int flags = report[4] & 0xFF;
                int len = report[5] & 0xFF;

                if ((flags & FLAG_BAR) != 0 && len >= 30) {
                    // 10 bar lamps, 3 bytes (R, G, B) per lamp starting at byte offset 6
                    int[] rgb = new int[10];
                    for (int i = 0; i < 10; i++) {
                        int r = report[6 + 3 * i] & 0xFF;
                        int g = report[7 + 3 * i] & 0xFF;
                        int b = report[8 + 3 * i] & 0xFF;
                        rgb[i] = (r << 16) | (g << 8) | b;
                    }
                    barFrames++;
                    sink.bar(rgb);
                } else if ((flags & FLAG_WAVE) != 0) {
                    if (offset == 0) {
                        waveBytesReceived = 0;
                    }

                    if (offset != waveBytesReceived || offset + len > waveBuffer.length) {
                        waveBytesReceived = 0; // Missing chunk, reset and wait for next frame
                        continue;
                    }

                    System.arraycopy(report, 6, waveBuffer, (int) offset, len);
                    waveBytesReceived += len;

                    if ((flags & FLAG_FRAME_COMPLETE) != 0 && waveBytesReceived == waveBuffer.length) {
                        int divisor = (waveBuffer[0] & 0xFF) | ((waveBuffer[1] & 0xFF) << 8);
                        int[] reconstructedSamples = new int[512];
                        for (int i = 0; i < 512; i++) {
                            reconstructedSamples[i] = waveBuffer[2 + i] * divisor;
                        }
                        waveFrames++;
                        sink.wave(reconstructedSamples);
                        waveBytesReceived = 0;
                    }
                }
            }
        });
    }

    /**
     * Stops the decoding thread and closes the underlying HID reader.
     */
    @Override
    public void close() {
        running = false;
        try {
            telemetryThread.join(300);
        } catch (InterruptedException ignored) {
            Thread.currentThread().interrupt();
        }
        reader.close();
    }
}
