#include "HIDManager.h"
#include "HID_Specs.h"
#include <M5Unified.h>
#include <vector>

// Two transports can be compiled in side by side (S3: USB + BLE, Core2: BLE only).
// send() goes to every connected transport, unless "prefer USB" is set and USB is
// connected: then BLE is skipped, so the host does not see every key twice.

#if NG_TRANSPORT_USB
#include <USB.h>
#include <USBHID.h>
#endif

namespace HIDManager {
    // ---- shared state -------------------------------------------------------
    static uint8_t _lastLedStatus = 0;
    static QueueHandle_t ledQueue = nullptr;
    static HIDEventListener _eventListener = nullptr;
    static bool _preferUsb = true;
    static std::vector<uint8_t> reportMap; // must outlive begin()

    static void pushLed(uint8_t status) {
        if (ledQueue && xQueueSend(ledQueue, &status, 0) == pdTRUE) {
            if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, status);
        } else if (_eventListener) {
            _eventListener(HIDEvent::QUEUE_FULL, status);
        }
    }

    // ---- USB transport ------------------------------------------------------
#if NG_TRANSPORT_USB
    static USBHID usbHid;

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
            pushLed(status);
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

    static bool usbReady() { return usbHid.ready(); }
    static void usbBegin() {
        usbHid.begin();
        // Product/serial strings come from USB_PRODUCT / USB_SERIAL build flags.
    }
    static void usbReport(uint8_t id, const uint8_t* data, size_t len) { usbHid.SendReport(id, data, len); }
#else
    static bool usbReady() { return false; }
    static void usbBegin() {}
    static void usbReport(uint8_t, const uint8_t*, size_t) {}
#endif

    // ---- BLE transport ------------------------------------------------------
#if NG_TRANSPORT_BLE
    static NimBLEHIDDevice* hid = nullptr;
    static NimBLECharacteristic* inputKeyboard = nullptr;
    static NimBLECharacteristic* inputConsumer = nullptr;
    static NimBLECharacteristic* outputKeyboard = nullptr;
    static bool _bleConnected = false;

    // NimBLE-Arduino 2.x callback signatures (they carry NimBLEConnInfo).
    class MyOutputCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            NimBLEAttValue value = pCharacteristic->getValue();
            if (value.size() > 0) pushLed(value.data()[0]);
        }
    };

    class MyServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
            _bleConnected = true;
            Serial.println("[HID] BLE verbonden!");
        }
        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
            _bleConnected = false;
            Serial.println("[HID] BLE verbroken, start advertising...");
            NimBLEDevice::startAdvertising();
        }
    };

    static void bleBegin(const char* deviceName, const char* manufacturer) {
        // 1. Device & security (before HID init)
        NimBLEDevice::init(deviceName);
        NimBLEDevice::setSecurityAuth(true, true, true);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

        // 2. Server & HID device
        NimBLEServer* pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(new MyServerCallbacks());
        hid = new NimBLEHIDDevice(pServer);

        // 3. Reports. The IDs must match the Report IDs in the report map.
        inputKeyboard  = hid->getInputReport(HID::Reports::KEYBOARD);
        inputConsumer  = hid->getInputReport(HID::Reports::CONSUMER);
        outputKeyboard = hid->getOutputReport(HID::Reports::KEYBOARD);
        if (outputKeyboard) outputKeyboard->setCallbacks(new MyOutputCallbacks());

        // 4. Device info
        hid->setManufacturer(manufacturer);
        hid->setHidInfo(0x00, 0x01);

        // 5. Report map: keyboard (+LEDs) and consumer control
        if (reportMap.empty()) reportMap = HID::GetReportMap();
        hid->setReportMap(reportMap.data(), reportMap.size());

        // 6. Advertising (manual advertisement data: Linux/Windows stability).
        NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
        NimBLEAdvertisementData advData;
        advData.setFlags(0x06);
        advData.setAppearance(0x03C1);
        advData.setCompleteServices(hid->getHidService()->getUUID());
        advData.setName(deviceName);
        pAdv->setAdvertisementData(advData);
        pAdv->start();
    }

    static bool bleReady() {
        NimBLEServer* server = NimBLEDevice::getServer();
        return _bleConnected && server && server->getConnectedCount() > 0;
    }
    static void bleReport(uint8_t id, const uint8_t* data, size_t len) {
        NimBLECharacteristic* c = (id == HID::Reports::KEYBOARD) ? inputKeyboard : inputConsumer;
        if (!c) return;
        c->setValue(data, len);
        c->notify();
    }
