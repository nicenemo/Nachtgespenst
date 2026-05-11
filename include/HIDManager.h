#pragma once
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include "Bindings.h"

namespace HIDManager {
    // Initialiseer Bluetooth en HID
    void begin(const char* deviceName, const char* manufacturer);
    
    // Status check
    bool isConnected();

    // De centrale verzendmethode (beslist welke report gestuurd wordt)
    void send(const Binding& b);

    // Specifieke verzendmethoden (intern gebruikt door send of direct aanroepbaar)
    void sendKeyboardReport(const Binding& b);
    void sendConsumerReport(const Binding& b);
    void handleSystemCommand(const Binding& b);
    void handleLocalAction(const Binding& b);
}
