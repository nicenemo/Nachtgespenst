# 0014 A Settings page, switching the LEDs off, and persistent settings

Status: Proposed. Nothing is implemented. This is the first, small step of the Settings page that [0009](0009-unicode-pages-and-host-os-awareness.md) plans for its fourth phase; the Unicode and host OS settings join it later.

## Context
The LED bar cannot be switched off: the Locks page lights a segment per active lock and otherwise shows faint white (6, 6, 6), every layer page shows its colour at brightness 40 of 255, and the bar keeps its 5 V supply on (`M5.Power.setExtOutput(true)`). The backlight has a fixed schedule (dim after 60 s, off after 5 min). Routing (`PREFER`, `ONLY`) and brightness can only be changed with serial commands, which do not survive a reset. The owner asked for the LEDs to be switchable and asked whether a Settings page is wanted.

## Proposal
**A Settings page** after the Locks page (and before the LampArray test pages), built from large rows that can be changed with one tap each (cycle through the values), in LVGL like the other pages:

| Row | Values | Effect |
|---|---|---|
| LED bar | Off, Locks only, Layer colours | Off: all ten LEDs dark and the 5 V rail of the M-Bus off; Locks only: the bar shows only the host lock state, dark otherwise; Layer colours: today |
| LED brightness | 10, 25, 40 (default), 80, 120 of 255 | FastLED global brightness; 120 is the cap of the LampArray test bench |
| LED idle off | Never, 1 min, 5 min | like the backlight: dark after no touch, back on at the next touch |
| Screen brightness | 4 levels | the active backlight level (today fixed at 128) |
| Screen dim and off | 30 s / 1 min / 5 min / never | the two timeouts of `idleBacklight()` |
| Routing (CoreS3) | USB, Bluetooth, Both | `HIDManager::Prefer` |

Later rows: host OS and Unicode mode ([0009](0009-unicode-pages-and-host-os-awareness.md)), and the battery and device status of [0011](0011-performance-monitor-and-device-status.md).

**Persistence.** The values are stored in NVS (the `Preferences` library, one namespace), written when changed and read in `setup()`, so they survive a reset and a firmware update that keeps the NVS partition. Defaults are today's behaviour, so an unconfigured board looks exactly as it does now.

**Serial.** `LEDS OFF|LOCKS|ON` (and `LEDS?`), `SET <key> <value>` and `SET?` for the other rows, so the tests can set and check them without touching the screen, and `SerialTest` can verify that a setting survives a reboot (a `RESET` command).

**LED off and the LampArray.** "Off" is a master switch: the bar stays dark on every page, including the host lamps, so the owner's choice is predictable. The lamp test pages of [0010](0010-lamparray-lighting-and-picture-pages.md) show a notice when the bar is off, and `LampTest.java` sets `LEDS ON` first.

## Tests
- `SerialTest`: `LEDS OFF` then `LEDS?` reports `off`, a settings round trip, a reboot keeps the value.
- `LedCam`: with `LEDS OFF` the camera sees no glow on the Locks page and on a layer page (the existing `colours` run as a dark baseline), with `LEDS ON` the colours return.
- Screenshot of the Settings page on both boards for the site.
- Current draw is not measured here; with the 5 V rail off the base should draw nothing, which a USB power meter can show.

## Risks
- Cutting the M-Bus 5 V rail also cuts anything else on the bus; with only the LED base attached this is fine, and the setting says "LEDs" so the owner knows.
- Switching the rail back on needs a short delay before the first `FastLED.show()` (the LEDs start from reset).
- Too many rows for the Core2's 216 px body; the page uses two columns or scrolls, to be decided when it is built.

## Decisions needed
1. A Settings page now (rows as above), or only the `LEDS` serial command first?
2. Is cutting the 5 V rail with the LED bar off acceptable?
3. Which rows besides the LED and screen rows do you want first?
