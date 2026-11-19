import javax.sound.sampled.*;

/** Plays tones through the speaker of the PC (javax.sound, no Linux tools), to test the microphone of a board. */
final class Tones {
    static final float RATE = 44100f;

    /** A sine of hz Hz for seconds at the given level (0..1); returns when it has been played. */
    static void play(double hz, double seconds, double level) throws Exception {
        byte[] buf = new byte[(int) (RATE * seconds) * 2];
        for (int i = 0; i < buf.length / 2; i++) {
            double env = Math.min(1, Math.min(i / 441.0, (buf.length / 2 - i) / 441.0));         // 10 ms ramps against clicks
            short v = (short) (Math.sin(2 * Math.PI * hz * i / RATE) * 32767 * level * env);
            buf[2 * i] = (byte) v; buf[2 * i + 1] = (byte) (v >> 8);
        }
        AudioFormat fmt = new AudioFormat(RATE, 16, 1, true, false);
        try (SourceDataLine line = AudioSystem.getSourceDataLine(fmt)) {
            line.open(fmt, buf.length); line.start(); line.write(buf, 0, buf.length); line.drain();
        }
    }

    /** The tone starts in a virtual thread so that the caller can read the board while it sounds. */
    static Thread playAsync(double hz, double seconds, double level) {
        return Thread.ofVirtual().name("tone").start(() -> { try { play(hz, seconds, level); } catch (Exception e) { System.err.println("cannot play a tone: " + e); } });
    }
}
