#include "Lamps.h"
#include "HID_Specs.h"
#include "Settings.h"
#include <M5Unified.h>
#include <algorithm>
#include <esp_heap_caps.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// HID LampArray test bench: storage, report handling and the two lamp pages. See Lamps.h and ADR 0010.
// The screen lamps are drawn straight to the panel (no LVGL widgets): a RGB565 copy of the lamp area lives in
// PSRAM, a frame only repaints the rows that contain changed lamps.

namespace Lamps {
namespace {

struct Lamp { uint8_t r, g, b, i; };

constexpr int STRIP_H = 12;               // status text line at the bottom of the body
constexpr uint16_t MAX_COLS = 320, MAX_ROWS = 190;

Lamp* g_lamp = nullptr;                   // BAR_LAMPS + maxGrid entries (PSRAM when present)
uint8_t* g_dirty = nullptr;               // one bit per lamp, set by the report handlers, cleared by the drawing code
uint32_t g_maxGrid = 0;
uint16_t g_cols = 16, g_rows = 12;
volatile bool g_auto = true;
volatile uint16_t g_reqId = 0;
int g_brightOverride = -1;                 // LAMPS BRIGHT (a test command): cleared by LAMPS RESET, never stored

// counters since resetCounters()
volatile uint32_t c_reports, c_multi, c_range, c_ctrl, c_short, c_bad, c_lampsSet, c_frames, c_bytes, c_featGet, c_featSet;
volatile int64_t c_t0, c_t1;
volatile bool g_barTouched, g_barReady;
// Latency from the report that completes a frame to what the user sees (microseconds): the bar on the worker task or the main
// loop, and the screen. Average and maximum since LAMPS RESET.
struct Lat { uint32_t n = 0, max = 0; uint64_t sum = 0; };
Lat g_latBar, g_latScr;
inline void latAdd(Lat& l, int64_t us) { uint32_t u = us < 0 ? 0 : (uint32_t)us; l.n++; l.sum += u; if (u > l.max) l.max = u; }
volatile int64_t g_barStampUs = 0, g_pendingSinceUs = 0, g_mainBarStamp = 0;
volatile uint32_t c_barShows = 0;
TaskHandle_t g_worker = nullptr;
volatile bool g_workerOn = true;
void (*g_sink)() = nullptr;
void (*g_frameSink)() = nullptr;
uint32_t g_drawn, g_drawnFrames, g_drawUs;

// page state
Page g_page = Page::None;
int g_ax, g_ay, g_aw, g_ah;               // body rectangle on the screen
uint16_t* g_fb = nullptr;                 // RGB565 copy of the grid area (aw x gridH)
int g_gh, g_cw, g_ch, g_ox, g_oy;         // grid area height, cell size, grid origin inside the area
bool g_full = false;
uint32_t g_lastStrip = 0;
uint32_t g_barShown[BAR_LAMPS];

inline uint16_t c565(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }
inline uint8_t scale(uint8_t v, uint8_t i) { return (uint8_t)(((uint16_t)v * i + 127) / 255); }
inline uint16_t lampTotal() { return BAR_LAMPS + g_cols * g_rows; }

inline void put16(uint8_t*& p, uint16_t v) { *p++ = v & 0xFF; *p++ = v >> 8; }
inline void put32(uint8_t*& p, uint32_t v) { for (int k = 0; k < 4; k++) { *p++ = v & 0xFF; v >>= 8; } }

void store(uint16_t id, uint8_t r, uint8_t g, uint8_t b, uint8_t i) {
    if (id >= lampTotal() || !g_lamp) { c_bad++; return; }
    g_lamp[id] = {r, g, b, i};
    __atomic_fetch_or(&g_dirty[id >> 3], (uint8_t)(1u << (id & 7)), __ATOMIC_RELAXED);
    if (id < BAR_LAMPS) g_barTouched = true;
    c_lampsSet++;
}

void frameComplete() {
    int64_t now = esp_timer_get_time();
    c_frames++;
    if (g_pendingSinceUs == 0) g_pendingSinceUs = now;
    if (g_barTouched) {
        g_barTouched = false; g_barStampUs = now;
        if (g_workerOn && g_worker && g_sink && g_page != Page::None) xTaskNotifyGive(g_worker);   // the worker shows it at once, on its own core
        else g_barReady = true;                                                                       // the main loop shows it
    }
    if (g_frameSink) g_frameSink();
}

// Runs on the core the main loop does not use: puts the bar lamps on the LED bar as soon as a frame has changed them, so neither the
// screen drawing nor the main loop's sleep delays the bar.
void workerTask(void*) {
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (!g_workerOn || g_page == Page::None || !g_sink) continue;
        int64_t stamp = g_barStampUs;
        g_sink();
        if (stamp) { latAdd(g_latBar, esp_timer_get_time() - stamp); c_barShows++; }
    }
}

void note(uint16_t len) {
    int64_t now = esp_timer_get_time();
    if (c_t0 == 0) c_t0 = now;
    c_t1 = now;
    c_reports++;
    c_bytes += len + 1;
}

// ---- drawing ----------------------------------------------------------------------------------------------
int g_dirtyMin, g_dirtyMax;

void drawCell(uint32_t k) {               // k = grid lamp index (lamp id - BAR_LAMPS)
    uint16_t c = k % g_cols, r = k / g_cols;
    const Lamp& l = g_lamp[BAR_LAMPS + k];
    uint16_t col = c565(scale(l.r, l.i), scale(l.g, l.i), scale(l.b, l.i));
    int x0 = g_ox + c * g_cw, y0 = g_oy + r * g_ch;
    for (int yy = 0; yy < g_ch; yy++) {
        uint16_t* p = g_fb + (size_t)(y0 + yy) * g_aw + x0;
        for (int xx = 0; xx < g_cw; xx++) p[xx] = col;
    }
    if (y0 < g_dirtyMin) g_dirtyMin = y0;
    if (y0 + g_ch - 1 > g_dirtyMax) g_dirtyMax = y0 + g_ch - 1;
}

void pushRows(int y0, int y1) {
    if (y1 < y0) return;
    M5.Display.startWrite();
    M5.Display.pushImage(g_ax, g_ay + y0, g_aw, y1 - y0 + 1, (const lgfx::rgb565_t*)(g_fb + (size_t)y0 * g_aw));
    M5.Display.endWrite();
}

void drawStrip(bool force) {
    uint32_t now = millis();
    if (!force && now - g_lastStrip < 250) return;
    g_lastStrip = now;
    char b[64];
    snprintf(b, sizeof(b), "%ux%u n=%u rx=%lu fr=%lu %s", g_cols, g_rows, (unsigned)lampTotal(),
             (unsigned long)c_reports, (unsigned long)c_frames, g_auto ? "auto" : "host");
    M5.Display.startWrite();
    M5.Display.fillRect(g_ax, g_ay + g_ah - STRIP_H, g_aw, STRIP_H, TFT_BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Display.drawString(b, g_ax + 2, g_ay + g_ah - STRIP_H + 2);
    M5.Display.endWrite();
}

void drawBarPage() {
    int gap = 3, sw = (g_aw - 11 * gap) / 10, sh = g_ah - STRIP_H - 22;
    M5.Display.startWrite();
    M5.Display.fillRect(g_ax, g_ay, g_aw, g_ah - STRIP_H, 0x0841);
    M5.Display.setTextSize(1);
    for (uint8_t i = 0; i < BAR_LAMPS; i++) {
        const Lamp& l = g_lamp[i];
        int x = g_ax + gap + i * (sw + gap), y = g_ay + 6;
        M5.Display.fillRect(x, y, sw, sh, c565(scale(l.r, l.i), scale(l.g, l.i), scale(l.b, l.i)));
        M5.Display.drawRect(x - 1, y - 1, sw + 2, sh + 2, 0x4208);
        char t[8];
        snprintf(t, sizeof(t), "%3u", l.i);
        M5.Display.setTextColor(TFT_WHITE, 0x0841);
        M5.Display.drawString(t, x + 2, y + sh + 4);
        snprintf(t, sizeof(t), "%u", i);
        M5.Display.setTextColor(0x8410, 0x0841);
        M5.Display.drawString(t, x + sw / 2 - 2, y + sh + 13);
    }
    M5.Display.endWrite();
}

void freeFb() { if (g_fb) { heap_caps_free(g_fb); g_fb = nullptr; } }

void layoutGrid() {
    g_gh = g_ah - STRIP_H;
    g_cw = std::max(1, g_aw / g_cols);
    g_ch = std::max(1, g_gh / g_rows);
    g_ox = (g_aw - g_cw * g_cols) / 2;
    g_oy = (g_gh - g_ch * g_rows) / 2;
}

void fullScreenRedraw() {
    layoutGrid();
    freeFb();
    size_t bytes = (size_t)g_aw * g_gh * 2;
    g_fb = (uint16_t*)heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!g_fb) g_fb = (uint16_t*)heap_caps_malloc(bytes, MALLOC_CAP_8BIT);
    if (!g_fb) return;
    memset(g_fb, 0, bytes);
    memset(g_dirty, 0, ((size_t)lampTotal() + 7) / 8);
    g_dirtyMin = g_gh; g_dirtyMax = -1;
    for (uint32_t k = 0; k < (uint32_t)g_cols * g_rows; k++) drawCell(k);
    pushRows(0, g_gh - 1);
    g_dirtyMin = g_gh; g_dirtyMax = -1;
    g_drawnFrames = c_frames;
    g_full = false;
}

} // namespace

