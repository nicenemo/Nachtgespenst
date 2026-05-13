#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Camera Control (0x90)
 */
namespace HID::CameraControl {
    constexpr uint8_t PAGE_ID = 0x90;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t CAMERA_AUTO_FOCUS = 0x20;
    
    constexpr uint8_t CAMERA_SHUTTER = 0x21;

}
