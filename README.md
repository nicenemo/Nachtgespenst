# Nachtgespenst

Firmware for the M5Stack Core2 and CoreS3 that turns the touchscreen into a
Bluetooth LE (and, on the CoreS3, USB) HID macro pad. Each *layer* shows a grid
of keys on the display; tapping a tile sends the matching HID report to the host.

## Features

- Touchscreen UI built with LVGL 9: one tile grid per layer, plus a Locks page
- Keyboard and consumer-control bindings, including modifier chords
- BLE HID via NimBLE-Arduino; USB HID alongside BLE on the CoreS3
- Status LED feedback (FastLED) and backlight idle dimming
- Keymaps are written in C++, ZMK-style, using generated HID shortcut macros
  such as `kp_A`, `cp_MUTE`, `lcs(...)` and `lhyp(...)`

## Hardware

| Environment | Board            | Transports |
|-------------|------------------|------------|
| `core2`     | M5Stack Core2    | BLE        |
| `cores3`    | M5Stack CoreS3   | BLE + USB  |

## Building

Requires [PlatformIO](https://platformio.org/).

```sh
pio run -e core2        # or: pio run -e cores3
pio run -e core2 -t upload
pio device monitor      # 115200 baud, with exception decoder
```

A full build takes around 12 minutes per environment.

## Configuring the keymap

Layers are defined in [`include/Keymap.h`](include/Keymap.h) as an array of
`GridLayer` (title, bindings, rows, columns, LED colour). Edit it and rebuild.

## Regenerating the HID headers

The headers in `include/hid_pages/`, `include/hid_shortcuts/` and the top-level
`HID_*.h` files are generated from the USB HID Usage Tables. Do not edit them by
hand. To regenerate, run from inside `tools/`:

```sh
jbang GenerateHeaders.jsh
cp -r include/. ../include/
```

This needs [JBang](https://www.jbang.dev/) and a recent JDK; setup and Java upgrades are in [docs/wiki/tooling.md](docs/wiki/tooling.md).
The scripts are executable, so `./GenerateHeaders.jsh` works too.

The generator reads `tools/data/hut.json`, which is checked in. To update it from
a newer revision of the USB HID Usage Tables, edit the URL in `tools/FetchHut.jsh`
and run it from inside `tools/`:

```sh
jbang FetchHut.jsh
```

It downloads the specification PDF to `tools/data/hut_spec.pdf` (cached and
gitignored; delete it to force a re-download) and extracts the embedded JSON
to `tools/data/hut.json`. Then regenerate as above.

## Project layout

- `src/` — firmware (`main.cpp`, `ui.cpp`, `HIDManager.cpp`, `SerialCmd.cpp`, ...)
- `include/` — headers, keymap and generated HID vocabulary
- `tools/` — header generator and HUT fetcher, templates and a serial test client
- `docs/` — additional documentation

## Name

Nachtgespenst is inspired by Ben Aaronovitch's *Rivers of London* series, in
particular the novella *The October Man*, which is set in Germany. Its German
setting evokes long, nested sentences, officialdom and paperwork, and the
dubbed German television the author grew up with. The name is a nod to that
mix: a German word for a night ghost, for a small device that sits quietly
beside the keyboard and does its work on command. (The English audiobook is the
better way to experience the story.)

## Licence

MIT, see [LICENSE](LICENSE). Third-party components and their licences are
listed in [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md).
