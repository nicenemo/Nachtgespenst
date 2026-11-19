#include "Disk.h"
#include <M5Unified.h>
// The microSD card is used on the CoreS3 only; on the original ESP32 (Core2) the SPI and SD drivers do not fit in its IRAM next to the rest.
#if !NG_HAS_BTNS
#define NG_DISK_SD 1
#include <SD.h>
#include <SPI.h>
#else
#define NG_DISK_SD 0
#endif
#include "Lamps.h"
#include "HIDManager.h"
#include "Bindings.h"
#include "HID_Shortcuts.h"
#include <esp_heap_caps.h>
#include <esp_timer.h>
#include <esp_crc.h>
#include <mbedtls/sha256.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <miniz.h>

// Block layout inside the lamps (little endian, lamp k holds bytes 4k..4k+3 as r, g, b, intensity):
//   0..1 sequence number   2 flags (1 last, 2 acknowledge wanted, 4 first)   3 session id (low byte)
//   4..5 payload length    6..7 reserved
//   8..  payload           then CRC-32 (IEEE) over everything before it
// Block 0 has the flag "first" and a header as payload: "NGD1", compressed size, original size, CRC-32 of the compressed data
// (u32 each), the method (0 stored, 1 raw deflate), the mode (0 show, 1 fast), the length of the file name and the name.
// Data blocks follow with sequence numbers 1, 2, ...
namespace Disk {
namespace {
constexpr uint8_t F_LAST = 1, F_ACK = 2, F_FIRST = 4, F_CREDIT = 8, F_REQ = 16, F_HASH = 32, F_RESEND = 64;
constexpr uint8_t GAMEPAD_ID = 11;   // HID::Reports::GAMEPAD
constexpr size_t HDR = 8, TRAILER = 4;

uint8_t* g_buf = nullptr;                 // compressed bytes
char* g_text = nullptr;                   // decoded text (PSRAM)
char g_excerpt[640];
char g_name[32] = "pg164.txt";

volatile State g_state = State::Idle;
volatile bool g_active = false;
uint8_t g_session = 0;
uint16_t g_expected = 0;
uint32_t g_total = 0, g_original = 0, g_wholeCrc = 0, g_got = 0;
uint8_t g_method = 1;
volatile bool g_fast = false;
volatile uint32_t c_ok = 0, c_bad = 0, c_dup = 0;
volatile uint16_t g_lastBytes = 0;
int64_t g_t0 = 0, g_t1 = 0;
uint32_t g_lines = 0;
bool g_crcOk = false, g_saved = false;

// One pending key for the main loop; a newer ack replaces an older one (the ack is cumulative), a NAK is only queued when no ack waits.
volatile int8_t g_pending = -1;           // index into KEYS
volatile bool g_decode = false;

const Binding KEYS[] = {kp_F13, kp_F14, kp_F15, kp_F16, kp_F17, kp_F18, kp_F19, kp_F20, kp_F21, kp_F22, kp_F23, kp_F24};
constexpr int K_NAK = 8, K_DONE_OK = 10, K_DONE_BAD = 11;

// ---- hashes and the way back (LOAD) ----
uint8_t g_sha[32], g_pcSha[32];
volatile uint32_t g_shaUs = 0, g_txReports = 0;      // time of the last SHA-256 of the document, reports sent back
bool g_haveSha = false, g_havePc = false;
volatile int8_t g_match = -1;
uint8_t g_pcWhich = 0; uint32_t g_pcRate = 0;
volatile bool g_loadReq = false;
char g_loadName[32];

struct TxState {
    volatile Tx phase = Tx::Idle;
    volatile uint32_t total = 0, off = 0, credit = 0, kbps10 = 0, tReq = 0, tLastCredit = 0, tStart = 0;
    volatile uint8_t payload = 32, window = 32;
    volatile uint16_t gap10 = 0;
    volatile bool gotCredit = false;
} tx;
TaskHandle_t g_txTask = nullptr;

void sha256sum(const uint8_t* d, size_t n, uint8_t out[32]) { uint32_t t = micros(); mbedtls_sha256(d, n, out, 0); g_shaUs = micros() - t; }
void toHex(const uint8_t* b, char out[65]) { static const char* H = "0123456789abcdef"; for (int i = 0; i < 32; i++) { out[2 * i] = H[b[i] >> 4]; out[2 * i + 1] = H[b[i] & 15]; } out[64] = 0; }

// ---- sound: events from the HID task, played from the main loop ----
bool g_sound = true;
volatile uint32_t e_blocks = 0, e_start = 0, e_err = 0, e_crc = 0;
uint32_t s_blocks = 0, s_start = 0, s_err = 0, s_lastTone = 0;
struct Step { uint16_t hz, ms; };
Step g_seq[16]; int g_seqLen = 0, g_seqPos = 0; uint32_t g_seqNext = 0;

void playSeq(const Step* s, int n) { memcpy(g_seq, s, n * sizeof(Step)); g_seqLen = n; g_seqPos = 0; g_seqNext = millis(); }

// ---- SD card ----
bool g_sdOk = false;

inline uint32_t u32(const uint8_t* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24); }
inline uint16_t u16(const uint8_t* p) { return p[0] | (p[1] << 8); }

void startSession(uint8_t sid) {
    g_session = sid; g_expected = 0; g_got = 0; g_total = g_original = g_wholeCrc = 0;
    c_ok = c_bad = c_dup = 0; g_lastBytes = 0; g_lines = 0; g_crcOk = false; g_saved = false;
    g_t0 = esp_timer_get_time(); g_t1 = g_t0;
    g_excerpt[0] = 0;
    g_state = State::Receiving;
}

void queueKey(int k, bool replace) { if (replace || g_pending < 0) g_pending = (int8_t)k; }
void nak() { c_bad++; e_err++; queueKey(K_NAK, false); }

// File names on the card: letters, digits, dot, dash and underscore only.
void cleanName(const uint8_t* n, size_t len) {
    size_t o = 0;
    for (size_t i = 0; i < len && o < sizeof(g_name) - 1; i++) {
        char c = (char)n[i];
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '.' || c == '-' || c == '_') g_name[o++] = c;
    }
    g_name[o] = 0;
    if (o == 0 || g_name[0] == '.') strcpy(g_name, "file.txt");
}

