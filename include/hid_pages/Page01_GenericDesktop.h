#pragma once
#include <stdint.h>

namespace HID::Desktop {
    // De Usage Page ID voor Generic Desktop Controls
    constexpr uint8_t PAGE_ID = 0x01;

    // ==========================================
    // SYSTEM CONTROLS (Power Management)
    // AANGEPAST: Nu uint8_t i.p.v. const char*
    // ==========================================
    constexpr uint8_t SYS_PWR_DN      = 0x81;
    constexpr uint8_t SYS_SLEEP       = 0x82;
    constexpr uint8_t SYS_WAKE        = 0x83;
    constexpr uint8_t SYS_CONTEXT_MNU = 0x84; 
    constexpr uint8_t SYS_MAIN_MENU   = 0x85; 
    constexpr uint8_t SYS_APP_MENU    = 0x86; 
    constexpr uint8_t SYS_MENU_HELP   = 0x87; 
    constexpr uint8_t SYS_MENU_EXIT   = 0x88; 
    constexpr uint8_t SYS_MENU_SELECT = 0x89; 
    constexpr uint8_t SYS_DOCK        = 0xA0;
    constexpr uint8_t SYS_UNDOCK      = 0xA1;
    constexpr uint8_t SYS_SETUP       = 0xA2;

    // ==========================================
    // POINTER & NAVIGATION (Generic GUI)
    // ==========================================
    constexpr uint8_t POINTER         = 0x01;
    constexpr uint8_t MOUSE           = 0x02;
    constexpr uint8_t JOYSTICK        = 0x04;
    constexpr uint8_t GAMEPAD         = 0x05;
    constexpr uint8_t KEYBOARD        = 0x06;
    constexpr uint8_t KEYPAD          = 0x07;
    constexpr uint8_t MULTI_AXIS      = 0x08; // Voor 3D mice

    // ==========================================
    // AXES (Assen voor emulatie)
    // ==========================================
    namespace Axis {
        constexpr uint8_t X           = 0x30; 
        constexpr uint8_t Y           = 0x31; 
        constexpr uint8_t Z           = 0x32; 
        constexpr uint8_t RX          = 0x33; 
        constexpr uint8_t RY          = 0x34; 
        constexpr uint8_t RZ          = 0x35; 
        constexpr uint8_t SLIDER      = 0x36;
        constexpr uint8_t DIAL        = 0x37;
        constexpr uint8_t WHEEL       = 0x38; 
        constexpr uint8_t HAT_SWITCH  = 0x39; 
    }

    // ==========================================
    // DIRECTIONAL CONTROLS (DPAD Style)
    // ==========================================
    namespace DPad {
        constexpr uint8_t UP          = 0x90;
        constexpr uint8_t DOWN        = 0x91;
        constexpr uint8_t LEFT        = 0x92;
        constexpr uint8_t RIGHT       = 0x93;
    }
}
