#pragma once
#include "../HID_Types.h"

namespace HID::CameraControl {
    constexpr uint16_t PAGE_ID = 0x0090;
    namespace OTHER {
        constexpr Usage CAMERA_AUTO_FOCUS = { 0x20, UsageKind::OTHER, "CAMERA_AUTO_FOCUS" };
        constexpr Usage CAMERA_SHUTTER = { 0x21, UsageKind::OTHER, "CAMERA_SHUTTER" };
    }
}
