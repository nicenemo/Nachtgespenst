# Nachtgespenst and the ZMK Cornish ZEN configuration

Nachtgespenst's keymap style and its header generator come from the owner's earlier ZMK work. Two repositories on the owner's GitHub are the sources: [`franken-cornish-zen`](https://github.com/nicenemo/franken-cornish-zen) (a ZMK configuration for the 42-key Corne-ish Zen: 31 layers, home-row mods, combos, Unicode, APL and Greek layers; MIT, 2024, based on a 2023 configuration by Darryl deHaan) and [`zmk_unicode_headers`](https://github.com/nicenemo/zmk_unicode_headers) (a Python generator for the header files used by urob's Unicode extension for ZMK).

Method and caveat: read on 2026-11-18 from the public GitHub pages (`layers.md`, `config/corneish_zen.keymap`, the folder listings, the README of the generator). The keymap and the headers were summarised by a web fetch, not copied, so details may be off in places; check the files before relying on a specific binding.

## What carried over

1. **The function-key pages are the same layout.** ZMK `FUN` holds F7 F8 F9 / F4 F5 F6 / F1 F2 F3 with F10 on a thumb key; Nachtgespenst's `F1-F10` page has the same rows with Esc, F10 and Enter underneath. ZMK `HFUN` (F11 to F24) puts F17 to F19 / F14 to F16 / F11 to F13 and F20 on a thumb, and F23 F24 / F21 F22 in one column; Nachtgespenst splits that into `F11-F20` (same rows) and `F21-F24` (the 2x2 block).
2. **Plain, Hyper and Meh families.** ZMK has `FUN`/`HFUN` (plain), `HPR FUN`/`HHPR FUN` (wrapped in `LH(...)`, Hyper) and `LMEH FUN`/`HLMEH FUN` (wrapped in `LM(...)`, Meh). Nachtgespenst has the same three families per page: `F1-F10`, `F1-F10 HYP`, `F1-F10 MEH`, and likewise for F11 to F20 and F21 to F24, written with `lhyp(...)` and `lmeh(...)`.
3. **Chord vocabulary.** ZMK defines one-word aliases in `keys/custom_modifiers.h` (`LHYPER`, `RHYPER`, `LMEH`, `RMEH`, and two-modifier chords such as `LCG`, `LCA`, `LAG`, `LSC`). Nachtgespenst's `Bindings.h` has the same set as masks (`M_LCG`, `M_LCA`, `M_LAG`, `M_LCS`, `M_LMEH`, `M_LHYP`, ...) and wrapper macros (`lcs`, `lmeh`, `lhyp`, ...).
4. **Key names by page.** ZMK uses prefixes by usage page: `K_` (keyboard), `KP_` (keypad), `C_` (consumer), `C_AL_` (application launch). Nachtgespenst generates `kp_`, `cp_` and `gp_` macros per HID usage page.
5. **Generated headers from a specification.** `zmk_unicode_headers` reads the Unicode character database, abbreviates block and character names, cleans them into valid C identifiers and writes one header per block with `UC_<BLOCK>_<NAME>` macros. `tools/GenerateHeaders.jsh` does the same job for the USB HID Usage Tables: one namespace and one shortcut header per usage page, cleaned names (`Usage.getCleanName()`), generated from JSON extracted from the specification. The language changed from Python to Java (JBang, FreeMarker templates, [ADR 0003](../adr/0003-jbang-pinned-jdk.md)).

## Layer mapping

| ZMK layer (`franken-cornish-zen`) | Nachtgespenst page | Notes |
|---|---|---|
| `FUN` F1-F10, `HFUN` F11-F24 | `F1-F10`, `F11-F20`, `F21-F24` | same key layout, see above |
| `HPR FUN`, `HHPR FUN` (`LH`) | `... HYP` pages | Hyper = Ctrl+Shift+Alt+Win |
| `LMEH FUN`, `HLMEH FUN` (`LM`) | `... MEH` pages | Meh = Ctrl+Shift+Alt |
| `KEYPAD` | `Numpad Full` | 7 8 9 / 4 5 6 / 1 2 3 / 0 with the operators |
| `NUM`, `SYM1`, `SYM2` | `Symbol Pad` | the numeric block of `SYM2` plus the shifted symbols of `SYM1` |
| `NAV` | `Edit/Nav` | arrows, Home, End, Page Up and Down, plus clipboard keys |
| `VOLUME`, `CONSUMER`, `POWER` | `Media` | transport, volume, mute, and power/sleep/wake |
| `APPL` (application launch) | `App Launch` | browser, file manager, mail, calendar, task manager, calculator |
| `DISPLAY` (brightness) | none | the brightness usages exist in the vocabulary, no page uses them yet |
| `MOUSE`, `CONFIG` | none | Bluetooth/transport choice is a firmware command (`PREFER`, `ONLY`) |
| `APL/BQN`, `LOGIC`, `MATH`, three Greek layers, four punctuation layers | none | these type Unicode through OS input modes; USB/BLE HID has no Unicode |
| `NGRAM`, `QWERTY` (base typing layers) | none | a macro pad has no typing layer |
| none | `Maven Tools`, `Office/Prod`, `System`, `Edit Keys`, `MS Teams`, `MS Outlook`, `PowerPoint`, `Browser Ctrl`, `Debug IDE`, `International`, `Language`, `Hard Locks`, `Locks` | application-specific pages and the Locks page are new |

## Differences in kind

- **Typing keyboard vs macro pad.** The ZMK configuration is built for typing: home-row mods (hold-taps with prior-idle and tapping terms), layer-taps on the thumbs, 10 combos that toggle special layers. Nachtgespenst has no timing-dependent behaviours; a tile is a key press, a chord, or a launcher.
- **Navigating layers.** ZMK uses `&lt`, `&tog` and combos; Nachtgespenst uses the three Core2 buttons or the CoreS3 bottom bar, plus the Locks page.
- **Feedback.** Nachtgespenst shows the layer colour on the LED bar, the host lock state on the Locks page, and screenshots/tests are scriptable over serial; the ZMK keymap has none of that.
- **Transports.** ZMK handles BLE and USB through its own stack; Nachtgespenst chooses per report with `PREFER` and `ONLY` (S3).

## Ideas this suggests (not planned)

- A brightness page from the `DISPLAY` layer idea (the usages are already generated).
- Unicode input on Linux (Ctrl+Shift+U) as a page of typed symbols, if ever wanted; it would need an input-mode setting per host OS, like the `unicode.dtsi` behaviours in ZMK.
- Reusing `zmk_unicode_headers` ideas for naming if Unicode pages are added.
