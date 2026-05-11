#include <M5Unified.h>
#include <FastLED.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "LedColors.h"

int currentLayerIndex = 0;
CRGB leds[LED_COUNT];

uint8_t currentFlashBrightness = 85; 
const uint8_t BASE_BRIGHTNESS = 85;   
const uint8_t FLASH_BRIGHTNESS = 128;  
const uint8_t FADE_SPEED = 2;          

void updateHardwareFeedback(bool isNewKey = false);

// Callback die wordt getriggerd als de PC de Caps Lock status wijzigt
void onHIDEvent(HIDManager::HIDEvent event, uint8_t status) {
    if (event == HIDManager::HIDEvent::LED_UPDATED) {
        Serial.printf("[MAIN] HID Event: LED update ontvangen (0x%02X)\n", status);
        updateHardwareFeedback(false);
    }
}

void updateHardwareFeedback(bool isNewKey) {
    if (isNewKey) {
        currentFlashBrightness = FLASH_BRIGHTNESS;
    }

    CRGB layerColor = ALL_LAYERS[currentLayerIndex].color;
    CRGB animatedColor = layerColor;
    animatedColor.nscale8_video(currentFlashBrightness); 

    // Haal de laatste bekende status op
    uint8_t ledStatus = HIDManager::getLedStatus();

    for (int i = 0; i < LED_COUNT; i++) {
        if (i < 5) {
            // RECHTS: Toont de kleur van de actieve Layer
            leds[i] = animatedColor;
        } else {
            // LINKS: Toont Caps Lock status (Bit 1 / 0x02)
            if (ledStatus & 0x02) { 
                leds[i] = CRGB::White;
                leds[i].nscale8_video(120); 
            } else {
                leds[i] = CRGB::Black;
            }
        }
    }
    FastLED.show();
}

void drawUI() {
    Serial.printf("[MAIN] Tekenen UI voor Layer %d\n", currentLayerIndex);
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
            M5.Lcd.setTextDatum(middle_center);
            M5.Lcd.drawString(b.glyph, x + (w / 2), y + (h / 2));
        }
    }
    M5.Lcd.endWrite();
    updateHardwareFeedback(false);
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    Serial.begin(115200);
    delay(1000); 
    Serial.println("--- M5 MACROPAD BOOT ---");

    setCpuFrequencyMhz(80); 
    M5.Lcd.setRotation(1);
    
    FastLED.addLeds<WS2812, 25, GRB>(leds, LED_COUNT);
    
    // HID Manager starten
    HIDManager::begin(K_DEVICE_NAME, K_MANUFACTURER);
    HIDManager::setEventListener(onHIDEvent);
    
    drawUI();
}

void loop() {
    M5.update();
    
    // Animatie faden (voor de visuele feedback bij toetsdruk)
    if (currentFlashBrightness > BASE_BRIGHTNESS) {
        if (currentFlashBrightness - FADE_SPEED < BASE_BRIGHTNESS) {
            currentFlashBrightness = BASE_BRIGHTNESS;
        } else {
            currentFlashBrightness -= FADE_SPEED;
        }
        updateHardwareFeedback(false); 
    }

    // Touch input afhandeling
    if (M5.Touch.getCount() > 0) {
        auto detail = M5.Touch.getDetail();
        if (detail.wasPressed()) {
            const GridLayer& current = ALL_LAYERS[currentLayerIndex];
            int col = detail.x / (M5.Lcd.width() / current.cols);
            int row = detail.y / (M5.Lcd.height() / current.rows);
            int index = row * current.cols + col;
            
            if (index < (current.rows * current.cols)) {
                Serial.printf("[MAIN] Toets indrukken op index %d\n", index);
                HIDManager::send(current.tiles[index]);
                M5.Speaker.tone(1200, 20);
                updateHardwareFeedback(true); 
            }
        }
    }

    // Fysieke knoppen onder scherm voor layer-wissel
    if (M5.BtnA.wasPressed()) { 
        currentLayerIndex = (currentLayerIndex > 0) ? currentLayerIndex - 1 : TOTAL_LAYERS - 1; 
        drawUI(); 
    }
    if (M5.BtnB.wasPressed()) { 
        currentLayerIndex = 0; 
        drawUI(); 
    }
    if (M5.BtnC.wasPressed()) { 
        currentLayerIndex = (currentLayerIndex < TOTAL_LAYERS - 1) ? currentLayerIndex + 1 : 0; 
        drawUI(); 
    }

    // Stabiele delay (geen sleep meer)
    delay(10);
}
