#pragma once
#include "../HID_Types.h"

namespace HID::MedicalInstrument {
    constexpr uint8_t PAGE_ID = 0x0040;
    namespace OTHER {
        constexpr Usage MEDICAL_ULTRASOUND = { 0x01, UsageKind::OTHER, "MEDICAL_ULTRASOUND" };
        constexpr Usage VCR_ACQUISITION = { 0x20, UsageKind::OTHER, "VCR_ACQUISITION" };
        constexpr Usage FREEZE_THAW = { 0x21, UsageKind::OTHER, "FREEZE_THAW" };
        constexpr Usage CLIP_STORE = { 0x22, UsageKind::OTHER, "CLIP_STORE" };
        constexpr Usage UPDATE = { 0x23, UsageKind::OTHER, "UPDATE" };
        constexpr Usage NEXT = { 0x24, UsageKind::OTHER, "NEXT" };
        constexpr Usage SAVE = { 0x25, UsageKind::OTHER, "SAVE" };
        constexpr Usage PRINT = { 0x26, UsageKind::OTHER, "PRINT" };
        constexpr Usage MICROPHONE_ENABLE = { 0x27, UsageKind::OTHER, "MICROPHONE_ENABLE" };
        constexpr Usage CINE = { 0x40, UsageKind::OTHER, "CINE" };
        constexpr Usage TRANSMIT_POWER = { 0x41, UsageKind::OTHER, "TRANSMIT_POWER" };
        constexpr Usage VOLUME = { 0x42, UsageKind::OTHER, "VOLUME" };
        constexpr Usage FOCUS = { 0x43, UsageKind::OTHER, "FOCUS" };
        constexpr Usage DEPTH = { 0x44, UsageKind::OTHER, "DEPTH" };
        constexpr Usage SOFT_STEP_PRIMARY = { 0x60, UsageKind::OTHER, "SOFT_STEP_PRIMARY" };
        constexpr Usage SOFT_STEP_SECONDARY = { 0x61, UsageKind::OTHER, "SOFT_STEP_SECONDARY" };
        constexpr Usage DEPTH_GAIN_COMPENSATION = { 0x70, UsageKind::OTHER, "DEPTH_GAIN_COMPENSATION" };
        constexpr Usage ZOOM_SELECT = { 0x80, UsageKind::OTHER, "ZOOM_SELECT" };
        constexpr Usage ZOOM_ADJUST = { 0x81, UsageKind::OTHER, "ZOOM_ADJUST" };
        constexpr Usage SPECTRAL_DOPPLER_MODE_SELECT = { 0x82, UsageKind::OTHER, "SPECTRAL_DOPPLER_MODE_SELECT" };
        constexpr Usage SPECTRAL_DOPPLER_ADJUST = { 0x83, UsageKind::OTHER, "SPECTRAL_DOPPLER_ADJUST" };
        constexpr Usage COLOR_DOPPLER_MODE_SELECT = { 0x84, UsageKind::OTHER, "COLOR_DOPPLER_MODE_SELECT" };
        constexpr Usage COLOR_DOPPLER_ADJUST = { 0x85, UsageKind::OTHER, "COLOR_DOPPLER_ADJUST" };
        constexpr Usage MOTION_MODE_SELECT = { 0x86, UsageKind::OTHER, "MOTION_MODE_SELECT" };
        constexpr Usage MOTION_MODE_ADJUST = { 0x87, UsageKind::OTHER, "MOTION_MODE_ADJUST" };
        constexpr Usage 2_D_MODE_SELECT = { 0x88, UsageKind::OTHER, "2_D_MODE_SELECT" };
        constexpr Usage 2_D_MODE_ADJUST = { 0x89, UsageKind::OTHER, "2_D_MODE_ADJUST" };
        constexpr Usage SOFT_CONTROL_SELECT = { 0xA0, UsageKind::OTHER, "SOFT_CONTROL_SELECT" };
        constexpr Usage SOFT_CONTROL_ADJUST = { 0xA1, UsageKind::OTHER, "SOFT_CONTROL_ADJUST" };
    }
}
