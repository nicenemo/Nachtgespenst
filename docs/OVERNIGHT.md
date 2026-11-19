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
   - [~] Heartbeat diagnostics are built in (`NG_DEBUG_HEARTBEAT`, default 0); the stall did not reproduce on later runs (stale bonds suspected). Kept in the TODO as a flake.
3. **LampArray without root** (the hidraw nodes are `root:root 0600`, so `LampTest` cannot run until the owner installs the udev rule)
   - [x] The kernel parses the 434-byte descriptor on USB and BLE; `featget` counts the attribute reads.
   - [x] The udev rule was installed; `LampTest` ran on all three links and the results are in the wiki (including controller comparison, USB limits, intake without drawing and latency).
4. **Page switching speed** ([ADR 0007](adr/0007-page-switching-speed-and-prerendering.md), accepted)
   - [x] Stage 1: coalesced page changes, measured.
   - [x] Stage 2: `-O2` and 48-row strips; CoreS3 727 to 318 ms, Core2 838 to 380 ms.
5. **LEDs off, persistent settings, Settings page** ([ADR 0014](adr/0014-settings-page-and-leds-off.md))
   - [x] Done and verified with the camera on both boards.
   - [ ] Not built: the touch page is still open (the settings backend and serial commands exist).
6. **Review items from the TODO that need no decision**
   - [x] Instead of rejecting them, a real System Control report was added; Power Down and Sleep are refused over serial.
   - [~] `STATE?` has `usb=` and `ble=`, the LED bits have names in `HIDManager`; `LAYER_COLORS` and the NimBLE warning are still open.
   - [x] Behind `NG_SERIAL_INPUT`; `TAP` only on the Locks page.
7. **Stress and flakes**
   - [x] `SerialTest`: CoreS3 43 of 43, Core2 25 of 25 on the final firmware.
8. **Camera**
   - [~] Core2 done (all seven states, photo on the site); the CoreS3 bar follows the host LED report only.
9. **Documentation, site, changelog**
   - [x] TODO, ADRs 0007 and 0010, wiki, CHANGELOG and the site pages are updated on `feature/lamparray`; publishing the site to GitHub Pages is not done.

## Needs the owner (not done tonight)
- **udev rule** (`udev-setup.yml`, kos project): without it `LampTest` cannot open `/dev/hidrawN`. Everything else about the LampArray is built and flashed.
- Decisions: ADR 0010 (scope of the lamp work), 0011, 0012, 0013 (which HID ideas), 0014 (whether the Settings page stays as built), the owner's other HID plan, the BLE pairing window with a button, own USB product IDs (pid.codes or Espressif).
- Real touch tests on both boards, Windows, Android and iPhone pairing with the new names and the emoji.
