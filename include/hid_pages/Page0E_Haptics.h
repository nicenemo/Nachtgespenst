#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Haptics (0x0E)
 */
namespace HID::Haptics {
    constexpr uint8_t PAGE_ID = 0x0E;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t SIMPLE_HAPTIC_CONTROLLER = 0x01;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t AUTO_TRIGGER = 0x20;
    
    constexpr uint8_t MANUAL_TRIGGER = 0x21;
    
    constexpr uint8_t INTENSITY = 0x23;
    
    constexpr uint8_t REPEAT_COUNT = 0x24;
    
    constexpr uint8_t RETRIGGER_PERIOD = 0x25;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t WAVEFORM_LIST = 0x10;
    
    constexpr uint8_t DURATION_LIST = 0x11;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t AUTO_TRIGGER_ASSOCIATED_CONTROL = 0x22;
    
    constexpr uint8_t WAVEFORM_VENDOR_PAGE = 0x26;
    
    constexpr uint8_t WAVEFORM_VENDOR_ID = 0x27;
    
    constexpr uint8_t WAVEFORM_CUTOFF_TIME = 0x28;
    
    constexpr uint8_t WAVEFORM_NONE = 0x1001;
    
    constexpr uint8_t WAVEFORM_STOP = 0x1002;
    
    constexpr uint8_t WAVEFORM_CLICK = 0x1003;
    
    constexpr uint8_t WAVEFORM_BUZZ_CONTINUOUS = 0x1004;
    
    constexpr uint8_t WAVEFORM_RUMBLE_CONTINUOUS = 0x1005;
    
    constexpr uint8_t WAVEFORM_PRESS = 0x1006;
    
    constexpr uint8_t WAVEFORM_RELEASE = 0x1007;
    
    constexpr uint8_t WAVEFORM_HOVER = 0x1008;
    
    constexpr uint8_t WAVEFORM_SUCCESS = 0x1009;
    
    constexpr uint8_t WAVEFORM_ERROR = 0x100A;
    
    constexpr uint8_t WAVEFORM_INK_CONTINUOUS = 0x100B;
    
    constexpr uint8_t WAVEFORM_PENCIL_CONTINUOUS = 0x100C;
    
    constexpr uint8_t WAVEFORM_MARKER_CONTINUOUS = 0x100D;
    
    constexpr uint8_t WAVEFORM_CHISEL_MARKER_CONTINUOUS = 0x100E;
    
    constexpr uint8_t WAVEFORM_BRUSH_CONTINUOUS = 0x100F;
    
    constexpr uint8_t WAVEFORM_ERASER_CONTINUOUS = 0x1010;
    
    constexpr uint8_t WAVEFORM_SPARKLE_CONTINUOUS = 0x1011;

}
