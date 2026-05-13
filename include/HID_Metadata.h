#pragma once
#include "HID_Types.h"

namespace HID {
    struct PageInfo {
        uint16_t id;
        const char* name;
        const char* macro; // Prefix used for Bindings (e.g., "kp")
    };

    namespace Page {
        constexpr PageInfo Arcade = { 0x0091, "Arcade", "ar" };
        constexpr PageInfo AuxiliaryDisplay = { 0x0014, "Auxiliary Display", "au" };
        constexpr PageInfo BarcodeScanner = { 0x008C, "Barcode Scanner", "ba" };
        constexpr PageInfo BatterySystem = { 0x0085, "Battery System", "ba" };
        constexpr PageInfo BrailleDisplay = { 0x0041, "Braille Display", "br" };
        constexpr PageInfo Button = { 0x0009, "Button", "bu" };
        constexpr PageInfo CameraControl = { 0x0090, "Camera Control", "ca" };
        constexpr PageInfo Consumer = { 0x000C, "Consumer", "cp" };
        constexpr PageInfo Digitizers = { 0x000D, "Digitizers", "di" };
        constexpr PageInfo EyeandHeadTrackers = { 0x0012, "Eye and Head Trackers", "ey" };
        constexpr PageInfo FIDOAlliance = { 0xF1D0, "FIDO Alliance", "fi" };
        constexpr PageInfo GameControls = { 0x0005, "Game Controls", "ga" };
        constexpr PageInfo GenericDesktop = { 0x0001, "Generic Desktop", "gp" };
        constexpr PageInfo GenericDeviceControls = { 0x0006, "Generic Device Controls", "ge" };
        constexpr PageInfo Haptics = { 0x000E, "Haptics", "ha" };
        constexpr PageInfo KeyboardKeypad = { 0x0007, "Keyboard/Keypad", "kp" };
        constexpr PageInfo LED = { 0x0008, "LED", "le" };
        constexpr PageInfo LightingAndIllumination = { 0x0059, "Lighting And Illumination", "li" };
        constexpr PageInfo MagneticStripeReader = { 0x008E, "Magnetic Stripe Reader", "ma" };
        constexpr PageInfo MedicalInstrument = { 0x0040, "Medical Instrument", "me" };
        constexpr PageInfo Monitor = { 0x0080, "Monitor", "mo" };
        constexpr PageInfo MonitorEnumerated = { 0x0081, "Monitor Enumerated", "mo" };
        constexpr PageInfo Ordinal = { 0x000A, "Ordinal", "or" };
        constexpr PageInfo PhysicalInputDevice = { 0x000F, "Physical Input Device", "ph" };
        constexpr PageInfo Power = { 0x0084, "Power", "po" };
        constexpr PageInfo Scales = { 0x008D, "Scales", "sc" };
        constexpr PageInfo Sensors = { 0x0020, "Sensors", "se" };
        constexpr PageInfo SimulationControls = { 0x0002, "Simulation Controls", "si" };
        constexpr PageInfo SoC = { 0x0011, "SoC", "so" };
        constexpr PageInfo SportControls = { 0x0004, "Sport Controls", "sp" };
        constexpr PageInfo TelephonyDevice = { 0x000B, "Telephony Device", "te" };
        constexpr PageInfo VESAVirtualControls = { 0x0082, "VESA Virtual Controls", "ve" };
        constexpr PageInfo VRControls = { 0x0003, "VR Controls", "vr" };
    }

    constexpr PageInfo ALL_PAGES[] = {
        Page::Arcade,
        Page::AuxiliaryDisplay,
        Page::BarcodeScanner,
        Page::BatterySystem,
        Page::BrailleDisplay,
        Page::Button,
        Page::CameraControl,
        Page::Consumer,
        Page::Digitizers,
        Page::EyeandHeadTrackers,
        Page::FIDOAlliance,
        Page::GameControls,
        Page::GenericDesktop,
        Page::GenericDeviceControls,
        Page::Haptics,
        Page::KeyboardKeypad,
        Page::LED,
        Page::LightingAndIllumination,
        Page::MagneticStripeReader,
        Page::MedicalInstrument,
        Page::Monitor,
        Page::MonitorEnumerated,
        Page::Ordinal,
        Page::PhysicalInputDevice,
        Page::Power,
        Page::Scales,
        Page::Sensors,
        Page::SimulationControls,
        Page::SoC,
        Page::SportControls,
        Page::TelephonyDevice,
        Page::VESAVirtualControls,
        Page::VRControls
    };

    constexpr uint8_t PAGE_COUNT = sizeof(ALL_PAGES) / sizeof(ALL_PAGES[0]);

    inline const PageInfo* getPageInfo(uint16_t id) {
        for (uint8_t i = 0; i < PAGE_COUNT; i++) {
            if (ALL_PAGES[i].id == id) return &ALL_PAGES[i];
        }
        return nullptr;
    }
}
