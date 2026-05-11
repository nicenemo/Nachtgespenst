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

    class MyOutputCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic) {
            std::string value = pCharacteristic->getValue();
            if (value.length() > 0) {
                uint8_t status = (uint8_t)value[0];
                if (xQueueSend(ledQueue, &status, 0) == pdTRUE) {
                    if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, status);
                }
            }
        }
    };

    class MyServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) { 
            _connected = true; 
            Serial.println("[HID] Verbonden!");
        }
        void onDisconnect(NimBLEServer* pServer) { 
            _connected = false; 
            Serial.println("[HID] Verbroken, start advertising...");
            NimBLEDevice::startAdvertising(); 
        }
    };

    void begin(const char* deviceName, const char* manufacturer) {
        ledQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));

        // 1. Initialiseer Device & Security (Vóór HID init)
        NimBLEDevice::init(deviceName);
        NimBLEDevice::setSecurityAuth(true, true, true); 
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

        // 2. Server & HID Device
        NimBLEServer* pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());
        hid = new NimBLEHIDDevice(pServer);

        // 3. Reports instellen
        inputKeyboard = hid->getInputReport(1);
        // inputConsumer = hid->getInputReport(2); // Tijdelijk uit
        outputKeyboard = hid->getOutputReport(1);
        if (outputKeyboard) outputKeyboard->setCallbacks(new MyOutputCallbacks());

        // 4. Device Info
        hid->setManufacturer(manufacturer);
        hid->setHidInfo(0x00, 0x01);
        
        // 5. REPORT MAP: Alleen keyboard block laden
        hid->setReportMap((uint8_t*)HID::Reports::KeyboardBlock, sizeof(HID::Reports::KeyboardBlock));

        // 6. Advertising (Handmatige methode voor Linux/Windows stabiliteit)
        NimBLEAdvertising* pAdv = pServer->getAdvertising();
        pAdv->setAppearance(0x03C1);
        pAdv->addServiceUUID(hid->getHidService()->getUUID());
        
        NimBLEAdvertisementData advData;
        advData.setFlags(0x06); 
        advData.setName(deviceName);
        pAdv->setAdvertisementData(advData);
        
        pAdv->start();
        Serial.println("[HID] Manager gestart (MINIMAL KEYBOARD ONLY)");
    }

    uint8_t getLedStatus() {
        uint8_t status;
        if (xQueueReceive(ledQueue, &status, 0) == pdTRUE) {
            _lastLedStatus = status;
        }
        return _lastLedStatus;
    }

    void handleAsyncEvents() {}
    void setEventListener(HIDEventListener listener) { _eventListener = listener; }
    bool isConnected() { return _connected; }

    void send(const Binding& b) {
        if (NimBLEDevice::getServer()->getConnectedCount() == 0) return;

        if (b.page == HIDPage::KEYBOARD) {
            // Lokale Toggle voor LED status zodat de M5 direct reageert
            if (b.payload == 0x39 || b.payload == 0x53) {
                uint8_t bit = (b.payload == 0x39) ? 0x02 : 0x01;
                _lastLedStatus ^= bit; 
                if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, _lastLedStatus);
            }

            uint8_t report[8] = { b.modifier, 0, (uint8_t)b.payload, 0, 0, 0, 0, 0 };
            inputKeyboard->setValue(report, 8);
            inputKeyboard->notify();
            
            delay(15); // Stabiele delay tussen press en release
            
            uint8_t release[8] = {0};
            inputKeyboard->setValue(release, 8);
            inputKeyboard->notify();
        } 
        /* else if (b.page == HIDPage::CONSUMER) {
            // Uitgecommentarieerd omdat de Report Map nu geen Consumer block heeft
            Serial.println("[HID] Consumer key genegeerd (Alleen Keyboard mode)");
        } 
        */
    }
}
