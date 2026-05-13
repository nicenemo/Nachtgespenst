//DEPS com.fasterxml.jackson.core:jackson-databind:2.18.2
//DEPS org.freemarker:freemarker:2.3.33
//DEPS org.apache.pdfbox:pdfbox:2.0.32
//JAVA 26
//JBANG_JSHELL_OPTIONS --enable-preview

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

// --- DATA CLASSES (Restored with full getters and sanitized logic) ---

class Usage {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("Kinds") public List<String> kinds;

    public int getId() { return id; }
    public String getName() { return name; }
    public List<String> getKinds() { return kinds; }

    public String getIdHex() { return String.format("%02X", id); }
    
    public String getCleanName() {
        if (name == null) return "ID_" + id;
        return name.replace("Keyboard ", "")
                   .replace("Keypad ", "")
                   .replace("System ", "SYS_")
                   .replaceAll("[ /\\-\\(\\)\\.]+", "_")
                   .toUpperCase()
                   .replaceAll("^_+|_+$", "");
    }

    public String getPrimaryKind() {
        if (kinds == null || kinds.isEmpty()) return "OTHER";
        return switch(kinds.get(0)) {
            case "CA", "CP", "CL", "LC", "DV", "Sel", "OOC", "MC", "OSC", "RTC" -> kinds.get(0);
            default -> "OTHER";
        };
    }
}

class Page {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("UsageIds") public List<Usage> usages;

    public int getId() { return id; }
    public String getName() { return name; }
    public List<Usage> getUsages() { return usages; }

    public String getIdHex() { return String.format("%04X", id); }

    // FIXED: Strips slashes and spaces to prevent "NoSuchFileException"
    public String getNamespace() { 
        return name.replaceAll("[ /\\-\\(\\)\\.]+", ""); 
    }

    public String getConstantName() { 
        return name.replaceAll("[ /\\-\\(\\)\\.]+", "_").toUpperCase(); 
    }

    public String getFilename() { return getNamespace() + ".h"; }
    
    public String getMacroPrefix() {
        return switch(id) {
            case 0x01 -> "gp"; 
            case 0x07 -> "kp"; 
            case 0x0C -> "cp"; 
            default -> {
                String clean = name.toLowerCase().replaceAll("[^a-z]", "");
                yield (clean.length() >= 2) ? clean.substring(0, 2) : clean;
            }
        };
    }

    public Map<String, List<Usage>> getGroupedUsages() {
        if (usages == null) return Collections.emptyMap();
        return usages.stream().collect(Collectors.groupingBy(Usage::getPrimaryKind));
    }
}

class HutData {
    @JsonProperty("UsagePages") public List<Page> pages;
    public List<Page> getPages() { return pages; }
}

// --- MAIN EXECUTION ---

public void main() throws Exception {
    Path dataDir = Paths.get("data");
    Path includeDir = Paths.get("include");
    Path jsonPath = dataDir.resolve("hut.json");

    // 1. Download and Extract logic
    if (!Files.exists(jsonPath)) {
        Files.createDirectories(dataDir);
        String pdfUrl = "https://www.usb.org/sites/default/files/hut1_7.pdf";
        System.out.println("Downloading HUT 1.7 PDF...");
        
        Path tempPdf = Files.createTempFile("hut_spec", ".pdf");
        try (InputStream in = new URL(pdfUrl).openStream()) {
            Files.copy(in, tempPdf, StandardCopyOption.REPLACE_EXISTING);
        }

        try (PDDocument doc = PDDocument.load(tempPdf.toFile())) {
            PDDocumentNameDictionary names = doc.getDocumentCatalog().getNames();
            Map<String, PDComplexFileSpecification> embeddedFiles = names.getEmbeddedFiles().getNames();
            for (var entry : embeddedFiles.entrySet()) {
                if (entry.getKey().toLowerCase().endsWith(".json")) {
                    try (InputStream is = entry.getValue().getEmbeddedFile().createInputStream()) {
                        Files.copy(is, jsonPath, StandardCopyOption.REPLACE_EXISTING);
                        System.out.println("Extracted: " + entry.getKey());
                        break;
                    }
                }
            }
        } finally { Files.deleteIfExists(tempPdf); }
    }

    // 2. Load Data
    ObjectMapper mapper = new ObjectMapper().configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
    HutData data = mapper.readValue(jsonPath.toFile(), HutData.class);
    
    List<Page> activePages = data.getPages().stream()
        .filter(p -> p.getId() != 0 && p.getName() != null && !p.getName().contains("Reserved"))
        .toList();

    // 3. Setup Templates
    Configuration cfg = new Configuration(Configuration.VERSION_2_3_33);
    cfg.setDirectoryForTemplateLoading(new File("templates"));
    cfg.setDefaultEncoding("UTF-8");

    // 4. Ensure Directories
    Files.createDirectories(includeDir.resolve("hid_pages"));
    Files.createDirectories(includeDir.resolve("hid_shortcuts"));

    // 5. Generate
    for (var page : activePages) {
        Map<String, Object> model = Map.of("page", page);
        
        // Using BufferedWriters ensures we handle the sanitized filenames correctly
        try (Writer writer = Files.newBufferedWriter(includeDir.resolve("hid_pages").resolve(page.getFilename()))) {
            cfg.getTemplate("page_definition.ftl").process(model, writer);
        }
        try (Writer writer = Files.newBufferedWriter(includeDir.resolve("hid_shortcuts").resolve(page.getFilename()))) {
            cfg.getTemplate("shortcut_definition.ftl").process(model, writer);
        }
    }

    Map<String, Object> root = Map.of("pages", activePages);
    for (String t : List.of("HID_Types", "HID_Shortcuts", "HID_Metadata", "Bindings")) {
        try (Writer writer = Files.newBufferedWriter(includeDir.resolve(t + ".h"))) {
            cfg.getTemplate(t + ".ftl").process(root, writer);
        }
    }
    
    System.out.println("Generation complete for HUT 1.7.");
}

main();
