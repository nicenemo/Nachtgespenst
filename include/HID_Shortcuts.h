#pragma once
#include <Arduino.h>

enum class HIDPage : uint8_t { KEYBOARD, CONSUMER, SYSTEM, HAPTIC, LIGHTING };

struct Key {
    HIDPage page;
    uint16_t code;
};

#define K(c) Key{HIDPage::KEYBOARD, c}
#define C(c) Key{HIDPage::CONSUMER, c}

// 1. STATUS & SYSTEM
#define KC_NO      K(0x00)
#define KC_ROLL    K(0x01)
#define KC_POST    K(0x02)
#define KC_UNDEF   K(0x03)

// 2. LETTERS
#define KC_A K(0x04)
#define KC_B K(0x05)
#define KC_C K(0x06)
#define KC_D K(0x07)
#define KC_E K(0x08)
#define KC_F K(0x09)
#define KC_G K(0x0A)
#define KC_H K(0x0B)
#define KC_I K(0x0C)
#define KC_J K(0x0D)
#define KC_K K(0x0E)
#define KC_L K(0x0F)
#define KC_M K(0x10)
#define KC_N K(0x11)
#define KC_O K(0x12)
#define KC_P K(0x13)
#define KC_Q K(0x14)
#define KC_R K(0x15)
#define KC_S K(0x16)
#define KC_T K(0x17)
#define KC_U K(0x18)
#define KC_V K(0x19)
#define KC_W K(0x1A)
#define KC_X K(0x1B)
#define KC_Y K(0x1C)
#define KC_Z K(0x1D)

// 3. CIJFERS
#define KC_1 K(0x1E)
#define KC_2 K(0x1F)
#define KC_3 K(0x20)
#define KC_4 K(0x21)
#define KC_5 K(0x22)
#define KC_6 K(0x23)
#define KC_7 K(0x24)
#define KC_8 K(0x25)
#define KC_9 K(0x26)
#define KC_0 K(0x27)

// 4. INTERPUNCTIE
#define KC_MINS K(0x2D)
#define KC_EQL  K(0x2E)
#define KC_LBRC K(0x2F)
#define KC_RBRC K(0x30)
#define KC_BSLS K(0x31)
#define KC_SCLN K(0x33)
#define KC_QUOT K(0x34)
#define KC_GRV  K(0x35)
#define KC_COMM K(0x36)
#define KC_DOT  K(0x37)
#define KC_SLSH K(0x38)
#define KC_NUHS K(0x32)
#define KC_NUBS K(0x64)
#define KC_EURO K(0x31)

// 5. NAVIGATIE & STATUS
#define KC_ENT  K(0x28)
#define KC_ESC  K(0x29)
#define KC_BSPC K(0x2A)
#define KC_TAB  K(0x2B)
#define KC_SPC  K(0x2C)
#define KC_CAPS K(0x39)
#define KC_PSCR K(0x46)
#define KC_SLCK K(0x47)
#define KC_PAUS K(0x48)
#define KC_INS  K(0x49)
#define KC_HOME K(0x4A)
#define KC_PGUP K(0x4B)
#define KC_DEL  K(0x4C)
#define KC_END  K(0x4D)
#define KC_PGDN K(0x4E)
#define KC_RGHT K(0x4F)
#define KC_LEFT K(0x50)
#define KC_DOWN K(0x51)
#define KC_UP   K(0x52)
#define KC_APP  K(0x65)
#define KC_SYRQ K(0x9A)

// 6. NUMPAD
#define KC_NLCK K(0x53)
#define KC_PSLS K(0x54)
#define KC_PAST K(0x55)
#define KC_PMNS K(0x56)
#define KC_PPLS K(0x57)
#define KC_PENT K(0x58)
#define KC_P1   K(0x59)
#define KC_P2   K(0x5A)
#define KC_P3   K(0x5B)
#define KC_P4   K(0x5C)
#define KC_P5   K(0x5D)
#define KC_P6   K(0x5E)
#define KC_P7   K(0x5F)
#define KC_P8   K(0x60)
#define KC_P9   K(0x61)
#define KC_P0   K(0x62)
#define KC_PDOT K(0x63)
#define KC_PEQL K(0x67)
#define KC_PCOM K(0x85)
#define KC_P00  K(0xD0)
#define KC_P000 K(0xD1)
#define KC_PLPR K(0xB6)
#define KC_PRPR K(0xB7)
#define KC_PTLD K(0x35)

