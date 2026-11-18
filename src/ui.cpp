// Classic UI: the keymap as a plain tile grid drawn straight onto the display (no LVGL, no icon font).
// Same public API as the LVGL UI (ui.h), so main.cpp, SerialCmd.cpp and SerialTest work unchanged.
#include <M5Unified.h>
#include <FastLED.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "Theme.h"
#include "Icons.h"
#include "IconCodes.h"
#include "ui.h"

namespace {
// Slim title bar on top when the board has hardware/capacitive buttons (Core2: BtnA/B/C below the
// screen); otherwise a taller bottom bar with big prev/next touch buttons (CoreS3).
constexpr int HEADER_H = NG_HAS_BTNS ? 24 : 38;
constexpr int GAP = 3;
constexpr int NAV_W = 72;
constexpr int MAX_TILES = 32;

struct LockDef { const char* name; uint8_t ledBit; CRGB color; };
const LockDef LOCKS[3] = {
    {"CAPS",   0x02, LOCK_COLORS[0]},
    {"NUM",    0x01, LOCK_COLORS[1]},
    {"SCROLL", 0x04, LOCK_COLORS[2]},
};

int g_page = 0;
int g_prevPage = 0; // page to return to when leaving Locks via toggle
uint8_t g_lastLock = LK_NONE;
volatile uint8_t g_hostLed = 0;
volatile bool g_ledDirty = true;
uint32_t g_pressCount = 0;
const char* g_lastKey = "-";

bool g_swallowTouch = false;  // touch that woke a dimmed screen must not press a tile
int g_hlIdx = -1;             // tile that is highlighted after a press
uint32_t g_hlUntil = 0;

CRGB g_leds[LED_COUNT];

// ---------- colours ----------
uint16_t rgb(uint8_t r, uint8_t g, uint8_t b) { return M5.Display.color565(r, g, b); }
uint16_t lighten(uint16_t c) {
    int r = ((c >> 11) & 31) * 255 / 31, g = ((c >> 5) & 63) * 255 / 63, b = (c & 31) * 255 / 31;
    return rgb(min(255, r + 70), min(255, g + 70), min(255, b + 70));
}

// ---------- LED bar ----------
void updateLeds() {
#if NG_HAS_LEDBAR
    if (g_page == LAYER_COUNT) {
        uint8_t mask = 0; // host LED bits: num=1 caps=2 scroll=4 -> bar mask caps=1 num=2 scroll=4
        for (int i = 0; i < 3; i++) if (g_hostLed & LOCKS[i].ledBit) mask |= 1u << i;
        ledBarLocks(g_leds, LED_COUNT, mask);
    } else {
        fill_solid(g_leds, LED_COUNT, LAYERS[g_page].color);
    }
    FastLED.show();
#endif
}

void onHid(HIDManager::HIDEvent e, uint8_t led) {
    if (e == HIDManager::HIDEvent::LED_UPDATED) { g_hostLed = led; g_ledDirty = true; }
}

// ---------- text helpers ----------
// Largest font whose text fits in maxW x maxH; as a last resort the text is shortened with "..".
void drawFit(const char* s, int cx, int cy, int maxW, int maxH, uint16_t fg) {
    static const lgfx::IFont* const FONTS[] = {&fonts::FreeSansBold18pt7b, &fonts::FreeSansBold12pt7b,
                                               &fonts::FreeSansBold9pt7b, &fonts::Font2};
    char buf[24];
    snprintf(buf, sizeof buf, "%s", s);
    const lgfx::IFont* f = FONTS[3];
    for (const lgfx::IFont* cand : FONTS) {
        M5.Display.setFont(cand);
        if (M5.Display.textWidth(buf) <= maxW && M5.Display.fontHeight() <= maxH) { f = cand; break; }
    }
    M5.Display.setFont(f);
    size_t n = strlen(buf);
    while (n > 2 && M5.Display.textWidth(buf) > maxW) { buf[--n] = 0; if (n > 2) { buf[n - 1] = '.'; buf[n - 2] = '.'; } }
    M5.Display.setTextColor(fg);
    M5.Display.setTextDatum(middle_center);
    M5.Display.drawString(buf, cx, cy);
}

// Arrow tiles are drawn as shapes (the classic UI has no icon font). dir: 'U','D','L','R'.
void drawArrow(char dir, int cx, int cy, int s, uint16_t c) {
    switch (dir) {
        case 'U': M5.Display.fillTriangle(cx, cy - s, cx - s, cy + s / 2, cx + s, cy + s / 2, c); break;
        case 'D': M5.Display.fillTriangle(cx, cy + s, cx - s, cy - s / 2, cx + s, cy - s / 2, c); break;
        case 'L': M5.Display.fillTriangle(cx - s, cy, cx + s / 2, cy - s, cx + s / 2, cy + s, c); break;
        default:  M5.Display.fillTriangle(cx + s, cy, cx - s / 2, cy - s, cx - s / 2, cy + s, c); break;
    }
}

char arrowOf(const TileLook& lk) {
    if (!lk.icon) return 0;
    if (!strcmp(lk.icon, IC_ARROW_UPWARD)) return 'U';
    if (!strcmp(lk.icon, IC_ARROW_DOWNWARD)) return 'D';
    if (!strcmp(lk.icon, IC_ARROW_BACK)) return 'L';
    if (!strcmp(lk.icon, IC_ARROW_FORWARD)) return 'R';
    return 0;
}

// ---------- geometry ----------
int bodyTop() { return NG_HAS_BTNS ? HEADER_H : 0; }
int bodyH() { return M5.Display.height() - HEADER_H; }

void tileRect(const GridLayer& L, int i, int& x, int& y, int& w, int& h) {
    int W = M5.Display.width(), H = bodyH();
    w = (W - (L.cols + 1) * GAP) / L.cols;
    h = (H - (L.rows + 1) * GAP) / L.rows;
    x = GAP + (i % L.cols) * (w + GAP);
    y = bodyTop() + GAP + (i / L.cols) * (h + GAP);
}

void lockRect(int i, int& x, int& y, int& w, int& h) {
    int W = M5.Display.width();
    w = (W - 4 * GAP) / 3;
    h = bodyH() - 2 * GAP;
    x = GAP + i * (w + GAP);
    y = bodyTop() + GAP;
}

// ---------- drawing ----------
void drawTile(const GridLayer& L, int i, bool highlight) {
    const Binding& b = L.tiles[i];
    int x, y, w, h;
    tileRect(L, i, x, y, w, h);
    Theme th = ThemeManager::getTheme();
    bool empty = (b.usage.id == 0);
    uint16_t bg = empty ? th.defaultKeyBg : ThemeManager::getKeyColor(b, true);
    uint16_t fg = ThemeManager::getKeyColor(b, false);
    if (highlight) bg = lighten(bg);
    M5.Display.fillRoundRect(x, y, w, h, 4, bg);
    M5.Display.drawRoundRect(x, y, w, h, 4, th.gridLines);
    if (empty) return;

    TileLook lk = icons_look(L.title, b);
    char arrow = arrowOf(lk);
    const char* text = lk.text[0] ? lk.text : (lk.caption ? lk.caption : "?");
    int cy = y + h / 2 + (lk.badge[0] && h >= 36 ? 4 : 0);
    if (arrow) drawArrow(arrow, x + w / 2, y + h / 2, min(min(w, h) / 3, 14), fg);
    else drawFit(text, x + w / 2, cy, w - 6, h - (lk.badge[0] ? 16 : 6), fg);
    if (lk.badge[0]) {
        M5.Display.setFont(&fonts::Font0);
        M5.Display.setTextColor(rgb(255, 224, 138));
        M5.Display.setTextDatum(top_left);
        M5.Display.drawString(lk.badge, x + 3, y + 3);
    }
}

void drawLockTile(int i) {
    int x, y, w, h;
    lockRect(i, x, y, w, h);
    bool on = g_hostLed & LOCKS[i].ledBit;
    uint16_t bg = on ? rgb(LOCKS[i].color.r, LOCKS[i].color.g, LOCKS[i].color.b) : rgb(32, 36, 44);
    uint16_t fg = on ? BLACK : rgb(160, 168, 184);
    bool last = g_lastLock == i;
    M5.Display.fillRoundRect(x, y, w, h, 6, bg);
    M5.Display.drawRoundRect(x, y, w, h, 6, last ? WHITE : rgb(58, 64, 80));
    if (last) M5.Display.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 5, WHITE);
    drawFit(LOCKS[i].name, x + w / 2, y + h / 2 - 22, w - 8, 24, fg);
    drawFit(on ? "ON" : "OFF", x + w / 2, y + h / 2 + 18, w - 8, 36, fg);
}

