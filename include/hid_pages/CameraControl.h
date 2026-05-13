#pragma once
#include "../HID_Types.h"

namespace HID::CameraControl {
    constexpr uint8_t PAGE_ID = 0x0090;
    namespace OSC {
        constexpr Usage CAMERA_AUTO_FOCUS = { 0x20, UsageKind::OSC, "CAMERA_AUTO_FOCUS" };
        constexpr Usage CAMERA_SHUTTER = { 0x21, UsageKind::OSC, "CAMERA_SHUTTER" };
    }
}
