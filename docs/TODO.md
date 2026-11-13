# Nog te doen (geparkeerd 2026-11-13, vervolg op docs/OVERNIGHT.md)

Hervat met: "ga verder met docs/TODO.md". Eerst `git status` bekijken. Niet committen tenzij gevraagd. Builds duren ~12 min (cores3 dan core2, zie "Bekend / tips"); geen twee `pio` builds tegelijk in dezelfde `.pio`.

## Klaar (gebouwd, geflasht, getest op beide borden)
- [x] UI iconen + paging (Core2 knoppen A/B/C, S3 on-screen prev/next).
- [x] Locks-pagina + LED-bar code (`LedColors.h`, `setExtOutput(true)`); LED-bar zelf nog niet visueel bevestigd (zie Open).
- [x] Caps/Num/Scroll met standaardcodes 0x39/0x53/0x47; host-LED komt terug (S3 USB en Core2 BLE).
- [x] S3: USB + BLE tegelijk, `setPreferUsb()` default aan. BLE koppelen met S3 (hci0) en Core2 (dongle hci1) werkt; geen dubbele toetsen bij USB+BLE.
- [x] Geen blokkerende delays; scherm dimt na 60 s, uit na 5 min; wakker-tik wordt ingeslikt (drukt geen tegel).
- [x] Code review (embedded) en fixes: persistente non-blocking press-queue met release-retry (`HIDManager.cpp`, queue 16, 1 s deadline), USB-connected = `tud_mounted()`, BLE-security zonder MITM + peer-adres in log, atomics i.p.v. LED-queue, USB LED-report op report-id, loop slaapt 1..10 ms (`ui_loop()` geeft ms terug), `toupper((unsigned char))`.
- [x] Tests: S3 SerialTest 12/12, C2 11/11; stress 50x (S3 USB) en 30x (C2 BLE) alle toetsen aangekomen (down+up).

## Open
1. **Door gebruiker te testen**: Core2 LED-bar brandt (anders `tools/ledtest`, GPIO25 vs 15); echte aanraking op beide borden incl. wakker worden uit scherm-uit (eerste tik mag geen toets sturen).
2. S3 `LED_PIN`/ledbar (nu NG_HAS_LEDBAR=0, pin 9 is een gok) = niet werkende LED-bar op CoreS3.
3. Review-punten nog niet gedaan:
   - **System-control toetsen** (`gp_SYS_POWER_DOWN`, Keymap.h:52) worden stil genegeerd: System report (ID 3) toevoegen aan `HID_Specs.h::GetReportMap` + `send()`, of compile-time afkeuren.
   - Display double buffer nutteloos (synchrone `pushImage`): `pushImageDMA`+`waitDMA` of 1 buffer.
   - `KEY`/`TAP` serial-commando's (typen echt op host) achter een build-flag/debug-env; seriele activiteit laat scherm niet wakker.
   - Build reproduceerbaarheid: `platform` pinnen, `lib_ldf_mode = chain+` met expliciete `lib_deps` (bouwtijd), `-I .../FS/src` workaround, `__DATE__/__TIME__` vervangen door git-versie.
   - BLE: alleen gebonden peers accepteren / pairing-venster met knop (een vreemde central kan de enige verbinding bezetten).
   - Kleinigheden: magische LED-bits (0x01/0x02/0x04) benoemen (nu deels `LED_*` in HIDManager.cpp, nog 0x02/0x01/0x04 in `ui.cpp`), `LAYER_COLORS` als `inline constexpr` + dubbelen weg, report map als constexpr array, NL/EN comments.
4. Onverklaard: eenmalig meldde Core2 `conn=1` terwijl de host geen verbinding zag (reset hielp). Niet reproduceerbaar; bij herhaling peer-adres uit de seriele log (`[HID] BLE verbonden met ...`) bekijken.
5. Optioneel: `STATE?` splitsen in `usb=`/`ble=` (nu `conn=`); NimBLE `startServices()` deprecation warning; Office/Prod Hyper-iconen zijn afgeleid uit de letters (controleren in `Icons.cpp`).

## Bekend / tips
- Flashen S3 (TinyUSB): `stty -F /dev/ttyACM0 1200; sleep 3`, daarna esptool zoals in OVERNIGHT.md. Core2: direct esptool op /dev/ttyUSB0.
- Testtool: `PATH=$HOME/jbinst/jbang/bin:$PATH jbang tools/SerialTest.java [--board S3|C2] [--shot dir]`.
- `KEY`/`TAP` op een toetsenbordpagina typt echt op de host; test op de Locks-pagina met Scroll Lock.
- Test: evdev-teller leest ALTIJD tijdens het sturen (kernel-buffer ~64 events loopt anders over en geeft schijnbaar verloren toetsen).
- Core2 reset zonder knop: RTS-puls op /dev/ttyUSB0. Core2 BLE-bond zit op de USB-dongle (hci1, 34:02:86:6E:1A:EE), S3 op hci0.
- Niet `pkill -f` met patronen uit je eigen commandoregel.
