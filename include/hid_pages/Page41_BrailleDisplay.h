#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Braille Display (0x41)
 */
namespace HID::BrailleDisplay {
    constexpr uint8_t PAGE_ID = 0x41;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t BRAILLE_DISPLAY = 0x01;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t ID_8_DOT_BRAILLE_CELL = 0x03;
    
    constexpr uint8_t ID_6_DOT_BRAILLE_CELL = 0x04;
    
    constexpr uint8_t NUMBER_OF_BRAILLE_CELLS = 0x05;
    
    constexpr uint8_t SCREEN_READER_IDENTIFIER = 0x07;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t BRAILLE_ROW = 0x02;
    
    constexpr uint8_t SCREEN_READER_CONTROL = 0x06;
    
    constexpr uint8_t ROUTER_SET_1 = 0xFA;
    
    constexpr uint8_t ROUTER_SET_2 = 0xFB;
    
    constexpr uint8_t ROUTER_SET_3 = 0xFC;
    
    constexpr uint8_t BRAILLE_BUTTONS = 0x200;
    
    constexpr uint8_t BRAILLE_FACE_CONTROLS = 0x20C;
    
    constexpr uint8_t BRAILLE_LEFT_CONTROLS = 0x20D;
    
    constexpr uint8_t BRAILLE_RIGHT_CONTROLS = 0x20E;
    
    constexpr uint8_t BRAILLE_TOP_CONTROLS = 0x20F;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t ROUTER_KEY = 0x100;
    
    constexpr uint8_t ROW_ROUTER_KEY = 0x101;
    
    constexpr uint8_t BRAILLE_DOT_1 = 0x201;
    
    constexpr uint8_t BRAILLE_DOT_2 = 0x202;
    
    constexpr uint8_t BRAILLE_DOT_3 = 0x203;
    
    constexpr uint8_t BRAILLE_DOT_4 = 0x204;
    
    constexpr uint8_t BRAILLE_DOT_5 = 0x205;
    
    constexpr uint8_t BRAILLE_DOT_6 = 0x206;
    
    constexpr uint8_t BRAILLE_DOT_7 = 0x207;
    
    constexpr uint8_t BRAILLE_DOT_8 = 0x208;
    
    constexpr uint8_t BRAILLE_SPACE = 0x209;
    
    constexpr uint8_t BRAILLE_LEFT_SPACE = 0x20A;
    
    constexpr uint8_t BRAILLE_RIGHT_SPACE = 0x20B;
    
    constexpr uint8_t BRAILLE_JOYSTICK_CENTER = 0x210;
    
    constexpr uint8_t BRAILLE_JOYSTICK_UP = 0x211;
    
    constexpr uint8_t BRAILLE_JOYSTICK_DOWN = 0x212;
    
    constexpr uint8_t BRAILLE_JOYSTICK_LEFT = 0x213;
    
    constexpr uint8_t BRAILLE_JOYSTICK_RIGHT = 0x214;
    
    constexpr uint8_t BRAILLE_D_PAD_CENTER = 0x215;
    
    constexpr uint8_t BRAILLE_D_PAD_UP = 0x216;
    
    constexpr uint8_t BRAILLE_D_PAD_DOWN = 0x217;
    
    constexpr uint8_t BRAILLE_D_PAD_LEFT = 0x218;
    
    constexpr uint8_t BRAILLE_D_PAD_RIGHT = 0x219;
    
    constexpr uint8_t BRAILLE_PAN_LEFT = 0x21A;
    
    constexpr uint8_t BRAILLE_PAN_RIGHT = 0x21B;
    
    constexpr uint8_t BRAILLE_ROCKER_UP = 0x21C;
    
    constexpr uint8_t BRAILLE_ROCKER_DOWN = 0x21D;
    
    constexpr uint8_t BRAILLE_ROCKER_PRESS = 0x21E;

}
