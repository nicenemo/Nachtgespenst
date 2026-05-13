#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Monitor (0x80)
 */
namespace HID::Monitor {
    constexpr uint8_t PAGE_ID = 0x80;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t MONITOR_CONTROL = 0x01;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t EDID_INFORMATION = 0x02;
    
    constexpr uint8_t VDIF_INFORMATION = 0x03;
    
    constexpr uint8_t VESA_VERSION = 0x04;

}
