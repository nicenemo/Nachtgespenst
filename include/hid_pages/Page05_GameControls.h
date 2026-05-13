#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Game Controls (0x05)
 */
namespace HID::GameControls {
    constexpr uint8_t PAGE_ID = 0x05;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t ID_3D_GAME_CONTROLLER = 0x01;
    
    constexpr uint8_t PINBALL_DEVICE = 0x02;
    
    constexpr uint8_t GUN_DEVICE = 0x03;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t GAMEPAD_FIRE_JUMP = 0x37;
    
    constexpr uint8_t GAMEPAD_TRIGGER = 0x39;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t POINT_OF_VIEW = 0x20;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t TURN_RIGHT_LEFT = 0x21;
    
    constexpr uint8_t PITCH_FORWARD_BACKWARD = 0x22;
    
    constexpr uint8_t ROLL_RIGHT_LEFT = 0x23;
    
    constexpr uint8_t MOVE_RIGHT_LEFT = 0x24;
    
    constexpr uint8_t MOVE_FORWARD_BACKWARD = 0x25;
    
    constexpr uint8_t MOVE_UP_DOWN = 0x26;
    
    constexpr uint8_t LEAN_RIGHT_LEFT = 0x27;
    
    constexpr uint8_t LEAN_FORWARD_BACKWARD = 0x28;
    
    constexpr uint8_t HEIGHT_OF_POV = 0x29;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint8_t FLIPPER = 0x2A;
    
    constexpr uint8_t SECONDARY_FLIPPER = 0x2B;
    
    constexpr uint8_t BUMP = 0x2C;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t GUN_SELECTOR = 0x32;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t GUN_BOLT = 0x30;
    
    constexpr uint8_t GUN_CLIP = 0x31;
    
    constexpr uint8_t GUN_SAFETY = 0x36;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t NEW_GAME = 0x2D;
    
    constexpr uint8_t SHOOT_BALL = 0x2E;
    
    constexpr uint8_t PLAYER = 0x2F;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint8_t FORM_FITTING_GAMEPAD = 0x3A;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t GUN_SINGLE_SHOT = 0x33;
    
    constexpr uint8_t GUN_BURST = 0x34;
    
    constexpr uint8_t GUN_AUTOMATIC = 0x35;

}
