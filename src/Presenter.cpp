#include <M5Unified.h>
#include <Preferences.h>
#include "Config.h"
#include "Presenter.h"
#include "HIDManager.h"
#include "HID_Specs.h"
#include "HID_Shortcuts.h"
#include "Bindings.h"

namespace Presenter {
namespace {
Preferences g_prefs;
bool g_enabled = false;               // the pointer is off until it is switched on (B or the tile)
float g_gain = 20.0f;                 // pixels per degree of rotation
uint16_t g_idleMs = 2000;             // still for this long = the pointer falls asleep
bool g_upright = false;               // hold: false = flat (screen up), true = upright (like a phone)
bool g_flipX = false, g_flipY = false;
Ptr g_state = Ptr::Off;
uint32_t g_reports = 0, g_clicks = 0, g_scrolls = 0;
uint8_t g_mods = 0; uint32_t g_modSent = 0; int g_bleWas = 0; uint8_t g_clickButton = 0;
uint32_t g_lastMove = 0, g_clickRelAt = 0;
float g_bias[3] = {0, 0, 0};
float g_accX = 0, g_accY = 0;
float g_feed[3] = {0, 0, 0}; uint32_t g_feedUntil = 0;
constexpr float WAKE_DPS = 25.0f;     // a deliberate movement wakes the pointer, a bump or the noise does not
constexpr float MOVING_DPS = 5.0f;    // above this the board counts as moving
constexpr float DEAD_DPS = 0.8f;      // below this nothing moves the cursor
int g_wakeCount = 0;

void sendMouse(uint8_t buttons, int dx, int dy, int wheel = 0) {
    dx = dx > 127 ? 127 : dx < -127 ? -127 : dx; dy = dy > 127 ? 127 : dy < -127 ? -127 : dy; wheel = wheel > 127 ? 127 : wheel < -127 ? -127 : wheel;
#if NG_PRESENTER
    uint8_t d[4] = {buttons, (uint8_t)(int8_t)dx, (uint8_t)(int8_t)dy, (uint8_t)(int8_t)wheel};
    HIDManager::sendRaw(HID::Reports::MOUSE, d, 4);
#else
    (void)buttons; (void)dx; (void)dy; (void)wheel;
#endif
}

void sendMods() {
#if NG_PRESENTER
    uint8_t rep[8] = {(uint8_t)((g_mods & M_CTRL ? 0x01 : 0) | (g_mods & M_SHIFT ? 0x02 : 0) | (g_mods & M_ALT ? 0x04 : 0) | (g_mods & M_WIN ? 0x08 : 0)), 0, 0, 0, 0, 0, 0, 0};
    HIDManager::sendRaw(HID::Reports::KEYBOARD, rep, 8);       // a keyboard report with only modifiers: they are held until the next report
#endif
    g_modSent = millis();
}
}

void begin() {
    g_prefs.begin("ngpres", false);
    g_enabled = g_prefs.getBool("on", false);
    g_gain = g_prefs.getFloat("gain", 20.0f);
    g_idleMs = g_prefs.getUShort("idle", 2000);
    g_upright = g_prefs.getBool("up", false);
    g_flipX = g_prefs.getBool("fx", false); g_flipY = g_prefs.getBool("fy", false);
    g_state = g_enabled ? Ptr::Asleep : Ptr::Off;
}

const char* name(Action a) {
    static const char* const n[] = {"prev", "next", "start", "blank", "end", "laser"};
    return n[(int)a];
}

static Binding bindingFor(Action a) {
    switch (a) {
        case Action::Prev:  return kp_PAGEUP;
        case Action::Next:  return kp_PAGEDOWN;
        case Action::Start: return kp_F5;
        case Action::Blank: return kp_B;
        case Action::End:   return kp_ESCAPE;
        default:            return lct(kp_L);       // the laser pointer of PowerPoint
    }
}

bool send(Action a) {
    Binding b = bindingFor(a);
    if (!HIDManager::canSend(b)) return false;
    HIDManager::send(b);
    return true;
}

bool click(uint8_t button) {
    if (g_clickRelAt) return false;
    g_clickButton = button ? button : 1;
    sendMouse(g_clickButton, 0, 0); g_clicks++; g_clickRelAt = millis() + 25; if (!g_clickRelAt) g_clickRelAt = 1;
    return true;
}

void scroll(int ticks) { if (!ticks) return; sendMouse(0, 0, 0, ticks); g_scrolls++; }
uint32_t scrolls() { return g_scrolls; }
uint8_t modifiers() { return g_mods; }
void toggleModifiers(uint8_t mask) { g_mods = (g_mods == mask) ? 0 : mask; sendMods(); }
void releaseModifiers() { if (g_mods) { g_mods = 0; sendMods(); } }

Ptr pointer() { return g_state; }
bool pointerEnabled() { return g_enabled; }
uint32_t reports() { return g_reports; }
uint32_t clicks() { return g_clicks; }
const char* pointerName() { return g_state == Ptr::Off ? "off" : g_state == Ptr::Asleep ? "still" : "moving"; }

void setPointerEnabled(bool on) {
    g_enabled = on; g_prefs.putBool("on", on);
    g_state = on ? Ptr::Asleep : Ptr::Off; g_accX = g_accY = 0; g_wakeCount = 0;
}
void togglePointer() { setPointerEnabled(!g_enabled); }

void feedRaw(float gx, float gy, float gz, uint32_t ms) { g_feed[0] = gx; g_feed[1] = gy; g_feed[2] = gz; g_feedUntil = millis() + ms; }
void feed(float yawDps, float pitchDps, uint32_t ms) { feedRaw(pitchDps, 0, yawDps, ms); }

void loop() {
    uint32_t now = millis();
    if (g_clickRelAt && (int32_t)(now - g_clickRelAt) >= 0) { sendMouse(0, 0, 0); g_clickRelAt = 0; }
    int ble = (int)HIDManager::isBleConnected();
    if (g_bleWas != ble) { g_bleWas = ble; if (!ble) g_mods = 0; else if (g_mods) sendMods(); }      // the host is gone: nothing may stay held
    if (g_mods && now - g_modSent > 1000) sendMods();                                              // a key report of the clicker lets go of the modifiers: hold them again
    static uint32_t last = 0;
    if (now - last < 8) return;
    float dt = (now - last) / 1000.0f; last = now;
    if (dt > 0.1f) dt = 0.1f;
    if (!g_enabled) { g_state = Ptr::Off; return; }

    float yaw, pitch;
    if ((int32_t)(g_feedUntil - now) > 0) { yaw = g_upright ? g_feed[1] : g_feed[2]; pitch = g_feed[0]; }
    else {
        float g[3] = {0, 0, 0};
        if (!M5.Imu.isEnabled() || !M5.Imu.update()) return;
        auto d = M5.Imu.getImuData();
        g[0] = d.gyro.x; g[1] = d.gyro.y; g[2] = d.gyro.z;
        float rate = fabsf(g[0] - g_bias[0]) + fabsf(g[1] - g_bias[1]) + fabsf(g[2] - g_bias[2]);
        if (rate < 3.0f) for (int i = 0; i < 3; i++) g_bias[i] += (g[i] - g_bias[i]) * 0.02f;   // held still: learn the zero point of the gyroscope
        float x = g[0] - g_bias[0], y = g[1] - g_bias[1], z = g[2] - g_bias[2];
        yaw = g_upright ? y : z; pitch = x;
    }
    float motion = fmaxf(fabsf(yaw), fabsf(pitch));

    if (g_state == Ptr::Asleep) {
        if (motion > WAKE_DPS) { if (++g_wakeCount >= 2) { g_state = Ptr::Active; g_lastMove = now; g_wakeCount = 0; } }
        else g_wakeCount = 0;
        if (g_state == Ptr::Asleep) return;                // asleep: nothing is sent
    }
    if (motion > MOVING_DPS) g_lastMove = now;
    else if (now - g_lastMove > g_idleMs) { g_state = Ptr::Asleep; g_accX = g_accY = 0; return; }

    if (fabsf(yaw) < DEAD_DPS) yaw = 0;
    if (fabsf(pitch) < DEAD_DPS) pitch = 0;
    g_accX += (g_flipX ? yaw : -yaw) * g_gain * dt;        // turning left moves the pointer left
    g_accY += (g_flipY ? pitch : -pitch) * g_gain * dt;    // tilting the nose up moves it up
    int dx = (int)g_accX, dy = (int)g_accY;
    if (dx || dy) { g_accX -= dx; g_accY -= dy; sendMouse(0, dx, dy); g_reports++; }
}

bool set(const char* key, const char* value) {
    float f = atof(value);
    if (!strcmp(key, "gain") && f >= 1 && f <= 100) { g_gain = f; g_prefs.putFloat("gain", f); return true; }
    if (!strcmp(key, "idle") && f >= 0.5f && f <= 30) { g_idleMs = (uint16_t)(f * 1000); g_prefs.putUShort("idle", g_idleMs); return true; }
    bool on = !strcmp(value, "on"), isBool = on || !strcmp(value, "off");
    if (!isBool) return false;
    if (!strcmp(key, "hold")) return false;
    if (!strcmp(key, "upright")) { g_upright = on; g_prefs.putBool("up", on); return true; }
    if (!strcmp(key, "flipx")) { g_flipX = on; g_prefs.putBool("fx", on); return true; }
    if (!strcmp(key, "flipy")) { g_flipY = on; g_prefs.putBool("fy", on); return true; }
    return false;
}

String status() {
    char b[200];
    snprintf(b, sizeof(b), "PRESENTER pointer=%s gain=%.0f idle=%.1f upright=%d flipx=%d flipy=%d reports=%lu clicks=%lu scrolls=%lu mods=%u ble=%d",
             pointerName(), g_gain, g_idleMs / 1000.0f, g_upright, g_flipX, g_flipY, (unsigned long)g_reports, (unsigned long)g_clicks, (unsigned long)g_scrolls, (unsigned)g_mods, (int)HIDManager::isBleConnected());
    return String(b);
}
}
