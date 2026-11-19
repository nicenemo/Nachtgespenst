# Changelog

All notable changes to Nachtgespenst. The project has no tagged releases yet, so entries are grouped by milestone and dated by the commits they come from. Format loosely follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## Unreleased (2026-11-19): LampArray, identity and settings

### Added
- **HID LampArray** (usage page 0x59) on both boards, over USB and Bluetooth: lamps 0 to 9 are the LED bar, lamps 10 and up a screen grid of up to 320 x 190 lamps in PSRAM; two test pages (screen lamps, bar lamps). Measured in `docs/wiki/lamparray-testing.md` and on the site ([ADR 0010](docs/adr/0010-lamparray-lighting-and-picture-pages.md)).
- **`tools/LampTest.java`** (info, enum, bar, screen, rate, speed, interference) and the `LAMPS*` serial commands; the CoreS3 delivers 1,000 reports and up to about 810,000 lamps per second over USB, both boards about 30 reports per second over BlueZ with either laptop controller.
- **Persistent settings** in NVS (`SET`, `SET?`): LED mode off, locks or on, bar brightness, LED idle time, backlight, dim and off times, USB/BLE preference. `LEDS OFF` also cuts the 5 V rail of the base. `WAKE`, `BL` and `LEDS` serial commands.
- **System Control report** (Power Down, Sleep, Wake Up) and tiles for it; the firmware refuses Power Down and Sleep over the serial port, so tests cannot suspend the host.
- **Device identity:** pid.codes test ID 1209:0001, Bluetooth PnP ID, short names NG-S3 and NG-C2 on USB, and a moon and ghost emoji in the Bluetooth names (`...-c2`, `...-cs3`).
- **Lamp bar worker task** on the second core with a FastLED mutex (`LAMPS WORKER ON|OFF`) and latency counters in `LAMPS?`: a lamp frame is on the bar 1.4 to 3 ms after its last report (6 ms and up to 40 ms from the main loop). `LampTest latency` and `LampTest ingest` (intake without drawing: 1.5 to 1.9 million lamps per second).
- **Disk demo** (page "Disk", [wiki](docs/wiki/disk-drive.md), [ADR 0017](docs/adr/0017-disk-drive-data-channels.md)): a file from the PC over the lamps (blocks with CRC and byte offsets, acknowledged with F13 to F24), unpacked and written to the SD card of the CoreS3, and sent back over a new game pad with 384 buttons; credits through the lamps as pushback, variable bit rate, SHA-256 on both ends shown on the board with the speeds, modem-like sounds and a fast mode. 28.6 kB/s in, 37 kB/s back over USB.
- **Demo window** `tools/DemoApp.java` (Swing, FlatLaf, virtual threads, the ghost of the site) and `tools/Demo.java`: the cycle or any part: pages, the file transfer, LED bar effects, C64 raster bars and sine scroller, and a photo slideshow with prime-modulo dissolves (five Wikimedia Commons photos). The shared code of `LampTest` moved into classes (`Hid`, `Ser`, `Evdev`, `Proto`, `DiskDrive`, `Canvas`, ...).
- **Camera tests:** the Locks page colours (all seven states on the Core2 on a sheet of white paper, now on the site), a `dark` test for `LEDS OFF`, and the bar of the other board is switched off during a test.
- **Site:** the page split into pages with a hamburger menu, a LampArray page, a banner with the moon and ghost.

### Changed
- The USB HID descriptor has a game pad collection; the PnP versions are 0x0103 and 0x0203, so Bluetooth pairing has to be renewed once.
- Page changes are coalesced (25 ms) and the firmware builds with `-O2`: a page switch takes about 320 ms on the CoreS3 (was 730) and 380 ms on the Core2 (was 840).
- `SerialTest` has 43 checks on the CoreS3 and 25 on the Core2 (settings, System Control, serial guards).
- The GitHub repository is `nicenemo/nachtgespenst`; `tools/Flash.java` also finds the bootloader IDs.

### Fixed
- Lowering BlueZ's connection interval (kos ADR 0015) did not raise the Bluetooth report rate (still about 30 per second): documented as a negative result.
- A burst of lamp reports no longer fills the host queue in the tests; counters no longer count the control report as an update; the System Control check reads the keyboard node where Linux folds it in.

## 2026-11-17 to 2026-11-18

