#pragma once
#include <stdint.h>

/**
 * HID Usage Page: VESA Virtual Controls (0x82)
 */
namespace HID::VESAVirtualControls {
    constexpr uint8_t PAGE_ID = 0x82;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t DEGAUSS = 0x01;
    
    constexpr uint8_t BRIGHTNESS = 0x10;
    
    constexpr uint8_t CONTRAST = 0x12;
    
    constexpr uint8_t RED_VIDEO_GAIN = 0x16;
    
    constexpr uint8_t GREEN_VIDEO_GAIN = 0x18;
    
    constexpr uint8_t BLUE_VIDEO_GAIN = 0x1A;
    
    constexpr uint8_t FOCUS = 0x1C;
    
    constexpr uint8_t HORIZONTAL_POSITION = 0x20;
    
    constexpr uint8_t HORIZONTAL_SIZE = 0x22;
    
    constexpr uint8_t HORIZONTAL_PINCUSHION = 0x24;
    
    constexpr uint8_t HORIZONTAL_PINCUSHION_BALANCE = 0x26;
    
    constexpr uint8_t HORIZONTAL_MISCONVERGENCE = 0x28;
    
    constexpr uint8_t HORIZONTAL_LINEARITY = 0x2A;
    
    constexpr uint8_t HORIZONTAL_LINEARITY_BALANCE = 0x2C;
    
    constexpr uint8_t VERTICAL_POSITION = 0x30;
    
    constexpr uint8_t VERTICAL_SIZE = 0x32;
    
    constexpr uint8_t VERTICAL_PINCUSHION = 0x34;
    
    constexpr uint8_t VERTICAL_PINCUSHION_BALANCE = 0x36;
    
    constexpr uint8_t VERTICAL_MISCONVERGENCE = 0x38;
    
    constexpr uint8_t VERTICAL_LINEARITY = 0x3A;
    
    constexpr uint8_t VERTICAL_LINEARITY_BALANCE = 0x3C;
    
    constexpr uint8_t PARALLELOGRAM_DISTORTION_KEY_BALANCE = 0x40;
    
    constexpr uint8_t TRAPEZOIDAL_DISTORTION_KEY = 0x42;
    
    constexpr uint8_t TILT_ROTATION = 0x44;
    
    constexpr uint8_t TOP_CORNER_DISTORTION_CONTROL = 0x46;
    
    constexpr uint8_t TOP_CORNER_DISTORTION_BALANCE = 0x48;
    
    constexpr uint8_t BOTTOM_CORNER_DISTORTION_CONTROL = 0x4A;
    
    constexpr uint8_t BOTTOM_CORNER_DISTORTION_BALANCE = 0x4C;
    
    constexpr uint8_t HORIZONTAL_MOIRÉ = 0x56;
    
    constexpr uint8_t VERTICAL_MOIRÉ = 0x58;
    
    constexpr uint8_t RED_VIDEO_BLACK_LEVEL = 0x6C;
    
    constexpr uint8_t GREEN_VIDEO_BLACK_LEVEL = 0x6E;
    
    constexpr uint8_t BLUE_VIDEO_BLACK_LEVEL = 0x70;
    
    constexpr uint8_t HORIZONTAL_FREQUENCY = 0xAC;
    
    constexpr uint8_t VERTICAL_FREQUENCY = 0xAE;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t INPUT_LEVEL_SELECT = 0x5E;
    
    constexpr uint8_t INPUT_SOURCE_SELECT = 0x60;
    
    constexpr uint8_t AUTO_SIZE_CENTER = 0xA2;
    
    constexpr uint8_t POLARITY_HORIZONTAL_SYNCHRONIZATION = 0xA4;
    
    constexpr uint8_t POLARITY_VERTICAL_SYNCHRONIZATION = 0xA6;
    
    constexpr uint8_t SYNCHRONIZATION_TYPE = 0xA8;
    
    constexpr uint8_t SCREEN_ORIENTATION = 0xAA;
    
    constexpr uint8_t SETTINGS = 0xB0;
    
    constexpr uint8_t ON_SCREEN_DISPLAY = 0xCA;
    
    constexpr uint8_t STEREO_MODE = 0xD4;

}