void drawHeader() {
    int W = M5.Display.width(), H = M5.Display.height();
    int y0 = NG_HAS_BTNS ? 0 : H - HEADER_H;
    uint16_t bar = rgb(22, 27, 38);
    char page[12], title[40];
    snprintf(page, sizeof page, "%d/%d", g_page + 1, (int)LAYER_COUNT + 1);
    snprintf(title, sizeof title, "%s", g_page == LAYER_COUNT ? "Locks" : LAYERS[g_page].title);
    M5.Display.fillRect(0, y0, W, HEADER_H, bar);
#if NG_HAS_BTNS
    M5.Display.setFont(&fonts::FreeSansBold9pt7b);
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(middle_left);
    M5.Display.drawString(title, 8, y0 + HEADER_H / 2);
    M5.Display.setTextColor(rgb(154, 164, 186));
    M5.Display.setTextDatum(middle_right);
    M5.Display.drawString(page, W - 8, y0 + HEADER_H / 2);
#else
    uint16_t btn = rgb(44, 51, 64);
    M5.Display.fillRoundRect(2, y0 + 2, NAV_W, HEADER_H - 4, 4, btn);
    M5.Display.fillRoundRect(W - NAV_W - 2, y0 + 2, NAV_W, HEADER_H - 4, 4, btn);
    drawArrow('L', 2 + NAV_W / 2, y0 + HEADER_H / 2, 9, WHITE);
    drawArrow('R', W - NAV_W / 2 - 2, y0 + HEADER_H / 2, 9, WHITE);
    char line[56];
    snprintf(line, sizeof line, "%s  %s", title, page);
    drawFit(line, W / 2, y0 + HEADER_H / 2, W - 2 * (NAV_W + 8), HEADER_H - 6, WHITE);
#endif
}

