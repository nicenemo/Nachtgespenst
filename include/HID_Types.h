#pragma once
#include <stdint.h>

namespace HID {
    enum class UsageKind : uint8_t {
        OTHER = 0, CA, CP, CL, LC, DV, Sel, OOC, MC, OSC, RTC
    };

    struct Usage {
        uint16_t id;
        UsageKind kind;
        const char* identifier;
    };

    enum class Page : uint16_t {
            ARCADE = 0x0091,
        AUXILIARY_DISPLAY = 0x0014,
        BARCODE_SCANNER = 0x008C,
        BATTERY_SYSTEM = 0x0085,
        BRAILLE_DISPLAY = 0x0041,
        BUTTON = 0x0009,
        CAMERA_CONTROL = 0x0090,
        CONSUMER = 0x000C,
        DIGITIZERS = 0x000D,
        EYE_AND_HEAD_TRACKERS = 0x0012,
        FIDO_ALLIANCE = 0xF1D0,
        GAME_CONTROLS = 0x0005,
        GENERIC_DESKTOP = 0x0001,
        GENERIC_DEVICE_CONTROLS = 0x0006,
        HAPTICS = 0x000E,
        KEYBOARD_KEYPAD = 0x0007,
        LED = 0x0008,
        LIGHTING_AND_ILLUMINATION = 0x0059,
        MAGNETIC_STRIPE_READER = 0x008E,
        MEDICAL_INSTRUMENT = 0x0040,
        MONITOR = 0x0080,
        MONITOR_ENUMERATED = 0x0081,
        ORDINAL = 0x000A,
        PHYSICAL_INPUT_DEVICE = 0x000F,
        POWER = 0x0084,
        SCALES = 0x008D,
        SENSORS = 0x0020,
        SIMULATION_CONTROLS = 0x0002,
        SOC = 0x0011,
        SPORT_CONTROLS = 0x0004,
        TELEPHONY_DEVICE = 0x000B,
        VESA_VIRTUAL_CONTROLS = 0x0082,
        VR_CONTROLS = 0x0003
    };
}
