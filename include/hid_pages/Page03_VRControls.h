#pragma once
#include <stdint.h>

/**
 * HID Usage Page: VR Controls (0x03)
 */
namespace HID::VRControls {
    constexpr uint8_t PAGE_ID = 0x03;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t BELT = 0x01;
    
    constexpr uint8_t BODY_SUIT = 0x02;
    
    constexpr uint8_t GLOVE = 0x04;
    
    constexpr uint8_t HEAD_MOUNTED_DISPLAY = 0x06;
    
    constexpr uint8_t HAND_TRACKER = 0x07;
    
    constexpr uint8_t OCULOMETER = 0x08;
    
    constexpr uint8_t VEST = 0x09;
    
    constexpr uint8_t ANIMATRONIC_DEVICE = 0x0A;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t FLEXOR = 0x03;
    
    constexpr uint8_t HEAD_TRACKER = 0x05;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t STEREO_ENABLE = 0x20;
    
    constexpr uint8_t DISPLAY_ENABLE = 0x21;

}