// 7. FUNCTIE TOETSEN
#define KC_F1  K(0x3A)
#define KC_F2  K(0x3B)
#define KC_F3  K(0x3C)
#define KC_F4  K(0x3D)
#define KC_F5  K(0x3E)
#define KC_F6  K(0x3F)
#define KC_F7  K(0x40)
#define KC_F8  K(0x41)
#define KC_F9  K(0x42)
#define KC_F10 K(0x43)
#define KC_F11 K(0x44)
#define KC_F12 K(0x45)
#define KC_F13 K(0x68)
#define KC_F14 K(0x69)
#define KC_F15 K(0x6A)
#define KC_F16 K(0x6B)
#define KC_F17 K(0x6C)
#define KC_F18 K(0x6D)
#define KC_F19 K(0x6E)
#define KC_F20 K(0x6F)
#define KC_F21 K(0x70)
#define KC_F22 K(0x71)
#define KC_F23 K(0x72)
#define KC_F24 K(0x73)

// 8. INTERNATIONAL & LNG
#define KC_INT1 K(0x87)
#define KC_INT2 K(0x88)
#define KC_INT3 K(0x89)
#define KC_INT4 K(0x8A)
#define KC_INT5 K(0x8B)
#define KC_INT6 K(0x8C)
#define KC_INT7 K(0x8D)
#define KC_INT8 K(0x8E)
#define KC_INT9 K(0x8F)
#define KC_LNG1 K(0x90)
#define KC_LNG2 K(0x91)
#define KC_LNG3 K(0x92)
#define KC_LNG4 K(0x93)
#define KC_LNG5 K(0x94)
#define KC_LNG6 K(0x95)
#define KC_LNG7 K(0x96)
#define KC_LNG8 K(0x97)
#define KC_LNG9 K(0x98)

// Extra aliassen & Speciale toetsen
#define KC_KANA K(0x92)
#define KC_HIRA K(0x93)
#define KC_ROMA K(0x87)
#define KC_HENK K(0x8A)
#define KC_MHEN K(0x8B)
#define KC_CMP  K(0xB4) // Compose key (volgens HID spec 0xB4)

// 9. HARDWARE LOCKING KEYS
#define KC_LCAPS K(0x82)
#define KC_LNUM  K(0x83)
#define KC_LSCRL K(0x84)

// 10. MEDIA & SYSTEM
#define KC_PWR   K(0x66)
#define KC_SLP   K(0xF8)
#define KC_WAKE  K(0xF9)
#define MC_MUTE  C(0xE2)
#define MC_VOLU  C(0xE9)
#define MC_VOLD  C(0xEA)
#define MC_PLAY  C(0xCD)
#define MC_STOP  C(0xB7)
#define MC_NEXT  C(0xB5)
#define MC_PREV  C(0xB6)
#define MC_MAIL  C(0x18A)
#define MC_CALC  C(0x192)
#define MC_EXPL  C(0x194)
#define MC_WWW   C(0x182)
#define MC_CONF  C(0x184)
#define MC_SPRD  C(0x185)
#define MC_PRES  C(0x186)
#define MC_SCHD  C(0x18E)
#define MC_TASK  C(0x18F)

// 11. MODIFIERS RAW
#define KC_LCTL K(0xE0)
#define KC_LSFT K(0xE1)
#define KC_LALT K(0xE2)
#define KC_LGUI K(0xE3)
#define KC_RCTL K(0xE4)
#define KC_RSFT K(0xE5)
#define KC_RALT K(0xE6)
#define KC_RGUI K(0xE7)

// 12. BITMASKS
#define MOD_NONE 0x00
#define M_LCTL   0x01
#define M_LSFT   0x02
#define M_LALT   0x04
#define M_LGUI   0x08
#define M_RCTL   0x10
#define M_RSFT   0x20
#define M_RALT   0x40
#define M_RGUI   0x80

// 13. COMBINATIES
#define M_LCS  (M_LCTL | M_LSFT)
#define M_LCA  (M_LCTL | M_LALT)
#define M_LAS  (M_LALT | M_LSFT)
#define M_LCAS (M_LCTL | M_LSFT | M_LALT)
#define M_LMEH (M_LCTL | M_LSFT | M_LALT)
#define M_LHYP (M_LCTL | M_LSFT | M_LALT | M_LGUI)
#define M_RCS  (M_RCTL | M_RSFT)
#define M_RCA  (M_RCTL | M_RALT)
#define M_RAS  (M_RALT | M_RSFT)
#define M_RCAS (M_RCTL | M_RSFT | M_RALT)
#define M_RMEH (M_RCTL | M_RSFT | M_RALT)
#define M_RHYP (M_RCTL | M_RSFT | M_RALT | M_RGUI)

// 14. DOUBLE TAPS
#define M_DLCT 0x11 
#define M_DLSF 0x22
#define M_DLAL 0x44
#define M_DLGU 0x88
#define M_DRCT 0x13
#define M_DRSF 0x23
#define M_DRAL 0x43
#define M_DRGU 0x83

// 15. EDIT
#define KC_UNDO K(0x76)
#define KC_CUT  K(0x77)
#define KC_COPY K(0x78)
#define KC_PSTE K(0x79)
#define KC_FIND K(0x7A)