bool saveToSd() {
#if NG_DISK_SD
    if (!sdMount() || !g_text) return false;
    String path = String("/") + g_name;
    SD.remove(path);
    File f = SD.open(path, FILE_WRITE);
    if (!f) return false;
    size_t n = f.write((const uint8_t*)g_text, g_original);
    f.close();
    File r = SD.open(path, FILE_READ);
    size_t size = r ? r.size() : 0;
    if (r) r.close();
    return n == g_original && size == g_original;
#else
    return false;
#endif
}

// One game pad report: 48 data bytes = offset u32, flags, length, up to 38 payload bytes, CRC-32 of the first 44 bytes.
void makeReport(uint8_t* r, uint32_t off, uint8_t flags, const uint8_t* payload, uint8_t len) {
    memset(r, 0, 48);
    r[0] = off; r[1] = off >> 8; r[2] = off >> 16; r[3] = off >> 24; r[4] = flags; r[5] = len;
    if (len) memcpy(r + 6, payload, len);
    uint32_t c = esp_crc32_le(0, r, 44);
    r[44] = c; r[45] = c >> 8; r[46] = c >> 16; r[47] = c >> 24;
}

// Runs on the other core: sends the file back as game pad reports, as far as the credits from the host allow (flow control through
// the lamps). The header report (size and SHA-256) repeats until the first credit arrives; then data follows within the window.
void txTask(void*) {
    uint8_t rep[48]; uint32_t n = 0, lastHeader = 0;
    for (;;) {
        if (tx.phase == Tx::Idle || tx.phase == Tx::DoneOk || tx.phase == Tx::Failed) { vTaskDelay(pdMS_TO_TICKS(5)); continue; }
        uint32_t now = millis();
        if (tx.phase == Tx::Header) {
            if (tx.gotCredit) { tx.phase = Tx::Data; tx.tStart = now; tx.tLastCredit = now; continue; }
            if (now - tx.tReq > 10000) { tx.phase = Tx::Failed; continue; }
            if (now - lastHeader >= 60) {
                uint8_t pl[36]; uint32_t t = tx.total; pl[0] = t; pl[1] = t >> 8; pl[2] = t >> 16; pl[3] = t >> 24; memcpy(pl + 4, g_sha, 32);
                makeReport(rep, 0, 2, pl, 36);
                HIDManager::sendRaw(GAMEPAD_ID, rep, 48); lastHeader = now;
            }
            vTaskDelay(1); continue;
        }
        // Data phase
        if (tx.credit >= tx.total) {                                   // the host has everything
            uint32_t ms = now - tx.tStart; if (ms == 0) ms = 1;
            tx.kbps10 = (uint32_t)((uint64_t)tx.total * 10000 / 1024 / ms);
            tx.phase = Tx::DoneOk; continue;
        }
        if (now - tx.tLastCredit > 600) {
            if (now - tx.tLastCredit > 12000) { tx.phase = Tx::Failed; continue; }
            if (tx.off > tx.credit) tx.off = tx.credit;                  // no news from the host: go back to the last credit
            tx.tLastCredit = now - 400;                                  // and try again in 200 ms
        }
        uint32_t win = (uint32_t)tx.window * tx.payload, off = tx.off;
        if (off < tx.total && off < tx.credit + win) {
            uint32_t len = tx.total - off; if (len > tx.payload) len = tx.payload;
            makeReport(rep, off, off + len >= tx.total ? 1 : 0, (const uint8_t*)g_text + off, (uint8_t)len);
            if (HIDManager::sendRaw(GAMEPAD_ID, rep, 48)) {
                if (tx.off == off) tx.off = off + len;                   // (a resend request may have moved it meanwhile)
                g_txReports++;
                e_crc = rep[44]; if ((++n & 31) == 0) e_blocks++;
                if (tx.gap10) delayMicroseconds((uint32_t)tx.gap10 * 10);
                if ((n & 15) == 0) vTaskDelay(1);
            } else vTaskDelay(1);
        } else vTaskDelay(1);
    }
}

