//DEPS com.fasterxml.jackson.core:jackson-databind:2.18.2
//DEPS org.freemarker:freemarker:2.3.33
//DEPS org.apache.pdfbox:pdfbox:2.0.32
//JAVA 21+

import com.fasterxml.jackson.databind.*;
import com.fasterxml.jackson.annotation.*;
import org.apache.pdfbox.pdmodel.*;
import org.apache.pdfbox.pdmodel.common.filespecification.*;
import freemarker.template.*;
import java.nio.file.*;
import java.io.*;
import java.net.URL;
import java.util.*;
import java.util.stream.Collectors;

// --- 1. DATA CLASSES (Top-level for JShell compatibility) ---

class Usage {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("Description") public String description;
    @JsonProperty("Kinds") public List<String> kinds;

    public String getName() { return name != null ? name : "UNKNOWN"; }
    public String getIdHex() { return String.format("%02X", id); }
    public String getDescription() { return description; }
    
    public String getCleanName() {
        if (name == null) return "ID_" + id;
        String c = name.replace("Keyboard ", "").replace("Keypad ", "").replace("System ", "SYS_")
                    .replaceAll("[ /\\-\\(\\)\\.]+", "_").toUpperCase().replaceAll("^_+|_+$", "");
        return (c.isEmpty() || Character.isDigit(c.charAt(0))) ? "ID_" + c : c;
    }
    
    public String getPrimaryKind() { return (kinds == null || kinds.isEmpty()) ? "OTHER" : kinds.get(0); }
}

class Page {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("UsageIds") public List<Usage> usages;

    public String getName() { return name; }
    public String getIdHex() { return String.format("%02X", id); }
    public List<Usage> getUsages() { return usages != null ? usages : List.of(); }

    public String getNamespace() {
        if (name == null) return "Page" + getIdHex();
        return Map.of("Generic Desktop", "Desktop", "Keyboard/Keypad", "Keyboard", "LED", "LEDs", "Physical Interface Device", "Haptics")
                  .getOrDefault(name, name.replaceAll("[ /\\-\\(\\)\\.]+", ""));
    }

    public String getConstantName() {
        return (name == null) ? "PAGE_" + id : name.toUpperCase().replaceAll("[ /\\-\\(\\)\\.]+", "_").replaceAll("^_+|_+$", "");
    }

    public String getFilename() { return "Page" + getIdHex() + "_" + getNamespace() + ".h"; }
    
    public String getMacroChar() { 
        return switch(id) { case 1->"D"; case 7->"K"; case 12->"C"; default->"H"+getIdHex(); }; 
    }

    public Map<String, List<Usage>> getGroupedUsages() {
        return getUsages().stream().collect(Collectors.groupingBy(Usage::getPrimaryKind, TreeMap::new, Collectors.toList()));
    }
}

class HutData { @JsonProperty("UsagePages") public List<Page> pages; }

// --- 2. EXECUTION LOGIC ---

void run() throws Exception {
    System.out.println(">>> [START] HID Header Generation (HUT 1.7)");

    Path dataDir = Paths.get("data");
    Path pdfPath = dataDir.resolve("hut1_7.pdf");
    Path jsonPath = dataDir.resolve("hut.json");
    Files.createDirectories(dataDir);

    if (!Files.exists(jsonPath)) {
        System.out.println(">>> [INFO] Extracting JSON from PDF...");
        try (PDDocument doc = PDDocument.load(pdfPath.toFile())) {
            var names = doc.getDocumentCatalog().getNames();
            Map<String, PDComplexFileSpecification> efMap = names.getEmbeddedFiles().getNames();
            for (var entry : efMap.entrySet()) {
                if (entry.getKey().toLowerCase().contains("usage")) {
                    try (InputStream is = entry.getValue().getEmbeddedFile().createInputStream()) {
                        Files.copy(is, jsonPath, StandardCopyOption.REPLACE_EXISTING);
                    }
                    break;
                }
            }
        }
    }

    ObjectMapper mapper = new ObjectMapper().configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
    HutData data = mapper.readValue(jsonPath.toFile(), HutData.class);
    
    List<Page> activePages = data.pages.stream()
        .filter(p -> p.getName() != null && !p.getName().contains("Reserved"))
        .toList();

    Configuration cfg = new Configuration(Configuration.VERSION_2_3_33);
    cfg.setDirectoryForTemplateLoading(new File("templates"));
    cfg.setDefaultEncoding("UTF-8");

    Files.createDirectories(Paths.get("include/hid_pages/"));
    Files.createDirectories(Paths.get("include/hid_shortcuts/"));

    for (var page : activePages) {
        Map<String, Object> model = Map.of("page", page);
        try (var w = Files.newBufferedWriter(Paths.get("include/hid_pages/").resolve(page.getFilename()))) {
            cfg.getTemplate("page_definition.ftl").process(model, w);
        }
        try (var w = Files.newBufferedWriter(Paths.get("include/hid_shortcuts/").resolve(page.getFilename()))) {
            cfg.getTemplate("shortcut_definition.ftl").process(model, w);
        }
    }

    Map<String, Object> root = Map.of("pages", activePages);
    for (String m : List.of("HID_Types", "HID_Metadata", "HID_Shortcuts")) {
        try (var w = Files.newBufferedWriter(Paths.get("include/" + m + ".h"))) {
            cfg.getTemplate(m + ".ftl").process(root, w);
        }
    }
    System.out.println(">>> [SUCCESS] Headers generated.");
}

run();
