//DEPS com.fasterxml.jackson.core:jackson-databind:2.18.2
//DEPS org.freemarker:freemarker:2.3.33
//DEPS org.apache.pdfbox:pdfbox:2.0.32
//JAVA 26
//JBANG_JSHELL_OPTIONS --enable-preview

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.core.type.TypeReference;
import freemarker.template.*;
import org.apache.pdfbox.pdmodel.*;
import org.apache.pdfbox.pdmodel.common.filespecification.*;
import java.nio.file.*;
import java.io.File;
import java.net.URI;
import java.net.http.*;
import java.util.*;
import java.util.concurrent.*;

// --- 1. GLOBALS ---
// Replace with the URL you provided
final String PDF_URL  = "https://usb.org/sites/default/files/hut1_7.pdf"; 
final Path PDF_PATH   = Path.of("data/hut1_7.pdf");
final Path JSON_PATH  = Path.of("data/hut.json");
final Path DIR_CONST  = Path.of("include/hid_pages");
final Path DIR_META   = Path.of("include/hid_shortcuts");
final Path DIR_ROOT   = Path.of("include");

// --- 2. UTILS ---

void downloadFile(String url, Path dest) throws Exception {
    System.out.println(">>> Downloading: " + url);
    var client = HttpClient.newHttpClient();
    var request = HttpRequest.newBuilder().uri(URI.create(url)).build();
    var response = client.send(request, HttpResponse.BodyHandlers.ofFile(dest));
    if (response.statusCode() != 200) throw new RuntimeException("Download failed: " + response.statusCode());
}

String toPascalCase(String r) {
    if (r == null) return "Unknown";
    return Arrays.stream(r.trim().split("[ /\\-\\(\\)]+"))
        .filter(w -> !w.isBlank())
        .map(w -> w.substring(0, 1).toUpperCase() + w.substring(1).toLowerCase())
        .map(w -> w.replaceAll("[^a-zA-Z0-9]", ""))
        .collect(java.util.stream.Collectors.joining());
}

String toScreamingSnake(String r) {
    if (r == null) return "UNKNOWN";
    var s = Arrays.stream(r.trim().split("[ /\\-\\(\\)]+"))
        .filter(w -> !w.isBlank())
        .map(String::toUpperCase)
        .map(w -> w.replaceAll("[^A-Z0-9]", ""))
        .collect(java.util.stream.Collectors.joining("_"));
    return (s.isEmpty() || Character.isDigit(s.charAt(0))) ? "ID_" + s : s;
}

String getMacroChar(String id) {
    return switch (id) { 
        case "01"->"D"; case "07"->"K"; case "08"->"L"; 
        case "0C"->"C"; case "0E"->"H"; case "59"->"I"; default->"X"; 
    };
}

String getPrefix(String id) {
    return switch (id) { 
        case "01"->"DC"; case "07"->"KC"; case "08"->"LC"; 
        case "0C"->"MC"; case "0E"->"HC"; case "59"->"IC"; default->"UC"; 
    };
}

void render(Map<String, Object> page, String tpl, Path dir, Configuration cfg) throws Exception {
    var idInt = (Integer) page.get("Id");
    var idHex = "%02X".formatted(idInt);
    var originalName = (String) page.get("Name");
    var pascalName = toPascalCase(originalName);

    Files.createDirectories(dir);
    try (var writer = Files.newBufferedWriter(dir.resolve("Page" + idHex + "_" + pascalName + ".h"))) {
        // We pass 'page' RAW. 
        // This allows your templates to use ${usage.Name} (keeping spaces) 
        // instead of my previous snake_case versions.
        cfg.getTemplate(tpl).process(Map.of(
            "page", page, 
            "pageName", originalName, 
            "pageId", idHex, 
            "namespaceName", pascalName,
            "prefix", getPrefix(idHex), 
            "macroName", getMacroChar(idHex)
        ), writer);
    }
}

// --- 3. DOWNLOAD & PDF EXTRACTION ---