// ---- public ---------------------------------------------------------------------------------------------------
void begin() {
    bool psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM) > 1024 * 1024;
    g_maxGrid = psram ? (uint32_t)MAX_COLS * MAX_ROWS : 2400;
    size_t n = BAR_LAMPS + g_maxGrid;
    g_lamp = (Lamp*)heap_caps_calloc(n, sizeof(Lamp), psram ? MALLOC_CAP_SPIRAM : MALLOC_CAP_8BIT);
    g_dirty = (uint8_t*)heap_caps_calloc((n + 7) / 8, 1, MALLOC_CAP_8BIT);
    if (!g_lamp || !g_dirty) { g_maxGrid = 0; g_cols = g_rows = 0; }
    xTaskCreatePinnedToCore(workerTask, "lamp-bar", 3072, nullptr, 2, &g_worker, ARDUINO_RUNNING_CORE == 0 ? 1 : 0);
    Serial.printf("[Lamps] %u lamps (bar %u + grid %ux%u), max grid %lu, psram=%d\n", (unsigned)lampTotal(), BAR_LAMPS,
                  g_cols, g_rows, (unsigned long)g_maxGrid, (int)psram);
}

uint16_t count() { return lampTotal(); }
uint16_t maxGridLamps() { return (uint16_t)std::min<uint32_t>(g_maxGrid, 65535 - BAR_LAMPS); }
uint16_t cols() { return g_cols; }
uint16_t rows() { return g_rows; }
bool autonomous() { return g_auto; }

