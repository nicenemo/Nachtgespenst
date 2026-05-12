#pragma once
#include <Arduino.h>
#include "Bindings.h"
#include "LedColors.h" 

struct GridLayer {
    const char* pageName;    
    const Binding* tiles;    
    uint8_t rows;            
    uint8_t cols;            
    CRGB color;            
};

// ==========================================
// 1. BINDING ARRAYS (Visuele Layouts)
// ==========================================

// --- 1. Maven Tools (3x3) ---
const Binding TILES_MAVEN[] = {
    CS(BTN_B),  CS(BTN_Y),  CS(BTN_U), 
    BTN_NO,     BTN_NO,     BTN_NO,    
    CS(BTN_I),  CS(BTN_Q),  CS(BTN_E)  
};

// --- 2. Office & Productivity (4x3) ---
const Binding TILES_OFFICE[] = {
    HYP(BTN_W), HYP(BTN_X), HYP(BTN_P),
    HYP(BTN_O), HYP(BTN_T), HYP(BTN_N),
    HYP(BTN_B), HYP(BTN_E), HYP(BTN_Y),
    HYP(BTN_L), BTN_NO,     BTN_NO
};

// --- 3. Edit & Navigatie (4x3) ---
const Binding TILES_EDIT_NAV[] = {
    BTN_INS,    BTN_HOME,   BTN_PGUP,
    BTN_DEL,    BTN_END,    BTN_PGDN,
    BTN_NO,     BTN_UP,     BTN_NO,
    BTN_LEFT,   BTN_DOWN,   BTN_RGHT
};

// --- 4. Systeem & Edit (4x3) ---
const Binding TILES_SYS_EDIT[] = {
    BTN_ESC,    BTN_APP,    BTN_PSCR, 
    BTN_UNDO,   BTN_CUT,    BTN_COPY,
    BTN_PSTE,   BTN_FIND,   BTN_SYRQ,
    BTN_SLCK,   BTN_PAUS,   BTN_PWR
};

// --- 5. Functietoetsen F1-F20 (4x3 met ankers) ---
const Binding TILES_F1_F10[] = {
    BTN_F1,  BTN_F2,  BTN_F3,
    BTN_F4,  BTN_F5,  BTN_F6,
    BTN_F7,  BTN_F8,  BTN_F9,
    BTN_NO,  BTN_F10, BTN_NO
};

const Binding TILES_F1_F10_MEH[] = {
    MEH(BTN_F1), MEH(BTN_F2), MEH(BTN_F3),
    MEH(BTN_F4), MEH(BTN_F5), MEH(BTN_F6),
    MEH(BTN_F7), MEH(BTN_F8), MEH(BTN_F9),
    BTN_NO,      MEH(BTN_F10), BTN_NO
};

const Binding TILES_F1_F10_HYP[] = {
    HYP(BTN_F1), HYP(BTN_F2), HYP(BTN_F3),
    HYP(BTN_F4), HYP(BTN_F5), HYP(BTN_F6),
    HYP(BTN_F7), HYP(BTN_F8), HYP(BTN_F9),
    BTN_NO,      HYP(BTN_F10), BTN_NO
};

const Binding TILES_F11_F20[] = {
    BTN_F11, BTN_F12, BTN_F13,
    BTN_F14, BTN_F15, BTN_F16,
    BTN_F17, BTN_F18, BTN_F19,
    BTN_NO,  BTN_F20, BTN_NO
};

const Binding TILES_F11_F20_MEH[] = {
    MEH(BTN_F11), MEH(BTN_F12), MEH(BTN_F13),
    MEH(BTN_F14), MEH(BTN_F15), MEH(BTN_F16),
    MEH(BTN_F17), MEH(BTN_F18), MEH(BTN_F19),
    BTN_NO,       MEH(BTN_F20), BTN_NO
};

const Binding TILES_F11_F20_HYP[] = {
    HYP(BTN_F11), HYP(BTN_F12), HYP(BTN_F13),
    HYP(BTN_F14), HYP(BTN_F15), HYP(BTN_F16),
    HYP(BTN_F17), HYP(BTN_F18), HYP(BTN_F19),
    BTN_NO,       HYP(BTN_F20), BTN_NO
};

// --- 6. Functietoetsen F21-F24 (2x2 Grids) ---
const Binding TILES_F21_F24[] = {
    BTN_F21, BTN_F22,
    BTN_F23, BTN_F24
};

