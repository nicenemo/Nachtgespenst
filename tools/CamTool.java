///usr/bin/env jbang "$0" "$@" ; exit $?
//SOURCES Cams.java
//SOURCES V4l2.java
//SOURCES ConsoleUi.java
//SOURCES ConsoleTheme.java
//SOURCES Boards.java
//DEPS com.fazecast:jSerialComm:2.11.0
//JAVA 27
//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.nio.file.*;
import java.util.*;
import java.util.List;

/**
 * The cameras of the demo window from the command line, for scripts and tests (no window needed).
 *
 * <pre>
 * jbang tools/CamTool.java list                           the cameras: number, device and name
 * jbang tools/CamTool.java snap OUT.png [CAM...] [--turn N] [--mirror] [--flip] [--wait SECONDS] [--size WxH]
 *                                                         open one or two cameras (number or /dev/videoN; the same one twice is fine),
 *                                                         wait for a frame, write them side by side to OUT.png, release the cameras
 * </pre>
 * A camera that is missing or in use is reported on the line for that camera and the exit code is 1; nothing crashes. Run it as ONE plain command (the cameras are not visible in the sandbox; plain Java through Video4Linux2, no ffmpeg).
 */
public class CamTool {
    public static void main(String[] args) throws Exception {
        if (args.length == 0 || args[0].equals("list")) {
            List<Cams.Device> d = Cams.list();
            if (d.isEmpty()) System.out.println("no cameras found");
            for (int i = 0; i < d.size(); i++) System.out.println(i + "  " + d.get(i).node() + "  " + d.get(i).name());
            return;
        }
        if (!args[0].equals("snap") || args.length < 2) { System.err.println("usage: CamTool.java list | snap OUT.png [CAM...] [--turn N] [--mirror] [--flip] [--wait S] [--size WxH]"); System.exit(2); }
        Path out = Path.of(args[1]); List<String> wants = new ArrayList<>(); int turn = 0; boolean mirror = false, flip = false; double wait = 6; int capW = 640, capH = 360;
        for (int i = 2; i < args.length; i++) switch (args[i]) {
            case "--turn" -> turn = Integer.parseInt(args[++i]) & 3;
            case "--mirror" -> mirror = true;
            case "--flip" -> flip = true;
            case "--wait" -> wait = Double.parseDouble(args[++i]);
            case "--size" -> { String[] wh = args[++i].split("x"); capW = Integer.parseInt(wh[0]); capH = Integer.parseInt(wh[1]); }
            default -> wants.add(args[i]);
        }
        List<Cams.Device> devs = Cams.list();
        if (wants.isEmpty()) wants.add("0");
        List<Cams.Cam> cams = new ArrayList<>(); List<String> names = new ArrayList<>();
        for (String w : wants) {
            String node = w.startsWith("/dev/") ? w : (w.matches("\\d+") && Integer.parseInt(w) < devs.size() ? devs.get(Integer.parseInt(w)).node() : null);
            if (node == null) { System.out.println(w + ": no such camera"); cams.add(null); names.add(w); continue; }
            cams.add(new Cams.Cam(node, capW, capH, () -> { })); names.add(node);
        }
        long end = System.currentTimeMillis() + (long) (wait * 1000);
        boolean ok = true;
        while (System.currentTimeMillis() < end) {
            boolean all = true;
            for (Cams.Cam c : cams) if (c != null && c.frame() == null && c.problem() == null) all = false;
            if (all) break;
            Thread.sleep(100);
        }
        List<BufferedImage> imgs = new ArrayList<>();
        for (int i = 0; i < cams.size(); i++) {
            Cams.Cam c = cams.get(i);
            if (c == null) { ok = false; continue; }
            BufferedImage im = c.frame();
            if (im == null) { System.out.println(names.get(i) + ": " + (c.problem() != null ? c.problem() : "no frame within " + wait + " s")); ok = false; }
            else { System.out.println(names.get(i) + ": " + im.getWidth() + "x" + im.getHeight()); imgs.add(transform(im, turn, mirror, flip)); }
        }
        for (Cams.Cam c : cams) if (c != null) c.close();          // the devices are free again
        if (!imgs.isEmpty()) {
            int w = imgs.stream().mapToInt(BufferedImage::getWidth).sum(), h = imgs.stream().mapToInt(BufferedImage::getHeight).max().getAsInt();
            BufferedImage sheet = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB); Graphics2D g = sheet.createGraphics(); int x = 0;
            for (BufferedImage im : imgs) { g.drawImage(im, x, 0, null); x += im.getWidth(); }
            g.dispose(); ImageIO.write(sheet, "png", out.toFile()); System.out.println("wrote " + out);
        }
        System.exit(ok ? 0 : 1);
    }

    static BufferedImage transform(BufferedImage im, int q, boolean mirror, boolean flip) {
        boolean odd = (q & 1) == 1; int iw = im.getWidth(), ih = im.getHeight(), w = odd ? ih : iw, h = odd ? iw : ih;
        BufferedImage o = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB); Graphics2D g = o.createGraphics();
        AffineTransform at = new AffineTransform(); at.translate(w / 2.0, h / 2.0); if (mirror) at.scale(-1, 1); if (flip) at.scale(1, -1); at.rotate(q * Math.PI / 2); at.translate(-iw / 2.0, -ih / 2.0);
        g.drawImage(im, at, null); g.dispose(); return o;
    }
}
