#pragma once
#include "../HID_Types.h"

namespace HID::VRControls {
    constexpr uint8_t PAGE_ID = 0x0003;
    namespace OTHER {
        constexpr Usage BELT = { 0x01, UsageKind::OTHER, "BELT" };
        constexpr Usage BODY_SUIT = { 0x02, UsageKind::OTHER, "BODY_SUIT" };
        constexpr Usage FLEXOR = { 0x03, UsageKind::OTHER, "FLEXOR" };
        constexpr Usage GLOVE = { 0x04, UsageKind::OTHER, "GLOVE" };
        constexpr Usage HEAD_TRACKER = { 0x05, UsageKind::OTHER, "HEAD_TRACKER" };
        constexpr Usage HEAD_MOUNTED_DISPLAY = { 0x06, UsageKind::OTHER, "HEAD_MOUNTED_DISPLAY" };
        constexpr Usage HAND_TRACKER = { 0x07, UsageKind::OTHER, "HAND_TRACKER" };
        constexpr Usage OCULOMETER = { 0x08, UsageKind::OTHER, "OCULOMETER" };
        constexpr Usage VEST = { 0x09, UsageKind::OTHER, "VEST" };
        constexpr Usage ANIMATRONIC_DEVICE = { 0x0A, UsageKind::OTHER, "ANIMATRONIC_DEVICE" };
        constexpr Usage STEREO_ENABLE = { 0x20, UsageKind::OTHER, "STEREO_ENABLE" };
        constexpr Usage DISPLAY_ENABLE = { 0x21, UsageKind::OTHER, "DISPLAY_ENABLE" };
    }
}
