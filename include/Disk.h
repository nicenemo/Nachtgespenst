#pragma once
#include <Arduino.h>

// Disk: the board as a disk drive for a file sent over the HID LampArray (demo page "Disk").
//
// The host writes data blocks into the lamps with ordinary multi updates (8 lamps = 32 bytes per report, the four bytes of a lamp
// are r, g, b and intensity) and marks the end of a block with the "update complete" flag, which is the clock edge. The board
// checks the block and answers with key presses, like the status lines of a drive: F13..F20 acknowledge (the key is the block
// number modulo 8), F21 asks for a resend, F23 and F24 end the session (file saved, or failed). The finished file is unpacked
// and written to the SD card when there is one. Mode "show" draws the progress and plays modem-like sounds, mode "fast" does
// neither. The way back (LOAD) sends the file as game pad input reports (HID report 11: 384 buttons = 48 bytes of data lines), the
// host answers with credits written into the lamps (flow control and a resend request) and with its own SHA-256 of the file, which
// the demo screen shows next to the one the board computed. See docs/wiki/disk-drive.md for the formats and the adaptive bit rate.
namespace Disk {
    constexpr uint16_t DATA_FIRST_LAMP = 10;     // block lamps follow the ten bar lamps
    constexpr uint16_t MAX_BLOCK_LAMPS = 1024;   // 4 kB per block, the largest block the sender may use
    constexpr size_t   MAX_FILE = 512 * 1024;    // compressed bytes the receive buffer holds

    enum class State : uint8_t { Idle, Receiving, Verifying, DoneOk, DoneBad };

    void begin();                // allocate the buffers (PSRAM)
    void enter();                // the Disk page is shown: make room for a full block, mount the SD card and start listening
    void leave();                // another page is shown: stop listening (the received text stays)
    void reset();                // forget the session and the text
    void onFrame();              // HID task: a frame (one block) is complete
    void loop();                 // main loop: acknowledgements as key presses, sounds, unpacking and saving after the last block
    bool active();
    bool quiet();                // a fast transfer is running (either way): the UI must not draw and nothing may make a sound
    void setSound(bool on);

    enum class Tx : uint8_t { Idle, Header, Data, DoneOk, Failed };   // the way back (LOAD): the file leaves as game pad reports

    struct View {
        State state; uint32_t blocksOk, blocksBad, blocksDup, bytes, total, original;
        uint32_t kbps10;         // received payload rate, tenths of kB/s, over the session so far
        uint16_t lastBlockBytes; // payload size of the last block (the sender's current block size)
        uint32_t lines;
        bool crcOk, fast, sdPresent, saved;
        uint32_t sdMB, sdUsedMB;
        char name[32];
        Tx tx; uint32_t txBytes, txTotal, txKbps10;                 // LOAD progress and the speed the board measured while sending
        char sha[65], pcSha[65];                                   // SHA-256 of the document as the board computed it, and as the PC sent it
        int8_t match;                                              // -1 no PC hash yet, 0 different, 1 equal
        uint32_t shaUs;                                            // how long the SHA-256 of the document took on the board, microseconds
        uint8_t pcWhich; uint32_t pcRate;                          // which hash the PC sent (0 original, 1 what it received back) and its speed in bytes/s
    };
    View view();
    const char* excerpt();       // the first lines of the decoded text (empty until a transfer is done)
    String status();             // "DISK state=... " for the serial command DISK?

    bool sdMount();              // true when a card is in and readable (tries again on every call until it works)
    String sdStatus();           // "SD present=1 type=SDHC size_mb=.. used_mb=.." for the serial command SD?
    String dir();                // root directory of the card, one "name size" line per file (at most 8)
    bool helpSection(const char* name, Print& out);   // the section "== name" of /help.txt on the card (the manual of the shell); false without a card or section
}
