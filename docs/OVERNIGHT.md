# Night plan 2026-11-19

Written at the start of the night. The owner is asleep, the room is dark (camera tests possible), both boards are on the Linux laptop. Everything here can be done and tested without asking; what needs a decision or root rights is listed at the end. The previous night's plan is in [archive/OVERNIGHT.md](archive/OVERNIGHT.md). Status is updated as the night goes on (`[x]` done and tested, `[~]` partly, `[ ]` not done, `[!]` blocked).

## Rules for the night
- Branch `feature/lamparray` carries the firmware the boards run (LampArray bench, new identity). Work on it; `develop` gets docs and site only; nothing goes to `main`, no pull requests.
- One `pio` build at a time, in the background, with `-d <project>`; flash with `jbang tools/Flash.java`; confirm `INFO fw=` before trusting a test; both boards pass `SerialTest` before a commit.
- Commit per topic, explicit paths, the secrets hooks stay on (never `--no-verify`).
- British English in docs and comments; the TODO, the site and the wiki are updated for everything that gets finished.

## Plan
1. **Orientation and Bluetooth** (no build)
   - [x] Pair both boards again (the bonds were removed): the names are `🌙👻-cs3` and `🌙👻-c2`, BlueZ shows `Modalias usb:v1209p0001d0101` (CoreS3) and `d0201` (Core2), so the PnP ID reaches the host.
   - [x] `SerialTest --board S3`: 29 of 29 with the new identity (USB and BLE routing, BLE evdev found by the `-cs3` name).
   - [x] Camera (lights off): `LedCam colours` on both boards, all 26 layer colours show the right hue.
2. **Regression: the Core2 stops answering on serial while a Bluetooth host is bonded** (found while testing step 1)
   - [~] Symptom: with the bond `SerialTest --board C2` loses replies for 20 to 30 seconds after every port open (the open resets the board); without the bond 11 of 11 pass. The CoreS3 is not affected.
   - [ ] Diagnose with a heartbeat build, fix, test with and without a bond on both boards.
3. **LampArray without root** (the hidraw nodes are `root:root 0600`, so `LampTest` cannot run until the owner installs the udev rule)
   - [ ] Check the report descriptors the kernel got (USB and BLE) through sysfs and a small parser; check the feature report counters over serial (`featget`).
   - [ ] Keep `LampTest` ready (compiles, documented); results go to the wiki when the owner has run it.
4. **Page switching speed** ([ADR 0007](adr/0007-page-switching-speed-and-prerendering.md), accepted)
   - [ ] Stage 1: coalesce page changes; measure with `SerialTest --latency`.
   - [ ] Stage 2: slimmer pages (tile pool, flatter styles, the display buffer question), measure again; stop when the numbers stop improving.
5. **LEDs off, persistent settings, Settings page** ([ADR 0014](adr/0014-settings-page-and-leds-off.md))
   - [ ] `LEDS OFF|LOCKS|ON`, brightness, idle off, stored in NVS; camera test of the dark bar.
   - [ ] Settings page (LED, screen, routing) with screenshots on both boards.
6. **Review items from the TODO that need no decision**
   - [ ] System-control keys rejected at compile time (a descriptor change would force another re-pairing).
   - [ ] `STATE?` gets `usb=` and `ble=` next to `conn=`; the magic LED bits get names; `LAYER_COLORS` becomes `inline constexpr`; the NimBLE `startServices()` warning.
   - [ ] `KEY`/`TAP` behind `NG_SERIAL_INPUT`.
7. **Stress and flakes**
   - [ ] `SerialTest` repeated on both boards (S3 USB and BLE, C2); record anything odd for the two unexplained cases in the TODO.
8. **Camera**
   - [ ] Locks page colours (caps, num, scroll and the mixes) with the camera on both boards (the TODO's open item 2).
9. **Documentation, site, changelog**
   - [ ] TODO, ADR status lines, wiki (hardware testing, LampArray, workflow lessons), site (pages that mention the features, the story), README, CHANGELOG.

## Needs the owner (not done tonight)
- **udev rule** (`udev-setup.yml`, kos project): without it `LampTest` cannot open `/dev/hidrawN`. Everything else about the LampArray is built and flashed.
- Decisions: ADR 0010 (scope of the lamp work), 0011, 0012, 0013 (which HID ideas), 0014 (whether the Settings page stays as built), the owner's other HID plan, the BLE pairing window with a button, own USB product IDs (pid.codes or Espressif).
- Real touch tests on both boards, Windows, Android and iPhone pairing with the new names and the emoji.
