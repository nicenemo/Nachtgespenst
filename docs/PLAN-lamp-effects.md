# Plan: lamp positions, LED bar effects, the microphone demo and the C64 grid demos

**Status 2026-11-19: sections 1 to 4 are built and tested on the CoreS3** ([microphone-and-effects](wiki/microphone-and-effects.md), [demo-app](wiki/demo-app.md)); the Core2 is open.

Separate plan, written 2026-11-19 at the owner's request. Nothing here is built yet. It builds on the LampArray bench ([ADR 0010](adr/0010-lamparray-lighting-and-picture-pages.md), [wiki](wiki/lamparray-testing.md)) and the Disk demo ([ADR 0017](adr/0017-disk-drive-data-channels.md)).

## 1. Lamp positions: a logical grid with depth

Today (`src/Lamps.cpp`, `onGetFeature`): every lamp has Z = 0, the ten bar lamps sit on a line along X with a 12 mm pitch, and the screen grid starts 20 mm below it. That is not the device.

- **Screen lamps above the bar.** The screen is in front of the base, so its lamps get a higher Z than the bar lamps (bar Z = 0 at the base, screen at the height of the display glass). Hosts such as Windows Dynamic Lighting mostly use X and Y; Z orders the layers.
- **Screen lamps as a logical grid.** Lamp `10 + row * cols + col` sits at the centre of its cell in the active area of the panel (about 40.8 x 30.6 mm), so a grid of any size covers the panel exactly and a lamp's neighbours are its neighbours on the glass.
- **Bar lamps at their physical places.** The ten LEDs of the M5GO base are not one row in the middle. **Measured with the webcam on the CoreS3 base (2026-11-19, `LampTest barmap`):** one chain in the shape of a U around the bottom of the board. **Lamps 0 to 4 run down the right side (0 at the top, 4 at the bottom), lamps 5 to 9 run up the left side (5 at the bottom, 9 at the top).** Each side is a straight line about 60 pixels long in the picture with the lamps 12 pixels apart. Confirmed with the board upside down (screen down, right side to the camera): lamps 0 to 4 stand on a line again, now with lamp 0 to the left and lamp 4 to the right in the picture, the reverse of the first measurement, so lamp 0 is at the top of the right side and lamp 4 at the bottom, without relying on how the board was lying. The Core2 base is probably the same chain (not measured). The demo effects and the preview already use this mapping; the firmware attribute reports (positions) do not yet. Positions go into the attribute reports in micrometres, the bounding box follows from them, and the lamp order follows the LED index.
- **To do:** measure or find the LED coordinates for both bases (they may differ between the Core2 and the CoreS3 base), update `LAMP_ATTR` (bounding box) and `LAMP_RESP` (X, Y, Z), enumerate on Linux and check with the lamp pages that a gradient along X looks like a gradient on the device.

## 2. LED bar effects (autonomous mode)

The LampArray has an autonomous mode: while the host has not taken control, the device runs its own effects. That is the right home for them; a host that takes control still gets plain lamps.

- Effects: **Nightrider** (a scanner with a fading tail, left and right), **breath** (slow sine on the brightness), plus rainbow, colour wipe, comet, sparkle, fire and a VU meter (see 3).
- Selection: a setting (`SET effect nightrider|breath|...`, stored in NVS), a serial command and later the Settings page. The speed and the colour follow the layer colour or a setting.
- They run on the bar worker task (second core), so the screen and the main loop never slow them down ([latency numbers](wiki/lamparray-testing.md)).
- Test: the webcam (`LedCam`): a moving scanner shows up as a moving bright spot in the frames; breath as a brightness curve.

## 3. Microphone demo

- The board listens with `M5.Mic` (both boards have a microphone): level, and later a small FFT, drive the bar (VU meter, spectrum on the ten LEDs) and optionally the screen grid.
- **Test without a person:** the PC plays sound through its speaker from JBang (`javax.sound.sampled`, no Linux tools): a sine sweep, a 440 Hz tone with a rising level, a beat. The board reports the level it hears (`MIC?` over serial) and the test checks that the level follows the tone, that silence is silent and that the sweep moves across the spectrum.
- Limits: a room is noisy and speaker distance matters; the test compares relative levels (tone on against tone off), not absolute values.

## 4. C64-style demos on the lamp grid (decided and built, see [demo-app](wiki/demo-app.md))

Decisions of 2026-11-19: the effects run on the PC and send only lamp updates; the prime-modulo dissolve is `(i * p) mod m` with a rising threshold; the raster bars and the scroller use 40 x 25 with the 16 C64 colours; the photos are shown in full colour on 160 x 95 lamps (2 x 2 pixels each, a transition takes 1.9 s over USB); five photos from Wikimedia Commons. Still open from the original idea: the autonomous bar effects on the board and the microphone demo (sections 2 and 3).

Original notes:

Questions to settle first are in the reply to the owner; summary of the ideas:

- **Photo slideshow with a prime-modulo transition.** A dissolve from picture A to picture B where lamp `i` switches when `(i * p) mod m` falls below a rising threshold; with `p` coprime to `m` the order is a permutation of the lamps, a scattered but regular dissolve that needs no table, in the way of C64 demos. A few primes give different looks.
- **Raster bars (copper bars).** Horizontal coloured bands that move over the rows with a sine. A band is one colour for a whole row, so it is one range update per row: about 25 reports per frame, fast over USB.
- **Sine scroller.** A scrolling text in an 8 x 8 font whose columns are shifted up and down with a sine, on the grid.
- Palette: the 16 C64 colours. Grid: 40 x 25 (1,000 lamps, the C64 text screen; the data area of the Disk demo is 1,024 lamps) or 32 x 24.
- Where it runs: either the PC paints the lamps (the point of a LampArray; 40 x 25 is about 8 frames per second per full repaint over USB with multi updates, much faster with range updates) or the board runs the effect itself from pictures on the SD card.
