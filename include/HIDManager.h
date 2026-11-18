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
    // Event types that tell the listener what is happening
    enum class HIDEvent { 
        LED_UPDATED,  // New LED state data from the host
        QUEUE_FULL    // The buffer is full, data is dropped
    };
    
    // Signature of the callback function
    typedef void (*HIDEventListener)(HIDEvent event, uint8_t ledStatus);

    // --- Public constants ---
    const uint8_t QUEUE_LENGTH = 10;
    const unsigned long RELEASE_DELAY = 15; // ms

    // Initialise Bluetooth and HID
    void begin(const char* deviceName, const char* manufacturer);
    
    // Status checks
    bool isConnected();      // USB or BLE
    bool isUsbConnected();
    bool isBleConnected();

    // Prevent double keystrokes when both transports are connected:
    //   USB  = skip BLE while USB is mounted (default), BLE = skip USB while BLE is connected,
    //   BOTH = send to every connected transport,
    //   USB_ONLY / BLE_ONLY = never use the other transport, even when the chosen one is down.
    enum class Prefer : uint8_t { USB, BLE, BOTH, USB_ONLY, BLE_ONLY };
    void setPrefer(Prefer p);
    Prefer prefer();
    void setPreferUsb(bool prefer); // true = Prefer::USB, false = Prefer::BOTH
    bool preferUsb();

    // The central send method (decides which report is sent)
    void send(const Binding& b);
    
    // Event-driven functionality
    void setEventListener(HIDEventListener listener);
    uint8_t getLedStatus(); 
    
    // Non-blocking handling (call from the main loop in main.cpp)
    void handleAsyncEvents();
}
