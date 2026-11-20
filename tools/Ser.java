import com.fazecast.jSerialComm.SerialPort;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * High-level serial communication client for Nachtgespenst boards.
 * <p>
 * Manages synchronous request-response exchanges, streaming multi-line command output,
 * key-value telemetry parsing, and binary frame grabs (e.g. {@code SHOT} screenshots).
 * </p>
 */
final class Ser implements AutoCloseable {

    private static final int DEFAULT_BAUD_RATE = 115200;
    private final SerialPort port;

    /**
     * Initializes and opens the specified serial port in non-blocking mode.
     *
     * @param port the jSerialComm serial port instance
     * @throws IllegalStateException if the port cannot be opened
     */
    Ser(SerialPort port) {
        this.port = port;
        port.setBaudRate(DEFAULT_BAUD_RATE);
        port.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
        if (!port.openPort()) {
            throw new IllegalStateException("Cannot open serial port " + port.getSystemPortPath());
        }
    }

    /**
     * Transmits a command string terminated by a newline character.
     *
     * @param command the command text to send
     */
    void send(String command) {
        byte[] bytes = (command + "\n").getBytes(StandardCharsets.US_ASCII);
        port.writeBytes(bytes, bytes.length);
    }

    /**
     * Flushes and discards any pending input bytes in the serial receive buffer.
     *
     * @throws InterruptedException if interrupted while sleeping
     */
    void drain() throws InterruptedException {
        Thread.sleep(150);
        byte[] buffer = new byte[4096];
        while (port.bytesAvailable() > 0) {
            port.readBytes(buffer, Math.min(buffer.length, port.bytesAvailable()));
        }
    }

    /**
     * Reads a single non-empty line of text from the serial port within the specified timeout.
     *
     * @param timeoutMs maximum time to wait in milliseconds
     * @return the trimmed line, or {@code null} if the timeout elapsed
     * @throws InterruptedException if the calling thread is interrupted
     */
    String line(long timeoutMs) throws InterruptedException {
        long deadline = System.currentTimeMillis() + timeoutMs;
        StringBuilder sb = new StringBuilder();
        byte[] singleByte = new byte[1];

        while (System.currentTimeMillis() < deadline) {
            if (port.bytesAvailable() > 0 && port.readBytes(singleByte, 1) == 1) {
                if (singleByte[0] == '\n') {
                    String s = sb.toString().trim();
                    if (!s.isEmpty()) {
                        return s;
                    }
                    sb.setLength(0);
                } else if (singleByte[0] != '\r') {
                    sb.append((char) singleByte[0]);
                }
            } else {
                Thread.sleep(1);
            }
        }
        return null;
    }

    /**
     * Sends a command and returns all response lines, waiting until the board has been
     * quiet for {@code quietMs}.
     *
     * @param command the command string to send
     * @param firstLineTimeoutMs timeout for the initial response line
     * @param quietMs silence duration signaling end of output
     * @return list of reply lines
     * @throws InterruptedException if interrupted while waiting
     */
    List<String> cmdAll(String command, long firstLineTimeoutMs, long quietMs) throws InterruptedException {
        List<String> output = new ArrayList<>();
        String first = cmd(command, firstLineTimeoutMs);
        if (first == null) {
            return output;
        }

        output.add(first);
        String continuation;
        while ((continuation = line(quietMs)) != null) {
            output.add(continuation);
        }
        return output;
    }

    /**
     * Sends a command and waits for a response line beginning with {@code OK} or {@code ERR}.
     *
     * @param command   the command string to send
     * @param timeoutMs maximum time to wait
     * @return the status line, or {@code null} on timeout
     * @throws InterruptedException if interrupted while waiting
     */
    String cmd(String command, long timeoutMs) throws InterruptedException {
        send(command);
        long deadline = System.currentTimeMillis() + timeoutMs;

        while (System.currentTimeMillis() < deadline) {
            long remaining = Math.max(1, deadline - System.currentTimeMillis());
            String l = line(remaining);
            if (l == null) {
                return null;
            }
            if (l.startsWith("OK") || l.startsWith("ERR")) {
                return l;
            }
        }
        return null;
    }

    /**
     * Reads an exact number of binary bytes from the serial port.
     *
     * @param count     number of bytes requested
     * @param timeoutMs maximum time to wait
     * @return the byte array, or {@code null} if fewer bytes were received before timeout
     * @throws InterruptedException if interrupted
     */
    byte[] bytes(int count, long timeoutMs) throws InterruptedException {
        byte[] buffer = new byte[count];
        int received = 0;
        long deadline = System.currentTimeMillis() + timeoutMs;

        while (received < count && System.currentTimeMillis() < deadline) {
            int available = port.bytesAvailable();
            if (available > 0) {
                received += port.readBytes(buffer, Math.min(available, count - received), received);
            } else {
                Thread.sleep(2);
            }
        }
        return received == count ? buffer : null;
    }

    /**
     * Executes a command and parses its whitespace-delimited {@code key=value} tokens into a map.
     *
     * @param command the command text
     * @return map of parsed key-value properties
     * @throws InterruptedException if interrupted
     */
    Map<String, String> kv(String command) throws InterruptedException {
        Map<String, String> result = new LinkedHashMap<>();
        String response = cmd(command, 3000);
        if (response != null) {
            for (String token : response.split("\\s+")) {
                int eq = token.indexOf('=');
                if (eq > 0) {
                    result.put(token.substring(0, eq), token.substring(eq + 1));
                }
            }
        }
        return result;
    }

    /**
     * Uncompressed 24-bit RGB display screenshot captured from the board.
     *
     * @param width  display width in pixels
     * @param height display height in pixels
     * @param pixels raw RGB888 pixel bytes (length {@code width * height * 3})
     */
    record Shot(int width, int height, byte[] pixels) {
        /**
         * Extracts the RGB color triplet at coordinates (x, y).
         *
         * @param x horizontal coordinate (0..width-1)
         * @param y vertical coordinate (0..height-1)
         * @return 3-element integer array containing [red, green, blue] in 0..255
         */
        int[] at(int x, int y) {
            int index = (y * width + x) * 3;
            return new int[]{
                    pixels[index] & 0xFF,
                    pixels[index + 1] & 0xFF,
                    pixels[index + 2] & 0xFF
            };
        }

        /** Compatibility alias for width. */
        int w() { return width; }

        /** Compatibility alias for height. */
        int h() { return height; }

        /** Compatibility alias for pixels. */
        byte[] px() { return pixels; }
    }

    /**
     * Captures a full framebuffer screenshot from the device using the {@code shot} command.
     *
     * @return the captured {@link Shot}, or {@code null} on failure or timeout
     * @throws Exception if an I/O error occurs
     */
    Shot shot() throws Exception {
        drain();
        send("shot");

        String header = null;
        long deadline = System.currentTimeMillis() + 5000;
        while (System.currentTimeMillis() < deadline) {
            String s = line(1000);
            if (s != null && s.startsWith("OK SHOT")) {
                header = s;
                break;
            }
        }

        if (header == null) {
            return null;
        }

        String[] parts = header.split("\\s+");
        int width = Integer.parseInt(parts[2]);
        int height = Integer.parseInt(parts[3]);

        byte[] pixelData = bytes(width * height * 3, 90000);
        drain();

        return pixelData == null ? null : new Shot(width, height, pixelData);
    }

    /**
     * Closes the underlying serial port.
     */
    @Override
    public void close() {
        port.closePort();
    }
}
