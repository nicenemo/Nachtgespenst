# Disk drive demo

The board as a disk drive for a file that comes from the PC and goes back to it, without a driver or a serial cable. The page is called **Disk** and sits after the Locks page. The PC sends with `LampTest disk` (`tools/LampTest.java`).

The idea is a fast loader with many wires: the lamps of the HID LampArray are the data lines towards the board, the keys F13 to F24 are its status lines, and a game pad with 384 buttons is the data line back to the PC.

## The way in (SAVE): lamps as data lines

The PC writes a block into the lamps 10 and up with ordinary multi updates (8 lamps = 32 bytes per report; the four bytes of a lamp are r, g, b and intensity) and marks the end with the "update complete" flag, which is the clock edge. The board checks the block and writes the result on its status lines:

| Key | Meaning |
|---|---|
| F13 to F20 | block acknowledged; the key is the block number modulo 8 (cumulative) |
| F21 | resend from the last acknowledged block (CRC error or a missing block) |
| F23 / F24 | the file is saved / the transfer failed |

All twelve keys F13 to F24 reach the host as their own keys, over USB and over Bluetooth (evdev codes 183 to 194; `SerialTest` checks this). The sender reads them from the keyboard device of the board and grabs the device (`EVIOCGRAB`), so the desktop never sees them; no root rights are needed.

Block format (little endian, in lamp bytes): sequence number (2), flags (1: last, 2 acknowledge wanted, 4 first), session (1), payload length (2), two parameter bytes, the payload, a CRC-32 over everything before it, padded to whole lamps. Every data block starts with the byte offset of its data in the file, so a resend may cut the file into other blocks than before (the sender changes the block size after errors) and the board still appends exactly the part it does not have yet. A repeated block is acknowledged without being stored twice, and a repeated block after the end of the session makes the board say F23 or F24 again. Block 0 carries a header: `NGD1`, compressed size, original size, CRC-32 of the compressed data, the method (raw deflate), the mode (show or fast) and the file name. The file is compressed by the PC (raw deflate, about 232 kB for the 619 kB text) and unpacked by the board with the inflate routine in the ROM of the ESP32. With an SD card in the board (`SD?`) the unpacked file is written to it and read back to check the size; without a card it stays in RAM.

## The way back (LOAD): the game pad as data lines

The board has a second HID collection, a **game pad with 384 buttons** (report ID 11, 48 bytes). A button is a bit; a host that reads the raw HID node (hidraw) sees all of them, evdev only maps the first. One report carries: offset (4), flags (1: last, 2 header), length (1), up to 38 bytes of file data and a CRC-32 of the first 44 bytes. Over USB the report goes out every millisecond, over Bluetooth as a notification.

**The lamps are the pushback.** The PC asks for the file with a request block (the file name). The board answers with a header report (size and its SHA-256) and then only sends within the window the PC grants: every *credit* is a lamp frame with the next offset the PC wants, the payload size per report, the window in reports and a gap. A resend request makes the board go back to that offset; silence from the PC makes it go back by itself after 600 ms.

## Why 384 buttons and 1,024 lamps

**384 buttons = 48 bytes per game pad report.**
- One USB report must fit one 64-byte full-speed packet: the report ID plus 48 data bytes make 49. The same report fits a Bluetooth notification (ATT MTU 247 leaves 244 bytes), so USB and Bluetooth share one descriptor and one report size.
- The overhead is fixed: 4 bytes offset, 1 flags, 1 length and a 4-byte CRC-32 leave **38 bytes of data, 79%**. With the 128 buttons that DirectInput knows (16 bytes) only 6 bytes would be data (38%); with 504 buttons (the USB maximum of 63 bytes) it would be 53 of 63 bytes (84%), a small gain for a report that no longer fits comfortably in a Bluetooth packet with headroom.
- A host that reads the raw HID node sees every bit; evdev and DirectInput only map the first buttons. This is a data channel for a tool, not a game controller, so the mapping does not matter.

**1,024 data lamps = 4 kB per block.**
- Ten lamps are the LED bar, so the data lamps are ids 10 to 1,033 (a 32 x 32 grid, set when the page opens). The same lamp store serves the lamp test pages (up to 60,800 screen lamps); the LampArray limit is 65,535 lamps, because a lamp id is 16 bits.
- The cost of a block is 12 bytes (header and CRC), 0.3% of 4 kB, so larger blocks hardly save overhead. The speed is set by the number of reports (8 lamps = 32 bytes each, about 1,000 per second over USB), not by the block size. A CRC error costs a whole block, so large blocks make resends expensive on a lossy link: the sender cuts the block back to as little as 16 lamps after errors and opens it up again after clean stretches.
- The board could hold the whole compressed book (232,700 bytes) in one block (the full 60,800-lamp grid is 243 kB), but that would only trade speed for a very long resend.

