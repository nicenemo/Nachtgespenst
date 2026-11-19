# LampArray test bench

The branch `feature/lamparray` turns both boards into a HID **LampArray** device (HID usage page 0x59, the standard behind Windows Dynamic Lighting) and adds a test bench to see how far that goes: how many lamps can be set on the screen, how fast, over Bluetooth on both boards and over USB on the CoreS3. Background and the larger plan: [ADR 0010](../adr/0010-lamparray-lighting-and-picture-pages.md). This is a demo and a measurement tool, not a product feature: the key pages and their layer colours are not touched.

## What the firmware does
- **Lamps.** One LampArray: lamps 0 to 9 are the ten LEDs of the bar, lamps 10 and up are a grid of screen lamps (default 16 x 12, at most 320 x 190 = 60,800, so a lamp can be a single pixel). `LAMPS GRID c r` changes the grid, and the lamp count the host reads follows.
- **Two test pages** after the Locks page: *Lamps: screen* (the grid, drawn straight to the panel from a PSRAM copy, only the rows that changed are pushed) and *Lamps: bar* (ten swatches with the intensity of each lamp; the LED bar shows the same ten lamps). Other pages are not coloured by the host.
- **Reports** (all in `include/HID_Specs.h`): attributes (feature 5), attributes request (6) and response (7), multi update of up to 8 lamps (output 8), range update (9) and the control report (10, autonomous mode). A multi update is 51 bytes. The screen and the bar only change when a report carries the *update complete* flag.
- **Intensity.** A lamp is red, green, blue and intensity, each 0 to 255; the shown colour is `colour * intensity / 255`. The bar then goes through FastLED's global brightness (40 of 255 by default, `LAMPS BRIGHT n` up to 120), so at low brightness the steps at the dark end are coarse: the camera test shows how many are visible.
- **Serial.** `LAMPS?` (counters: reports, lamps set, frames, receive window in microseconds, draws), `LAMPS RESET`, `LAMPS GRID c r`, `LAMPS GET id`, `LAMPS BRIGHT n`.
- **Bluetooth** asks for a 7.5 to 15 ms connection interval, MTU 247 and 251-byte data packets after connecting.

## Before the first run
1. **Pair again.** The HID report map changed, and hosts cache it. Remove both boards on the Linux laptop (`bluetoothctl remove <address>`) and pair them again, and do the same on Windows, Android and the iPhone. The boards are now called `NG-S3` and `NG-C2`. The CoreS3 on USB needs no pairing.
2. **hidraw access.** The tool writes reports to `/dev/hidrawN`, which is `root:root 0600` by default. One udev rule opens exactly the Nachtgespenst nodes (USB and Bluetooth, matched on the ID 1209:0001, see [ADR 0015](../adr/0015-device-identity-names-and-usb-bluetooth-ids.md)):

   ```
   # /etc/udev/rules.d/70-nachtgespenst-hidraw.rules
   SUBSYSTEM=="hidraw", KERNELS=="000[35]:1209:0001.*", TAG+="uaccess"
   ```
   The owner installs it with the Ansible playbook `udev-setup.yml` (and removes it with `udev-teardown.yml`), kept in the kos project; or by hand with `sudo udevadm control --reload && sudo udevadm trigger --subsystem-match=hidraw`. One-off alternative: `sudo setfacl -m u:$USER:rw /dev/hidrawN` for the node `LampTest.java info` reports.
3. **Sandbox.** No new entry is needed: `LampTest.java` lives in `tools/`, which the generic `*jbang*tools/*.java*` exclusion covers (hidraw, serial and the camera are invisible inside the sandbox). Run the tool as one plain command.

## What was verified without root (2026-11-19)
- **Descriptor accepted by Linux on all three links.** The kernel holds the same 434-byte report descriptor for the CoreS3 over USB, the CoreS3 over Bluetooth and the Core2 over Bluetooth (`/sys/class/hidraw/hidrawN/device/report_descriptor`, readable without root). Parsed with a small script, the report sizes are exactly what the firmware expects: attributes 22 bytes (feature 5), request 2 (6), response 28 (7), multi update 50 (output 8), range update 9 (9), control 1 (10), next to the keyboard (8 in, 1 out) and consumer (2 in) reports.
- **The host reads the feature reports.** The `LAMPS?` counter `featget` rises when the kernel or BlueZ reads the attributes report right after connecting.
- **Identity.** BlueZ shows `Modalias usb:v1209p0001d0101` (CoreS3) and `d0201` (Core2), the PnP ID of ADR 0015.
- **Right after the first pairing** BlueZ creates the Bluetooth HID node of a board with ID `0005:0000:0000`; after one disconnect and reconnect it is `0005:1209:0001`, which is what the udev rule matches. So reconnect once after pairing (`bluetoothctl disconnect ADDRESS`, then `connect`).

