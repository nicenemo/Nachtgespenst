# Nog te doen (geparkeerd 2026-11-12, vervolg op docs/OVERNIGHT.md)

Hervat met: "ga verder met docs/TODO.md". Eerst controleren of de UI-agent klaar is (zie "UI icons + paging" in OVERNIGHT.md) en `git status` bekijken. Niet committen tenzij gevraagd. Builds duren ~12 min; geen twee `pio` builds tegelijk in dezelfde `.pio`.

## Status (2026-11-12, 19:40)
Gebouwd en geflasht op beide borden (`core2`, `cores3`); SerialTest: S3 12/12 PASS, C2 11/11 PASS. Alles staat nog ongecommit op `develop`.

Klaar:
- UI iconen + paging (Core2 knoppen A/B/C, S3 on-screen prev/next); screenshots in /tmp/claude-1000/shots3/.
- Locks-pagina + LED-bar code (`LedColors.h`: `lockColor`, `mixColors`, `ledBarLocks`; `setExtOutput(true)` in `ui.cpp`).
- Caps/Num/Scroll met standaardcodes 0x39/0x53/0x47 (`kp_CAPS_LOCK`, `kp_KP_NUM_LOCK_AND_CLEAR`, `kp_SCROLL_LOCK`). S3: KEY_SCROLLLOCK op host gezien.
- S3: USB + BLE tegelijk (`HIDManager.cpp`, `NG_TRANSPORT_USB`/`NG_TRANSPORT_BLE`; `setPreferUsb()` default aan: BLE wordt overgeslagen zolang USB verbonden is). S3 adverteert als `Nachtgespenst-S3` (bluetoothctl gezien).
- Geen blokkerende delays meer (loslaten via `millis()`, CDC-wacht met time-out). Scherm: dim na 60 s, uit na 5 min, tik wakker (`idleBacklight()` in `ui.cpp`). Serial `TAP`/`KEY` maken het scherm niet wakker.
- Core2 flasht weer (esptool 460800, chip esp32, bootloader 0x1000).

## Nog te doen
1. **Door gebruiker te testen**: Core2 LED-bar brandt (anders `tools/ledtest`, GPIO25 vs 15); echte aanraking op beide borden incl. wakker worden uit scherm-uit; S3 koppelen via BLE en controleren dat er bij USB+BLE geen dubbele toetsen komen; Caps/Num via BLE op Core2 (host-LED terugmelding).
2. Optioneel: `STATE?` splitsen in `usb=`/`ble=` (nu alleen `conn=`); serial-commando's het scherm laten wakker maken.
3. S3 `LED_PIN`/ledbar (nu NG_HAS_LEDBAR=0, pin 9 is een gok).
4. NimBLE `startServices()` deprecation warning.
5. SerialTest `--shot` op Core2 duurt ~20 s per pagina (27 pagina's > 500 s): ruime time-out gebruiken.

## Bekend / tips
- Flashen S3 (TinyUSB): `stty -F /dev/ttyACM0 1200; sleep 3`, daarna esptool zoals in OVERNIGHT.md. Core2: direct esptool op /dev/ttyUSB0.
- Testtool: `PATH=$HOME/jbinst/jbang/bin:$PATH jbang tools/SerialTest.java [--board S3|C2] [--shot dir]`.
- `KEY`/`TAP` op een toetsenbordpagina typt echt op de host; test op de Locks-pagina met Scroll Lock.
- Niet `pkill -f` met patronen uit je eigen commandoregel.
