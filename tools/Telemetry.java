import java.util.concurrent.locks.LockSupport;

/**
 * What the board shows or hears, read from the game pad reports of the HID node (not from the serial port): the colours of the ten bar LEDs and the
 * microphone waveform. The board switches it on with TELEM BAR|WAVE|BOTH|OFF. A virtual thread reads the reports (48 bytes: offset, flags, length,
 * payload, CRC-32) and hands the result to the sink.
 */
final class Telemetry implements AutoCloseable {
    interface Sink { void bar(int[] rgb); void wave(int[] samples); }

    private final HidReader rd; private final Thread t; private volatile boolean run = true;
    volatile long barFrames = 0, waveFrames = 0;

    Telemetry(String node, Sink sink) throws Exception {
        rd = new HidReader(node);
        t = Thread.ofVirtual().name("telemetry").start(() -> {
            byte[] frame = new byte[2 + 512]; int got = 0;
            while (run) {
                byte[] r = rd.q.poll();
                if (r == null) { LockSupport.parkNanos(1_000_000); continue; }
                java.util.zip.CRC32 c = new java.util.zip.CRC32(); c.update(r, 0, 44);
                if (c.getValue() != Proto.le32(r, 44)) continue;
                int flags = r[4] & 255, len = r[5] & 255; long off = Proto.le32(r, 0);
                if ((flags & 0x04) != 0 && len >= 30) {
                    int[] rgb = new int[10];
                    for (int i = 0; i < 10; i++) rgb[i] = ((r[6 + 3 * i] & 255) << 16) | ((r[7 + 3 * i] & 255) << 8) | (r[8 + 3 * i] & 255);
                    barFrames++; sink.bar(rgb);
                } else if ((flags & 0x08) != 0) {
                    if (off == 0) got = 0;
                    if (off != got || off + len > frame.length) { got = 0; continue; }       // a chunk went missing: wait for the next frame
                    System.arraycopy(r, 6, frame, (int) off, len); got += len;
                    if ((flags & 1) != 0 && got == frame.length) {
                        int div = (frame[0] & 255) | ((frame[1] & 255) << 8);
                        int[] v = new int[512];
                        for (int i = 0; i < 512; i++) v[i] = frame[2 + i] * div;
                        waveFrames++; sink.wave(v); got = 0;
                    }
                }
            }
        });
    }

    public void close() { run = false; try { t.join(300); } catch (InterruptedException ignored) { } rd.close(); }
}
