#pragma once
#include "../HID_Types.h"

namespace HID::EyeandHeadTrackers {
    constexpr uint8_t PAGE_ID = 0x0012;
    namespace OTHER {
        constexpr Usage TRACKER_QUALITY = { 0x100, UsageKind::OTHER, "TRACKER_QUALITY" };
        constexpr Usage MINIMUM_TRACKING_DISTANCE = { 0x101, UsageKind::OTHER, "MINIMUM_TRACKING_DISTANCE" };
        constexpr Usage OPTIMUM_TRACKING_DISTANCE = { 0x102, UsageKind::OTHER, "OPTIMUM_TRACKING_DISTANCE" };
        constexpr Usage MAXIMUM_TRACKING_DISTANCE = { 0x103, UsageKind::OTHER, "MAXIMUM_TRACKING_DISTANCE" };
        constexpr Usage MAXIMUM_SCREEN_PLANE_WIDTH = { 0x104, UsageKind::OTHER, "MAXIMUM_SCREEN_PLANE_WIDTH" };
        constexpr Usage MAXIMUM_SCREEN_PLANE_HEIGHT = { 0x105, UsageKind::OTHER, "MAXIMUM_SCREEN_PLANE_HEIGHT" };
        constexpr Usage DISPLAY_MANUFACTURER_ID = { 0x200, UsageKind::OTHER, "DISPLAY_MANUFACTURER_ID" };
        constexpr Usage DISPLAY_PRODUCT_ID = { 0x201, UsageKind::OTHER, "DISPLAY_PRODUCT_ID" };
        constexpr Usage DISPLAY_SERIAL_NUMBER = { 0x202, UsageKind::OTHER, "DISPLAY_SERIAL_NUMBER" };
        constexpr Usage DISPLAY_MANUFACTURER_DATE = { 0x203, UsageKind::OTHER, "DISPLAY_MANUFACTURER_DATE" };
        constexpr Usage CALIBRATED_SCREEN_WIDTH = { 0x204, UsageKind::OTHER, "CALIBRATED_SCREEN_WIDTH" };
        constexpr Usage CALIBRATED_SCREEN_HEIGHT = { 0x205, UsageKind::OTHER, "CALIBRATED_SCREEN_HEIGHT" };
    }
    namespace DV {
        constexpr Usage SENSOR_TIMESTAMP = { 0x20, UsageKind::DV, "SENSOR_TIMESTAMP" };
        constexpr Usage POSITION_X = { 0x21, UsageKind::DV, "POSITION_X" };
        constexpr Usage POSITION_Y = { 0x22, UsageKind::DV, "POSITION_Y" };
        constexpr Usage POSITION_Z = { 0x23, UsageKind::DV, "POSITION_Z" };
        constexpr Usage ROTATION_ABOUT_X_AXIS = { 0x29, UsageKind::DV, "ROTATION_ABOUT_X_AXIS" };
        constexpr Usage ROTATION_ABOUT_Y_AXIS = { 0x2A, UsageKind::DV, "ROTATION_ABOUT_Y_AXIS" };
        constexpr Usage ROTATION_ABOUT_Z_AXIS = { 0x2B, UsageKind::DV, "ROTATION_ABOUT_Z_AXIS" };
        constexpr Usage SAMPLING_FREQUENCY = { 0x300, UsageKind::DV, "SAMPLING_FREQUENCY" };
        constexpr Usage CONFIGURATION_STATUS = { 0x301, UsageKind::DV, "CONFIGURATION_STATUS" };
        constexpr Usage DEVICE_MODE_REQUEST = { 0x400, UsageKind::DV, "DEVICE_MODE_REQUEST" };
    }
    namespace CL {
        constexpr Usage CAPABILITIES = { 0x11, UsageKind::CL, "CAPABILITIES" };
        constexpr Usage CONFIGURATION = { 0x12, UsageKind::CL, "CONFIGURATION" };
        constexpr Usage STATUS = { 0x13, UsageKind::CL, "STATUS" };
        constexpr Usage CONTROL = { 0x14, UsageKind::CL, "CONTROL" };
    }
    namespace CP {
        constexpr Usage TRACKING_DATA = { 0x10, UsageKind::CP, "TRACKING_DATA" };
        constexpr Usage GAZE_POINT = { 0x24, UsageKind::CP, "GAZE_POINT" };
        constexpr Usage LEFT_EYE_POSITION = { 0x25, UsageKind::CP, "LEFT_EYE_POSITION" };
        constexpr Usage RIGHT_EYE_POSITION = { 0x26, UsageKind::CP, "RIGHT_EYE_POSITION" };
        constexpr Usage HEAD_POSITION = { 0x27, UsageKind::CP, "HEAD_POSITION" };
        constexpr Usage HEAD_DIRECTION_POINT = { 0x28, UsageKind::CP, "HEAD_DIRECTION_POINT" };
    }
    namespace CA {
        constexpr Usage EYE_TRACKER = { 0x01, UsageKind::CA, "EYE_TRACKER" };
        constexpr Usage HEAD_TRACKER = { 0x02, UsageKind::CA, "HEAD_TRACKER" };
    }
}
