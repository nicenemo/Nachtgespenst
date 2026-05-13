#pragma once
#include <stdint.h>

namespace HID {
    enum class UsageKind : uint8_t {
        OTHER = 0, CA, CP, CL, LC, DV, Sel, OOC, MC, OSC, RTC [cite: 10]
    };

    struct Usage {
        uint16_t id; [cite: 11]
        UsageKind kind; [cite: 11]
        const char* identifier; [cite: 11]
    };

    enum class Page : uint16_t {
            ARCADE = 0x0091, [cite: 12]
        AUXILIARY_DISPLAY = 0x0014, [cite: 12]
        BARCODE_SCANNER = 0x008C, [cite: 12]
        BATTERY_SYSTEM = 0x0085, [cite: 12]
        BRAILLE_DISPLAY = 0x0041, [cite: 12]
        BUTTON = 0x0009, [cite: 12]
        CAMERA_CONTROL = 0x0090, [cite: 12]
        CONSUMER = 0x000C, [cite: 12]
        DIGITIZERS = 0x000D, [cite: 12]
        EYE_AND_HEAD_TRACKERS = 0x0012, [cite: 12]
        FIDO_ALLIANCE = 0xF1D0, [cite: 12]
        GAME_CONTROLS = 0x0005, [cite: 12]
        GENERIC_DESKTOP = 0x0001, [cite: 12]
        GENERIC_DEVICE_CONTROLS = 0x0006, [cite: 12]
        HAPTICS = 0x000E, [cite: 12]
        KEYBOARD_KEYPAD = 0x0007, [cite: 12]
        LED = 0x0008, [cite: 12]
        LIGHTING_AND_ILLUMINATION = 0x0059, [cite: 12]
        MAGNETIC_STRIPE_READER = 0x008E, [cite: 12]
        MEDICAL_INSTRUMENT = 0x0040, [cite: 12]
        MONITOR = 0x0080, [cite: 12]
        MONITOR_ENUMERATED = 0x0081, [cite: 12]
        ORDINAL = 0x000A, [cite: 12]
        PHYSICAL_INPUT_DEVICE = 0x000F, [cite: 12]
        POWER = 0x0084, [cite: 12]
        SCALES = 0x008D, [cite: 12]
        SENSORS = 0x0020, [cite: 12]
        SIMULATION_CONTROLS = 0x0002, [cite: 12]
        SOC = 0x0011, [cite: 12]
        SPORT_CONTROLS = 0x0004, [cite: 12]
        TELEPHONY_DEVICE = 0x000B, [cite: 12]
        VESA_VIRTUAL_CONTROLS = 0x0082, [cite: 12]
        VR_CONTROLS = 0x0003 [cite: 12]
    };
}
