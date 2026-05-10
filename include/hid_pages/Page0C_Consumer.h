#pragma once
#include <stdint.h>

namespace HID::Consumer {
    constexpr uint8_t PAGE_ID = 0x0C;

    // Audio & Media (16-bit constants)
    constexpr uint16_t MUTE           = 0x00E2;
    constexpr uint16_t VOL_UP         = 0x00E9;
    constexpr uint16_t VOL_DN         = 0x00EA;
    constexpr uint16_t PLAY_PAUSE     = 0x00CD;
    constexpr uint16_t STOP           = 0x00B7;
    constexpr uint16_t NEXT_TRACK     = 0x00B5;
    constexpr uint16_t PREV_TRACK     = 0x00B6;

    // Scherm & Helderheid
    constexpr uint16_t BRIGHT_UP      = 0x006F;
    constexpr uint16_t BRIGHT_DN      = 0x0070;

    // Applicatie Launchers
    constexpr uint16_t AL_CALC        = 0x0192; 
    constexpr uint16_t AL_EMAIL       = 0x018A;
    constexpr uint16_t AL_BROWSER     = 0x0194;
    constexpr uint16_t AL_FILES       = 0x0193;
    constexpr uint16_t AL_DICT        = 0x0196;

    // Browser navigatie
    constexpr uint16_t AC_HOME        = 0x0223;
    constexpr uint16_t AC_BACK        = 0x0224;
    constexpr uint16_t AC_FORWARD     = 0x0225;
    constexpr uint16_t AC_REFRESH     = 0x0227;
    constexpr uint16_t AC_BOOKMARKS   = 0x022A;
}
