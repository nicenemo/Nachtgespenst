#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Medical Instrument (0x40)
 */
namespace HID::MedicalInstrument {
    constexpr uint8_t PAGE_ID = 0x40;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t MEDICAL_ULTRASOUND = 0x01;

    // ==========================================
    // KIND: Linear Control (Relative/Range) (LC)
    // ==========================================
    
    constexpr uint8_t CINE = 0x40;
    
    constexpr uint8_t TRANSMIT_POWER = 0x41;
    
    constexpr uint8_t VOLUME = 0x42;
    
    constexpr uint8_t FOCUS = 0x43;
    
    constexpr uint8_t DEPTH = 0x44;
    
    constexpr uint8_t SOFT_STEP_PRIMARY = 0x60;
    
    constexpr uint8_t SOFT_STEP_SECONDARY = 0x61;
    
    constexpr uint8_t DEPTH_GAIN_COMPENSATION = 0x70;
    
    constexpr uint8_t ZOOM_ADJUST = 0x81;
    
    constexpr uint8_t SPECTRAL_DOPPLER_ADJUST = 0x83;
    
    constexpr uint8_t COLOR_DOPPLER_ADJUST = 0x85;
    
    constexpr uint8_t MOTION_MODE_ADJUST = 0x87;
    
    constexpr uint8_t ID_2_D_MODE_ADJUST = 0x89;
    
    constexpr uint8_t SOFT_CONTROL_ADJUST = 0xA1;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t VCR_ACQUISITION = 0x20;
    
    constexpr uint8_t FREEZE_THAW = 0x21;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t CLIP_STORE = 0x22;
    
    constexpr uint8_t UPDATE = 0x23;
    
    constexpr uint8_t NEXT = 0x24;
    
    constexpr uint8_t SAVE = 0x25;
    
    constexpr uint8_t PRINT = 0x26;
    
    constexpr uint8_t MICROPHONE_ENABLE = 0x27;
    
    constexpr uint8_t ZOOM_SELECT = 0x80;
    
    constexpr uint8_t SPECTRAL_DOPPLER_MODE_SELECT = 0x82;
    
    constexpr uint8_t COLOR_DOPPLER_MODE_SELECT = 0x84;
    
    constexpr uint8_t MOTION_MODE_SELECT = 0x86;
    
    constexpr uint8_t ID_2_D_MODE_SELECT = 0x88;
    
    constexpr uint8_t SOFT_CONTROL_SELECT = 0xA0;

}