void drawPage() {
    int W = M5.Display.width(), H = M5.Display.height();
    M5.Display.startWrite();
    M5.Display.fillRect(0, 0, W, H, rgb(11, 14, 20));
    if (g_page == LAYER_COUNT) {
        for (int i = 0; i < 3; i++) drawLockTile(i);
    } else {
        const GridLayer& L = LAYERS[g_page];
        for (int i = 0; i < L.rows * L.cols && i < MAX_TILES; i++) drawTile(L, i, false);
    }
    drawHeader();
    M5.Display.endWrite();
    g_hlIdx = -1;
    g_ledDirty = true;
}

void refreshLocks() {
    M5.Display.startWrite();
    for (int i = 0; i < 3; i++) drawLockTile(i);
    M5.Display.endWrite();
}

// ---------- touch ----------
// Returns the tile index at (x, y) for the current page, or -1.
int hitTile(int x, int y) {
    if (g_page == LAYER_COUNT) {
        for (int i = 0; i < 3; i++) {
            int tx, ty, tw, th; lockRect(i, tx, ty, tw, th);
            if (x >= tx && x < tx + tw && y >= ty && y < ty + th) return i;
        }
        return -1;
    }
    const GridLayer& L = LAYERS[g_page];
    for (int i = 0; i < L.rows * L.cols && i < MAX_TILES; i++) {
        int tx, ty, tw, th; tileRect(L, i, tx, ty, tw, th);
        if (x >= tx && x < tx + tw && y >= ty && y < ty + th) return i;
    }
    return -1;
}

void handleTouch(int x, int y) {
    int H = M5.Display.height(), W = M5.Display.width();
#if NG_HAS_BTNS
    if (y < HEADER_H) return;                                  // title bar: nothing to press
#else
    if (y >= H - HEADER_H) {                                   // bottom bar: prev / Locks / next
        if (x < NAV_W) ui_set_layer(g_page - 1);
        else if (x >= W - NAV_W) ui_set_layer(g_page + 1);
        else ui_toggle_locks();
        return;
    }
#endif
    int idx = hitTile(x, y);
    if (idx < 0) return;
    if (ui_press(g_page, idx) && g_page != LAYER_COUNT) {
        M5.Display.startWrite();
        drawTile(LAYERS[g_page], idx, true);
        M5.Display.endWrite();
        g_hlIdx = idx; g_hlUntil = millis() + 120;
    }
}
} // namespace

// ---------- public API ----------
uint8_t ui_page_count() { return LAYER_COUNT + 1; }
int ui_layer() { return g_page; }
uint8_t ui_locks_page() { return LAYER_COUNT; }
void ui_toggle_locks() { ui_set_layer(g_page == LAYER_COUNT ? g_prevPage : LAYER_COUNT); }

void ui_begin() {
#if NG_HAS_LEDBAR
    M5.Power.setExtOutput(true); // 5V on the M-Bus for the LED base
    FastLED.addLeds<WS2812, LED_PIN, GRB>(g_leds, LED_COUNT);
    FastLED.setBrightness(40);
#endif
    HIDManager::setEventListener(onHid);
    g_hostLed = HIDManager::getLedStatus();
    drawPage();
}

