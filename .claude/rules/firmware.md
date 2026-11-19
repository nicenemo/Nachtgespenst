---
paths:
  - "src/**"
  - "include/**"
  - "platformio.ini"
  - "tools/SerialTest.java"
  - "tools/ledtest/**"
---

# Firmware changes: build, flash, test

- Every firmware change ends with build, flash and `SerialTest` on **both** boards (S3: 48 checks, C2: 30); a compile-only check is not enough because the HID routing and the UI differ per board. Skill: `flash-and-test`.
- Order: build core2 or cores3 in the background (one at a time) -> `jbang tools/Flash.java <env>` -> `jbang tools/Serial.java <S3|C2> "info"` to confirm `fw=` -> `jbang tools/SerialTest.java --board <S3|C2>`.
- Do not edit generated headers (`include/hid_*`, `HID_Types.h`, `HID_Metadata.h`, `HID_Shortcuts.h`, `Bindings.h` parts that are generated): change `tools/templates` or the generator and regenerate.
- `include/ShellHelp.h` is generated: the commands of the shell are described in `docs/shell/commands.txt`; change that file and run `jbang tools/Manual.java` (it also writes `docs/wiki/shell.md` and `site/shell.html`), then put the new `help.txt` on the SD card with `jbang tools/Shell.java S3 install-help`. Commands and arguments are lower case.
- `ui.h` is the contract between `main.cpp`, `SerialCmd.cpp` and the UI implementation (LVGL on `develop`, direct drawing on `feature/classic-ui`). Keep both implementations behind the same API and the same page layout (`SerialTest` taps by coordinates: header 24 px on top on the Core2, 38 px bar at the bottom on the CoreS3, gap 3).
- UI performance is measured, not guessed: `jbang tools/SerialTest.java --latency` before and after. Reference: classic about 92 ms (CoreS3), LVGL about 360 to 700 ms.
- Sources and log strings are English; comments are English. Keep the serial protocol replies (`OK ...`, `ERR ...`) stable: tests and tools parse them.
- A build edit of `platformio.ini` invalidates the whole build (about 12 minutes). Batch such edits.