// The main loop prepares a LOAD: the file is in RAM from the SAVE, or is read from the SD card; its SHA-256 is computed here.
bool prepareLoad() {
    if (!g_text || strcmp(g_loadName, g_name) != 0) {
#if !NG_DISK_SD
        return false;
#else
        if (!sdMount()) return false;
        File f = SD.open(String("/") + g_loadName, FILE_READ);
        if (!f || f.isDirectory()) return false;
        size_t n = f.size();
        if (n == 0 || n > 4 * 1024 * 1024) { f.close(); return false; }
        char* buf = (char*)heap_caps_malloc(n + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buf) { f.close(); return false; }
        size_t got = f.read((uint8_t*)buf, n); f.close();
        if (got != n) { heap_caps_free(buf); return false; }
        buf[n] = 0;
        if (g_text) heap_caps_free(g_text);
        g_text = buf; g_original = n; strncpy(g_name, g_loadName, sizeof(g_name) - 1);
#endif
    }
    sha256sum((const uint8_t*)g_text, g_original, g_sha); g_haveSha = true;
    g_match = g_havePc ? (memcmp(g_sha, g_pcSha, 32) == 0) : -1;
    return true;
}
}  // namespace

#if NG_DISK_SD
bool sdMount() {
    if (g_sdOk) return true;
    SPI.begin(36, 35, 37, 4);                  // CoreS3: microSD on the display SPI bus, chip select GPIO4
    g_sdOk = SD.begin(4, SPI, 25000000);
    if (g_sdOk && SD.cardType() == CARD_NONE) { SD.end(); g_sdOk = false; }
    return g_sdOk;
}