### Added
- **CoreS3 LED bar.** The M5GO base bar on the CoreS3 is driven from GPIO5 (M-Bus pin 8), like GPIO25 on the Core2. Found with a camera-driven pin sweep and confirmed on all 26 layer colours.
- **Selectable USB/BLE routing.** `HIDManager::Prefer` with `USB` (default), `BLE`, `BOTH`, `USB_ONLY` and `BLE_ONLY`, plus the serial commands `PREFER USB|BLE|BOTH`, `ONLY USB|BLE` and `PREFER?`.
- **`tools/LedCam.java`.** JBang tool that films the LED bars with the laptop webcam: `pins` finds the CoreS3 bar pin, `colours` cycles every layer and writes a contact sheet. Frame differencing uses the Vector API.
- **`tools/ledtest` CoreS3 pin finder** (`env:cores3`, `main_s3.cpp`) that steps through every M-Bus and Grove pin.
- **Project site** (`site/index.html`, published with GitHub Pages) with screenshots of every page on both boards, the camera test results and the project setup.
- Architecture decision records for the Claude Code sandbox, the project-local PlatformIO and the pinned JDK; a tooling wiki; the sandbox test plan; `tools/FetchHut.jsh` to download the USB HID Usage Tables.
- This changelog.
- **Secret scanning.** Tracked `pre-commit` and `pre-push` hooks that run trufflehog, with ADR 0016.
- **Board tools in JBang:** `tools/Flash.java` (find the board by USB ID, CoreS3 1200-baud touch, flash, wait for the board) and `tools/Serial.java` (send text commands), with the shared `tools/Boards.java`; `SerialTest --latency` measures how long a page takes to reach the display. They replace first versions written as bash scripts ([ADR 0008](adr/0008-jbang-for-project-tooling.md)).
- **`PERF?`** serial command (LVGL builds) with the build, render and flush time of the last page, and a fast-paging burst test in `SerialTest --latency`.
- **Knowledge for Claude Code:** `.claude/rules`, `.claude/skills` (`flash-and-test`, `camera-led-test`, `publish-site`), `.claude/agents` (`hardware-tester`, `firmware-builder`), wiki pages (`claude-code-workflow`, `hardware-testing`, `troubleshooting`) and ADRs 0004 to 0007.

### Changed
- `platformio.ini` sets `core_dir` to the project-local `.pio/core`, so `pio` no longer needs a `PLATFORMIO_CORE_DIR` prefix (an env prefix kept commands inside the sandbox).
- `SerialTest` finds the USB and the Bluetooth input device of a board separately (by `Phys`, not by name) and checks every routing mode against both.
- All documentation is in British English; the Dutch code comments and serial log strings were translated to English.
- `GenerateHeaders.jsh` runs on Java 27 and no longer downloads the specification itself (that moved to `FetchHut.jsh`).
- `CLAUDE.md` and `docs/TODO.md` describe the S3 flashing procedure, the sandbox pitfalls and the camera test set-up.

### Fixed
- Test failures caused by the test reading the wrong host input device on the S3.
- Serial commands, flashing and the camera tools no longer silently run inside the sandbox when called as a single plain command (documented in `docs/TODO.md`).

## 2026-11-13: hardening after a code review
- Non-blocking press queue with release retry, USB "mounted" check, BLE security without MITM, atomic LED state, a loop that sleeps instead of spinning, and a wake-up tap that no longer presses a tile.
- Open points parked in `docs/TODO.md`.

## 2026-11-12: LVGL interface, CoreS3, USB and a test harness
The first day of working with Claude Code (see `CLAUDE.md` and `docs/OVERNIGHT.md`).
- **User interface rewritten with LVGL 9:** tile grid per layer, icon font, themes, paging buttons, a Locks page for Caps/Num/Scroll with host LED feedback, and backlight dimming after 60 s of idle (off after 5 min).
- **CoreS3 port** with its own PlatformIO environment; USB HID through TinyUSB, running side by side with BLE.
- **LED bar code** for the Core2 (layer colour, lock states).
- **Serial line protocol and test harness** (`PING`, `INFO`, `STATE?`, `LAYER`, `TAP`, `KEY`, `SHOT`) with `tools/SerialTest.java`; the screenshots on the project site come from the `SHOT` command.
- Device identity per board (`Nachtgespenst-C2`, `Nachtgespenst-S3`), `Config.h`, `CLAUDE.md` and the overnight plan.

## 2026-05-13 to 2026-05-14: generated HID vocabulary
- HID code generation from the USB HID Usage Tables 1.7: the specification is downloaded and extracted to JSON, FreeMarker templates render per-page usage constants and shortcut macros.
- ZMK-style keymap with chord helpers (`lcs(...)`, `lhyp(...)`, `lmeh(...)`).
- Several template clean-ups.

## 2026-05-10 to 2026-05-12: first commits in this repository
- Initial Core2 macro pad: BLE keyboard, pairing improvements, LED bar on the Core2 base.
- Layers and grids, more keys, keyboard fixes and a lot of keymap experimentation (including a few broken keymaps along the way).

The two May 2026 sections are dated by the commits in this repository, whose history starts on 2026-05-10. The firmware itself is older: an earlier version was made about a year before Claude Code joined, with Gemini in its chat client (Android app and browser) and a little GitHub Copilot. The keymap style and the header generator come from the owner's ZMK work, see [docs/wiki/zmk-comparison.md](docs/wiki/zmk-comparison.md).
