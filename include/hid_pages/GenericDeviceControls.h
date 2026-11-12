#pragma once
#include "../HID_Types.h"

namespace HID::GenericDeviceControls {
    constexpr uint16_t PAGE_ID = 0x0006;
    namespace OTHER {
        constexpr Usage BACKGROUND_NONUSER_CONTROLS = { 0x01, UsageKind::OTHER, "BACKGROUND_NONUSER_CONTROLS" };
        constexpr Usage BATTERY_STRENGTH = { 0x20, UsageKind::OTHER, "BATTERY_STRENGTH" };
        constexpr Usage WIRELESS_CHANNEL = { 0x21, UsageKind::OTHER, "WIRELESS_CHANNEL" };
        constexpr Usage WIRELESS_ID = { 0x22, UsageKind::OTHER, "WIRELESS_ID" };
        constexpr Usage DISCOVER_WIRELESS_CONTROL = { 0x23, UsageKind::OTHER, "DISCOVER_WIRELESS_CONTROL" };
        constexpr Usage SECURITY_CODE_CHARACTER_ENTERED = { 0x24, UsageKind::OTHER, "SECURITY_CODE_CHARACTER_ENTERED" };
        constexpr Usage SECURITY_CODE_CHARACTER_ERASED = { 0x25, UsageKind::OTHER, "SECURITY_CODE_CHARACTER_ERASED" };
        constexpr Usage SECURITY_CODE_CLEARED = { 0x26, UsageKind::OTHER, "SECURITY_CODE_CLEARED" };
        constexpr Usage SEQUENCE_ID = { 0x27, UsageKind::OTHER, "SEQUENCE_ID" };
        constexpr Usage SEQUENCE_ID_RESET = { 0x28, UsageKind::OTHER, "SEQUENCE_ID_RESET" };
        constexpr Usage RF_SIGNAL_STRENGTH = { 0x29, UsageKind::OTHER, "RF_SIGNAL_STRENGTH" };
        constexpr Usage SOFTWARE_VERSION = { 0x2A, UsageKind::OTHER, "SOFTWARE_VERSION" };
        constexpr Usage PROTOCOL_VERSION = { 0x2B, UsageKind::OTHER, "PROTOCOL_VERSION" };
        constexpr Usage HARDWARE_VERSION = { 0x2C, UsageKind::OTHER, "HARDWARE_VERSION" };
        constexpr Usage MAJOR = { 0x2D, UsageKind::OTHER, "MAJOR" };
        constexpr Usage MINOR = { 0x2E, UsageKind::OTHER, "MINOR" };
        constexpr Usage REVISION = { 0x2F, UsageKind::OTHER, "REVISION" };
        constexpr Usage HANDEDNESS = { 0x30, UsageKind::OTHER, "HANDEDNESS" };
        constexpr Usage EITHER_HAND = { 0x31, UsageKind::OTHER, "EITHER_HAND" };
        constexpr Usage LEFT_HAND = { 0x32, UsageKind::OTHER, "LEFT_HAND" };
        constexpr Usage RIGHT_HAND = { 0x33, UsageKind::OTHER, "RIGHT_HAND" };
        constexpr Usage BOTH_HANDS = { 0x34, UsageKind::OTHER, "BOTH_HANDS" };
        constexpr Usage GRIP_POSE_OFFSET = { 0x40, UsageKind::OTHER, "GRIP_POSE_OFFSET" };
        constexpr Usage POINTER_POSE_OFFSET = { 0x41, UsageKind::OTHER, "POINTER_POSE_OFFSET" };
    }
}
