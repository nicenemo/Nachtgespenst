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
import java.text.Normalizer;

// --- DATA CLASSES ---

class Usage {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("Kinds") public List<String> kinds;
    public String uniqueName;

    public int getId() { return id; }
    public String getName() { return name; }
    public List<String> getKinds() { return kinds; }

    public String getIdHex() { return String.format("%02X", id); }
    
    public String getCleanName() {
        return uniqueName != null ? uniqueName : computeCleanName();
    }

    public String computeCleanName() {
        if (name == null) return "ID_" + id;
        
        // 1. Strip accents (e.g., MOIRÉ -> MOIRE)
        String normalized = Normalizer.normalize(name, Normalizer.Form.NFD);
        normalized = normalized.replaceAll("\\p{M}", "");

        // 2. Clean and Strip prefixes to match Keymap.h expectations
        String clean = normalized.replace("Keyboard ", "")
                   .replace("Keypad ", "KP_")
                   .replace("System ", "SYS_")
                   .replace("AL ", "")  // Strips App Launch
                   .replace("AC ", "")  // Strips App Control
                   .replaceAll("[^a-zA-Z0-9_]+", "_")
                   .toUpperCase()
                   .replaceAll("^_+|_+$", "");

        // 3. Fix numeric identifiers (e.g., "10" -> "ID_10")
        if (clean.isEmpty() || Character.isDigit(clean.charAt(0))) {
            clean = "ID_" + clean;
        }
        
        return clean;
    }

    public String getPrimaryKind() {
        if (kinds == null || kinds.isEmpty()) return "OTHER";
        String k = kinds.get(0);
        return switch(k) {
            case "Application Collection" -> "CA";
            case "Consumer Control"       -> "CP";
            case "Logical Control"        -> "LC";
            case "On/Off Control"         -> "OOC";
            case "Momentary Control"      -> "MC";
            case "One-shot Control"       -> "OSC";
            case "Static Value"           -> "DV";
            case "Selector"               -> "Sel";
            case "Re-trigger Control"     -> "RTC";
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

    public String getNamespace() { 
        return name.replaceAll("[^a-zA-Z0-9]+", ""); 
    }

    public String getConstantName() { 
        return name.replaceAll("[^a-zA-Z0-9]+", "_").toUpperCase();
    }

    public String getFilename() { return getNamespace() + ".h"; }
    
    public String prefixOverride;

    public String getMacroPrefix() {
        if (prefixOverride != null) return prefixOverride;
        return computePrefix();
    }

    public String computePrefix() {
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
    Path pdfPath = dataDir.resolve("hut_spec.pdf");

    Files.createDirectories(dataDir);
    Files.createDirectories(includeDir.resolve("hid_pages"));
    Files.createDirectories(includeDir.resolve("hid_shortcuts"));

    if (!Files.exists(jsonPath)) {
        String pdfUrl = "https://www.usb.org/sites/default/files/hut1_7.pdf";
        if (!Files.exists(pdfPath)) {
            System.out.println("Downloading PDF...");
            try (InputStream in = new URL(pdfUrl).openStream()) {
                Files.copy(in, pdfPath, StandardCopyOption.REPLACE_EXISTING);
            }
        }
        try (PDDocument doc = PDDocument.load(pdfPath.toFile())) {
            PDDocumentNameDictionary names = doc.getDocumentCatalog().getNames();
            Map<String, PDComplexFileSpecification> embeddedFiles = names.getEmbeddedFiles().getNames();
            for (var entry : embeddedFiles.entrySet()) {
                if (entry.getKey().toLowerCase().endsWith(".json")) {
                    try (InputStream is = entry.getValue().getEmbeddedFile().createInputStream()) {
                        Files.copy(is, jsonPath, StandardCopyOption.REPLACE_EXISTING);
                        break;
                    }
                }
            }
        }
    }

    ObjectMapper mapper = new ObjectMapper().configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
    HutData data = mapper.readValue(jsonPath.toFile(), HutData.class);
    
    List<Page> activePages = data.getPages().stream()
        .filter(p -> p.getId() != 0 && p.getName() != null && !p.getName().contains("Reserved"))
        .toList();

    // Names that collide with Arduino/ESP-IDF preprocessor macros or identifiers
    Set<String> reservedNames = Set.of("INPUT", "OUTPUT", "HIGH", "LOW", "DEFAULT", "DISABLED", "PI", "TRUE", "FALSE", "NULL", "EOF", "BIT0", "INPUT_PULLUP", "INPUT_PULLDOWN", "OUTPUT_OPEN_DRAIN");
    Set<String> reservedPrefixes = Set.of("BR", "PI", "IN", "OR", "IF", "DO", "TO", "ON", "OK", "NO", "AT", "LO", "HI");

    // Unique usage names within each page (shortcut macros are flat per page)
    for (var page : activePages) {
        if (page.usages == null) continue;
        Set<String> seen = new HashSet<>();
        for (var u : page.usages) {
            String n = u.computeCleanName();
            if (reservedNames.contains(n)) n = n + "_";
            if (!seen.add(n)) {
                n = n + "_" + u.getIdHex();
                if (!seen.add(n)) n = n + "_" + u.id;
            }
            u.uniqueName = n;
        }
    }

    // Unique, non-reserved namespace-alias/macro prefixes
    Set<String> usedPrefixes = new HashSet<>();
    for (var page : activePages) {
        String full = page.getName().toLowerCase().replaceAll("[^a-z]", "");
        String pre = page.computePrefix();
        int len = pre.length();
        while ((usedPrefixes.contains(pre) || reservedPrefixes.contains(pre.toUpperCase())) && len < full.length()) {
            len++;
            pre = full.substring(0, len);
        }
        usedPrefixes.add(pre);
        page.prefixOverride = pre;
    }

    Configuration cfg = new Configuration(Configuration.VERSION_2_3_33);
    cfg.setDirectoryForTemplateLoading(new File("templates"));
    cfg.setDefaultEncoding("UTF-8");

    for (var page : activePages) {
        Map<String, Object> model = Map.of("page", page);
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
    
    System.out.println("Generation complete.");
}

main();
