///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 27
//SOURCES Boards.java
//SOURCES Libc.java
//SOURCES Hid.java
//SOURCES HidReader.java
//SOURCES Evdev.java
//SOURCES Ser.java
//SOURCES Proto.java
//SOURCES DiskDrive.java
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import com.fazecast.jSerialComm.SerialPort;
import java.io.*;
import java.util.*;

/**
 * The shell of a board (docs/wiki/shell.md).
 * <pre>
 * jbang tools/Shell.java S3                     an interactive shell: type commands, help lists them (exit or Ctrl-D leaves)
 * jbang tools/Shell.java S3 -c "help layer"     one command (-c can be repeated)
 * jbang tools/Shell.java S3 install-help        copy docs/shell/commands.txt to the SD card of the CoreS3 as help.txt, so that help prints the full texts
 * </pre>
 * Run it as ONE plain command (the serial port and, for install-help, /dev/hidraw are needed). "shot" is binary and is not sent.
 */
public class Shell {
    public static void main(String[] args) throws Exception {
        if (args.length < 1) { System.err.println("usage: jbang tools/Shell.java S3|C2 [-c \"command\"]... | install-help"); System.exit(2); }
        Boards.Board b = Boards.parse(args[0]);
        Optional<SerialPort> sp = Boards.find(b);
        if (sp.isEmpty()) { System.err.println("no " + b.tag() + " board found on USB"); System.exit(1); }
        if (!Boards.usable(sp.get())) { System.err.println(Boards.SANDBOX_HINT); System.exit(3); }
        try (Ser s = new Ser(sp.get())) {
            if (args.length >= 2 && args[1].equals("install-help")) { installHelp(s, b); return; }
            List<String> cmds = new ArrayList<>();
            for (int i = 1; i + 1 < args.length; i++) if (args[i].equals("-c")) cmds.add(args[++i]);
            if (!cmds.isEmpty()) { for (String c : cmds) run(s, c); return; }
            System.out.println(b.tag() + " shell: help lists the commands, exit leaves.");
            BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
            while (true) {
                System.out.print(b.tag() + "> "); System.out.flush();
                String line = in.readLine();
                if (line == null || line.trim().equals("exit")) break;
                if (!line.isBlank()) run(s, line.trim());
            }
        }
    }

    static void run(Ser s, String c) throws Exception {
        if (c.startsWith("shot")) { System.out.println("shot is binary: use jbang tools/SerialTest.java --shot DIR"); return; }
        List<String> r = s.cmdAll(c, 3000, 250);
        if (r.isEmpty()) System.out.println("(no reply)"); else r.forEach(System.out::println);
    }

    /** The manual goes to the SD card through the Disk path (compressed over the lamps, written by the board) and is checked with "help". */
    static void installHelp(Ser s, Boards.Board b) throws Exception {
        if (!b.tag().equals("S3")) { System.err.println("only the CoreS3 uses an SD card"); System.exit(2); }
        String node = Hid.find("S3", true);
        if (node == null) { System.err.println("no USB HID node for the CoreS3"); System.exit(2); }
        List<String> sd = s.cmdAll("sd", 2000, 100);
        if (sd.isEmpty() || !sd.get(0).contains("present=1")) { System.err.println("no SD card in the board: " + sd); System.exit(1); }
        try (Hid h = new Hid(node)) {
            DiskDrive.Options o = new DiskDrive.Options();
            o.via = "usb"; o.tag = "S3"; o.name = "help.txt"; o.file = "docs/shell/commands.txt"; o.load = false;
            int[] bad = {0};
            new DiskDrive(o, new DiskDrive.Reporter() {
                public void check(String name, boolean ok, String detail) { if (!ok) { bad[0]++; System.out.println("FAIL " + name + " " + detail); } }
                public void info(String msg) { if (!msg.startsWith("  ")) System.out.println(msg); }
            }).disk(s, h);
            s.cmd("layer 0", 3000);
            if (bad[0] > 0) { System.err.println("the transfer failed"); System.exit(1); }
        }
        List<String> r = s.cmdAll("help layer", 3000, 300);
        r.forEach(System.out::println);
        boolean ok = r.stream().anyMatch(l -> l.startsWith("reply:"));
        System.out.println(ok ? "help.txt is on the card: help prints the full text" : "help still prints the built-in text (is help.txt on the card?)");
        System.exit(ok ? 0 : 1);
    }
}
