///usr/bin/env jbang "$0" "$@" ; exit $?
//DEPS org.apache.pdfbox:pdfbox:2.0.32
//JAVA 27
//JBANG_JSHELL_OPTIONS --enable-preview

import org.apache.pdfbox.pdmodel.*;
import org.apache.pdfbox.pdmodel.common.filespecification.*;
import java.io.*;
import java.net.URI;
import java.nio.file.*;
import java.util.*;

/**
 * Fetches the USB HID Usage Tables specification and extracts its machine-readable
 * usage database.
 *
 * <p>The USB-IF publishes the HID Usage Tables (HUT) as a PDF that carries the full
 * table as an embedded JSON attachment. This script downloads that PDF and pulls the
 * attachment out as {@code data/hut.json}, which {@code GenerateHeaders.jsh} consumes.
 *
 * <p>Run from inside {@code tools/} (all paths are relative to the working directory):
 * <pre>
 *   jbang FetchHut.jsh
 * </pre>
 *
 * <p>The extracted {@code data/hut.json} is checked into git, so this only needs to be
 * run when moving to a newer revision of the specification. The downloaded PDF
 * ({@code data/hut_spec.pdf}) is cached between runs and is gitignored; delete it to
 * force a fresh download. An existing {@code hut.json} is overwritten.
 */

/** Location of the HID Usage Tables PDF on usb.org. Bump when a new revision ships. */
static final String HUT_PDF_URL = "https://www.usb.org/sites/default/files/hut1_7.pdf";

/**
 * Downloads {@link #HUT_PDF_URL} to {@code target}, unless the file already exists.
 *
 * @param target where to store the PDF; parent directories must already exist
 * @throws IOException if the download or the write fails
 */
void downloadPdf(Path target) throws IOException {
    if (Files.exists(target)) {
        System.out.println("Using cached " + target);
        return;
    }
    System.out.println("Downloading " + HUT_PDF_URL + " ...");
    try (InputStream in = URI.create(HUT_PDF_URL).toURL().openStream()) {
        Files.copy(in, target);
    }
}

/**
 * Extracts the first embedded {@code .json} attachment of a PDF.
 *
 * @param pdf    the PDF to read
 * @param target where to write the JSON; an existing file is replaced
 * @throws IOException if the PDF cannot be read, has no embedded files, or contains
 *                     no {@code .json} attachment
 */
void extractJson(Path pdf, Path target) throws IOException {
    try (PDDocument doc = PDDocument.load(pdf.toFile())) {
        PDDocumentNameDictionary names = doc.getDocumentCatalog().getNames();
        if (names == null || names.getEmbeddedFiles() == null) {
            throw new IOException(pdf + " has no embedded files");
        }
        Map<String, PDComplexFileSpecification> embedded = names.getEmbeddedFiles().getNames();
        for (var entry : embedded.entrySet()) {
            if (entry.getKey().toLowerCase().endsWith(".json")) {
                try (InputStream is = entry.getValue().getEmbeddedFile().createInputStream()) {
                    Files.copy(is, target, StandardCopyOption.REPLACE_EXISTING);
                }
                System.out.println("Extracted " + entry.getKey() + " -> " + target);
                return;
            }
        }
        throw new IOException("No .json attachment found in " + pdf);
    }
}

/** Entry point: ensures {@code data/} exists, then downloads and extracts. */
public void main() throws Exception {
    Path dataDir = Paths.get("data");
    Files.createDirectories(dataDir);
    Path pdf = dataDir.resolve("hut_spec.pdf");
    downloadPdf(pdf);
    extractJson(pdf, dataDir.resolve("hut.json"));
}

main();
