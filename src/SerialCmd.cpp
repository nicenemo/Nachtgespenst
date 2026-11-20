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
#include "ShellHelp.h"
#include "Presenter.h"

// The shell of the boards. Commands and their arguments are lower case, nothing else is accepted. Replies are "OK ..." or "ERR ...". The manual is docs/shell/commands.txt
// (tools/Manual.java generates ShellHelp.h from it; the same file is help.txt on the SD card, where "help <command>" reads the full text).
namespace {
char g_buf[96];
size_t g_len = 0;
bool g_overflow = false;

String up(const char* s) { String r(s ? s : ""); r.toUpperCase(); return r; }

bool num(const char* s, int& v) {
    if (!s) return false;
    char* e; long x = strtol(s, &e, 10);
    if (*e || e == s) return false;
    v = (int)x; return true;
}

// help: the list, or one command (the text from help.txt on the SD card, otherwise the usage line and the summary built into the firmware).
void help(const char* name) {
    if (!name) {
        Serial.println("OK HELP");
        for (int i = 0; i < SHELL_HELP_COUNT; i++) Serial.printf("%-8s %s\n", SHELL_HELP[i].name, SHELL_HELP[i].summary);
        Serial.println("help <command> explains one command");
        return;
    }
    for (int i = 0; i < SHELL_HELP_COUNT; i++) {
        if (strcmp(SHELL_HELP[i].name, name)) continue;
        Serial.printf("OK HELP %s\n", name);
        if (!Disk::helpSection(name, Serial)) Serial.printf("usage: %s\n%s\n", SHELL_HELP[i].usage, SHELL_HELP[i].summary);
        return;
    }
    Serial.println("ERR help unknown");
}

void handle(char* line) {
    while (*line == ' ') line++;
    if (!*line) return;
    char* save = nullptr;
    char* cmd = strtok_r(line, " \t", &save);
    char* a = strtok_r(nullptr, " \t", &save);
    char* b = strtok_r(nullptr, " \t", &save);
    char* extra = strtok_r(nullptr, " \t", &save);
    int x = 0, y = 0;
    auto is = [&](const char* n) { return !strcmp(cmd, n); };

    if (is("help") && !b) help(a);
    else if (is("ping") && !a) Serial.println("OK PONG");
    else if (is("info") && !a)
        Serial.printf("OK INFO board=%s name=%s fw=%s_%s w=%d h=%d pages=%d locks=%d presenter=%d\n", NG_BOARD_ID, K_DEVICE_NAME,
                      __DATE__, __TIME__, (int)M5.Display.width(), (int)M5.Display.height(),
                      (int)ui_page_count(), (int)ui_locks_page(), NG_PRESENTER);
    else if (is("state") && !a) Serial.printf("OK %s\n", ui_state().c_str());
    else if (is("wake") && !a) { ui_wake(); ui_leds_refresh(); Serial.println("OK WAKE"); }
    else if (is("bl") && a && !b) {
        if (!strcmp(a, "auto")) { ui_backlight(-1); Serial.println("OK BL AUTO"); }
        else if (num(a, x) && x >= 0 && x <= 255) { ui_backlight(x); Serial.printf("OK BL %d\n", x); }
        else Serial.println("ERR bl");
    }
    else if (is("leds") && !a) Serial.printf("OK LEDS %s\n", Settings::summary().c_str());
    else if (is("leds") && a && !b) {
        if (Settings::set("led", a)) { ui_leds_refresh(); Serial.printf("OK LEDS %s\n", up(a).c_str()); } else Serial.println("ERR leds");
    }
    else if (is("set") && !a) Serial.printf("OK SET %s\n", Settings::summary().c_str());
    else if (is("set") && a && b && !extra) {
        if (Settings::set(a, b)) { ui_leds_refresh(); if (!strcmp(a, "prefer")) HIDManager::setPrefer((HIDManager::Prefer)Settings::prefer()); Serial.printf("OK SET %s\n", Settings::summary().c_str()); }
        else Serial.println("ERR set");
    }
    else if (is("perf") && !a) Serial.printf("OK %s\n", ui_perf().c_str());
    else if (is("prefer") && !a) {
        static const char* const names[] = {"USB", "BLE", "BOTH", "USB_ONLY", "BLE_ONLY"};
        Serial.printf("OK PREFER %s usb=%d ble=%d\n", names[(int)HIDManager::prefer()], (int)HIDManager::isUsbConnected(), (int)HIDManager::isBleConnected());
    }
    else if (is("prefer") && a && !b) {
        if (!strcmp(a, "usb")) { HIDManager::setPreferUsb(true); Serial.println("OK PREFER USB"); }
        else if (!strcmp(a, "ble")) { HIDManager::setPrefer(HIDManager::Prefer::BLE); Serial.println("OK PREFER BLE"); }
        else if (!strcmp(a, "both")) { HIDManager::setPrefer(HIDManager::Prefer::BOTH); Serial.println("OK PREFER BOTH"); }
        else Serial.println("ERR prefer");
    }
    else if (is("only") && a && !b) {
        if (!strcmp(a, "usb")) { HIDManager::setPrefer(HIDManager::Prefer::USB_ONLY); Serial.println("OK ONLY USB"); }
        else if (!strcmp(a, "ble")) { HIDManager::setPrefer(HIDManager::Prefer::BLE_ONLY); Serial.println("OK ONLY BLE"); }
        else Serial.println("ERR only");
    }
    else if (is("lamps") && !a) Serial.printf("OK %s\n", Lamps::status().c_str());
    else if (is("lamps") && a) {
        if (!strcmp(a, "reset") && !b) { Lamps::resetCounters(); ui_leds_refresh(); Serial.println("OK LAMPS RESET"); }
        else if (!strcmp(a, "grid") && num(b, x) && num(extra, y)) {
            if (x > 0 && y > 0 && Lamps::setGrid(x, y)) Serial.printf("OK LAMPS GRID %d %d lamps=%u max=%u\n", x, y, (unsigned)Lamps::count(), (unsigned)Lamps::maxGridLamps());
            else Serial.println("ERR grid");
        }
        else if (!strcmp(a, "get") && num(b, x) && !extra) {
            uint8_t v[4]; Lamps::getLamp(x < 0 ? 65535 : x, v);
            Serial.printf("OK LAMP %d %u %u %u %u\n", x, v[0], v[1], v[2], v[3]);
        }
        else if (!strcmp(a, "worker") && b && !extra) {
            if (!strcmp(b, "on")) Lamps::setWorker(true); else if (!strcmp(b, "off")) Lamps::setWorker(false); else { Serial.println("ERR lamps"); return; }
            Serial.printf("OK LAMPS WORKER %s\n", Lamps::workerOn() ? "ON" : "OFF");
        }
        else if (!strcmp(a, "bright") && num(b, x) && !extra) { Lamps::setBarBrightness(x < 1 ? 1 : x > 255 ? 255 : x); ui_leds_refresh(); Serial.printf("OK LAMPS BRIGHT %u\n", (unsigned)Lamps::barBrightness()); }
        else Serial.println("ERR lamps");
    }
    else if (is("disk") && !a) Serial.printf("OK %s\n", Disk::status().c_str());
    else if (is("disk") && a && !b && !strcmp(a, "reset")) { Disk::reset(); Serial.println("OK DISK RESET"); }
    else if (is("disk") && a && !b && !strcmp(a, "dir")) Serial.printf("OK DIR\n%s", Disk::dir().c_str());
    else if (is("disk") && a && b && !extra && !strcmp(a, "sound") && (!strcmp(b, "on") || !strcmp(b, "off"))) { Disk::setSound(!strcmp(b, "on")); Serial.printf("OK DISK SOUND %s\n", up(b).c_str()); }
    else if (is("sd") && !a) Serial.printf("OK %s\n", Disk::sdStatus().c_str());
    else if (is("mic") && !a) Serial.printf("OK %s\n", BarFx::micStatus().c_str());
    else if (is("mic") && a && !b && (!strcmp(a, "on") || !strcmp(a, "off"))) { BarFx::micCommand(!strcmp(a, "on")); Serial.printf("OK MIC %s\n", up(a).c_str()); }
    else if (is("bar") && !a) { uint32_t c[10]; ui_bar_colors(c); Serial.print("OK BAR"); for (int i = 0; i < 10; i++) Serial.printf(" %06lx", (unsigned long)c[i]); Serial.println(); }
    else if (is("wave") && !a) {
        static int16_t w[BarFx::WAVE_SAMPLES]; BarFx::micWave(w);
        String o = "OK WAVE "; o.reserve(8 + 4 * BarFx::WAVE_SAMPLES + 2); char h[5];
        for (int i = 0; i < BarFx::WAVE_SAMPLES; i++) { snprintf(h, sizeof(h), "%04x", (uint16_t)w[i]); o += h; }
        Serial.println(o);
    }
    else if (is("telem") && !a) { static const char* const n[] = {"OFF", "BAR", "WAVE", "BOTH"}; Serial.printf("OK TELEM %s\n", n[Telem::mode() & 3]); }
    else if (is("telem") && a && !b) {
        int m = !strcmp(a, "off") ? 0 : !strcmp(a, "bar") ? 1 : !strcmp(a, "wave") ? 2 : !strcmp(a, "both") ? 3 : -1;
        if (m < 0) Serial.println("ERR telem"); else { Telem::set((uint8_t)m); Serial.printf("OK TELEM %s\n", up(a).c_str()); }
    }
#if NG_PRESENTER
    else if (is("pointer") && !a) Serial.printf("OK %s\n", Presenter::status().c_str());
    else if (is("pointer") && a && !b && !strcmp(a, "on")) { Presenter::setPointerEnabled(true); Serial.printf("OK %s\n", Presenter::status().c_str()); }
    else if (is("pointer") && a && !b && !strcmp(a, "off")) { Presenter::setPointerEnabled(false); Serial.printf("OK %s\n", Presenter::status().c_str()); }
    else if (is("pointer") && a && !b && !strcmp(a, "toggle")) { Presenter::togglePointer(); Serial.printf("OK %s\n", Presenter::status().c_str()); }
    else if (is("pointer") && a && b && extra && !strcmp(a, "feed")) { Presenter::feed(atof(b), atof(extra), 600); Serial.println("OK POINTER FEED"); }
    else if (is("pointer") && a && b && !extra) { Serial.println(Presenter::set(a, b) ? "OK POINTER SET" : "ERR pointer"); }
#endif
    else if (is("shot") && !a) { ui_flush(); Serial.print("OK "); ui_screenshot(Serial); }   // the page is built first: its log lines must not come between "OK" and the header
    else if (is("layer") && num(a, x) && !b) {
        if (x < 0 || x >= (int)ui_page_count()) Serial.println("ERR range");
        else { ui_set_layer(x); Serial.printf("OK LAYER %d\n", ui_layer()); }
    }
#if NG_SERIAL_INPUT   // tap and key really type on the host: build with -D NG_SERIAL_INPUT=0 to leave them out
    else if (is("tap") && num(a, x) && num(b, y) && !extra) Serial.println(ui_tap(x, y) ? "OK TAP" : "ERR tap");
    else if (is("down") && num(a, x) && num(b, y) && !extra) Serial.println(ui_touch(x, y, true) ? "OK DOWN" : "ERR down");
    else if (is("up") && !a) Serial.println(ui_touch(0, 0, false) ? "OK UP" : "ERR up");
    else if (is("btn") && a && b && !extra && a[1] == 0 && a[0] >= 'a' && a[0] <= 'c' && (!strcmp(b, "down") || !strcmp(b, "up")))
        Serial.println(ui_button(a[0] - 'a', !strcmp(b, "down")) ? (!strcmp(b, "down") ? "OK BTN DOWN" : "OK BTN UP") : "ERR btn");
#if NG_PRESENTER
    else if (is("clicker") && a && !b) {
        static const char* const names[] = {"prev", "next", "start", "blank", "end", "laser"};
        int k = -1; for (int i = 0; i < 6; i++) if (!strcmp(a, names[i])) k = i;
        if (k >= 0) Serial.println(Presenter::send((Presenter::Action)k) ? "OK CLICKER" : "ERR clicker"); else Serial.println("ERR clicker");
    }
    else if (is("pointer") && a && !strcmp(a, "click") && (!b || !strcmp(b, "left") || !strcmp(b, "right") || !strcmp(b, "middle")) && !extra)
        Serial.println(Presenter::click(!b || !strcmp(b, "left") ? 1 : !strcmp(b, "right") ? 2 : 4) ? "OK POINTER CLICK" : "ERR pointer");
    else if (is("pointer") && a && b && !extra && !strcmp(a, "scroll") && num(b, x) && x >= -127 && x <= 127) { Presenter::scroll(x); Serial.println("OK POINTER SCROLL"); }
    else if (is("imu") && a && b && extra) { Presenter::feedRaw(atof(a), atof(b), atof(extra), 600); Serial.println("OK IMU"); }          // pretend the three axes of the gyroscope turn (degrees per second) for 0.6 s
    else if (is("mod") && a && !b) {
        static const char* const n[] = {"ctrl", "shift", "alt", "win", "cs", "as", "meh", "hyper"};
        static const uint8_t m[] = {Presenter::M_CTRL, Presenter::M_SHIFT, Presenter::M_ALT, Presenter::M_WIN, Presenter::M_CS, Presenter::M_AS, Presenter::M_MEH, Presenter::M_HYPER};
        int k = -1; for (int i = 0; i < 8; i++) if (!strcmp(a, n[i])) k = i;
        if (!strcmp(a, "off")) { Presenter::releaseModifiers(); Serial.printf("OK MOD %u\n", (unsigned)Presenter::modifiers()); }
        else if (k >= 0) { Presenter::toggleModifiers(m[k]); Serial.printf("OK MOD %u\n", (unsigned)Presenter::modifiers()); }
        else Serial.println("ERR mod");
    }
#endif
    else if (is("key") && num(a, x) && num(b, y) && !extra) Serial.println(ui_press(x, y, true) ? "OK KEY" : "ERR key");
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
