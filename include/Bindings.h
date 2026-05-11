#pragma once
#include <Arduino.h>
#include "HID_Shortcuts.h"
#include "DisplayColors.h"

struct Binding {
    const char* glyph;
    uint16_t color;
    uint32_t payload;
    HIDPage page;
    uint8_t modifier;
    uint16_t textColor;
};

// --- Helper Macros ---
#define MEH(b)  {b.glyph, b.color, b.payload, b.page, M_LMEH, b.textColor}
#define HYP(b)  {b.glyph, b.color, b.payload, b.page, M_LHYP, b.textColor}
#define SHFT(b) {b.glyph, b.color, b.payload, b.page, M_LSFT, b.textColor}
#define CS(b)   {b.glyph, b.color, b.payload, b.page, M_LCS,  b.textColor}
#define B_EMPTY {"", BLACK, 0, HIDPage::KEYBOARD, MOD_NONE, WHITE}

// Systeem & Navigatie
const Binding BTN_NO    = {"",      BLACK,    KC_NO.code,    HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_ESC   = {"ESC",   RED,      KC_ESC.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_ENT   = {"ENT",   BLUE,     KC_ENT.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_BSPC  = {"BSPC",  RED,      KC_BSPC.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_TAB   = {"TAB",   DARKGREY, KC_TAB.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SPC   = {"SPC",   DARKGREY, KC_SPC.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_DEL   = {"DEL",   RED,      KC_DEL.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INS   = {"INS",   DARKGREY, KC_INS.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_HOME  = {"HOME",  DARKGREY, KC_HOME.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_END   = {"END",   DARKGREY, KC_END.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PGUP  = {"PGUP",  DARKGREY, KC_PGUP.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PGDN  = {"PGDN",  DARKGREY, KC_PGDN.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_UP    = {"UP",    BLUE,     KC_UP.code,    HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_DOWN  = {"DOWN",  BLUE,     KC_DOWN.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LEFT  = {"LEFT",  BLUE,     KC_LEFT.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RGHT  = {"RGHT",  BLUE,     KC_RGHT.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};

// Letters (Maven/Office)
const Binding BTN_B = {"B", DARKGREY, KC_B.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_Y = {"Y", DARKGREY, KC_Y.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_U = {"U", DARKGREY, KC_U.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_I = {"I", DARKGREY, KC_I.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_Q = {"Q", DARKGREY, KC_Q.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_E = {"E", DARKGREY, KC_E.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_W = {"W", DARKGREY, KC_W.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_X = {"X", DARKGREY, KC_X.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P = {"P", DARKGREY, KC_P.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_O = {"O", DARKGREY, KC_O.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_T = {"T", DARKGREY, KC_T.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_N = {"N", DARKGREY, KC_N.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_Z = {"Z", DARKGREY, KC_Z.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_C = {"C", DARKGREY, KC_C.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_V = {"V", DARKGREY, KC_V.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// Functietoetsen F1-F24
const Binding BTN_F1 = {"F1", DARKGREY, KC_F1.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F2 = {"F2", DARKGREY, KC_F2.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F3 = {"F3", DARKGREY, KC_F3.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F4 = {"F4", DARKGREY, KC_F4.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F5 = {"F5", DARKGREY, KC_F5.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F6 = {"F6", DARKGREY, KC_F6.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F7 = {"F7", DARKGREY, KC_F7.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F8 = {"F8", DARKGREY, KC_F8.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F9 = {"F9", DARKGREY, KC_F9.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F10 = {"F10", DARKGREY, KC_F10.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F11 = {"F11", DARKGREY, KC_F11.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F12 = {"F12", DARKGREY, KC_F12.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F13 = {"F13", ORANGE, KC_F13.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F14 = {"F14", ORANGE, KC_F14.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F15 = {"F15", ORANGE, KC_F15.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F16 = {"F16", ORANGE, KC_F16.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F17 = {"F17", ORANGE, KC_F17.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F18 = {"F18", ORANGE, KC_F18.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F19 = {"F19", ORANGE, KC_F19.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F20 = {"F20", ORANGE, KC_F20.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F21 = {"F21", ORANGE, KC_F21.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F22 = {"F22", ORANGE, KC_F22.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F23 = {"F23", ORANGE, KC_F23.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_F24 = {"F24", ORANGE, KC_F24.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};

// International & Language (Staan in HID_Shortcuts.h)
const Binding BTN_INT1 = {"INT1", DARKGREY, KC_INT1.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT2 = {"INT2", DARKGREY, KC_INT2.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT3 = {"INT3", DARKGREY, KC_INT3.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT4 = {"INT4", DARKGREY, KC_INT4.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT5 = {"INT5", DARKGREY, KC_INT5.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT6 = {"INT6", DARKGREY, KC_INT6.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT7 = {"INT7", DARKGREY, KC_INT7.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT8 = {"INT8", DARKGREY, KC_INT8.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT9 = {"INT9", DARKGREY, KC_INT9.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

const Binding BTN_LNG1 = {"LNG1", BLUE, KC_LNG1.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG2 = {"LNG2", BLUE, KC_LNG2.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG3 = {"LNG3", BLUE, KC_LNG3.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG4 = {"LNG4", BLUE, KC_LNG4.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG5 = {"LNG5", BLUE, KC_LNG5.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

const Binding BTN_CMP    = {"COMP", PURPLE, KC_CMP.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_APP    = {"MENU", DARKGREY, KC_APP.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_BPIPE  = {"PIPE", DARKGREY, KC_BPIPE.code,HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LCAPS  = {"CAPS", RED, KC_LCAPS.code,    HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNUM   = {"NUM",  RED, KC_LNUM.code,     HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LSCRL  = {"SCRL", RED, KC_LSCRL.code,    HIDPage::KEYBOARD, MOD_NONE, WHITE};
