#pragma once
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
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
    bool isConnected();

    // De centrale verzendmethode (beslist welke report gestuurd wordt)
    void send(const Binding& b);
    
    // Event-driven functionaliteit
    void setEventListener(HIDEventListener listener);
    uint8_t getLedStatus(); 
    
    // Non-blocking afhandeling (aanroepen in de main loop van main.cpp)
    void handleAsyncEvents();
}
