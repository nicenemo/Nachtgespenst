#include <M5Unified.h>
#include <FastLED.h>
#include <math.h>
#include <esp_heap_caps.h>
#include <lvgl.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "Lamps.h"
#include "Disk.h"
#include "BarFx.h"
#include "Telem.h"
#include "Settings.h"
#include "Theme.h"
#include "Icons.h"
#include "ConsoleStyle.h"
#include "Presenter.h"
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
const LockDef LOCKS[LOCK_COUNT] = {
    {"Caps",    HIDManager::LED_CAPS,    LOCK_COLORS[0]},
    {"Num",     HIDManager::LED_NUM,     LOCK_COLORS[1]},
    {"Scroll",  HIDManager::LED_SCROLL,  LOCK_COLORS[2]},
    {"Compose", HIDManager::LED_COMPOSE, LOCK_COLORS[3]},      // on Linux the desktop sets these two LEDs (xkb indicators)
    {"Kana",    HIDManager::LED_KANA,    LOCK_COLORS[4]},
};

// After the Locks page: the Disk demo (file transfer over the LampArray, see Disk.h) and the two LampArray test pages (see Lamps.h):
// the screen lamps and the LED bar lamps. The lamp pages stay last (the test tools count from the end).
constexpr int PAGE_DISK = LAYER_COUNT + 1;
constexpr int PAGE_LAMP_SCREEN = LAYER_COUNT + 2;
constexpr int PAGE_LAMP_BAR = LAYER_COUNT + 3;
lv_obj_t* g_diskBar = nullptr; lv_obj_t* g_diskState = nullptr; lv_obj_t* g_diskStats = nullptr; lv_obj_t* g_diskText = nullptr;
int g_diskLit = -1;
bool g_lampEnter = false; // a lamp page was just built: draw it once LVGL has painted the empty body

int g_page = 0;
int g_prevPage = 0; // page to return to when leaving Locks via toggle
// Page changes are coalesced: ui_set_layer() only records the target page and the new header; the body is built once
// in ui_loop() (or when something needs the screen: SHOT, an injected tap), so a burst of presses builds one page.
bool g_pageDirty = false;
uint32_t g_pageStamp = 0;
constexpr uint32_t PAGE_COALESCE_MS = 25;
// Idle handling (backlight and LED bar) and the manual overrides of the serial commands WAKE and BL.
uint32_t g_lastActive = 0;
int g_blOverride = -1;          // -1 = automatic
bool g_ledIdleOff = false;      // the bar is dark because of "ledidle"
bool g_barPowered = false;      // 5 V rail of the M-Bus (LED base) is on
uint8_t g_lastLock = LK_NONE;
volatile uint8_t g_hostLed = 0;
volatile bool g_ledDirty = true;
uint32_t g_pressCount = 0;
const char* g_lastKey = "-";

lv_obj_t* g_header = nullptr;
lv_obj_t* g_title = nullptr;
lv_obj_t* g_pageLbl = nullptr;
lv_obj_t* g_body = nullptr;
#if !NG_PRESENTER      // the Locks page (and its LEDs) exist on the CoreS3 only; the Core2 is a clicker
lv_obj_t* g_lockTiles[LOCK_COUNT] = {};
lv_obj_t* g_lockState[LOCK_COUNT] = {};
// A small round LED in every lock tile, behind the same lattice as the lamps of the LED bar in the demo window. It shows the colour of that lock on the LED bar (LOCK_COLORS).
constexpr int LED_PX = 24;
lv_obj_t* g_lockLed[LOCK_COUNT] = {};
uint32_t* g_ledBuf[LOCK_COUNT] = {};                                  // allocated when the Locks page is first built (the Core2 never builds it: no static RAM for it)
void drawLed(int i, bool on) {
    const CRGB c = LOCK_COLORS[i];
    if (!g_ledBuf[i]) g_ledBuf[i] = (uint32_t*)heap_caps_malloc(LED_PX * LED_PX * 4, MALLOC_CAP_8BIT);
    uint32_t* b = g_ledBuf[i]; if (!b) return;
    const float r0 = LED_PX / 2.0f - 0.5f;
    for (int y = 0; y < LED_PX; y++) for (int x = 0; x < LED_PX; x++) {
        float dx = x - r0, dy = y - r0, d = sqrtf(dx * dx + dy * dy);
        if (d > r0 + 0.5f) { b[y * LED_PX + x] = 0; continue; }                          // outside the circle: transparent
        bool lattice = (x % 4 == 0) || (y % 4 == 0);
        float k = on ? (lattice ? 0.16f : 1.0f) : (lattice ? 0.05f : 0.20f);             // lit: the colour behind dark lines; off: the bare lattice
        uint8_t a = d > r0 ? 128 : 255;                                                  // a soft edge
        b[y * LED_PX + x] = ((uint32_t)a << 24) | ((uint32_t)(c.r * k) << 16) | ((uint32_t)(c.g * k) << 8) | (uint32_t)(c.b * k);
    }
}
#endif

// injected touch
bool g_swallowTouch = false; // touch that woke a dimmed screen must not press a tile
int g_injX = 0, g_injY = 0;
uint32_t g_injUntil = 0;
// A touch that is held until it is released (shell: touch down / touch up). Released by first moving off every control, so no click fires.
bool g_holdOn = false; int g_holdX = 0, g_holdY = 0; uint32_t g_holdEnd = 0;
uint8_t g_btnDown = 0;                  // bit 0..2: Core2 button A, B, C held (really or by the shell)
#if NG_HAS_BTNS
lv_obj_t* g_btnPill[3] = {};            // the A, B, C indicators in the header of the Core2
#endif

CRGB g_leds[LED_COUNT];
volatile uint32_t g_barSnap[10];   // what the bar shows, for BAR? (the demo window mirrors it)

// Timing for PERF? (microseconds). Build = last buildPage(); render and flush are accumulated since that build
// (render = time inside lv_timer_handler/lv_refr_now that actually drew strips, flush = the part spent in pushImage).
uint32_t g_perfBuildUs = 0, g_perfRenderUs = 0, g_perfFlushUs = 0, g_perfStrips = 0;
// Two partial render buffers of DISP_ROWS rows: fewer, taller strips mean fewer walks over the widget tree (ADR 0007).
#ifndef NG_DISP_ROWS
#define NG_DISP_ROWS 48
#endif
uint8_t g_dispBuf[2][320 * NG_DISP_ROWS * 2] __attribute__((aligned(4)));

lv_color_t c565(uint16_t c) {
    return lv_color_make(((c >> 11) & 31) * 255 / 31, ((c >> 5) & 63) * 255 / 63, (c & 31) * 255 / 31);
}