String sdStatus() {
    if (!sdMount()) return String("SD present=0");
    const char* t = SD.cardType() == CARD_MMC ? "MMC" : SD.cardType() == CARD_SD ? "SD" : SD.cardType() == CARD_SDHC ? "SDHC" : "unknown";
    char b[120];
    snprintf(b, sizeof(b), "SD present=1 type=%s size_mb=%llu used_mb=%llu", t, SD.cardSize() / (1024 * 1024), SD.usedBytes() / (1024 * 1024));
    return String(b);
}

String dir() {
    String out;
    if (!sdMount()) return out;
    File root = SD.open("/");
    if (!root) return out;
    int n = 0;
    for (File f = root.openNextFile(); f && n < 8; f = root.openNextFile(), n++) {
        char line[80];
        snprintf(line, sizeof(line), "%s%s  %lu\n", f.name(), f.isDirectory() ? "/" : "", (unsigned long)f.size());
        out += line;
    }
    root.close();
    return out;
}
#else
bool sdMount() { return false; }
String sdStatus() { return String("SD present=0 (not used on this board)"); }
String dir() { return String(); }
#endif

void begin() {
    g_buf = (uint8_t*)heap_caps_malloc(MAX_FILE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!g_buf) g_buf = (uint8_t*)heap_caps_malloc(64 * 1024, MALLOC_CAP_8BIT);
    xTaskCreatePinnedToCore(txTask, "disk-tx", 4096, nullptr, 2, &g_txTask, ARDUINO_RUNNING_CORE == 0 ? 1 : 0);
    Serial.printf("[Disk] receive buffer %s\n", g_buf ? "ok" : "FAILED");
}

void reset() {
    g_state = State::Idle; g_expected = 0; g_got = 0; g_total = g_original = 0; c_ok = c_bad = c_dup = 0;
    g_lastBytes = 0; g_lines = 0; g_crcOk = false; g_saved = false; g_fast = false; g_pending = -1; g_decode = false; g_excerpt[0] = 0;
    g_haveSha = g_havePc = false; g_match = -1; tx.phase = Tx::Idle; tx.kbps10 = 0; g_loadReq = false;
    if (g_text) { heap_caps_free(g_text); g_text = nullptr; }
}

void enter() {
    if (!g_buf) return;
    Lamps::setGrid(32, 32);                       // 1,024 lamps behind the ten bar lamps hold the largest block
    Lamps::setFrameSink(onFrame);
    s_blocks = e_blocks; s_start = e_start; s_err = e_err;
    sdMount();
    if (!M5.Speaker.isEnabled()) M5.Speaker.begin();                       // the microphone effect may have taken the I2S port
    M5.Speaker.setVolume(96);
    g_active = true;
}

void leave() {
    g_active = false;
    Lamps::setFrameSink(nullptr);
    M5.Speaker.stop();
    g_seqLen = 0;
}

bool active() { return g_active; }
bool quiet() { return g_fast && (g_state == State::Receiving || tx.phase == Tx::Header || tx.phase == Tx::Data); }
void setSound(bool on) { g_sound = on; }

