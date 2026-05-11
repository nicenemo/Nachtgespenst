#pragma once
#include <Arduino.h>
#include "Bindings.h"
#include "LedColors.h" // Nodig voor de CRGB kleuren

struct GridLayer {
    const char* pageName;    
    const Binding* tiles;    
    uint8_t rows;            
    uint8_t cols;            
    CRGB color;            // Nieuw veld om de unieke LED kleur per grid op te slaan
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
    CS(BTN_Z),  CS(BTN_X),  CS(BTN_C), 
    CS(BTN_V),  BTN_UP,     BTN_PGUP,  
    BTN_LEFT,   BTN_DOWN,   BTN_RGHT,  
    BTN_HOME,   BTN_DEL,    BTN_PGDN   
};

// --- 4. F1-F10 (4x3) ---
const Binding TILES_F1_F10[] = {
    BTN_F7,     BTN_F8,     BTN_F9,
    BTN_F4,     BTN_F5,     BTN_F6,
    BTN_F1,     BTN_F2,     BTN_F3,
    BTN_ESC,    BTN_F10,    BTN_ENT
};

// --- 5. F1-F10 MEH (4x3) ---
const Binding TILES_F1_F10_MEH[] = {
    MEH(BTN_F7), MEH(BTN_F8), MEH(BTN_F9),
    MEH(BTN_F4), MEH(BTN_F5), MEH(BTN_F6),
    MEH(BTN_F1), MEH(BTN_F2), MEH(BTN_F3),
    BTN_ESC,     MEH(BTN_F10),BTN_ENT
};

// --- 6. F1-F10 HYPER (4x3) ---
const Binding TILES_F1_F10_HYP[] = {
    HYP(BTN_F7), HYP(BTN_F8), HYP(BTN_F9),
    HYP(BTN_F4), HYP(BTN_F5), HYP(BTN_F6),
    HYP(BTN_F1), HYP(BTN_F2), HYP(BTN_F3),
    BTN_ESC,     HYP(BTN_F10),BTN_ENT
};

// --- 7. F11-F20 (4x3) ---
const Binding TILES_F11_F20[] = {
    BTN_F17,    BTN_F18,    BTN_F19,
    BTN_F14,    BTN_F15,    BTN_F16,
    BTN_F11,    BTN_F12,    BTN_F13,
    BTN_ESC,    BTN_F20,    BTN_ENT
};

// --- 8. F11-F20 MEH (4x3) ---
const Binding TILES_F11_F20_MEH[] = {
    MEH(BTN_F17), MEH(BTN_F18), MEH(BTN_F19),
    MEH(BTN_F14), MEH(BTN_F15), MEH(BTN_F16),
    MEH(BTN_F11), MEH(BTN_F12), MEH(BTN_F13),
    BTN_ESC,      MEH(BTN_F20), BTN_ENT
};

// --- 9. F11-F20 HYPER (4x3) ---
const Binding TILES_F11_F20_HYP[] = {
    HYP(BTN_F17), HYP(BTN_F18), HYP(BTN_F19),
    HYP(BTN_F14), HYP(BTN_F15), HYP(BTN_F16),
    HYP(BTN_F11), HYP(BTN_F12), HYP(BTN_F13),
    BTN_ESC,      HYP(BTN_F20), BTN_ENT
};

// --- 10. F21-F24 (2x2) ---
const Binding TILES_F21_F24[] = { 
    BTN_F23, BTN_F24, 
    BTN_F21, BTN_F22 
};

// --- 11. F21-F24 MEH (2x2) ---
const Binding TILES_F21_F24_MEH[] = { 
    MEH(BTN_F23), MEH(BTN_F24), 
    MEH(BTN_F21), MEH(BTN_F22) 
};

// --- 12. F21-F24 HYP (2x2) ---
const Binding TILES_F21_F24_HYP[] = { 
    HYP(BTN_F23), HYP(BTN_F24), 
    HYP(BTN_F21), HYP(BTN_F22) 
};