bool setGrid(uint16_t cols, uint16_t rows) {
    if (!g_lamp || cols < 1 || rows < 1 || cols > MAX_COLS || rows > MAX_ROWS || (uint32_t)cols * rows > g_maxGrid) return false;
    g_cols = cols; g_rows = rows;
    memset(g_lamp + BAR_LAMPS, 0, (size_t)cols * rows * sizeof(Lamp));
    memset(g_dirty, 0, ((size_t)lampTotal() + 7) / 8);
    g_full = true;                       // the page redraws on the next tick
    return true;
}

void onOutput(uint8_t id, const uint8_t* d, uint16_t len) {
    using namespace HID::Reports;
    uint16_t want = id == LAMP_MULTI ? LAMP_MULTI_LEN : id == LAMP_RANGE ? LAMP_RANGE_LEN : id == LAMP_CTRL ? 1 : 0;
    if (!want || !d) return;
    if (len == want + 1 && d[0] == id) { d++; len--; }   // tolerate a leading report ID
    note(len);
    if (id == LAMP_CTRL) { if (len >= 1) { g_auto = d[0] != 0; c_ctrl++; } else c_short++; return; }
    if (id == LAMP_RANGE) {
        c_range++;
        if (len < LAMP_RANGE_LEN) { c_short++; return; }
        uint16_t first = d[1] | (d[2] << 8), last = d[3] | (d[4] << 8);
        if (last >= lampTotal()) last = lampTotal() - 1;
        for (uint32_t k = first; k <= last; k++) store(k, d[5], d[6], d[7], d[8 - 0]);
        if (d[0] & 1) frameComplete();
        return;
    }
    c_multi++;                                            // LAMP_MULTI
    if (len < 2) { c_short++; return; }
    uint8_t n = d[0] > LAMP_MULTI_MAX ? LAMP_MULTI_MAX : d[0];
    for (uint8_t k = 0; k < n; k++) {
        if (2 + 2 * (k + 1) > len || 18 + 4 * (k + 1) > len) { c_short++; break; }
        const uint8_t* c = d + 18 + 4 * k;
        store(d[2 + 2 * k] | (d[3 + 2 * k] << 8), c[0], c[1], c[2], c[3]);
    }
    if (d[1] & 1) frameComplete();
}

