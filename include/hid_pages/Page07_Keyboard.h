#pragma once
#include <stdint.h>

namespace HID::Keyboard {
    constexpr uint8_t PAGE_ID = 0x07;

    // --- 0x00 - 0x03: Control & Status ---
    constexpr uint8_t NONE            = 0x00;
    constexpr uint8_t ERROR_ROLLOVER  = 0x01;
    constexpr uint8_t POST_FAIL       = 0x02;
    constexpr uint8_t ERROR_UNDEFINED = 0x03;
    
    // --- 0x04 - 0x1D: Letters (A-Z) ---
    constexpr uint8_t A = 0x04; constexpr uint8_t B = 0x05; constexpr uint8_t C = 0x06;
    constexpr uint8_t D = 0x07; constexpr uint8_t E = 0x08; constexpr uint8_t F = 0x09;
    constexpr uint8_t G = 0x0A; constexpr uint8_t H = 0x0B; constexpr uint8_t I = 0x0C;
    constexpr uint8_t J = 0x0D; constexpr uint8_t K = 0x0E; constexpr uint8_t L = 0x0F;
    constexpr uint8_t M = 0x10; constexpr uint8_t N = 0x11; constexpr uint8_t O = 0x12;
    constexpr uint8_t P = 0x13; constexpr uint8_t Q = 0x14; constexpr uint8_t R = 0x15;
    constexpr uint8_t S = 0x16; constexpr uint8_t T = 0x17; constexpr uint8_t U = 0x18;
    constexpr uint8_t V = 0x19; constexpr uint8_t W = 0x1A; constexpr uint8_t X = 0x1B;
    constexpr uint8_t Y = 0x1C; constexpr uint8_t Z = 0x1D;

    // --- 0x1E - 0x27: Cijfers (1 t/m 0) ---
    constexpr uint8_t KEY_1 = 0x1E; constexpr uint8_t KEY_2 = 0x1F; constexpr uint8_t KEY_3 = 0x20;
    constexpr uint8_t KEY_4 = 0x21; constexpr uint8_t KEY_5 = 0x22; constexpr uint8_t KEY_6 = 0x23;
    constexpr uint8_t KEY_7 = 0x24; constexpr uint8_t KEY_8 = 0x25; constexpr uint8_t KEY_9 = 0x26;
    constexpr uint8_t KEY_0 = 0x27;

    // --- 0x28 - 0x2C: System Keys ---
    constexpr uint8_t ENTER           = 0x28;
    constexpr uint8_t ESCAPE          = 0x29;
    constexpr uint8_t BACKSPACE       = 0x2A;
    constexpr uint8_t TAB             = 0x2B;
    constexpr uint8_t SPACE           = 0x2C;

    // --- 0x2D - 0x38: Punctuations & Symbols (HID/ZMK Standaard) ---
    // Dit is het blok dat je miste:
    constexpr uint8_t MINUS           = 0x2D; // - and _
    constexpr uint8_t EQUAL           = 0x2E; // = and +
    constexpr uint8_t LEFT_BRACE      = 0x2F; // [ and {
    constexpr uint8_t RIGHT_BRACE     = 0x30; // ] and }
    constexpr uint8_t BACKSLASH       = 0x31; // \ and |
    constexpr uint8_t NON_US_HASH     = 0x32; // # and ~
    constexpr uint8_t SEMICOLON       = 0x33; // ; and :
    constexpr uint8_t QUOTE           = 0x34; // ' and "
    constexpr uint8_t GRAVE           = 0x35; // ` and ~
    constexpr uint8_t COMMA           = 0x36; // , and <
    constexpr uint8_t DOT             = 0x37; // . and >
    constexpr uint8_t SLASH           = 0x38; // / and ?

    // --- 0x39 - 0x45: Locks & F-Keys ---
    constexpr uint8_t CAPS_LOCK       = 0x39;
    constexpr uint8_t F1 = 0x3A; constexpr uint8_t F2 = 0x3B; constexpr uint8_t F3 = 0x3C;
    constexpr uint8_t F4 = 0x3D; constexpr uint8_t F5 = 0x3E; constexpr uint8_t F6 = 0x3F;
    constexpr uint8_t F7 = 0x40; constexpr uint8_t F8 = 0x41; constexpr uint8_t F9 = 0x42;
    constexpr uint8_t F10 = 0x43; constexpr uint8_t F11 = 0x44; constexpr uint8_t F12 = 0x45;

    // --- 0x46 - 0x52: Control & Navigation ---
    constexpr uint8_t PRINT_SCREEN    = 0x46;
    constexpr uint8_t SCROLL_LOCK     = 0x47;
    constexpr uint8_t PAUSE           = 0x48;
    constexpr uint8_t INSERT          = 0x49;
    constexpr uint8_t HOME            = 0x4A;
    constexpr uint8_t PAGE_UP         = 0x4B;
    constexpr uint8_t DELETE          = 0x4C; // De officiële 0x4C (ASCII 127)
    constexpr uint8_t END             = 0x4D;
    constexpr uint8_t PAGE_DOWN       = 0x4E;
    constexpr uint8_t RIGHT           = 0x4F;
    constexpr uint8_t LEFT            = 0x50;
    constexpr uint8_t DOWN            = 0x51;
    constexpr uint8_t UP              = 0x52;

    // --- 0x53 - 0x63: Keypad (Numpad) ---
    constexpr uint8_t NUM_LOCK        = 0x53;
    constexpr uint8_t KP_SLASH        = 0x54;
    constexpr uint8_t KP_ASTERISK     = 0x55;
    constexpr uint8_t KP_MINUS        = 0x56;
    constexpr uint8_t KP_PLUS         = 0x57;
    constexpr uint8_t KP_ENTER        = 0x58;
    constexpr uint8_t KP_1 = 0x59; constexpr uint8_t KP_2 = 0x5A; constexpr uint8_t KP_3 = 0x5B;
    constexpr uint8_t KP_4 = 0x5C; constexpr uint8_t KP_5 = 0x5D; constexpr uint8_t KP_6 = 0x5E;
    constexpr uint8_t KP_7 = 0x5F; constexpr uint8_t KP_8 = 0x60; constexpr uint8_t KP_9 = 0x61;
    constexpr uint8_t KP_0 = 0x62; constexpr uint8_t KP_DOT = 0x63;
    
    // --- 0x64: ISO/Non-US backslash ---
    constexpr uint8_t NON_US_BSLS     = 0x64;
}
