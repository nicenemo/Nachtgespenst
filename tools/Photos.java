import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URI;
import java.net.URLEncoder;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Image loader and downsampler for displaying photographs on the lamp matrix.
 * <p>
 * Fetches curated high-contrast photographs of European landmarks from Wikimedia Commons
 * under open licences (CC0, CC BY-SA 3.0 / 4.0), caches them locally in {@code tools/data/photos/},
 * and downsamples them to matrix dimensions (e.g. 15&times;7 or 160&times;95) using an area-averaging
 * box filter.
 * </p>
 */
final class Photos {

    /** Metadata record for a Wikimedia Commons test photograph. */
    record Landmark(String commonsFile, String credit) {}

    /** Curated list of recognizable European landmarks with open licensing. */
    static final List<Landmark> LIST = List.of(
            new Landmark("File:Pont_des_Invalides_and_the_Eiffel_Tower,_Paris_31_August_2019.jpg", "Eiffel Tower and the Pont des Invalides, Paris (CC0)"),
            new Landmark("File:Tower_Bridge_from_Shad_Thames.jpg", "Tower Bridge, London (Colin, CC BY-SA 4.0)"),
            new Landmark("File:Brandenburger_Tor_abends.jpg", "Brandenburg Gate, Berlin (Thomas Wolf, CC BY-SA 3.0)"),
            new Landmark("File:Colosseo_2020.jpg", "Colosseum, Rome (FeaturedPics, CC BY-SA 4.0)"),
            new Landmark("File:Deventer_aan_de_IJssel_2024.jpg", "Deventer seen from across the IJssel (Smiley.toerist, CC BY-SA 4.0)")
    );

    private static final String USER_AGENT = "nachtgespenst-demo/1.0 (public domain test images)";
    private static final Path CACHE_DIR = Path.of("tools/data/photos");

    private Photos() {
        // Non-instantiable utility class
    }

    /**
     * Loads the photograph at index {@code index}, downloading it from Wikimedia Commons
     * on demand if not already cached on disk.
     *
     * @param index photo index (0..4)
     * @return the decoded {@link BufferedImage}
     * @throws Exception if fetching or decoding fails
     */
    static BufferedImage load(int index) throws Exception {
        Files.createDirectories(CACHE_DIR);
        Path cachedFile = CACHE_DIR.resolve("photo" + index + ".jpg");

        if (!Files.exists(cachedFile)) {
            Landmark landmark = LIST.get(index);
            HttpClient client = HttpClient.newBuilder()
                    .followRedirects(HttpClient.Redirect.NORMAL)
                    .build();

            String apiUrl = "https://commons.wikimedia.org/w/api.php?action=query&prop=imageinfo&iiprop=url%7Cextmetadata&iiurlwidth=640&format=json&titles="
                    + URLEncoder.encode(landmark.commonsFile(), StandardCharsets.UTF_8);

            HttpRequest apiReq = HttpRequest.newBuilder(URI.create(apiUrl))
                    .header("User-Agent", USER_AGENT)
                    .build();
            String jsonResponse = client.send(apiReq, HttpResponse.BodyHandlers.ofString()).body();

            Matcher urlMatcher = Pattern.compile("\"thumburl\":\"([^\"]+)\"").matcher(jsonResponse);
            if (!urlMatcher.find()) {
                throw new IOException("No thumbnail URL found for " + landmark.commonsFile());
            }

            String license = "?";
            Matcher licMatcher = Pattern.compile("\"LicenseShortName\":\\{\"value\":\"([^\"]+)\"").matcher(jsonResponse);
            if (licMatcher.find()) {
                license = licMatcher.group(1);
            }

            String thumbUrl = urlMatcher.group(1).replace("\\/", "/");
            HttpRequest imgReq = HttpRequest.newBuilder(URI.create(thumbUrl))
                    .header("User-Agent", USER_AGENT)
                    .build();
            byte[] imageBytes = client.send(imgReq, HttpResponse.BodyHandlers.ofByteArray()).body();

            Files.write(cachedFile, imageBytes);
            String creditEntry = landmark.credit() + " | " + landmark.commonsFile() + " | " + license + "\n";
            Files.writeString(
                    CACHE_DIR.resolve("credits.txt"),
                    creditEntry,
                    StandardCharsets.UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.APPEND
            );
        }

        return ImageIO.read(cachedFile.toFile());
    }

    /**
     * Crops the image to match the target aspect ratio (centre crop) and downsamples
     * it to {@code cols x rows} pixels using an area box filter.
     *
     * @param img  source high-resolution image
     * @param cols target grid column count
     * @param rows target grid row count
     * @return 1D array of length {@code cols * rows} containing 24-bit 0xRRGGBB values
     */
    static int[] toGrid(BufferedImage img, int cols, int rows) {
        double targetAspect = (double) cols / rows;
        double srcWidth = img.getWidth();
        double srcHeight = img.getHeight();

        // Calculate centre crop rectangle
        double cropWidth = Math.min(srcWidth, srcHeight * targetAspect);
        double cropHeight = cropWidth / targetAspect;
        double cropX = (srcWidth - cropWidth) / 2.0;
        double cropY = (srcHeight - cropHeight) / 2.0;

        int[] output = new int[cols * rows];

        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                int startX = (int) (cropX + x * cropWidth / cols);
                int endX = Math.max(startX + 1, (int) (cropX + (x + 1) * cropWidth / cols));
                int startY = (int) (cropY + y * cropHeight / rows);
                int endY = Math.max(startY + 1, (int) (cropY + (y + 1) * cropHeight / rows));

                long sumR = 0;
                long sumG = 0;
                long sumB = 0;
                long pixelCount = 0;

                for (int py = startY; py < endY && py < img.getHeight(); py++) {
                    for (int px = startX; px < endX && px < img.getWidth(); px++) {
                        int pixel = img.getRGB(px, py);
                        sumR += (pixel >> 16) & 0xFF;
                        sumG += (pixel >> 8) & 0xFF;
                        sumB += pixel & 0xFF;
                        pixelCount++;
                    }
                }

                if (pixelCount == 0) {
                    pixelCount = 1;
                }

                int avgR = (int) (sumR / pixelCount);
                int avgG = (int) (sumG / pixelCount);
                int avgB = (int) (sumB / pixelCount);
                output[y * cols + x] = (avgR << 16) | (avgG << 8) | avgB;
            }
        }
        return output;
    }
}
