# Nog te doen (geparkeerd 2026-11-12, vervolg op docs/OVERNIGHT.md)

Hervat met: "ga verder met docs/TODO.md". Eerst controleren of de UI-agent klaar is (zie "UI icons + paging" in OVERNIGHT.md) en `git status` bekijken. Niet committen tenzij gevraagd. Builds duren ~12 min; geen twee `pio` builds tegelijk in dezelfde `.pio`.

## Status bij parkeren (07:00)
UI-agent en builds zijn met opzet afgebroken (laptop ging dicht). De code voor iconen + paging staat op schijf maar is NIET volledig gebouwd/getest: eerst `pio run -e core2` en `-e cores3` (via een scriptbestand + `setsid nohup`), dan flashen en `SerialTest.java --shot` draaien. Core2 draait mogelijk nog de LED-testfirmware (`tools/ledtest`) of oude firmware.

## Volgorde
1. **UI icons + paging afronden** (agent was bezig): iconen op Office/Prod, Edit/Nav, System, F-toetsen met modifiers, symbol pad e.a.; Core2 gebruikt de capacitieve knoppen onderaan (A = vorige, B = Locks, C = volgende); CoreS3 krijgt grote on-screen prev/next. Screenshots in /tmp/claude-1000/shots2/ bekijken.
2. **Core2 LED-bar + Locks-pagina** (onderzoek klaar, zie "Core2 LED bar" in OVERNIGHT.md):
   - `M5.Power.setExtOutput(true)` na `M5.begin()`, vóór `FastLED.addLeds`; 10 SK6812 op GPIO25 (oudere M5GO-voet: GPIO15), GRB, helderheid <= 40.
   - Eerst door de gebruiker laten bevestigen met `tools/ledtest/` dat de LED's branden (kan overschreven zijn door een latere flash).
   - Caps oranje (255,80,0), Num groen (0,220,60), Scroll blauw (40,80,255). Bar in segmenten 4/3/3; actieve locks vullen hun segment; meerdere actief = kleuren optellen (`qadd8`). Tegelachtergrond per lock in eigen kleur, gedimd als uit.
   - API in `LedColors.h`: `lockColor(LockId)`, `mixColors(const CRGB*, n)`, `ledBarLocks(leds, activeMask)` (caps=1, num=2, scroll=4).
2b. **Caps/Num/Scroll Lock echt laten werken** met de gangbare keyboard-page-codes (page 0x07): Caps Lock 0x39, Scroll Lock 0x47, Num Lock 0x53 ("Keypad Num Lock and Clear"). NIET de oude "Locking Caps/Num/Scroll Lock" (0x82/0x83/0x84), die hosts nauwelijks ondersteunen. Controleren in `ui_press` (`kp_CAPS_LOCK`, `kp_KP_NUM_LOCK_AND_CLEAR`, `kp_SCROLL_LOCK`) en in `Keymap.h` dat de generator niet per ongeluk een locking-variant of een `_<idHex>`-hernoemde naam oplevert; ook dat de HID-rapportmap (`HID_Specs.h`) 0x39/0x47/0x53 doorlaat. Testen op beide transports: toets komt aan (evdev `KEY_CAPSLOCK`/`KEY_NUMLOCK`/`KEY_SCROLLLOCK`), host-LED-toestand komt terug via het output-report en de tegels volgen die. Let op: Num Lock 0x53 werkt alleen als de host de keypad-semantiek toepast; check in `evtest` dat `KEY_NUMLOCK` verschijnt.
3. **S3: USB en BLE tegelijk** (`HIDManager`): twee transports naast elkaar, toets naar alle verbonden transports, optie om één kanaal voor te trekken (bv. alleen USB zolang USB verbonden) tegen dubbele toetsaanslagen. S3 adverteert als `Nachtgespenst-S3`. Core2 blijft BLE-only. Testen: `bluetoothctl` ziet de S3 en USB blijft werken; BLE-naam-check in `tools/SerialTest.java`. Koppelen met de laptop doet de gebruiker zelf (T450s heeft nu een USB-BT-dongle).
4. **Geen blokkerende delays meer**:
   - `HIDManager.cpp`: `delay(RELEASE_DELAY)` (4x) vervangen door loslaten via `millis()`-planning in `handleAsyncEvents()` (ook voor USB).
   - `main.cpp`: `delay(1500)` bij opstarten vervangen door wachten op USB-CDC-verbinding met time-out; `delay(5)` in `loop()` vervangen.
   - Geen deep sleep (herstart chip, scherm knippert, BLE/USB opnieuw opzetten). Wel: BLE modem sleep en scherm dimmen/backlight uit na inactiviteit, tik zet weer aan zonder herinit.
5. **Overig**: S3 `LED_PIN`/ledbar (nu NG_HAS_LEDBAR=0, pin 9 is een gok); echte aanraking testen op beide borden; NimBLE `startServices()` deprecation warning; `docs/OVERNIGHT.md` Core2-regels bij stap 2 bijwerken (Core2 flasht weer: esptool 460800, chip esp32, bootloader 0x1000, SerialTest C2 = 12/12 PASS).

## Bekend / tips
- Flashen S3 (TinyUSB): `stty -F /dev/ttyACM0 1200; sleep 3`, daarna esptool zoals in OVERNIGHT.md. Core2: direct esptool op /dev/ttyUSB0.
- Testtool: `PATH=$HOME/jbinst/jbang/bin:$PATH jbang tools/SerialTest.java [--board S3|C2] [--shot dir]`.
- `KEY`/`TAP` op een toetsenbordpagina typt echt op de host; test op de Locks-pagina met Scroll Lock.
- Niet `pkill -f` met patronen uit je eigen commandoregel.
