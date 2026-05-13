#pragma once
#include "../HID_Types.h"

namespace HID::VRControls {
    constexpr uint8_t PAGE_ID = 0x0003;
    namespace OOC {
        constexpr Usage STEREO_ENABLE = { 0x20, UsageKind::OOC, "STEREO_ENABLE" };
        constexpr Usage DISPLAY_ENABLE = { 0x21, UsageKind::OOC, "DISPLAY_ENABLE" };
    }
    namespace CP {
        constexpr Usage FLEXOR = { 0x03, UsageKind::CP, "FLEXOR" };
        constexpr Usage HEAD_TRACKER = { 0x05, UsageKind::CP, "HEAD_TRACKER" };
    }
    namespace CA {
        constexpr Usage BELT = { 0x01, UsageKind::CA, "BELT" };
        constexpr Usage BODY_SUIT = { 0x02, UsageKind::CA, "BODY_SUIT" };
        constexpr Usage GLOVE = { 0x04, UsageKind::CA, "GLOVE" };
        constexpr Usage HEAD_MOUNTED_DISPLAY = { 0x06, UsageKind::CA, "HEAD_MOUNTED_DISPLAY" };
        constexpr Usage HAND_TRACKER = { 0x07, UsageKind::CA, "HAND_TRACKER" };
        constexpr Usage OCULOMETER = { 0x08, UsageKind::CA, "OCULOMETER" };
        constexpr Usage VEST = { 0x09, UsageKind::CA, "VEST" };
        constexpr Usage ANIMATRONIC_DEVICE = { 0x0A, UsageKind::CA, "ANIMATRONIC_DEVICE" };
    }
}
