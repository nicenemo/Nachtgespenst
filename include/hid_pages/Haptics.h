#pragma once
#include "../HID_Types.h"

namespace HID::Haptics {
    constexpr uint16_t PAGE_ID = 0x000E;
    namespace OTHER {
        constexpr Usage SIMPLE_HAPTIC_CONTROLLER = { 0x01, UsageKind::OTHER, "SIMPLE_HAPTIC_CONTROLLER" };
        constexpr Usage WAVEFORM_LIST = { 0x10, UsageKind::OTHER, "WAVEFORM_LIST" };
        constexpr Usage DURATION_LIST = { 0x11, UsageKind::OTHER, "DURATION_LIST" };
        constexpr Usage AUTO_TRIGGER = { 0x20, UsageKind::OTHER, "AUTO_TRIGGER" };
        constexpr Usage MANUAL_TRIGGER = { 0x21, UsageKind::OTHER, "MANUAL_TRIGGER" };
        constexpr Usage AUTO_TRIGGER_ASSOCIATED_CONTROL = { 0x22, UsageKind::OTHER, "AUTO_TRIGGER_ASSOCIATED_CONTROL" };
        constexpr Usage INTENSITY = { 0x23, UsageKind::OTHER, "INTENSITY" };
        constexpr Usage REPEAT_COUNT = { 0x24, UsageKind::OTHER, "REPEAT_COUNT" };
        constexpr Usage RETRIGGER_PERIOD = { 0x25, UsageKind::OTHER, "RETRIGGER_PERIOD" };
        constexpr Usage WAVEFORM_VENDOR_PAGE = { 0x26, UsageKind::OTHER, "WAVEFORM_VENDOR_PAGE" };
        constexpr Usage WAVEFORM_VENDOR_ID = { 0x27, UsageKind::OTHER, "WAVEFORM_VENDOR_ID" };
        constexpr Usage WAVEFORM_CUTOFF_TIME = { 0x28, UsageKind::OTHER, "WAVEFORM_CUTOFF_TIME" };
        constexpr Usage WAVEFORM_NONE = { 0x1001, UsageKind::OTHER, "WAVEFORM_NONE" };
        constexpr Usage WAVEFORM_STOP = { 0x1002, UsageKind::OTHER, "WAVEFORM_STOP" };
        constexpr Usage WAVEFORM_CLICK = { 0x1003, UsageKind::OTHER, "WAVEFORM_CLICK" };
        constexpr Usage WAVEFORM_BUZZ_CONTINUOUS = { 0x1004, UsageKind::OTHER, "WAVEFORM_BUZZ_CONTINUOUS" };
        constexpr Usage WAVEFORM_RUMBLE_CONTINUOUS = { 0x1005, UsageKind::OTHER, "WAVEFORM_RUMBLE_CONTINUOUS" };
        constexpr Usage WAVEFORM_PRESS = { 0x1006, UsageKind::OTHER, "WAVEFORM_PRESS" };
        constexpr Usage WAVEFORM_RELEASE = { 0x1007, UsageKind::OTHER, "WAVEFORM_RELEASE" };
        constexpr Usage WAVEFORM_HOVER = { 0x1008, UsageKind::OTHER, "WAVEFORM_HOVER" };
        constexpr Usage WAVEFORM_SUCCESS = { 0x1009, UsageKind::OTHER, "WAVEFORM_SUCCESS" };
        constexpr Usage WAVEFORM_ERROR = { 0x100A, UsageKind::OTHER, "WAVEFORM_ERROR" };
        constexpr Usage WAVEFORM_INK_CONTINUOUS = { 0x100B, UsageKind::OTHER, "WAVEFORM_INK_CONTINUOUS" };
        constexpr Usage WAVEFORM_PENCIL_CONTINUOUS = { 0x100C, UsageKind::OTHER, "WAVEFORM_PENCIL_CONTINUOUS" };
        constexpr Usage WAVEFORM_MARKER_CONTINUOUS = { 0x100D, UsageKind::OTHER, "WAVEFORM_MARKER_CONTINUOUS" };
        constexpr Usage WAVEFORM_CHISEL_MARKER_CONTINUOUS = { 0x100E, UsageKind::OTHER, "WAVEFORM_CHISEL_MARKER_CONTINUOUS" };
        constexpr Usage WAVEFORM_BRUSH_CONTINUOUS = { 0x100F, UsageKind::OTHER, "WAVEFORM_BRUSH_CONTINUOUS" };
        constexpr Usage WAVEFORM_ERASER_CONTINUOUS = { 0x1010, UsageKind::OTHER, "WAVEFORM_ERASER_CONTINUOUS" };
        constexpr Usage WAVEFORM_SPARKLE_CONTINUOUS = { 0x1011, UsageKind::OTHER, "WAVEFORM_SPARKLE_CONTINUOUS" };
    }
}
