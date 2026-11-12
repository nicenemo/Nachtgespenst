#include <M5Unified.h>
#include <FastLED.h>
#include <lvgl.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "Theme.h"
#include "Icons.h"
#include "IconCodes.h"
#define LV_SYMBOL_LEFT_ICON IC_ARROW_BACK
#define LV_SYMBOL_RIGHT_ICON IC_ARROW_FORWARD
#include "ui.h"

// ---------- state ----------
namespace {
// Slim title bar on top when the board has hardware/capacitive buttons (Core2: BtnA/B/C below the
// screen); otherwise a taller bottom bar with big prev/next touch buttons (CoreS3).
constexpr int HEADER_H = NG_HAS_BTNS ? 24 : 38;
constexpr int GAP = 3;
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

lv_obj_t* g_header = nullptr;
lv_obj_t* g_title = nullptr;
lv_obj_t* g_pageLbl = nullptr;
lv_obj_t* g_body = nullptr;
lv_obj_t* g_lockTiles[3] = {};
lv_obj_t* g_lockState[3] = {};

// injected touch
int g_injX = 0, g_injY = 0;
uint32_t g_injUntil = 0;

CRGB g_leds[LED_COUNT];
uint8_t g_dispBuf[2][320 * 24 * 2] __attribute__((aligned(4)));

lv_color_t c565(uint16_t c) {
    return lv_color_make(((c >> 11) & 31) * 255 / 31, ((c >> 5) & 63) * 255 / 63, (c & 31) * 255 / 31);
}

// ---------- display / input ----------
void flushCb(lv_display_t* disp, const lv_area_t* a, uint8_t* px) {
    int w = a->x2 - a->x1 + 1, h = a->y2 - a->y1 + 1;
    M5.Display.startWrite();
    M5.Display.pushImage(a->x1, a->y1, w, h, (const lgfx::rgb565_t*)px);
    M5.Display.endWrite();
    lv_display_flush_ready(disp);
}

void readCb(lv_indev_t*, lv_indev_data_t* d) {
    if ((int32_t)(g_injUntil - millis()) > 0) {
        d->state = LV_INDEV_STATE_PRESSED; d->point.x = g_injX; d->point.y = g_injY; return;
    }
    if (M5.Touch.getCount() > 0) {
        auto t = M5.Touch.getDetail(0);
        if (t.isPressed() && t.y < M5.Display.height()) { d->state = LV_INDEV_STATE_PRESSED; d->point.x = t.x; d->point.y = t.y; return; }
    }
    d->state = LV_INDEV_STATE_RELEASED;
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

// ---------- actions ----------
void refreshLocks() {
    for (int i = 0; i < 3; i++) {
        if (!g_lockTiles[i]) continue;
        bool on = g_hostLed & LOCKS[i].ledBit;
        lv_obj_set_style_bg_color(g_lockTiles[i], on ? c565(M5.Display.color565(LOCKS[i].color.r, LOCKS[i].color.g, LOCKS[i].color.b)) : lv_color_hex(0x20242c), 0);
        lv_obj_set_style_border_width(g_lockTiles[i], g_lastLock == i ? 3 : 1, 0);
        lv_obj_set_style_border_color(g_lockTiles[i], g_lastLock == i ? lv_color_white() : lv_color_hex(0x3a4050), 0);
        lv_obj_set_style_text_color(g_lockTiles[i], on ? lv_color_black() : lv_color_hex(0xa0a8b8), 0);
        lv_label_set_text(g_lockState[i], on ? "ON" : "OFF");
    }
}

void buildPage();

void onTile(lv_event_t* e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    ui_press(g_page, (int)idx);
}
#if !NG_HAS_BTNS
void onNav(lv_event_t* e) {
    ui_set_layer(g_page + (int)(intptr_t)lv_event_get_user_data(e));
}
void onLocksToggle(lv_event_t*) { ui_toggle_locks(); }
#endif

lv_obj_t* makeTile(lv_obj_t* parent, int x, int y, int w, int h, lv_color_t bg, lv_color_t fg, intptr_t idx) {
    lv_obj_t* t = lv_button_create(parent);
    lv_obj_set_pos(t, x, y); lv_obj_set_size(t, w, h);
    lv_obj_set_style_bg_color(t, bg, 0);
    lv_obj_set_style_bg_opa(t, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(t, 6, 0);
    lv_obj_set_style_shadow_width(t, 0, 0);
    lv_obj_set_style_border_width(t, 1, 0);
    lv_obj_set_style_border_color(t, lv_color_hex(0x3a4050), 0);
    lv_obj_set_style_text_color(t, fg, 0);
    lv_obj_set_style_pad_all(t, 2, 0);
    lv_obj_set_style_transform_width(t, -2, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(t, -2, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(t, lv_color_lighten(bg, 60), LV_STATE_PRESSED);
    lv_obj_add_event_cb(t, onTile, LV_EVENT_CLICKED, (void*)idx);
    return t;
}

void addTileContent(lv_obj_t* t, const char* layer, const Binding& b, int cw, int ch) {
    TileLook lk = icons_look(layer, b);
    bool capRoom = lk.caption && ch >= 56 && cw >= 50;
    if (lk.icon) {
        int px = ch >= 58 ? 36 : (ch >= 38 ? 28 : 20);
        if (cw < 40 && px > 28) px = 28;
        lv_obj_t* l = lv_label_create(t);
        lv_obj_set_style_text_font(l, icons_font(px), 0);
        lv_label_set_text(l, lk.icon);
        lv_obj_align(l, LV_ALIGN_CENTER, 0, capRoom ? -8 : 0);
        if (capRoom) {
            lv_obj_t* c = lv_label_create(t);
            lv_obj_set_width(c, cw - 6);
            lv_obj_set_style_text_align(c, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_font(c, &lv_font_montserrat_12, 0);
            lv_label_set_long_mode(c, LV_LABEL_LONG_DOT);
            lv_label_set_text(c, lk.caption);
            lv_obj_align(c, LV_ALIGN_BOTTOM_MID, 0, 0);
        }
    } else {
        size_t n = strlen(lk.text);
        const lv_font_t* f;
        if (n <= 1)      f = ch >= 56 ? &lv_font_montserrat_32 : (ch >= 36 ? &lv_font_montserrat_28 : &lv_font_montserrat_20);
        else if (n <= 3) f = ch >= 56 ? &lv_font_montserrat_28 : (ch >= 36 ? &lv_font_montserrat_24 : &lv_font_montserrat_20);
        else if (n <= 6) f = ch >= 36 ? &lv_font_montserrat_20 : &lv_font_montserrat_16;
        else             f = ch >= 36 ? &lv_font_montserrat_16 : &lv_font_montserrat_14;
        lv_obj_t* l = lv_label_create(t);
        lv_obj_set_width(l, cw - 6);
        lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(l, f, 0);
        lv_label_set_long_mode(l, LV_LABEL_LONG_DOT);
        lv_label_set_text(l, lk.text);
        lv_obj_align(l, LV_ALIGN_CENTER, 0, ch >= 36 && lk.badge[0] ? 4 : 0);
        lv_obj_set_height(l, lv_font_get_line_height(f));
    }
    if (lk.badge[0]) {
        lv_obj_t* bl = lv_label_create(t);
        lv_obj_set_style_text_font(bl, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(bl, lv_color_hex(0xFFE08A), 0);
        lv_obj_set_style_bg_color(bl, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(bl, LV_OPA_50, 0);
        lv_obj_set_style_radius(bl, 3, 0);
        lv_obj_set_style_pad_hor(bl, 2, 0);
        lv_label_set_text(bl, lk.badge);
        lv_obj_align(bl, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_add_flag(bl, LV_OBJ_FLAG_IGNORE_LAYOUT);
    }
}

void buildPage() {
    lv_obj_clean(g_body);
    for (int i = 0; i < 3; i++) { g_lockTiles[i] = nullptr; g_lockState[i] = nullptr; }
    int W = M5.Display.width(), H = M5.Display.height() - HEADER_H;
    char buf[48];

    if (g_page == LAYER_COUNT) {
        lv_label_set_text(g_title, "Locks");
        int w = (W - 4 * GAP) / 3, h = H - 2 * GAP;
        static const char* const LOCK_ICONS[3] = {IC_KEYBOARD_CAPSLOCK, IC_DIALPAD, IC_LOCK};
        for (int i = 0; i < 3; i++) {
            lv_obj_t* t = makeTile(g_body, GAP + i * (w + GAP), GAP, w, h, lv_color_hex(0x20242c), lv_color_white(), i);
            lv_obj_t* ic = lv_label_create(t);
            lv_obj_set_style_text_font(ic, icons_font(36), 0);
            lv_label_set_text(ic, LOCK_ICONS[i]);
            lv_obj_align(ic, LV_ALIGN_CENTER, 0, -40);
            lv_obj_t* l = lv_label_create(t);
            lv_obj_set_style_text_font(l, &lv_font_montserrat_16, 0);
            lv_label_set_text(l, LOCKS[i].name);
            lv_obj_align(l, LV_ALIGN_CENTER, 0, -6);
            lv_obj_t* s = lv_label_create(t);
            lv_obj_set_style_text_font(s, &lv_font_montserrat_24, 0);
            lv_obj_align(s, LV_ALIGN_CENTER, 0, 28);
            g_lockTiles[i] = t; g_lockState[i] = s;
        }
        refreshLocks();
    } else {
        const GridLayer& L = LAYERS[g_page];
        lv_label_set_text(g_title, L.title);
        int cw = (W - (L.cols + 1) * GAP) / L.cols;
        int ch = (H - (L.rows + 1) * GAP) / L.rows;
        Theme th = ThemeManager::getTheme();
        for (int i = 0; i < L.rows * L.cols && i < MAX_TILES; i++) {
            const Binding& b = L.tiles[i];
            int x = GAP + (i % L.cols) * (cw + GAP), y = GAP + (i / L.cols) * (ch + GAP);
            bool empty = (b.usage.id == 0);
            lv_color_t bg = c565(empty ? th.defaultKeyBg : ThemeManager::getKeyColor(b, true));
            lv_color_t fg = c565(ThemeManager::getKeyColor(b, false));
            lv_obj_t* t = makeTile(g_body, x, y, cw, ch, bg, fg, i);
            if (empty) { lv_obj_add_state(t, LV_STATE_DISABLED); lv_obj_set_style_bg_opa(t, LV_OPA_40, 0); continue; }
            addTileContent(t, L.title, b, cw, ch);
        }
    }
    snprintf(buf, sizeof(buf), NG_HAS_BTNS ? "%d/%d" : "%d/%d", g_page + 1, (int)LAYER_COUNT + 1);
    lv_label_set_text(g_pageLbl, buf);
#if !NG_HAS_BTNS
    // single readable line in the bottom bar: "Title  n/27"
    lv_label_set_text_fmt(g_title, "%s  %s", lv_label_get_text(g_title), buf);
    lv_obj_add_flag(g_pageLbl, LV_OBJ_FLAG_HIDDEN);
#endif
    g_ledDirty = true;
}

#if !NG_HAS_BTNS
lv_obj_t* navBtn(const char* txt, int dir, lv_align_t al) {
    lv_obj_t* b = lv_button_create(g_header);
    lv_obj_set_size(b, 72, HEADER_H - 4);
    lv_obj_align(b, al, al == LV_ALIGN_LEFT_MID ? 2 : -2, 0);
    lv_obj_set_style_bg_color(b, lv_color_hex(0x2c3340), 0);
    lv_obj_set_style_shadow_width(b, 0, 0);
    lv_obj_set_style_bg_color(b, lv_color_hex(0x4a5570), LV_STATE_PRESSED);
    lv_obj_t* l = lv_label_create(b);
    lv_obj_set_style_text_font(l, icons_font(28), 0);
    lv_label_set_text(l, txt);
    lv_obj_center(l);
    lv_obj_add_event_cb(b, onNav, LV_EVENT_CLICKED, (void*)(intptr_t)dir);
    return b;
}
#endif
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

    lv_init();
    lv_tick_set_cb([]() -> uint32_t { return millis(); });
    int W = M5.Display.width(), H = M5.Display.height();
    lv_display_t* d = lv_display_create(W, H);
    lv_display_set_color_format(d, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(d, g_dispBuf[0], g_dispBuf[1], sizeof(g_dispBuf[0]), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d, flushCb);
    lv_indev_t* in = lv_indev_create();
    lv_indev_set_type(in, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(in, readCb);

    lv_obj_t* scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0b0e14), 0);
    lv_obj_set_style_text_color(scr, lv_color_hex(0xe6e9f0), 0);
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    icons_init();
    g_header = lv_obj_create(scr);
    lv_obj_remove_style_all(g_header);
    lv_obj_set_size(g_header, W, HEADER_H);
    lv_obj_set_pos(g_header, 0, NG_HAS_BTNS ? 0 : H - HEADER_H);
    lv_obj_set_style_bg_color(g_header, lv_color_hex(0x161b26), 0);
    lv_obj_set_style_bg_opa(g_header, LV_OPA_COVER, 0);
    lv_obj_remove_flag(g_header, LV_OBJ_FLAG_SCROLLABLE);
    g_title = lv_label_create(g_header);
    lv_obj_set_style_text_font(g_title, &lv_font_montserrat_16, 0);
    g_pageLbl = lv_label_create(g_header);
    lv_obj_set_style_text_font(g_pageLbl, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(g_pageLbl, lv_color_hex(0x9aa4ba), 0);
#if NG_HAS_BTNS
    lv_obj_align(g_title, LV_ALIGN_LEFT_MID, 8, 0);
    lv_obj_align(g_pageLbl, LV_ALIGN_RIGHT_MID, -8, 0);
#else
    navBtn(LV_SYMBOL_LEFT_ICON, -1, LV_ALIGN_LEFT_MID);
    navBtn(LV_SYMBOL_RIGHT_ICON, +1, LV_ALIGN_RIGHT_MID);
    // centre: title above/next to the page number; tap = jump to / back from Locks
    lv_obj_t* mid = lv_button_create(g_header);
    lv_obj_set_size(mid, W - 2 * 76, HEADER_H - 4);
    lv_obj_align(mid, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(mid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(mid, 0, 0);
    lv_obj_add_event_cb(mid, onLocksToggle, LV_EVENT_CLICKED, nullptr);
    lv_obj_set_parent(g_title, mid);
    lv_obj_set_parent(g_pageLbl, mid);
    lv_obj_set_style_text_font(g_title, &lv_font_montserrat_16, 0);
    lv_obj_set_width(g_title, W - 2 * 80);
    lv_obj_set_style_text_align(g_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(g_title, LV_LABEL_LONG_DOT);
    lv_obj_align(g_title, LV_ALIGN_CENTER, 0, 0);
#endif

    g_body = lv_obj_create(scr);
    lv_obj_remove_style_all(g_body);
    lv_obj_set_pos(g_body, 0, NG_HAS_BTNS ? HEADER_H : 0);
    lv_obj_set_size(g_body, W, H - HEADER_H);
    lv_obj_remove_flag(g_body, LV_OBJ_FLAG_SCROLLABLE);

    buildPage();
}

void ui_set_layer(int n) {
    int cnt = ui_page_count();
    int np = ((n % cnt) + cnt) % cnt;
    if (np == LAYER_COUNT && g_page != LAYER_COUNT) g_prevPage = g_page;
    g_page = np;
    // Deferred rebuild is not needed: callers run outside LVGL's event dispatch
    // for serial use; from event callbacks lv_obj_clean on the pressed parent is
    // safe because the button lives in the header, not the body.
    buildPage();
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

void ui_tap(int x, int y) { g_injX = x; g_injY = y; g_injUntil = millis() + 120; }

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
    lv_refr_now(nullptr);
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
    if (M5.Touch.getCount() > 0 || M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed())
        lastActive = millis();
    uint32_t idle = millis() - lastActive;
    uint8_t want = idle < 60000 ? 128 : (idle < 300000 ? 16 : 0);
    if (want != level) { level = want; M5.Display.setBrightness(want); }
}

void ui_loop() {
    static uint32_t lastLedPoll = 0;
    idleBacklight();
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
    lv_timer_handler();
}
