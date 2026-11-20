///usr/bin/env jbang "$0" "$@" ; exit $?
//JAVA 27

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Automated test harness exercising {@code DemoApp} through its IPC control interface.
 * <p>
 * Spawns {@code DemoApp --control --headless} as a child process and communicates
 * over stdin/stdout pipes to verify:
 * <ul>
 *     <li>Presentation slides navigation, timing, and pause/resume transitions.</li>
 *     <li>Single-camera exclusivity rule (switching cameras guarantees only one active stream).</li>
 *     <li>Blank screen mode and toolbar toggle states.</li>
 *     <li>Off-screen window screenshot capture.</li>
 *     <li>Hardware demo triggers (when {@code --board} is provided).</li>
 * </ul>
 * </p>
 * <pre>
 *   jbang tools/DemoTest.java [--board] [--shot DIR]
 * </pre>
 */
public class DemoTest {

    private static int passCount = 0;
    private static int failCount = 0;
    private static BufferedWriter processIn;
    private static BufferedReader processOut;

    /** Sends a control command line and returns the first response line. */
    static String sendCommand(String cmd) throws IOException {
        processIn.write(cmd + "\n");
        processIn.flush();
        String reply = processOut.readLine();
        return (reply != null) ? reply : "ERR no reply";
    }

    /** Parses a key=value response line into a dictionary map. */
    static Map<String, String> parseKeyValue(String line) {
        Map<String, String> map = new HashMap<>();
        Matcher matcher = Pattern.compile("(\\w+)=(\"[^\"]*\"|\\S+)").matcher(line);
        while (matcher.find()) {
            map.put(matcher.group(1), matcher.group(2).replace("\"", ""));
        }
        return map;
    }

    static void check(String assertionName, boolean conditionMet, String detail) {
        if (conditionMet) {
            passCount++;
        } else {
            failCount++;
        }
        String status = conditionMet ? "PASS " : "FAIL ";
        String suffix = detail.isEmpty() ? "" : "  (" + detail + ")";
        System.out.println(status + assertionName + suffix);
    }

