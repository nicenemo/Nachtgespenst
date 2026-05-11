#pragma once
#include <Arduino.h>
#include "Bindings.h"

struct GridLayer {
    const char* pageName;    
    const Binding* tiles;    
    uint8_t rows;            
    uint8_t cols;            
};

// Maven (3x3)
const Binding TILES_MAVEN[] = {
    CS(BTN_B), CS(BTN_Y), CS(BTN_U), 
    BTN_NO,    BTN_NO,    BTN_NO,    
    CS(BTN_I), CS(BTN_Q), CS(BTN_E)  
};

// Office (3x3)
const Binding TILES_OFFICE[] = {
    HYP(BTN_W), HYP(BTN_X), HYP(BTN_P),
    HYP(BTN_O), HYP(BTN_T), HYP(BTN_N),
    HYP(BTN_B), HYP(BTN_E), HYP(BTN_Y)
};

// Edit/Nav (4x3)
const Binding TILES_EDIT_NAV[] = {
    CS(BTN_Z),  CS(BTN_X),  CS(BTN_C), 
    CS(BTN_V),  BTN_UP,     BTN_PGUP,  
    BTN_LEFT,   BTN_DOWN,   BTN_RGHT,  
    BTN_HOME,   BTN_DEL,    BTN_PGDN   
};

// F1-F10 Numpad Style
const Binding TILES_F1_F10[] = {
    BTN_F7,  BTN_F8,  BTN_F9,
    BTN_F4,  BTN_F5,  BTN_F6,
    BTN_F1,  BTN_F2,  BTN_F3,
    BTN_ESC, BTN_F10, BTN_ENT
};

const Binding TILES_F1_F10_MEH[] = {
    MEH(BTN_F7), MEH(BTN_F8), MEH(BTN_F9),
    MEH(BTN_F4), MEH(BTN_F5), MEH(BTN_F6),
    MEH(BTN_F1), MEH(BTN_F2), MEH(BTN_F3),
    BTN_ESC,     MEH(BTN_F10), BTN_ENT
};

const Binding TILES_F1_F10_HYP[] = {
    HYP(BTN_F7), HYP(BTN_F8), HYP(BTN_F9),
    HYP(BTN_F4), HYP(BTN_F5), HYP(BTN_F6),
    HYP(BTN_F1), HYP(BTN_F2), HYP(BTN_F3),
    BTN_ESC,     HYP(BTN_F10), BTN_ENT
};

// F11-F20 Numpad Style
const Binding TILES_F11_F20[] = {
    BTN_F17, BTN_F18, BTN_F19,
    BTN_F14, BTN_F15, BTN_F16,
    BTN_F11, BTN_F12, BTN_F13,
    BTN_ESC, BTN_F20, BTN_ENT
};

const Binding TILES_F11_F20_MEH[] = {
    MEH(BTN_F17), MEH(BTN_F18), MEH(BTN_F19),
    MEH(BTN_F14), MEH(BTN_F15), MEH(BTN_F16),
    MEH(BTN_F11), MEH(BTN_F12), MEH(BTN_F13),
    BTN_ESC,      MEH(BTN_F20), BTN_ENT
};

const Binding TILES_F11_F20_HYP[] = {
    HYP(BTN_F17), HYP(BTN_F18), HYP(BTN_F19),
    HYP(BTN_F14), HYP(BTN_F15), HYP(BTN_F16),
    HYP(BTN_F11), HYP(BTN_F12), HYP(BTN_F13),
    BTN_ESC,      HYP(BTN_F20), BTN_ENT
};

// F21-F24 2x2 Grids
const Binding TILES_F21_F24[] = { BTN_F23, BTN_F24, BTN_F21, BTN_F22 };
const Binding TILES_F21_F24_MEH[] = { MEH(BTN_F23), MEH(BTN_F24), MEH(BTN_F21), MEH(BTN_F22) };
const Binding TILES_F21_F24_HYP[] = { HYP(BTN_F23), HYP(BTN_F24), HYP(BTN_F21), HYP(BTN_F22) };

// International 3x3 Numpad Style
const Binding TILES_INT[] = {
    BTN_INT7, BTN_INT8, BTN_INT9,
    BTN_INT4, BTN_INT5, BTN_INT6,
    BTN_INT1, BTN_INT2, BTN_INT3
};

// Language Grid (4x3)
const Binding TILES_LANG[] = {
    BTN_LNG1,  BTN_LNG2,  BTN_LNG3,
    BTN_LNG4,  BTN_LNG5,  BTN_CMP,
    BTN_APP,   BTN_BPIPE, BTN_LCAPS,
    BTN_LNUM,  BTN_LSCRL, BTN_ENT
};

const GridLayer ALL_LAYERS[] = {
    {"Maven Tools",   TILES_MAVEN,    3, 3},
    {"Office",        TILES_OFFICE,   3, 3},
    {"Edit & Nav",    TILES_EDIT_NAV, 4, 3},
    {"F1-F10",        TILES_F1_F10,   4, 3},
    {"F1-F10 MEH",    TILES_F1_F10_MEH, 4, 3},
    {"F1-F10 HYP",    TILES_F1_F10_HYP, 4, 3},
    {"F11-F20",       TILES_F11_F20,  4, 3},
    {"F11-F20 MEH",   TILES_F11_F20_MEH, 4, 3},
    {"F11-F20 HYP",   TILES_F11_F20_HYP, 4, 3},
    {"F21-F24",       TILES_F21_F24,  2, 2},
    {"F21-F24 MEH",   TILES_F21_F24_MEH, 2, 2},
    {"F21-F24 HYP",   TILES_F21_F24_HYP, 2, 2},
    {"International", TILES_INT,      3, 3},
    {"Language",      TILES_LANG,     4, 3}
};

const uint8_t TOTAL_LAYERS = sizeof(ALL_LAYERS) / sizeof(GridLayer);
