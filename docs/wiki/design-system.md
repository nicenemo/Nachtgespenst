# The console style

One look for the demo window (`tools/DemoApp.java`, Swing) and the screens of the boards (`src/ui.cpp`, LVGL): a black background with rounded blocks in four colours, Geist Sans for buttons and headings and Geist Mono for live data. The style is original; it takes no names, logos or artwork from any film or series. Code: `tools/ConsoleTheme.java`, `tools/ConsoleUi.java`, `include/ConsoleStyle.h`, `src/ConsoleStyle.cpp`.

## Colours

| Role | Colour | Used for |
|---|---|---|
| Background | `#000000` | everything; blocks are separated by black gaps |
| Primary | amber `#FF9900` | the elbow, the title block, active and **pressed** blocks |
| Secondary | peach `#FFCC99` | information bars, live data, the Core2 button indicators |
| Tertiary | lavender `#9999FF` | navigation: Connect, the page number, the nav buttons of the CoreS3 |
| Alert | coral `#CC6666` | errors and a failed check (SHA-256 different) |
| Off | `#2A2A44` (device), `#1B1208` (window) | a block that is switched off |

Text on a coloured block is black. The colours of the keys on the device (`Theme.cpp`: media blue, consumer red, locks green, modifiers) stay: they say what a key does. Only the chrome (header, pressed state, Locks and Disk pages) uses the four colours.

## Sizes

| | Demo window | M5Stack (320 x 240) |
|---|---|---|
| Gap between blocks | 6 px | 3 px |
| Padding in a block | 10 px | 2 to 8 px |
| Block corner radius | 14 px | 10 px |
| Bar height | 56 px (header), 30 px (status bar) | 24 px (Core2, top), 38 px (CoreS3, bottom) |
| Elbow (outer / inner radius) | `ConsoleUi.elbow` exists (56 / 18 px) but the demo window does not use it | none: the bar is a row of pills, LVGL only has one radius per object |
| Text in blocks | bottom right | bottom right (title blocks), centred on the key tiles |
| Sans (Geist Bold) | 14 px buttons, 13 px bands | 12, 16, 20, 28 px |
| Mono (Geist Mono) | 13 px log and status | 12 px |

No upper case text and no condensed fonts: labels are sentence case (`Caps`, `Num`, `Scroll`, `On`, `Off`).

## Fonts

* Desktop: `tools/fonts/Geist-Bold.ttf`, `Geist-Regular.ttf` and `GeistMono-Regular.ttf` are loaded by `ConsoleTheme` from the fonts folder; if a file is missing the window falls back to `SansSerif` and `Monospaced`.
* Device: `tools/fonts/gen_fonts.py` embeds Geist Bold and Geist Mono as C arrays (`src/geist_font_data.c`); `Console::begin()` makes one Tiny TTF font per size. Missing glyphs fall back to the Montserrat of the same size (`lv_font_t::fallback`), the icons come from the Material Icons subset (`Icons.cpp`).
* Licence: Geist is SIL OFL 1.1 (`tools/fonts/OFL.txt`, `THIRD-PARTY-NOTICES.md`). The embedded copies are unmodified.

### Bitmap fonts instead of Tiny TTF

Tiny TTF draws from the outline at run time, which costs time on a page build. If `perf` shows the pages getting slower, convert the sizes to bitmap fonts with `lv_font_conv` (`npm i -g lv_font_conv`). 4 bits per pixel is enough on this panel; the ranges are ASCII plus Latin-1 and the arrows and punctuation the keys use:

```
lv_font_conv --font tools/fonts/Geist-Bold.ttf -r 0x20-0x7E,0xA0-0xFF,0x2018-0x201D,0x2190-0x2193 --size 12 --bpp 4 --no-compress --format lvgl --lv-font-name geist_sans_12 -o src/geist_sans_12.c
lv_font_conv --font tools/fonts/Geist-Bold.ttf -r 0x20-0x7E,0xA0-0xFF,0x2018-0x201D,0x2190-0x2193 --size 16 --bpp 4 --no-compress --format lvgl --lv-font-name geist_sans_16 -o src/geist_sans_16.c
lv_font_conv --font tools/fonts/Geist-Bold.ttf -r 0x20-0x7E,0xA0-0xFF,0x2018-0x201D,0x2190-0x2193 --size 20 --bpp 4 --no-compress --format lvgl --lv-font-name geist_sans_20 -o src/geist_sans_20.c
lv_font_conv --font tools/fonts/Geist-Bold.ttf -r 0x20-0x7E,0xA0-0xFF,0x2018-0x201D,0x2190-0x2193 --size 28 --bpp 4 --no-compress --format lvgl --lv-font-name geist_sans_28 -o src/geist_sans_28.c
lv_font_conv --font tools/fonts/GeistMono-Regular.ttf -r 0x20-0x7E,0xA0-0xFF --size 12 --bpp 4 --no-compress --format lvgl --lv-font-name geist_mono_12 -o src/geist_mono_12.c
lv_font_conv --font tools/fonts/GeistMono-Regular.ttf -r 0x20-0x7E,0xA0-0xFF --size 14 --bpp 4 --no-compress --format lvgl --lv-font-name geist_mono_14 -o src/geist_mono_14.c
```

Add `LV_FONT_DECLARE(geist_sans_16);` (and the others) to `ConsoleStyle.h`, and return `&geist_sans_16` and so on from `Console::sans()` and `Console::mono()` instead of the Tiny TTF fonts. The converted fonts are derivatives of Geist and stay under the OFL (keep the name Geist out of any font you modify and sell). Each 4 bpp size costs about 10 to 25 kB of flash.

## Pressed, focused and held

* A block that is pressed turns amber with black text (`Console::block`, state `LV_STATE_PRESSED`): the keys, the nav buttons and the title block of the CoreS3.
* Core2: the header ends in three peach indicators A, B, C. They turn amber while the capacitive button below the screen is held, so you see which button the board saw.
* Focus (`LV_STATE_FOCUS_KEY`) is a peach ring of 2 px; nothing on the boards takes keyboard focus yet, the style is ready for a rotary or key input.
* Tests: `down <x> <y>` holds a touch, `up` releases it without a click, `btn <a|b|c> <down|up>` holds a Core2 button. `SerialTest` compares screenshots (tile amber while held, back to its colour afterwards, no key sent, indicator B amber).
