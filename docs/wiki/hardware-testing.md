# Flashing and testing the boards

Run every command in this page as a single plain command: [claude-code-workflow](claude-code-workflow.md).

## 1. Build

```sh
.pio/venv/bin/pio run -e cores3     # then core2, never both at once
```

Takes 11 to 13 minutes; start it in the background. The result is `.pio/build/<env>/firmware.bin`. Build from the branch you are on and do not edit `platformio.ini` meanwhile.

## 2. Flash

```sh
tools/flash.sh cores3     # 1200-baud touch, esptool, waits until the board is back
tools/flash.sh core2      # the CP2104 resets the chip by itself
```

Options: `--image FILE` (flash a different build) and `--dry-run` (find the port, print what would happen). Only the application is written (`0x10000`), so bonds and the bootloader are untouched.

Why the touch: the CoreS3 firmware uses TinyUSB, whose serial port cannot reset the chip into the bootloader, so a plain `pio run -t upload` fails with `No serial data received`. Opening the port at 1200 baud drops the chip into the ROM bootloader (it then shows as "USB JTAG/serial debug unit", the same USB ID as the application). If that ever fails, hold the side button for about 3 seconds until the green LED lights; too long switches the device off.

## 3. Check what runs on the board

```sh
tools/serial.sh S3 "INFO" "PREFER?"
```

`INFO` shows `fw=<build date and time>`; compare it with the build time. Do this before trusting a test result.

## 4. Functional tests

```sh
jbang tools/SerialTest.java --board S3      # 29 checks: protocol, layers, tap, USB and BLE routing
jbang tools/SerialTest.java --board C2      # 11 checks
jbang tools/SerialTest.java                 # both boards
```

Wait 6 to 8 seconds after a flash before starting (`tools/flash.sh` already does). The test reads the host input device of the board (USB and Bluetooth separately, matched on `Phys`), so the board must be paired and connected.

- Screenshots of every page: `jbang tools/SerialTest.java --shot DIR` writes `DIR/<board>/pageNN.png`. The Core2 takes about 10 minutes because its UART is slow (about 20 s per page); the CoreS3 takes seconds.
- Page-switch timing: `jbang tools/SerialTest.java --latency`. It reports PING, the `SHOT` read-out, the `LAYER` acknowledgement and "page on screen" = (`LAYER` + `SHOT`) minus a plain `SHOT`. A page is only really on screen after a render, which LVGL does after answering `LAYER`, hence the subtraction. The sample is one sparse page, two dense 6x4 pages and the Locks page, so it leans towards the slow pages.

Reference values, 2026-11-18: classic UI about 92 ms (CoreS3) and 100 ms (Core2); LVGL about 360 ms on mixed pages and about 700 ms on the dense pages (CoreS3), 450 to 630 ms (Core2).

## 5. Bluetooth state from the host

```sh
bluetoothctl devices
bluetoothctl info 44:1B:F6:E3:A3:9D        # S3; the Core2 is 08:3A:F2:45:02:62
```

Read-only subcommands only; pairing and removing are done by the owner. The S3 bond lives on the built-in adapter (hci0), the Core2 bond on the USB dongle (hci1).

## 6. LED bars with the webcam

`jbang tools/LedCam.java colours --port /dev/ttyUSB0 --out DIR --crop 300,400,620,600 --clean` cycles `LAYER 0..25`, films the bar and writes `DIR/montage.png` (a contact sheet next to swatches of the expected colours). `pins` finds the CoreS3 bar pin with the `tools/ledtest` pin-finder sketch (flash `tools/ledtest`, env `cores3`, first; flash the real firmware afterwards).

Set-up that works: bar towards the camera, at least 15 cm between the boards, a white surface in front of each bar, a dim room, camera shutter open. The LED itself is overexposed in the picture, so judge the reflection. The numeric verdict is weak; look at the sheet. Turn the lights off at night: it produces the most saturated frames.

Pitfall: opening the CoreS3 serial port from Java (jSerialComm) can reset the native USB chip; allow 1.5 s after opening before the first command (`LedCam pins` retries `LIST`).

## 7. After testing

- Leave the boards on the firmware of the branch you consider current (check `INFO fw=`), on layer 0.
- LED bars cannot be switched off completely: the Locks page shows a segment for every host lock that is on; with all locks off it is faint white.
- Delete camera frames.
