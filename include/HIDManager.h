#pragma once
#include <Arduino.h>
// Transports (can be combined): USB HID (NG_TRANSPORT_USB=1, Core S3) and BLE (default on).
#ifndef NG_TRANSPORT_USB
#define NG_TRANSPORT_USB 0
#endif
#ifndef NG_TRANSPORT_BLE
#define NG_TRANSPORT_BLE 1
#endif
#if NG_TRANSPORT_BLE
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#endif
#include "Bindings.h"

namespace HIDManager {
    // Event types om aan de luisteraar te laten weten wat er gebeurt
    enum class HIDEvent { 
        LED_UPDATED,  // Er is nieuwe LED status data van de PC
        QUEUE_FULL    // De buffer is vol, data wordt overgeslagen
    };
    
    // De signature van de callback functie
    typedef void (*HIDEventListener)(HIDEvent event, uint8_t ledStatus);

    // --- Publieke Constanten ---
    const uint8_t QUEUE_LENGTH = 10;
    const unsigned long RELEASE_DELAY = 15; // ms

    // Initialiseer Bluetooth en HID
    void begin(const char* deviceName, const char* manufacturer);
    
    // Status check
    bool isConnected();      // USB or BLE
    bool isUsbConnected();
    bool isBleConnected();

    // Prevent double keystrokes: while USB is connected, skip BLE (default on).
    void setPreferUsb(bool prefer);
    bool preferUsb();

    // De centrale verzendmethode (beslist welke report gestuurd wordt)
    void send(const Binding& b);
    
    // Event-driven functionaliteit
    void setEventListener(HIDEventListener listener);
    uint8_t getLedStatus(); 
    
    // Non-blocking afhandeling (aanroepen in de main loop van main.cpp)
    void handleAsyncEvents();
}