// --- 13. Symbol Pad (6x4) ---
const Binding TILES_SYMBOL_PAD[] = {
    BTN_K7,   BTN_K8,   BTN_K9,   BTN_EXLM,
    BTN_K4,   BTN_K5,   BTN_K6,   BTN_AT,
    BTN_K1,   BTN_K2,   BTN_K3,   BTN_HASH,
    BTN_K0,   BTN_DOT,  BTN_COMM, BTN_DLR,
    BTN_LPRN, BTN_RPRN, BTN_MINS, BTN_PLUS,
    BTN_ESC,  BTN_TAB,  BTN_BSPC, BTN_ENT
};

// --- 14. Numpad Full (6x4) ---
const Binding TILES_NUM_FULL[] = {
    BTN_NLCK, BTN_PSLS, BTN_PAST, BTN_PMNS,
    BTN_P7,   BTN_P8,   BTN_P9,   BTN_PPLS,
    BTN_P4,   BTN_P5,   BTN_P6,   BTN_PDOT,
    BTN_P1,   BTN_P2,   BTN_P3,   BTN_PENT,
    BTN_P0,   BTN_P00,  BTN_P000, BTN_PCOM,
    BTN_ESC,  BTN_TAB,  BTN_BSPC, BTN_ENT
};

// --- 15. International (3x3) ---
const Binding TILES_INT[] = {
    BTN_INT7, BTN_INT8, BTN_INT9,
    BTN_INT4, BTN_INT5, BTN_INT6,
    BTN_INT1, BTN_INT2, BTN_INT3
};

// --- 16. Language & Specials (4x3) ---
const Binding TILES_LANG[] = {
    BTN_LNG1,   BTN_LNG2,   BTN_LNG3,
    BTN_LNG4,   BTN_LNG5,   BTN_CMP,
    BTN_APP,    BTN_BPIPE,  BTN_LCAPS,
    BTN_LNUM,   BTN_LSCRL,  BTN_ENT
};

// --- 17. Media & System (4x3) ---
const Binding TILES_MEDIA[] = {
    BTN_PREV,   BTN_PLAY,   BTN_NEXT,
    BTN_VOLD,   BTN_MUTE,   BTN_VOLU,
    BTN_STOP,   BTN_MAIL,   BTN_EXPL,
    BTN_PWR,    BTN_SLP,    BTN_WAKE
};

// --- 18. MS Teams (4x3) ---
const Binding TILES_TEAMS[] = {
    CS(BTN_M),  CS(BTN_O),  CS(BTN_D),
    BTN_NO,     BTN_UP,     BTN_NO,
    BTN_LEFT,   BTN_DOWN,   BTN_RGHT,
    BTN_ESC,    BTN_ENT,    BTN_BSPC
};

// --- 19. MS Outlook (4x3) ---
const Binding TILES_OUTLOOK[] = {
    CS(BTN_N),  CS(BTN_R),  CS(BTN_F),
    BTN_NO,     BTN_UP,     BTN_NO,
    BTN_LEFT,   BTN_DOWN,   BTN_RGHT,
    BTN_DEL,    BTN_ENT,    BTN_BSPC
};

// --- 20. PowerPoint (4x3) ---
const Binding TILES_POWERPOINT[] = {
    BTN_F5,     SHFT(BTN_F5), BTN_ESC,
    BTN_NO,     BTN_UP,       BTN_PGUP,
    BTN_LEFT,   BTN_DOWN,     BTN_RGHT,
    BTN_HOME,   BTN_PGDN,     BTN_END
};

// --- 21. Browser Control (4x3) ---
const Binding TILES_BROWSER[] = {
    BTN_F5,     CS(BTN_R),    CS(BTN_T),
    BTN_F7,     BTN_UP,       CS(BTN_W), 
    BTN_LEFT,   BTN_DOWN,     BTN_RGHT,
    BTN_HOME,   BTN_F11,      BTN_END
};

// --- 22. Debug IDE (4x3) ---
const Binding TILES_DEBUG[] = {
    BTN_F5,     BTN_F9,       CS(BTN_F5),
    BTN_F10,    BTN_F11,      SHFT(BTN_F11),
    BTN_NO,     BTN_UP,       BTN_NO,
    BTN_LEFT,   BTN_DOWN,     BTN_RGHT
};

