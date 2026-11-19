#include "Settings.h"
#include <Preferences.h>
#include <algorithm>

namespace Settings {
namespace {
Preferences g_prefs;
bool g_open = false;
LedMode g_led = LedMode::Layers;
uint8_t g_bright = 40, g_bl = 128, g_prefer = 0, g_effect = 0;
uint16_t g_ledIdle = 0, g_dim = 60, g_off = 300;

void defaults() {
    g_led = LedMode::Layers; g_bright = 40; g_ledIdle = 0; g_bl = 128; g_dim = 60; g_off = 300; g_prefer = 0; g_effect = 0;
}

const char* const EFFECTS[] = {"none", "nightrider", "breath", "rainbow", "comet", "sparkle", "vu"};

void load() {
    defaults();
    if (!g_open) return;
    uint8_t m = g_prefs.getUChar("led", (uint8_t)LedMode::Layers);
    g_led = m <= 2 ? (LedMode)m : LedMode::Layers;
    g_bright = constrain(g_prefs.getUChar("bright", 40), 1, 120);
    g_ledIdle = constrain(g_prefs.getUShort("ledidle", 0), 0, 1440);
    g_bl = constrain(g_prefs.getUChar("bl", 128), 16, 255);
    g_dim = constrain(g_prefs.getUShort("dim", 60), 5, 3600);
    g_off = std::max<uint16_t>(g_prefs.getUShort("off", 300), g_dim + 10);
    g_prefer = g_prefs.getUChar("prefer", 0) <= 2 ? g_prefs.getUChar("prefer", 0) : 0;
    g_effect = g_prefs.getUChar("fx", 0) < (uint8_t)Effect::Count ? g_prefs.getUChar("fx", 0) : 0;
}

bool number(const char* v, long lo, long hi, long& out) {
    if (!v || !*v) return false;
    char* e; long x = strtol(v, &e, 10);
    if (*e || x < lo || x > hi) return false;
    out = x; return true;
}
}  // namespace

void begin() { g_open = g_prefs.begin("ng", false); load(); }

LedMode ledMode() { return g_led; }
uint8_t ledBright() { return g_bright; }
uint16_t ledIdleMin() { return g_ledIdle; }
uint8_t backlight() { return g_bl; }
uint16_t dimSec() { return g_dim; }
uint16_t offSec() { return g_off; }
uint8_t prefer() { return g_prefer; }
Effect effect() { return (Effect)g_effect; }
const char* effectName(Effect e) { return EFFECTS[(int)e < (int)Effect::Count ? (int)e : 0]; }

bool set(const char* key, const char* value) {
    if (!key || !value) return false;
    String k(key), v(value);
    k.toLowerCase(); v.toLowerCase();
    long x;
    if (k == "led") {
        if (v == "off") g_led = LedMode::Off; else if (v == "locks") g_led = LedMode::Locks; else if (v == "on" || v == "layers") g_led = LedMode::Layers; else return false;
        if (g_open) g_prefs.putUChar("led", (uint8_t)g_led);
    } else if (k == "bright") {
        if (!number(value, 1, 120, x)) return false;
        g_bright = (uint8_t)x; if (g_open) g_prefs.putUChar("bright", g_bright);
    } else if (k == "ledidle") {
        if (!number(value, 0, 1440, x)) return false;
        g_ledIdle = (uint16_t)x; if (g_open) g_prefs.putUShort("ledidle", g_ledIdle);
    } else if (k == "bl") {
        if (!number(value, 16, 255, x)) return false;
        g_bl = (uint8_t)x; if (g_open) g_prefs.putUChar("bl", g_bl);
    } else if (k == "dim") {
        if (!number(value, 5, 3600, x)) return false;
        g_dim = (uint16_t)x; if (g_off <= g_dim) g_off = g_dim + 10;
        if (g_open) { g_prefs.putUShort("dim", g_dim); g_prefs.putUShort("off", g_off); }
    } else if (k == "off") {
        if (!number(value, 15, 86400, x) || x <= g_dim) return false;
        g_off = (uint16_t)std::min<long>(x, 65535); if (g_open) g_prefs.putUShort("off", g_off);
    } else if (k == "prefer") {
        if (v == "usb") g_prefer = 0; else if (v == "ble") g_prefer = 1; else if (v == "both") g_prefer = 2; else return false;
        if (g_open) g_prefs.putUChar("prefer", g_prefer);
    } else if (k == "effect") {
        int found = -1;
        for (int i = 0; i < (int)Effect::Count; i++) if (v == EFFECTS[i]) found = i;
        if (found < 0) return false;
        g_effect = (uint8_t)found; if (g_open) g_prefs.putUChar("fx", g_effect);
    } else if (k == "defaults") {
        if (g_open) g_prefs.clear();
        defaults();
    } else return false;
    return true;
}

String summary() {
    static const char* const led[] = {"off", "locks", "on"};
    static const char* const pref[] = {"usb", "ble", "both"};
    char b[140];
    snprintf(b, sizeof(b), "led=%s bright=%u ledidle=%u bl=%u dim=%u off=%u prefer=%s effect=%s", led[(int)g_led], g_bright, g_ledIdle, g_bl, g_dim, g_off, pref[g_prefer], EFFECTS[g_effect]);
    return String(b);
}
}  // namespace Settings
