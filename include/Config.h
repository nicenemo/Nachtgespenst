#pragma once
#ifndef K_DEVICE_NAME
#define K_DEVICE_NAME "Nachtgespenst"
#endif
#ifndef NG_BOARD_ID
#define NG_BOARD_ID "UNKNOWN"
#endif
#define K_MANUFACTURER "BKA - KDA"
#define K_MODEL "NG-81-V12"
#define LED_COUNT       10
#define TYPING_ANIM_MS  300
// WS2812 data pin: Core2 side LED bar = GPIO25; CoreS3 has no bar (GPIO25 does not exist), use Port B pin
#ifndef LED_PIN
#define LED_PIN 9
#endif
// 1 when a verified WS2812 LED bar is present (Core2 side bar on GPIO25). CoreS3: none.
#ifndef NG_HAS_LEDBAR
#define NG_HAS_LEDBAR 0
#endif
// 1 when the board has (capacitive) buttons BtnA/B/C below the screen (Core2). CoreS3: none -> on-screen nav bar.
#ifndef NG_HAS_BTNS
#define NG_HAS_BTNS 0
#endif