// HID task: runs for every completed lamp frame while the page is shown. Reads the block straight from the lamp store.
void onFrame() {
    if (!g_active || !g_buf) return;
    const uint8_t* b = Lamps::raw(DATA_FIRST_LAMP);
    if (!b) return;
    uint16_t seq = u16(b); uint8_t flags = b[2], sid = b[3];
    size_t len = u16(b + 4);
    if (len + HDR + TRAILER > (size_t)MAX_BLOCK_LAMPS * 4) { nak(); return; }
    uint32_t crc = esp_crc32_le(0, b, HDR + len);
    if (crc != u32(b + HDR + len)) { nak(); return; }
    if (flags & (F_REQ | F_CREDIT | F_HASH)) {                // the host talking back: a request, flow control, or its hash
        if ((flags & F_HASH) && len >= 37) {
            memcpy(g_pcSha, b + HDR, 32); g_pcRate = u32(b + HDR + 32); g_pcWhich = b[HDR + 36]; g_havePc = true;
            g_match = g_haveSha ? (memcmp(g_sha, g_pcSha, 32) == 0) : 0;
        } else if ((flags & F_CREDIT) && len >= 6 && tx.phase != Tx::Idle) {
            uint32_t wanted = u32(b + HDR);
            uint8_t pl = b[6] < 4 ? 4 : (b[6] > 38 ? 38 : b[6]);
            tx.payload = pl; tx.window = b[7] ? b[7] : 1; tx.gap10 = u16(b + HDR + 4);
            if (flags & F_RESEND) tx.off = wanted;
            tx.credit = wanted; tx.tLastCredit = millis(); tx.gotCredit = true;
        } else if ((flags & F_REQ) && len > 0 && len < sizeof(g_loadName)) {
            memcpy(g_loadName, b + HDR, len); g_loadName[len] = 0;
            tx.payload = b[6] < 4 ? 4 : (b[6] > 38 ? 38 : b[6]); tx.window = b[7] ? b[7] : 1; tx.gap10 = 0;
            tx.phase = Tx::Idle; tx.gotCredit = false; g_loadReq = true; e_start++;
        } else { nak(); }
        return;
    }
    g_lastBytes = (uint16_t)len;

    if (flags & F_FIRST) {
        if (len < 19 || memcmp(b + HDR, "NGD1", 4) || (size_t)19 + b[HDR + 18] > len) { nak(); return; }
        if (g_state != State::Receiving || sid != g_session || g_expected > 1) startSession(sid);   // a repeat of block 0 keeps the session
        if (g_expected == 0) {
            g_total = u32(b + HDR + 4); g_original = u32(b + HDR + 8); g_wholeCrc = u32(b + HDR + 12); g_method = b[HDR + 16];
            g_fast = b[HDR + 17] == 1;
            cleanName(b + HDR + 19, b[HDR + 18]);
            if (g_total > MAX_FILE) { c_bad++; g_state = State::DoneBad; queueKey(K_DONE_BAD, true); return; }
            g_expected = 1; c_ok++; e_start++;
        } else c_dup++;
        if (flags & F_ACK) queueKey((g_expected - 1) & 7, true);
        return;
    }
    if ((g_state == State::DoneOk || g_state == State::DoneBad) && sid == g_session) {   // the sender missed the end of the session: say it again
        queueKey(g_state == State::DoneOk ? K_DONE_OK : K_DONE_BAD, true); return;
    }
    if (g_state != State::Receiving || sid != g_session || len < 4) { nak(); return; }
    // A data block starts with the byte offset of its data in the file, so a resend may cut the file into other blocks than before.
    uint32_t off = u32(b + HDR); size_t dlen = len - 4;
    if (off + dlen <= g_got) {                           // everything in this block is already stored (the sender missed an acknowledgement)
        c_dup++;
        if (flags & F_ACK) queueKey(seq & 7, true);
        return;
    }
    if (off > g_got) { nak(); return; }                  // a block went missing: ask for a resend from the last acknowledged one
    size_t skip = g_got - off, take = dlen - skip;       // the part of this block that is new
    if (g_got + take > g_total) { nak(); return; }
    memcpy(g_buf + g_got, b + HDR + 4 + skip, take); g_got += take; g_expected++; c_ok++;
    g_t1 = esp_timer_get_time();
    e_crc = crc; e_blocks++;
    if ((flags & F_LAST) || g_got == g_total) {
        g_state = State::Verifying; g_decode = true;     // the main loop unpacks and saves, then reports with F23 or F24
        return;
    }
    if (flags & F_ACK) queueKey(seq & 7, true);
}