void onSetFeature(uint8_t id, const uint8_t* d, uint16_t len) {
    if (id != HID::Reports::LAMP_REQ || !d) return;
    if (len == 3 && d[0] == id) { d++; len--; }
    if (len >= 2) { g_reqId = d[0] | (d[1] << 8); c_featSet++; }
}

uint16_t onGetFeature(uint8_t id, uint8_t* out, uint16_t maxLen) {
    uint8_t buf[40];
    uint8_t* p = buf;
    if (id == HID::Reports::LAMP_ATTR) {
        put16(p, lampTotal());
        put32(p, 120000); put32(p, 60000); put32(p, 1000);   // bounding box in micrometres
        put32(p, 4);                                         // LampArrayKind: peripheral
        put32(p, 1000);                                      // minimum update interval: 1 ms
    } else if (id == HID::Reports::LAMP_RESP) {
        uint16_t lid = g_reqId < lampTotal() ? g_reqId : 0;
        uint32_t x, y;
        uint32_t purposes;
        if (lid < BAR_LAMPS) { x = lid * 12000; y = 0; purposes = 0x02; }           // bar: accent, 12 mm pitch
        else {
            uint32_t k = lid - BAR_LAMPS, c = k % g_cols, r = k / g_cols;           // screen: 40.8 x 30.6 mm below the bar
            x = 33600 + (uint32_t)((2 * c + 1) * 20400ull / g_cols);
            y = 20000 + (uint32_t)((2 * r + 1) * 15300ull / g_rows);
            purposes = 0x20;                                                        // presentation
        }
        put16(p, lid); put32(p, x); put32(p, y); put32(p, 0);
        put32(p, purposes); put32(p, 1000);
        *p++ = 255; *p++ = 255; *p++ = 255; *p++ = 255; *p++ = 1; *p++ = 0;
    } else return 0;
    c_featGet++;
    uint16_t n = p - buf;
    if (n > maxLen) n = maxLen;
    memcpy(out, buf, n);
    return n;
}

CRGB barColor(uint8_t i) {
    if (!g_lamp || i >= BAR_LAMPS) return CRGB::Black;
    const Lamp& l = g_lamp[i];
    return CRGB(scale(l.r, l.i), scale(l.g, l.i), scale(l.b, l.i));
}

bool barChanged() { bool v = g_barReady; g_barReady = false; if (v) g_mainBarStamp = g_barStampUs; return v; }
void barShown() { if (g_mainBarStamp) { latAdd(g_latBar, esp_timer_get_time() - g_mainBarStamp); c_barShows++; g_mainBarStamp = 0; } }
void setBarSink(void (*fn)()) { g_sink = fn; }
void setFrameSink(void (*fn)()) { g_frameSink = fn; }
const uint8_t* raw(uint16_t id) { return g_lamp && id < lampTotal() ? (const uint8_t*)&g_lamp[id] : nullptr; }
void setWorker(bool on) { g_workerOn = on; }
bool workerOn() { return g_workerOn; }
bool pending() { return g_page != Page::None && c_frames != g_drawnFrames; }

void getLamp(uint16_t id, uint8_t rgbi[4]) {
    if (!g_lamp || id >= lampTotal()) { memset(rgbi, 0, 4); return; }
    memcpy(rgbi, &g_lamp[id], 4);
}

void resetCounters() {
    c_reports = c_multi = c_range = c_ctrl = c_short = c_bad = c_lampsSet = c_frames = c_bytes = c_featGet = c_featSet = 0;
    c_t0 = c_t1 = 0;
    g_drawn = g_drawnFrames = g_drawUs = 0;
    g_latBar = Lat(); g_latScr = Lat(); c_barShows = 0; g_pendingSinceUs = g_mainBarStamp = g_barStampUs = 0;
    g_auto = true;
    g_brightOverride = -1;
    if (g_lamp) { memset(g_lamp, 0, (size_t)lampTotal() * sizeof(Lamp)); memset(g_dirty, 0, ((size_t)lampTotal() + 7) / 8); }
    g_barTouched = false; g_barReady = true;     // the bar goes dark
    g_full = true;
}

