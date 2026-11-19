///usr/bin/env jbang "$0" "$@" ; exit $?
//JAVA 27

import java.io.*;
import java.nio.file.*;
import java.util.*;

/**
 * Tests the demo window through its control interface (DemoApp --control --headless): the talk, the slides, the camera toggles with the rule that only
 * one video stream runs, the black screen, the toolbar and a screenshot. With --board it also runs a short demo on the real board.
 *
 * <pre>
 * jbang tools/DemoTest.java [--board] [--shot DIR]
 * </pre>
 * Run it as ONE plain command (a camera and the serial port are not visible in the sandbox; without them the camera checks still pass: the panel only says no camera).
 */
public class DemoTest {
    static int pass, fail;
    static BufferedWriter out; static BufferedReader in;

    static String ask(String cmd) throws IOException { out.write(cmd + "\n"); out.flush(); String r = in.readLine(); return r == null ? "ERR no reply" : r; }
    static Map<String, String> kv(String r) {
        Map<String, String> m = new HashMap<>(); java.util.regex.Matcher x = java.util.regex.Pattern.compile("(\\w+)=(\"[^\"]*\"|\\S+)").matcher(r);
        while (x.find()) m.put(x.group(1), x.group(2).replace("\"", "")); return m;
    }
    static void check(String name, boolean ok, String detail) { if (ok) pass++; else fail++; System.out.println((ok ? "PASS " : "FAIL ") + name + (detail.isEmpty() ? "" : "  (" + detail + ")")); }

    public static void main(String[] args) throws Exception {
        boolean board = false; Path shotDir = null;
        for (int i = 0; i < args.length; i++) { if (args[i].equals("--board")) board = true; else if (args[i].equals("--shot")) shotDir = Path.of(args[++i]); }
        if (shotDir != null) Files.createDirectories(shotDir);
        List<String> cmd = new ArrayList<>(List.of("jbang", Path.of("tools/DemoApp.java").toAbsolutePath().toString(), "--control", "--headless"));
        if (!board) cmd.add("--no-board");
        Process p = new ProcessBuilder(cmd).redirectError(ProcessBuilder.Redirect.DISCARD).start();
        out = new BufferedWriter(new OutputStreamWriter(p.getOutputStream())); in = new BufferedReader(new InputStreamReader(p.getInputStream()));

        Map<String, String> s = kv(ask("state"));
        int slides = Integer.parseInt(s.get("slide").split("/")[1]);
        check("state: the first slide is shown, nothing runs", s.get("slide").startsWith("1/") && "stopped".equals(s.get("talk")) && "off".equals(s.get("camera")) && "0".equals(s.get("streams")) && slides >= 10, s.toString());

        check("talk start", ask("talk start").startsWith("OK"), "");
        s = kv(ask("state"));
        check("the talk runs on slide 1", "running".equals(s.get("talk")) && s.get("slide").startsWith("1/"), s.get("slide") + " " + s.get("talk"));
        ask("talk next"); ask("talk next");
        check("next, next: slide 3", kv(ask("state")).get("slide").startsWith("3/"), "");
        ask("talk prev");
        check("prev: slide 2", kv(ask("state")).get("slide").startsWith("2/"), "");
        ask("talk pause");
        check("pause", "1".equals(kv(ask("state")).get("paused")), "");
        ask("talk pause");
        check("resume", "0".equals(kv(ask("state")).get("paused")), "");
        ask("talk goto 5");
        check("goto 5", kv(ask("state")).get("slide").startsWith("5/"), "");
        check("goto beyond the last slide -> ERR", ask("talk goto 999").startsWith("ERR"), "");
        ask("talk stop");
        check("stop", "stopped".equals(kv(ask("state")).get("talk")), "");

        // the black screen and the toolbar
        ask("blank"); check("blank on", "1".equals(kv(ask("state")).get("blank")), ""); ask("blank"); check("blank off", "0".equals(kv(ask("state")).get("blank")), "");
        ask("toolbar"); check("toolbar hidden", "0".equals(kv(ask("state")).get("toolbar")), ""); ask("toolbar"); check("toolbar shown", "1".equals(kv(ask("state")).get("toolbar")), "");

        // cameras: one stream at a time
        ask("camera tripod"); s = kv(ask("state"));
        check("camera tripod: shown, one stream", "tripod".equals(s.get("camera")) && "1".equals(s.get("streams")), s.get("camera") + " streams=" + s.get("streams"));
        ask("camera laptop"); s = kv(ask("state"));
        check("camera laptop replaces the tripod camera: still one stream", "laptop".equals(s.get("camera")) && "1".equals(s.get("streams")), s.get("camera") + " streams=" + s.get("streams"));
        check("turn, mirror and flip of the shown camera", ask("camera turn right").equals("OK") && ask("camera turn left").equals("OK") && ask("camera turn half").equals("OK") && ask("camera mirror").equals("OK") && ask("camera flip").equals("OK"), "");
        ask("camera off"); s = kv(ask("state"));
        check("camera off frees the device: no stream", "off".equals(s.get("camera")) && "0".equals(s.get("streams")), s.toString());
        check("turn without a camera -> ERR", ask("camera turn right").startsWith("ERR"), "");
        check("unknown camera command -> ERR", ask("camera banana").startsWith("ERR"), "");

        // a screenshot of the window
        if (shotDir != null) {
            Path f = shotDir.resolve("demo-window.png"); String r = ask("shot " + f);
            check("shot writes a picture", r.startsWith("OK") && Files.exists(f) && Files.size(f) > 5000, r);
        }

        // the demos need the board
        if (board) {
            s = kv(ask("state"));
            if (!"connected".equals(s.get("board"))) check("board connected", false, s.get("board") + " " + s.get("info"));
            else {
                check("demo bar:nightrider starts", ask("demo bar:nightrider").equals("OK"), "");
                check("it runs", "running".equals(kv(ask("state")).get("demo")), "");
                check("and ends", ask("wait 90").equals("OK"), "");
            }
        } else {
            check("demo without a board -> ERR", ask("demo disk").startsWith("ERR"), "");
        }
        check("unknown command -> ERR", ask("banana").startsWith("ERR"), "");
        ask("quit"); p.waitFor();
        System.out.println("SUMMARY pass=" + pass + " fail=" + fail + (fail == 0 ? " -> PASS" : " -> FAIL"));
        System.exit(fail == 0 ? 0 : 1);
    }
}