static void sounds() {
    if (!g_sound || g_fast || !M5.Speaker.isEnabled()) { s_blocks = e_blocks; s_start = e_start; s_err = e_err; return; }
    uint32_t now = millis();
    if (s_start != e_start) {                              // the handshake of a modem: answer tone, two carriers, then a rising squeal
        s_start = e_start;
        static const Step hello[] = {{2100, 260}, {0, 40}, {1300, 90}, {2250, 90}, {980, 70}, {1850, 70},
                                     {600, 30}, {900, 30}, {1200, 30}, {1500, 30}, {1800, 30}, {2100, 30}, {2400, 30}, {0, 40}};
        playSeq(hello, sizeof(hello) / sizeof(hello[0]));
    }
    if (g_seqLen) {
        if (now >= g_seqNext) {
            if (g_seqPos >= g_seqLen) g_seqLen = 0;
            else { const Step& s = g_seq[g_seqPos++]; if (s.hz) M5.Speaker.tone(s.hz, s.ms); g_seqNext = now + s.ms; }
        }
        s_blocks = e_blocks; s_err = e_err;
        return;
    }
    if (s_err != e_err) { s_err = e_err; M5.Speaker.tone(140, 90); s_lastTone = now; return; }
    if (s_blocks != e_blocks && now - s_lastTone >= 28) {   // the data: one warble per block, its pitch from the block's CRC
        s_blocks = e_blocks; s_lastTone = now;
        M5.Speaker.tone(700 + (e_crc & 0x3F) * 30, 24);
    }
}

