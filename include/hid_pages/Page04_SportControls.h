#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Sport Controls (0x04)
 */
namespace HID::SportControls {
    constexpr uint8_t PAGE_ID = 0x04;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t BASEBALL_BAT = 0x01;
    
    constexpr uint8_t GOLF_CLUB = 0x02;
    
    constexpr uint8_t ROWING_MACHINE = 0x03;
    
    constexpr uint8_t TREADMILL = 0x04;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t OAR = 0x30;
    
    constexpr uint8_t SLOPE = 0x31;
    
    constexpr uint8_t RATE = 0x32;
    
    constexpr uint8_t STICK_SPEED = 0x33;
    
    constexpr uint8_t STICK_FACE_ANGLE = 0x34;
    
    constexpr uint8_t STICK_HEEL_TOE = 0x35;
    
    constexpr uint8_t STICK_FOLLOW_THROUGH = 0x36;
    
    constexpr uint8_t STICK_TEMPO = 0x37;
    
    constexpr uint8_t STICK_HEIGHT = 0x39;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t STICK_TYPE = 0x38;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t PUTTER = 0x50;
    
    constexpr uint8_t ID_1_IRON = 0x51;
    
    constexpr uint8_t ID_2_IRON = 0x52;
    
    constexpr uint8_t ID_3_IRON = 0x53;
    
    constexpr uint8_t ID_4_IRON = 0x54;
    
    constexpr uint8_t ID_5_IRON = 0x55;
    
    constexpr uint8_t ID_6_IRON = 0x56;
    
    constexpr uint8_t ID_7_IRON = 0x57;
    
    constexpr uint8_t ID_8_IRON = 0x58;
    
    constexpr uint8_t ID_9_IRON = 0x59;
    
    constexpr uint8_t ID_10_IRON = 0x5A;
    
    constexpr uint8_t ID_11_IRON = 0x5B;
    
    constexpr uint8_t SAND_WEDGE = 0x5C;
    
    constexpr uint8_t LOFT_WEDGE = 0x5D;
    
    constexpr uint8_t POWER_WEDGE = 0x5E;
    
    constexpr uint8_t ID_1_WOOD = 0x5F;
    
    constexpr uint8_t ID_3_WOOD = 0x60;
    
    constexpr uint8_t ID_5_WOOD = 0x61;
    
    constexpr uint8_t ID_7_WOOD = 0x62;
    
    constexpr uint8_t ID_9_WOOD = 0x63;

}
