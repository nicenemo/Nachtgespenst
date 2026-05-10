#include <M5Unified.h>
#include <HijelHID_BLEKeyboard.h> // NEW HEADER
#include <FastLED.h>
#include <NimBLEDevice.h>
#include "Config.h"
#include "Keymap.h"
#include "Colors.h"

CRGB leds[LED_COUNT];
HijelHID_BLEKeyboard ble(K_DEVICE_NAME, K_MANUFACTURER, 100);

QueueHandle_t q;
volatile int activeLayer = 0;
volatile bool isTyping = false;
unsigned long typingTimer = 0;

void hidTask(void* pv) {
    // This library handles NimBLE init internally
    ble.begin();
    
    int keyIndex;
    for (;;) {
        if (xQueueReceive(q, &keyIndex, portMAX_DELAY)) {
            if (ble.isPaired()) { // Use isPaired() for better stability
                isTyping = true;
                typingTimer = millis();
                ble.print(DECK[activeLayer][keyIndex].payload);
            }
        }
    }
}

void updateLEDs() {
    if (!ble.isConnected()) {
        uint8_t breathe = beatsin8(15, 30, 180);
        fill_solid(leds, LED_COUNT, CRGB(0, 0, breathe));
    } else if (isTyping) {
        fill_solid(leds, LED_COUNT, CRGB(255, 80, 0));
        if (millis() - typingTimer > 200) isTyping = false;
    } else {
        uint8_t idle = beatsin8(10, 5, 40);
        fill_solid(leds, LED_COUNT, CRGB(0, idle, 0));
    }
    FastLED.show();
}

void draw() {
    M5.Lcd.startWrite();
    M5.Lcd.fillScreen(0x0000);
    for (int i = 0; i < 9; i++) {
        int col = i % 3; int row = i / 3;
        M5.Lcd.fillRect(col * 106 + 4, row * 80 + 4, 98, 72, DECK[activeLayer][i].color);
        M5.Lcd.setTextColor(0xFFFF);
        M5.Lcd.drawString(DECK[activeLayer][i].glyph, col * 106 + 53, row * 80 + 40);
    }
    M5.Lcd.setCursor(10, 225);
    M5.Lcd.printf(ble.isConnected() ? "L%d | CONNECTED" : "L%d | PAIRING...", activeLayer);
    M5.Lcd.endWrite();
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    FastLED.addLeds<SK6812, 25, GRB>(leds, LED_COUNT);
    FastLED.setBrightness(60);
    M5.Lcd.setRotation(1);
    M5.Lcd.setFont(&fonts::efontCN_24);
    M5.Lcd.setTextDatum(middle_center);
    q = xQueueCreate(16, sizeof(int));
    xTaskCreatePinnedToCore(hidTask, "HID_TASK", 4096, NULL, 3, NULL, 0);
    draw();
}

void loop() {
    M5.update();
    updateLEDs();
    static bool lastConnState = false;
    if (ble.isConnected() != lastConnState) {
        lastConnState = ble.isConnected();
        draw();
    }
    if (M5.BtnA.wasPressed()) { activeLayer = (activeLayer + 8) % 9; draw(); }
    if (M5.BtnC.wasPressed()) { activeLayer = (activeLayer + 1) % 9; draw(); }
    if (M5.Touch.getCount()) {
        auto detail = M5.Touch.getDetail();
        if (detail.wasPressed()) {
            int index = (detail.y / 80) * 3 + (detail.x / 106);
            if (index >= 0 && index < 9) {
                xQueueSend(q, &index, 0);
                M5.Speaker.tone(4500, 30); 
            }
        }
    }
    vTaskDelay(5);
}