    public static void main(String[] args) throws Exception {
        boolean testRealBoard = false;
        Path screenshotDir = null;

        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--board" -> testRealBoard = true;
                case "--shot" -> screenshotDir = Path.of(args[++i]);
            }
        }

        if (screenshotDir != null) {
            Files.createDirectories(screenshotDir);
        }

        List<String> launchCmd = resolveLaunchCommand(testRealBoard);
        Process process = new ProcessBuilder(launchCmd)
                .redirectError(ProcessBuilder.Redirect.DISCARD)
                .start();

        processIn = new BufferedWriter(new OutputStreamWriter(process.getOutputStream()));
        processOut = new BufferedReader(new InputStreamReader(process.getInputStream()));

        // --- 1. Initial State Checks ---
        Map<String, String> state = parseKeyValue(sendCommand("state"));
        int slideCount = Integer.parseInt(state.get("slide").split("/")[1]);
        check("state: the first slide is shown, nothing runs",
                state.get("slide").startsWith("1/")
                        && "stopped".equals(state.get("talk"))
                        && "off".equals(state.get("camera"))
                        && "0".equals(state.get("streams"))
                        && slideCount >= 10,
                state.toString());

        // --- 2. Talk Sequencer Navigation ---
        check("talk start", sendCommand("talk start").startsWith("OK"), "");
        state = parseKeyValue(sendCommand("state"));
        check("the talk runs on slide 1",
                "running".equals(state.get("talk")) && state.get("slide").startsWith("1/"),
                state.get("slide") + " " + state.get("talk"));

        sendCommand("talk next");
        sendCommand("talk next");
        check("next, next: slide 3", parseKeyValue(sendCommand("state")).get("slide").startsWith("3/"), "");

        sendCommand("talk prev");
        check("prev: slide 2", parseKeyValue(sendCommand("state")).get("slide").startsWith("2/"), "");

        sendCommand("talk pause");
        check("pause", "1".equals(parseKeyValue(sendCommand("state")).get("paused")), "");

        sendCommand("talk pause");
        check("resume", "0".equals(parseKeyValue(sendCommand("state")).get("paused")), "");

        sendCommand("talk goto 5");
        check("goto 5", parseKeyValue(sendCommand("state")).get("slide").startsWith("5/"), "");

        check("goto beyond the last slide -> ERR", sendCommand("talk goto 999").startsWith("ERR"), "");

        sendCommand("talk stop");
        check("stop", "stopped".equals(parseKeyValue(sendCommand("state")).get("talk")), "");

        // --- 3. Screen Blanking & Toolbar Toggles ---
        sendCommand("blank");
        check("blank on", "1".equals(parseKeyValue(sendCommand("state")).get("blank")), "");
        sendCommand("blank");
        check("blank off", "0".equals(parseKeyValue(sendCommand("state")).get("blank")), "");

        sendCommand("toolbar");
        check("toolbar hidden", "0".equals(parseKeyValue(sendCommand("state")).get("toolbar")), "");
        sendCommand("toolbar");
        check("toolbar shown", "1".equals(parseKeyValue(sendCommand("state")).get("toolbar")), "");

        // --- 4. Camera Stream Exclusivity Rules ---
        sendCommand("camera tripod");
        state = parseKeyValue(sendCommand("state"));
        check("camera tripod: shown, one stream",
                "tripod".equals(state.get("camera")) && "1".equals(state.get("streams")),
                state.get("camera") + " streams=" + state.get("streams"));

        sendCommand("camera laptop");
        state = parseKeyValue(sendCommand("state"));
        check("camera laptop replaces tripod: strictly one active stream",
                "laptop".equals(state.get("camera")) && "1".equals(state.get("streams")),
                state.get("camera") + " streams=" + state.get("streams"));

        check("turn, mirror and flip of the shown camera",
                sendCommand("camera turn right").equals("OK")
                        && sendCommand("camera turn left").equals("OK")
                        && sendCommand("camera turn half").equals("OK")
                        && sendCommand("camera mirror").equals("OK")
                        && sendCommand("camera flip").equals("OK"),
                "");

        sendCommand("camera off");
        state = parseKeyValue(sendCommand("state"));
        check("camera off frees device: zero active streams",
                "off".equals(state.get("camera")) && "0".equals(state.get("streams")),
                state.toString());

        check("turn without camera -> ERR", sendCommand("camera turn right").startsWith("ERR"), "");
        check("unknown camera command -> ERR", sendCommand("camera invalid").startsWith("ERR"), "");

        // --- 5. Screenshot Capture ---
        if (screenshotDir != null) {
            Path targetFile = screenshotDir.resolve("demo-window.png");
            String reply = sendCommand("shot " + targetFile);
            check("shot writes picture file",
                    reply.startsWith("OK") && Files.exists(targetFile) && Files.size(targetFile) > 5000,
                    reply);
        }

        // --- 6. Hardware Demos ---
        if (testRealBoard) {
            state = parseKeyValue(sendCommand("state"));
            if (!"connected".equals(state.get("board"))) {
                check("board connected", false, state.get("board") + " " + state.get("info"));
            } else {
                check("demo bar:nightrider starts", sendCommand("demo bar:nightrider").equals("OK"), "");
                check("it runs", "running".equals(parseKeyValue(sendCommand("state")).get("demo")), "");
                check("and ends cleanly", sendCommand("wait 90").equals("OK"), "");
            }
        } else {
            check("demo without board -> ERR", sendCommand("demo disk").startsWith("ERR"), "");
        }

        check("unknown command -> ERR", sendCommand("unknown_cmd").startsWith("ERR"), "");

        sendCommand("quit");
        process.waitFor();

        System.out.printf("SUMMARY pass=%d fail=%d -> %s%n",
                passCount, failCount, (failCount == 0) ? "PASS" : "FAIL");
        System.exit(failCount == 0 ? 0 : 1);
    }

    private static List<String> resolveLaunchCommand(boolean withBoard) {
        String jbangPath = findExecutable("jbang");
        List<String> cmd = new ArrayList<>();

        if (jbangPath != null) {
            cmd.addAll(List.of(
                    jbangPath,
                    Path.of("tools/DemoApp.java").toAbsolutePath().toString(),
                    "--control",
                    "--headless"
            ));
        } else {
            // Direct JVM launch fallback if jbang is not installed in system PATH
            String javaBin = ProcessHandle.current().info().command().orElse("java");
            String classPath = System.getProperty("java.class.path");
            cmd.addAll(List.of(
                    javaBin,
                    "--enable-native-access=ALL-UNNAMED",
                    "--add-modules=jdk.incubator.vector",
                    "-cp", classPath,
                    "DemoApp",
                    "--control",
                    "--headless"
            ));
        }

        if (!withBoard) {
            cmd.add("--no-board");
        }
        return cmd;
    }

    private static String findExecutable(String name) {
        String pathEnv = System.getenv("PATH");
        if (pathEnv == null) return null;
        for (String dir : pathEnv.split(File.pathSeparator)) {
            File f = new File(dir, name);
            if (f.isFile() && f.canExecute()) {
                return f.getAbsolutePath();
            }
        }
        return null;
    }
}
