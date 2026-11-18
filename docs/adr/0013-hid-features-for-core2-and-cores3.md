# 0013 Which parts of the HID specification fit the Core2 and the CoreS3

Status: Proposed. A survey and a ranking; nothing is implemented except the LampArray test bench of [0010](0010-lamparray-lighting-and-picture-pages.md). Claims about host support are from memory of the specifications and platform documentation and must be tested on the owner's hosts (Linux, Windows, Android, iPhone SE) before anything is built on them.

## Context
The owner asked what else from the HID specification can be used with the features of the two boards. The boards differ in ways that matter for HID:

| | Core2 | CoreS3 |
|---|---|---|
| MCU, transports | ESP32; Bluetooth LE only (the USB-C port is a CP2104 serial bridge, no USB device) | ESP32-S3; native USB device plus Bluetooth LE |
| Screen and touch | 320 x 240, capacitive, two touch points, three touch buttons below the screen | 320 x 240, capacitive, multi-touch, no buttons |
| Motion | MPU6886 (accelerometer, gyroscope) | BMI270 (accelerometer, gyroscope) and BMM150 (magnetometer) |
| Light, proximity | none | LTR-553 (ambient light and proximity) |
| Camera | none | GC0308, 0.3 megapixel |
| Haptics | vibration motor | none, as far as I know (to verify) |
| Audio | speaker, PDM microphone | speaker, two microphones |
| Other | RTC, battery 390 mAh, microSD, M-Bus (LED bar base) | RTC, battery, microSD, M-Bus (LED bar base) |

Already used: Keyboard (page 0x07) and its LEDs (0x08), Consumer control (0x0C). In progress: Lighting and Illumination, LampArray (0x59).

## What the specification offers, matched to the hardware
| # | Idea | HID page or standard | Board | What it does | Host support (to test) | Effort |
|---|---|---|---|---|---|---|
| 1 | **Call control with a mute light** | Telephony (0x0B: hook switch, phone mute, flash, redial, drop) and the LED page (0x08: Mute, Off-Hook, Ring, Do Not Disturb) | both | a mute, answer and hang-up page; the host drives the bar (red while muted) through the standard LED output report, no tool needed | Teams and Zoom drive headset-like devices this way over USB; BLE and the other hosts unclear | small: one collection, one page |
| 2 | **Battery level** | BLE Battery Service (0x180F); HID Battery Strength (page 0x06, usage 0x20) | both (battery) | the host shows the charge of the pad; the status page of [0011](0011-performance-monitor-and-device-status.md) shows it locally | the BLE service is widely shown (Android, iOS, Windows, Linux); the HID usage less so | small |
| 3 | **Haptic feedback** | Haptics (page 0x0E, simple haptic controller: click, buzz, rumble waveforms) | Core2 | the motor ticks on a tile press (local, no HID needed) and the host can trigger a buzz as a notification | host side rare; the local tick is the value | small; the placeholder `HapticsBlock` in `HID_Specs.h` has the wrong page number (0x0F is Physical Input Device) and needs fixing before use |
| 4 | **Ambient light** | Sensors (page 0x20, ambient light sensor) | CoreS3 | adaptive screen brightness on the host; the pad also dims its own backlight from the same sensor | Windows reads HID sensors; Linux through iio; Android and iOS no | medium |
| 5 | **Proximity as presence** | Sensors, human presence (biometric category) | CoreS3 | "lock when I leave"; the LTR-553 only sees centimetres, so a pad on the desk edge is not a presence detector | Windows 11 presence sensing exists; range is the problem | medium, doubtful value |
| 6 | **Touch as a pointer** | Mouse (Generic Desktop), Digitizer: touch screen or touch pad (0x0D), Wheel with resolution multiplier | both | a small trackpad page, a scroll strip, a drag area; absolute or relative pointer | mouse works everywhere including iOS (as an AssistiveTouch pointer); multi-touch digitizer works on Linux, Windows, Android; a Windows Precision Touchpad needs a certified-style descriptor | medium |
| 7 | **Motion as a controller** | Mouse (air mouse from the gyroscope), Game Pad or Joystick (tilt as axes), Sensors (accelerometer, gyroscope, compass) | both; compass CoreS3 | tilt to scroll or steer, an air mouse for presenting, a page switch by turning the pad | mouse and gamepad work almost everywhere | medium |
| 8 | **Raw HID configuration channel** | vendor-defined usage page (0xFF00..), 32 to 64 byte input and output reports | both | the serial protocol (`LAYER`, `LAMPS`, settings, host OS reporting of [0009](0009-unicode-pages-and-host-os-awareness.md)) over USB and Bluetooth without a serial cable; also a browser keymap editor through WebHID | WebHID: Chrome and Edge, USB; Bluetooth HID in the browser is not reliable | medium |
| 9 | **More usages on the pages already in use** | Keyboard F13 to F24, International and Language keys, Application and Menu; Consumer application launch (AL), application control (AC: search, home, back, forward, zoom, pan), media, camera shutter; System control (power, sleep, wake) | both | more key pages: window and browser control, language switching, a Bluetooth camera shutter; the generated vocabulary already contains all of them | Android and iOS accept the consumer set; System control is the open item in the TODO | trivial (keymap), System control small |
| 10 | **Game pad page** | Game Controls and Generic Desktop game pad: 16 buttons and two sticks | both | a tile page as a macro game pad, touch drag as a stick | broad | small |
| 11 | **Presentation clicker** | Keyboard page up and down, F5, B, and consumer usages | both | keys only, no new HID; a page designed for it | broad | trivial |

