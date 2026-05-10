#include <M5Unified.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <FastLED.h>

#include "Config.h"
#include "HID_Specs.h"
#include "HID_Shortcuts.h"
#include "Keymap.h"
#include "DisplayColors.h"
#include "LedColors.h"

// --- Globalen ---
NimBLEHIDDevice* hid = nullptr;
NimBLECharacteristic* inputConsumer = nullptr;
NimBLECharacteristic* inputKeyboard = nullptr;
bool connected = false;

int currentLayerIndex = 0;
CRGB leds[LED_COUNT];

// --- LED Functie ---
void updateHardwareFeedback() {
    CRGB color = LAYER_COLORS[currentLayerIndex];
    fill_solid(leds, LED_COUNT, color);
    FastLED.show();
    M5.Speaker.tone(1200, 50);
}

// --- Rendering ---
void drawUI() {
    M5.Lcd.startWrite();
    M5.Lcd.fillScreen(BLACK);
    
    const Binding* grid = ALL_GRIDS[currentLayerIndex];
    int rows = (currentLayerIndex >= 1 && currentLayerIndex <= 3) ? 4 : 3;
    int cols = 3;
    int w = M5.Lcd.width() / cols;
    int h = M5.Lcd.height() / rows;

    M5.Lcd.setFont(&fonts::FreeSansBold12pt7b);
    for (int i = 0; i < (rows * cols); i++) {
        int x = (i % cols) * w;
        int y = (i / cols) * h;
        M5.Lcd.fillRect(x + 2, y + 2, w - 4, h - 4, grid[i].color);
        M5.Lcd.setTextColor(grid[i].textColor);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString(grid[i].glyph, x + (w / 2), y + (h / 2));
    }
    
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setFont(&fonts::Font2); 
    M5.Lcd.drawString(GRID_NAMES[currentLayerIndex], M5.Lcd.width() / 2, 0);
    
    M5.Lcd.endWrite();
    updateHardwareFeedback();
}

// --- Bluetooth Callbacks ---
class MyServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) { 
        connected = true; 
        Serial.println("Bluetooth verbonden!"); 
    }
    void onDisconnect(NimBLEServer* pServer) { 
        connected = false; 
        Serial.println("Bluetooth verbroken, start advertising...");
        NimBLEDevice::startAdvertising(); 
    }
};

// --- Bluetooth Task (Core 0) ---
void btTask(void* pvParameters) {
    NimBLEDevice::init(K_DEVICE_NAME);
    NimBLEServer* pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Belangrijk: Stel beveiliging in VOORDAT je HID start. 
    // Veel moderne OS accepteren geen onbeveiligde toetsenborden.
    NimBLEDevice::setSecurityAuth(true, true, true); 
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

    hid = new NimBLEHIDDevice(pServer);
    inputKeyboard = hid->getInputReport(1);
    inputConsumer = hid->getInputReport(2);

    hid->setManufacturer(K_MANUFACTURER);
    hid->setHidInfo(0x00, 0x01);
    hid->setReportMap((uint8_t*)HID::Reports::KeyboardBlock, sizeof(HID::Reports::KeyboardBlock));

    NimBLEAdvertising* pAdv = pServer->getAdvertising();
    pAdv->setAppearance(0x03C1);
    pAdv->addServiceUUID(hid->getHidService()->getUUID());
    
    NimBLEAdvertisementData advData;
    advData.setFlags(0x06);
    advData.setName(K_DEVICE_NAME);
    pAdv->setAdvertisementData(advData);
    
    pAdv->start();
    Serial.println("Wachten op verbinding...");

    for (;;) { vTaskDelay(pdMS_TO_TICKS(1000)); }
}

// --- HID Verzend Logica ---
void sendKey(const Binding& b) {
    // Betrouwbaardere check: Is er echt een actieve verbinding?
    if (NimBLEDevice::getServer()->getConnectedCount() == 0) {
        Serial.println("Fout: Geen actieve BT verbinding.");
        return;
    }
    
    Serial.printf("Versturen: %s (Payload: 0x%X)\n", b.glyph, b.payload);
    M5.Speaker.tone(2000, 20); 

    if (b.page == HIDPage::KEYBOARD) {
        uint8_t report[8] = { b.modifier, 0, (uint8_t)b.payload, 0, 0, 0, 0, 0 };
        inputKeyboard->setValue(report, 8);
        inputKeyboard->notify();
        delay(15);
        uint8_t release[8] = {0};
        inputKeyboard->setValue(release, 8);
        inputKeyboard->notify();
    } 
    else if (b.page == HIDPage::CONSUMER) {
        uint16_t key = (uint16_t)b.payload;
        uint8_t report[2] = { (uint8_t)(key & 0xFF), (uint8_t)(key >> 8) };
        inputConsumer->setValue(report, 2);
        inputConsumer->notify();
        delay(15);
        uint8_t release[2] = {0, 0};
        inputConsumer->setValue(release, 2);
        inputConsumer->notify();
    }
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);
    
    M5.Lcd.setRotation(1);
    
    FastLED.addLeds<WS2812, 25, GRB>(leds, LED_COUNT);
    FastLED.setBrightness(40);

    xTaskCreatePinnedToCore(btTask, "btTask", 4096, NULL, 1, NULL, 0);
    
    delay(500);
    drawUI();
}

void loop() {
    M5.update();

    // Layer wisselen met fysieke knoppen
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

    // Touch input
    if (M5.Touch.getCount() > 0) {
        auto detail = M5.Touch.getDetail();
        if (detail.wasPressed()) {
            int rows = (currentLayerIndex >= 1 && currentLayerIndex <= 3) ? 4 : 3;
            int col = detail.x / (M5.Lcd.width() / 3);
            int row = detail.y / (M5.Lcd.height() / rows);
            int idx = (row * 3) + col;
            
            if (idx >= 0 && idx < (rows * 3)) {
                sendKey(ALL_GRIDS[currentLayerIndex][idx]);
            }
        }
    }
    delay(5);
}
