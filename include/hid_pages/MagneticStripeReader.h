#pragma once
#include "../HID_Types.h"

namespace HID::MagneticStripeReader {
    constexpr uint16_t PAGE_ID = 0x008E;
    namespace OTHER {
        constexpr Usage MSR_DEVICE_READ_ONLY = { 0x01, UsageKind::OTHER, "MSR_DEVICE_READ_ONLY" };
        constexpr Usage TRACK_1_LENGTH = { 0x11, UsageKind::OTHER, "TRACK_1_LENGTH" };
        constexpr Usage TRACK_2_LENGTH = { 0x12, UsageKind::OTHER, "TRACK_2_LENGTH" };
        constexpr Usage TRACK_3_LENGTH = { 0x13, UsageKind::OTHER, "TRACK_3_LENGTH" };
        constexpr Usage TRACK_JIS_LENGTH = { 0x14, UsageKind::OTHER, "TRACK_JIS_LENGTH" };
        constexpr Usage TRACK_DATA = { 0x20, UsageKind::OTHER, "TRACK_DATA" };
        constexpr Usage TRACK_1_DATA = { 0x21, UsageKind::OTHER, "TRACK_1_DATA" };
        constexpr Usage TRACK_2_DATA = { 0x22, UsageKind::OTHER, "TRACK_2_DATA" };
        constexpr Usage TRACK_3_DATA = { 0x23, UsageKind::OTHER, "TRACK_3_DATA" };
        constexpr Usage TRACK_JIS_DATA = { 0x24, UsageKind::OTHER, "TRACK_JIS_DATA" };
    }
}