Not HID, but only possible on one board because of the native USB: the CoreS3 can also present a **USB camera** (UVC; the GC0308 gives about VGA), a **USB drive** (mass storage for the SD card), and **USB audio** (speaker and microphones). The Core2 cannot, it has no USB device. These are separate USB classes with their own ADRs if wanted.

## Constraints
- **One descriptor change, once.** Every new collection changes the report map, and bonded Bluetooth hosts cache it, so each change means removing and pairing again on every host. Batch the report map changes (LampArray, telephony, battery, raw HID, pointer) into as few releases as possible, and keep the report IDs stable.
- **Report map size and BLE.** The map is read in chunks; the LampArray alone adds about 330 bytes. A map of 1 kB is fine; check the MTU and Android and iOS behaviour after each addition.
- **iOS** accepts keyboard, consumer control and a pointer, but ignores most other usages; features 1, 4, 5 and 7 will not work on the iPhone, and that is acceptable.
- **The Core2 has no USB HID**: everything for it goes over Bluetooth, which is slower and less supported (the LampArray over Bluetooth is itself unverified on Windows).
- **Composite USB on the CoreS3.** Extra HID collections can share the one interface (report IDs, as now) or become separate interfaces (a pointer on its own interface avoids the keyboard boot-protocol problem on hosts that treat a keyboard interface specially).

## Recommendation
Order by value for effort, and group the descriptor changes:
1. **Release A (the LampArray branch, now):** the LampArray report map.
2. **Release B:** telephony and mute LED (1), battery (2), System control (the TODO item), the local haptic tick on the Core2 (3, no HID needed), plus the keymap pages of 9, 10 and 11, which need no descriptor work. One re-pairing for the owner's four hosts.
3. **Release C:** pointer and touch (6, 7) and raw HID (8), because they share the transport questions; decide after B whether they are wanted.
4. **Only if wanted:** ambient light and presence (4, 5) on the CoreS3, and the USB classes.

## Tests
Per feature the same pattern as the LampArray bench: the host-visible effect on each of the four hosts (LED, mute state, battery value, pointer movement), a serial counter or readback on the board, and the camera for light and a screenshot for the screen. Raw HID can reuse the `hidraw` access of `LampTest.java`.

## Decisions needed
1. Which of the eleven ideas are wanted, and in which order (suggested: release B as listed)?
2. Is losing iOS for the non-keyboard features acceptable?
3. Is a pointer or touchpad page wanted, or is the pad a key pad only?
4. Should the USB camera, drive and audio classes of the CoreS3 get an ADR of their own?
