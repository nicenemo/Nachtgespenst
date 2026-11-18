# 0009 Unicode pages, host OS awareness and a settings page

Status: Proposed. Nothing is implemented; the decisions at the end are the owner's.

## Context
What a macro pad adds to a physical keyboard is the screen: keycap colours and legends in software (see [zmk-comparison](../wiki/zmk-comparison.md)). The same screen can offer characters that are awkward to type: dashes of several lengths, the interrobang, typographic quotes, maths, arrows, currency. Wanted, in the owner's words: the interrobang and its relatives, the em dash family up to the three-em dash, a way to switch the Unicode method between macOS, Windows and Linux, automatic detection of the host if that is possible over USB and BLE (one of the two alone is acceptable) with an override in any case, and perhaps emoji (judged less useful).

Inspiration: the owner's ZMK configuration (`franken-cornish-zen`: DISPLAY, APPL, MATH, LOGIC, Greek and punctuation layers, a Unicode leader system), the Unicode header generator `zmk_unicode_headers`, QMK (Unicode modes, OS detection), ZMK's Unicode extension, X11/Wayland Compose sequences, WinCompose and macOS's Option-key layers. The method per operating system and the detection signals are in [unicode-input-methods](../wiki/unicode-input-methods.md).

## Proposed pages
Grids follow the existing 3x3, 4x3, 5x3 and 6x4 sizes. Code points in brackets are the ones certain to be wanted; the rest is a proposal.

| Group | Page | Contents (examples) |
|---|---|---|
| Typography | Dashes and spaces | en dash U+2013, em dash U+2014, two-em dash U+2E3A, three-em dash U+2E3B, figure dash U+2012, horizontal bar U+2015, minus U+2212, non-breaking hyphen U+2011, no-break space U+00A0, narrow no-break space U+202F, thin space U+2009, zero-width space U+200B, soft hyphen U+00AD |
| Typography | Quotes and marks | curly quotes U+201C U+201D U+2018 U+2019, low quotes U+201E U+201A, guillemets U+00AB U+00BB U+2039 U+203A, ellipsis U+2026, bullet U+2022, middle dot U+00B7, dagger U+2020, double dagger U+2021, section U+00A7, pilcrow U+00B6, primes U+2032 U+2033 |
| Typography | Rare punctuation | interrobang U+203D, inverted interrobang U+2E18, irony mark (reversed question mark) U+2E2E, double question U+2047, question-exclamation U+2048, exclamation-question U+2049, inverted ! and ? U+00A1 U+00BF, reference mark U+203B, asterism U+2042 |
| Symbols | Maths and logic | plus-minus, times, divide, not equal, approximately, at most, at least, infinity, square root, sum, product, integral, partial, nabla, element of, intersection, union, subset, and, or, not, implies, equivalent, for all, exists, empty set |
| Symbols | Greek | alpha to omega in lower and upper case, as in the ZMK `GR_MODERN` layer |
| Symbols | Arrows and key glyphs | arrows U+2190 to U+2195, double arrows, return arrows, and the Mac key glyphs command U+2318, option U+2325, shift U+21E7, control U+2303, delete U+232B, escape U+238B, tab U+21E5 |
| Symbols | Currency and units | euro U+20AC, pound, yen, cent, rupee, bitcoin, rouble, won; degree, micro, ohm, per mille, numero, vulgar fractions, superscript 2 and 3 |
| Symbols | Marks and status | check marks U+2713 U+2714, crosses U+2717 U+2718, stars, ballot boxes U+2610 to U+2612, warning, information, heart, note |
| Symbols | Sub- and superscripts | the digits as superscripts and subscripts (H₂O, x²) |
| Letters | European letters | e acute and grave, e and i with diaeresis, o and u umlaut, a umlaut, sharp s, c cedilla, n tilde, oe, ae, o slash, a ring, the Dutch ij ligature U+0133 |
| Device | Display | host brightness down and up (the consumer usages are already generated), and the device's own backlight and idle dimming |
| Device | Settings | host OS per transport (Auto, Windows, macOS, Linux), output routing (`PREFER` and `ONLY`), device brightness, LED bar on or off, theme dark or light, a "test character" tile |
| Device | Hex entry | 0 to 9, A to F, Enter, Backspace: type any code point with the active method |
| Host | Window management | OS-aware chords: snap and switch windows, show the desktop, lock the screen, mission control; the same tile sends the right chord for the detected OS |
| Optional, last | Work emoji | check, cross, warning, thumbs up, folded hands, party, rocket, fire, eyes, light bulb; all but the first three need characters above U+FFFF |

Two cross-cutting extensions follow from knowing the host: **OS-aware modifiers** (Ctrl on Windows and Linux becomes Cmd on macOS in the editing, browser and PowerPoint pages) and a small OS badge in the header ("LIN", "MAC", "WIN", with "auto" or "set").

