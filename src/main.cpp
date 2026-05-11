#include <M5Unified.h>
#include <FastLED.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "LedColors.h"

int currentLayerIndex = 0;
CRGB leds[LED_COUNT];

void updateHardwareFeedback() {
    CRGB color = (currentLayerIndex < (sizeof(LAYER_COLORS)/sizeof(LAYER_COLORS[0]))) 
                 ? LAYER_COLORS[currentLayerIndex] : CRGB::Blue;
    fill_solid(leds, LED_COUNT, color);
    FastLED.show();
    M5.Speaker.tone(1200, 50);
}

void drawUI() {
    M5.Lcd.startWrite();
    M5.Lcd.fillScreen(BLACK);
    const GridLayer& current = ALL_LAYERS[currentLayerIndex];
    int w = M5.Lcd.width() / current.cols;
    int h = M5.Lcd.height() / current.rows;

    M5.Lcd.setFont(&fonts::FreeSansBold12pt7b);
    for (int i = 0; i < (current.rows * current.cols); i++) {
        int x = (i % current.cols) * w;
        int y = (i / current.cols) * h;
        const Binding& b = current.tiles[i];
        if (b.payload != KC_NO.code) {
            M5.Lcd.fillRect(x + 2, y + 2, w - 4, h - 4, b.color);
            M5.Lcd.setTextColor(b.textColor);
            M5.Lcd.setTextDatum(MC_DATUM);
            M5.Lcd.drawString(b.glyph, x + (w / 2), y + (h / 2));
        }
    }
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setFont(&fonts::Font2); 
    M5.Lcd.drawString(current.pageName, M5.Lcd.width() / 2, 2);
    M5.Lcd.endWrite();
    updateHardwareFeedback();
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);
    M5.Lcd.setRotation(1);
    FastLED.addLeds<WS2812, 25, GRB>(leds, LED_COUNT);
    
    // Start Bluetooth via de Manager
    HIDManager::begin(K_DEVICE_NAME, K_MANUFACTURER);
    
    delay(500);
    drawUI();
}

void loop() {
    M5.update();
    if (M5.BtnA.wasPressed()) { currentLayerIndex = (currentLayerIndex > 0) ? currentLayerIndex - 1 : TOTAL_LAYERS - 1; drawUI(); }
    if (M5.BtnB.wasPressed()) { currentLayerIndex = 0; drawUI(); }
    if (M5.BtnC.wasPressed()) { currentLayerIndex = (currentLayerIndex < TOTAL_LAYERS - 1) ? currentLayerIndex + 1 : 0; drawUI(); }

    if (M5.Touch.getCount() > 0) {
        auto detail = M5.Touch.getDetail();
        if (detail.wasPressed()) {
            const GridLayer& current = ALL_LAYERS[currentLayerIndex];
            int col = detail.x / (M5.Lcd.width() / current.cols);
            int row = detail.y / (M5.Lcd.height() / current.rows);
            int idx = (row * current.cols) + col;
            if (idx >= 0 && idx < (current.rows * current.cols)) {
                if (current.tiles[idx].payload != KC_NO.code) {
                    HIDManager::send(current.tiles[idx]);
                    M5.Speaker.tone(2000, 20);
                }
            }
        }
    }
    delay(5);
}
