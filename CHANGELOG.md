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

### Changed
- **Branch `feature/classic-ui`:** LVGL and the icon font are removed; the screen is drawn directly with plain tiles, text labels and arrow shapes, with a bottom navigation bar on the CoreS3. Same `ui.h` API, so the serial protocol and `SerialTest` are unchanged.
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

## 2026-05-10 to 2026-05-12: first firmware
- Initial Core2 macro pad: BLE keyboard, pairing improvements, LED bar on the Core2 base.
- Layers and grids, more keys, keyboard fixes and a lot of keymap experimentation (including a few broken keymaps along the way).
