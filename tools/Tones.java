import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Line;
import javax.sound.sampled.Mixer;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.TargetDataLine;
import java.util.ArrayList;
import java.util.List;

/**
 * Pure Java audio frequency tone synthesizer using {@code javax.sound.sampled}.
 * <p>
 * Generates pure sine tones to stimulate the board's microphone during acoustic testing.
 * Implements 10 ms amplitude envelope ramping at onset and offset to eliminate transient
 * DC pops and speaker clicks.
 * </p>
 */
final class Tones {

    /** Standard CD-quality sample rate in Hz. */
    public static final float SAMPLE_RATE = 44100.0f;

    /** Selected audio output mixer (or {@code null} to use the system default). */
    static volatile Mixer.Info output;

    /** Master volume multiplier in range [0.0, 1.0]. */
    static volatile double volume = 1.0;

    private Tones() {
        // Non-instantiable utility class
    }

    /**
     * Enumerates available audio playback or recording devices present on the system.
     *
     * @param forOutput {@code true} to list output devices (speakers), {@code false} for input (microphones)
     * @return list of matching {@link Mixer.Info} handles
     */
    static List<Mixer.Info> devices(boolean forOutput) {
        List<Mixer.Info> matchedDevices = new ArrayList<>();
        Class<?> lineClass = forOutput ? SourceDataLine.class : TargetDataLine.class;
        Line.Info targetInfo = new Line.Info(lineClass);

        for (Mixer.Info info : AudioSystem.getMixerInfo()) {
            try {
                Mixer mixer = AudioSystem.getMixer(info);
                Line.Info[] available = forOutput
                        ? mixer.getSourceLineInfo(targetInfo)
                        : mixer.getTargetLineInfo(targetInfo);
                if (available.length > 0) {
                    matchedDevices.add(info);
                }
            } catch (RuntimeException ignored) {
                // Skip devices that cannot be queried
            }
        }
        return matchedDevices;
    }

    /**
     * Synthesizes and plays a pure sine wave synchronously.
     *
     * @param frequencyHz tone frequency in Hertz
     * @param seconds     playback duration in seconds
     * @param level       nominal amplitude [0.0, 1.0]
     * @throws Exception if opening or writing to the audio line fails
     */
    static void play(double frequencyHz, double seconds, double level) throws Exception {
        int totalSamples = (int) (SAMPLE_RATE * seconds);
        byte[] pcmBuffer = new byte[totalSamples * 2]; // 16-bit mono = 2 bytes per sample
        double rampSamples = 441.0; // 10 ms at 44.1 kHz

        for (int i = 0; i < totalSamples; i++) {
            // Envelope ramp to prevent audio clicks
            double rampIn = i / rampSamples;
            double rampOut = (totalSamples - i) / rampSamples;
            double envelope = Math.clamp(Math.min(rampIn, rampOut), 0.0, 1.0);

            double angle = 2.0 * Math.PI * frequencyHz * i / SAMPLE_RATE;
            short sample = (short) (Math.sin(angle) * 32767.0 * level * volume * envelope);

            pcmBuffer[2 * i] = (byte) (sample & 0xFF);
            pcmBuffer[2 * i + 1] = (byte) ((sample >> 8) & 0xFF);
        }

        AudioFormat format = new AudioFormat(SAMPLE_RATE, 16, 1, true, false);
        Mixer.Info chosenMixer = output;

        try (SourceDataLine line = (chosenMixer != null)
                ? AudioSystem.getSourceDataLine(format, chosenMixer)
                : AudioSystem.getSourceDataLine(format)) {

            line.open(format, pcmBuffer.length);
            line.start();
            line.write(pcmBuffer, 0, pcmBuffer.length);
            line.drain();
        }
    }

    /**
     * Starts tone playback asynchronously on a virtual thread.
     *
     * @param frequencyHz tone frequency in Hertz
     * @param seconds     duration in seconds
     * @param level       nominal amplitude [0.0, 1.0]
     * @return the running virtual {@link Thread}
     */
    static Thread playAsync(double frequencyHz, double seconds, double level) {
        return Thread.ofVirtual().name("tone-synthesizer").start(() -> {
            try {
                play(frequencyHz, seconds, level);
            } catch (Exception e) {
                System.err.println("Cannot play audio tone: " + e.getMessage());
            }
        });
    }
}