## Running it
```
jbang tools/LampTest.java info   --board S3 --via usb
jbang tools/LampTest.java enum   --board S3 --via usb
jbang tools/LampTest.java bar    --board S3 --via ble --crop 680,400,920,550     # crop = the LED bar in the camera frame
jbang tools/LampTest.java screen --board C2 --via ble --tiers 8x6,16x12,32x24 --shot /tmp/lamps
```
- `enum` reads the attributes report, requests the attributes of up to 200 lamps and prints the time per lamp (what a host pays when the device appears).
- `bar` sets ten different colours and intensities and reads them back, ramps the intensity of all ten lamps from 0 to 255 (the camera measures the brightness when `--crop` is given; frames are deleted afterwards, only a cropped contact sheet `intensity.png` stays), checks the three colour channels and fires 2,000 range updates back to back.
- `screen` sets each grid size (default 8x6 up to 320x190), sends frames as fast as the host can write, waits for the board, and prints frames per second, lamps per second, the loss (reports the board never received), how often the board drew, and checks sampled lamps in a screenshot. `--shot DIR` keeps the screenshots. A screenshot over the Core2's UART takes about 20 s.
- `--via usb|ble` chooses the transport (the Core2 only has Bluetooth). `PREFER` and `ONLY` do not matter: lamp reports are not routed by them.

Results of the first measurements: see the bottom of this page.

## Results (2026-11-19, Linux laptop with BlueZ, firmware of branch `feature/lamparray`)
Every functional check passes on all three links: CoreS3 over USB, CoreS3 over Bluetooth and Core2 over Bluetooth. The screen is compared with the lamps by screenshot (every sampled lamp matches at every grid size), the bar by readback and the webcam.

| | CoreS3, USB | CoreS3, Bluetooth | Core2, Bluetooth |
|---|---|---|---|
| Reports delivered per second | 1,000 | 16 (51-byte multi update), 30 (10-byte range update) | 29 |
| Lamps per second | 8,000 | 126 (multi), 300 (range) | 231 (multi), 288 (range) |
| Loss when sending as fast as possible | none | none for multi updates; range updates lose about a third when flooded | about a third of the range updates |
| Enumerating the lamp attributes | 0.4 ms per lamp (202 lamps: 0.1 s) | 61 ms per lamp (202 lamps: 12 s) | 63 ms per lamp (202 lamps: 13 s) |
| Time to draw a changed screen | about 45 ms, 175 ms for all 60,800 pixels | about 48 ms | about 80 ms |

Screen frame rate that follows from the delivery rate (all lamps changing every frame, eight lamps per multi update):

| Grid | Lamps | USB | Bluetooth, CoreS3 | Bluetooth, Core2 |
|---|---|---|---|---|
| 8 x 6 | 48 | 166 fps | 2.7 fps | 4.7 fps |
| 16 x 12 | 192 | 42 fps | 0.7 fps | 1.2 fps |
| 32 x 24 | 768 | 10 fps | 0.2 fps | 0.3 fps |
| 64 x 48 | 3,072 | 2.6 fps | 0.04 fps | 0.1 fps |
| 128 x 96 | 12,288 | 0.7 fps | 0.01 fps | not measured |
| 320 x 190 (one lamp per pixel) | 60,800 | 0.1 fps (7.6 s per frame) | not measured (about 8 minutes per frame) | not measured |

What this means:
- **USB is the transport for lamps.** 8,000 lamps per second is enough for the 10-lamp bar at hundreds of frames per second, for a 16 x 12 matrix at 40 fps and for a 32 x 24 matrix at 10 fps. One lamp per pixel is possible but takes seconds per frame: use the Photo channel of ADR 0010 for video-like content.
- **Bluetooth carries the bar and effects, not pictures.** About 30 range updates per second is a smooth 30 fps for the whole bar in one colour; per-lamp colours on the ten lamps need two multi updates, so 8 to 14 fps. A full tile layer (tier B, about 34 lamps) is about 5 multi updates, so 3 to 6 fps. The limit is the host side: BlueZ sends one queued write per connection interval (about 30 ms) and ignores the 7.5 to 15 ms the firmware asks for; Windows, with shorter intervals, is expected to be faster and has to be measured.
- **Do not flood Bluetooth.** BlueZ queues output reports without limit and drains them at those rates, so a burst arrives minutes late (2,000 reports take about 70 seconds), and the 10-byte range updates start to be dropped when more than about 50 are queued. Send at most one report per 30 ms, and let the queue drain between tests (`LampTest` does).
- **Enumeration over Bluetooth is slow** because every lamp needs a write and a read: 768 lamps take about 48 seconds when a host connects, 3,072 lamps about 3 minutes. Windows enumerates all lamps, so a tier C or D matrix over Bluetooth is not practical; tier A or B (10 or about 34 lamps) is.
- **Intensity.** Every lamp has red, green, blue and intensity (0 to 255) and the shown colour is `colour x intensity / 255`. On the bar, at the global brightness of 40 of 255, the camera sees no difference for intensities 1 to 12 and a steady, monotonic rise from 16 (CoreS3: 11 of 16 steps visible) or 32 (Core2: 7 of 16, with a jump between 24 and 32 that is partly the webcam's exposure): the ten-bit style range is squeezed into about 40 real levels. `LAMPS BRIGHT n` raises the global brightness (to 120 at most); a gamma curve in the firmware would spread the steps better.
- **Counters.** `LAMPS?` counts every report including the control report, which is why a run of N updates shows N + 1.

How the numbers are produced: `LampTest.java screen`, `bar` and `rate` (`jbang tools/LampTest.java rate --board S3 --via ble --pace-us 1` sweeps the send interval and reports what the board actually saw). Camera brightness curves: `bar --crop ...`.
