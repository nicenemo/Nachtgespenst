#pragma once
#include "../HID_Types.h"

namespace HID::GenericDeviceControls {
    constexpr uint8_t PAGE_ID = 0x0006;
    namespace OTHER {
        constexpr Usage SEQUENCE_ID_RESET = { 0x28, UsageKind::OTHER, "SEQUENCE_ID_RESET" };
        constexpr Usage MAJOR = { 0x2D, UsageKind::OTHER, "MAJOR" };
        constexpr Usage MINOR = { 0x2E, UsageKind::OTHER, "MINOR" };
        constexpr Usage REVISION = { 0x2F, UsageKind::OTHER, "REVISION" };
        constexpr Usage HANDEDNESS = { 0x30, UsageKind::OTHER, "HANDEDNESS" };
    }
    namespace DV {
        constexpr Usage BATTERY_STRENGTH = { 0x20, UsageKind::DV, "BATTERY_STRENGTH" };
        constexpr Usage WIRELESS_CHANNEL = { 0x21, UsageKind::DV, "WIRELESS_CHANNEL" };
        constexpr Usage WIRELESS_ID = { 0x22, UsageKind::DV, "WIRELESS_ID" };
        constexpr Usage SEQUENCE_ID = { 0x27, UsageKind::DV, "SEQUENCE_ID" };
        constexpr Usage RF_SIGNAL_STRENGTH = { 0x29, UsageKind::DV, "RF_SIGNAL_STRENGTH" };
    }
    namespace CL {
        constexpr Usage SOFTWARE_VERSION = { 0x2A, UsageKind::CL, "SOFTWARE_VERSION" };
        constexpr Usage PROTOCOL_VERSION = { 0x2B, UsageKind::CL, "PROTOCOL_VERSION" };
        constexpr Usage HARDWARE_VERSION = { 0x2C, UsageKind::CL, "HARDWARE_VERSION" };
    }
    namespace Sel {
        constexpr Usage EITHER_HAND = { 0x31, UsageKind::Sel, "EITHER_HAND" };
        constexpr Usage LEFT_HAND = { 0x32, UsageKind::Sel, "LEFT_HAND" };
        constexpr Usage RIGHT_HAND = { 0x33, UsageKind::Sel, "RIGHT_HAND" };
        constexpr Usage BOTH_HANDS = { 0x34, UsageKind::Sel, "BOTH_HANDS" };
    }
    namespace CP {
        constexpr Usage GRIP_POSE_OFFSET = { 0x40, UsageKind::CP, "GRIP_POSE_OFFSET" };
        constexpr Usage POINTER_POSE_OFFSET = { 0x41, UsageKind::CP, "POINTER_POSE_OFFSET" };
    }
    namespace OSC {
        constexpr Usage DISCOVER_WIRELESS_CONTROL = { 0x23, UsageKind::OSC, "DISCOVER_WIRELESS_CONTROL" };
        constexpr Usage SECURITY_CODE_CHARACTER_ENTERED = { 0x24, UsageKind::OSC, "SECURITY_CODE_CHARACTER_ENTERED" };
        constexpr Usage SECURITY_CODE_CHARACTER_ERASED = { 0x25, UsageKind::OSC, "SECURITY_CODE_CHARACTER_ERASED" };
        constexpr Usage SECURITY_CODE_CLEARED = { 0x26, UsageKind::OSC, "SECURITY_CODE_CLEARED" };
    }
    namespace CA {
        constexpr Usage BACKGROUND_NONUSER_CONTROLS = { 0x01, UsageKind::CA, "BACKGROUND_NONUSER_CONTROLS" };
    }
}
