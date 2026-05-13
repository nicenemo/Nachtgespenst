#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Keyboard/Keypad (0x07)
 */
namespace HID::Keyboard {
    constexpr uint8_t PAGE_ID = 0x07;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t LEFTCONTROL = 0xE0;
    
    constexpr uint8_t LEFTSHIFT = 0xE1;
    
    constexpr uint8_t LEFTALT = 0xE2;
    
    constexpr uint8_t LEFT_GUI = 0xE3;
    
    constexpr uint8_t RIGHTCONTROL = 0xE4;
    
    constexpr uint8_t RIGHTSHIFT = 0xE5;
    
    constexpr uint8_t RIGHTALT = 0xE6;
    
    constexpr uint8_t RIGHT_GUI = 0xE7;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t ERRORROLLOVER = 0x01;
    
    constexpr uint8_t POSTFAIL = 0x02;
    
    constexpr uint8_t ERRORUNDEFINED = 0x03;
    
    constexpr uint8_t A = 0x04;
    
    constexpr uint8_t B = 0x05;
    
    constexpr uint8_t C = 0x06;
    
    constexpr uint8_t D = 0x07;
    
    constexpr uint8_t E = 0x08;
    
    constexpr uint8_t F = 0x09;
    
    constexpr uint8_t G = 0x0A;
    
    constexpr uint8_t H = 0x0B;
    
    constexpr uint8_t I = 0x0C;
    
    constexpr uint8_t J = 0x0D;
    
    constexpr uint8_t K = 0x0E;
    
    constexpr uint8_t L = 0x0F;
    
    constexpr uint8_t M = 0x10;
    
    constexpr uint8_t N = 0x11;
    
    constexpr uint8_t O = 0x12;
    
    constexpr uint8_t P = 0x13;
    
    constexpr uint8_t Q = 0x14;
    
    constexpr uint8_t R = 0x15;
    
    constexpr uint8_t S = 0x16;
    
    constexpr uint8_t T = 0x17;
    
    constexpr uint8_t U = 0x18;
    
    constexpr uint8_t V = 0x19;
    
    constexpr uint8_t W = 0x1A;
    
    constexpr uint8_t X = 0x1B;
    
    constexpr uint8_t Y = 0x1C;
    
    constexpr uint8_t Z = 0x1D;
    
    constexpr uint8_t ID_1_AND_BANG = 0x1E;
    
    constexpr uint8_t ID_2_AND_AT = 0x1F;
    
    constexpr uint8_t ID_3_AND_HASH = 0x20;
    
    constexpr uint8_t ID_4_AND_DOLLAR = 0x21;
    
    constexpr uint8_t ID_5_AND_PERCENT = 0x22;
    
    constexpr uint8_t ID_6_AND_CARET = 0x23;
    
    constexpr uint8_t ID_7_AND_AMPERSAND = 0x24;
    
    constexpr uint8_t ID_8_AND_STAR = 0x25;
    
    constexpr uint8_t ID_9_AND_LEFT_BRACKET = 0x26;
    
    constexpr uint8_t ID_0_AND_RIGHT_BRACKET = 0x27;
    
    constexpr uint8_t RETURN_ENTER = 0x28;
    
    constexpr uint8_t ESCAPE = 0x29;
    
    constexpr uint8_t DELETE = 0x2A;
    
    constexpr uint8_t TAB = 0x2B;
    
    constexpr uint8_t SPACEBAR = 0x2C;
    
    constexpr uint8_t DASH_AND_UNDERSCORE = 0x2D;
    
    constexpr uint8_t EQUALS_AND_PLUS = 0x2E;
    
    constexpr uint8_t LEFT_BRACE = 0x2F;
    
    constexpr uint8_t RIGHT_BRACE = 0x30;
    
    constexpr uint8_t BACKSLASH_AND_PIPE = 0x31;
    
    constexpr uint8_t NON_US_HASH_AND_TILDE = 0x32;
    
    constexpr uint8_t SEMICOLON_AND_COLON = 0x33;
    
    constexpr uint8_t LEFT_APOS_AND_DOUBLE = 0x34;
    
    constexpr uint8_t GRAVE_ACCENT_AND_TILDE = 0x35;
    
    constexpr uint8_t COMMA_AND_LESSTHAN = 0x36;
    
    constexpr uint8_t PERIOD_AND_GREATERTHAN = 0x37;
    
    constexpr uint8_t FORWARDSLASH_AND_QUESTIONMARK = 0x38;
    
    constexpr uint8_t CAPS_LOCK = 0x39;
    
    constexpr uint8_t F1 = 0x3A;
    
    constexpr uint8_t F2 = 0x3B;
    
    constexpr uint8_t F3 = 0x3C;
    
    constexpr uint8_t F4 = 0x3D;
    
    constexpr uint8_t F5 = 0x3E;
    
    constexpr uint8_t F6 = 0x3F;
    
