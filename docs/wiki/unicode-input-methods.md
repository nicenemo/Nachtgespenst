# Typing Unicode characters from a HID keyboard

A USB or Bluetooth keyboard can only send key usages, never characters. To type `‽` or `⸻` the host has to turn a key sequence into a code point, and every operating system does that differently. This page is the reference for the proposal in [ADR 0009](../adr/0009-unicode-pages-and-host-os-awareness.md). Sources: the QMK documentation of its Unicode modes and OS detection (read 2026-11-18), the Android documentation of the Android Information Service, and what is visible in the Arduino-ESP32 and NimBLE sources of this project.

## Input methods per host

| Host | Sequence for one code point | Host prerequisite | Limits |
|---|---|---|---|
| Linux (IBus, most GTK and Qt applications) | tap Ctrl+Shift+U, type the hex digits, tap Space (Enter also works; Esc cancels) | IBus (the default on most distributions that ship it); without IBus it works mainly in GTK applications | all code points; some applications, terminals and games ignore it |
| macOS | hold Option, type four hex digits, release Option | the input source "Unicode Hex Input" added in System Settings and selected; it replaces the normal layout while active (it behaves like US QWERTY otherwise) | all code points, above U+FFFF as a UTF-16 surrogate pair (two Option sequences); can disable Option-based shortcuts |
| Windows with WinCompose | tap the Compose key (Right Alt by default), then `u`, the hex digits, Enter | WinCompose installed and running (a third-party tool) | all code points; the mode QMK recommends for Windows |
| Windows with HexNumpad | hold Left Alt, tap Numpad `+`, type the hex digits, release Alt | registry value `EnableHexNumpad` = `1` under `HKCU\Control Panel\Input Method`, then a reboot | only up to U+FFFF (no emoji); not the classic Alt code system (which is code page 1252) |
| Emacs | Ctrl+X, 8, Enter (`insert-char`), then the hex digits and Enter | none | Emacs only |
| Windows, Alt codes | hold Alt, type the decimal code on the numeric keypad (for example 0150 for the en dash, 0151 for the em dash, 0128 for the euro), release Alt | none; NumLock on | Windows-1252 characters only |
| Windows, Word and WordPad | type the hex digits, then Alt+X | none | only inside Word-like editors (the owner's ZMK config has this as `WUC`) |
| ChromeOS | probably Ctrl+Shift+U like Linux | none | believed, not verified |
| macOS, iOS and iPadOS, direct combinations | Option or Option+Shift with a key on the US/ABC layout (for example Option+hyphen for the en dash, Option+Shift+hyphen for the em dash, Option+semicolon for the ellipsis) | none | a subset of characters; believed from the Mac layout and to be verified on iOS and iPadOS, which have no hex input |
| Android | none | n/a | no keyboard-level Unicode method; emoji and symbols come from the on-screen keyboard |

Practical consequences:
- The hex digits are sent as ordinary key usages. Digits are the same on every layout; the letters A to F assume a QWERTY-like layout (on AZERTY the `A` key is in another place).
- Between the keystrokes of a sequence a short delay is needed; QMK defaults to 10 ms. The start and the end of the sequence need the modifier held or the finishing key tapped, so the firmware needs a small sequence engine, not just single reports.
- Every method needs one-time set-up on the host. A macro pad can document it, not change it.
- Characters above U+FFFF (emoji) work on Linux and macOS (surrogate pairs) and with WinCompose, but not with HexNumpad.

The owner's ZMK configuration selects the mode with six tiles on its CONFIG layer: `UC_SET_MACOS`, `UC_SET_WIN_ALT`, `UC_SET_WIN_COMPOSE`, `UC_SET_LINUX`, `UC_SET_LINUX_ALT`, `UC_SET_EMACS`; the proposal mirrors them.

## Detecting the host

| Channel | Signal | Source and status |
|---|---|---|
| USB | request for the Microsoft string descriptor 0xEE, request for the BOS descriptor, sequence and lengths of descriptor requests, HID `SET_IDLE` and `SET_PROTOCOL` | QMK's OS detection (Linux, Windows, macOS, iOS) works from the USB setup behaviour, debounced for 200 to 250 ms; it admits it is a best guess, can be fooled by KVM switches and can report late on Apple Silicon |
| BLE, Android | the Android Information Service: primary service `e73e0001-ef1b-4e74-8291-2e4f3164f3b5`, characteristic `e73e0002-ef1b-4e74-8291-2e4f3164f3b5` holds the API level (4 bytes, little endian) | documented by Android (from Android 14); the keyboard is the GATT client; readable without authentication while the phone is discoverable |
| BLE, Apple | the host's Device Information Service reports the manufacturer string "Apple Inc." | anecdotal (a Nordic developer forum answer), not verified here |
| BLE, Windows and Linux | no known reliable signal; candidates are the negotiated ATT MTU, connection parameters and the order of the GATT reads | to be measured on real hosts |

What this project's stack allows:
- USB (CoreS3, Arduino-ESP32 with TinyUSB): `tud_descriptor_device_cb` and `tud_descriptor_string_cb` are weak symbols in `esp32-hal-tinyusb.c`, `tud_descriptor_bos_cb` and `tud_vendor_control_xfer_cb` are normal functions there, and `tud_hid_set_idle_cb` and `tud_hid_set_protocol_cb` are in `USBHID.cpp`. Requests for descriptors can therefore be observed by overriding the weak symbols or wrapping the others at link time (`-Wl,--wrap=...`). The setup packet's `wLength`, which QMK uses, is not visible through these callbacks.
- BLE (NimBLE-Arduino 2.x): `NimBLEServer::getClient(connInfo)` returns a GATT client for an existing connection, so the keyboard can discover and read the host's services after the host has connected. `onMTUChange` reports the negotiated MTU.
- Only Linux hosts are available for experiments here; Windows, macOS, Android and iOS behaviour has to be measured by connecting the boards to those machines.