const Binding TILES_F21_F24_MEH[] = {
    MEH(BTN_F21), MEH(BTN_F22),
    MEH(BTN_F23), MEH(BTN_F24)
};

const Binding TILES_F21_F24_HYP[] = {
    HYP(BTN_F21), HYP(BTN_F22),
    HYP(BTN_F23), HYP(BTN_F24)
};

// --- 7. Numpad Full (6x4) ---
const Binding TILES_NUM_FULL[] = {
    BTN_NLCK, BTN_PSLS, BTN_PAST, BTN_PMNS,
    BTN_P7,   BTN_P8,   BTN_P9,   BTN_PPLS,
    BTN_P4,   BTN_P5,   BTN_P6,   BTN_PENT,
    BTN_P1,   BTN_P2,   BTN_P3,   BTN_PENT,
    BTN_P0,   BTN_P00,  BTN_PDOT, BTN_PENT,
    BTN_PLPR, BTN_PRPR, BTN_PEQL, BTN_PCOM
};

// --- 8. International & Language ---
const Binding TILES_INT[] = {
    BTN_INT1, BTN_INT2, BTN_INT3,
    BTN_INT4, BTN_INT5, BTN_INT6,
    BTN_INT7, BTN_INT8, BTN_INT9
};

const Binding TILES_LANG[] = {
    BTN_LNG1, BTN_LNG2, BTN_LNG3,
    BTN_LNG4, BTN_LNG5, BTN_LNG6,
    BTN_KANA, BTN_HIRA, BTN_ROMA,
    BTN_HENK, BTN_MHEN, BTN_CMP
};

// --- 9. Hardware Locking ---
const Binding TILES_LOCKING[] = {
    BTN_LCAPS, BTN_LNUM, BTN_LSCRL
};

// --- 10. Media ---
const Binding TILES_MEDIA[] = {
    BTN_MUTE, BTN_VOLU, BTN_VOLD,
    BTN_PREV, BTN_PLAY, BTN_NEXT,
    BTN_WWW,  BTN_MAIL, BTN_CALC,
    BTN_EXPL, BTN_TASK, BTN_STOP
};

// ==========================================
// 2. LAYER DEFINITIES
// ==========================================

const GridLayer LAYERS[] = {
    {"Maven Tools",   TILES_MAVEN,       3, 3, LAYER_COLORS[0]},
    {"Office",        TILES_OFFICE,      4, 3, LAYER_COLORS[1]},
    {"Edit & Nav",    TILES_EDIT_NAV,    4, 3, LAYER_COLORS[2]},
    {"System",        TILES_SYS_EDIT,    4, 3, LAYER_COLORS[3]},
    
    // F-Toetsen (4x3)
    {"F1-F10",        TILES_F1_F10,      4, 3, LAYER_COLORS[4]},
    {"F1-F10 MEH",    TILES_F1_F10_MEH,  4, 3, LAYER_COLORS[4]},
    {"F1-F10 HYP",    TILES_F1_F10_HYP,  4, 3, LAYER_COLORS[4]},
    {"F11-F20",       TILES_F11_F20,     4, 3, LAYER_COLORS[5]},
    {"F11-F20 MEH",   TILES_F11_F20_MEH, 4, 3, LAYER_COLORS[5]},
    {"F11-F20 HYP",   TILES_F11_F20_HYP, 4, 3, LAYER_COLORS[5]},
    
    // F-Toetsen (2x2)
    {"F21-F24",       TILES_F21_F24,     2, 2, LAYER_COLORS[6]},
    {"F21-F24 MEH",   TILES_F21_F24_MEH, 2, 2, LAYER_COLORS[6]},
    {"F21-F24 HYP",   TILES_F21_F24_HYP, 2, 2, LAYER_COLORS[6]},
    
    // Overige
    {"Numpad Full",   TILES_NUM_FULL,    6, 4, LAYER_COLORS[7]},
    {"International", TILES_INT,         3, 3, LAYER_COLORS[8]},
    {"Language",      TILES_LANG,        4, 3, LAYER_COLORS[9]},
    {"Hard Locks",    TILES_LOCKING,     1, 3, LAYER_COLORS[10]},
    {"Media & Sys",   TILES_MEDIA,       4, 3, LAYER_COLORS[11]}
};

const uint8_t LAYER_COUNT = sizeof(LAYERS) / sizeof(GridLayer);