// --- 23. App Launch (4x3) ---
const Binding TILES_APP_LAUNCH[] = {
    BTN_WWW,    BTN_EXPL,     BTN_MAIL,
    BTN_CAL,    BTN_TASKS,    BTN_CALC,
    BTN_DOC,    BTN_XLS,      BTN_PPT,
    BTN_ESC,    BTN_ENT,      BTN_BSPC
};

// --- 24. Edit Keys (3x3) ---
const Binding TILES_EDIT_KEYS[] = {
    BTN_FIND,   BTN_UNDO,     BTN_NO,
    BTN_CUT,    BTN_COPY,     BTN_PSTE,
    BTN_ESC,    BTN_ENT,      BTN_BSPC
};

// --- 25. Status (3x3) ---
const Binding TILES_STATUS[] = {
    BTN_NO,     BTN_UP,       BTN_NO,
    BTN_LEFT,   BTN_DOWN,     BTN_RGHT,
    BTN_ESC,    BTN_ENT,      BTN_BSPC
};

// ==========================================
// 2. LAYER CONFIGURATIE (Totaal: 25 Grids)
// ==========================================

const GridLayer ALL_LAYERS[] = {
    {"Maven Tools",   TILES_MAVEN,         3, 3, LAYER_COLORS[0]},
    {"Office",        TILES_OFFICE,        4, 3, LAYER_COLORS[1]},
    {"Edit & Nav",    TILES_EDIT_NAV,      4, 3, LAYER_COLORS[2]},
    {"F1-F10",        TILES_F1_F10,        4, 3, LAYER_COLORS[3]},
    {"F1-F10 MEH",    TILES_F1_F10_MEH,    4, 3, LAYER_COLORS[4]},
    {"F1-F10 HYP",    TILES_F1_F10_HYP,    4, 3, LAYER_COLORS[5]},
    {"F11-F20",       TILES_F11_F20,       4, 3, LAYER_COLORS[6]},
    {"F11-F20 MEH",   TILES_F11_F20_MEH,   4, 3, LAYER_COLORS[7]},
    {"F11-F20 HYP",   TILES_F11_F20_HYP,   4, 3, LAYER_COLORS[8]},
    {"F21-F24",       TILES_F21_F24,       2, 2, LAYER_COLORS[9]},
    {"F21-F24 MEH",   TILES_F21_F24_MEH,   2, 2, LAYER_COLORS[10]},
    {"F21-F24 HYP",   TILES_F21_F24_HYP,   2, 2, LAYER_COLORS[11]},
    {"Symbol Pad",    TILES_SYMBOL_PAD,    6, 4, LAYER_COLORS[12]},
    {"Numpad Full",   TILES_NUM_FULL,      6, 4, LAYER_COLORS[13]},
    {"International", TILES_INT,           3, 3, LAYER_COLORS[14]},
    {"Language",      TILES_LANG,          4, 3, LAYER_COLORS[15]},
    {"Media",         TILES_MEDIA,         4, 3, LAYER_COLORS[16]},
    {"MS Teams",      TILES_TEAMS,         4, 3, LAYER_COLORS[17]},
    {"MS Outlook",    TILES_OUTLOOK,       4, 3, LAYER_COLORS[18]},
    {"PowerPoint",    TILES_POWERPOINT,    4, 3, LAYER_COLORS[19]},
    {"Browser Ctrl",  TILES_BROWSER,       4, 3, LAYER_COLORS[20]},
    {"Debug IDE",     TILES_DEBUG,         4, 3, LAYER_COLORS[21]},
    {"App Launch",    TILES_APP_LAUNCH,    4, 3, LAYER_COLORS[22]},
    {"Edit Keys",     TILES_EDIT_KEYS,     3, 3, LAYER_COLORS[23]},
    {"Status Info",   TILES_STATUS,        3, 3, LAYER_COLORS[24]}
};

const uint8_t TOTAL_LAYERS = sizeof(ALL_LAYERS) / sizeof(GridLayer);
