///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//SOURCES Boards.java
//JAVA 17+
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;

import java.nio.charset.StandardCharsets;
import java.util.Optional;

/**
 * Send one or more serial commands to a board and print each reply.
 *
 * <pre>
 * jbang tools/Serial.java S3 "LAYER 26" "STATE?"      # S3 or C2 (core2/cores3 also work)
 * </pre>
 *
 * Text commands only (PING, INFO, STATE?, LAYER n, TAP x y, KEY l i, PREFER ..., ONLY ...). SHOT is binary: use
 * {@code jbang tools/SerialTest.java --shot DIR}. A command that gets no reply within 3 seconds is reported and the
 * exit code is 1 (the very first command is retried once: opening the CoreS3 port can reset the chip).
 *
 * Run it as ONE plain command (no pipe, cd, &amp;&amp;, ;, VAR=x prefix) so that it matches "*jbang*Serial.java*" in
 * sandbox.excludedCommands and runs outside the Claude Code sandbox. Exit codes: 0 all answered, 1 no reply,
 * 2 usage, 3 port cannot be opened (sandbox or permissions).
 */
public class Serial {
    static void die(String msg, int code) { System.err.println("error: " + msg); System.exit(code); }

    public static void main(String[] args) throws Exception {
        if (args.length < 2) die("usage: jbang tools/Serial.java S3|C2 \"COMMAND\" [\"COMMAND\" ...]", 2);
        Boards.Board b = null;
        try { b = Boards.parse(args[0]); } catch (IllegalArgumentException e) { die(e.getMessage(), 2); }
        Optional<SerialPort> found = Boards.find(b);
        if (found.isEmpty()) die("no " + b.tag() + " board on USB (looked for " + b.usbId() + ")", 1);
        SerialPort p = found.get();
        p.setBaudRate(115200);
        p.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, 0, 0);
        if (!p.openPort()) die(Boards.SANDBOX_HINT, 3);

        int missed = 0;
        try {
            for (int i = 0; i < args.length - 1; i++) {
                String cmd = args[i + 1];
                if (cmd.trim().toUpperCase().startsWith("SHOT")) { System.out.println(cmd + " -> skipped: SHOT is binary, use SerialTest --shot"); continue; }
                String reply = ask(p, cmd, 3000);
                if (reply == null && i == 0) { Thread.sleep(1500); reply = ask(p, cmd, 3000); }   // the first command may race a reset
                if (reply == null) { missed++; System.out.println(cmd + " -> (no reply)"); }
                else System.out.println(reply);
            }
        } finally {
            p.closePort();
        }
        System.exit(missed == 0 ? 0 : 1);
    }

    /** Sends a command and returns the first reply line that starts with OK or ERR; log lines in between are skipped. */
    static String ask(SerialPort p, String cmd, long timeoutMs) throws InterruptedException {
        drain(p);
        byte[] out = (cmd + "\n").getBytes(StandardCharsets.US_ASCII);
        p.writeBytes(out, out.length);
        StringBuilder line = new StringBuilder();
        byte[] one = new byte[1];
        long end = System.currentTimeMillis() + timeoutMs;
        while (System.currentTimeMillis() < end) {
            if (p.bytesAvailable() > 0 && p.readBytes(one, 1) == 1) {
                char c = (char) (one[0] & 0xff);
                if (c == '\n') {
                    String s = line.toString().trim(); line.setLength(0);
                    if (s.startsWith("OK") || s.startsWith("ERR")) return s;
                } else if (c != '\r') line.append(c);
            } else Thread.sleep(2);
        }
        return null;
    }

    static void drain(SerialPort p) throws InterruptedException {
        Thread.sleep(50);
        byte[] b = new byte[4096];
        while (p.bytesAvailable() > 0) p.readBytes(b, Math.min(b.length, p.bytesAvailable()));
    }
}
