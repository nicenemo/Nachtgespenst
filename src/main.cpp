#include <M5Unified.h>
#include <BleKeyboard.h>
#include "Config.h"
#include "Keymap.h"
#include "Colors.h"

/**
 * NACHGTESPENST FIRMWARE - V12 (2026 REVISION)
 * Optimized for M5Unified + NimBLE
 */

// Initialize BLE Keyboard with BKA Metadata
BleKeyboard ble(K_DEVICE_NAME, K_MANUFACTURER, 100);

QueueHandle_t q;
volatile int activeLayer = 0;
unsigned long lastBattUpdate = 0;

/**
 * Task: HID_PROCESSOR
 * Runs on Core 0 to handle BLE stack traffic separately from UI
 */
void hidTask(void* pv) {
    ble.begin();
    int keyIndex;
    for (;;) {
        if (xQueueReceive(q, &keyIndex, portMAX_DELAY)) {
            if (ble.isConnected()) {
                // Transmit the macro payload
                ble.print(DECK[activeLayer][keyIndex].payload);
            }
        }
    }
}

/**
 * UI: RENDER_GRID
 * Draws the 3x3 touch interface using LGFX-supported colors
 */
void draw() {
    M5.Lcd.startWrite();
    M5.Lcd.fillScreen(K_COLOR_BLACK); 
    
    for (int i = 0; i < 9; i++) {
        int col = i % 3;
        int row = i / 3;
        int x = col * 106; // Screen width 320 / 3
        int y = row * 80;  // Screen height 240 / 3

        // Draw Key Housing
        M5.Lcd.fillRect(x + 4, y + 4, 98, 72, DECK[activeLayer][i].color);
        
        // Draw UTF-8 Glyph (LGFX handles multi-byte strings)
        M5.Lcd.setTextColor(K_COLOR_WHITE);
        M5.Lcd.drawString(DECK[activeLayer][i].glyph, x + 53, y + 40);
    }
    
    // Status Footer
    M5.Lcd.setTextColor(K_COLOR_NEUTRAL); 
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(10, 225);
    M5.Lcd.printf("L%d | %s | BATT: %d%%", activeLayer, K_MODEL, M5.Power.getBatteryLevel());
    
    M5.Lcd.endWrite();
}

void setup() {
    // Start M5Unified with automatic config
    auto cfg = M5.config();
    M5.begin(cfg);

    // Display Setup
    M5.Lcd.setRotation(1);
    M5.Lcd.setBrightness(128);
    
    // Set Font to efontCN to ensure UTF-8 Glyphs/Emojis render correctly
    M5.Lcd.setFont(&fonts::efontCN_24);
    M5.Lcd.setTextDatum(middle_center);

    // Initialize Event Queue
    q = xQueueCreate(16, sizeof(int));

    // Launch Bluetooth Task on Core 0
    xTaskCreatePinnedToCore(hidTask, "HID_TASK", 4096, NULL, 3, NULL, 0);

    // Initial Screen Draw
    draw();
}

void loop() {
    // Update internal state for Touch and Buttons
    M5.update();

    // 1. Sync Battery Level with BLE Host (Every 60s)
    if (millis() - lastBattUpdate > 60000) {
        if (ble.isConnected()) {
            ble.setBatteryLevel(M5.Power.getBatteryLevel());
        }
        lastBattUpdate = millis();
    }

    // 2. Physical Button Navigation (A=Left, C=Right)
    if (M5.BtnA.wasPressed()) { 
        activeLayer = (activeLayer + 8) % 9; 
        draw(); 
    }
    if (M5.BtnC.wasPressed()) { 
        activeLayer = (activeLayer + 1) % 9; 
        draw(); 
    }

    // 3. Touch Matrix Logic
    if (M5.Touch.getCount()) {
        auto detail = M5.Touch.getDetail();
        if (detail.wasPressed()) {
            int col = detail.x / 106;
            int row = detail.y / 80;
            int index = (row * 3) + col;

            if (index >= 0 && index < 9) {
                // Queue the key press for the Bluetooth task
                xQueueSend(q, &index, 0);
                
                // Feedback: Audio + Visual Haptic
                M5.Speaker.tone(4000, 40); 
                M5.Lcd.drawRect(col * 106 + 2, row * 80 + 2, 102, 76, K_COLOR_WHITE);
            }
        }
        // Redraw on release to clear the white selection border
        if (detail.wasReleased()) {
            draw();
        }
    }
    
    // Prevent Watchdog Reset
    vTaskDelay(1);
}
