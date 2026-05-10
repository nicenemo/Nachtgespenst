#pragma once
#include <stdint.h>

namespace HID::LEDs {
    constexpr uint8_t PAGE_ID = 0x08;

    // Bitmaskers voor het inkomende HID Output Report
    enum Mask : uint8_t {
        NUM_LOCK    = 0x01, // Host geeft aan: NumLock is actief
        CAPS_LOCK   = 0x02, // Host geeft aan: CapsLock is actief
        SCROLL_LOCK = 0x04, // Host geeft aan: ScrollLock is actief
        COMPOSE     = 0x08, // Host geeft aan: Compose mode actief
        KANA        = 0x10, // Host geeft aan: Kana mode actief (Japan)
        POWER       = 0x20, // Led voor power status
        SHIFT       = 0x40, // Led voor Shift state
        MUTE        = 0x80  // Hardware mute indicator
    };
}
