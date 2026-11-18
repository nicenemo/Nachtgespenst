# Troubleshooting

Symptom, cause, fix. Every row happened at least once. Background: [claude-code-workflow](claude-code-workflow.md), [hardware-testing](hardware-testing.md).

## Sandbox

| Symptom | Cause | Fix |
|---|---|---|
| `SKIP /dev/ttyACM0: cannot open`, `FAIL no Nachtgespenst board found` | the call ran inside the sandbox | run the tool as a single plain command that matches `excludedCommands` (no `cd`, pipe, `;`, `&&`, `VAR=x`) |
| `Host key verification failed` on `git push` | same: `~/.ssh` is hidden | plain `git push`, no `\| tail` |
| `esptool: could not open port ... No such file or directory` although `ls /sys/class/tty` lists it | same | `jbang tools/Flash.java` (it says so explicitly and exits with code 3) |
| `pio device list` prints nothing | env prefix (`PLATFORMIO_CORE_DIR=...`) kept the call sandboxed | drop the prefix; `core_dir` is set in `platformio.ini` |
| `Failed to connect to system D-Bus` from `bluetoothctl` | sandbox | use an allowed read-only subcommand as a plain command |
| `curl` returns `000` / connection refused | host not on the network allowlist | use `WebFetch` for public pages, or a single unsandboxed `curl` for your own site |
| `socket(): Actie is niet toegestaan` from `paplay` | audio socket blocked | no sound from here; use a push notification |
| `grep: ...: Toegang geweigerd` for `tools/.claude/...`, `tools/.mcp.json` | sandbox placeholder files | ignore; never stage them |
| cannot create files in `.claude/agents` or `.claude/skills` | write-protected placeholders | create them with an unsandboxed call; see claude-code-workflow |

## PlatformIO and builds

| Symptom | Cause | Fix |
|---|---|---|
| `*** [src/HIDManager.cpp] .../.sconsign314.dblite: No such file or directory` | `platformio.ini` edited, or a second build started, while a build ran | wait for the running build; one build at a time; rebuild |
| build killed (`exit 137`) during `Scanning dependencies...` | a foreground call hit its timeout | start builds with `run_in_background` |
| `Please specify upload_port` or `Looking for upload port...` fails | udev rules missing, port hidden | `jbang tools/Flash.java`, or `--upload-port` with an unsandboxed call |
| a plain `pio run -t upload` on the CoreS3: `Failed to connect to ESP32-S3: No serial data received` | TinyUSB cannot auto-reset | `jbang tools/Flash.java cores3` (1200-baud touch first) |
| a build takes 10 minutes again after `pio run -t upload` | `upload` rebuilds | add `-t nobuild` to flash an existing image (or use `jbang tools/Flash.java`) |
| the wrong UI or firmware appears after flashing | `.pio/build/<env>/firmware.bin` came from another branch | `jbang tools/Flash.java` prints branch and warns about stale images; check `INFO fw=` |
| old commit does not build (`...rest of setup() and loop() remain the same ...`) | that commit was a work in progress | use the last working commit (`16fbf03` for the pre-code-generation firmware) |
| `RAM: ... of 327680 bytes` on the CoreS3 | PlatformIO counts internal RAM only; both boards have `BOARD_HAS_PSRAM` | do not treat it as the whole memory |

## Serial, USB and Bluetooth

| Symptom | Cause | Fix |
|---|---|---|
| no `/dev/ttyACM*` / `/dev/ttyUSB*` at all | kernel modules not loaded after a kernel update | reboot; `lsmod \| grep -E "cdc_acm\|cp210x"` |
| first command after opening the CoreS3 port gets no reply | opening the port reset the chip | wait 1.5 s after opening, retry (`LedCam` does) |
| the CoreS3 shows "USB JTAG/serial debug unit" but is silent | it is in the ROM bootloader, not running the app | reflash with `jbang tools/Flash.java` |
| evdev check fails with 0 events on the S3 | the test read the Bluetooth device while keys go over USB (or the reverse) | `SerialTest` matches by `Phys`; node numbers swap after reboots |
| must the board be paired again after a flash? | no: bonds survive an application-only flash | do not erase flash; `jbang tools/Flash.java` writes `0x10000` only |
| key presses arrive twice | USB and BLE both connected | `PREFER USB` (default) or `ONLY USB|BLE` |

## Camera

| Symptom | Cause | Fix |
|---|---|---|
| first frame completely black | exposure not settled / shutter closed | read 25 frames, keep the last; open the shutter |
| bar looks white or cyan | camera saturates | judge the reflection on a white surface |
| numeric colour check says `CHECK` for dim layers | glow is only a few brightness levels | judge the contact sheet by eye |
| pin sweep: all later pins score the same | the lit area is smaller than the percentile; exposure shifts | use the top-200 mean (already the default) and re-baseline per pin |

## Git, GitHub and the site

| Symptom | Cause | Fix |
|---|---|---|
| `bad substitution` in zsh | `"$c:path"` | `"${c}:path"` |
| `no matches found: --include=*.cpp` | unquoted glob | quote it |
| `git push` rejected, `fetch first` on `gh-pages` | the publish workflow pushed first | `git fetch origin gh-pages`, compare, do not force |
| `hanskruse.eu/Nachtgespenst/` is 404 | Pages paths are case-sensitive; the repository is `nachtgespenst` since 2026-11-18 | use `/nachtgespenst/` |
| screenshot of the page shows alt text on black | `loading="lazy"` images are not loaded by a one-shot headless screenshot | strip the attribute in a temporary copy |
| `could not lock config file .git/config` | sandbox protects `.git/config` | operations that write the config (upstream tracking) need an unsandboxed call |
