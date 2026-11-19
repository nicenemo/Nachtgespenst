#pragma once
#include <Arduino.h>

// Persistent settings (ADR 0014), stored in NVS under the namespace "ng". Defaults reproduce the behaviour of the
// firmware before settings existed, so an unconfigured board looks exactly as it did.
namespace Settings {
    enum class LedMode : uint8_t { Off = 0, Locks = 1, Layers = 2 };

    void begin();                                   // open NVS and load the stored values (call first in setup())

    LedMode ledMode();                              // Off: bar dark and its 5 V rail off; Locks: only host lock state; Layers: layer colours
    uint8_t ledBright();                            // FastLED global brightness, 1..120
    uint16_t ledIdleMin();                          // minutes without activity before the bar goes dark, 0 = never
    uint8_t backlight();                            // active screen brightness, 16..255
    uint16_t dimSec();                              // seconds without activity before the screen dims
    uint16_t offSec();                              // seconds without activity before the screen goes off
    uint8_t prefer();                               // HIDManager::Prefer as a number (0 USB, 1 BLE, 2 BOTH)

    // key: led (off|locks|on), bright, ledidle, bl, dim, off, prefer (usb|ble|both), defaults (any value).
    // Returns false for an unknown key or a value out of range; a valid value is stored at once.
    bool set(const char* key, const char* value);
    String summary();                               // "led=on bright=40 ledidle=0 bl=128 dim=60 off=300 prefer=usb"
}
