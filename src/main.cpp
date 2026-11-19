#include <M5Unified.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "Lamps.h"
#include "ui.h"
#include "SerialCmd.h"

void setup() {
    auto cfg = M5.config();
    cfg.internal_spk = false; // no beeps: speaker/amp never initialised
    cfg.internal_mic = false;
    M5.begin(cfg);
    Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
    for (uint32_t t = millis(); !Serial && millis() - t < 1500;) yield(); // wait for CDC, bounded
#endif
    Serial.printf("NACHTGESPENST board=%s name=%s fw=%s %s\n", NG_BOARD_ID, K_DEVICE_NAME, __DATE__, __TIME__);

    Lamps::begin();   // before HIDManager: BLE and USB handlers may deliver reports as soon as the host connects
    HIDManager::begin(K_BLE_NAME, K_MANUFACTURER);   // the Bluetooth name carries the emoji; USB strings stay ASCII

    ui_begin();
}

void loop() {
    M5.update();
    serialcmd_poll();
#if NG_DEBUG_HEARTBEAT
    static uint32_t hb = 0;
    if (millis() - hb >= 1000) { hb = millis(); Serial.printf("[hb] t=%lu heap=%u ble=%d\n", (unsigned long)millis(), (unsigned)ESP.getFreeHeap(), (int)HIDManager::isBleConnected()); }
#endif

    HIDManager::handleAsyncEvents();

#if NG_HAS_BTNS
    // Core2 capacitive buttons below the screen: A = previous page, B = Locks page <-> back, C = next page
    if (M5.BtnA.wasPressed()) ui_set_layer(ui_layer() - 1);
    if (M5.BtnB.wasPressed()) ui_toggle_locks();
    if (M5.BtnC.wasPressed()) ui_set_layer(ui_layer() + 1);
#endif
    delay(ui_loop()); // sleep until LVGL's next job (1..10 ms) instead of spinning
}
