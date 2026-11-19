# LampArray test bench

The branch `feature/lamparray` turns both boards into a HID **LampArray** device (HID usage page 0x59, the standard behind Windows Dynamic Lighting) and adds a test bench to see how far that goes: how many lamps can be set on the screen, how fast, over Bluetooth on both boards and over USB on the CoreS3. Background and the larger plan: [ADR 0010](../adr/0010-lamparray-lighting-and-picture-pages.md). This is a demo and a measurement tool, not a product feature: the key pages and their layer colours are not touched.

## What the firmware does
- **Lamps.** One LampArray: lamps 0 to 9 are the ten LEDs of the bar, lamps 10 and up are a grid of screen lamps (default 16 x 12, at most 320 x 190 = 60,800, so a lamp can be a single pixel). `lamps grid c r` changes the grid, and the lamp count the host reads follows.
- **Two test pages** after the Locks page: *Lamps: screen* (the grid, drawn straight to the panel from a PSRAM copy, only the rows that changed are pushed) and *Lamps: bar* (ten swatches with the intensity of each lamp; the LED bar shows the same ten lamps). Other pages are not coloured by the host.
- **Reports** (all in `include/HID_Specs.h`): attributes (feature 5), attributes request (6) and response (7), multi update of up to 8 lamps (output 8), range update (9) and the control report (10, autonomous mode). A multi update is 51 bytes. The screen and the bar only change when a report carries the *update complete* flag.
- **Intensity.** A lamp is red, green, blue and intensity, each 0 to 255; the shown colour is `colour * intensity / 255`. The bar then goes through FastLED's global brightness (40 of 255 by default, `lamps bright n` up to 120), so at low brightness the steps at the dark end are coarse: the camera test shows how many are visible.
- **Serial.** `lamps` (counters: reports, lamps set, frames, receive window in microseconds, draws), `lamps reset`, `lamps grid c r`, `lamps get id`, `lamps bright n`.
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
- **The host reads the feature reports.** The `lamps` counter `featget` rises when the kernel or BlueZ reads the attributes report right after connecting.
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
- `rate` sweeps the send interval and reports what the board actually saw (reports per second over the board's own receive window). `speed` (USB) switches lamps as fast as possible with 8-lamp multi updates and with range updates of 1 to 60,800 lamps. `interference` repeats the same burst with 100 serial PINGs per second, on the lamp page and on a layer page, to see what else changes the rate.
- `--via usb|ble` chooses the transport (the Core2 only has Bluetooth). `prefer` and `only` do not matter: lamp reports are not routed by them.

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
- **Bluetooth carries the bar and effects, not pictures.** About 30 range updates per second is a smooth 30 fps for the whole bar in one colour; per-lamp colours on the ten lamps need two multi updates, so 8 to 14 fps. A full tile layer (tier B, about 34 lamps) is about 5 multi updates, so 3 to 6 fps. The limit is the host side: the host delivers about one queued write per 33 ms and lowering BlueZ's connection interval did not change that (see below); Windows is expected to behave differently and has to be measured.
- **Do not flood Bluetooth.** BlueZ queues output reports without limit and drains them at those rates, so a burst arrives minutes late (2,000 reports take about 70 seconds), and the 10-byte range updates start to be dropped when more than about 50 are queued. Send at most one report per 30 ms, and let the queue drain between tests (`LampTest` does).
- **Enumeration over Bluetooth is slow** because every lamp needs a write and a read: 768 lamps take about 48 seconds when a host connects, 3,072 lamps about 3 minutes. Windows enumerates all lamps, so a tier C or D matrix over Bluetooth is not practical; tier A or B (10 or about 34 lamps) is.
- **Intensity.** Every lamp has red, green, blue and intensity (0 to 255) and the shown colour is `colour x intensity / 255`. On the bar, at the global brightness of 40 of 255, the camera sees no difference for intensities 1 to 12 and a steady, monotonic rise from 16 (CoreS3: 11 of 16 steps visible) or 32 (Core2: 7 of 16, with a jump between 24 and 32 that is partly the webcam's exposure): the ten-bit style range is squeezed into about 40 real levels. `lamps bright n` raises the global brightness (to 120 at most); a gamma curve in the firmware would spread the steps better.
- **Counters.** `lamps` counts every report including the control report, which is why a run of N updates shows N + 1.

## Speed limits and what disturbs the measurement (2026-11-19)
Two Bluetooth controllers sit in the laptop and both were used: `hci1` is the built-in **Intel** controller (USB 8087:0a2a, Bluetooth 5) and `hci0` is the removable **CSR8510 A10** dongle (USB 0a12:0001, Bluetooth 4.0). Each board was bonded to one controller at a time (the other bond removed) and measured with `rate`. The `HID_PHYS` of the hidraw node shows which controller carries the board.

**Bluetooth: the controller makes no difference, the link does.** Reports delivered per second to the board, steady state, with the send queue full:

| Board | Report | Intel (BT 5) | CSR dongle (BT 4.0) |
|---|---|---|---|
| CoreS3 | multi update, 51 bytes (8 lamps) | 15.8 /s = 127 lamps/s | 15.9 /s = 127 lamps/s |
| CoreS3 | range update, 10 bytes | 30.1 /s | 30.8 /s |
| Core2 | multi update, 51 bytes | 29.2 /s = 234 lamps/s | 30.4 /s = 243 lamps/s |
| Core2 | range update, 10 bytes | 30.1 /s | 30.4 /s |

So no: neither controller handles more than about 30 reports per second, and the number is the same with a Bluetooth 4.0 and a Bluetooth 5 radio, i.e. it is not the radio. 30 per second is one write per 33 ms: BlueZ sends one queued write per connection event and its default connection interval is about 30 ms; the 7.5 to 15 ms the firmware asks for is not applied. **Tested: lowering the connection interval does not help.** The interval was lowered through BlueZ's `main.conf` (`MinConnectionInterval` 6, `MaxConnectionInterval` 9, i.e. 7.5 to 11.25 ms; kos ADR 0015, `bluez-setup.yml`), `bluetoothd` was restarted, the kernel showed 6 and 9 in `conn_min_interval` and `conn_max_interval` of both controllers and the boards reconnected. The Core2 then still delivered 30.1 to 30.6 reports per second at every send interval. So either the link did not use the new interval (the board may request another one, or the host keeps 30 ms for HID over GATT) or the limit is elsewhere in BlueZ's write path. Next step: a `btmon` capture while a board reconnects, to see the interval that is really negotiated. The tuning is still active and can be removed with `bluez-teardown.yml`. Open question: why the CoreS3 manages only half the rate with 51-byte writes (16 against 30 per second) while the Core2 does not; both controllers show it, so it is on the board's side (the ESP32-S3 stack, its MTU or data length negotiation) and has to be looked into before the multi update is used over Bluetooth on the CoreS3. The range update (one colour for many lamps) is the efficient Bluetooth report anyway: 30 reports per second can switch up to 30 times 60,800 lamps.

**USB (CoreS3 only): 1,000 reports per second is the ceiling, then the firmware.** Full-speed USB polls an interrupt endpoint once per millisecond, so one report per millisecond:

| Report | Reports per second | Lamps switched per second |
|---|---|---|
| multi update, 8 lamps | 960 to 1,000 | 7,700 |
| range, 1 lamp | 910 | 910 |
| range, 192 lamps | 910 | 174,000 |
| range, 768 lamps | 880 | 673,000 |
| range, 3,072 lamps | 253 | 777,000 |
| range, 12,288 lamps | 62 | 761,000 |
| range, 60,800 lamps (every pixel) | 13 | 810,000 |

From about 3,000 lamps per report the limit is no longer USB but the firmware. On a lamp page, where the board also draws, about 800,000 lamps per second get through (storing and drawing share the CPU); with nothing drawn the intake is much higher, see `ingest` below. Lamp storage could be made faster (set the changed bits per range instead of per lamp) if that ever matters. What the screen shows is limited by the panel instead: a changed screen takes 45 ms to draw (175 ms for all 60,800 pixels), so at most about 20 frames per second, and frames are merged when updates come faster.

**What disturbs the measurement?** Tested with `interference` (a burst with and without a serial PING every 10 ms, on the bar page, on the screen lamp page where the board redraws, and on a layer page with no lamp drawing):
- **The USB serial console is not in the way.** On the CoreS3 over USB the rate moves by +0.3 to +0.4% (952 against 948 reports per second) with or without 100 serial messages per second, and with or without drawing. The serial port and the HID interface share the bus, but at these rates the bus has room.
- **The CoreS3 over Bluetooth is not disturbed either** (16.0 to 16.1 reports per second in every variant).
- **The Core2 over Bluetooth is slightly sensitive to serial traffic:** 100 PINGs per second cost 2 to 7% (28.8 to 30.3 against 31.1 reports per second), because its serial port is a UART with interrupts and the main loop answers every message on the same CPU as the Bluetooth stack. The drawing does not matter. `LampTest` therefore keeps the serial port quiet while it sends and reads the counters afterwards.
- **Real sources of error, not caused by the console:** (1) the host queue: BlueZ queues output reports without a bound, so sending faster than the link drains makes a burst arrive late (and range updates get dropped past about 50 queued); the tool paces its sending and waits for the queue to drain before and after every test. (2) Other Bluetooth devices on the same controller: the laptop has speakers and headphones paired, they were not connected during the measurements (`bluetoothctl devices Connected` showed only the two boards); a connected audio device would take airtime. (3) The second board stays connected but idle during a measurement. (4) The host side of USB: one write is one blocking transfer, the host counted 997 to 1,000 per second and the board 948 to 961 (the board's window starts at the first report, so the difference of about 5% is start-up and scheduling jitter; use the host figure as the ceiling and the board figure as what was received).
- **Do not test the Power Down and Sleep tiles.** They really power down or suspend the computer. The test tools only press the Wake Up tile of the Media page (harmless); the firmware now refuses Power Down and Sleep over the serial port (`key` and `tap` answer `ERR`).

How the numbers are produced: `LampTest.java screen`, `bar` and `rate` (`jbang tools/LampTest.java rate --board S3 --via ble --pace-us 1` sweeps the send interval and reports what the board actually saw). Camera brightness curves: `bar --crop ...`.

## Where the bar lamps are (webcam, 2026-11-19)
`LampTest barmap` lights every bar lamp on its own while the webcam films the board, compares each frame with the average of the other nine (a change of exposure drops out) and prints where the light is in the picture. Show one side of the board to the camera at a time. On the CoreS3 base: with the right side towards the camera lamps 0 to 4 stand on one line along the near edge (lamp 0 to the right in the picture, lamp 4 to the left, 12 pixels apart); with the left side towards the camera lamps 5 to 9 do (lamp 5 to the right, lamp 9 to the left). With the board lying flat and its screen up that is **one chain in a U: down the right side (0 at the top), round the bottom and up the left side (9 at the top)**. A third run with the board upside down (right side to the camera) put lamp 0 on the other end of the strip in the picture, which confirms the order. `LampTest effectcheck` follows a Nightrider with the webcam: the light travels over the whole strip. The demo's bar effects and the window use this mapping; the position attributes in the firmware still need it.

## Intake without drawing (`ingest`, 2026-11-19)
`LampTest ingest` is the speed test on an ordinary layer page: nothing is drawn and the bar does not follow the lamps, so it measures the intake of the board alone. CoreS3 over USB, every report arrived:

| Report | Reports per second | Lamps per second |
|---|---|---|
| multi update, 8 lamps | 963 | 7,700 |
| range, 1 lamp | 908 | 908 |
| range, 192 lamps | 908 | 174,000 |
| range, 768 lamps | 908 | 698,000 |
| range, 3,072 lamps | 476 | 1.46 million |
| range, 12,288 lamps | 129 | 1.58 million |
| range, 60,800 lamps | 31 | 1.86 million |

Storing a lamp therefore costs about 0.5 microseconds, not the 1.25 that the numbers on a lamp page suggested: the rest was the drawing competing for the CPU. More lamps do not help: a lamp ID is 16 bits, so one LampArray holds at most 65,535 lamps and the 60,810 of the grid and the bar are already 93% of that.

## Latency to the LED bar and the screen, and the bar worker task (2026-11-19)
Question: do the screen drawing, the bar update in the main loop or sleeping in the main loop delay what the user sees? The board stamps the report that completes a frame and measures until the bar has been shown (`bar_lat_us`) and until the screen has been drawn (`scr_lat_us`), average and maximum, in `lamps`. `LampTest latency` sends 40 frames 150 ms apart (the bar lamps plus a 16 x 12 grid) on the bar page and on the screen lamp page, once with the bar shown by a worker task on the second core (`lamps worker on`, the default) and once by the main loop (`lamps worker off`). The worker task takes the FastLED mutex that the main loop uses for the layer colours, so the two cannot write the bar at the same time, and `ui_loop` no longer sleeps while a lamp frame is waiting to be drawn.

| Board and link | Bar, worker on (avg / max) | Bar, main loop (avg / max) | Screen drawn |
|---|---|---|---|
| CoreS3, USB, bar page | 1.4 / 2.5 ms | 5.9 / 10.9 ms | 56 / 62 ms |
| CoreS3, USB, screen lamp page | 1.4 / 2.5 ms | 6.7 / 41 ms | 40 / 201 ms |
| CoreS3, Bluetooth, bar page | 1.6 / 2.7 ms | 5.2 / 11.9 ms | 57 / 63 ms |
| CoreS3, Bluetooth, screen lamp page | 1.6 / 2.7 ms | 5.7 / 10.9 ms | 36 to 39 ms |
| Core2, Bluetooth, bar page | 3.0 / 3.1 ms | 6.0 / 10.9 ms | 59 / 63 ms |
| Core2, Bluetooth, screen lamp page | 3.4 / 5.0 ms | 5.9 / 17 ms | 48 / 205 ms |

The worker makes the bar two to four times faster and keeps the maximum low while the screen is drawing, so a lamp frame is on the bar within a few milliseconds of its last report on both boards, and the screen follows 40 to 60 ms later because drawing takes that long. The screen drawing is unchanged by the worker. Remaining latency on the Core2 (3 ms against 1.4 ms) is its slower FastLED path and the shared core of the Bluetooth stack.
