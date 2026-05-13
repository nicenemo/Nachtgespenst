#pragma once
#include <stdint.h>

/**
 * HID Usage Page: FIDO Alliance (0xF1D0)
 */
namespace HID::FIDOAlliance {
    constexpr uint8_t PAGE_ID = 0xF1D0;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t U2F_AUTHENTICATOR_DEVICE = 0x01;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t INPUT_REPORT_DATA = 0x20;
    
    constexpr uint8_t OUTPUT_REPORT_DATA = 0x21;

}