    constexpr uint8_t F7 = 0x40;
    
    constexpr uint8_t F8 = 0x41;
    
    constexpr uint8_t F9 = 0x42;
    
    constexpr uint8_t F10 = 0x43;
    
    constexpr uint8_t F11 = 0x44;
    
    constexpr uint8_t F12 = 0x45;
    
    constexpr uint8_t PRINTSCREEN = 0x46;
    
    constexpr uint8_t SCROLL_LOCK = 0x47;
    
    constexpr uint8_t PAUSE = 0x48;
    
    constexpr uint8_t INSERT = 0x49;
    
    constexpr uint8_t HOME = 0x4A;
    
    constexpr uint8_t PAGEUP = 0x4B;
    
    constexpr uint8_t DELETE_FORWARD = 0x4C;
    
    constexpr uint8_t END = 0x4D;
    
    constexpr uint8_t PAGEDOWN = 0x4E;
    
    constexpr uint8_t RIGHTARROW = 0x4F;
    
    constexpr uint8_t LEFTARROW = 0x50;
    
    constexpr uint8_t DOWNARROW = 0x51;
    
    constexpr uint8_t UPARROW = 0x52;
    
    constexpr uint8_t NUM_LOCK_AND_CLEAR = 0x53;
    
    constexpr uint8_t FORWARDSLASH = 0x54;
    
    constexpr uint8_t STAR = 0x55;
    
    constexpr uint8_t DASH = 0x56;
    
    constexpr uint8_t PLUS = 0x57;
    
    constexpr uint8_t ENTER = 0x58;
    
    constexpr uint8_t ID_1_AND_END = 0x59;
    
    constexpr uint8_t ID_2_AND_DOWN_ARROW = 0x5A;
    
    constexpr uint8_t ID_3_AND_PAGEDN = 0x5B;
    
    constexpr uint8_t ID_4_AND_LEFT_ARROW = 0x5C;
    
    constexpr uint8_t ID_5 = 0x5D;
    
    constexpr uint8_t ID_6_AND_RIGHT_ARROW = 0x5E;
    
    constexpr uint8_t ID_7_AND_HOME = 0x5F;
    
    constexpr uint8_t ID_8_AND_UP_ARROW = 0x60;
    
    constexpr uint8_t ID_9_AND_PAGEUP = 0x61;
    
    constexpr uint8_t ID_0_AND_INSERT = 0x62;
    
    constexpr uint8_t PERIOD_AND_DELETE = 0x63;
    
    constexpr uint8_t NON_US_BACKSLASH_AND_PIPE = 0x64;
    
    constexpr uint8_t APPLICATION = 0x65;
    
    constexpr uint8_t POWER = 0x66;
    
    constexpr uint8_t EQUALS = 0x67;
    
    constexpr uint8_t F13 = 0x68;
    
    constexpr uint8_t F14 = 0x69;
    
    constexpr uint8_t F15 = 0x6A;
    
    constexpr uint8_t F16 = 0x6B;
    
    constexpr uint8_t F17 = 0x6C;
    
    constexpr uint8_t F18 = 0x6D;
    
    constexpr uint8_t F19 = 0x6E;
    
    constexpr uint8_t F20 = 0x6F;
    
    constexpr uint8_t F21 = 0x70;
    
    constexpr uint8_t F22 = 0x71;
    
    constexpr uint8_t F23 = 0x72;
    
    constexpr uint8_t F24 = 0x73;
    
    constexpr uint8_t EXECUTE = 0x74;
    
    constexpr uint8_t HELP = 0x75;
    
    constexpr uint8_t MENU = 0x76;
    
    constexpr uint8_t SELECT = 0x77;
    
    constexpr uint8_t STOP = 0x78;
    
    constexpr uint8_t AGAIN = 0x79;
    
    constexpr uint8_t UNDO = 0x7A;
    
    constexpr uint8_t CUT = 0x7B;
    
    constexpr uint8_t COPY = 0x7C;
    
    constexpr uint8_t PASTE = 0x7D;
    
    constexpr uint8_t FIND = 0x7E;
    
    constexpr uint8_t MUTE = 0x7F;
    
    constexpr uint8_t VOLUME_UP = 0x80;
    
    constexpr uint8_t VOLUME_DOWN = 0x81;
    
    constexpr uint8_t LOCKING_CAPS_LOCK = 0x82;
    
    constexpr uint8_t LOCKING_NUM_LOCK = 0x83;
    
    constexpr uint8_t LOCKING_SCROLL_LOCK = 0x84;
    
    constexpr uint8_t COMMA = 0x85;
    
    constexpr uint8_t EQUAL_SIGN = 0x86;
    
    constexpr uint8_t INTERNATIONAL1 = 0x87;
    
    constexpr uint8_t INTERNATIONAL2 = 0x88;
    
    constexpr uint8_t INTERNATIONAL3 = 0x89;
    
    constexpr uint8_t INTERNATIONAL4 = 0x8A;
    
