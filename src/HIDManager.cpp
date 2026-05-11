#include "HIDManager.h"
#include "HID_Specs.h"
#include <M5Unified.h>

namespace HIDManager {
    NimBLEHIDDevice* hid = nullptr;
    NimBLECharacteristic* inputKeyboard = nullptr;
    NimBLECharacteristic* inputConsumer = nullptr;
    NimBLECharacteristic* outputKeyboard = nullptr;
    bool _connected = false;
    uint8_t _lastLedStatus = 0;
    QueueHandle_t ledQueue = nullptr;
    HIDEventListener _eventListener = nullptr;

    // Callback voor data die de PC naar de M5 stuurt (zoals Caps Lock status)
    class MyOutputCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic) {
            std::string value = pCharacteristic->getValue();
            if (value.length() > 0) {
                uint8_t status = (uint8_t)value[0];
                Serial.printf("[HID-DEBUG] LED Data ontvangen: 0x%02X\n", status);
                if (xQueueSend(ledQueue, &status, 0) == pdTRUE) {
                    if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, status);
                }
            }
        }
    };

    class MyServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) { 
            _connected = true; 
            Serial.println("[HID-STATUS] Verbonden (Basis)");
        };

        void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
            _connected = true;
            Serial.printf("[HID-STATUS] Verbonden met client! Conn ID: %d\n", desc->conn_handle);
            // Optimaliseer de verbinding voor snelheid
            pServer->updateConnParams(desc->conn_handle, 6, 12, 0, 60);
        };

        void onDisconnect(NimBLEServer* pServer) { 
            _connected = false; 
            Serial.println("[HID-STATUS] Verbinding verbroken. Start Advertising...");
            NimBLEDevice::startAdvertising(); 
        }
    };

    void begin(const char* deviceName, const char* manufacturer) {
        Serial.println("[HID-INIT] Start NimBLE HID Setup...");
        ledQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));

        NimBLEDevice::init(deviceName);
        
        // --- SECURITY SETUP (Cruciaal voor Windows/macOS HID) ---
        NimBLEDevice::setSecurityAuth(true, true, true); 
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

        NimBLEServer* pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());

        hid = new NimBLEHIDDevice(pServer);
        inputKeyboard = hid->getInputReport(1);
        inputConsumer = hid->getInputReport(2);
        
        outputKeyboard = hid->getOutputReport(1);
        if (outputKeyboard) {
            outputKeyboard->setCallbacks(new MyOutputCallbacks());
        }

        hid->setManufacturer(manufacturer);
        // Gebruik de NimBLE-specifieke setters
        hid->setPnp(0x02, 0xe502, 0xa111, 0x0210);
        hid->setHidInfo(0x00, 0x01);
        
        std::vector<uint8_t> reportMap = HID::GetReportMap();
        hid->setReportMap((uint8_t*)reportMap.data(), reportMap.size());

        // hid->startServices() is deprecated, NimBLE doet dit nu automatisch

        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setAppearance(0x03C1); // HID Keyboard Appearance
        pAdvertising->addServiceUUID(hid->getHidService()->getUUID());
        pAdvertising->start();

        Serial.println("[HID-INIT] Setup voltooid. Wachten op pairing...");
    }

    uint8_t getLedStatus() {
        uint8_t status;
        if (xQueueReceive(ledQueue, &status, 0) == pdTRUE) {
            _lastLedStatus = status;
        }
        return _lastLedStatus;
    }

    void handleAsyncEvents() { }

    void setEventListener(HIDEventListener listener) { _eventListener = listener; }
    bool isConnected() { return _connected; }

    void send(const Binding& b) {
        if (!_connected) {
            // Geen Serial print hier om de log niet te vervuilen bij onbedoelde touches
            return;
        }

        if (b.page == HIDPage::KEYBOARD) {
            Serial.printf("[HID-SEND] Key: 0x%02X, Mod: 0x%02X\n", b.payload, b.modifier);
            uint8_t report[8] = { b.modifier, 0, (uint8_t)b.payload, 0, 0, 0, 0, 0 };
            inputKeyboard->setValue(report, 8);
            inputKeyboard->notify();
            
            delay(20); // Handmatige delay voor stabiliteit
            
            uint8_t release[8] = {0};
            inputKeyboard->setValue(release, 8);
            inputKeyboard->notify();
        } 
        else if (b.page == HIDPage::CONSUMER) {
            Serial.printf("[HID-SEND] Consumer: 0x%04X\n", b.payload);
            uint16_t key = (uint16_t)b.payload;
            inputConsumer->setValue((uint8_t*)&key, 2);
            inputConsumer->notify();
            
            delay(20);
            
            uint16_t release = 0;
            inputConsumer->setValue((uint8_t*)&release, 2);
            inputConsumer->notify();
        }
    }
}
