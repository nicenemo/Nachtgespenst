# 0010 HID LampArray lighting, a photo and webcam page, and a JBang tool on the PC

Status: Proposed. Nothing is implemented. Builds on [0009](0009-unicode-pages-and-host-os-awareness.md) for the page menu and on [0007](0007-page-switching-speed-and-prerendering.md) for rendering.

## Context
The boards have two things the owner wants to use for something fun: a 10-pixel LED bar (M5GO base, GPIO25 on the Core2 and GPIO5 on the CoreS3) and a 320 x 240 screen. The HID specification has a page for exactly this: Lighting and Illumination (usage page 0x59, "LampArray"). The generated vocabulary in `include/hid_pages/LightingAndIllumination.h` already contains all 33 usages (the LampArray collection, its attributes report, the lamp attributes request and response, the multi-update and range-update reports, the control report and autonomous mode). `HID_Specs.h` has a small placeholder `LightingBlock` (one output report with an ID and three colour bytes) that is deliberately not advertised: it is not a real LampArray, so a host would expect reports the firmware does not provide.

Windows 11 has native support for HID LampArray devices (Dynamic Lighting: Settings, Personalization, Dynamic Lighting; effects, brightness, syncing several devices, app integration). Microsoft publishes two references: the `ArduinoHidForWindows` library and the `RP2040MacropadHidSample`, a macro pad with a LampArray built on TinyUSB, the stack the CoreS3 already uses for USB HID. Both are documented for USB; whether Windows also accepts a LampArray over Bluetooth HID is not stated in the pages read and has to be tested (the owner has a Windows machine). Linux, macOS, Android and iOS have no native LampArray consumer, so there the colours would come from a tool.

