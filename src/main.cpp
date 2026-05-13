#include <M5Unified.h>
#include <FastLED.h>
#include "Config.h"
#include "Keymap.h"
#include "HIDManager.h"
#include "LedColors.h"
#include "Theme.h"

int currentLayerIndex = 0;
CRGB leds[LED_COUNT];

void updateHardwareFeedback(bool isNewKey = false);

void drawUI() {
    M5.Lcd.startWrite();
    Theme theme = ThemeManager::getTheme();
    M5.Lcd.fillScreen(theme.background);
    
    const GridLayer& current = LAYERS[currentLayerIndex];
    int w = M5.Lcd.width() / current.cols;
    int h = M5.Lcd.height() / current.rows;

    for (int i = 0; i < (current.rows * current.cols); i++) {
        int x = (i % current.cols) * w;
        int y = (i / current.cols) * h;
        const Binding& b = current.tiles[i];
        
        uint16_t bg = ThemeManager::getKeyColor(b, true);
        uint16_t fg = ThemeManager::getKeyColor(b, false);

        // Render Tile
        M5.Lcd.fillRect(x + 2, y + 2, w - 4, h - 4, bg);
        M5.Lcd.drawRect(x, y, w, h, theme.gridLines);
        
        // 1. Tiny Overlay (Original Request: identifier in corner)
        if (b.usage.id != 0) {
            M5.Lcd.setFont(&fonts::Font0);
            M5.Lcd.setTextColor(theme.overlayText);
            M5.Lcd.setTextDatum(top_left);
            M5.Lcd.drawString(b.usage.identifier, x + 4, y + 4);
        }

        // 2. Main Label (Original Request: Friendly Label)
        if (b.label != nullptr) {
            M5.Lcd.setFont(&fonts::FreeSansBold12pt7b);
            M5.Lcd.setTextColor(fg);
            M5.Lcd.setTextDatum(middle_center);
            M5.Lcd.drawString(b.label, x + (w / 2), y + (h / 2));
        }
    }

    // 3. Grid Title Overlay (Tiny White letters on top)
    M5.Lcd.setFont(&fonts::Font0);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextDatum(top_left);
    M5.Lcd.drawString(current.title, 4, 4);

    M5.Lcd.endWrite();
    updateHardwareFeedback(false);
}

// ... rest of setup() and loop() remain the same ...