void loop() {
    sounds();
    if (g_loadReq) {
        g_loadReq = false;
        if (prepareLoad()) {
            g_txReports = 0; tx.total = g_original; tx.off = 0; tx.credit = 0; tx.kbps10 = 0; tx.gotCredit = false; tx.tReq = millis();
            tx.phase = Tx::Header;
        } else tx.phase = Tx::Failed;
    }
    static Tx lastTx = Tx::Idle;
    if (tx.phase != lastTx) {                                // the end of a LOAD: a chime or a buzz
        lastTx = tx.phase;
        if ((lastTx == Tx::DoneOk || lastTx == Tx::Failed) && g_sound && !g_fast && M5.Speaker.isEnabled()) {
            static const Step good[] = {{880, 110}, {1320, 110}, {1760, 220}};
            static const Step bad[] = {{440, 160}, {330, 160}, {220, 320}};
            playSeq(lastTx == Tx::DoneOk ? good : bad, 3);
        }
    }
    if (g_decode) {
        g_decode = false;
        bool ok = g_got == g_total && esp_crc32_le(0, g_buf, g_got) == g_wholeCrc;
        g_crcOk = ok;
        if (ok && g_original > 0 && g_original <= 4 * 1024 * 1024) {
            g_text = (char*)heap_caps_malloc(g_original + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
            if (!g_text) g_text = (char*)heap_caps_malloc(g_original + 1, MALLOC_CAP_8BIT);
            if (g_text) {
                if (g_method == 0) { size_t n = g_got < g_original ? g_got : g_original; memcpy(g_text, g_buf, n); g_text[n] = 0; ok = n == g_original; }
                else {
                    static tinfl_decompressor d;                     // about 11 kB: not on the 8 kB stack of the loop task
                    tinfl_init(&d);
                    size_t inLen = g_got, outLen = g_original;
                    tinfl_status st = tinfl_decompress(&d, g_buf, &inLen, (uint8_t*)g_text, (uint8_t*)g_text, &outLen, TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
                    ok = st == TINFL_STATUS_DONE && outLen == g_original;
                    g_text[ok ? outLen : 0] = 0;
                }
            } else ok = false;
        } else ok = false;
        if (ok) {
            for (const char* p = g_text; *p; p++) if (*p == '\n') g_lines++;
            // The first lines after the licence marker; plain ASCII for the small LVGL fonts.
            const char* s = g_text;
            if (!strncmp(s, "***", 3)) { while (*s && *s != '\n') s++; }
            while (*s == '\n' || *s == '\r' || *s == ' ') s++;
            size_t n = 0;
            for (; *s && n < sizeof(g_excerpt) - 1; s++) {
                unsigned char c = (unsigned char)*s;
                if (c >= 0x80 && c < 0xC0) continue;               // continuation byte of a UTF-8 character
                g_excerpt[n++] = c >= 0xC0 ? '\'' : (c == '\r' ? ' ' : (char)c);
            }
            g_excerpt[n] = 0;
            sha256sum((const uint8_t*)g_text, g_original, g_sha); g_haveSha = true;
            g_match = g_havePc ? (memcmp(g_sha, g_pcSha, 32) == 0) : -1;
            if (sdMount()) g_saved = saveToSd();                  // no card: the file stays in RAM (and on screen)
            ok = !g_sdOk || g_saved;
        }
        g_state = ok ? State::DoneOk : State::DoneBad;
        queueKey(ok ? K_DONE_OK : K_DONE_BAD, true);
        if (g_sound && !g_fast && M5.Speaker.isEnabled()) {
            static const Step good[] = {{880, 110}, {1320, 110}, {1760, 220}};
            static const Step bad[] = {{440, 160}, {330, 160}, {220, 320}};
            playSeq(ok ? good : bad, 3);
        }
    }
    int8_t k = g_pending;
    if (k >= 0 && HIDManager::canSend(KEYS[k])) {
        g_pending = -1;
        HIDManager::send(KEYS[k]);
    }
}

View view() {
    View v{};
    v.state = g_state; v.blocksOk = c_ok; v.blocksBad = c_bad; v.blocksDup = c_dup; v.bytes = g_got; v.total = g_total; v.original = g_original;
    int64_t us = g_t1 - g_t0;
    v.kbps10 = us > 0 ? (uint32_t)((int64_t)g_got * 10 * 1000000 / 1024 / us) : 0;
    v.lastBlockBytes = g_lastBytes; v.lines = g_lines; v.crcOk = g_crcOk; v.fast = g_fast; v.saved = g_saved;
    v.sdPresent = g_sdOk;
#if NG_DISK_SD
    if (g_sdOk) { v.sdMB = (uint32_t)(SD.cardSize() / (1024 * 1024)); v.sdUsedMB = (uint32_t)(SD.usedBytes() / (1024 * 1024)); }
#endif
    strncpy(v.name, g_name, sizeof(v.name) - 1);
    v.tx = tx.phase; v.txBytes = tx.off < tx.credit ? tx.credit : tx.off; v.txTotal = tx.total; v.txKbps10 = tx.kbps10;
    if (tx.phase == Tx::Data && v.txBytes > tx.total) v.txBytes = tx.total;
    if (g_haveSha) toHex(g_sha, v.sha); else v.sha[0] = 0;
    if (g_havePc) toHex(g_pcSha, v.pcSha); else v.pcSha[0] = 0;
    v.match = g_match; v.pcWhich = g_pcWhich; v.pcRate = g_pcRate; v.shaUs = g_shaUs;
    return v;
}

const char* excerpt() { return g_excerpt; }

String status() {
    static const char* const names[] = {"idle", "receiving", "verifying", "done", "failed"};
    View v = view();
    static const char* const txn[] = {"idle", "header", "data", "done", "failed"};
    char b[640];
    snprintf(b, sizeof(b), "DISK state=%s ok=%lu bad=%lu dup=%lu bytes=%lu total=%lu original=%lu kbps10=%lu block=%u lines=%lu crc=%d fast=%d saved=%d sd=%d name=%s active=%d tx=%s txbytes=%lu txkbps10=%lu match=%d pcrate=%lu shaus=%lu txreports=%lu sha=%s pcsha=%s",
             names[(int)v.state], (unsigned long)v.blocksOk, (unsigned long)v.blocksBad, (unsigned long)v.blocksDup, (unsigned long)v.bytes,
             (unsigned long)v.total, (unsigned long)v.original, (unsigned long)v.kbps10, v.lastBlockBytes, (unsigned long)v.lines, (int)v.crcOk,
             (int)v.fast, (int)v.saved, (int)v.sdPresent, v.name, (int)g_active, txn[(int)v.tx], (unsigned long)v.txBytes,
             (unsigned long)v.txKbps10, (int)v.match, (unsigned long)v.pcRate, (unsigned long)g_shaUs, (unsigned long)g_txReports, v.sha[0] ? v.sha : "-", v.pcSha[0] ? v.pcSha : "-");
    return String(b);
}
}  // namespace Disk
