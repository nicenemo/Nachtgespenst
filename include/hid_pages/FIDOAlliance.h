#pragma once
#include "../HID_Types.h"

namespace HID::FIDOAlliance {
    constexpr uint8_t PAGE_ID = 0xF1D0;
    namespace OTHER {
        constexpr Usage U2F_AUTHENTICATOR_DEVICE = { 0x01, UsageKind::OTHER, "U2F_AUTHENTICATOR_DEVICE" };
        constexpr Usage INPUT_REPORT_DATA = { 0x20, UsageKind::OTHER, "INPUT_REPORT_DATA" };
        constexpr Usage OUTPUT_REPORT_DATA = { 0x21, UsageKind::OTHER, "OUTPUT_REPORT_DATA" };
    }
}
