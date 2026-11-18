#include <M5Unified.h>
#include "Config.h"
#include "SerialCmd.h"
#include "ui.h"
#include "HIDManager.h"

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
    else if (!strcmp(cmd, "SHOT") && !a) { Serial.print("OK "); ui_screenshot(Serial); }
    else if (!strcmp(cmd, "LAYER") && num(a, x) && !b) {
        if (x < 0 || x >= (int)ui_page_count()) Serial.println("ERR range");
        else { ui_set_layer(x); Serial.printf("OK LAYER %d\n", ui_layer()); }
    }
    else if (!strcmp(cmd, "TAP") && num(a, x) && num(b, y) && !extra) { ui_tap(x, y); Serial.println("OK TAP"); }
    else if (!strcmp(cmd, "KEY") && num(a, x) && num(b, y) && !extra)
        Serial.println(ui_press(x, y) ? "OK KEY" : "ERR key");
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