// ---------- display / input ----------
void flushCb(lv_display_t* disp, const lv_area_t* a, uint8_t* px) {
    uint32_t t0 = micros();
    int w = a->x2 - a->x1 + 1, h = a->y2 - a->y1 + 1;
    M5.Display.startWrite();
    M5.Display.pushImage(a->x1, a->y1, w, h, (const lgfx::rgb565_t*)px);
    M5.Display.endWrite();
    g_perfFlushUs += micros() - t0;
    g_perfStrips++;
    lv_display_flush_ready(disp);
}

void readCb(lv_indev_t*, lv_indev_data_t* d) {
    if (g_holdOn) {
        if (g_holdEnd && (int32_t)(millis() - g_holdEnd) >= 0) g_holdOn = false;
        else { d->state = LV_INDEV_STATE_PRESSED; d->point.x = g_holdX; d->point.y = g_holdY; return; }
    }
    if ((int32_t)(g_injUntil - millis()) > 0) {
        d->state = LV_INDEV_STATE_PRESSED; d->point.x = g_injX; d->point.y = g_injY; return;
    }
    if (g_swallowTouch) {
        if (M5.Touch.getCount() > 0) { d->state = LV_INDEV_STATE_RELEASED; return; }
        g_swallowTouch = false; // finger lifted
    }
    if (M5.Touch.getCount() > 0) {
        auto t = M5.Touch.getDetail(0);
        if (t.isPressed() && t.y < M5.Display.height()) { d->state = LV_INDEV_STATE_PRESSED; d->point.x = t.x; d->point.y = t.y; return; }
    }
    d->state = LV_INDEV_STATE_RELEASED;
}

// ---------- LED bar ----------
// The bar follows the setting "led": off (dark, 5 V rail off), locks (only the host lock state) or layers (layer colours).
SemaphoreHandle_t g_ledMutex = nullptr;   // FastLED is used by the main loop and by the lamp worker task on the other core
void updateLedsLocked();
void updateLeds() {
    if (g_ledMutex) xSemaphoreTake(g_ledMutex, portMAX_DELAY);
    updateLedsLocked();
    if (g_ledMutex) xSemaphoreGive(g_ledMutex);
}
void updateLedsLocked() {
#if NG_HAS_LEDBAR
    using LM = Settings::LedMode;
    LM mode = Settings::ledMode();
    if (mode == LM::Off || g_ledIdleOff) {
        fill_solid(g_leds, LED_COUNT, CRGB::Black);
        for (int i = 0; i < 10; i++) g_barSnap[i] = 0;
        FastLED.show();
        if (mode == LM::Off && g_barPowered) { M5.Power.setExtOutput(false); g_barPowered = false; }
        return;
    }
    if (!g_barPowered) { M5.Power.setExtOutput(true); g_barPowered = true; delay(40); } // the LEDs need a moment after the rail returns
    FastLED.setBrightness(Lamps::barBrightness());
#if NG_PRESENTER
    {   // the clicker: the bar shows the pointer: dark while it is off, dim lavender while it sleeps, amber while it moves
        Presenter::Ptr p = Presenter::pointer();
        CRGB c = p == Presenter::Ptr::Active ? CRGB(255, 153, 0) : p == Presenter::Ptr::Asleep ? CRGB(24, 24, 64) : CRGB::Black;
        fill_solid(g_leds, LED_COUNT, c);
        for (int i = 0; i < 10 && i < LED_COUNT; i++) g_barSnap[i] = ((uint32_t)g_leds[i].r << 16) | ((uint32_t)g_leds[i].g << 8) | g_leds[i].b;
        FastLED.show();
        return;
    }
#endif
    uint8_t mask = 0; // host LED bits: num=1 caps=2 scroll=4 -> bar mask caps=1 num=2 scroll=4
    for (int i = 0; i < LOCK_COUNT; i++) if (g_hostLed & LOCKS[i].ledBit) mask |= 1u << i;
    if (g_page >= PAGE_LAMP_SCREEN) {
        for (int i = 0; i < LED_COUNT; i++) g_leds[i] = Lamps::barColor(i); // lamps 0..9 of the LampArray
    } else if (g_page == PAGE_DISK) {
        Disk::View v = Disk::view();                                          // progress of the transfer along the bar
        bool sendingBack = v.tx == Disk::Tx::Header || v.tx == Disk::Tx::Data;
        int lit = sendingBack ? (v.txTotal ? (int)((uint64_t)v.txBytes * LED_COUNT / v.txTotal) : 0) : (v.total ? (int)((uint64_t)v.bytes * LED_COUNT / v.total) : 0);
        CRGB c = v.state == Disk::State::DoneOk ? CRGB(0, 255, 40) : v.state == Disk::State::DoneBad ? CRGB(255, 0, 0) : CRGB(0, 90, 255);
        for (int i = 0; i < LED_COUNT; i++) g_leds[i] = (v.state == Disk::State::DoneOk || v.state == Disk::State::DoneBad || i < lit) ? c : CRGB(4, 4, 4);
    } else if (g_page == LAYER_COUNT) {
        ledBarLocks(g_leds, LED_COUNT, mask);
    } else if (mode == LM::Locks) {
        ledBarLocks(g_leds, LED_COUNT, mask, false);
    } else {
        Settings::Effect fx = Settings::effect();                           // an autonomous effect in the colour of the layer, or the plain colour
        if (fx != Settings::Effect::None) BarFx::render(fx, g_leds, millis(), LAYERS[g_page].color);
        else fill_solid(g_leds, LED_COUNT, LAYERS[g_page].color);
    }
    for (int i = 0; i < 10 && i < LED_COUNT; i++) g_barSnap[i] = ((uint32_t)g_leds[i].r << 16) | ((uint32_t)g_leds[i].g << 8) | g_leds[i].b;
    FastLED.show();
#endif
}

// Runs on the other core: draws the autonomous bar effects about 60 times a second and feeds the microphone effect and MIC?.
void effectTask(void*) {
    for (;;) {
        Settings::Effect fx = Settings::effect();
        bool layerPage = g_page < LAYER_COUNT && Settings::ledMode() == Settings::LedMode::Layers && !g_ledIdleOff;
        BarFx::micWanted((fx == Settings::Effect::Vu && layerPage) || BarFx::micCommandOn() || Telem::wantsMic());
        BarFx::micLoop();
        Telem::loop();
        if (fx != Settings::Effect::None && layerPage) updateLeds();
        vTaskDelay(pdMS_TO_TICKS(fx != Settings::Effect::None ? 16 : 20));
    }
}

void onHid(HIDManager::HIDEvent e, uint8_t led) {
    if (e == HIDManager::HIDEvent::LED_UPDATED) { g_hostLed = led; g_ledDirty = true; }
}

