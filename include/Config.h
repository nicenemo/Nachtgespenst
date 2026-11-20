#pragma once
#ifndef K_DEVICE_NAME
#define K_DEVICE_NAME "Nachtgespenst"
#endif
#ifndef NG_BOARD_ID
#define NG_BOARD_ID "UNKNOWN"
#endif
#define K_MANUFACTURER "Nachtgespenst"
// Identity of the boards (ADR 0015): names are short ASCII, the Bluetooth PnP ID uses the USB vendor ID source (0x02).
// pid.codes test ID 1209:0001, only until the project has its own; the PnP version is board (high byte) and report map revision (low byte).
#define NG_VID 0x1209
#define NG_PID 0x0001
#ifndef NG_PNP_VERSION
#define NG_PNP_VERSION 0x0001
#endif
#ifndef NG_SERIAL_INPUT
#define NG_SERIAL_INPUT 1   // serial KEY and TAP commands (tests); 0 removes them from a build that must not type on its own
#endif
// Temporary diagnostics (see docs/OVERNIGHT.md): a heartbeat line every second and extra BLE logging.
#ifndef NG_DEBUG_HEARTBEAT
#define NG_DEBUG_HEARTBEAT 0
#endif
#define LED_COUNT       10
// WS2812 data pin: Core2 side LED bar = GPIO25, CoreS3 M5GO base bar = GPIO5 (both M-Bus pin 8)
#ifndef LED_PIN
#define LED_PIN 9
#endif
// 1 when a WS2812 LED bar is present (Core2: GPIO25, CoreS3: GPIO5).
#ifndef NG_HAS_LEDBAR
#define NG_HAS_LEDBAR 0
#endif
// 1 when the board has (capacitive) buttons BtnA/B/C below the screen (Core2). CoreS3: none -> on-screen nav bar.
#ifndef NG_HAS_BTNS
#define NG_HAS_BTNS 0
#endif
// 1 when the board is a presentation clicker and air mouse (Core2): one page, a mouse report in the report map, the gyroscope as a pointer.
#ifndef NG_PRESENTER
#define NG_PRESENTER NG_HAS_BTNS
#endif

// Bluetooth name: plain ASCII (NG), no emoji: some hosts (Windows) cope badly with it. The Core2 is a presentation clicker and says so.
// This supersedes the emoji names of ADR 0015. USB, INFO and the HID names are K_DEVICE_NAME.
#if NG_HAS_BTNS
#define K_BLE_NAME "NG-C2-clicker"
#else
#define K_BLE_NAME "NG-S3-ble"
#endif
