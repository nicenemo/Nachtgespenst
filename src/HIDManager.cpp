#include "HIDManager.h"
#include "HID_Specs.h"
#include <M5Unified.h>
#include <vector>

#if NG_TRANSPORT_USB
#include <USB.h>
#include <USBHID.h>

namespace HIDManager {
    static std::vector<uint8_t> reportMap;
    static USBHID usbHid;
    static uint8_t _lastLedStatus = 0;
    static QueueHandle_t ledQueue = nullptr;
    static HIDEventListener _eventListener = nullptr;

    class NgUsbDevice : public USBHIDDevice {
    public:
        uint16_t _onGetDescriptor(uint8_t* buffer) override {
            memcpy(buffer, reportMap.data(), reportMap.size());
            return reportMap.size();
        }
        void _onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len) override {
            // Keyboard LED output report (Num/Caps/Scroll bits)
            if (len == 0) return;
            uint8_t status = buffer[len - 1];
            if (len >= 2) status = buffer[1]; // report id may be included
            if (len == 1) status = buffer[0];
            if (ledQueue && xQueueSend(ledQueue, &status, 0) == pdTRUE) {
                if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, status);
            } else if (_eventListener) {
                _eventListener(HIDEvent::QUEUE_FULL, status);
            }
        }
    };
    static NgUsbDevice usbDevice;

    // The Arduino USB stack (CDC on boot) builds the configuration descriptor
    // before setup() runs, so the HID device must be registered at static-init
    // time, otherwise wDescriptorLength is 0 and the kernel rejects the interface.
    static struct UsbRegistrar {
        UsbRegistrar() {
            reportMap = HID::GetReportMap();
            USBHID::addDevice(&usbDevice, reportMap.size());
        }
    } usbRegistrar;

    void begin(const char* deviceName, const char* manufacturer) {
        ledQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));
        usbHid.begin();
        // Product/serial strings come from USB_PRODUCT / USB_SERIAL build flags
        // (the core applies them at USB init, before setup()).
        Serial.println("[HID] USB HID gestart (keyboard + consumer)");
    }

    uint8_t getLedStatus() {
        uint8_t status;
        if (ledQueue && xQueueReceive(ledQueue, &status, 0) == pdTRUE) _lastLedStatus = status;
        return _lastLedStatus;
    }
    static bool _relPending = false; static uint8_t _relReport = 0; static uint8_t _relLen = 0; static uint32_t _relAt = 0;
    static void scheduleRelease(uint8_t reportId, uint8_t len) { _relPending = true; _relReport = reportId; _relLen = len; _relAt = millis() + RELEASE_DELAY; }
    static void flushRelease(bool force) {
        if (_relPending && (force || (int32_t)(millis() - _relAt) >= 0)) {
            uint8_t rel[8] = {0};
            usbHid.SendReport(_relReport, rel, _relLen);
            _relPending = false;
        }
    }
    void handleAsyncEvents() { flushRelease(false); }
    void setEventListener(HIDEventListener l) { _eventListener = l; }
    bool isConnected() { return usbHid.ready(); }

    void send(const Binding& b) {
        flushRelease(true);
        if (b.page == HID::Page::KEYBOARD_KEYPAD) {
            uint8_t mod = b.modifier, key = 0;
            uint16_t id = b.usage.id;
            if (id >= 0xE0 && id <= 0xE7) mod |= (uint8_t)(1u << (id - 0xE0));
            else if (id <= 0xDD) key = (uint8_t)id;
            else return;
            uint8_t report[8] = { mod, 0, key, 0, 0, 0, 0, 0 };
            usbHid.SendReport(HID::Reports::KEYBOARD, report, sizeof(report));
            scheduleRelease(HID::Reports::KEYBOARD, sizeof(report));
        } else if (b.page == HID::Page::CONSUMER) {
            if (b.usage.id == 0 || b.usage.id > 0x3FF) return;
            uint8_t report[2] = { (uint8_t)(b.usage.id & 0xFF), (uint8_t)(b.usage.id >> 8) };
            usbHid.SendReport(HID::Reports::CONSUMER, report, sizeof(report));
            scheduleRelease(HID::Reports::CONSUMER, sizeof(report));
        }
    }
}
#else

namespace HIDManager {
    NimBLEHIDDevice* hid = nullptr;
    NimBLECharacteristic* inputKeyboard = nullptr;
    NimBLECharacteristic* inputConsumer = nullptr;
    NimBLECharacteristic* outputKeyboard = nullptr;
    bool _connected = false;
    uint8_t _lastLedStatus = 0;
    QueueHandle_t ledQueue = nullptr;
    HIDEventListener _eventListener = nullptr;

    // Report map must outlive begin(): keep it in static storage.
    static std::vector<uint8_t> reportMap;

