///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS com.fasterxml.jackson.core:jackson-databind:2.18.2
//DEPS org.freemarker:freemarker:2.3.33
//JAVA 27
//JBANG_JSHELL_OPTIONS --enable-preview

import com.fasterxml.jackson.databind.*;
import com.fasterxml.jackson.annotation.*;
import freemarker.template.*;
import java.nio.file.*;
import java.io.*;
import java.util.*;
import java.util.stream.Collectors;
import java.text.Normalizer;

/**
 * Generates the C++ HID vocabulary headers from the USB HID Usage Tables.
 *
 * <p>Reads {@code data/hut.json} (see {@code FetchHut.jsh} for how to obtain it) and
 * renders the FreeMarker templates in {@code templates/} into {@code include/}:
 * <ul>
 *   <li>{@code include/hid_pages/<Page>.h} - per-page namespaces with usage constants</li>
 *   <li>{@code include/hid_shortcuts/<Page>.h} - shortcut macros such as {@code kp_A}</li>
 *   <li>{@code include/HID_Types.h}, {@code HID_Shortcuts.h}, {@code HID_Metadata.h},
 *       {@code Bindings.h} - top-level aggregates</li>
 * </ul>
 *
 * <p>Run from inside {@code tools/} (all paths are relative to the working directory):
 * <pre>
 *   jbang GenerateHeaders.jsh
 *   cp -r include/. ../include/
 * </pre>
 * The output lands in {@code tools/include/} (gitignored); the firmware builds from the
 * checked-in top-level {@code include/}, so the copy step is required.
 *
 * <p>The naming rules in {@link Usage#computeCleanName()} must stay consistent with the
 * identifiers used in {@code include/Keymap.h}.
 */

// --- DATA CLASSES ---

/** One usage (key, control, ...) within a usage page, as found in {@code hut.json}. */
class Usage {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("Kinds") public List<String> kinds;
    public String uniqueName;

    public int getId() { return id; }
    public String getName() { return name; }
    public List<String> getKinds() { return kinds; }

    /** @return the usage id as two upper-case hex digits, e.g. {@code 04} */
    public String getIdHex() { return String.format("%02X", id); }

    /**
     * Name used in templates: the de-duplicated {@link #uniqueName} once the main
     * routine has assigned one, otherwise the freshly computed clean name.
     *
     * @return a valid C++ identifier fragment
     */
    public String getCleanName() {
        return uniqueName != null ? uniqueName : computeCleanName();
    }

    /**
     * Turns the spec name into an identifier: strips accents, drops the
     * {@code Keyboard }/{@code AL }/{@code AC } prefixes, abbreviates {@code Keypad }
     * and {@code System }, replaces other punctuation with underscores, upper-cases,
     * and prefixes {@code ID_} when the result is empty or starts with a digit.
     *
     * @return the cleaned name, not yet guaranteed unique within its page
     */
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

    /**
     * Maps the first usage type listed in the spec to the short code used for
     * grouping in the page headers (e.g. {@code OOC} for On/Off Control).
     *
     * @return the kind code, or {@code OTHER} when none is listed or it is unknown
     */
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

/** A usage page (keyboard, consumer, ...) and its usages. */
class Page {
    @JsonProperty("Id") public int id;
    @JsonProperty("Name") public String name;
    @JsonProperty("UsageIds") public List<Usage> usages;

    public int getId() { return id; }
    public String getName() { return name; }
    public List<Usage> getUsages() { return usages; }

    public String getIdHex() { return String.format("%04X", id); }

    /** @return the page name stripped of non-alphanumerics, used as the C++ namespace */
    public String getNamespace() {
        return name.replaceAll("[^a-zA-Z0-9]+", "");
    }

    /** @return the page name as an upper-case, underscore-separated constant name */
    public String getConstantName() {
        return name.replaceAll("[^a-zA-Z0-9]+", "_").toUpperCase();
    }

    public String getFilename() { return getNamespace() + ".h"; }
    
    /** Final, collision-free prefix assigned by the main routine; {@code null} before that. */
    public String prefixOverride;

    /** @return the shortcut macro / namespace-alias prefix, e.g. {@code kp} */
    public String getMacroPrefix() {
        if (prefixOverride != null) return prefixOverride;
        return computePrefix();
    }

    /**
     * Default prefix: fixed for the pages used by the keymap ({@code gp} generic desktop,
     * {@code kp} keyboard, {@code cp} consumer), otherwise the first two letters of the
     * page name. Uniqueness is enforced later by the main routine.
     *
     * @return the preferred prefix
     */
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

    /** @return this page's usages grouped by {@link Usage#getPrimaryKind()} */
    public Map<String, List<Usage>> getGroupedUsages() {
        if (usages == null) return Collections.emptyMap();
        return usages.stream().collect(Collectors.groupingBy(Usage::getPrimaryKind));
    }
}

/** Root of {@code hut.json}. Unknown properties are ignored. */
class HutData {
    @JsonProperty("UsagePages") public List<Page> pages;
    public List<Page> getPages() { return pages; }
}

// --- MAIN EXECUTION ---

/**
 * Loads the usage tables, makes names and prefixes unique and C++-safe, then renders
 * the per-page and aggregate headers.
 *
 * @throws Exception if {@code data/hut.json} is missing (run {@code FetchHut.jsh}) or
 *                   a template cannot be read or rendered
 */
public void main() throws Exception {
    Path dataDir = Paths.get("data");
    Path includeDir = Paths.get("include");
    Path jsonPath = dataDir.resolve("hut.json");

    if (!Files.exists(jsonPath)) {
        System.err.println(jsonPath + " not found. Run `jbang FetchHut.jsh` first.");
        System.exit(1);
    }

    Files.createDirectories(includeDir.resolve("hid_pages"));
    Files.createDirectories(includeDir.resolve("hid_shortcuts"));

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
