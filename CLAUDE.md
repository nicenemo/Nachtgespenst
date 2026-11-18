# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Nachtgespenst is firmware for an M5Stack Core2 (ESP32) that acts as a Bluetooth LE HID macro pad. The touchscreen shows a grid of keys per "layer"; tapping a tile sends a HID report to the host. PlatformIO + Arduino framework, C++17, with M5Unified, NimBLE-Arduino and FastLED (status LEDs).

## Commands

- PlatformIO is project-local: `pio` below means `PLATFORMIO_CORE_DIR=$PWD/.pio/core .pio/venv/bin/pio`. Setup, JBang/SDKMAN and Java bumps: `docs/wiki/tooling.md`; decisions: `docs/adr/`.
- Build: `pio run -e core2` / `pio run -e cores3` (~12 min each)
- Upload: `pio run -e <env> -t upload` (builds first if sources changed). To flash the existing build without building: `pio run -e <env> -t nobuild -t upload` (stale if you edited sources since the last build). Both match the sandbox-excluded `*pio run*upload*` pattern.
- Flash the S3 (TinyUSB, no auto-reset): first `stty -F /dev/ttyACM0 1200; sleep 3` (ROM download mode), then `pio run -e cores3 -t nobuild -t upload --upload-port /dev/ttyACM0`. Core2 resets itself over its CP2104. Details and sandbox pitfalls: `docs/TODO.md` "Bekend / tips".
- Serial monitor: `pio device monitor` (115200, esp32 exception decoder)
- Serial test: `jbang tools/SerialTest.java [--board S3|C2]` (run as a single plain command: any pipe/`cd`/`&&` keeps it sandboxed and hides the serial ports)
- No tests or linter are configured.
- Update the HID spec data (rare): from inside `tools/`, run `jbang FetchHut.jsh` (downloads the HUT PDF into `tools/data/`, extracts `hut.json`).
- Regenerate HID headers: from inside `tools/`, run `jbang GenerateHeaders.jsh` (paths `data`, `include`, `templates` are relative to `tools/`; Java 27 with `--enable-preview`, deps via `//DEPS`). Note the output lands in `tools/include/`, not the top-level `include/`.

## Architecture

**Generated HID vocabulary (the bulk of the repo).** `tools/GenerateHeaders.jsh` reads `tools/data/hut.json` (checked in; produced from the USB HID Usage Tables PDF by `tools/FetchHut.jsh`) and renders FreeMarker templates in `tools/templates/*.ftl` into:
- `include/hid_pages/<Page>.h` — per-page namespaces with usage constants (`HID::Consumer::OTHER::MUTE`, etc., grouped by usage kind)
- `include/hid_shortcuts/<Page>.h` — `#ifndef`-guarded macros such as `kp_A`, `cp_MUTE`, each expanding to `Binding(HID::Page::X, HID::X::KIND::NAME)`; prefix is per page (`kp_` keyboard, `cp_` consumer, ...)
- `include/HID_Types.h`, `HID_Metadata.h`, `HID_Shortcuts.h`, `Bindings.h` — top-level aggregates

Do not hand-edit generated files; change the `.ftl` templates or the generator and regenerate. Name cleaning (stripping `Keyboard `/`AL `/`AC ` prefixes, accent removal, `ID_` prefix for numeric names) lives in `Usage.getCleanName()` in the generator and must stay consistent with the names `Keymap.h` uses. The generator writes to `tools/include/` (gitignored); the checked-in `include/` copies are what the firmware builds, so copy over after regenerating (`cp -r tools/include/. include/`).

**Binding model.** `Bindings.h` defines `Binding` (page, usage, modifier bitmask, label, usage kind) plus chord helpers/macros (`lcs(...)`, `lhyp(...)`, `M_L*`/`M_R*` modifier masks, `none`). Keymaps compose these with the shortcut macros, ZMK-style.

**Keymap / UI.** `include/Keymap.h` holds the hand-written `LAYERS[]` array of `GridLayer` (title, inline `Binding` array, rows, cols, LED colour from `LAYER_COLORS`). `src/ui.cpp` (LVGL 9, with `Theme`/`ThemeManager` colours and `Icons.cpp` glyphs) renders the current layer as a tile grid plus a Locks page, and drives LED feedback via `LedColors.h`; `src/main.cpp` only wires `M5`, `HIDManager` and `ui_*`; `src/SerialCmd.cpp` is the line protocol used by `tools/SerialTest.java` (`PING`, `INFO`, `STATE?`, `LAYER n`, `TAP x y`, `KEY l i`, `SHOT`, `PREFER USB|BLE|BOTH`, `ONLY USB|BLE`, `PREFER?`). `tools/ledtest` is a separate PlatformIO project (Core2 LED sweep, CoreS3 LED pin finder).

**BLE HID.** `HIDManager` (`src/HIDManager.cpp`) owns NimBLE init and `send(const Binding&)`, which chooses the report type by `Binding.page`. It queues reports (`QUEUE_LENGTH`) and releases keys after `RELEASE_DELAY` via `handleAsyncEvents()`, which must be called from the main loop. Host LED state is surfaced through `setEventListener`. On the S3, `HIDManager::Prefer` (USB default, BLE, BOTH, USB_ONLY, BLE_ONLY) decides which transports receive a report. LED bars: Core2 `LED_PIN=25`, CoreS3 `LED_PIN=5` (both M5GO base, 10 pixels).

## Notes

- Documentation (README, `docs/`, this file) is written in British English (colour, licence, behaviour). Identifiers such as `LAYER_COLORS` and file names such as `LICENSE` stay as they are.
- Comments in some headers (e.g. `HIDManager.h`) are in Dutch.
- `out.txt` files are captured build logs (gitignored), not source.
