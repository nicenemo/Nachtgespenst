#include "HIDManager.h"
#include "HID_Specs.h"
#include "Lamps.h"
#include <M5Unified.h>
#include <vector>
#include <atomic>

// Two transports can be compiled in side by side (S3: USB + BLE, Core2: BLE only).
// send() goes to every connected transport, unless "prefer USB" is set and USB is
// connected: then BLE is skipped, so the host does not see every key twice.

#if NG_TRANSPORT_USB
#include <USB.h>
#include <USBHID.h>
#include "tusb.h"
#endif

namespace HIDManager {
    // ---- shared state -------------------------------------------------------
    // Written by the NimBLE / TinyUSB tasks, read by the main loop: atomics.
    static std::atomic<uint8_t> _ledStatus{0};
    static std::atomic<uint8_t> _prefer{(uint8_t)HIDManager::Prefer::USB};
    static HIDEventListener _eventListener = nullptr; // set once before traffic starts
    static std::vector<uint8_t> reportMap; // must outlive begin()

    // Host LED bits (HID LED page): Num=0x01, Caps=0x02, Scroll=0x04
    constexpr uint8_t LED_NUM = 0x01, LED_CAPS = 0x02, LED_SCROLL = 0x04;

    static void pushLed(uint8_t status) {
        _ledStatus.store(status);
        if (_eventListener) _eventListener(HIDEvent::LED_UPDATED, status);
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
            if (report_id >= HID::Reports::LAMP_MULTI && report_id <= HID::Reports::LAMP_CTRL) { Lamps::onOutput(report_id, buffer, len); return; }
            // Keyboard LED output report (Num/Caps/Scroll bits). TinyUSB hands the
            // payload without the report id; tolerate a leading id byte anyway.
            if (report_id != HID::Reports::KEYBOARD || len == 0) return;
            pushLed((len >= 2 && buffer[0] == report_id) ? buffer[1] : buffer[0]);
        }
        // A SET_REPORT on the control pipe (Windows HidD_SetOutputReport, Linux without an OUT endpoint) carries its report ID
        // in the request, so the Arduino USB layer calls this instead of _onOutput.
        void _onSetFeature(uint8_t report_id, const uint8_t* buffer, uint16_t len) override {
            if (report_id == HID::Reports::LAMP_REQ) Lamps::onSetFeature(report_id, buffer, len);
            else if (report_id >= HID::Reports::LAMP_MULTI && report_id <= HID::Reports::LAMP_CTRL) Lamps::onOutput(report_id, buffer, len);
            else _onOutput(report_id, buffer, len);
        }
        uint16_t _onGetFeature(uint8_t report_id, uint8_t* buffer, uint16_t len) override {
            return Lamps::onGetFeature(report_id, buffer, len);
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

    // "Connected" = enumerated and not suspended. usbHid.ready() only says the
    // endpoint is free right now, which flips while a report is in flight.
    static bool usbReady() { return tud_mounted() && !tud_suspended(); }
    static void usbBegin() {
        usbHid.begin();
        // Product/serial strings come from USB_PRODUCT / USB_SERIAL build flags.
    }
    // Short timeout: a stalled host must not block the main loop for the default 100 ms.
    static bool usbReport(uint8_t id, const uint8_t* data, size_t len) { return usbHid.SendReport(id, data, len, 1); }
#else
    static bool usbReady() { return false; }
    static void usbBegin() {}
    static bool usbReport(uint8_t, const uint8_t*, size_t) { return false; }
#endif

    // ---- BLE transport ------------------------------------------------------
#if NG_TRANSPORT_BLE
    static NimBLEHIDDevice* hid = nullptr;
    static NimBLECharacteristic* inputKeyboard = nullptr;
    static NimBLECharacteristic* inputConsumer = nullptr;
    static NimBLECharacteristic* outputKeyboard = nullptr;
    static std::atomic<bool> _bleConnected{false};

    // NimBLE-Arduino 2.x callback signatures (they carry NimBLEConnInfo).
    class MyOutputCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
            NimBLEAttValue value = pCharacteristic->getValue();
            if (value.size() > 0) pushLed(value.data()[0]);
        }
    };

    // LampArray characteristics (feature reports 5 to 7, output reports 8 to 10). The attributes are built when the host reads.
    static NimBLECharacteristic* lampChar[HID::Reports::LAMP_CTRL + 1] = {};
    class LampCallbacks : public NimBLECharacteristicCallbacks {
        void onWrite(NimBLECharacteristic* c, NimBLEConnInfo&) override {
            NimBLEAttValue v = c->getValue();
            for (uint8_t id = HID::Reports::LAMP_REQ; id <= HID::Reports::LAMP_CTRL; id++) {
                if (lampChar[id] != c) continue;
                if (id == HID::Reports::LAMP_REQ) Lamps::onSetFeature(id, v.data(), v.size());
                else if (id >= HID::Reports::LAMP_MULTI) Lamps::onOutput(id, v.data(), v.size());
                return;
            }
        }
        void onRead(NimBLECharacteristic* c, NimBLEConnInfo&) override {
            for (uint8_t id = HID::Reports::LAMP_ATTR; id <= HID::Reports::LAMP_RESP; id++) {
                if (lampChar[id] != c || id == HID::Reports::LAMP_REQ) continue;
                uint8_t buf[40];
                uint16_t n = Lamps::onGetFeature(id, buf, sizeof(buf));
                if (n) c->setValue(buf, n);
                return;
            }
        }
    };

    class MyServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
            _bleConnected = true;
            Serial.printf("[HID] BLE connected to %s\n", connInfo.getAddress().toString().c_str());
            // Short connection interval and long data packets: the LampArray test streams reports.
            pServer->updateConnParams(connInfo.getConnHandle(), 6, 12, 0, 400);
            pServer->setDataLen(connInfo.getConnHandle(), 251);
        }
        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
            _bleConnected = false;
            Serial.println("[HID] BLE disconnected, restarting advertising...");
            NimBLEDevice::startAdvertising();
        }
    };

    static void bleBegin(const char* deviceName, const char* manufacturer) {
        // 1. Device & security (before HID init)
        NimBLEDevice::init(deviceName);
        NimBLEDevice::setMTU(247);      // a LampArray multi update is 51 bytes
        // Bonding + secure connections, no MITM: with NO_INPUT_OUTPUT a MITM request
        // cannot be satisfied and strict hosts would refuse to pair.
        NimBLEDevice::setSecurityAuth(true, false, true);
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
        // LampArray reports (see HID_Specs.h): feature 5 to 7 and output 8 to 10.
        {
            auto* lcb = new LampCallbacks();
            for (uint8_t id = HID::Reports::LAMP_ATTR; id <= HID::Reports::LAMP_CTRL; id++) {
                lampChar[id] = id <= HID::Reports::LAMP_RESP ? hid->getFeatureReport(id) : hid->getOutputReport(id);
                if (lampChar[id]) lampChar[id]->setCallbacks(lcb);
            }
        }

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
    static bool bleReport(uint8_t id, const uint8_t* data, size_t len) {
        NimBLECharacteristic* c = (id == HID::Reports::KEYBOARD) ? inputKeyboard : inputConsumer;
        if (!c) return false;
        c->setValue(data, len);
        return c->notify();
    }
#else
    static void bleBegin(const char*, const char*) {}
    static bool bleReady() { return false; }
    static bool bleReport(uint8_t, const uint8_t*, size_t) { return false; }
#endif

    // ---- common -------------------------------------------------------------
    // Press queue + release state machine. Nothing blocks: a report that the
    // transport cannot take yet stays queued and is retried from the main loop.
    // A press is only released after RELEASE_DELAY, and a release that cannot be
    // delivered is retried (otherwise the host sees a stuck key).
    struct Report { uint8_t id, len, data[8]; };
    static constexpr uint8_t QCAP = 16;
    static Report _q[QCAP];
    static uint8_t _qHead = 0, _qCount = 0;
    static bool _pressed = false;             // a press is out, release pending
    static bool _relUsb = false, _relBle = false;
    static uint8_t _relId = 0, _relLen = 0;
    static uint32_t _relAt = 0, _deadline = 0;

    static void pump() {
        uint32_t now = millis();
        if (_pressed) {
            if ((int32_t)(now - _relAt) < 0) return;
            uint8_t rel[8] = {0};
            if (_relUsb && (usbReport(_relId, rel, _relLen) || !usbReady())) _relUsb = false;
            if (_relBle && (bleReport(_relId, rel, _relLen) || !bleReady())) _relBle = false;
            if ((_relUsb || _relBle) && (int32_t)(now - _deadline) > 0) _relUsb = _relBle = false; // link is dead
            if (!_relUsb && !_relBle) _pressed = false;
            return;
        }
        if (_qCount == 0) return;
        Report& r = _q[_qHead];
        bool usb = usbReady(), ble = bleReady();
        switch ((Prefer)_prefer.load()) {
            case Prefer::USB:  ble = ble && !usb; break; // USB wins while mounted
            case Prefer::BLE:  usb = usb && !ble; break; // BLE wins while connected
            case Prefer::BOTH: break;
            case Prefer::USB_ONLY: ble = false; break;
            case Prefer::BLE_ONLY: usb = false; break;
        }
        bool sentUsb = usb && usbReport(r.id, r.data, r.len);
        bool sentBle = ble && bleReport(r.id, r.data, r.len);
        if (sentUsb || sentBle) {
            _pressed = true; _relUsb = sentUsb; _relBle = sentBle; _relId = r.id; _relLen = r.len;
            _relAt = now + RELEASE_DELAY; _deadline = _relAt + 1000;
        } else if (usb || ble) {
            if ((int32_t)(now - _deadline) <= 0) return; // transport busy: retry next loop
        }
        // sent, or no transport / gave up: drop from queue
        _qHead = (_qHead + 1) % QCAP; _qCount--;
        _deadline = now + 1000; // budget for the next head item
    }

    static void dispatch(uint8_t id, const uint8_t* data, uint8_t len) {
        if (_qCount >= QCAP) return; // overflow: drop the newest press
        if (_qCount == 0 && !_pressed) _deadline = millis() + 1000;
        Report& r = _q[(_qHead + _qCount) % QCAP];
        r.id = id; r.len = len; memcpy(r.data, data, len);
        _qCount++;
        pump();
    }

    void begin(const char* deviceName, const char* manufacturer) {
        if (reportMap.empty()) reportMap = HID::GetReportMap();
        usbBegin();
        bleBegin(deviceName, manufacturer);
        Serial.printf("[HID] started usb=%d ble=%d (keyboard + consumer)\n", NG_TRANSPORT_USB, NG_TRANSPORT_BLE);
    }

    uint8_t getLedStatus() { return _ledStatus.load(); }

    void handleAsyncEvents() { pump(); }
    void setEventListener(HIDEventListener l) { _eventListener = l; }
    bool isConnected() { return usbReady() || bleReady(); }
    bool isUsbConnected() { return usbReady(); }
    bool isBleConnected() { return bleReady(); }
    void setPrefer(Prefer p) { _prefer = (uint8_t)p; }
    Prefer prefer() { return (Prefer)_prefer.load(); }
    void setPreferUsb(bool prefer) { setPrefer(prefer ? Prefer::USB : Prefer::BOTH); }
    bool preferUsb() { return prefer() == Prefer::USB; }

    static void sendKeyboard(const Binding& b) {
        uint8_t mod = b.modifier, key = 0;
        uint16_t id = b.usage.id;
        if (id >= 0xE0 && id <= 0xE7) mod |= (uint8_t)(1u << (id - 0xE0)); // modifier usage -> modifier byte
        else if (id <= 0xDD) key = (uint8_t)id;
        else return; // outside the keyboard usage range of the report map

        // BLE hosts do not always report the LED state back: toggle locally
        // so the tiles follow immediately (only when BLE is the sole path).
        if ((id == 0x39 || id == 0x53) && !usbReady() && bleReady())
            pushLed(_ledStatus.load() ^ ((id == 0x39) ? LED_CAPS : LED_NUM));
        uint8_t report[8] = { mod, 0, key, 0, 0, 0, 0, 0 };
        dispatch(HID::Reports::KEYBOARD, report, sizeof(report));
    }

    static void sendConsumer(const Binding& b) {
        if (b.usage.id == 0 || b.usage.id > 0x3FF) return; // 16-bit usage, 0 = released
        uint8_t report[2] = { (uint8_t)(b.usage.id & 0xFF), (uint8_t)(b.usage.id >> 8) };
        dispatch(HID::Reports::CONSUMER, report, sizeof(report));
    }

    void send(const Binding& b) {
        if (b.page == HID::Page::KEYBOARD_KEYPAD) sendKeyboard(b);
        else if (b.page == HID::Page::CONSUMER)   sendConsumer(b);
    }
}
