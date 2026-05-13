#pragma once
#include "../HID_Types.h"

namespace HID::Monitor {
    constexpr uint8_t PAGE_ID = 0x0080;
    namespace OTHER {
        constexpr Usage EDID_INFORMATION = { 0x02, UsageKind::OTHER, "EDID_INFORMATION" };
        constexpr Usage VDIF_INFORMATION = { 0x03, UsageKind::OTHER, "VDIF_INFORMATION" };
        constexpr Usage VESA_VERSION = { 0x04, UsageKind::OTHER, "VESA_VERSION" };
    }
    namespace CA {
        constexpr Usage MONITOR_CONTROL = { 0x01, UsageKind::CA, "MONITOR_CONTROL" };
    }
}
