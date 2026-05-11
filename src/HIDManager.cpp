#include "HIDManager.h"
#include "HID_Specs.h"
#include <M5Unified.h>

namespace HIDManager {
    NimBLEHIDDevice* hid = nullptr;
    NimBLECharacteristic* inputKeyboard = nullptr;
    NimBLECharacteristic* inputConsumer = nullptr;
    bool _connected = false;

    // Callbacks voor Bluetooth verbinding
    // De signaturen zijn hier aangepast naar de standaard NimBLE verwachting
    class MyServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer) { 
            _connected = true; 
            Serial.println("HID: Device verbonden");
        }

        void onDisconnect(NimBLEServer* pServer) { 
            _connected = false; 
            Serial.println("HID: Device verbroken, start advertising...");
            NimBLEDevice::startAdvertising(); 
        }
        
        // Extra variant die sommige versies van NimBLE vereisen voor stabiliteit
        void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
            _connected = true;
            Serial.println("HID: Verbonden met client");
        }
    };

    void begin(const char* deviceName, const char* manufacturer) {
        NimBLEDevice::init(deviceName);
        NimBLEServer* pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());

        hid = new NimBLEHIDDevice(pServer);
        inputKeyboard = hid->getInputReport(1);
        inputConsumer = hid->getInputReport(2);

        hid->setManufacturer(manufacturer);
        hid->setHidInfo(0x00, 0x01);
        
        // Report map vertelt de PC wat voor soort HID device we zijn
        hid->setReportMap((uint8_t*)HID::Reports::KeyboardBlock, sizeof(HID::Reports::KeyboardBlock));

        pServer->getAdvertising()->start();
        Serial.println("HID Manager: Initialisatie voltooid");
    }

    bool isConnected() { 
        return _connected; 
    }

    void sendKeyboardReport(const Binding& b) {
        // Standaard 8-byte keyboard report
        uint8_t report[8] = { b.modifier, 0, (uint8_t)b.payload, 0, 0, 0, 0, 0 };
        inputKeyboard->setValue(report, 8);
        inputKeyboard->notify();
        
        delay(15); // Korte pauze voor verwerking door OS
        
        // Release alle toetsen
        uint8_t release[8] = {0};
        inputKeyboard->setValue(release, 8);
        inputKeyboard->notify();
    }

    void sendConsumerReport(const Binding& b) {
        // 16-bit consumer report (Media keys etc)
        uint16_t key = (uint16_t)b.payload;
        uint8_t report[2] = { (uint8_t)(key & 0xFF), (uint8_t)(key >> 8) };
        inputConsumer->setValue(report, 2);
        inputConsumer->notify();
        
        delay(15);
        
        uint8_t release[2] = {0, 0};
        inputConsumer->setValue(release, 2);
        inputConsumer->notify();
    }

    void handleSystemCommand(const Binding& b) {
        // Gereserveerd voor toekomstige uitbreiding (Power/Sleep)
        Serial.printf("HID: System command 0x%X genegeerd (nog geen mapping)\n", b.payload);
    }

    void handleLocalAction(const Binding& b) {
        // Acties die alleen op de M5Stack zelf plaatsvinden
        if (b.page == HIDPage::HAPTIC) {
            M5.Speaker.tone(400, 50);
        }
        // Lighting acties kunnen hier eventueel via een callback naar main.cpp
    }

    void send(const Binding& b) {
        // Alleen verzenden als we verbonden zijn, tenzij het een lokale actie is
        if (!_connected && b.page != HIDPage::HAPTIC && b.page != HIDPage::LIGHTING) {
            Serial.println("HID: Niet verzonden (geen BT verbinding)");
            return;
        }

        switch (b.page) {
            case HIDPage::KEYBOARD: 
                sendKeyboardReport(b); 
                break;
            case HIDPage::CONSUMER: 
                sendConsumerReport(b); 
                break;
            case HIDPage::SYSTEM:   
                handleSystemCommand(b); 
                break;
            case HIDPage::HAPTIC:
            case HIDPage::LIGHTING: 
                handleLocalAction(b);   
                break;
            default: 
                Serial.println("HID: Onbekende pagina");
                break;
        }
    }
}
