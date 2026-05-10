#pragma once
#include <stdint.h>

namespace HID::Haptics {
    // De Usage Page ID voor Physical Interface Device
    constexpr uint8_t PAGE_ID = 0x0F;

    // ==========================================
    // EFFECT TYPES (Usage 0x25)
    // Welk soort trilling of kracht moet er worden uitgevoerd
    // ==========================================
    namespace Effect {
        constexpr uint8_t CONSTANT_FORCE    = 0x26;
        constexpr uint8_t RAMP              = 0x27;
        constexpr uint8_t SQUARE            = 0x30;
        constexpr uint8_t SINE              = 0x31;
        constexpr uint8_t TRIANGLE          = 0x32;
        constexpr uint8_t SAWTOOTH_UP       = 0x33;
        constexpr uint8_t SAWTOOTH_DOWN     = 0x34;
        constexpr uint8_t SPRING            = 0x40; // Kracht evenredig met afstand
        constexpr uint8_t DAMPER            = 0x41; // Kracht evenredig met snelheid
        constexpr uint8_t INERTIA           = 0x42; // Kracht evenredig met versnelling
        constexpr uint8_t FRICTION          = 0x43; // Constante weerstand
    }

    // ==========================================
    // PID STATE & CONTROL (Output naar Host / Input naar Device)
    // ==========================================
    namespace Control {
        constexpr uint8_t PID_STATE_REPORT  = 0x02; // Status van het device
        constexpr uint8_t EFFECT_CONTROL    = 0x77; // Start/Stop/Pause
        constexpr uint8_t DEVICE_CONTROL    = 0x96; // Reset/Enable/Disable
        constexpr uint8_t DEVICE_GAIN       = 0x7D; // Master volume van de trilling
        
        // Waardes voor Effect Control
        enum EffectOp : uint8_t {
            START           = 0x01,
            START_SOLO      = 0x02, // Stop alle anderen, start deze
            STOP            = 0x03
        };

        // Waardes voor Device Control
        enum DeviceOp : uint8_t {
            ENABLE_ACTUATORS  = 0x01,
            DISABLE_ACTUATORS = 0x02,
            STOP_ALL_EFFECTS  = 0x03,
            DEVICE_RESET      = 0x04,
            DEVICE_PAUSE      = 0x05,
            DEVICE_CONTINUE   = 0x06
        };
    }

    // ==========================================
    // DYNAMIC PARAMETERS
    // Parameters die een effect definiëren
    // ==========================================
    namespace Param {
        constexpr uint8_t DURATION          = 0x50; // Milliseconden
        constexpr uint8_t SAMPLE_PERIOD     = 0x51;
        constexpr uint8_t GAIN              = 0x52; // 0-10000 (meestal)
        constexpr uint8_t TRIGGER_BUTTON    = 0x53;
        constexpr uint8_t TRIGGER_REPEAT    = 0x54;
        constexpr uint8_t DIRECTION         = 0x57; // Hoek in graden
        constexpr uint8_t START_DELAY       = 0x59;
    }

    // ==========================================
    // HAPTIC FEEDBACK (Specifiek voor schermen/oppervlaktes)
    // Nieuwere extensies voor touch-devices
    // ==========================================
    namespace Feedback {
        constexpr uint8_t WAVEFORM_LIST     = 0x10;
        constexpr uint8_t DURATION_LIST     = 0x11;
        
        // Standaard haptische golfvormen
        enum Waveform : uint8_t {
            NONE            = 0x00,
            CLICK           = 0x01,
            BUZZ            = 0x02,
            RUMBLE          = 0x03,
            SHARP_CLICK     = 0x04,
            SOFT_BUMP       = 0x05
        };
    }
}
