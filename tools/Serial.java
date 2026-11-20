///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//SOURCES Boards.java
//JAVA 27
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;

import java.nio.charset.StandardCharsets;
import java.util.Optional;

/**
 * Command-line utility to send one or more text commands to a Nachtgespenst board
 * over its serial command shell and display the responses.
 * <pre>
 *   jbang tools/Serial.java S3 "layer 26" "state"
 * </pre>
 */
public class Serial {

    private static void fail(String message, int exitCode) {
        System.err.println("error: " + message);
        System.exit(exitCode);
    }

    public static void main(String[] args) throws Exception {
        if (args.length < 2) {
            fail("usage: jbang tools/Serial.java S3|C2 \"COMMAND\" [\"COMMAND\" ...]", 2);
        }

        Boards.Board board = null;
        try {
            board = Boards.parse(args[0]);
        } catch (IllegalArgumentException e) {
            fail(e.getMessage(), 2);
        }

        Optional<SerialPort> portOpt = Boards.find(board);
        if (portOpt.isEmpty()) {
            fail("No " + board.tag() + " board detected on USB (expected VID:PID " + board.usbId() + ")", 1);
        }

        SerialPort port = portOpt.get();
        port.setBaudRate(115200);
        port.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);

        if (!port.openPort()) {
            fail(Boards.SANDBOX_HINT, 3);
        }

        int missedCommands = 0;
        try {
            for (int i = 0; i < args.length - 1; i++) {
                String cmd = args[i + 1];
                if (cmd.trim().toUpperCase().startsWith("SHOT")) {
                    System.out.println(cmd + " -> skipped: SHOT is binary; use SerialTest --shot");
                    continue;
                }

                String reply = sendAndReceive(port, cmd, 3000);
                if (reply == null && i == 0) {
                    // Port open on CoreS3 can cause a reset race; retry the first command once
                    Thread.sleep(1500);
                    reply = sendAndReceive(port, cmd, 3000);
                }

                if (reply == null) {
                    missedCommands++;
                    System.out.println(cmd + " -> (no reply)");
                } else {
                    System.out.println(reply);
                    String continuation;
                    while ((continuation = readContinuationLine(port, 250)) != null) {
                        System.out.println(continuation);
                    }
                }
            }
        } finally {
            port.closePort();
        }

        System.exit(missedCommands == 0 ? 0 : 1);
    }

    /**
     * Transmits a command and returns the primary status line starting with {@code OK} or {@code ERR}.
     */
    static String sendAndReceive(SerialPort port, String command, long timeoutMs) throws InterruptedException {
        flushReceiveBuffer(port);
        byte[] payload = (command + "\n").getBytes(StandardCharsets.US_ASCII);
        port.writeBytes(payload, payload.length);

        StringBuilder lineBuffer = new StringBuilder();
        byte[] singleByte = new byte[1];
        long deadline = System.currentTimeMillis() + timeoutMs;

        while (System.currentTimeMillis() < deadline) {
            if (port.bytesAvailable() > 0 && port.readBytes(singleByte, 1) == 1) {
                char c = (char) (singleByte[0] & 0xFF);
                if (c == '\n') {
                    String line = lineBuffer.toString().trim();
                    lineBuffer.setLength(0);
                    if (line.startsWith("OK") || line.startsWith("ERR")) {
                        return line;
                    }
                } else if (c != '\r') {
                    lineBuffer.append(c);
                }
            } else {
                Thread.sleep(2);
            }
        }
        return null;
    }

    /**
     * Reads subsequent lines of a multi-line response until the board has been silent for {@code silenceMs}.
     */
    static String readContinuationLine(SerialPort port, long silenceMs) throws InterruptedException {
        StringBuilder lineBuffer = new StringBuilder();
        byte[] singleByte = new byte[1];
        long deadline = System.currentTimeMillis() + silenceMs;

        while (System.currentTimeMillis() < deadline) {
            if (port.bytesAvailable() > 0 && port.readBytes(singleByte, 1) == 1) {
                char c = (char) (singleByte[0] & 0xFF);
                if (c == '\n') {
                    return lineBuffer.toString();
                }
                if (c != '\r') {
                    lineBuffer.append(c);
                }
                deadline = System.currentTimeMillis() + silenceMs; // Reset quiet timer on character arrival
            } else {
                Thread.sleep(2);
            }
        }
        return !lineBuffer.isEmpty() ? lineBuffer.toString() : null;
    }

    private static void flushReceiveBuffer(SerialPort port) throws InterruptedException {
        Thread.sleep(50);
        byte[] trash = new byte[4096];
        while (port.bytesAvailable() > 0) {
            port.readBytes(trash, Math.min(trash.length, port.bytesAvailable()));
        }
    }
}
