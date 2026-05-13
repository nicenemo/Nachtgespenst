//DEPS org.apache.pdfbox:pdfbox:2.0.32
//JAVA 26
//JBANG_JSHELL_OPTIONS --enable-preview

import org.apache.pdfbox.pdmodel.*;
import org.apache.pdfbox.pdmodel.common.filespecification.*;
import java.nio.file.*;
import java.io.File;
import java.util.*;

// --- 1. CONFIG ---
final Path PDF_PATH = Path.of("data/hut1_7.pdf");

// --- 2. EXECUTION ---

if (!Files.exists(PDF_PATH)) {
    System.err.println("❌ ERROR: PDF file not found at: " + PDF_PATH.toAbsolutePath());
    System.exit(1);
}

try (PDDocument doc = PDDocument.load(PDF_PATH.toFile())) {
    System.out.println(">>> Inspecting: " + PDF_PATH.getFileName());
    
    var catalog = doc.getDocumentCatalog();
    var names = catalog.getNames();
    
    if (names == null || names.getEmbeddedFiles() == null) {
        System.out.println("ℹ️ No attachments found in this PDF.");
    } else {
        Map<String, PDComplexFileSpecification> efMap = names.getEmbeddedFiles().getNames();
        
        if (efMap == null || efMap.isEmpty()) {
            System.out.println("ℹ️ The embedded files dictionary is empty.");
        } else {
            System.out.println("✅ Found " + efMap.size() + " attachment(s):");
            System.out.println("------------------------------------------");
            
            efMap.forEach((name, spec) -> {
                var ef = spec.getEmbeddedFile();
                String sizeStr = (ef != null) ? (ef.getSize() + " bytes") : "unknown size";
                System.out.printf("- [%s] (%s)%n", name, sizeStr);
            });
        }
    }
} catch (Exception e) {
    System.err.println("❌ ERROR reading PDF: " + e.getMessage());
    System.exit(1);
}

System.out.println("------------------------------------------");
/exit
