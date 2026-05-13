#pragma once
#include "../HID_Types.h"

namespace HID::FIDOAlliance {
    constexpr uint8_t PAGE_ID = 0xF1D0;
    namespace DV {
        constexpr Usage INPUT_REPORT_DATA = { 0x20, UsageKind::DV, "INPUT_REPORT_DATA" };
        constexpr Usage OUTPUT_REPORT_DATA = { 0x21, UsageKind::DV, "OUTPUT_REPORT_DATA" };
    }
    namespace CA {
        constexpr Usage U2F_AUTHENTICATOR_DEVICE = { 0x01, UsageKind::CA, "U2F_AUTHENTICATOR_DEVICE" };
    }
}
