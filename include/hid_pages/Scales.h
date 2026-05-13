#pragma once
#include "../HID_Types.h"

namespace HID::Scales {
    constexpr uint8_t PAGE_ID = 0x008D;
    namespace OTHER {
        constexpr Usage SCALES = { 0x01, UsageKind::OTHER, "SCALES" };
        constexpr Usage SCALE_DEVICE = { 0x20, UsageKind::OTHER, "SCALE_DEVICE" };
        constexpr Usage SCALE_CLASS = { 0x21, UsageKind::OTHER, "SCALE_CLASS" };
        constexpr Usage SCALE_CLASS_I_METRIC = { 0x22, UsageKind::OTHER, "SCALE_CLASS_I_METRIC" };
        constexpr Usage SCALE_CLASS_II_METRIC = { 0x23, UsageKind::OTHER, "SCALE_CLASS_II_METRIC" };
        constexpr Usage SCALE_CLASS_III_METRIC = { 0x24, UsageKind::OTHER, "SCALE_CLASS_III_METRIC" };
        constexpr Usage SCALE_CLASS_IIIL_METRIC = { 0x25, UsageKind::OTHER, "SCALE_CLASS_IIIL_METRIC" };
        constexpr Usage SCALE_CLASS_IV_METRIC = { 0x26, UsageKind::OTHER, "SCALE_CLASS_IV_METRIC" };
        constexpr Usage SCALE_CLASS_III_ENGLISH = { 0x27, UsageKind::OTHER, "SCALE_CLASS_III_ENGLISH" };
        constexpr Usage SCALE_CLASS_IIIL_ENGLISH = { 0x28, UsageKind::OTHER, "SCALE_CLASS_IIIL_ENGLISH" };
        constexpr Usage SCALE_CLASS_IV_ENGLISH = { 0x29, UsageKind::OTHER, "SCALE_CLASS_IV_ENGLISH" };
        constexpr Usage SCALE_CLASS_GENERIC = { 0x2A, UsageKind::OTHER, "SCALE_CLASS_GENERIC" };
        constexpr Usage SCALE_ATTRIBUTE_REPORT = { 0x30, UsageKind::OTHER, "SCALE_ATTRIBUTE_REPORT" };
        constexpr Usage SCALE_CONTROL_REPORT = { 0x31, UsageKind::OTHER, "SCALE_CONTROL_REPORT" };
        constexpr Usage SCALE_DATA_REPORT = { 0x32, UsageKind::OTHER, "SCALE_DATA_REPORT" };
        constexpr Usage SCALE_STATUS_REPORT = { 0x33, UsageKind::OTHER, "SCALE_STATUS_REPORT" };
        constexpr Usage SCALE_WEIGHT_LIMIT_REPORT = { 0x34, UsageKind::OTHER, "SCALE_WEIGHT_LIMIT_REPORT" };
        constexpr Usage SCALE_STATISTICS_REPORT = { 0x35, UsageKind::OTHER, "SCALE_STATISTICS_REPORT" };
        constexpr Usage DATA_WEIGHT = { 0x40, UsageKind::OTHER, "DATA_WEIGHT" };
        constexpr Usage DATA_SCALING = { 0x41, UsageKind::OTHER, "DATA_SCALING" };
        constexpr Usage WEIGHT_UNIT = { 0x50, UsageKind::OTHER, "WEIGHT_UNIT" };
        constexpr Usage WEIGHT_UNIT_MILLIGRAM = { 0x51, UsageKind::OTHER, "WEIGHT_UNIT_MILLIGRAM" };
        constexpr Usage WEIGHT_UNIT_GRAM = { 0x52, UsageKind::OTHER, "WEIGHT_UNIT_GRAM" };
        constexpr Usage WEIGHT_UNIT_KILOGRAM = { 0x53, UsageKind::OTHER, "WEIGHT_UNIT_KILOGRAM" };
        constexpr Usage WEIGHT_UNIT_CARATS = { 0x54, UsageKind::OTHER, "WEIGHT_UNIT_CARATS" };
        constexpr Usage WEIGHT_UNIT_TAELS = { 0x55, UsageKind::OTHER, "WEIGHT_UNIT_TAELS" };
        constexpr Usage WEIGHT_UNIT_GRAINS = { 0x56, UsageKind::OTHER, "WEIGHT_UNIT_GRAINS" };
        constexpr Usage WEIGHT_UNIT_PENNYWEIGHTS = { 0x57, UsageKind::OTHER, "WEIGHT_UNIT_PENNYWEIGHTS" };
        constexpr Usage WEIGHT_UNIT_METRIC_TON = { 0x58, UsageKind::OTHER, "WEIGHT_UNIT_METRIC_TON" };
        constexpr Usage WEIGHT_UNIT_AVOIR_TON = { 0x59, UsageKind::OTHER, "WEIGHT_UNIT_AVOIR_TON" };
        constexpr Usage WEIGHT_UNIT_TROY_OUNCE = { 0x5A, UsageKind::OTHER, "WEIGHT_UNIT_TROY_OUNCE" };
        constexpr Usage WEIGHT_UNIT_OUNCE = { 0x5B, UsageKind::OTHER, "WEIGHT_UNIT_OUNCE" };
        constexpr Usage WEIGHT_UNIT_POUND = { 0x5C, UsageKind::OTHER, "WEIGHT_UNIT_POUND" };
        constexpr Usage CALIBRATION_COUNT = { 0x60, UsageKind::OTHER, "CALIBRATION_COUNT" };
        constexpr Usage RE_ZERO_COUNT = { 0x61, UsageKind::OTHER, "RE_ZERO_COUNT" };
        constexpr Usage SCALE_STATUS = { 0x70, UsageKind::OTHER, "SCALE_STATUS" };
        constexpr Usage SCALE_STATUS_FAULT = { 0x71, UsageKind::OTHER, "SCALE_STATUS_FAULT" };
        constexpr Usage SCALE_STATUS_STABLE_AT_CENTER_OF_ZERO = { 0x72, UsageKind::OTHER, "SCALE_STATUS_STABLE_AT_CENTER_OF_ZERO" };
        constexpr Usage SCALE_STATUS_IN_MOTION = { 0x73, UsageKind::OTHER, "SCALE_STATUS_IN_MOTION" };
        constexpr Usage SCALE_STATUS_WEIGHT_STABLE = { 0x74, UsageKind::OTHER, "SCALE_STATUS_WEIGHT_STABLE" };
        constexpr Usage SCALE_STATUS_UNDER_ZERO = { 0x75, UsageKind::OTHER, "SCALE_STATUS_UNDER_ZERO" };
        constexpr Usage SCALE_STATUS_OVER_WEIGHT_LIMIT = { 0x76, UsageKind::OTHER, "SCALE_STATUS_OVER_WEIGHT_LIMIT" };
        constexpr Usage SCALE_STATUS_REQUIRES_CALIBRATION = { 0x77, UsageKind::OTHER, "SCALE_STATUS_REQUIRES_CALIBRATION" };
        constexpr Usage SCALE_STATUS_REQUIRES_REZEROING = { 0x78, UsageKind::OTHER, "SCALE_STATUS_REQUIRES_REZEROING" };
        constexpr Usage ZERO_SCALE = { 0x80, UsageKind::OTHER, "ZERO_SCALE" };
        constexpr Usage ENFORCED_ZERO_RETURN = { 0x81, UsageKind::OTHER, "ENFORCED_ZERO_RETURN" };
    }
}
