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
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

/**
 * Interactive terminal shell and diagnostic runner for Nachtgespenst boards.
 * <pre>
 *   jbang tools/Shell.java S3                    # Open interactive prompt
 *   jbang tools/Shell.java S3 -c "help layer"    # Execute one or more commands
 *   jbang tools/Shell.java S3 install-help       # Upload help.txt to CoreS3 SD card
 * </pre>
 */
public class Shell {

    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            System.err.println("usage: jbang tools/Shell.java S3|C2 [-c \"command\"]... | install-help");
            System.exit(2);
        }

        Boards.Board board = Boards.parse(args[0]);
        Optional<SerialPort> portOpt = Boards.find(board);
        if (portOpt.isEmpty()) {
            System.err.println("No " + board.tag() + " board found on USB");
            System.exit(1);
        }

        SerialPort serialPort = portOpt.get();
        if (!Boards.usable(serialPort)) {
            System.err.println(Boards.SANDBOX_HINT);
            System.exit(3);
        }

        try (Ser ser = new Ser(serialPort)) {
            if (args.length >= 2 && args[1].equals("install-help")) {
                installHelp(ser, board);
                return;
            }

            List<String> commands = new ArrayList<>();
            for (int i = 1; i + 1 < args.length; i++) {
                if (args[i].equals("-c")) {
                    commands.add(args[++i]);
                }
            }

            if (!commands.isEmpty()) {
                for (String cmd : commands) {
                    executeCommand(ser, cmd);
                }
                return;
            }

            System.out.println(board.tag() + " shell: type 'help' for commands, 'exit' or Ctrl-D to leave.");
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

            while (true) {
                System.out.print(board.tag() + "> ");
                System.out.flush();
                String line = reader.readLine();
                if (line == null || line.trim().equalsIgnoreCase("exit")) {
                    break;
                }
                if (!line.isBlank()) {
                    executeCommand(ser, line.trim());
                }
            }
        }
    }

    private static void executeCommand(Ser ser, String cmd) throws Exception {
        if (cmd.startsWith("shot")) {
            System.out.println("shot is binary: use jbang tools/SerialTest.java --shot DIR");
            return;
        }

        List<String> replies = ser.cmdAll(cmd, 3000, 250);
        if (replies.isEmpty()) {
            System.out.println("(no reply)");
        } else {
            replies.forEach(System.out::println);
        }
    }

    /**
     * Uploads {@code docs/shell/commands.txt} to the CoreS3 SD card as {@code help.txt}
     * across the LampArray virtual Disk interface.
     */
    static void installHelp(Ser ser, Boards.Board board) throws Exception {
        if (!board.tag().equals("S3")) {
            System.err.println("Only the CoreS3 board supports an SD card");
            System.exit(2);
        }

        String hidNode = Hid.find("S3", true);
        if (hidNode == null) {
            System.err.println("No USB HID device node found for the CoreS3");
            System.exit(2);
        }

        List<String> sdStatus = ser.cmdAll("sd", 2000, 100);
        if (sdStatus.isEmpty() || !sdStatus.getFirst().contains("present=1")) {
            System.err.println("No SD card present in the board: " + sdStatus);
            System.exit(1);
        }

        try (Hid hid = new Hid(hidNode)) {
            DiskDrive.Options options = new DiskDrive.Options();
            options.via = "usb";
            options.tag = "S3";
            options.name = "help.txt";
            options.file = "docs/shell/commands.txt";
            options.load = false;

            int[] failures = {0};
            new DiskDrive(options, new DiskDrive.Reporter() {
                @Override
                public void check(String name, boolean ok, String detail) {
                    if (!ok) {
                        failures[0]++;
                        System.out.println("FAIL " + name + " " + detail);
                    }
                }

                @Override
                public void info(String msg) {
                    if (!msg.startsWith("  ")) {
                        System.out.println(msg);
                    }
                }
            }).disk(ser, hid);

            ser.cmd("layer 0", 3000);
            if (failures[0] > 0) {
                System.err.println("SD card transfer failed");
                System.exit(1);
            }
        }

        List<String> helpCheck = ser.cmdAll("help layer", 3000, 300);
        helpCheck.forEach(System.out::println);

        boolean hasExtendedText = helpCheck.stream().anyMatch(l -> l.startsWith("reply:"));
        System.out.println(hasExtendedText
                ? "help.txt is installed on the SD card: extended manual verified"
                : "help still displays firmware fallback text (check SD card mount)");

        System.exit(hasExtendedText ? 0 : 1);
    }
}
