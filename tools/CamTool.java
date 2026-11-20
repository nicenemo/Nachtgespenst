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
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

/**
 * Command-line camera capture and testing tool.
 * <p>
 * Allows scripts and automated tests to inspect video capture devices, grab snapshots,
 * and compose side-by-side verification sheets without launching a graphical user interface.
 * </p>
 * <pre>
 *   jbang tools/CamTool.java list
 *   jbang tools/CamTool.java snap output.png [CAM...] [--turn N] [--mirror] [--flip] [--wait S] [--size WxH]
 * </pre>
 */
public class CamTool {

    public static void main(String[] args) throws Exception {
        if (args.length == 0 || args[0].equals("list")) {
            listCameras();
            return;
        }

        if (!args[0].equals("snap") || args.length < 2) {
            System.err.println("usage: CamTool.java list | snap OUT.png [CAM...] [--turn N] [--mirror] [--flip] [--wait S] [--size WxH]");
            System.exit(2);
        }

        Path outputPath = Path.of(args[1]);
        List<String> requestedCams = new ArrayList<>();
        int turn = 0;
        boolean mirror = false;
        boolean flip = false;
        double waitSeconds = 6.0;
        int captureW = 640;
        int captureH = 360;

        for (int i = 2; i < args.length; i++) {
            switch (args[i]) {
                case "--turn" -> turn = Integer.parseInt(args[++i]) & 3;
                case "--mirror" -> mirror = true;
                case "--flip" -> flip = true;
                case "--wait" -> waitSeconds = Double.parseDouble(args[++i]);
                case "--size" -> {
                    String[] wh = args[++i].split("x");
                    captureW = Integer.parseInt(wh[0]);
                    captureH = Integer.parseInt(wh[1]);
                }
                default -> requestedCams.add(args[i]);
            }
        }

        List<Cams.Device> availableDevices = Cams.list();
        if (requestedCams.isEmpty()) {
            requestedCams.add("0"); // Default to first camera
        }

        List<Cams.Cam> activeCams = new ArrayList<>();
        List<String> cameraNames = new ArrayList<>();

        for (String target : requestedCams) {
            String node = null;
            if (target.startsWith("/dev/")) {
                node = target;
            } else if (target.matches("\\d+")) {
                int index = Integer.parseInt(target);
                if (index < availableDevices.size()) {
                    node = availableDevices.get(index).node();
                }
            }

            if (node == null) {
                System.out.println(target + ": no such camera");
                activeCams.add(null);
                cameraNames.add(target);
                continue;
            }

            activeCams.add(new Cams.Cam(node, captureW, captureH, () -> {}));
            cameraNames.add(node);
        }

        // Wait for frames to be delivered
        long deadline = System.currentTimeMillis() + (long) (waitSeconds * 1000.0);
        while (System.currentTimeMillis() < deadline) {
            boolean allReady = true;
            for (Cams.Cam cam : activeCams) {
                if (cam != null && cam.frame() == null && cam.problem() == null) {
                    allReady = false;
                    break;
                }
            }
            if (allReady) {
                break;
            }
            Thread.sleep(100);
        }

        boolean success = true;
        List<BufferedImage> frames = new ArrayList<>();
        for (int i = 0; i < activeCams.size(); i++) {
            Cams.Cam cam = activeCams.get(i);
            String name = cameraNames.get(i);

            if (cam == null) {
                success = false;
                continue;
            }

            BufferedImage frame = cam.frame();
            if (frame == null) {
                String prob = cam.problem() != null ? cam.problem() : "no frame within " + waitSeconds + " s";
                System.out.println(name + ": " + prob);
                success = false;
            } else {
                System.out.println(name + ": " + frame.getWidth() + "x" + frame.getHeight());
                frames.add(transform(frame, turn, mirror, flip));
            }
        }

        // Release hardware resources immediately
        for (Cams.Cam cam : activeCams) {
            if (cam != null) {
                cam.close();
            }
        }

        // Compose side-by-side contact sheet
        if (!frames.isEmpty()) {
            int totalW = frames.stream().mapToInt(BufferedImage::getWidth).sum();
            int maxH = frames.stream().mapToInt(BufferedImage::getHeight).max().orElse(0);

            BufferedImage sheet = new BufferedImage(totalW, maxH, BufferedImage.TYPE_INT_RGB);
            Graphics2D g = sheet.createGraphics();
            int currentX = 0;
            for (BufferedImage img : frames) {
                g.drawImage(img, currentX, 0, null);
                currentX += img.getWidth();
            }
            g.dispose();

            ImageIO.write(sheet, "png", outputPath.toFile());
            System.out.println("wrote " + outputPath);
        }

        System.exit(success ? 0 : 1);
    }

    private static void listCameras() {
        List<Cams.Device> devices = Cams.list();
        if (devices.isEmpty()) {
            System.out.println("no cameras found");
            return;
        }
        for (int i = 0; i < devices.size(); i++) {
            System.out.printf("%d  %-16s  %s%n", i, devices.get(i).node(), devices.get(i).name());
        }
    }

    /**
     * Applies geometric transforms (rotation by quarter turns, mirroring, flipping) to an image.
     */
    static BufferedImage transform(BufferedImage src, int turns, boolean mirror, boolean flip) {
        boolean transposed = ((turns & 1) == 1);
        int srcW = src.getWidth();
        int srcH = src.getHeight();
        int dstW = transposed ? srcH : srcW;
        int dstH = transposed ? srcW : srcH;

        BufferedImage dst = new BufferedImage(dstW, dstH, BufferedImage.TYPE_INT_RGB);
        Graphics2D g = dst.createGraphics();

        AffineTransform transform = new AffineTransform();
        transform.translate(dstW / 2.0, dstH / 2.0);
        if (mirror) transform.scale(-1, 1);
        if (flip) transform.scale(1, -1);
        transform.rotate(turns * Math.PI / 2.0);
        transform.translate(-srcW / 2.0, -srcH / 2.0);

        g.drawImage(src, transform, null);
        g.dispose();
        return dst;
    }
}