// ---------- actions ----------
#if !NG_PRESENTER
void refreshLocks() {
    for (int i = 0; i < LOCK_COUNT; i++) {
        if (!g_lockTiles[i]) continue;
        bool on = g_hostLed & LOCKS[i].ledBit;
        lv_obj_set_style_bg_color(g_lockTiles[i], on ? lv_color_hex(Console::AMBER) : lv_color_hex(Console::DIM), 0);   // the small LED in the tile has the colour of the lock
        lv_obj_set_style_border_width(g_lockTiles[i], g_lastLock == i ? 3 : 0, 0);
        lv_obj_set_style_border_color(g_lockTiles[i], lv_color_hex(Console::PEACH), 0);
        lv_obj_set_style_text_color(g_lockTiles[i], on ? lv_color_black() : lv_color_hex(Console::PEACH), 0);
        lv_label_set_text(g_lockState[i], on ? "On" : "Off");
        if (g_lockLed[i]) { drawLed(i, on); lv_obj_invalidate(g_lockLed[i]); }
    }
}
#endif

void buildPage();

const char* pageTitle(int page) {
    return page == PAGE_DISK ? "Disk" : page == PAGE_LAMP_SCREEN ? "Lamps: screen" : page == PAGE_LAMP_BAR ? "Lamps: bar" : page == LAYER_COUNT ? "Locks" : LAYERS[page].title;
}

// Title and page counter of the header; cheap, so it follows a page change at once while the body waits for the coalescing.
void setHeader(int page) {
#if NG_PRESENTER
    (void)page;
    lv_label_set_text(g_title, "Presenter");
    lv_label_set_text(g_pageLbl, HIDManager::isBleConnected() ? "BLE" : "no BLE");
    return;
#endif
    char buf[48];
    snprintf(buf, sizeof(buf), "%d/%d", page + 1, (int)ui_page_count());
#if NG_HAS_BTNS
    lv_label_set_text(g_title, pageTitle(page));
    lv_label_set_text(g_pageLbl, buf);
#else
    // single readable line in the bottom bar: "Title  n/27"
    lv_label_set_text_fmt(g_title, "%s  %s", pageTitle(page), buf);
    lv_label_set_text(g_pageLbl, buf);
    lv_obj_add_flag(g_pageLbl, LV_OBJ_FLAG_HIDDEN);
#endif
}

void flushPage() {
    if (!g_pageDirty) return;
    g_pageDirty = false;
    buildPage();
}

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
    Console::block(t, 0);
    lv_obj_remove_flag(t, LV_OBJ_FLAG_PRESS_LOCK);               // sliding off the key cancels the press (no click), also for the shell command up
    lv_obj_set_style_bg_color(t, bg, 0);                         // the colour of a key says what it does (Theme); the chrome is the console palette
    lv_obj_set_style_text_color(t, fg, 0);
    lv_obj_set_style_pad_all(t, 2, 0);
    lv_obj_set_style_transform_width(t, -2, LV_STATE_PRESSED);
    lv_obj_set_style_transform_height(t, -2, LV_STATE_PRESSED);
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
            lv_obj_set_style_text_font(c, Console::sans(12), 0);
            lv_label_set_long_mode(c, LV_LABEL_LONG_DOT);
            lv_label_set_text(c, lk.caption);
            lv_obj_align(c, LV_ALIGN_BOTTOM_MID, 0, 0);
        }
    } else {
        size_t n = strlen(lk.text);
        const lv_font_t* f;
        if (n <= 1)      f = ch >= 36 ? Console::sans(28) : Console::sans(20);
        else if (n <= 3) f = ch >= 56 ? Console::sans(28) : Console::sans(20);
        else if (n <= 6) f = ch >= 36 ? Console::sans(20) : Console::sans(16);
        else             f = Console::sans(16);
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
        lv_obj_set_style_text_font(bl, Console::mono(12), 0);
        lv_obj_set_style_text_color(bl, lv_color_hex(Console::PEACH), 0);
        lv_obj_set_style_bg_color(bl, lv_color_hex(Console::INK), 0);
        lv_obj_set_style_bg_opa(bl, LV_OPA_70, 0);
        lv_obj_set_style_radius(bl, 3, 0);
        lv_obj_set_style_pad_hor(bl, 2, 0);
        lv_label_set_text(bl, lk.badge);
        lv_obj_align(bl, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_obj_add_flag(bl, LV_OBJ_FLAG_IGNORE_LAYOUT);
    }
}

// The Disk page: progress, speeds, the SHA-256 the board computed next to the one the PC sent, and the first lines of the book.
lv_obj_t* g_diskHash = nullptr;