    // NimBLE-Arduino 2.x callback signatures (they carry NimBLEConnInfo).
    class MyOutputCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            NimBLEAttValue value = pCharacteristic->getValue();
            if (value.size() > 0) {
                uint8_t status = value.data()[0];
                if (xQueueSend(ledQueue, &status, 0) == pdTRUE) {
                    if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, status);
                } else if (_eventListener) {
                    _eventListener(HIDEvent::QUEUE_FULL, status);
                }
            }
        }
    };

    class MyServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
            _connected = true;
            Serial.println("[HID] Verbonden!");
        }
        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
            _connected = false;
            Serial.println("[HID] Verbroken, start advertising...");
            NimBLEDevice::startAdvertising();
        }
    };

    void begin(const char* deviceName, const char* manufacturer) {
        ledQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));

        // 1. Device & security (before HID init)
        NimBLEDevice::init(deviceName);
        NimBLEDevice::setSecurityAuth(true, true, true);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

        // 2. Server & HID device
        NimBLEServer* pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());
        hid = new NimBLEHIDDevice(pServer);

        // 3. Reports. The IDs must match the Report IDs in the report map
        //    (HID::Reports::ID::KEYBOARD = 1, CONSUMER = 2).
        inputKeyboard  = hid->getInputReport(HID::Reports::KEYBOARD);
        inputConsumer  = hid->getInputReport(HID::Reports::CONSUMER);
        outputKeyboard = hid->getOutputReport(HID::Reports::KEYBOARD);
        if (outputKeyboard) outputKeyboard->setCallbacks(new MyOutputCallbacks());

        // 4. Device info
        hid->setManufacturer(manufacturer);
        hid->setHidInfo(0x00, 0x01);

        // 5. Report map: keyboard (+LEDs) and consumer control
        reportMap = HID::GetReportMap();
        hid->setReportMap(reportMap.data(), reportMap.size());

        // 6. Advertising (manual advertisement data: Linux/Windows stability).
        //    The HID service UUID has to be in the advertisement data itself,
        //    otherwise setAdvertisementData() would drop it.
        NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
        NimBLEAdvertisementData advData;
        advData.setFlags(0x06);
        advData.setAppearance(0x03C1);
        advData.setCompleteServices(hid->getHidService()->getUUID());
        advData.setName(deviceName);
        pAdv->setAdvertisementData(advData);

        pAdv->start();
        Serial.println("[HID] Manager gestart (keyboard + consumer)");
    }

    uint8_t getLedStatus() {
        uint8_t status;
        if (ledQueue && xQueueReceive(ledQueue, &status, 0) == pdTRUE) {
            _lastLedStatus = status;
        }
        return _lastLedStatus;
    }

    static bool _relPending = false; static bool _relKbd = true; static uint32_t _relAt = 0;
    static void scheduleRelease(bool kbd) { _relPending = true; _relKbd = kbd; _relAt = millis() + RELEASE_DELAY; }
    static void flushRelease(bool force) {
        if (!_relPending || (!force && (int32_t)(millis() - _relAt) < 0)) return;
        _relPending = false;
        if (_relKbd) { uint8_t r[8] = {0}; inputKeyboard->setValue(r, sizeof(r)); inputKeyboard->notify(); }
        else         { uint8_t r[2] = {0}; inputConsumer->setValue(r, sizeof(r)); inputConsumer->notify(); }
    }
    void handleAsyncEvents() { flushRelease(false); }
    void setEventListener(HIDEventListener listener) { _eventListener = listener; }
    bool isConnected() { return _connected; }

    static void sendKeyboard(const Binding& b) {
        uint8_t mod = b.modifier;
        uint8_t key = 0;
        uint16_t id = b.usage.id;

        if (id >= 0xE0 && id <= 0xE7) {
            // Modifier usage (e.g. kp_LEFT_SHIFT): goes into the modifier byte
            mod |= (uint8_t)(1u << (id - 0xE0));
        } else if (id <= 0xDD) {
            key = (uint8_t)id;
        } else {
            return; // outside the keyboard usage range of the report map
        }

        // Local toggle so the M5 reflects Caps/Num Lock immediately
        if (id == 0x39 || id == 0x53) {
            uint8_t bit = (id == 0x39) ? 0x02 : 0x01;
            _lastLedStatus ^= bit;
            if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, _lastLedStatus);
        }

        uint8_t report[8] = { mod, 0, key, 0, 0, 0, 0, 0 };
        inputKeyboard->setValue(report, sizeof(report));
        inputKeyboard->notify();

        scheduleRelease(true);
    }

    static void sendConsumer(const Binding& b) {
        // Report ID 2: one 16-bit usage (0 = released)
        if (b.usage.id == 0 || b.usage.id > 0x3FF) return;

        uint8_t report[2] = { (uint8_t)(b.usage.id & 0xFF), (uint8_t)(b.usage.id >> 8) };
        inputConsumer->setValue(report, sizeof(report));
        inputConsumer->notify();

        scheduleRelease(false);
    }

    void send(const Binding& b) {
        flushRelease(true);
        NimBLEServer* server = NimBLEDevice::getServer();
        if (!server || server->getConnectedCount() == 0) return;

        if (b.page == HID::Page::KEYBOARD_KEYPAD) {
            sendKeyboard(b);
        } else if (b.page == HID::Page::CONSUMER) {
            sendConsumer(b);
        }
    }
}

#endif // NG_TRANSPORT_USB
