#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Generic Device Controls (0x06)
 */
namespace HID::GenericDeviceControls {
    constexpr uint8_t PAGE_ID = 0x06;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t BACKGROUND_NONUSER_CONTROLS = 0x01;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t SOFTWARE_VERSION = 0x2A;
    
    constexpr uint8_t PROTOCOL_VERSION = 0x2B;
    
    constexpr uint8_t HARDWARE_VERSION = 0x2C;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t GRIP_POSE_OFFSET = 0x40;
    
    constexpr uint8_t POINTER_POSE_OFFSET = 0x41;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t SEQUENCE_ID_RESET = 0x28;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t BATTERY_STRENGTH = 0x20;
    
    constexpr uint8_t WIRELESS_CHANNEL = 0x21;
    
    constexpr uint8_t WIRELESS_ID = 0x22;
    
    constexpr uint8_t SEQUENCE_ID = 0x27;
    
    constexpr uint8_t RF_SIGNAL_STRENGTH = 0x29;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t HANDEDNESS = 0x30;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t DISCOVER_WIRELESS_CONTROL = 0x23;
    
    constexpr uint8_t SECURITY_CODE_CHARACTER_ENTERED = 0x24;
    
    constexpr uint8_t SECURITY_CODE_CHARACTER_ERASED = 0x25;
    
    constexpr uint8_t SECURITY_CODE_CLEARED = 0x26;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t MAJOR = 0x2D;
    
    constexpr uint8_t MINOR = 0x2E;
    
    constexpr uint8_t REVISION = 0x2F;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t EITHER_HAND = 0x31;
    
    constexpr uint8_t LEFT_HAND = 0x32;
    
    constexpr uint8_t RIGHT_HAND = 0x33;
    
    constexpr uint8_t BOTH_HANDS = 0x34;

}