## Design
1. **Sequence engine in `HIDManager`.** A Unicode character is a short script: modifier down, keys with 10 ms spacing, finishing key, modifier up. The engine runs in the existing non-blocking pump, per transport, so the OS of the transport that receives the report decides the sequence. It must keep the press queue's guarantees (no blocking, release retry).
2. **Binding kinds.** `Binding` becomes one of: HID usage (today), Unicode code point(s) with a label, and a local action (settings tiles, brightness, OS selection, routing). Actions must not be sent to the host.
3. **Generated names.** `tools/GenerateUnicode.java` (JBang, [ADR 0008](0008-jbang-for-project-tooling.md)) builds `uc_...` macros and labels from the JDK's own Unicode data (`Character.getName`), so no download is needed, following the idea of `zmk_unicode_headers` and of `GenerateHeaders.jsh`.
4. **Legends need glyphs.** The LVGL fonts in use (Montserrat 12 to 32 pt) do not contain `‽`, `⸻`, most maths or arrows. A static bitmap font made with `lv_font_conv` from a font that has the glyphs (for example Noto Sans, Noto Sans Math and Noto Sans Symbols 2) is needed, which fits the bitmap-font step planned for speed in [ADR 0007](0007-page-switching-speed-and-prerendering.md): one font build for icons and Unicode legends. A few hundred glyphs at two sizes is roughly 100 to 200 kB of flash (estimate; flash is 35 % used). The classic UI would need its own glyph source or a text fallback (a name such as "3em" under a drawn dash).
5. **Navigation.** There are now 27 pages; about 15 more make paging with next and previous tedious. Proposal: a menu page of groups (Keys, Applications, Typography, Symbols, Device) reached with the Core2's B button or the CoreS3's title tap (today they open the Locks page, which moves into the Device group).
6. **Tests.** `SerialTest` gets `OS` commands and checks, per OS mode, that the right key events reach the host input device for a known code point (the typed character itself cannot be verified automatically; a manual check in a text field stays). The webcam is not needed.

## Host OS: detection and override
Per transport, because the CoreS3 can be connected to a computer over USB and to another over BLE at the same time: `os_usb` and `os_ble`, each Auto, Windows, macOS or Linux, remembered in flash (NVS), and for BLE remembered per bonded peer address.

Order of precedence: manual override, then the remembered value for this peer, then detection, then a configurable default. A wrong guess is then one tap on the Settings page, and an override survives reboots. Serial: `OS?` and `OS USB|BLE AUTO|WIN|MAC|LINUX`.

Detection, honest about what is known:
- **USB (CoreS3).** QMK shows that Linux, Windows, macOS and iOS can be told apart from the USB setup behaviour, with caveats (best guess, KVM switches, late results on Apple Silicon). Here the callbacks for descriptor and HID class requests can be observed (override or link-time wrap), but not the packet's `wLength` that QMK uses, so the signals available are fewer: the Windows-only string descriptor 0xEE request, the BOS request, `SET_IDLE` and `SET_PROTOCOL`. Whether that separates Linux from macOS must be measured.
- **BLE.** Android: documented and reliable on Android 14 and newer (read the Android Information Service from the host with `NimBLEServer::getClient`). Apple: the Device Information Service says "Apple Inc.", reported by others, to be verified; macOS versus iOS may not be separable, and iOS has no Unicode method anyway. Windows versus Linux: no known signal; the negotiated MTU, connection parameters and GATT read order are candidates.
- **Experiment first.** A `HOST?` command that prints every signal seen per connection, then connecting each available host (Linux, Windows, macOS, Android, iOS and iPadOS, all available) tells which signals are usable. For Apple hosts the complete Device Information Service (manufacturer, model, software revision) should be read: it might separate iPhone, iPad and Mac. Only then is a classifier written. Until then BLE hosts other than Android are "unsure" and use the remembered or default OS.

## Phasing
1. Foundation: sequence engine, Unicode and action bindings, OS state with override (serial and Settings page), `GenerateUnicode.java`, bitmap font with the first glyphs, menu page. Tests per OS mode.
2. Typography pages (dashes and spaces, quotes and marks, rare punctuation): the wanted set.
3. Symbols pages (maths, Greek, arrows, currency and units, marks, sub- and superscripts, European letters) and the hex entry pad.
4. Display and Settings pages; OS-aware modifiers and the window-management page.
5. Detection experiments (`HOST?`), then a classifier for the signals that prove usable.
6. Optional: work emoji.

## Risks and open questions
- Every method needs host set-up (WinCompose or the registry value on Windows, "Unicode Hex Input" on macOS, IBus on Linux); the macro pad cannot do it and must document it. Some applications ignore the methods (terminals, games, remote desktops).
- The letters A to F in the hex digits assume a QWERTY-like layout on the host.
- Windows has several incompatible methods (HexNumpad stops at U+FFFF, WinCompose is a third-party tool, Alt codes cover Windows-1252 only); they are selectable (decision 2).
- Flash and RAM for the glyph font; the speed work in 0007 should come first or together, because more pages make slow page building more noticeable.
- Detection can be wrong; that is why override and persistence come first.

