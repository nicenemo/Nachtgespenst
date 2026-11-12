# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Nachtgespenst is firmware for an M5Stack Core2 (ESP32) that acts as a Bluetooth LE HID macro pad. The touchscreen shows a grid of keys per "layer"; tapping a tile sends a HID report to the host. PlatformIO + Arduino framework, C++17, with M5Unified, NimBLE-Arduino and FastLED (status LEDs).

## Commands

- Build: `pio run -e core2` / `pio run -e cores3` (~12 min each)
- Upload: `pio run -t upload`
- Serial monitor: `pio device monitor` (115200, esp32 exception decoder)
- No tests or linter are configured.
- Regenerate HID headers: from inside `tools/`, run `jbang GenerateHeaders.jsh` (paths `data`, `include`, `templates` are relative to `tools/`; Java 26 with `--enable-preview`, deps via `//DEPS`). Note the output lands in `tools/include/`, not the top-level `include/`.

## Architecture

**Generated HID vocabulary (the bulk of the repo).** `tools/GenerateHeaders.jsh` reads `tools/data/hut.json` (extracted from the USB HID Usage Tables PDF `hut_spec.pdf` if missing) and renders FreeMarker templates in `tools/templates/*.ftl` into:
- `include/hid_pages/<Page>.h` — per-page namespaces with usage constants (`HID::Consumer::OTHER::MUTE`, etc., grouped by usage kind)
- `include/hid_shortcuts/<Page>.h` — `#ifndef`-guarded macros such as `kp_A`, `cp_MUTE`, each expanding to `Binding(HID::Page::X, HID::X::KIND::NAME)`; prefix is per page (`kp_` keyboard, `cp_` consumer, ...)
- `include/HID_Types.h`, `HID_Metadata.h`, `HID_Shortcuts.h`, `Bindings.h` — top-level aggregates

Do not hand-edit generated files; change the `.ftl` templates or the generator and regenerate. Name cleaning (stripping `Keyboard `/`AL `/`AC ` prefixes, accent removal, `ID_` prefix for numeric names) lives in `Usage.getCleanName()` in the generator and must stay consistent with the names `Keymap.h` uses. The generator writes to `tools/include/` (gitignored); the checked-in `include/` copies are what the firmware builds, so copy over after regenerating (`cp -r tools/include/. include/`).

**Binding model.** `Bindings.h` defines `Binding` (page, usage, modifier bitmask, label, usage kind) plus chord helpers/macros (`lcs(...)`, `lhyp(...)`, `M_L*`/`M_R*` modifier masks, `none`). Keymaps compose these with the shortcut macros, ZMK-style.

**Keymap / UI.** `include/Keymap.h` holds the hand-written `LAYERS[]` array of `GridLayer` (title, inline `Binding` array, rows, cols, LED color from `LAYER_COLORS`). `src/ui.cpp` (LVGL 9, with `Theme`/`ThemeManager` colours and `Icons.cpp` glyphs) renders the current layer as a tile grid plus a Locks page, and drives LED feedback via `LedColors.h`; `src/main.cpp` only wires `M5`, `HIDManager` and `ui_*`; `src/SerialCmd.cpp` is the line protocol used by `tools/SerialTest.java`.

**BLE HID.** `HIDManager` (`src/HIDManager.cpp`) owns NimBLE init and `send(const Binding&)`, which chooses the report type by `Binding.page`. It queues reports (`QUEUE_LENGTH`) and releases keys after `RELEASE_DELAY` via `handleAsyncEvents()`, which must be called from the main loop. Host LED state is surfaced through `setEventListener`.

## Notes

- Comments in some headers (e.g. `HIDManager.h`) are in Dutch.
- `out.txt` files are captured build logs (gitignored), not source.