## Idea
1. **The LED bar as a real LampArray.** Ten lamps in a line, RGB (plus intensity), kind "peripheral"; later also one lamp per tile, so host lighting effects can colour the keycaps (see "How far can the lamp count go?"). Windows Dynamic Lighting can then colour the bar, match the Windows accent colour, and sync it with the owner's other lighting devices, and any LampArray app can drive it.
2. **A tool on the PC** (`tools/Lamps.java`, JBang) that writes LampArray reports to the board (through the OS's HID interface, `hid4java` or `hidraw`) from: a chosen photo, the webcam, the screen's edge colours, an audio spectrum ([0012](0012-audio-monitor-and-music-visualiser.md)), a CPU or network load meter ([0011](0011-performance-monitor-and-device-status.md)). The bar then shows ten colours sampled from the picture along one edge, like an ambient light. The same reports work over USB and, if the OS exposes the Bluetooth HID device, over BLE, without any serial connection.
3. **A photo and webcam page on the screen.** A page "Photo" (in the Device group of the page menu) that shows a picture pushed from the PC: a photo chosen on the PC, or live webcam frames. A tool (`tools/Show.java`, or a mode of `Lamps.java`) scales the picture to 320 x 240, encodes it as JPEG and sends it; the firmware decodes it (M5GFX `drawJpg`) and draws it. Photo and ambient light can run together: the screen shows the picture while the bar shows its colours.

## Design
**LampArray (firmware).**
- Replace the placeholder `LightingBlock` by a descriptor that follows the HID Usage Tables 1.4 LampArray collection, with the reports Windows needs: the array attributes report (lamp count, bounding box, kind, minimum update interval), the lamp attributes request and response (per lamp: position in micrometres, update latency, purposes, colour level counts, programmable), the multi-update and range-update reports (colour per lamp, with an update-complete flag), and the control report (autonomous mode). The usage names exist in the generated header; the Microsoft samples are the reference for the byte layout.
- **Autonomous mode** is how today's behaviour survives: while the host has not taken control, the bar keeps showing the layer colour and the Locks colours exactly as now; when a host switches autonomous mode off, the firmware shows the host's colours; on disconnect, or after a timeout without updates, it returns to autonomous mode.
- USB handling in the TinyUSB HID callbacks (get and set report for feature and output reports); BLE handling in the NimBLE HID device (feature and output report characteristics). The LED output keeps one FastLED `show()` path, rate-limited to the minimum update interval the firmware advertises (30 Hz is a sensible start).
- **One descriptor change, once.** A new collection changes the HID report map. Bonded BLE hosts (Windows, Android, iOS) cache the report map, so after flashing the boards have to be removed and paired again. Other descriptor changes should therefore ride along (for example the System control report in the TODO).

**Picture transfer.**
- First transport: the existing serial protocol, a command `IMG <bytes>` followed by JPEG data, acknowledged per frame so the PC never overruns the board. CoreS3: USB CDC. Core2: the CP2104 UART at a higher baud rate (921600) in picture mode. Rough estimates, to be measured: a 320 x 240 JPEG at quality 70 is 15 to 25 kB; on the CoreS3 that is tens of milliseconds to transfer, decode in roughly 30 to 60 ms, so around 10 frames per second; on the Core2 about 0.25 s per frame, around 3 frames per second, so mainly photos there.
- Later, if a serial cable is not wanted: a vendor-defined HID output report channel (works over USB and BLE, slow) or a BLE GATT or Wi-Fi channel. Not in the first round.
- Rendering: the Photo page draws the decoded frame straight to the panel while it is active (LVGL idle, no widget tree), which also avoids the slow page build. The classic UI does the same with `drawJpg`.

**Tools (JBang, [ADR 0008](0008-jbang-for-project-tooling.md)).** `Lamps.java`: sources `photo FILE`, `webcam`, `screen`, `audio`, `load`; targets the LampArray over HID and, with `--screen`, the Photo page over serial. The webcam is captured with ffmpeg as `LedCam.java` already does; HID access through `hid4java` (cross-platform) so it runs on Windows and Linux. Webcam use is always an explicit command.

## Tests
- Descriptor: validate the report descriptor with a HID tool on Linux (`hidrd` or `usbhid-dump` on USB), and check that Windows lists the device under Dynamic Lighting and can set colours.
- Firmware: a serial command `LAMPS?` returns the current lamp colours and the mode (autonomous or host); `Lamps.java` sets known colours and the test reads them back.
- The webcam closes the loop: `LedCam` films the bar while the tool sends a known colour sequence, as in the existing colour test.
- Photo page: `SHOT` after pushing a known image, compared with the expected pixels.
- Compatibility: connect each host (Linux, Windows, Android, the iPhone SE) with the new descriptor and check that keys, media keys and the lock LEDs still work.

## How far can the lamp count go?
Every tile of the screen and even every pixel can be described as a lamp, but the standard transport decides how many are usable. Numbers below are estimates from the USB rules and from what was measured on the boards; the host side (how many lamps Windows copes with) is not verified.

- **Transport of a LampArray.** A multi-update report carries at most 8 lamps (a lamp id plus red, green, blue and intensity: about 51 bytes, which fits a 64-byte full-speed packet). The CoreS3's USB is full speed with a 1 ms poll interval, so at most 1000 reports per second, 8000 lamps per second in theory, half of that in practice. A range update colours a contiguous range of lamps with one colour and is cheap for flat areas. Over BLE the throughput is several times lower (guess: 5 to 20 kB/s of output reports).
- **Enumeration.** The host asks for the attributes of every lamp one by one (position, latency, level counts) when the device appears: a few milliseconds per lamp, so hundreds of lamps cost seconds at connect and tens of thousands would cost minutes.
- **The panel.** A full-screen flush measured 55 ms (CoreS3) and 73 ms (Core2), which caps full-frame animation at roughly 18 and 14 frames per second regardless of how the colours arrive; partial updates are cheaper.

| Tier | Lamps | Reports per frame | Frame rate over USB (theory / realistic) | Remark |
|---|---|---|---|---|
| A. The LED bar | 10 | 2 | 30 Hz or more easily | what 0010 starts with |
| B. Bar plus one lamp per tile (a 6x4 grid at most) | about 34 | 5 | 30 Hz easily; fine over BLE | the sweet spot: Windows effects (wave, gradient, breathing, accent colour) paint the keycaps of the macro pad, laid out with the real tile positions, together with the owner's other RGB devices |
| C. A coarse matrix, 16 x 12 | 192 | 24 | about 40 / 20 fps | a lamp is a 20 x 20 pixel block of the screen; good ambient and effect surface |
| D. A fine matrix, 32 x 24 | 768 | 96 | about 10 / 5 fps | stretch; slow enumeration; effects, not video |
| E. Every pixel, 320 x 240 | 76,800 | 9,600 | about 0.1 fps | not usable as a LampArray (enumeration alone would take minutes) |

**Every pixel is possible through the side channel instead.** Pushing the picture over the serial link (the Photo page) reaches full resolution: measured today, the CoreS3 moves 230 kB from the board to the PC in about 0.5 s over USB CDC, so about 0.15 to 0.3 s for a raw 153.6 kB RGB565 frame (3 to 6 fps), 10 or more frames per second with JPEG, and more again with changed-tile updates; the panel flush limits it to about 15 frames per second. The Core2's UART (921600 baud, about 90 kB/s) manages JPEG at a few frames per second. So the pixels belong to the Photo and video channel, and the LampArray stays coarse (tier B or C) so that standard hosts such as Windows can use it.

**Physical pixels are the other extreme.** A WS2812 matrix on the Grove port (8 x 8, 16 x 16 or 32 x 8 modules) makes real lamps: at 800 kHz a LED takes about 30 microseconds, so 256 LEDs update in about 8 ms and 1024 LEDs in about 31 ms, faster than any HID transport can feed them. The limit is power: a white LED draws up to 60 mA, so 256 LEDs would need about 15 A; the matrix needs its own supply and a software current limit (FastLED `setMaxPowerInVoltsAndMilliamps`), and the M-Bus 5 V cannot feed it. As a LampArray it is the same descriptor with 256 lamps (tier C or D numbers apply to the HID side).

Recommendation: build tier B first (bar plus tiles), design the lamp layout table so a tier C matrix is a configuration change, and leave full pixels to the Photo and video channel.

## Phasing
1. LampArray descriptor and USB handling on the CoreS3 with autonomous mode; check in Windows Dynamic Lighting and with a tool on Linux.
2. LampArray over BLE on both boards; test with the Windows machine whether Dynamic Lighting accepts it; re-pairing documented.
3. `Lamps.java` with photo and webcam sources (ambient light on the bar).
4. Photo page and `IMG` over serial (CoreS3 first, then the Core2), `Show` modes in the tool.
5. More sources (screen edge, audio, load), a notification effect.

## Risks and open questions
- Bluetooth LampArray support in Windows is unverified; USB is the documented path. The Core2 has no USB HID, so on the Core2 the LampArray depends on that BLE test.
- The report map grows (BLE report maps are read in chunks; check MTU and the host's caching) and a descriptor change forces re-pairing on every bonded host.
- An extra collection could confuse a host. Linux, Android and iOS are expected to ignore an unknown usage page; test each of the four.
- The ten lamps give a coarse picture; a better ambient effect needs a smarter mapping (edge sampling) than averaging strips.
- Decode time, memory (PSRAM or heap for a JPEG buffer) and a flow-controlled protocol need measuring; the serial `IMG` path must not disturb the normal protocol or the tests.
- Privacy: the tool only reads the webcam or screen when started explicitly, and the board accepts pictures only from the computer it is plugged into.

## Decisions needed
1. Scope and order: LampArray first, then the Photo page (as phased above)?
2. Is the Windows Dynamic Lighting integration the main goal, or the tool-driven effects (which also work on Linux)?
3. Is the picture protocol over serial acceptable for the first round (the picture then needs the board on a cable)?
4. Names: `Lamps.java` and `Show.java`, or one tool?
5. Lamp layout: the bar only, bar plus tiles (tier B, recommended), or a coarse matrix (tier C) from the start? And is a physical WS2812 matrix on the Grove port of interest?
