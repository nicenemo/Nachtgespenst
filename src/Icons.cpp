#include <Arduino.h>
#include <string.h>
#include "Icons.h"
#include "Keymap.h"
#include "IconCodes.h"

extern "C" const uint8_t ng_icon_ttf[];
extern "C" const size_t ng_icon_ttf_len;

static lv_font_t* g_if[3];

void icons_init() {
    static const int px[3] = {20, 28, 36};
    for (int i = 0; i < 3; i++) g_if[i] = lv_tiny_ttf_create_data(ng_icon_ttf, ng_icon_ttf_len, px[i]);
}

const lv_font_t* icons_font(int px) { return px <= 20 ? g_if[0] : px <= 28 ? g_if[1] : g_if[2]; }

namespace {
// Modifier masks, folded so that left and right variants are the same (bit0 Ctrl, 1 Shift, 2 Alt, 3 Win).
constexpr uint8_t CT = 1, SH = 2, AL = 4, GU = 8, HYP = 15, MEH = 7;

struct Entry {
    HID::Page page; uint16_t id;
    uint8_t need;      // modifiers that must be present
    uint8_t implied;   // part of the icon's meaning (not shown in the badge)
    const char* layer; // restrict to the layer with this title (nullptr = any)
    const char* icon;  // glyph or nullptr
    const char* text;  // text override or nullptr
    const char* cap;   // caption or nullptr
};
#define E(b, need, imp, layer, icon, text, cap) { (b).page, (b).usage.id, need, imp, layer, icon, text, cap }

// First match with the most specific (layer, then most modifier bits) wins.
const Entry TABLE[] = {
    // --- clipboard / editing: Ctrl is implied ---
    E(kp_C, CT, CT, nullptr, IC_CONTENT_COPY, nullptr, "Copy"),
    E(kp_V, CT, CT, nullptr, IC_CONTENT_PASTE, nullptr, "Paste"),
    E(kp_X, CT, CT, nullptr, IC_CONTENT_CUT, nullptr, "Cut"),
    E(kp_Z, CT, CT, nullptr, IC_UNDO, nullptr, "Undo"),
    E(kp_Y, CT, CT, nullptr, IC_REDO, nullptr, "Redo"),
    E(kp_F, CT, CT, nullptr, IC_SEARCH, nullptr, "Find"),
    E(kp_A, CT, CT, nullptr, IC_SELECT_ALL, nullptr, "All"),
    E(kp_S, CT, CT, nullptr, IC_SAVE, nullptr, "Save"),
    E(kp_P, CT, CT, nullptr, IC_PRINT, nullptr, "Print"),
    // --- Office/Prod (Hyper+letter launchers; mapping inferred from the layer title) ---
    E(kp_W, HYP, 0, "Office/Prod", IC_DESCRIPTION, nullptr, "Word"),
    E(kp_X, HYP, 0, "Office/Prod", IC_TABLE_CHART, nullptr, "Excel"),
    E(kp_P, HYP, 0, "Office/Prod", IC_SLIDESHOW, nullptr, "PowerPoint"),
    E(kp_O, HYP, 0, "Office/Prod", IC_MAIL, nullptr, "Outlook"),
    E(kp_T, HYP, 0, "Office/Prod", IC_GROUPS, nullptr, "Teams"),
    E(kp_N, HYP, 0, "Office/Prod", IC_STICKY_NOTE_2, nullptr, "OneNote"),
    E(kp_B, HYP, 0, "Office/Prod", IC_PUBLIC, nullptr, "Browser"),
    E(kp_E, HYP, 0, "Office/Prod", IC_FOLDER, nullptr, "Explorer"),
    E(kp_L, HYP, 0, "Office/Prod", IC_LOCK, nullptr, "Lock"),
    // --- navigation / basic keys ---
    E(kp_UPARROW, 0, 0, nullptr, IC_ARROW_UPWARD, nullptr, nullptr),
    E(kp_DOWNARROW, 0, 0, nullptr, IC_ARROW_DOWNWARD, nullptr, nullptr),
    E(kp_LEFTARROW, 0, 0, nullptr, IC_ARROW_BACK, nullptr, nullptr),
    E(kp_RIGHTARROW, 0, 0, nullptr, IC_ARROW_FORWARD, nullptr, nullptr),
    E(kp_HOME, 0, 0, nullptr, IC_FIRST_PAGE, nullptr, "Home"),
    E(kp_END, 0, 0, nullptr, IC_LAST_PAGE, nullptr, "End"),
    E(kp_PAGEUP, 0, 0, nullptr, IC_KEYBOARD_DOUBLE_ARROW_UP, nullptr, "PgUp"),
    E(kp_PAGEDOWN, 0, 0, nullptr, IC_KEYBOARD_DOUBLE_ARROW_DOWN, nullptr, "PgDn"),
    E(kp_INSERT, 0, 0, nullptr, nullptr, "Ins", nullptr),
    E(kp_DELETE_FORWARD, 0, 0, nullptr, nullptr, "Del", nullptr),
    E(kp_DELETE, 0, 0, nullptr, IC_BACKSPACE, nullptr, "Bksp"),
    E(kp_RETURN_ENTER, 0, 0, nullptr, IC_KEYBOARD_RETURN, nullptr, "Enter"),
    E(kp_KP_ENTER, 0, 0, nullptr, IC_KEYBOARD_RETURN, nullptr, "Enter"),
    E(kp_TAB, 0, 0, nullptr, IC_KEYBOARD_TAB, nullptr, "Tab"),
    E(kp_ESCAPE, 0, 0, nullptr, nullptr, "Esc", nullptr),
    E(kp_SPACEBAR, 0, 0, nullptr, IC_SPACE_BAR, nullptr, "Space"),
    E(kp_CAPS_LOCK, 0, 0, nullptr, IC_KEYBOARD_CAPSLOCK, nullptr, "Caps"),
    E(kp_LOCKING_CAPS_LOCK, 0, 0, nullptr, IC_KEYBOARD_CAPSLOCK, nullptr, "Caps"),
    E(kp_LOCKING_NUM_LOCK, 0, 0, nullptr, IC_DIALPAD, nullptr, "Num"),
    E(kp_LOCKING_SCROLL_LOCK, 0, 0, nullptr, IC_LOCK, nullptr, "Scroll"),
    E(kp_KP_NUM_LOCK_AND_CLEAR, 0, 0, nullptr, IC_DIALPAD, nullptr, "Num"),
    E(kp_SCROLL_LOCK, 0, 0, nullptr, IC_LOCK, nullptr, "ScrLk"),
    E(kp_PRINTSCREEN, 0, 0, nullptr, IC_SCREENSHOT_MONITOR, nullptr, "PrtSc"),
    E(kp_PAUSE, 0, 0, nullptr, IC_PAUSE, nullptr, "Pause"),
    E(kp_APPLICATION, 0, 0, nullptr, IC_MENU, nullptr, "Menu"),
    // --- layer specific function keys ---
    E(kp_F5, 0, 0, "PowerPoint", IC_PLAY_ARROW, nullptr, "Start"),
    E(kp_F5, SH, 0, "PowerPoint", IC_PLAY_CIRCLE, nullptr, "Here"),
    E(kp_F5, 0, 0, "Browser Ctrl", IC_REFRESH, nullptr, "Reload"),
    E(kp_R, CT | SH, 0, "Browser Ctrl", IC_REFRESH, nullptr, "Hard"),
    E(kp_T, CT | SH, 0, "Browser Ctrl", IC_RESTORE_PAGE, nullptr, "Reopen"),
    E(kp_W, CT | SH, 0, "Browser Ctrl", IC_CLOSE, nullptr, "Close"),
    E(kp_F11, 0, 0, "Browser Ctrl", IC_FULLSCREEN, nullptr, "Full"),
    E(kp_F5, 0, 0, "Debug IDE", IC_PLAY_ARROW, nullptr, "Run"),
    E(kp_F5, CT | SH, 0, "Debug IDE", IC_RESTART_ALT, nullptr, "Restart"),
    E(kp_F9, 0, 0, "Debug IDE", IC_RADIO_BUTTON_CHECKED, nullptr, "Break"),
    E(kp_F10, 0, 0, "Debug IDE", IC_REDO, nullptr, "Over"),
    E(kp_F11, 0, 0, "Debug IDE", IC_SUBDIRECTORY_ARROW_RIGHT, nullptr, "Into"),
    E(kp_F11, SH, 0, "Debug IDE", IC_NORTH, nullptr, "Out"),
    E(kp_M, CT | SH, 0, "MS Teams", IC_MIC_OFF, nullptr, "Mute"),
    E(kp_O, CT | SH, 0, "MS Teams", IC_VIDEOCAM, nullptr, "Video"),
    E(kp_D, CT | SH, 0, "MS Teams", IC_CALL_END, nullptr, "Decline"),
    E(kp_R, CT | SH, 0, "MS Outlook", IC_REPLY_ALL, nullptr, "Reply all"),
    // --- consumer / system ---
    E(cp_SCAN_PREVIOUS_TRACK, 0, 0, nullptr, IC_SKIP_PREVIOUS, nullptr, "Prev"),
    E(cp_SCAN_NEXT_TRACK, 0, 0, nullptr, IC_SKIP_NEXT, nullptr, "Next"),
    E(cp_PLAY_PAUSE, 0, 0, nullptr, IC_PLAY_ARROW, nullptr, "Play/Pause"),
    E(cp_PLAY, 0, 0, nullptr, IC_PLAY_ARROW, nullptr, "Play"),
    E(cp_STOP, 0, 0, nullptr, IC_STOP, nullptr, "Stop"),
    E(cp_MUTE, 0, 0, nullptr, IC_VOLUME_OFF, nullptr, "Mute"),
    E(cp_VOLUME_DECREMENT, 0, 0, nullptr, IC_VOLUME_DOWN, nullptr, "Vol -"),
    E(cp_VOLUME_INCREMENT, 0, 0, nullptr, IC_VOLUME_UP, nullptr, "Vol +"),
    E(cp_BRIGHTNESS_DECREMENT, 0, 0, nullptr, IC_BRIGHTNESS_LOW, nullptr, "Dim"),
    E(cp_BRIGHTNESS_INCREMENT, 0, 0, nullptr, IC_BRIGHTNESS_HIGH, nullptr, "Bright"),
    E(cp_EMAIL_READER, 0, 0, nullptr, IC_MAIL, nullptr, "Mail"),
    E(cp_LOCAL_MACHINE_BROWSER, 0, 0, nullptr, IC_FOLDER, nullptr, "Files"),
    E(cp_INTERNET_BROWSER, 0, 0, nullptr, IC_LANGUAGE, nullptr, "Browser"),
    E(cp_CALENDAR_SCHEDULE, 0, 0, nullptr, IC_CALENDAR_MONTH, nullptr, "Calendar"),
    E(cp_PROCESS_TASK_MANAGER, 0, 0, nullptr, IC_MEMORY, nullptr, "Tasks"),
    E(cp_CALCULATOR, 0, 0, nullptr, IC_CALCULATE, nullptr, "Calc"),
    E(gp_SYS_POWER_DOWN, 0, 0, nullptr, IC_POWER_SETTINGS_NEW, nullptr, "Power"),
    E(gp_SYS_SLEEP, 0, 0, nullptr, IC_BEDTIME, nullptr, "Sleep"),
    E(gp_SYS_WAKE_UP, 0, 0, nullptr, IC_WB_SUNNY, nullptr, "Wake"),
};

uint8_t fold(uint8_t m) { return (m | (m >> 4)) & 0x0F; }

void modText(char* out, size_t n, uint8_t m, bool plus) {
    out[0] = 0;
    if (!m) return;
    if (m == HYP) { snprintf(out, n, "%sHyper", plus ? "+" : ""); return; }
    if (m == MEH) { snprintf(out, n, "%sMeh", plus ? "+" : ""); return; }
    size_t l = 0;
    auto add = [&](bool on, const char* s) {
        if (!on) return;
        l += snprintf(out + l, n - l, "%s%s", (l || plus) ? "+" : "", s);
    };
    add(m & CT, "Ctrl"); add(m & SH, "Shift"); add(m & AL, "Alt"); add(m & GU, "Win");
}

// Printed character of a US-layout keyboard usage; returns false if the usage is not a printable key.
bool printable(uint16_t id, bool shift, char* out) {
    static const char sh[] = "!@#$%^&*()";
    if (id >= 0x04 && id <= 0x1D) { out[0] = 'A' + (id - 4); out[1] = 0; return true; }
    if (id >= 0x1E && id <= 0x27) { out[0] = shift ? sh[id - 0x1E] : (id == 0x27 ? '0' : '1' + (id - 0x1E)); out[1] = 0; return true; }
    static const char plain[] = "-=[]\\#;'`,./";
    static const char shifted[] = "_+{}|~:\"~<>?";
    if (id >= 0x2D && id <= 0x38) { int i = id - 0x2D; out[0] = shift ? shifted[i] : plain[i]; out[1] = 0; return true; }
    if (id >= 0x54 && id <= 0x57) { out[0] = "/*-+"[id - 0x54]; out[1] = 0; return true; }
    if (id >= 0x59 && id <= 0x61) { out[0] = '1' + (id - 0x59); out[1] = 0; return true; }
    if (id == 0x62) { strcpy(out, "0"); return true; }
    if (id == 0x63) { strcpy(out, "."); return true; }
    if (id == 0x85) { strcpy(out, ","); return true; }
    if (id >= 0x3A && id <= 0x45) { snprintf(out, 6, "F%d", id - 0x3A + 1); return true; }
    if (id >= 0x68 && id <= 0x73) { snprintf(out, 6, "F%d", id - 0x68 + 13); return true; }
    return false;
}
} // namespace

