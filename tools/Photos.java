import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URI;
import java.net.http.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.regex.*;
import javax.imageio.ImageIO;

/**
 * Five photos from Wikimedia Commons, chosen to be recognisable and not upsetting: the Blue Marble, Earthrise, the Statue of Liberty and the Pillars of
 * Creation (all public domain) and Deventer seen from across the IJssel (CC BY-SA 4.0, Smiley.toerist: credit it wherever it is shown). They are fetched once (640 pixels wide) into tools/data/photos
 * (not committed) and cut to the grid with a box filter.
 */
final class Photos {
    static final String[][] LIST = {
        {"File:The_Blue_Marble_(remastered).jpg", "The Blue Marble (NASA)"},
        {"File:NASA-Apollo8-Dec24-Earthrise.jpg", "Earthrise (Apollo 8, NASA)"},
        {"File:Deventer_aan_de_IJssel_2024.jpg", "Deventer seen from across the IJssel (Smiley.toerist, CC BY-SA 4.0)"},
        {"File:Statue_of_Liberty_7.jpg", "Statue of Liberty"},
        {"File:Pillars_of_creation_2014_HST_WFC3-UVIS_full-res_denoised.jpg", "Pillars of Creation (Hubble, NASA/ESA)"}};
    static final String UA = "nachtgespenst-demo/1.0 (public domain test images)";
    static final Path DIR = Path.of("tools/data/photos");

    static BufferedImage load(int n) throws Exception {
        Files.createDirectories(DIR);
        Path f = DIR.resolve("photo" + n + ".jpg");
        if (!Files.exists(f)) {
            HttpClient hc = HttpClient.newBuilder().followRedirects(HttpClient.Redirect.NORMAL).build();
            String api = "https://commons.wikimedia.org/w/api.php?action=query&prop=imageinfo&iiprop=url%7Cextmetadata&iiurlwidth=640&format=json&titles="
                    + URI.create("x:" + LIST[n][0].replace(" ", "_")).getRawSchemeSpecificPart().replace("(", "%28").replace(")", "%29");
            String js = hc.send(HttpRequest.newBuilder(URI.create(api)).header("User-Agent", UA).build(), HttpResponse.BodyHandlers.ofString()).body();
            Matcher m = Pattern.compile("\"thumburl\":\"([^\"]+)\"").matcher(js);
            if (!m.find()) throw new IOException("no thumbnail for " + LIST[n][0]);
            String lic = "?"; Matcher l = Pattern.compile("\"LicenseShortName\":\\{\"value\":\"([^\"]+)\"").matcher(js); if (l.find()) lic = l.group(1);
            byte[] img = hc.send(HttpRequest.newBuilder(URI.create(m.group(1).replace("\\/", "/"))).header("User-Agent", UA).build(), HttpResponse.BodyHandlers.ofByteArray()).body();
            Files.write(f, img);
            Files.writeString(DIR.resolve("credits.txt"), LIST[n][1] + " | " + LIST[n][0] + " | " + lic + "\n", StandardCharsets.UTF_8, StandardOpenOption.CREATE, StandardOpenOption.APPEND);
        }
        return ImageIO.read(f.toFile());
    }

    /** The picture cut to the aspect of the grid (centre crop) and reduced with a box filter to cols x rows, as 0xRRGGBB. */
    static int[] toGrid(BufferedImage img, int cols, int rows) {
        double ar = (double) cols / rows, w = img.getWidth(), h = img.getHeight();
        double cw = Math.min(w, h * ar), ch = cw / ar, x0 = (w - cw) / 2, y0 = (h - ch) / 2;
        int[] out = new int[cols * rows];
        for (int y = 0; y < rows; y++) for (int x = 0; x < cols; x++) {
            int xa = (int) (x0 + x * cw / cols), xb = Math.max(xa + 1, (int) (x0 + (x + 1) * cw / cols)), ya = (int) (y0 + y * ch / rows), yb = Math.max(ya + 1, (int) (y0 + (y + 1) * ch / rows));
            long r = 0, g = 0, b = 0, cnt = 0;
            for (int yy = ya; yy < yb && yy < img.getHeight(); yy++) for (int xx = xa; xx < xb && xx < img.getWidth(); xx++) { int p = img.getRGB(xx, yy); r += (p >> 16) & 255; g += (p >> 8) & 255; b += p & 255; cnt++; }
            if (cnt == 0) cnt = 1;
            out[y * cols + x] = ((int) (r / cnt) << 16) | ((int) (g / cnt) << 8) | (int) (b / cnt);
        }
        return out;
    }
}
