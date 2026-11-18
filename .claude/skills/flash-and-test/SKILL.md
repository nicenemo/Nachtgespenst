---
name: flash-and-test
description: Build, flash and test the Nachtgespenst boards (Core2 and CoreS3). Use after any firmware change, when asked to flash, test, screenshot or time the boards, and before claiming that a firmware change works. Encodes the exact command shapes that keep hardware access out of the Claude Code sandbox.
---

# Flash and test the boards

Everything below is a **single plain command** (no pipe, `cd`, `&&`, `;`, `VAR=x` prefix, redirect). Anything else runs inside the sandbox and cannot see the serial ports. Background: `docs/wiki/claude-code-workflow.md`.

## 1. Pick the branch and build it
- `git branch --show-current`: `develop` = LVGL UI, `feature/classic-ui` = direct drawing. Build the branch you mean to flash.
- Start the build in the background, one environment at a time (11 to 13 minutes each):
  `.pio/venv/bin/pio run -e cores3`, then `.pio/venv/bin/pio run -e core2` after it finished.
- Do not edit `platformio.ini`, switch branches or start another `pio` build while one runs.

## 2. Flash
- `tools/flash.sh cores3` or `tools/flash.sh core2`. It prints branch and image age and warns when sources are newer than the image. Use `--dry-run` to see what it would do, `--image FILE` for another build.
- If it says the port is "not visible here", the call was sandboxed: re-issue as a plain command that matches `tools/flash.sh*` in `excludedCommands` (or, if that entry is missing, disable the sandbox for that single call).

## 3. Confirm what is running
- `tools/serial.sh S3 "INFO"` and `tools/serial.sh C2 "INFO"`: `fw=` must match the build time.

## 4. Test
- `jbang tools/SerialTest.java --board S3` (29 checks) and `--board C2` (11 checks), or without `--board` for both. All must pass; the S3 run includes USB and BLE routing against the host input devices.
- Screenshots of all pages: `jbang tools/SerialTest.java --shot DIR` (Core2: about 10 minutes). Create `DIR` in an earlier call.
- Page-switch timing: `jbang tools/SerialTest.java --latency`.

## 5. Leave things tidy
- Boards on layer 0 and on the firmware of the branch you consider current; report which one that is.
- Delete scratch images and logs you created.

## Quick diagnosis
| Message | Meaning |
|---|---|
| `cannot open /dev/tty...`, `no Nachtgespenst board found` | sandboxed call: fix the command shape |
| `Failed to connect to ESP32-S3: No serial data received` | plain `pio upload` on the CoreS3: use `tools/flash.sh` |
| `.sconsign...dblite: No such file` | a config edit or second build during a build |
| evdev check 0 events on the S3 | test read the wrong input device or the board is not connected over that transport |

More: `docs/wiki/troubleshooting.md`.
