#include <M5Unified.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "Lamps.h"
#include "Disk.h"
#include "Settings.h"
#include "ui.h"
#include "Presenter.h"
#include "SerialCmd.h"

void setup() {
    auto cfg = M5.config();
    cfg.internal_spk = true;  // the Disk demo plays modem sounds and the microphone feeds the VU effect, so both are set up ...
    cfg.internal_mic = true;
    M5.begin(cfg);
    M5.Speaker.stop();        // ... but the speaker stays silent until something asks for a sound (no beeps at boot)
    M5.Speaker.setVolume(0);
    Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
    for (uint32_t t = millis(); !Serial && millis() - t < 1500;) yield(); // wait for CDC, bounded
#endif
    Serial.printf("NACHTGESPENST board=%s name=%s fw=%s %s\n", NG_BOARD_ID, K_DEVICE_NAME, __DATE__, __TIME__);

    Settings::begin();                                                  // stored settings first: the others read them
    Lamps::begin();
    Disk::begin();   // before HIDManager: BLE and USB handlers may deliver reports as soon as the host connects
    HIDManager::begin(K_BLE_NAME, K_MANUFACTURER);
    HIDManager::setPrefer((HIDManager::Prefer)Settings::prefer());     // routing chosen with SET prefer (default USB)   // the Bluetooth name carries the emoji; USB strings stay ASCII

    ui_begin();
    Presenter::begin();
}

void loop() {
    M5.update();
    serialcmd_poll();
#if NG_DEBUG_HEARTBEAT
    static uint32_t hb = 0;
    if (millis() - hb >= 1000) { hb = millis(); Serial.printf("[hb] t=%lu heap=%u ble=%d\n", (unsigned long)millis(), (unsigned)ESP.getFreeHeap(), (int)HIDManager::isBleConnected()); }
#endif

    HIDManager::handleAsyncEvents();
    Presenter::loop();                                  // the air mouse (does nothing unless the pointer is on)

#if NG_HAS_BTNS
    // Core2 capacitive buttons below the screen: A = previous page, B = Locks page <-> back, C = next page
    if (M5.BtnA.wasPressed()) ui_button(0, true);
    if (M5.BtnB.wasPressed()) ui_button(1, true);
    if (M5.BtnC.wasPressed()) ui_button(2, true);
    if (M5.BtnA.wasReleased()) ui_button(0, false);
    if (M5.BtnB.wasReleased()) ui_button(1, false);
    if (M5.BtnC.wasReleased()) ui_button(2, false);
#endif
    delay(ui_loop()); // sleep until LVGL's next job (1..10 ms) instead of spinning
}
