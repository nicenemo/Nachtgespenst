#pragma once
#include <stdint.h>

namespace HID {
    /**
     * Complete Page Mapping
     */
    enum class Page : uint16_t {
            ARCADE = 0x91,
        AUXILIARY_DISPLAY = 0x14,
        BARCODE_SCANNER = 0x8C,
        BATTERY_SYSTEM = 0x85,
        BRAILLE_DISPLAY = 0x41,
        BUTTON = 0x09,
        CAMERA_CONTROL = 0x90,
        CONSUMER = 0x0C,
        DIGITIZERS = 0x0D,
        EYE_AND_HEAD_TRACKERS = 0x12,
        FIDO_ALLIANCE = 0xF1D0,
        GAME_CONTROLS = 0x05,
        GENERIC_DESKTOP = 0x01,
        GENERIC_DEVICE_CONTROLS = 0x06,
        HAPTICS = 0x0E,
        KEYBOARD_KEYPAD = 0x07,
        LED = 0x08,
        LIGHTING_AND_ILLUMINATION = 0x59,
        MAGNETIC_STRIPE_READER = 0x8E,
        MEDICAL_INSTRUMENT = 0x40,
        MONITOR = 0x80,
        MONITOR_ENUMERATED = 0x81,
        ORDINAL = 0x0A,
        PHYSICAL_INPUT_DEVICE = 0x0F,
        POWER = 0x84,
        SCALES = 0x8D,
        SENSORS = 0x20,
        SIMULATION_CONTROLS = 0x02,
        SOC = 0x11,
        SPORT_CONTROLS = 0x04,
        TELEPHONY_DEVICE = 0x0B,
        VESA_VIRTUAL_CONTROLS = 0x82,
        VR_CONTROLS = 0x03
    };

    struct Key {
        Page page;
        uint16_t code;
    };
}