    constexpr uint8_t INTERNATIONAL5 = 0x8B;
    
    constexpr uint8_t INTERNATIONAL6 = 0x8C;
    
    constexpr uint8_t INTERNATIONAL7 = 0x8D;
    
    constexpr uint8_t INTERNATIONAL8 = 0x8E;
    
    constexpr uint8_t INTERNATIONAL9 = 0x8F;
    
    constexpr uint8_t LANG1 = 0x90;
    
    constexpr uint8_t LANG2 = 0x91;
    
    constexpr uint8_t LANG3 = 0x92;
    
    constexpr uint8_t LANG4 = 0x93;
    
    constexpr uint8_t LANG5 = 0x94;
    
    constexpr uint8_t LANG6 = 0x95;
    
    constexpr uint8_t LANG7 = 0x96;
    
    constexpr uint8_t LANG8 = 0x97;
    
    constexpr uint8_t LANG9 = 0x98;
    
    constexpr uint8_t ALTERNATE_ERASE = 0x99;
    
    constexpr uint8_t SYSREQ_ATTENTION = 0x9A;
    
    constexpr uint8_t CANCEL = 0x9B;
    
    constexpr uint8_t CLEAR = 0x9C;
    
    constexpr uint8_t PRIOR = 0x9D;
    
    constexpr uint8_t RETURN = 0x9E;
    
    constexpr uint8_t SEPARATOR = 0x9F;
    
    constexpr uint8_t OUT = 0xA0;
    
    constexpr uint8_t OPER = 0xA1;
    
    constexpr uint8_t CLEAR_AGAIN = 0xA2;
    
    constexpr uint8_t CRSEL_PROPS = 0xA3;
    
    constexpr uint8_t EXSEL = 0xA4;
    
    constexpr uint8_t DOUBLE_0 = 0xB0;
    
    constexpr uint8_t TRIPLE_0 = 0xB1;
    
    constexpr uint8_t THOUSANDS_SEPARATOR = 0xB2;
    
    constexpr uint8_t DECIMAL_SEPARATOR = 0xB3;
    
    constexpr uint8_t CURRENCY_UNIT = 0xB4;
    
    constexpr uint8_t CURRENCY_SUB_UNIT = 0xB5;
    
    constexpr uint8_t LEFT_BRACKET = 0xB6;
    
    constexpr uint8_t RIGHT_BRACKET = 0xB7;
    
    constexpr uint8_t LEFT_BRACE = 0xB8;
    
    constexpr uint8_t RIGHT_BRACE = 0xB9;
    
    constexpr uint8_t TAB = 0xBA;
    
    constexpr uint8_t BACKSPACE = 0xBB;
    
    constexpr uint8_t A = 0xBC;
    
    constexpr uint8_t B = 0xBD;
    
    constexpr uint8_t C = 0xBE;
    
    constexpr uint8_t D = 0xBF;
    
    constexpr uint8_t E = 0xC0;
    
    constexpr uint8_t F = 0xC1;
    
    constexpr uint8_t XOR = 0xC2;
    
    constexpr uint8_t CARET = 0xC3;
    
    constexpr uint8_t PERCENTAGE = 0xC4;
    
    constexpr uint8_t LESS = 0xC5;
    
    constexpr uint8_t GREATER = 0xC6;
    
    constexpr uint8_t AMPERSAND = 0xC7;
    
    constexpr uint8_t DOUBLE_AMPERSAND = 0xC8;
    
    constexpr uint8_t BAR = 0xC9;
    
    constexpr uint8_t DOUBLE_BAR = 0xCA;
    
    constexpr uint8_t COLON = 0xCB;
    
    constexpr uint8_t HASH = 0xCC;
    
    constexpr uint8_t SPACE = 0xCD;
    
    constexpr uint8_t AT = 0xCE;
    
    constexpr uint8_t BANG = 0xCF;
    
    constexpr uint8_t MEMORY_STORE = 0xD0;
    
    constexpr uint8_t MEMORY_RECALL = 0xD1;
    
    constexpr uint8_t MEMORY_CLEAR = 0xD2;
    
    constexpr uint8_t MEMORY_ADD = 0xD3;
    
    constexpr uint8_t MEMORY_SUBTRACT = 0xD4;
    
    constexpr uint8_t MEMORY_MULTIPLY = 0xD5;
    
    constexpr uint8_t MEMORY_DIVIDE = 0xD6;
    
    constexpr uint8_t PLUS_MINUS = 0xD7;
    
    constexpr uint8_t CLEAR = 0xD8;
    
    constexpr uint8_t CLEAR_ENTRY = 0xD9;
    
    constexpr uint8_t BINARY = 0xDA;
    
    constexpr uint8_t OCTAL = 0xDB;
    
    constexpr uint8_t DECIMAL = 0xDC;
    
    constexpr uint8_t HEXADECIMAL = 0xDD;

}
