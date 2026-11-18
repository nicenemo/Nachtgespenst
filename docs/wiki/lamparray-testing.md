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
1. **Pair again.** The HID report map changed, and hosts cache it. Remove both boards on the Linux laptop (`bluetoothctl remove <address>`) and pair them again. The CoreS3 on USB needs no re-pairing.
2. **hidraw access.** The tool writes reports to `/dev/hidrawN`, which is `root:root 0600` by default. As root add a rule (the first line is USB on the CoreS3, the second any Bluetooth HID device) and reload:

   ```
   # /etc/udev/rules.d/70-nachtgespenst-hidraw.rules
   SUBSYSTEM=="hidraw", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", TAG+="uaccess"
   SUBSYSTEM=="hidraw", KERNELS=="0005:*", TAG+="uaccess"
   ```
   `sudo udevadm control --reload && sudo udevadm trigger --subsystem-match=hidraw`, then reconnect the boards. One-off alternative: `sudo setfacl -m u:$USER:rw /dev/hidrawN` for the node `LampTest.java info` reports (the number changes on every reconnect).
3. **Sandbox.** No new entry is needed: `LampTest.java` lives in `tools/`, which the generic `*jbang*tools/*.java*` exclusion covers (hidraw, serial and the camera are invisible inside the sandbox). Run the tool as one plain command.

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

## Results
(not measured yet)