TileLook icons_look(const char* layerTitle, const Binding& b) {
    TileLook t{};
    uint8_t m = fold(b.modifier);
    const Entry* best = nullptr; int bestScore = -1;
    for (const Entry& e : TABLE) {
        if (e.page != b.page || e.id != b.usage.id) continue;
        if ((e.need & ~m) != 0) continue;
        if (e.implied && (m & (AL | GU))) continue; // e.g. Hyper+Y is not Ctrl+Y
        if (e.layer && (!layerTitle || strcmp(e.layer, layerTitle) != 0)) continue;
        int s = (e.layer ? 16 : 0) + __builtin_popcount(e.need);
        if (s > bestScore) { best = &e; bestScore = s; }
    }
    if (best) {
        t.icon = best->icon; t.caption = best->cap;
        if (best->text) snprintf(t.text, sizeof t.text, "%s", best->text);
        modText(t.badge, sizeof t.badge, m & ~best->implied, best->implied != 0);
        return t;
    }
    if (b.label) {
        snprintf(t.text, sizeof t.text, "%s", b.label);
        modText(t.badge, sizeof t.badge, m, false);
        return t;
    }
    char c[8];
    bool isKbd = (b.page == HID::Page::KEYBOARD_KEYPAD);
    bool shiftUsed = false;
    if (isKbd && printable(b.usage.id, (m & SH) != 0, c)) {
        // Shifted number/symbol keys are shown as the symbol they produce.
        bool sym = (b.usage.id >= 0x1E && b.usage.id <= 0x27) || (b.usage.id >= 0x2D && b.usage.id <= 0x38);
        shiftUsed = sym && (m & SH);
        snprintf(t.text, sizeof t.text, "%s", c);
    } else {
        // Fallback: the generated identifier, underscores to spaces.
        snprintf(t.text, sizeof t.text, "%s", b.usage.identifier ? b.usage.identifier : "?");
        for (char* p = t.text; *p; p++) if (*p == '_') *p = ' ';
    }
    modText(t.badge, sizeof t.badge, shiftUsed ? (m & ~SH) : m, false);
    return t;
}
