#pragma once

// ==========================================
// 1. SYSTEM & STATUS (0x00 - 0x03)
// ==========================================
#define KC_NO      0x00
#define KC_ROLL    0x01
#define KC_POST    0x02
#define KC_UNDEF   0x03

// ==========================================
// 2. LETTERS (0x04 - 0x1D)
// ==========================================
#define KC_A 0x04
#define KC_B 0x05
#define KC_C 0x06
#define KC_D 0x07
#define KC_E 0x08
#define KC_F 0x09
#define KC_G 0x0A
#define KC_H 0x0B
#define KC_I 0x0C
#define KC_J 0x0D
#define KC_K 0x0E
#define KC_L 0x0F
#define KC_M 0x10
#define KC_N 0x11
#define KC_O 0x12
#define KC_P 0x13
#define KC_Q 0x14
#define KC_R 0x15
#define KC_S 0x16
#define KC_T 0x17
#define KC_U 0x18
#define KC_V 0x19
#define KC_W 0x1A
#define KC_X 0x1B
#define KC_Y 0x1C
#define KC_Z 0x1D

// ==========================================
// 3. CIJFERS & BESTURING
// ==========================================
#define KC_1    0x1E
#define KC_2    0x1F
#define KC_3    0x20
#define KC_4    0x21
#define KC_5    0x22
#define KC_6    0x23
#define KC_7    0x24
#define KC_8    0x25
#define KC_9    0x26
#define KC_0    0x27
#define KC_ENT  0x28
#define KC_ESC  0x29
#define KC_BSPC 0x2A
#define KC_TAB  0x2B
#define KC_SPC  0x2C
#define KC_CAPS 0x39
#define KC_APP  0x65

// ==========================================
// 4. NAVIGATIE & SYMBOLEN
// ==========================================
#define KC_MINS  0x2D
#define KC_EQL   0x2E
#define KC_LBRC  0x2F
#define KC_RBRC  0x30
#define KC_BSLS  0x31
#define KC_SCLN  0x33
#define KC_QUOT  0x34
#define KC_GRV   0x35
#define KC_COMM  0x36
#define KC_DOT   0x37
#define KC_SLSH  0x38
#define KC_BPIPE 0x64
#define KC_PSCR  0x46
#define KC_SLCK  0x47
#define KC_PAUS  0x48
#define KC_INS   0x49
#define KC_HOME  0x4A
#define KC_PGUP  0x4B
#define KC_DEL   0x4C
#define KC_END   0x4D
#define KC_PGDN  0x4E
#define KC_RGHT  0x4F
#define KC_LEFT  0x50
#define KC_DOWN  0x51
#define KC_UP    0x52

// ==========================================
// 5. FUNCTIETOETSEN
// ==========================================
#define KC_F1  0x3A
#define KC_F2  0x3B
#define KC_F3  0x3C
#define KC_F4  0x3D
#define KC_F5  0x3E
#define KC_F6  0x3F
#define KC_F7  0x40
#define KC_F8  0x41
#define KC_F9  0x42
#define KC_F10 0x43
#define KC_F11 0x44
#define KC_F12 0x45
#define KC_F13 0x68
#define KC_F14 0x69
#define KC_F15 0x6A
#define KC_F16 0x6B
#define KC_F17 0x6C
#define KC_F18 0x6D
#define KC_F19 0x6E
#define KC_F20 0x6F
#define KC_F21 0x70
#define KC_F22 0x71
#define KC_F23 0x72
#define KC_F24 0x73

// ==========================================
// 6. NUMPAD & INTERNATIONAAL
// ==========================================
#define KC_NLCK  0x53
#define KC_PSLS  0x54
#define KC_PAST  0x55
#define KC_PMNS  0x56
#define KC_PPLS  0x57
#define KC_PENT  0x58
#define KC_P1    0x59
#define KC_P2    0x5A
#define KC_P3    0x5B
#define KC_P4    0x5C
#define KC_P5    0x5D
#define KC_P6    0x5E
#define KC_P7    0x5F
#define KC_P8    0x60
#define KC_P9    0x61
#define KC_P0    0x62
#define KC_PDOT  0x63
#define KC_00    0xD0
#define KC_000   0xD1
#define KC_KANA  0x88
#define KC_HENK  0x8A
#define KC_MHEN  0x8B
#define KC_RO    0x87
#define KC_JYEN  0x89
#define KC_PCMP  0x67

// ==========================================
// 7. MODIFIERS (BASIC BITS)
// ==========================================
#define MOD_NONE 0x00
// Links
#define M_LCTL   0x01
#define M_LSFT   0x02
#define M_LALT   0x04
#define M_LGUI   0x08
// Rechts
#define M_RCTL   0x10
#define M_RSFT   0x20
#define M_RALT   0x40
#define M_RGUI   0x80

// ==========================================
// 8. COMBINATIES LINKS
// ==========================================
#define M_LCS    (M_LCTL | M_LSFT)
#define M_LCA    (M_LCTL | M_LALT)
#define M_LCG    (M_LCTL | M_LGUI)
#define M_LSA    (M_LSFT | M_LALT)
#define M_LSG    (M_LSFT | M_LGUI)
#define M_LAG    (M_LALT | M_LGUI)
#define M_LCSA   (M_LCTL | M_LSFT | M_LALT)
#define M_LCSG   (M_LCTL | M_LSFT | M_LGUI)
#define M_LCAG   (M_LCTL | M_LALT | M_LGUI)
#define M_LSAG   (M_LSFT | M_LALT | M_LGUI)
#define M_LHYP   (M_LCTL | M_LSFT | M_LALT | M_LGUI)
#define M_LMEH   M_LCSA

// ==========================================
// 9. COMBINATIES RECHTS
// ==========================================
#define M_RCS    (M_RCTL | M_RSFT)
#define M_RCA    (M_RCTL | M_RALT)
#define M_RCG    (M_RCTL | M_RGUI)
#define M_RSA    (M_RSFT | M_RALT)
#define M_RSG    (M_RSFT | M_RGUI)
#define M_RAG    (M_RALT | M_RGUI)
#define M_RCSA   (M_RCTL | M_RSFT | M_RALT)
#define M_RCSG   (M_RCTL | M_RSFT | M_RGUI)
#define M_RCAG   (M_RCTL | M_RALT | M_RGUI)
#define M_RSAG   (M_RSFT | M_RALT | M_RGUI)
#define M_RHYP   (M_RCTL | M_RSFT | M_RALT | M_RGUI)
#define M_RMEH   M_RCSA

// Aliassen
#define M_MEH    M_LCSA
#define M_HYP    M_LHYP
#define M_ALGR   M_RALT