void setBarBrightness(uint8_t level) { g_brightOverride = level < 1 ? 1 : (level > 120 ? 120 : level); }
uint8_t barBrightness() { return g_brightOverride >= 0 ? (uint8_t)g_brightOverride : Settings::ledBright(); }

String status() {
    char b[560];
    int64_t span = c_t1 > c_t0 ? c_t1 - c_t0 : 0;
    snprintf(b, sizeof(b),
             "LAMPS mode=%s lamps=%u grid=%ux%u max=%u reports=%lu multi=%lu range=%lu ctrl=%lu short=%lu bad=%lu lamps_set=%lu frames=%lu "
             "bytes=%lu featget=%lu featset=%lu span_us=%lld drawn=%lu draw_us=%lu bright=%u bar_shows=%lu bar_lat_us=%lu/%lu scr_lat_us=%lu/%lu worker=%d",
             g_auto ? "auto" : "host", (unsigned)lampTotal(), g_cols, g_rows, (unsigned)maxGridLamps(), (unsigned long)c_reports,
             (unsigned long)c_multi, (unsigned long)c_range, (unsigned long)c_ctrl, (unsigned long)c_short, (unsigned long)c_bad,
             (unsigned long)c_lampsSet, (unsigned long)c_frames, (unsigned long)c_bytes, (unsigned long)c_featGet,
             (unsigned long)c_featSet, (long long)span, (unsigned long)g_drawn, (unsigned long)g_drawUs, (unsigned)barBrightness(),
             (unsigned long)c_barShows, (unsigned long)(g_latBar.n ? g_latBar.sum / g_latBar.n : 0), (unsigned long)g_latBar.max,
             (unsigned long)(g_latScr.n ? g_latScr.sum / g_latScr.n : 0), (unsigned long)g_latScr.max, (int)g_workerOn);
    return String(b);
}

void enterPage(Page p, int x, int y, int w, int h) {
    g_page = p; g_ax = x; g_ay = y; g_aw = w; g_ah = h;
    g_lastStrip = 0;
    if (p == Page::Screen) fullScreenRedraw();
    else if (p == Page::Bar) { drawBarPage(); for (auto& v : g_barShown) v = 0xFFFFFFFF; g_drawnFrames = c_frames; }
    drawStrip(true);
}

void leavePage() { g_page = Page::None; freeFb(); }

void tick() {
    if (g_page == Page::None) return;
    uint32_t fr = c_frames;
    if (g_page == Page::Screen) {
        if (g_full) { fullScreenRedraw(); drawStrip(true); }
        else if (fr != g_drawnFrames && g_fb) {
            uint32_t t0 = micros();
            int64_t since = g_pendingSinceUs; g_pendingSinceUs = 0;
            g_drawnFrames = fr;
            g_dirtyMin = g_gh; g_dirtyMax = -1;
            uint32_t bytes = ((uint32_t)lampTotal() + 7) / 8;
            for (uint32_t i = 0; i < bytes; i++) {
                uint8_t v = __atomic_exchange_n(&g_dirty[i], (uint8_t)0, __ATOMIC_RELAXED);
                while (v) {
                    int bit = __builtin_ctz(v); v &= v - 1;
                    uint32_t id = i * 8 + bit;
                    if (id >= BAR_LAMPS && id < lampTotal()) drawCell(id - BAR_LAMPS);
                }
            }
            pushRows(std::max(0, g_dirtyMin), std::min(g_gh - 1, g_dirtyMax));
            g_drawUs = micros() - t0; g_drawn++;
            if (since) latAdd(g_latScr, esp_timer_get_time() - since);
        }
    } else if (fr != g_drawnFrames) {
        int64_t since = g_pendingSinceUs; g_pendingSinceUs = 0;
        g_drawnFrames = fr;
        drawBarPage();
        g_drawn++;
        if (since) latAdd(g_latScr, esp_timer_get_time() - since);
    }
    drawStrip(false);
}

} // namespace Lamps