void ui_set_layer(int n) {
    int cnt = ui_page_count();
    int np = ((n % cnt) + cnt) % cnt;
    if (np == LAYER_COUNT && g_page != LAYER_COUNT) g_prevPage = g_page;
    g_page = np;
    drawPage();
}

bool ui_press(int layer, int idx) {
    if (layer < 0 || layer >= (int)ui_page_count()) return false;
    if (layer == LAYER_COUNT) {
        if (idx < 0 || idx > 2) return false;
        g_lastLock = idx;
        g_lastKey = LOCKS[idx].name;
        g_pressCount++;
        switch (idx) {
            case LK_CAPS:   HIDManager::send(kp_CAPS_LOCK); break;
            case LK_NUM:    HIDManager::send(kp_KP_NUM_LOCK_AND_CLEAR); break;
            default:        HIDManager::send(kp_SCROLL_LOCK); break;
        }
        g_hostLed = HIDManager::getLedStatus();
        g_ledDirty = true;
        return true;
    }
    const GridLayer& L = LAYERS[layer];
    if (idx < 0 || idx >= L.rows * L.cols) return false;
    const Binding& b = L.tiles[idx];
    if (b.usage.id == 0) return false;
    g_lastKey = b.label ? b.label : b.usage.identifier;
    g_pressCount++;
    HIDManager::send(b);
    return true;
}

void ui_tap(int x, int y) { handleTouch(x, y); }

String ui_state() {
    uint8_t led = HIDManager::getLedStatus();
    char b[200];
    snprintf(b, sizeof(b), "STATE page=%d/%d title=%s caps=%d num=%d scroll=%d last=%s presses=%lu conn=%d",
             g_page, (int)ui_page_count(), g_page == LAYER_COUNT ? "Locks" : LAYERS[g_page].title,
             !!(led & 0x02), !!(led & 0x01), !!(led & 0x04),
             g_lastKey, (unsigned long)g_pressCount, HIDManager::isConnected());
    return String(b);
}

void ui_screenshot(Stream& out) {
    int W = M5.Display.width(), H = M5.Display.height();
    out.printf("SHOT %d %d\n", W, H);
    static uint8_t row[320 * 3];
    for (int y = 0; y < H; y++) {
        M5.Display.readRectRGB(0, y, W, 1, row);
        out.write(row, W * 3);
        if ((y & 7) == 7) { out.flush(); delay(1); } // let a UART bridge drain; avoids lost tail
    }
    out.print("\nEND\n");
}

// Backlight: dim after 60 s without touch/button, off after 5 min; any touch wakes
// it (no re-init needed, unlike deep sleep).
static void idleBacklight() {
    static uint32_t lastActive = 0;
    static uint8_t level = 255;
    bool touched = M5.Touch.getCount() > 0;
    if (touched && level < 128) g_swallowTouch = true;
    if (touched || M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed())
        lastActive = millis();
    uint32_t idle = millis() - lastActive;
    uint8_t want = idle < 60000 ? 128 : (idle < 300000 ? 16 : 0);
    if (want != level) { level = want; M5.Display.setBrightness(want); }
}

uint32_t ui_loop() {
    static uint32_t lastLedPoll = 0;
    idleBacklight();
    // Touch: act on the press edge; a touch that woke the screen is swallowed until the finger lifts.
    if (g_swallowTouch) {
        if (M5.Touch.getCount() == 0) g_swallowTouch = false;
    } else if (M5.Touch.getCount() > 0) {
        auto t = M5.Touch.getDetail(0);
        if (t.wasPressed() && t.y < M5.Display.height()) handleTouch(t.x, t.y);
    }
    if (g_hlIdx >= 0 && (int32_t)(millis() - g_hlUntil) >= 0) {
        if (g_page != LAYER_COUNT) {
            M5.Display.startWrite();
            drawTile(LAYERS[g_page], g_hlIdx, false);
            M5.Display.endWrite();
        }
        g_hlIdx = -1;
    }
    // Keep in sync with the HID layer (local toggles do not always fire the listener).
    if (millis() - lastLedPoll > 100) {
        lastLedPoll = millis();
        uint8_t l = HIDManager::getLedStatus();
        if (l != g_hostLed) { g_hostLed = l; g_ledDirty = true; }
    }
    if (g_ledDirty) {
        g_ledDirty = false;
        if (g_page == LAYER_COUNT) refreshLocks();
        updateLeds();
    }
    return 5;
}