## Variable bit rate

The sender starts cautious and speeds up after clean stretches; every error cuts it back.
- SAVE: the block size (16 to 1,024 lamps), the number of blocks per acknowledgement and the gap between reports. Three clean windows: larger blocks, then a larger window, then a smaller gap. A NAK or a missing acknowledgement halves the block and the window and doubles the gap.
- LOAD: the payload per report (4 to 38 bytes), the window and the gap, steered by the credits. After 300 clean reports it opens up; a CRC error or a gap in the offsets cuts it back.
`--inject N` drops N per cent of the lamp reports on purpose to show the backing off.

## Sound and the fast mode

In the normal mode the page shows the progress and the board plays modem sounds: an answer tone, two carriers and a rising squeal at the start, a warble per block whose pitch comes from the block's CRC, a low buzz for every error and a chime or a falling buzz at the end. `--fast` sends a header flag that switches the display, the LED bar and the sound off for the duration of the transfer; the screen shows one line and afterwards the result. `DISK SOUND ON|OFF` mutes the board.

## The demo screen: hashes and speed

After the file is saved the board computes the **SHA-256** of the document and shows it. The PC computes the same hash and sends it back through the lamps (a hash block), the screen shows both and says MATCH or DIFFERENT. After the LOAD the PC sends the hash of what it received. The screen also shows the speeds: the save speed measured by the board, the load speed measured by the board while sending, and the speed the PC measured.

## Testing

```
jbang tools/LampTest.java disk --board S3 --via usb              # SAVE then LOAD, with sound and display
jbang tools/LampTest.java disk --board S3 --via usb --fast       # no display, no sound
jbang tools/LampTest.java disk --board S3 --via ble --inject 3   # lossy: watch the bit rate step back
jbang tools/LampTest.java disk --board C2 --via ble --no-load    # SAVE only
```
The book is *Twenty Thousand Leagues under the Sea* (Project Gutenberg #164, public domain), downloaded once to `tools/data/pg164.txt` (not committed). The test checks: the board answers, the file is saved and decoded, the board's SHA-256 equals the original's, the board shows a match with the PC's hash, the file comes back over the game pad byte for byte (SHA-256 equal to the original), the board's own speed measurement is present, and with an SD card the file was written.

Over the Bluetooth link the game pad descriptor means a new pairing is needed after the firmware update (remove the board in `bluetoothctl` and pair again).

## Measured (CoreS3, 2026-11-19)

| | Result |
|---|---|
| SAVE over USB (232,700 compressed bytes of the 618,734 byte book) | 7.95 s = **28.6 kB/s** (76 kB/s of text), no errors; the fast mode 7.90 s |
| LOAD over USB (the whole book over the game pad) | 16.4 s = **36.4 to 37 kB/s**, 16,332 reports for 16,282 needed, no errors, no duplicates (the fast mode 36.9 kB/s) |
| SHA-256 of the book on the board | 33 ms (hardware accelerated), equal to the PC's, shown with MATCH |
| SAVE over USB with 1% of the lamp reports dropped on purpose | completes with the same SHA-256 at **13 kB/s** (77 errors, the block size jumps between 64 and 512 lamps) |
| SAVE over Bluetooth | completes with the same SHA-256, but 0.4 kB/s (595 s): BlueZ delivers about 30 reports per second |
| LOAD over Bluetooth | does not work yet: the credits wait in the BlueZ queue and the resend requests pile up |
| SD card | 32 GB SDHC, the file is written and read back |

The USB speeds are the 1,000 reports per second of full-speed USB (32 bytes per lamp report, 38 bytes per game pad report); neither the display nor the sound slows it down, so the fast mode gains about 1%. Two bugs the tests found: `SendReport` with a 1 ms timeout reports false for a report that did go out (half of the game pad data came back twice, fixed with a longer wait), and a resend after an error used other block sizes than before, which only worked once blocks carried their byte offset. The Core2 does not use the SD card: its SPI and SD drivers made the IRAM overflow by 220 bytes.

