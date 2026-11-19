///usr/bin/env jbang "$0" "$@" ; exit $?
//JAVA 17+

import java.io.IOException;
import java.nio.file.*;
import java.util.List;
import java.util.Optional;

/**
 * Shows a full white or full black page on the laptop screen, for the webcam tests (the screen is the lamp).
 *
 * <pre>
 * jbang tools/Light.java white    light the boards for a photo
 * jbang tools/Light.java black    no light from the laptop screen while the LED bars are filmed
 * jbang tools/Light.java off      close the window again
 * </pre>
 * Firefox runs in kiosk mode (full screen, no chrome) with a throw-away profile, so the owner's browser and its tabs are
 * not touched. The pages are {@code tools/lightbox/white.html} and {@code black.html}. Switching between white and black
 * replaces the window. Run it as ONE plain command (it needs the display, which the Claude Code sandbox hides).
 * Exit codes: 0 done, 1 failed, 2 usage.
 */
public class Light {
    static final Path STATE = Path.of(System.getProperty("java.io.tmpdir"), "nachtgespenst-light");

    public static void main(String[] args) throws Exception {
        if (args.length != 1 || !List.of("white", "black", "off").contains(args[0])) {
            System.err.println("usage: jbang tools/Light.java white|black|off");
            System.exit(2);
        }
        Files.createDirectories(STATE);
        closeWindow();
        if (args[0].equals("off")) { System.out.println("closed"); return; }

        Path page = root().resolve("tools/lightbox/" + args[0] + ".html");
        if (!Files.isRegularFile(page)) { System.err.println("missing " + page); System.exit(1); }
        Path profile = STATE.resolve("profile");
        Files.createDirectories(profile);
        Files.writeString(profile.resolve("user.js"), String.join("\n",
                "user_pref(\"browser.shell.checkDefaultBrowser\", false);",
                "user_pref(\"browser.aboutwelcome.enabled\", false);",
                "user_pref(\"datareporting.policy.dataSubmissionPolicyBypassNotification\", true);",
                "user_pref(\"toolkit.telemetry.reportingpolicy.firstRun\", false);",
                "user_pref(\"browser.startup.homepage_override.mstone\", \"ignore\");",
                "user_pref(\"startup.homepage_welcome_url\", \"\");",
                "user_pref(\"browser.sessionstore.resume_from_crash\", false);", ""));

        ProcessBuilder pb = new ProcessBuilder("firefox", "--no-remote", "--new-instance", "--profile", profile.toString(),
                "--kiosk", page.toUri().toString());
        pb.environment().put("MOZ_ENABLE_WAYLAND", "1");
        pb.redirectOutput(ProcessBuilder.Redirect.DISCARD).redirectError(ProcessBuilder.Redirect.DISCARD);
        Process p = pb.start();
        Files.writeString(STATE.resolve("pid"), Long.toString(p.pid()));
        Thread.sleep(4000);                                   // let the window open and the page paint
        if (!p.isAlive()) { System.err.println("firefox exited early (is a display available to this call?)"); System.exit(1); }
        System.out.println(args[0] + " page is up (pid " + p.pid() + ")");
    }

    /** Stops the kiosk window of an earlier call, if there is one. */
    static void closeWindow() throws IOException, InterruptedException {
        Path pidFile = STATE.resolve("pid");
        if (!Files.exists(pidFile)) return;
        try {
            long pid = Long.parseLong(Files.readString(pidFile).trim());
            Optional<ProcessHandle> h = ProcessHandle.of(pid);
            h.ifPresent(ph -> { ph.descendants().forEach(ProcessHandle::destroy); ph.destroy(); });
            for (int i = 0; i < 20 && h.map(ProcessHandle::isAlive).orElse(false); i++) Thread.sleep(200);
            h.ifPresent(ph -> { if (ph.isAlive()) { ph.descendants().forEach(ProcessHandle::destroyForcibly); ph.destroyForcibly(); } });
        } catch (NumberFormatException ignored) { }
        Files.deleteIfExists(pidFile);
    }

    static Path root() {
        for (Path d = Path.of("").toAbsolutePath(); d != null; d = d.getParent())
            if (Files.exists(d.resolve("platformio.ini"))) return d;
        throw new IllegalStateException("run from inside the repository");
    }
}