#else
    static void bleBegin(const char*, const char*) {}
    static bool bleReady() { return false; }
    static void bleReport(uint8_t, const uint8_t*, size_t) {}
#endif

    // ---- common -------------------------------------------------------------
    // Which transports got the last press (and so need the release report).
    static bool _relPending = false;
    static bool _relUsb = false, _relBle = false;
    static uint8_t _relId = 0, _relLen = 0;
    static uint32_t _relAt = 0;

    static void flushRelease(bool force) {
        if (!_relPending || (!force && (int32_t)(millis() - _relAt) < 0)) return;
        _relPending = false;
        uint8_t rel[8] = {0};
        if (_relUsb) usbReport(_relId, rel, _relLen);
        if (_relBle) bleReport(_relId, rel, _relLen);
    }

    static void dispatch(uint8_t id, const uint8_t* data, uint8_t len) {
        bool usb = usbReady();
        bool ble = bleReady() && !(usb && _preferUsb);
        if (!usb && !ble) return;
        if (usb) usbReport(id, data, len);
        if (ble) bleReport(id, data, len);
        _relPending = true; _relUsb = usb; _relBle = ble;
        _relId = id; _relLen = len; _relAt = millis() + RELEASE_DELAY;
    }

    void begin(const char* deviceName, const char* manufacturer) {
        ledQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));
        if (reportMap.empty()) reportMap = HID::GetReportMap();
        usbBegin();
        bleBegin(deviceName, manufacturer);
        Serial.printf("[HID] gestart usb=%d ble=%d (keyboard + consumer)\n", NG_TRANSPORT_USB, NG_TRANSPORT_BLE);
    }

    uint8_t getLedStatus() {
        uint8_t status;
        if (ledQueue && xQueueReceive(ledQueue, &status, 0) == pdTRUE) _lastLedStatus = status;
        return _lastLedStatus;
    }

    void handleAsyncEvents() { flushRelease(false); }
    void setEventListener(HIDEventListener l) { _eventListener = l; }
    bool isConnected() { return usbReady() || bleReady(); }
    bool isUsbConnected() { return usbReady(); }
    bool isBleConnected() { return bleReady(); }
    void setPreferUsb(bool prefer) { _preferUsb = prefer; }
    bool preferUsb() { return _preferUsb; }

    static void sendKeyboard(const Binding& b) {
        uint8_t mod = b.modifier, key = 0;
        uint16_t id = b.usage.id;
        if (id >= 0xE0 && id <= 0xE7) mod |= (uint8_t)(1u << (id - 0xE0)); // modifier usage -> modifier byte
        else if (id <= 0xDD) key = (uint8_t)id;
        else return; // outside the keyboard usage range of the report map

        // BLE hosts do not always report the LED state back: toggle locally
        // so the tiles follow immediately (only when BLE is the sole path).
        if ((id == 0x39 || id == 0x53) && !usbReady() && bleReady()) {
            _lastLedStatus ^= (id == 0x39) ? 0x02 : 0x01;
            if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, _lastLedStatus);
        }
        uint8_t report[8] = { mod, 0, key, 0, 0, 0, 0, 0 };
        dispatch(HID::Reports::KEYBOARD, report, sizeof(report));
    }

    static void sendConsumer(const Binding& b) {
        if (b.usage.id == 0 || b.usage.id > 0x3FF) return; // 16-bit usage, 0 = released
        uint8_t report[2] = { (uint8_t)(b.usage.id & 0xFF), (uint8_t)(b.usage.id >> 8) };
        dispatch(HID::Reports::CONSUMER, report, sizeof(report));
    }

    void send(const Binding& b) {
        flushRelease(true);
        if (b.page == HID::Page::KEYBOARD_KEYPAD) sendKeyboard(b);
        else if (b.page == HID::Page::CONSUMER)   sendConsumer(b);
    }
}