if (!Files.exists(JSON_PATH)) {
    if (!Files.exists(PDF_PATH)) {
        Files.createDirectories(PDF_PATH.getParent());
        downloadFile(PDF_URL, PDF_PATH);
    }

    try (PDDocument doc = PDDocument.load(PDF_PATH.toFile())) {
        System.out.println(">>> Extracting HidUsageTables.json from PDF...");
        var catalog = doc.getDocumentCatalog();
        var names = catalog.getNames();
        if (names == null || names.getEmbeddedFiles() == null) throw new RuntimeException("No embedded files in PDF.");
        
        Map<String, PDComplexFileSpecification> efMap = names.getEmbeddedFiles().getNames();
        if (!efMap.containsKey("HidUsageTables.json")) throw new RuntimeException("HidUsageTables.json not found in PDF.");
        
        try (var is = efMap.get("HidUsageTables.json").getEmbeddedFile().createInputStream()) {
            Files.copy(is, JSON_PATH, StandardCopyOption.REPLACE_EXISTING);
        }
    } catch (Exception e) {
        System.err.println("❌ SOURCE ERROR: " + e.getMessage());
        System.exit(1);
    }
}

// --- 4. PARSING ---

List<Map<String, Object>> pages = null;
try {
    System.out.println(">>> Parsing " + JSON_PATH + "...");
    var mapper = new ObjectMapper();
    var jsonRoot = mapper.readTree(JSON_PATH.toFile());
    pages = mapper.convertValue(jsonRoot.get("UsagePages"), new TypeReference<List<Map<String, Object>>>() {});
} catch (Exception e) {
    System.err.println("❌ JSON PARSE ERROR: " + e.getMessage());
    System.exit(1);
}

// --- 5. RENDERING ---

final var cfg = new Configuration(Configuration.VERSION_2_3_33);
cfg.setDirectoryForTemplateLoading(new File("templates"));
cfg.setDefaultEncoding("UTF-8");

System.out.println(">>> Rendering " + pages.size() + " pages in parallel...");
var executor = Executors.newFixedThreadPool(Runtime.getRuntime().availableProcessors());
try {
    List<CompletableFuture<Void>> futures = new ArrayList<>();
    for (var page : pages) {
        futures.add(CompletableFuture.runAsync(() -> {
            try { render(page, "page_definition.ftl", DIR_CONST, cfg); } catch (Exception e) { throw new RuntimeException(e); }
        }, executor));
        futures.add(CompletableFuture.runAsync(() -> {
            try { render(page, "shortcut_definition.ftl", DIR_META, cfg); } catch (Exception e) { throw new RuntimeException(e); }
        }, executor));
    }
    CompletableFuture.allOf(futures.toArray(new CompletableFuture[0])).join();
} finally {
    executor.shutdown();
}

// --- 6. MASTER HEADERS ---

try {
    System.out.println(">>> Finalizing Master Headers...");
    var masterModel = pages.stream()
        .filter(p -> p.get("Name") != null && !((String) p.get("Name")).contains("Reserved"))
        .map(p -> {
            String idHex = "%02X".formatted((Integer) p.get("Id"));
            String name = (String) p.get("Name");
            return Map.of(
                "constantName", toScreamingSnake(name),
                "id", idHex,
                "filename", "Page%s_%s.h".formatted(idHex, toPascalCase(name)),
                "macroChar", getMacroChar(idHex)
            );
        }).toList();

    Files.createDirectories(DIR_ROOT);
    cfg.getTemplate("HID_Types.ftl").process(Map.of("pages", masterModel), Files.newBufferedWriter(DIR_ROOT.resolve("HID_Types.h")));
    cfg.getTemplate("HID_Shortcuts.ftl").process(Map.of("pages", masterModel), Files.newBufferedWriter(DIR_ROOT.resolve("HID_Shortcuts.h")));
} catch (Exception e) {
    System.err.println("❌ MASTER GENERATION ERROR: " + e.getMessage());
    System.exit(1);
}

System.out.println(">>> DONE");
/exit
