#include <M5Unified.h>
#include "Config.h"
#include "SerialCmd.h"
#include "ui.h"
#include "HIDManager.h"
#include "Lamps.h"
#include "Disk.h"
#include "BarFx.h"
#include "Telem.h"
#include "Settings.h"

namespace {
char g_buf[64];
size_t g_len = 0;
bool g_overflow = false;

void handle(char* line) {
    while (*line == ' ') line++;
    if (!*line) return;
    char* save = nullptr;
    char* cmd = strtok_r(line, " \t", &save);
    for (char* p = cmd; *p; p++) *p = toupper((unsigned char)*p);
    char* a = strtok_r(nullptr, " \t", &save);
    char* b = strtok_r(nullptr, " \t", &save);
    char* extra = strtok_r(nullptr, " \t", &save);
    auto num = [](char* s, int& v) {
        if (!s) return false;
        char* e; long x = strtol(s, &e, 10);
        if (*e || e == s) return false;
        v = (int)x; return true;
    };
    int x = 0, y = 0;

    if (!strcmp(cmd, "PING") && !a) Serial.println("OK PONG");
    else if (!strcmp(cmd, "INFO") && !a)
        Serial.printf("OK INFO board=%s name=%s fw=%s_%s w=%d h=%d pages=%d locks=%d\n", NG_BOARD_ID, K_DEVICE_NAME,
                      __DATE__, __TIME__, (int)M5.Display.width(), (int)M5.Display.height(),
                      (int)ui_page_count(), (int)ui_locks_page());
    else if (!strcmp(cmd, "STATE?") && !a) Serial.printf("OK %s\n", ui_state().c_str());
    // WAKE: restart the idle timers (screen and LED bar on again). BL <16..255|AUTO>: fixed backlight for tests and photos.
    else if (!strcmp(cmd, "WAKE") && !a) { ui_wake(); ui_leds_refresh(); Serial.println("OK WAKE"); }
    else if (!strcmp(cmd, "BL") && a && !b) {
        for (char* p = a; *p; p++) *p = toupper((unsigned char)*p);
        if (!strcmp(a, "AUTO")) { ui_backlight(-1); Serial.println("OK BL AUTO"); }
        else if (num(a, x) && x >= 0 && x <= 255) { ui_backlight(x); Serial.printf("OK BL %d\n", x); }
        else Serial.println("ERR bl");
    }
    // LEDS OFF|LOCKS|ON (stored): bar dark with its 5 V rail off | only the host lock state | layer colours. SET key value / SET? for the rest.
    else if (!strcmp(cmd, "LEDS") && a && !b) {
        for (char* p = a; *p; p++) *p = toupper((unsigned char)*p);
        if (Settings::set("led", a)) { ui_leds_refresh(); Serial.printf("OK LEDS %s\n", a); } else Serial.println("ERR leds");
    }
    else if (!strcmp(cmd, "LEDS?") && !a) Serial.printf("OK LEDS %s\n", Settings::summary().c_str());
    else if (!strcmp(cmd, "SET") && a && b && !extra) {
        if (Settings::set(a, b)) { ui_leds_refresh(); if (!strcasecmp(a, "prefer")) HIDManager::setPrefer((HIDManager::Prefer)Settings::prefer()); Serial.printf("OK SET %s\n", Settings::summary().c_str()); }
        else Serial.println("ERR set");
    }
    else if (!strcmp(cmd, "SET?") && !a) Serial.printf("OK SET %s\n", Settings::summary().c_str());
    else if (!strcmp(cmd, "PERF?") && !a) Serial.printf("OK %s\n", ui_perf().c_str());
    // PREFER USB: while USB is mounted, keys go over USB only (default).
    // PREFER BLE: while BLE is connected, keys go over BLE only (lets tests exercise BLE with USB attached).
    // PREFER BOTH: keys go to every connected transport.
    // ONLY USB / ONLY BLE: never use the other transport, even if the chosen one is not connected.
    else if (!strcmp(cmd, "PREFER") && a && !b) {
        for (char* p = a; *p; p++) *p = toupper((unsigned char)*p);
        if (!strcmp(a, "USB")) { HIDManager::setPreferUsb(true); Serial.println("OK PREFER USB"); }
        else if (!strcmp(a, "BLE")) { HIDManager::setPrefer(HIDManager::Prefer::BLE); Serial.println("OK PREFER BLE"); }
        else if (!strcmp(a, "BOTH")) { HIDManager::setPrefer(HIDManager::Prefer::BOTH); Serial.println("OK PREFER BOTH"); }
        else Serial.println("ERR prefer");
    }
    else if (!strcmp(cmd, "ONLY") && a && !b) {
        for (char* p = a; *p; p++) *p = toupper((unsigned char)*p);
        if (!strcmp(a, "USB")) { HIDManager::setPrefer(HIDManager::Prefer::USB_ONLY); Serial.println("OK ONLY USB"); }
        else if (!strcmp(a, "BLE")) { HIDManager::setPrefer(HIDManager::Prefer::BLE_ONLY); Serial.println("OK ONLY BLE"); }
        else Serial.println("ERR only");
    }
    else if (!strcmp(cmd, "PREFER?") && !a) {
        static const char* const names[] = {"USB", "BLE", "BOTH", "USB_ONLY", "BLE_ONLY"};
        Serial.printf("OK PREFER %s usb=%d ble=%d\n", names[(int)HIDManager::prefer()],
                      (int)HIDManager::isUsbConnected(), (int)HIDManager::isBleConnected());
    }
    // LampArray test bench (ADR 0010): LAMPS? | LAMPS RESET | LAMPS GRID c r | LAMPS GET id | LAMPS BRIGHT n
    else if (!strcmp(cmd, "LAMPS?") && !a) Serial.printf("OK %s\n", Lamps::status().c_str());
    else if (!strcmp(cmd, "LAMPS") && a) {
        for (char* p = a; *p; p++) *p = toupper((unsigned char)*p);
        if (!strcmp(a, "RESET") && !b) { Lamps::resetCounters(); ui_leds_refresh(); Serial.println("OK LAMPS RESET"); }
        else if (!strcmp(a, "GRID") && num(b, x) && num(extra, y)) {
            if (x > 0 && y > 0 && Lamps::setGrid(x, y)) Serial.printf("OK LAMPS GRID %d %d lamps=%u max=%u\n", x, y, (unsigned)Lamps::count(), (unsigned)Lamps::maxGridLamps());
            else Serial.println("ERR grid");
        }
        else if (!strcmp(a, "GET") && num(b, x) && !extra) {
            uint8_t v[4]; Lamps::getLamp(x < 0 ? 65535 : x, v);
            Serial.printf("OK LAMP %d %u %u %u %u\n", x, v[0], v[1], v[2], v[3]);
        }
        else if (!strcmp(a, "WORKER") && b && !extra) {
            for (char* p = b; *p; p++) *p = toupper((unsigned char)*p);
            if (!strcmp(b, "ON")) Lamps::setWorker(true); else if (!strcmp(b, "OFF")) Lamps::setWorker(false); else { Serial.println("ERR lamps"); return; }
            Serial.printf("OK LAMPS WORKER %s\n", Lamps::workerOn() ? "ON" : "OFF");
        }
        else if (!strcmp(a, "BRIGHT") && num(b, x) && !extra) { Lamps::setBarBrightness(x < 1 ? 1 : x > 255 ? 255 : x); ui_leds_refresh(); Serial.printf("OK LAMPS BRIGHT %u\n", (unsigned)Lamps::barBrightness()); }
        else Serial.println("ERR lamps");
    }
    // Disk demo (src/Disk.cpp): DISK? shows the state of the transfer, DISK RESET forgets it
    else if (!strcmp(cmd, "DISK?") && !a) Serial.printf("OK %s\n", Disk::status().c_str());
    else if (!strcmp(cmd, "DISK") && a && !b && !strcasecmp(a, "RESET")) { Disk::reset(); Serial.println("OK DISK RESET"); }
    else if (!strcmp(cmd, "DISK") && a && !b && !strcasecmp(a, "DIR")) Serial.printf("OK DIR\n%s", Disk::dir().c_str());
    else if (!strcmp(cmd, "DISK") && a && b && !extra && !strcasecmp(a, "SOUND") && (!strcasecmp(b, "ON") || !strcasecmp(b, "OFF"))) { Disk::setSound(!strcasecmp(b, "ON")); Serial.printf("OK DISK SOUND %s\n", b); }
    // Microphone (BarFx.cpp): MIC? gives level, peak and the dominant frequency; MIC ON|OFF samples without the vu effect
    else if (!strcmp(cmd, "MIC?") && !a) Serial.printf("OK %s\n", BarFx::micStatus().c_str());
    else if (!strcmp(cmd, "MIC") && a && !b && (!strcasecmp(a, "ON") || !strcasecmp(a, "OFF"))) { BarFx::micCommand(!strcasecmp(a, "ON")); Serial.printf("OK MIC %s\n", a); }
    // BAR?: the ten LED colours as hex, so that the demo window can mirror what an autonomous effect does
    else if (!strcmp(cmd, "BAR?") && !a) { uint32_t c[10]; ui_bar_colors(c); Serial.print("OK BAR"); for (int i = 0; i < 10; i++) Serial.printf(" %06lx", (unsigned long)c[i]); Serial.println(); }
    // WAVE?: the last microphone buffer, 512 signed 16-bit samples as hex (the demo window draws the oscilloscope on the lamp grid)
    else if (!strcmp(cmd, "WAVE?") && !a) { static int16_t w[BarFx::WAVE_SAMPLES]; BarFx::micWave(w); String o = "OK WAVE "; o.reserve(8 + 4 * BarFx::WAVE_SAMPLES + 2); char h[5]; for (int i = 0; i < BarFx::WAVE_SAMPLES; i++) { snprintf(h, sizeof(h), "%04x", (uint16_t)w[i]); o += h; } Serial.println(o); }
    // TELEM BAR|WAVE|BOTH|OFF: the bar colours and the microphone waveform go to the PC as game pad reports (the demo window reads them from the HID node)
    else if (!strcmp(cmd, "TELEM") && a && !b) {
        for (char* p = a; *p; p++) *p = toupper((unsigned char)*p);
        int m = !strcmp(a, "OFF") ? 0 : !strcmp(a, "BAR") ? 1 : !strcmp(a, "WAVE") ? 2 : !strcmp(a, "BOTH") ? 3 : -1;
        if (m < 0) Serial.println("ERR telem"); else { Telem::set((uint8_t)m); Serial.printf("OK TELEM %s\n", a); }
    }
    else if (!strcmp(cmd, "SD?") && !a) Serial.printf("OK %s\n", Disk::sdStatus().c_str());
    else if (!strcmp(cmd, "SHOT") && !a) { Serial.print("OK "); ui_screenshot(Serial); }
    else if (!strcmp(cmd, "LAYER") && num(a, x) && !b) {
        if (x < 0 || x >= (int)ui_page_count()) Serial.println("ERR range");
        else { ui_set_layer(x); Serial.printf("OK LAYER %d\n", ui_layer()); }
    }
#if NG_SERIAL_INPUT   // TAP and KEY really type on the host: build with -D NG_SERIAL_INPUT=0 to leave them out
    else if (!strcmp(cmd, "TAP") && num(a, x) && num(b, y) && !extra) Serial.println(ui_tap(x, y) ? "OK TAP" : "ERR tap");
    else if (!strcmp(cmd, "KEY") && num(a, x) && num(b, y) && !extra)
        Serial.println(ui_press(x, y, true) ? "OK KEY" : "ERR key");
#endif
    else Serial.println("ERR unknown");
}
} // namespace

void serialcmd_poll() {
    int guard = 256; // bound work per loop
    while (guard-- > 0 && Serial.available()) {
        int c = Serial.read();
        if (c < 0) break;
        if (c == '\r') continue;
        if (c == '\n') {
            if (g_overflow) Serial.println("ERR toolong");
            else { g_buf[g_len] = 0; handle(g_buf); }
            g_len = 0; g_overflow = false;
        } else if (g_len < sizeof(g_buf) - 1) g_buf[g_len++] = (char)c;
        else g_overflow = true;
    }
}
