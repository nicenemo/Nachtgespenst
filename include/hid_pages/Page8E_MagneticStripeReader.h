#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Magnetic Stripe Reader (0x8E)
 */
namespace HID::MagneticStripeReader {
    constexpr uint8_t PAGE_ID = 0x8E;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t MSR_DEVICE_READ_ONLY = 0x01;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t TRACK_1_LENGTH = 0x11;
    
    constexpr uint8_t TRACK_2_LENGTH = 0x12;
    
    constexpr uint8_t TRACK_3_LENGTH = 0x13;
    
    constexpr uint8_t TRACK_JIS_LENGTH = 0x14;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint8_t TRACK_DATA = 0x20;
    
    constexpr uint8_t TRACK_1_DATA = 0x21;
    
    constexpr uint8_t TRACK_2_DATA = 0x22;
    
    constexpr uint8_t TRACK_3_DATA = 0x23;
    
    constexpr uint8_t TRACK_JIS_DATA = 0x24;

}
