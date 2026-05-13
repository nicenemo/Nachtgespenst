#pragma once
#include "../HID_Types.h"

namespace HID::Scales {
    constexpr uint8_t PAGE_ID = 0x008D;
    namespace OTHER {
        constexpr Usage SCALE_CLASS = { 0x21, UsageKind::OTHER, "SCALE_CLASS" };
        constexpr Usage SCALE_STATUS = { 0x70, UsageKind::OTHER, "SCALE_STATUS" };
    }
    namespace OOC {
        constexpr Usage ZERO_SCALE = { 0x80, UsageKind::OOC, "ZERO_SCALE" };
        constexpr Usage ENFORCED_ZERO_RETURN = { 0x81, UsageKind::OOC, "ENFORCED_ZERO_RETURN" };
    }
    namespace DV {
        constexpr Usage DATA_WEIGHT = { 0x40, UsageKind::DV, "DATA_WEIGHT" };
        constexpr Usage DATA_SCALING = { 0x41, UsageKind::DV, "DATA_SCALING" };
        constexpr Usage CALIBRATION_COUNT = { 0x60, UsageKind::DV, "CALIBRATION_COUNT" };
        constexpr Usage RE_ZERO_COUNT = { 0x61, UsageKind::DV, "RE_ZERO_COUNT" };
    }
    namespace CL {
        constexpr Usage SCALE_DEVICE = { 0x20, UsageKind::CL, "SCALE_DEVICE" };
        constexpr Usage SCALE_ATTRIBUTE_REPORT = { 0x30, UsageKind::CL, "SCALE_ATTRIBUTE_REPORT" };
        constexpr Usage SCALE_CONTROL_REPORT = { 0x31, UsageKind::CL, "SCALE_CONTROL_REPORT" };
        constexpr Usage SCALE_DATA_REPORT = { 0x32, UsageKind::CL, "SCALE_DATA_REPORT" };
        constexpr Usage SCALE_STATUS_REPORT = { 0x33, UsageKind::CL, "SCALE_STATUS_REPORT" };
        constexpr Usage SCALE_WEIGHT_LIMIT_REPORT = { 0x34, UsageKind::CL, "SCALE_WEIGHT_LIMIT_REPORT" };
        constexpr Usage SCALE_STATISTICS_REPORT = { 0x35, UsageKind::CL, "SCALE_STATISTICS_REPORT" };
        constexpr Usage WEIGHT_UNIT = { 0x50, UsageKind::CL, "WEIGHT_UNIT" };
    }
    namespace Sel {
        constexpr Usage SCALE_CLASS_I_METRIC = { 0x22, UsageKind::Sel, "SCALE_CLASS_I_METRIC" };
        constexpr Usage SCALE_CLASS_II_METRIC = { 0x23, UsageKind::Sel, "SCALE_CLASS_II_METRIC" };
        constexpr Usage SCALE_CLASS_III_METRIC = { 0x24, UsageKind::Sel, "SCALE_CLASS_III_METRIC" };
        constexpr Usage SCALE_CLASS_IIIL_METRIC = { 0x25, UsageKind::Sel, "SCALE_CLASS_IIIL_METRIC" };
        constexpr Usage SCALE_CLASS_IV_METRIC = { 0x26, UsageKind::Sel, "SCALE_CLASS_IV_METRIC" };
        constexpr Usage SCALE_CLASS_III_ENGLISH = { 0x27, UsageKind::Sel, "SCALE_CLASS_III_ENGLISH" };
        constexpr Usage SCALE_CLASS_IIIL_ENGLISH = { 0x28, UsageKind::Sel, "SCALE_CLASS_IIIL_ENGLISH" };
        constexpr Usage SCALE_CLASS_IV_ENGLISH = { 0x29, UsageKind::Sel, "SCALE_CLASS_IV_ENGLISH" };
        constexpr Usage SCALE_CLASS_GENERIC = { 0x2A, UsageKind::Sel, "SCALE_CLASS_GENERIC" };
        constexpr Usage WEIGHT_UNIT_MILLIGRAM = { 0x51, UsageKind::Sel, "WEIGHT_UNIT_MILLIGRAM" };
        constexpr Usage WEIGHT_UNIT_GRAM = { 0x52, UsageKind::Sel, "WEIGHT_UNIT_GRAM" };
        constexpr Usage WEIGHT_UNIT_KILOGRAM = { 0x53, UsageKind::Sel, "WEIGHT_UNIT_KILOGRAM" };
        constexpr Usage WEIGHT_UNIT_CARATS = { 0x54, UsageKind::Sel, "WEIGHT_UNIT_CARATS" };
        constexpr Usage WEIGHT_UNIT_TAELS = { 0x55, UsageKind::Sel, "WEIGHT_UNIT_TAELS" };
        constexpr Usage WEIGHT_UNIT_GRAINS = { 0x56, UsageKind::Sel, "WEIGHT_UNIT_GRAINS" };
        constexpr Usage WEIGHT_UNIT_PENNYWEIGHTS = { 0x57, UsageKind::Sel, "WEIGHT_UNIT_PENNYWEIGHTS" };
        constexpr Usage WEIGHT_UNIT_METRIC_TON = { 0x58, UsageKind::Sel, "WEIGHT_UNIT_METRIC_TON" };
        constexpr Usage WEIGHT_UNIT_AVOIR_TON = { 0x59, UsageKind::Sel, "WEIGHT_UNIT_AVOIR_TON" };
        constexpr Usage WEIGHT_UNIT_TROY_OUNCE = { 0x5A, UsageKind::Sel, "WEIGHT_UNIT_TROY_OUNCE" };
        constexpr Usage WEIGHT_UNIT_OUNCE = { 0x5B, UsageKind::Sel, "WEIGHT_UNIT_OUNCE" };
        constexpr Usage WEIGHT_UNIT_POUND = { 0x5C, UsageKind::Sel, "WEIGHT_UNIT_POUND" };
        constexpr Usage SCALE_STATUS_FAULT = { 0x71, UsageKind::Sel, "SCALE_STATUS_FAULT" };
        constexpr Usage SCALE_STATUS_STABLE_AT_CENTER_OF_ZERO = { 0x72, UsageKind::Sel, "SCALE_STATUS_STABLE_AT_CENTER_OF_ZERO" };
        constexpr Usage SCALE_STATUS_IN_MOTION = { 0x73, UsageKind::Sel, "SCALE_STATUS_IN_MOTION" };
        constexpr Usage SCALE_STATUS_WEIGHT_STABLE = { 0x74, UsageKind::Sel, "SCALE_STATUS_WEIGHT_STABLE" };
        constexpr Usage SCALE_STATUS_UNDER_ZERO = { 0x75, UsageKind::Sel, "SCALE_STATUS_UNDER_ZERO" };
        constexpr Usage SCALE_STATUS_OVER_WEIGHT_LIMIT = { 0x76, UsageKind::Sel, "SCALE_STATUS_OVER_WEIGHT_LIMIT" };
        constexpr Usage SCALE_STATUS_REQUIRES_CALIBRATION = { 0x77, UsageKind::Sel, "SCALE_STATUS_REQUIRES_CALIBRATION" };
        constexpr Usage SCALE_STATUS_REQUIRES_REZEROING = { 0x78, UsageKind::Sel, "SCALE_STATUS_REQUIRES_REZEROING" };
    }
    namespace CA {
        constexpr Usage SCALES = { 0x01, UsageKind::CA, "SCALES" };
    }
}
