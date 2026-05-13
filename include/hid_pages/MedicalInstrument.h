#pragma once
#include "../HID_Types.h"

namespace HID::MedicalInstrument {
    constexpr uint8_t PAGE_ID = 0x0040;
    namespace OOC {
        constexpr Usage VCR_ACQUISITION = { 0x20, UsageKind::OOC, "VCR_ACQUISITION" };
        constexpr Usage FREEZE_THAW = { 0x21, UsageKind::OOC, "FREEZE_THAW" };
    }
    namespace LC {
        constexpr Usage CINE = { 0x40, UsageKind::LC, "CINE" };
        constexpr Usage TRANSMIT_POWER = { 0x41, UsageKind::LC, "TRANSMIT_POWER" };
        constexpr Usage VOLUME = { 0x42, UsageKind::LC, "VOLUME" };
        constexpr Usage FOCUS = { 0x43, UsageKind::LC, "FOCUS" };
        constexpr Usage DEPTH = { 0x44, UsageKind::LC, "DEPTH" };
        constexpr Usage SOFT_STEP_PRIMARY = { 0x60, UsageKind::LC, "SOFT_STEP_PRIMARY" };
        constexpr Usage SOFT_STEP_SECONDARY = { 0x61, UsageKind::LC, "SOFT_STEP_SECONDARY" };
        constexpr Usage DEPTH_GAIN_COMPENSATION = { 0x70, UsageKind::LC, "DEPTH_GAIN_COMPENSATION" };
        constexpr Usage ZOOM_ADJUST = { 0x81, UsageKind::LC, "ZOOM_ADJUST" };
        constexpr Usage SPECTRAL_DOPPLER_ADJUST = { 0x83, UsageKind::LC, "SPECTRAL_DOPPLER_ADJUST" };
        constexpr Usage COLOR_DOPPLER_ADJUST = { 0x85, UsageKind::LC, "COLOR_DOPPLER_ADJUST" };
        constexpr Usage MOTION_MODE_ADJUST = { 0x87, UsageKind::LC, "MOTION_MODE_ADJUST" };
        constexpr Usage 2_D_MODE_ADJUST = { 0x89, UsageKind::LC, "2_D_MODE_ADJUST" };
        constexpr Usage SOFT_CONTROL_ADJUST = { 0xA1, UsageKind::LC, "SOFT_CONTROL_ADJUST" };
    }
    namespace OSC {
        constexpr Usage CLIP_STORE = { 0x22, UsageKind::OSC, "CLIP_STORE" };
        constexpr Usage UPDATE = { 0x23, UsageKind::OSC, "UPDATE" };
        constexpr Usage NEXT = { 0x24, UsageKind::OSC, "NEXT" };
        constexpr Usage SAVE = { 0x25, UsageKind::OSC, "SAVE" };
        constexpr Usage PRINT = { 0x26, UsageKind::OSC, "PRINT" };
        constexpr Usage MICROPHONE_ENABLE = { 0x27, UsageKind::OSC, "MICROPHONE_ENABLE" };
        constexpr Usage ZOOM_SELECT = { 0x80, UsageKind::OSC, "ZOOM_SELECT" };
        constexpr Usage SPECTRAL_DOPPLER_MODE_SELECT = { 0x82, UsageKind::OSC, "SPECTRAL_DOPPLER_MODE_SELECT" };
        constexpr Usage COLOR_DOPPLER_MODE_SELECT = { 0x84, UsageKind::OSC, "COLOR_DOPPLER_MODE_SELECT" };
        constexpr Usage MOTION_MODE_SELECT = { 0x86, UsageKind::OSC, "MOTION_MODE_SELECT" };
        constexpr Usage 2_D_MODE_SELECT = { 0x88, UsageKind::OSC, "2_D_MODE_SELECT" };
        constexpr Usage SOFT_CONTROL_SELECT = { 0xA0, UsageKind::OSC, "SOFT_CONTROL_SELECT" };
    }
    namespace CA {
        constexpr Usage MEDICAL_ULTRASOUND = { 0x01, UsageKind::CA, "MEDICAL_ULTRASOUND" };
    }
}
