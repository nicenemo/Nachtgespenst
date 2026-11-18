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

// Bluetooth name: moon and ghost emoji (UTF-8 bytes, so no source encoding can mangle them) and the board in lower case,
// e.g. "<moon><ghost>-cs3". USB, INFO and the HID names stay ASCII (K_DEVICE_NAME). See ADR 0015.
#define K_BLE_EMOJI "\xF0\x9F\x8C\x99" "\xF0\x9F\x91\xBB"
#if NG_HAS_BTNS
#define K_BLE_NAME K_BLE_EMOJI "-c2"
#else
#define K_BLE_NAME K_BLE_EMOJI "-cs3"
#endif
