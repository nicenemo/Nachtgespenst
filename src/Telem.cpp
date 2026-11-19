#include "Telem.h"
#include "BarFx.h"
#include "HIDManager.h"
#include "ui.h"
#include <esp_crc.h>

namespace Telem {
namespace {
constexpr uint8_t GAMEPAD_ID = 11;
volatile uint8_t g_mode = Off;
uint32_t g_seq = 0, g_lastBar = 0, g_lastBuffers = 0;

void makeReport(uint8_t* r, uint32_t off, uint8_t flags, const uint8_t* payload, uint8_t len) {
    memset(r, 0, 48);
    r[0] = off; r[1] = off >> 8; r[2] = off >> 16; r[3] = off >> 24; r[4] = flags; r[5] = len;
    if (len) memcpy(r + 6, payload, len);
    uint32_t c = esp_crc32_le(0, r, 44);
    r[44] = c; r[45] = c >> 8; r[46] = c >> 16; r[47] = c >> 24;
}
}  // namespace

void set(uint8_t m) { g_mode = m & 3; g_lastBuffers = BarFx::micStats().buffers; }
uint8_t mode() { return g_mode; }
bool wantsMic() { return g_mode & Wave; }

void loop() {
    uint8_t m = g_mode;
    if (!m) return;
    uint8_t rep[48];
    uint32_t now = millis();
    if ((m & Bar) && now - g_lastBar >= 40) {                        // the bar, 25 times a second
        g_lastBar = now;
        uint32_t c[10]; ui_bar_colors(c);
        uint8_t pl[30]; for (int i = 0; i < 10; i++) { pl[3 * i] = c[i] >> 16; pl[3 * i + 1] = c[i] >> 8; pl[3 * i + 2] = c[i]; }
        makeReport(rep, g_seq++, 0x04, pl, 30);
        HIDManager::sendRaw(GAMEPAD_ID, rep, 48);
    }
    if (m & Wave) {                                                  // a new microphone buffer: 514 bytes in 14 reports
        BarFx::MicStats st = BarFx::micStats();
        if (st.buffers == g_lastBuffers) return;
        g_lastBuffers = st.buffers;
        static int16_t w[BarFx::WAVE_SAMPLES];
        BarFx::micWave(w);
        int peak = 1; for (int i = 0; i < BarFx::WAVE_SAMPLES; i++) { int a = abs(w[i]); if (a > peak) peak = a; }
        uint16_t div = (uint16_t)((peak + 126) / 127); if (div < 1) div = 1;
        static uint8_t frame[2 + BarFx::WAVE_SAMPLES];
        frame[0] = div; frame[1] = div >> 8;
        for (int i = 0; i < BarFx::WAVE_SAMPLES; i++) { int v = w[i] / div; frame[2 + i] = (uint8_t)(int8_t)(v < -127 ? -127 : (v > 127 ? 127 : v)); }
        const int total = sizeof(frame);
        for (int off = 0; off < total; off += 38) {
            int len = total - off < 38 ? total - off : 38;
            makeReport(rep, off, 0x08 | (off + len >= total ? 0x01 : 0), frame + off, (uint8_t)len);
            HIDManager::sendRaw(GAMEPAD_ID, rep, 48);
        }
    }
}
}  // namespace Telem