void buildDisk(int W, int H) {
    g_diskState = lv_label_create(g_body);
    lv_obj_set_style_text_font(g_diskState, Console::sans(16), 0);
    lv_obj_set_style_text_color(g_diskState, lv_color_hex(Console::AMBER), 0);
    lv_obj_align(g_diskState, LV_ALIGN_TOP_LEFT, 8, 4);
    g_diskBar = lv_bar_create(g_body);
    lv_obj_set_size(g_diskBar, W - 16, 10);
    lv_obj_set_style_radius(g_diskBar, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_radius(g_diskBar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(g_diskBar, lv_color_hex(Console::DIM), 0);
    lv_obj_set_style_bg_color(g_diskBar, lv_color_hex(Console::AMBER), LV_PART_INDICATOR);
    lv_obj_align(g_diskBar, LV_ALIGN_TOP_LEFT, 8, 28);
    lv_bar_set_range(g_diskBar, 0, 1000);
    g_diskStats = lv_label_create(g_body);
    lv_obj_set_style_text_font(g_diskStats, Console::mono(12), 0);
    lv_obj_set_style_text_color(g_diskStats, lv_color_hex(Console::LAVENDER), 0);
    lv_obj_align(g_diskStats, LV_ALIGN_TOP_LEFT, 8, 44);
    g_diskHash = lv_label_create(g_body);
    lv_obj_set_style_text_font(g_diskHash, Console::mono(12), 0);
    lv_obj_set_style_text_color(g_diskHash, lv_color_hex(0xE0E4F0), 0);
    lv_obj_align(g_diskHash, LV_ALIGN_TOP_LEFT, 8, 98);
    g_diskText = lv_label_create(g_body);
    lv_obj_set_style_text_font(g_diskText, Console::mono(12), 0);
    lv_obj_set_style_text_color(g_diskText, lv_color_hex(0x9AA0B0), 0);
    lv_label_set_long_mode(g_diskText, LV_LABEL_LONG_CLIP);
    lv_obj_set_size(g_diskText, W - 16, H - 174);
    lv_obj_align(g_diskText, LV_ALIGN_TOP_LEFT, 8, 172);
    Disk::enter();
    g_diskLit = -2;                              // refreshDisk() fills the labels at once
}

void refreshDisk() {
    if (g_page != PAGE_DISK || !g_diskState) return;
    static uint32_t last = 0, sig = 0;
    if (millis() - last < 120 && g_diskLit != -2) return;
    last = millis();
    Disk::View v = Disk::view();
    bool sending = v.tx == Disk::Tx::Header || v.tx == Disk::Tx::Data;
    uint32_t s = v.blocksOk * 31u + v.blocksBad * 7u + v.blocksDup * 3u + (uint32_t)v.state * 1000003u + v.bytes + v.txBytes * 5u + (uint32_t)v.tx * 99991u + (uint32_t)(v.match + 2) * 77u + v.pcRate + v.txKbps10;
    if (s == sig && g_diskLit != -2) return;
    sig = s; g_diskLit = (int)(v.bytes + v.txBytes) + 7 * (int)v.state + 13 * (int)v.tx;
    static bool fastShown = false;
    if (Disk::quiet()) {                                                      // nothing else is drawn until it is done
        if (!fastShown) { fastShown = true; lv_label_set_text(g_diskState, "Fast transfer (no display, no sound)"); }
        return;
    }
    fastShown = false;
    static const char* const names[] = {"Disk drive ready", "Loading", "Checking", "Saved", "Failed"};
    if (sending) lv_label_set_text_fmt(g_diskState, "Sending back: %s", v.name);
    else if (v.tx == Disk::Tx::DoneOk) lv_label_set_text_fmt(g_diskState, "Sent back: %s", v.name);
    else if (v.tx == Disk::Tx::Failed) lv_label_set_text(g_diskState, "Sending back failed");
    else lv_label_set_text_fmt(g_diskState, "%s%s%s", names[(int)v.state], v.name[0] && v.state != Disk::State::Idle ? ": " : "", v.state != Disk::State::Idle ? v.name : "");
    uint32_t done = sending || v.tx == Disk::Tx::DoneOk ? v.txBytes : v.bytes, tot = sending || v.tx == Disk::Tx::DoneOk ? v.txTotal : v.total;
    lv_bar_set_value(g_diskBar, tot ? (int32_t)((uint64_t)done * 1000 / tot) : 0, LV_ANIM_OFF);
    lv_label_set_text_fmt(g_diskStats, "save %lu.%lu kB/s   load %lu.%lu kB/s   (PC: %lu kB/s)\nblocks ok %lu bad %lu repeated %lu  block %u B\n%lu of %lu bytes  %lu lines  crc %s",
                          (unsigned long)(v.kbps10 / 10), (unsigned long)(v.kbps10 % 10), (unsigned long)(v.txKbps10 / 10), (unsigned long)(v.txKbps10 % 10),
                          (unsigned long)(v.pcRate / 1024), (unsigned long)v.blocksOk, (unsigned long)v.blocksBad, (unsigned long)v.blocksDup, v.lastBlockBytes,
                          (unsigned long)done, (unsigned long)tot, (unsigned long)v.lines, v.state == Disk::State::DoneOk ? "ok" : v.state == Disk::State::DoneBad ? "BAD" : "-");
    char h[240];
    snprintf(h, sizeof(h), "SHA-256 board  %.32s\n               %.32s\nSHA-256 PC     %.32s\n               %.32s\n%s  (%lu ms)",
             v.sha[0] ? v.sha : "-", v.sha[0] ? v.sha + 32 : "", v.pcSha[0] ? v.pcSha : "-", v.pcSha[0] ? v.pcSha + 32 : "",
             v.match == 1 ? "MATCH" : v.match == 0 ? "DIFFERENT" : "waiting for the hash of the PC", (unsigned long)(v.shaUs / 1000));
    lv_label_set_text(g_diskHash, h);
    lv_obj_set_style_text_color(g_diskHash, v.match == 1 ? lv_color_hex(Console::PEACH) : v.match == 0 ? lv_color_hex(Console::CORAL) : lv_color_hex(0xE0E4F0), 0);
    char sd[64] = "no SD card (the file stays in RAM)";
    if (v.sdPresent) snprintf(sd, sizeof(sd), "SD card %lu MB, %lu MB used%s", (unsigned long)v.sdMB, (unsigned long)v.sdUsedMB, v.saved ? ", file written" : "");
    String body = Disk::excerpt()[0] ? String(Disk::excerpt()) : (v.state == Disk::State::Idle && !sending ? Disk::dir() : String());   // no card access while a transfer runs
    String text = String(sd) + "\n" + body;
    lv_label_set_text(g_diskText, text.c_str());
    g_ledDirty = true;
}

#if NG_PRESENTER
// The one page of the Core2 has two modes. Clicker (the default): Next is half of the screen, Previous above the small keys. Pointer: the air mouse is on and
// the page is built for it (a big Click, the state of the pointer, Laser, and smaller Previous and Next). The physical buttons: A previous, B switches the mode, C next.
lv_obj_t* g_ptrTile = nullptr; lv_obj_t* g_ptrLbl = nullptr;     // the state tile of the pointer mode
int g_ptrShown = -1; int g_bleShown = -1; int g_modeBuilt = -1;

void refreshPointer() {
    Presenter::Ptr p = Presenter::pointer();
    g_ptrShown = (int)p; g_ledDirty = true;                               // the LED bar follows the pointer
    if (!g_ptrTile) return;
    lv_obj_set_style_bg_color(g_ptrTile, lv_color_hex(p == Presenter::Ptr::Active ? Console::AMBER : Console::LAVENDER), 0);
    lv_label_set_text(g_ptrLbl, p == Presenter::Ptr::Active ? "Pointer\nmoving" : "Pointer\nstill");
}
void scheduleRebuild() { g_pageDirty = true; g_pageStamp = millis(); g_ledDirty = true; }
void onAction(lv_event_t* e) {
    Presenter::Action a = (Presenter::Action)(intptr_t)lv_event_get_user_data(e);
    Presenter::send(a); g_pressCount++; g_lastKey = Presenter::name(a);
}
void onClickTile(lv_event_t*) { Presenter::click(); g_pressCount++; g_lastKey = "click"; }
void onMode(lv_event_t*) { Presenter::togglePointer(); g_pressCount++; g_lastKey = "mode"; scheduleRebuild(); }   // the page is rebuilt from the loop, not from inside this event

lv_obj_t* presenterTile(int x, int y, int w, int h, uint32_t colour, const char* text, const char* icon, int fontPx, lv_event_cb_t cb, intptr_t ud) {
    lv_obj_t* t = lv_button_create(g_body);
    lv_obj_set_pos(t, x, y); lv_obj_set_size(t, w, h);
    Console::block(t, colour);
    lv_obj_remove_flag(t, LV_OBJ_FLAG_PRESS_LOCK);
    if (cb) lv_obj_add_event_cb(t, cb, LV_EVENT_CLICKED, (void*)ud);
    else lv_obj_remove_flag(t, LV_OBJ_FLAG_CLICKABLE);
    if (icon) {
        lv_obj_t* ic = lv_label_create(t);
        lv_obj_set_style_text_font(ic, icons_font(36), 0);
        lv_label_set_text(ic, icon);
        lv_obj_align(ic, LV_ALIGN_CENTER, 0, -8);
    }
    lv_obj_t* l = lv_label_create(t);
    lv_obj_set_style_text_font(l, Console::sans(fontPx), 0);
    lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text(l, text);
    lv_obj_align(l, LV_ALIGN_BOTTOM_RIGHT, -6, -4);
    return t;
}

void buildPresenter(int W, int H) {
    g_ptrTile = nullptr; g_ptrLbl = nullptr;
    const bool pointerMode = Presenter::pointerEnabled();
    g_modeBuilt = pointerMode;
    const int ah = H - 2 * GAP;                                   // the height inside the gaps
    if (!pointerMode) {
        int wl = (W - 3 * GAP) / 2, wr = W - 3 * GAP - wl, xr = 2 * GAP + wl;
        int h1 = ah * 40 / 100, hs = (ah - h1 - 2 * GAP) / 2, ws = (wl - GAP) / 2;
        int ys1 = 2 * GAP + h1, ys2 = ys1 + hs + GAP;
        presenterTile(xr, GAP, wr, ah, Console::AMBER, "Next", IC_ARROW_FORWARD, 28, onAction, (intptr_t)Presenter::Action::Next);   // the biggest: half of the screen
        presenterTile(GAP, GAP, wl, h1, Console::LAVENDER, "Previous", IC_ARROW_BACK, 20, onAction, (intptr_t)Presenter::Action::Prev);
        presenterTile(GAP, ys1, ws, hs, Console::PEACH, "Start", nullptr, 16, onAction, (intptr_t)Presenter::Action::Start);
        presenterTile(2 * GAP + ws, ys1, ws, hs, Console::PEACH, "Blank", nullptr, 16, onAction, (intptr_t)Presenter::Action::Blank);
        presenterTile(GAP, ys2, ws, hs, Console::CORAL, "End", nullptr, 16, onAction, (intptr_t)Presenter::Action::End);
        presenterTile(2 * GAP + ws, ys2, ws, hs, Console::LAVENDER, "Pointer\nmode", nullptr, 16, onMode, 0);
    } else {
        int r1 = ah * 42 / 100, r2 = ah * 26 / 100, r3 = ah - r1 - r2 - 2 * GAP;
        int wc = (W - 3 * GAP) * 60 / 100, wl = W - 3 * GAP - wc, wh = (W - 3 * GAP) / 2, wh2 = W - 3 * GAP - wh;
        int y2 = 2 * GAP + r1, y3 = y2 + r2 + GAP;
        presenterTile(GAP, GAP, wc, r1, Console::PEACH, "Click", nullptr, 28, onClickTile, 0);
        presenterTile(2 * GAP + wc, GAP, wl, r1, Console::LAVENDER, "Laser", nullptr, 20, onAction, (intptr_t)Presenter::Action::Laser);
        g_ptrTile = presenterTile(GAP, y2, wh, r2, Console::LAVENDER, "Pointer\nstill", nullptr, 16, nullptr, 0);
        g_ptrLbl = lv_obj_get_child(g_ptrTile, 0);
        presenterTile(2 * GAP + wh, y2, wh2, r2, Console::CORAL, "Clicker\nmode", nullptr, 16, onMode, 0);
        presenterTile(GAP, y3, wh, r3, Console::PEACH, "Previous", IC_ARROW_BACK, 16, onAction, (intptr_t)Presenter::Action::Prev);
        presenterTile(2 * GAP + wh, y3, wh2, r3, Console::AMBER, "Next", IC_ARROW_FORWARD, 16, onAction, (intptr_t)Presenter::Action::Next);
        refreshPointer();
    }
}
#endif

void buildPage() {
    uint32_t perfT0 = micros();
    g_perfRenderUs = 0; g_perfFlushUs = 0; g_perfStrips = 0;
    lv_obj_clean(g_body);
#if !NG_PRESENTER
    for (int i = 0; i < LOCK_COUNT; i++) { g_lockTiles[i] = nullptr; g_lockState[i] = nullptr; g_lockLed[i] = nullptr; }
#endif
    int W = M5.Display.width(), H = M5.Display.height() - HEADER_H;
    Lamps::leavePage();
    Disk::leave();
    g_diskBar = g_diskState = g_diskStats = g_diskText = nullptr; g_diskLit = -1;

    setHeader(g_page);
#if NG_PRESENTER
    buildPresenter(W, H);
#else
    if (g_page >= PAGE_LAMP_SCREEN) {
        g_lampEnter = true;                     // the body stays empty; Lamps draws on the panel in ui_loop()
    } else if (g_page == PAGE_DISK) {
        buildDisk(W, H);
    } else if (g_page == LAYER_COUNT) {
        // two rows: Caps, Num, Scroll and below them Compose and Kana, each tile with its own small LED
        int w3 = (W - 4 * GAP) / 3, w2 = (W - 3 * GAP) / 2, h = (H - 3 * GAP) / 2;
        static const char* const LOCK_ICONS[LOCK_COUNT] = {IC_KEYBOARD_CAPSLOCK, IC_DIALPAD, IC_LOCK, IC_MENU, IC_LANGUAGE};
        for (int i = 0; i < LOCK_COUNT; i++) {
            int x = i < 3 ? GAP + i * (w3 + GAP) : GAP + (i - 3) * (w2 + GAP), y = i < 3 ? GAP : 2 * GAP + h, w = i < 3 ? w3 : w2;
            lv_obj_t* t = makeTile(g_body, x, y, w, h, lv_color_hex(Console::DIM), lv_color_hex(Console::PEACH), i);
            lv_obj_t* ic = lv_label_create(t);
            lv_obj_set_style_text_font(ic, icons_font(28), 0);
            lv_label_set_text(ic, LOCK_ICONS[i]);
            lv_obj_align(ic, LV_ALIGN_CENTER, 0, -24);
            lv_obj_t* l = lv_label_create(t);
            lv_obj_set_style_text_font(l, Console::sans(16), 0);
            lv_label_set_text(l, LOCKS[i].name);
            lv_obj_align(l, LV_ALIGN_CENTER, 0, 2);
            lv_obj_t* s = lv_label_create(t);
            lv_obj_set_style_text_font(s, Console::sans(20), 0);
            lv_obj_align(s, LV_ALIGN_CENTER, 0, 26);
            g_lockTiles[i] = t; g_lockState[i] = s;
            lv_obj_t* led = lv_canvas_create(t);
            drawLed(i, false);
            if (g_ledBuf[i]) lv_canvas_set_buffer(led, g_ledBuf[i], LED_PX, LED_PX, LV_COLOR_FORMAT_ARGB8888);
            lv_obj_align(led, LV_ALIGN_TOP_RIGHT, -4, 4);
            lv_obj_remove_flag(led, LV_OBJ_FLAG_CLICKABLE);
            g_lockLed[i] = led;
        }
        refreshLocks();
    } else {
        const GridLayer& L = LAYERS[g_page];
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
#endif
    g_ledDirty = true;
    g_perfBuildUs = micros() - perfT0;
}

#if !NG_HAS_BTNS
lv_obj_t* navBtn(const char* txt, int dir, lv_align_t al) {
    lv_obj_t* b = lv_button_create(g_header);
    lv_obj_set_size(b, 72, HEADER_H - 4);
    lv_obj_align(b, al, al == LV_ALIGN_LEFT_MID ? 2 : -2, 0);
    Console::block(b, Console::LAVENDER);                       // pressed: amber
    lv_obj_remove_flag(b, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_t* l = lv_label_create(b);
    lv_obj_set_style_text_font(l, icons_font(28), 0);
    lv_label_set_text(l, txt);
    lv_obj_center(l);
    lv_obj_add_event_cb(b, onNav, LV_EVENT_CLICKED, (void*)(intptr_t)dir);
    return b;
}
#endif
} // namespace

// Boot check: every tile that has a usage must be sendable (see HIDManager::canSend). A tile that is not is reported once
// on the serial port and ignored when pressed, instead of silently doing nothing.
static void checkKeymap() {
    int bad = 0;
    for (int l = 0; l < LAYER_COUNT; l++) {
        const GridLayer& L = LAYERS[l];
        for (int i = 0; i < L.rows * L.cols; i++) {
            const Binding& b = L.tiles[i];
            if (b.usage.id == 0 || HIDManager::canSend(b)) continue;
            Serial.printf("[keymap] layer %d (%s) tile %d: %s on page 0x%04X has no report\n", l, L.title, i, b.usage.identifier, (unsigned)b.page);
            bad++;
        }
    }
    if (!bad) Serial.println("[keymap] every tile can be sent");
}

// ---------- public API ----------
uint8_t ui_page_count() { return NG_PRESENTER ? 1 : LAYER_COUNT + 4; }
int ui_layer() { return g_page; }
uint8_t ui_locks_page() { return NG_PRESENTER ? 255 : LAYER_COUNT; }   // 255: this board has no Locks page
void ui_toggle_locks() { ui_set_layer(g_page == LAYER_COUNT ? g_prevPage : LAYER_COUNT); }

void ui_begin() {
#if NG_HAS_LEDBAR
    FastLED.addLeds<WS2812, LED_PIN, GRB>(g_leds, LED_COUNT);   // the 5 V rail and the brightness follow the settings (updateLeds)
#endif
    g_ledMutex = xSemaphoreCreateMutex();
    Lamps::setBarSink(updateLeds);
    xTaskCreatePinnedToCore(effectTask, "bar-fx", 4096, nullptr, 1, nullptr, ARDUINO_RUNNING_CORE == 0 ? 1 : 0);   // the lamp worker task puts lamps 0..9 on the bar without waiting for the main loop
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
    lv_obj_set_style_bg_color(scr, lv_color_hex(Console::INK), 0);
    lv_obj_set_style_text_color(scr, lv_color_hex(0xe6e9f0), 0);
    lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    icons_init();
    Console::begin();
    g_header = lv_obj_create(scr);
    lv_obj_remove_style_all(g_header);
    lv_obj_set_size(g_header, W, HEADER_H);
    lv_obj_set_pos(g_header, 0, NG_HAS_BTNS ? 0 : H - HEADER_H);
    lv_obj_set_style_bg_color(g_header, lv_color_hex(Console::INK), 0);
    lv_obj_set_style_bg_opa(g_header, LV_OPA_COVER, 0);
    lv_obj_remove_flag(g_header, LV_OBJ_FLAG_SCROLLABLE);
#if NG_HAS_BTNS
    // Core2: a slim bar of rounded blocks. Title (amber), page number (lavender) and the three indicators A, B, C that light up while
    // the capacitive buttons below the screen are held (or while the shell holds them: btn a down).
    lv_obj_t* tp = lv_obj_create(g_header);
    lv_obj_remove_style_all(tp); Console::pill(tp, Console::AMBER);
    lv_obj_set_pos(tp, 2, 2); lv_obj_set_size(tp, 162, HEADER_H - 4);
    lv_obj_remove_flag(tp, (lv_obj_flag_t)(LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE));
    g_title = lv_label_create(tp);
    lv_obj_set_style_text_font(g_title, Console::sans(12), 0);
    lv_obj_set_width(g_title, 148);
    lv_obj_set_style_text_align(g_title, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_long_mode(g_title, LV_LABEL_LONG_DOT);
    lv_obj_align(g_title, LV_ALIGN_RIGHT_MID, -8, 0);
    lv_obj_t* pp = lv_obj_create(g_header);
    lv_obj_remove_style_all(pp); Console::pill(pp, Console::LAVENDER);
    lv_obj_set_pos(pp, 167, 2); lv_obj_set_size(pp, 52, HEADER_H - 4);
    lv_obj_remove_flag(pp, (lv_obj_flag_t)(LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE));
    g_pageLbl = lv_label_create(pp);
    lv_obj_set_style_text_font(g_pageLbl, Console::mono(12), 0);
    lv_obj_center(g_pageLbl);
    static const char* const BTN_NAMES[3] = {"A", "B", "C"};
    for (int i = 0; i < 3; i++) {
        lv_obj_t* bp = lv_obj_create(g_header);
        lv_obj_remove_style_all(bp); Console::pill(bp, Console::PEACH);
        lv_obj_set_pos(bp, 222 + i * 32, 2); lv_obj_set_size(bp, 30, HEADER_H - 4);
        lv_obj_remove_flag(bp, (lv_obj_flag_t)(LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE));
        lv_obj_t* bl = lv_label_create(bp);
        lv_obj_set_style_text_font(bl, Console::sans(12), 0);
        lv_label_set_text(bl, BTN_NAMES[i]);
        lv_obj_center(bl);
        g_btnPill[i] = bp;
    }
#else
    g_title = lv_label_create(g_header);
    g_pageLbl = lv_label_create(g_header);
    lv_obj_add_flag(g_pageLbl, LV_OBJ_FLAG_HIDDEN);
    navBtn(LV_SYMBOL_LEFT_ICON, -1, LV_ALIGN_LEFT_MID);
    navBtn(LV_SYMBOL_RIGHT_ICON, +1, LV_ALIGN_RIGHT_MID);
    // centre: a peach block with the title and the page number at its right; tap = jump to / back from Locks
    lv_obj_t* mid = lv_button_create(g_header);
    lv_obj_set_size(mid, W - 2 * 76, HEADER_H - 4);
    lv_obj_align(mid, LV_ALIGN_CENTER, 0, 0);
    Console::block(mid, Console::PEACH);
    lv_obj_remove_flag(mid, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_add_event_cb(mid, onLocksToggle, LV_EVENT_CLICKED, nullptr);
    lv_obj_set_parent(g_title, mid);
    lv_obj_set_style_text_font(g_title, Console::sans(16), 0);
    lv_obj_set_width(g_title, W - 2 * 76 - 24);
    lv_obj_set_style_text_align(g_title, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_long_mode(g_title, LV_LABEL_LONG_DOT);
    lv_obj_align(g_title, LV_ALIGN_RIGHT_MID, -8, 0);
#endif

    g_body = lv_obj_create(scr);
    lv_obj_remove_style_all(g_body);
    lv_obj_set_pos(g_body, 0, NG_HAS_BTNS ? HEADER_H : 0);
    lv_obj_set_size(g_body, W, H - HEADER_H);
    lv_obj_remove_flag(g_body, LV_OBJ_FLAG_SCROLLABLE);

    buildPage();
    checkKeymap();
}

void ui_set_layer(int n) {
    int cnt = ui_page_count();
    int np = ((n % cnt) + cnt) % cnt;   // with one page (the presenter) this is always 0
    if (np == LAYER_COUNT && g_page != LAYER_COUNT) g_prevPage = g_page;
    g_page = np;
    // Only the target and the header change now; the body is built once in ui_loop() (see PAGE_COALESCE_MS), so a burst
    // of presses costs one page. The LED bar follows at once. Callers run outside LVGL's event dispatch (serial and the
    // main loop), and from the nav buttons the pressed object lives in the header, not in the body.
    setHeader(g_page);
    g_pageDirty = true;
    g_pageStamp = millis();
    g_ledDirty = true;
}

bool ui_press(int layer, int idx, bool fromSerial) {
    if (NG_PRESENTER) return false;                                  // the presenter has no keys to press by number
    if (layer < 0 || layer >= (int)ui_page_count() || layer > LAYER_COUNT) return false; // lamp pages have no keys
    if (layer == LAYER_COUNT) {
        if (idx < 0 || idx >= LOCK_COUNT) return false;
        g_lastLock = idx;
        g_lastKey = LOCKS[idx].name;
        g_pressCount++;
        switch (idx) {
            case LK_CAPS:   HIDManager::send(kp_CAPS_LOCK); break;
            case LK_NUM:    HIDManager::send(kp_KP_NUM_LOCK_AND_CLEAR); break;
            case LK_SCROLL: HIDManager::send(kp_SCROLL_LOCK); break;
            case LK_COMPOSE: HIDManager::send(kp_APPLICATION); break;           // KEY_COMPOSE on Linux
            default:        HIDManager::send(kp_INTERNATIONAL2); break;         // Katakana/Hiragana: KEY_KATAKANAHIRAGANA
        }
        g_hostLed = HIDManager::getLedStatus();
        g_ledDirty = true;
        return true;
    }
    const GridLayer& L = LAYERS[layer];
    if (idx < 0 || idx >= L.rows * L.cols) return false;
    const Binding& b = L.tiles[idx];
    if (!HIDManager::canSend(b)) return false;                   // empty tile, or a usage without a report in the report map
    // Power Down (0x81) and Sleep (0x82) really power down or suspend the computer: a test over the serial port must never press them.
    if (fromSerial && b.page == HID::Page::GENERIC_DESKTOP && (b.usage.id == 0x81 || b.usage.id == 0x82)) return false;
    g_lastKey = b.label ? b.label : b.usage.identifier;
    g_pressCount++;
    HIDManager::send(b);
    return true;
}

void ui_leds_refresh() { g_ledDirty = true; }
void ui_bar_colors(uint32_t out[10]) { for (int i = 0; i < 10; i++) out[i] = g_barSnap[i]; }

bool ui_tap(int x, int y) {
    flushPage();
    if (g_page != LAYER_COUNT && !NG_PRESENTER) return false;   // only the Locks page (every tile of the presenter page is harmless too): no tile there can power down or suspend the host
    g_injX = x; g_injY = y; g_injUntil = millis() + 120;
    return true;
}

// Hold a touch until it is released: for tests that must see the pressed state. Locks page or header only (no key can be sent by it:
// the release first moves off every control, so no click fires).
bool ui_touch(int x, int y, bool down) {
    if (down) {
        flushPage();
        bool inHeader = NG_HAS_BTNS ? y < HEADER_H : y >= M5.Display.height() - HEADER_H;
        if (g_page != LAYER_COUNT && !NG_PRESENTER && !inHeader) return false;
        g_holdX = x; g_holdY = y; g_holdOn = true; g_holdEnd = 0; g_lastActive = millis();
        return true;
    }
    if (!g_holdOn) return false;
    g_holdX = 1; g_holdY = NG_HAS_BTNS ? HEADER_H + 1 : 1;   // a gap between the tiles: leaving the control cancels the click
    g_holdEnd = millis() + 120;
    return true;
}

// Core2 buttons A, B, C (0..2): down runs the action of the button and lights its indicator; up puts the indicator out.
bool ui_button(int i, bool down) {
#if NG_HAS_BTNS
    if (i < 0 || i > 2) return false;
    if (down) {
        g_btnDown |= 1u << i; g_lastActive = millis();
        lv_obj_add_state(g_btnPill[i], LV_STATE_PRESSED);
#if NG_PRESENTER
        g_pressCount++;
        if (i == 0) { Presenter::send(Presenter::Action::Prev); g_lastKey = "prev"; }
        else if (i == 2) { Presenter::send(Presenter::Action::Next); g_lastKey = "next"; }
        else { Presenter::togglePointer(); g_lastKey = "mode"; scheduleRebuild(); }
#else
        if (i == 0) ui_set_layer(g_page - 1); else if (i == 1) ui_toggle_locks(); else ui_set_layer(g_page + 1);
#endif
    } else {
        g_btnDown &= ~(1u << i);
        lv_obj_remove_state(g_btnPill[i], LV_STATE_PRESSED);
    }
    return true;
#else
    (void)i; (void)down; return false;
#endif
}

void ui_wake() { g_lastActive = millis(); g_blOverride = -1; }
void ui_backlight(int level) { g_blOverride = level < 0 ? -1 : (level > 255 ? 255 : level); }

String ui_state() {
    uint8_t led = HIDManager::getLedStatus();
    char b[320];
    snprintf(b, sizeof(b), "STATE page=%d/%d title=%s caps=%d num=%d scroll=%d compose=%d kana=%d last=%s presses=%lu conn=%d usb=%d ble=%d touch=%d btn=%d ptr=%s clicks=%lu",
             g_page, (int)ui_page_count(),
             NG_PRESENTER ? "Presenter" : g_page == PAGE_DISK ? "Disk" : g_page == PAGE_LAMP_SCREEN ? "Lamps-screen" : g_page == PAGE_LAMP_BAR ? "Lamps-bar" : g_page == LAYER_COUNT ? "Locks" : LAYERS[g_page].title,
             !!(led & HIDManager::LED_CAPS), !!(led & HIDManager::LED_NUM), !!(led & HIDManager::LED_SCROLL), !!(led & HIDManager::LED_COMPOSE), !!(led & HIDManager::LED_KANA),
             g_lastKey, (unsigned long)g_pressCount, HIDManager::isConnected(), HIDManager::isUsbConnected(), HIDManager::isBleConnected(), (int)g_holdOn, (int)g_btnDown, NG_PRESENTER ? Presenter::pointerName() : "-", (unsigned long)Presenter::clicks());
    return String(b);
}

String ui_perf() {
    char b[160];
    snprintf(b, sizeof(b), "PERF build_ms=%.1f render_ms=%.1f flush_ms=%.1f strips=%lu page=%d",
             g_perfBuildUs / 1000.0, g_perfRenderUs / 1000.0, g_perfFlushUs / 1000.0, (unsigned long)g_perfStrips, g_page);
    return String(b);
}

void ui_flush() { flushPage(); }

void ui_screenshot(Stream& out) {
    int W = M5.Display.width(), H = M5.Display.height();
    flushPage();                                     // a LAYER may still be waiting to be built
    uint32_t perfT0 = micros(), perfStrips0 = g_perfStrips;
    lv_refr_now(nullptr);
    if (g_perfStrips != perfStrips0) g_perfRenderUs += micros() - perfT0;   // the SHOT forced a render: count it
    out.printf("SHOT %d %d\n", W, H);
    static uint8_t row[320 * 3];
    for (int y = 0; y < H; y++) {
        M5.Display.readRectRGB(0, y, W, 1, row);
        out.write(row, W * 3);
        if ((y & 7) == 7) { out.flush(); delay(1); } // let a UART bridge drain; avoids lost tail
    }
    out.print("\nEND\n");
}

// Backlight: dim after "dim" seconds without touch or button (default 60), off after "off" seconds (default 300); any
// touch wakes it (no re-init needed, unlike deep sleep). The LED bar goes dark after "ledidle" minutes. The serial
// commands WAKE (restart the timers) and BL n (fixed level, BL AUTO to return) override the timers.
static void idleBacklight() {
    static int level = -1;
    uint8_t active = Settings::backlight();
    bool touched = M5.Touch.getCount() > 0;
    if (touched && level >= 0 && level < active) g_swallowTouch = true;
    if (touched || M5.BtnA.isPressed() || M5.BtnB.isPressed() || M5.BtnC.isPressed()) g_lastActive = millis();
    uint32_t idle = millis() - g_lastActive;
    int want = idle < (uint32_t)Settings::dimSec() * 1000 ? active : (idle < (uint32_t)Settings::offSec() * 1000 ? 16 : 0);
    if (g_blOverride >= 0) want = g_blOverride;
    if (want != level) { level = want; M5.Display.setBrightness(want); }
    bool ledIdle = Settings::ledIdleMin() > 0 && idle > (uint32_t)Settings::ledIdleMin() * 60000UL;
    if (ledIdle != g_ledIdleOff) { g_ledIdleOff = ledIdle; g_ledDirty = true; }
}

uint32_t ui_loop() {
    static uint32_t lastLedPoll = 0;
    idleBacklight();
#if NG_PRESENTER
    if (g_modeBuilt != (int)Presenter::pointerEnabled() && !g_pageDirty) scheduleRebuild();      // the mode was changed (shell, button, tile): build the other page
    if (g_ptrShown != (int)Presenter::pointer()) refreshPointer();                        // the pointer fell asleep or woke up
    if (g_bleShown != (int)HIDManager::isBleConnected()) { g_bleShown = (int)HIDManager::isBleConnected(); setHeader(g_page); }
#endif
    if (g_pageDirty && millis() - g_pageStamp >= PAGE_COALESCE_MS) flushPage();
    // Keep in sync with the HID layer (local toggles do not always fire the listener).
    if (millis() - lastLedPoll > 100) {
        lastLedPoll = millis();
        uint8_t l = HIDManager::getLedStatus();
        if (l != g_hostLed) { g_hostLed = l; g_ledDirty = true; }
    }
    bool lampBar = g_page >= PAGE_LAMP_SCREEN && Lamps::barChanged();   // worker off: a LampArray frame touched lamps 0..9
    if (lampBar) g_ledDirty = true;
    if (g_ledDirty) {
        g_ledDirty = false;
#if !NG_PRESENTER
        if (g_page == LAYER_COUNT) refreshLocks();
#endif
        updateLeds();
        if (lampBar) Lamps::barShown();
    }
    uint32_t perfT0 = micros(), perfStrips0 = g_perfStrips;
    uint32_t next = lv_timer_handler();
    if (g_perfStrips != perfStrips0) g_perfRenderUs += micros() - perfT0;
    if (g_lampEnter) {
        g_lampEnter = false;
        lv_refr_now(nullptr); // let LVGL paint the empty body first, the lamps are drawn on top of it
        Lamps::enterPage(g_page == PAGE_LAMP_SCREEN ? Lamps::Page::Screen : Lamps::Page::Bar, 0, NG_HAS_BTNS ? HEADER_H : 0,
                         M5.Display.width(), M5.Display.height() - HEADER_H);
    }
    Lamps::tick();
    Disk::loop();                                    // acknowledgements go out as key presses
    refreshDisk();
    if (g_pageDirty || Lamps::pending() || (g_page == PAGE_DISK && Disk::view().state == Disk::State::Receiving)) return 1;                       // a page is waiting for its coalescing window
    return next < 1 ? 1 : (next > 10 ? 10 : next);
}
