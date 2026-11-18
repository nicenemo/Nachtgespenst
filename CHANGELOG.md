# Changelog

All notable changes to Nachtgespenst. The project has no tagged releases yet, so entries are grouped by milestone and dated by the commits they come from. Format loosely follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## Unreleased (2026-11-17 to 2026-11-18)

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
