#pragma once
#include "../HID_Types.h"

namespace HID::EyeandHeadTrackers {
    constexpr uint16_t PAGE_ID = 0x0012;
    namespace OTHER {
        constexpr Usage EYE_TRACKER = { 0x01, UsageKind::OTHER, "EYE_TRACKER" };
        constexpr Usage HEAD_TRACKER = { 0x02, UsageKind::OTHER, "HEAD_TRACKER" };
        constexpr Usage TRACKING_DATA = { 0x10, UsageKind::OTHER, "TRACKING_DATA" };
        constexpr Usage CAPABILITIES = { 0x11, UsageKind::OTHER, "CAPABILITIES" };
        constexpr Usage CONFIGURATION = { 0x12, UsageKind::OTHER, "CONFIGURATION" };
        constexpr Usage STATUS = { 0x13, UsageKind::OTHER, "STATUS" };
        constexpr Usage CONTROL = { 0x14, UsageKind::OTHER, "CONTROL" };
        constexpr Usage SENSOR_TIMESTAMP = { 0x20, UsageKind::OTHER, "SENSOR_TIMESTAMP" };
        constexpr Usage POSITION_X = { 0x21, UsageKind::OTHER, "POSITION_X" };
        constexpr Usage POSITION_Y = { 0x22, UsageKind::OTHER, "POSITION_Y" };
        constexpr Usage POSITION_Z = { 0x23, UsageKind::OTHER, "POSITION_Z" };
        constexpr Usage GAZE_POINT = { 0x24, UsageKind::OTHER, "GAZE_POINT" };
        constexpr Usage LEFT_EYE_POSITION = { 0x25, UsageKind::OTHER, "LEFT_EYE_POSITION" };
        constexpr Usage RIGHT_EYE_POSITION = { 0x26, UsageKind::OTHER, "RIGHT_EYE_POSITION" };
        constexpr Usage HEAD_POSITION = { 0x27, UsageKind::OTHER, "HEAD_POSITION" };
        constexpr Usage HEAD_DIRECTION_POINT = { 0x28, UsageKind::OTHER, "HEAD_DIRECTION_POINT" };
        constexpr Usage ROTATION_ABOUT_X_AXIS = { 0x29, UsageKind::OTHER, "ROTATION_ABOUT_X_AXIS" };
        constexpr Usage ROTATION_ABOUT_Y_AXIS = { 0x2A, UsageKind::OTHER, "ROTATION_ABOUT_Y_AXIS" };
        constexpr Usage ROTATION_ABOUT_Z_AXIS = { 0x2B, UsageKind::OTHER, "ROTATION_ABOUT_Z_AXIS" };
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
        constexpr Usage SAMPLING_FREQUENCY = { 0x300, UsageKind::OTHER, "SAMPLING_FREQUENCY" };
        constexpr Usage CONFIGURATION_STATUS = { 0x301, UsageKind::OTHER, "CONFIGURATION_STATUS" };
        constexpr Usage DEVICE_MODE_REQUEST = { 0x400, UsageKind::OTHER, "DEVICE_MODE_REQUEST" };
    }
}
