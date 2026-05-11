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

// --- Macros voor combinaties ---
#define MEH(b)  {b.glyph, b.color, b.payload, b.page, M_LMEH, b.textColor}
#define HYP(b)  {b.glyph, b.color, b.payload, b.page, M_LHYP, b.textColor}
#define SHFT(b) {b.glyph, b.color, b.payload, b.page, M_LSFT, b.textColor}
#define CS(b)   {b.glyph, b.color, b.payload, b.page, M_LCS,  b.textColor}

// 1. LETTERS (26)
const Binding BTN_A = {"A", DARKGREY, KC_A.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_B = {"B", DARKGREY, KC_B.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_C = {"C", DARKGREY, KC_C.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_D = {"D", DARKGREY, KC_D.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_E = {"E", DARKGREY, KC_E.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F = {"F", DARKGREY, KC_F.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_G = {"G", DARKGREY, KC_G.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_H = {"H", DARKGREY, KC_H.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_I = {"I", DARKGREY, KC_I.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_J = {"J", DARKGREY, KC_J.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K = {"K", DARKGREY, KC_K.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_L = {"L", DARKGREY, KC_L.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_M = {"M", DARKGREY, KC_M.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_N = {"N", DARKGREY, KC_N.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_O = {"O", DARKGREY, KC_O.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P = {"P", DARKGREY, KC_P.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_Q = {"Q", DARKGREY, KC_Q.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_R = {"R", DARKGREY, KC_R.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_S = {"S", DARKGREY, KC_S.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_T = {"T", DARKGREY, KC_T.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_U = {"U", DARKGREY, KC_U.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_V = {"V", DARKGREY, KC_V.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_W = {"W", DARKGREY, KC_W.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_X = {"X", DARKGREY, KC_X.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_Y = {"Y", DARKGREY, KC_Y.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_Z = {"Z", DARKGREY, KC_Z.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 2. CIJFERS (10)
const Binding BTN_K1 = {"1", DARKGREY, KC_1.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K2 = {"2", DARKGREY, KC_2.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K3 = {"3", DARKGREY, KC_3.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K4 = {"4", DARKGREY, KC_4.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K5 = {"5", DARKGREY, KC_5.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K6 = {"6", DARKGREY, KC_6.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K7 = {"7", DARKGREY, KC_7.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K8 = {"8", DARKGREY, KC_8.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K9 = {"9", DARKGREY, KC_9.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_K0 = {"0", DARKGREY, KC_0.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 3. F-KEYS (24)
const Binding BTN_F1  = {"F1",  DARKGREY, KC_F1.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F2  = {"F2",  DARKGREY, KC_F2.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F3  = {"F3",  DARKGREY, KC_F3.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F4  = {"F4",  DARKGREY, KC_F4.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F5  = {"F5",  DARKGREY, KC_F5.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F6  = {"F6",  DARKGREY, KC_F6.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F7  = {"F7",  DARKGREY, KC_F7.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F8  = {"F8",  DARKGREY, KC_F8.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F9  = {"F9",  DARKGREY, KC_F9.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F10 = {"F10", DARKGREY, KC_F10.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F11 = {"F11", DARKGREY, KC_F11.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F12 = {"F12", DARKGREY, KC_F12.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F13 = {"F13", BLUE,     KC_F13.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F14 = {"F14", BLUE,     KC_F14.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F15 = {"F15", BLUE,     KC_F15.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F16 = {"F16", BLUE,     KC_F16.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F17 = {"F17", BLUE,     KC_F17.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F18 = {"F18", BLUE,     KC_F18.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F19 = {"F19", BLUE,     KC_F19.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F20 = {"F20", BLUE,     KC_F20.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F21 = {"F21", BLUE,     KC_F21.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F22 = {"F22", BLUE,     KC_F22.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F23 = {"F23", BLUE,     KC_F23.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_F24 = {"F24", BLUE,     KC_F24.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 4. NAVIGATIE & EDITING (20)
const Binding BTN_ENT  = {"ENT",  RED,      KC_ENT.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_ESC  = {"ESC",  RED,      KC_ESC.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_BSPC = {"BSPC", RED,      KC_BSPC.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_TAB  = {"TAB",  DARKGREY, KC_TAB.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SPC  = {"SPC",  DARKGREY, KC_SPC.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INS  = {"INS",  DARKGREY, KC_INS.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_DEL  = {"DEL",  RED,      KC_DEL.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_HOME = {"HOME", DARKGREY, KC_HOME.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_END  = {"END",  DARKGREY, KC_END.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PGUP = {"PGUP", DARKGREY, KC_PGUP.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PGDN = {"PGDN", DARKGREY, KC_PGDN.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PSCR = {"PSCR", DARKGREY, KC_PSCR.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SLCK = {"SLCK", DARKGREY, KC_SLCK.code, HIDPage::KEYBOARD, MOD_NONE, WHITE}; 
const Binding BTN_PAUS = {"PAUS", DARKGREY, KC_PAUS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_UP   = {"UP",   BLUE,     KC_UP.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_DOWN = {"DOWN", BLUE,     KC_DOWN.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LEFT = {"LEFT", BLUE,     KC_LEFT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RGHT = {"RGHT", BLUE,     KC_RGHT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_APP  = {"APP",  DARKGREY, KC_APP.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SYRQ = {"SYRQ", DARKGREY, KC_SYRQ.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 5. LOCKS (Hier zitten de LEDs)
// Gebruik BTN_CAPS, BTN_NUM, BTN_SCRL voor je Framework 13
const Binding BTN_CAPS  = {"CAPS", ORANGE,   KC_CAPS.code,  HIDPage::KEYBOARD, MOD_NONE, BLACK}; 
const Binding BTN_NUM   = {"NUM",  ORANGE,   KC_NLCK.code,  HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_SCRL  = {"SCRL", ORANGE,   KC_SLCK.code,  HIDPage::KEYBOARD, MOD_NONE, BLACK};

// Echte locking scancodes (voor piep/test doeleinden)
const Binding BTN_LCAPS = {"L-CAP", RED,     KC_LCAPS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNUM  = {"L-NUM", RED,     KC_LNUM.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LSCRL = {"L-SCR", RED,     KC_LSCRL.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 6. NUMPAD (23 unieke codes)
const Binding BTN_P1   = {"P1",   DARKGREY, KC_P1.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P2   = {"P2",   DARKGREY, KC_P2.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P3   = {"P3",   DARKGREY, KC_P3.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P4   = {"P4",   DARKGREY, KC_P4.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P5   = {"P5",   DARKGREY, KC_P5.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P6   = {"P6",   DARKGREY, KC_P6.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P7   = {"P7",   DARKGREY, KC_P7.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P8   = {"P8",   DARKGREY, KC_P8.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P9   = {"P9",   DARKGREY, KC_P9.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P0   = {"P0",   DARKGREY, KC_P0.code,   HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PSLS = {"P/",   DARKGREY, KC_PSLS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PAST = {"P*",   DARKGREY, KC_PAST.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PMNS = {"P-",   DARKGREY, KC_PMNS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PPLS = {"P+",   DARKGREY, KC_PPLS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PENT = {"PENT", RED,      KC_PENT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PDOT = {"P.",   DARKGREY, KC_PDOT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PEQL = {"P=",   DARKGREY, KC_PEQL.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PCOM = {"P,",   DARKGREY, KC_PCOM.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P00  = {"P00",  DARKGREY, KC_P00.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_P000 = {"P000", DARKGREY, KC_P000.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PLPR = {"P(",   DARKGREY, KC_PLPR.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PRPR = {"P)",   DARKGREY, KC_PRPR.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_PTLD = {"P~",   DARKGREY, KC_PTLD.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 7. SYMBOLEN & INTERPUNCTIE (14)
const Binding BTN_MINS = {"-",    PURPLE,   KC_MINS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_EQL  = {"=",    PURPLE,   KC_EQL.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LBRC = {"[",    PURPLE,   KC_LBRC.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RBRC = {"]",    PURPLE,   KC_RBRC.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_BSLS = {"\\",   PURPLE,   KC_BSLS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SCLN = {";",    PURPLE,   KC_SCLN.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_QUOT = {"'",    PURPLE,   KC_QUOT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_GRV  = {"`",    PURPLE,   KC_GRV.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_COMM = {",",    PURPLE,   KC_COMM.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_DOT  = {".",    PURPLE,   KC_DOT.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SLSH = {"/",    PURPLE,   KC_SLSH.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_NUHS = {"#",    PURPLE,   KC_NUHS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_NUBS = {"\\|",  PURPLE,   KC_NUBS.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_EURO = {"EURO", PURPLE,   KC_EURO.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 8. INTERNATIONAAL (10)
const Binding BTN_INT1 = {"INT1", DARKGREY, KC_INT1.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT2 = {"INT2", DARKGREY, KC_INT2.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT3 = {"INT3", DARKGREY, KC_INT3.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT4 = {"INT4", DARKGREY, KC_INT4.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_INT5 = {"INT5", DARKGREY, KC_INT5.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG1 = {"LNG1", BLUE,     KC_LNG1.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG2 = {"LNG2", BLUE,     KC_LNG2.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_KANA = {"KANA", BLUE,     KC_KANA.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_HIRA = {"HIRA", BLUE,     KC_HIRA.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LNG5 = {"LNG5", BLUE,     KC_LNG5.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 9. MODIFIERS RAW (8)
const Binding BTN_LCTL = {"LCTL", PURPLE,   KC_LCTL.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LSFT = {"LSFT", PURPLE,   KC_LSFT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LALT = {"LALT", PURPLE,   KC_LALT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_LGUI = {"LGUI", PURPLE,   KC_LGUI.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RCTL = {"RCTL", PURPLE,   KC_RCTL.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RSFT = {"RSFT", PURPLE,   KC_RSFT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RALT = {"RALT", PURPLE,   KC_RALT.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_RGUI = {"RGUI", PURPLE,   KC_RGUI.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};

// 10. EDIT SPECIALS (5)
const Binding BTN_UNDO = {"UNDO", ORANGE,   KC_UNDO.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_CUT  = {"CUT",  ORANGE,   KC_CUT.code,  HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_COPY = {"COPY", ORANGE,   KC_COPY.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_PSTE = {"PSTE", ORANGE,   KC_PSTE.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};
const Binding BTN_FIND = {"FIND", ORANGE,   KC_FIND.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};

// 11. SYSTEEM (3)
const Binding BTN_PWR  = {"PWR",  RED,      KC_PWR.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_SLP  = {"SLEEP",BLUE,     KC_SLP.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_WAKE = {"WAKE", GREEN,    KC_WAKE.code, HIDPage::KEYBOARD, MOD_NONE, BLACK};

// 12. CONSUMER / MEDIA (16)
const Binding BTN_MUTE = {"MUTE", RED,      MC_MUTE.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_VOLU = {"VOL+", DARKGREY, MC_VOLU.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_VOLD = {"VOL-", DARKGREY, MC_VOLD.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_PLAY = {"PLAY", GREEN,    MC_PLAY.code, HIDPage::CONSUMER, MOD_NONE, BLACK};
const Binding BTN_STOP = {"STOP", RED,      MC_STOP.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_NEXT = {"NEXT", BLUE,     MC_NEXT.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_PREV = {"PREV", BLUE,     MC_PREV.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_MAIL = {"MAIL", BLUE,     MC_MAIL.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_CALC = {"CALC", DARKGREY, MC_CALC.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_EXPL = {"EXPL", DARKGREY, MC_EXPL.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_WWW  = {"WWW",  BLUE,     MC_WWW.code,  HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_DOC  = {"DOC",  BLUE,     MC_CONF.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_XLS  = {"XLS",  GREEN,    MC_SPRD.code, HIDPage::CONSUMER, MOD_NONE, BLACK};
const Binding BTN_PPT  = {"PPT",  ORANGE,   MC_PRES.code, HIDPage::CONSUMER, MOD_NONE, BLACK};
const Binding BTN_CAL  = {"CAL",  RED,      MC_SCHD.code, HIDPage::CONSUMER, MOD_NONE, WHITE};
const Binding BTN_TASK = {"TASK", BLUE,     MC_TASK.code, HIDPage::CONSUMER, MOD_NONE, WHITE};

// 13. STATUS & LEEG (4)
const Binding BTN_NO    = {" ",    BLACK,    KC_NO.code,    HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_ROLL  = {"ROLL", DARKGREY, KC_ROLL.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_POST  = {"POST", DARKGREY, KC_POST.code,  HIDPage::KEYBOARD, MOD_NONE, WHITE};
const Binding BTN_UNDEF = {"UNKN", DARKGREY, KC_UNDEF.code, HIDPage::KEYBOARD, MOD_NONE, WHITE};
