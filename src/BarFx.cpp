#include "BarFx.h"
#include <M5Unified.h>
#include <math.h>

namespace BarFx {
namespace {
CRGB scaled(CRGB c, float f) { f = f < 0 ? 0 : (f > 1 ? 1 : f); return CRGB((uint8_t)(c.r * f), (uint8_t)(c.g * f), (uint8_t)(c.b * f)); }

// ---- microphone ----
constexpr size_t N = 512;                 // 32 ms at 16 kHz
constexpr uint32_t RATE = 16000;
int16_t g_buf[N];
bool g_on = false, g_have = false;
volatile uint16_t g_level = 0, g_peak = 0, g_freq = 0;
int16_t g_wave[WAVE_SAMPLES];
volatile uint32_t g_buffers = 0;
float g_smooth = 0;
}  // namespace

volatile bool g_cmd = false;
void micCommand(bool on) { g_cmd = on; }
bool micCommandOn() { return g_cmd; }

void micWanted(bool on) {
    if (on == g_on) return;
    if (on) {
        M5.Speaker.end();                                  // the speaker and the microphone share the I2S port on the Core2
        auto cfg = M5.Mic.config(); cfg.sample_rate = RATE; cfg.noise_filter_level = 0; M5.Mic.config(cfg);
        g_on = M5.Mic.begin(); g_have = false; g_level = g_peak = g_freq = 0;
    } else {
        M5.Mic.end(); g_on = false; g_level = g_peak = g_freq = 0;
        M5.Speaker.begin();
    }
}

void micLoop() {
    if (!g_on || !M5.Mic.isEnabled()) return;
    if (M5.Mic.isRecording()) return;                      // a buffer is being filled
    if (g_have) {                                          // analyse the buffer that is complete
        int64_t sum = 0; int32_t mean = 0; int peak = 0; int crossings = 0;
        for (size_t i = 0; i < N; i++) mean += g_buf[i];
        mean /= (int32_t)N;
        int prev = g_buf[0] - mean;
        for (size_t i = 0; i < N; i++) {
            int v = g_buf[i] - mean; sum += (int64_t)v * v; if (abs(v) > peak) peak = abs(v);
            if (i && ((v >= 0) != (prev >= 0))) crossings++;
            prev = v;
        }
        g_level = (uint16_t)sqrt((double)sum / N); g_peak = (uint16_t)peak;
        g_freq = (uint16_t)((uint64_t)crossings * RATE / (2 * N));       // two crossings per period
        for (size_t i = 0; i < N; i++) g_wave[i] = (int16_t)(g_buf[i] - mean);
        g_buffers++;
    }
    M5.Mic.record(g_buf, N, RATE); g_have = true;
}

void micWave(int16_t out[WAVE_SAMPLES]) { memcpy(out, g_wave, sizeof(g_wave)); }

MicStats micStats() { return {g_on, g_level, g_peak, g_freq, g_buffers}; }

String micStatus() {
    MicStats s = micStats(); char b[100];
    snprintf(b, sizeof(b), "MIC active=%d level=%u peak=%u freq=%u buffers=%lu", (int)s.active, s.level, s.peak, s.freq, (unsigned long)s.buffers);
    return String(b);
}

void render(Settings::Effect e, CRGB* leds, uint32_t ms, CRGB base) {
    float t = ms / 1000.0f;
    for (int i = 0; i < 10; i++) leds[i] = CRGB::Black;
    switch (e) {
        case Settings::Effect::Nightrider: {                // a scanner up and down both sides
            float p = 2.0f + 2.0f * sinf(t * 3.2f);
            for (int r = 0; r < 5; r++) { CRGB c = scaled(base, expf(-fabsf(r - p) * 1.1f)); leds[rightOf(r)] = c; leds[leftOf(r)] = c; }
            break; }
        case Settings::Effect::Breath: {
            float f = (sinf(t * 1.6f - 1.5708f) + 1) / 2;
            CRGB c = scaled(base, 0.05f + 0.95f * f * f);
            for (int i = 0; i < 10; i++) leds[i] = c;
            break; }
        case Settings::Effect::Rainbow:                     // down both sides
            for (int r = 0; r < 5; r++) { CRGB c; c.setHSV((uint8_t)((r * 51) - (int)(t * 76)), 255, 255); leds[rightOf(r)] = c; leds[leftOf(r)] = c; }
            break;
        case Settings::Effect::Comet: {                     // along the chain: down the right side, round the bottom, up the left side
            float p = fmodf(t * 7.0f, 14.0f) - 2.0f;
            for (int i = 0; i < 10; i++) { float d = p - i; leds[i] = d >= 0 ? scaled(base, expf(-d * 0.7f)) : CRGB::Black; }
            break; }
        case Settings::Effect::Sparkle: {
            static float v[10]; static uint32_t last = 0;
            if (ms - last >= 16) { last = ms; for (int i = 0; i < 10; i++) { v[i] *= 0.9f; if (random(14) == 0) v[i] = 1; } }
            for (int i = 0; i < 10; i++) leds[i] = scaled(base, v[i]);
            break; }
        case Settings::Effect::Vu: {                        // the microphone level, rows lit from the bottom on both sides
            MicStats s = micStats();
            float db = s.level > 0 ? 20.0f * log10f(s.level / 32768.0f) : -90.0f;          // -60 dB (quiet room) .. -20 dB (loud)
            float x = (db + 60.0f) / 40.0f * 5.0f; if (x < 0) x = 0; if (x > 5) x = 5;
            g_smooth = x > g_smooth ? x : g_smooth * 0.85f + x * 0.15f;                      // quick attack, slow release
            for (int r = 0; r < 5; r++) {
                float lit = g_smooth - (4 - r); lit = lit < 0 ? 0 : (lit > 1 ? 1 : lit);       // row 4 (bottom) first
                CRGB c = r == 0 ? CRGB(255, 0, 0) : (r == 1 ? CRGB(255, 160, 0) : CRGB(0, 255, 40));
                leds[rightOf(r)] = scaled(c, lit); leds[leftOf(r)] = scaled(c, lit);
            }
            break; }
        default: for (int i = 0; i < 10; i++) leds[i] = base; break;
    }
}
}  // namespace BarFx