## Decisions
Answered by the owner on 2026-11-18:
1. **Hosts available for experiments:** Linux, Windows, Android, iOS, macOS and iPadOS.
2. **Unicode mode is selectable**, as in the owner's ZMK configuration, which has the tiles `UC_SET_MACOS`, `UC_SET_WIN_ALT`, `UC_SET_WIN_COMPOSE`, `UC_SET_LINUX`, `UC_SET_LINUX_ALT` and `UC_SET_EMACS` on its CONFIG layer (plus the plain chords `LUC` = Ctrl+Shift+U and `WUC` = Alt+X for Word). The same modes, with the same names, become tiles on the Settings page. What `LINUX_ALT` does exactly is not visible in the keymap (it comes from the unicode extension); it is to be taken over from there.
3. **Default when detection is unsure: Linux**, overruled by the user. The interface shows what was detected and what was chosen (see "UX for detection and choice").

Still open:
4. Navigation: a menu page of groups as proposed, or keep next and previous paging? (Suggestion: the menu page.)
5. Font: a static bitmap font with the glyph set, and a text fallback in the classic UI? (Suggestion: yes, together with the speed work of 0007.)
6. Order of the phases and whether the emoji page is wanted at all. (Suggestion: the order above, emoji last and optional.)

## More hosts, and what "host" means
Beyond the six hosts the owner has, the families that matter for the choice list:

| Family | Members | Unicode method |
|---|---|---|
| Linux-like | Linux (X11 and Wayland, GNOME, KDE), Steam Deck, Raspberry Pi OS, ChromeOS (believed to accept Ctrl+Shift+U, to verify), BSDs with IBus | Ctrl+Shift+U; the Linux console and some terminals have none |
| Windows | Windows 10 and 11, also inside a VM or a remote desktop session | WinCompose, HexNumpad, Alt codes (Windows-1252 only), Alt+X in Word |
| Apple desktop | macOS | Unicode Hex Input, or Option combinations for a subset |
| Apple mobile | iOS, iPadOS (visionOS and tvOS probably alike, unverified) | no hex input; Option combinations on the US layout (to verify) |
| Android-like | Android, Fire OS, Android TV, Samsung DeX | none |
| Other | smart TVs, game consoles, BIOS/UEFI and KVM menus | basic keys only |

The choice list is therefore: Auto, Linux, Windows, macOS, iPadOS, iOS, Android, ChromeOS, Other. "Other" means no Unicode and no OS-specific chords.

**Guest versus host.** The operating system that interprets the keys may be a guest the keyboard cannot see: Windows in a virtual machine or a remote desktop session opened from a Linux or Mac computer, a Linux VM on a Mac, a Citrix session. Detection sees only the machine the keyboard is physically attached to. The override must therefore be one tap away, not buried in a settings page.

## Fast paths per OS
Besides the generic hex method, some hosts have direct key combinations for common characters that need no host set-up. These matter because iOS and iPadOS have no hex input at all.
- **Apple (macOS, iOS, iPadOS, US/ABC layout):** Option and Option+Shift combinations type many of the wanted characters directly (en dash, em dash, ellipsis, curly quotes, plus-minus, not equal, at most, at least, divide, degree, euro, pound, copyright, trademark). Believed from the Mac layout and to be verified on the owner's iPad and iPhone.
- **Windows:** Alt codes on the numeric keypad for the Windows-1252 range (en dash Alt+0150, em dash Alt+0151, ellipsis Alt+0133, curly quotes Alt+0147, 0148, 0145 and 0146, euro Alt+0128): no registry change, but the keypad keys only work with NumLock on, which the keyboard knows from the host's LED report (and could correct).
- **Linux:** Compose sequences, only if the user has configured a Compose key; not a default.

Binding model: a Unicode tile carries the code point plus an optional per-OS fast path. A tile whose character has no method for the effective OS is shown dimmed (for example the interrobang on iPadOS or Android) and says why on the Settings page, instead of sending nothing silently.

## UX for detection and choice
- **Settings page, per transport (USB and BLE):** a line "Detected: Linux (USB, string request seen)" or "Detected: not sure", a row of tiles "Chosen: Auto, Linux, Windows, macOS, iPadOS, iOS, Android, ChromeOS, Other", and the Unicode mode tiles of decision 2.
- **Effective OS:** the manual choice if there is one, else the value remembered for this BLE peer, else a confident detection, else Linux (decision 3).
- **Header badge on every page:** the effective OS in three styles: detected (auto), chosen (set) and default (unsure). Tapping the badge opens the OS tile row, so a wrong guess, or a guest OS, is corrected in one tap on any page.
- Serial: `OS?` reports detected, chosen and effective OS per transport; `OS USB|BLE AUTO|LINUX|WINDOWS|MACOS|IPADOS|IOS|ANDROID|CHROMEOS|OTHER` sets the choice.
