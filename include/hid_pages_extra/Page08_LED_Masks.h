#pragma once
#include <stdint.h>

namespace HID::LEDs::Output {
    enum Mask : uint8_t {
        NUM_LOCK    = (1 << 0), 
        CAPS_LOCK   = (1 << 1), 
        SCROLL_LOCK = (1 << 2), 
        COMPOSE     = (1 << 3), 
        KANA        = (1 << 4), 
        POWER       = (1 << 5), 
        SHIFT       = (1 << 6), 
        MUTE        = (1 << 7)  
    };
}
