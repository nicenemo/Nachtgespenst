#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Eye and Head Trackers (0x12)
 */
namespace HID::EyeandHeadTrackers {
    constexpr uint8_t PAGE_ID = 0x12;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t EYE_TRACKER = 0x01;
    
    constexpr uint8_t HEAD_TRACKER = 0x02;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t CAPABILITIES = 0x11;
    
    constexpr uint8_t CONFIGURATION = 0x12;
    
    constexpr uint8_t STATUS = 0x13;
    
    constexpr uint8_t CONTROL = 0x14;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t TRACKING_DATA = 0x10;
    
    constexpr uint8_t GAZE_POINT = 0x24;
    
    constexpr uint8_t LEFT_EYE_POSITION = 0x25;
    
    constexpr uint8_t RIGHT_EYE_POSITION = 0x26;
    
    constexpr uint8_t HEAD_POSITION = 0x27;
    
    constexpr uint8_t HEAD_DIRECTION_POINT = 0x28;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t SENSOR_TIMESTAMP = 0x20;
    
    constexpr uint8_t POSITION_X = 0x21;
    
    constexpr uint8_t POSITION_Y = 0x22;
    
    constexpr uint8_t POSITION_Z = 0x23;
    
    constexpr uint8_t ROTATION_ABOUT_X_AXIS = 0x29;
    
    constexpr uint8_t ROTATION_ABOUT_Y_AXIS = 0x2A;
    
    constexpr uint8_t ROTATION_ABOUT_Z_AXIS = 0x2B;
    
    constexpr uint8_t SAMPLING_FREQUENCY = 0x300;
    
    constexpr uint8_t CONFIGURATION_STATUS = 0x301;
    
    constexpr uint8_t DEVICE_MODE_REQUEST = 0x400;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t TRACKER_QUALITY = 0x100;
    
    constexpr uint8_t MINIMUM_TRACKING_DISTANCE = 0x101;
    
    constexpr uint8_t OPTIMUM_TRACKING_DISTANCE = 0x102;
    
    constexpr uint8_t MAXIMUM_TRACKING_DISTANCE = 0x103;
    
    constexpr uint8_t MAXIMUM_SCREEN_PLANE_WIDTH = 0x104;
    
    constexpr uint8_t MAXIMUM_SCREEN_PLANE_HEIGHT = 0x105;
    
    constexpr uint8_t DISPLAY_MANUFACTURER_ID = 0x200;
    
    constexpr uint8_t DISPLAY_PRODUCT_ID = 0x201;
    
    constexpr uint8_t DISPLAY_SERIAL_NUMBER = 0x202;
    
    constexpr uint8_t DISPLAY_MANUFACTURER_DATE = 0x203;
    
    constexpr uint8_t CALIBRATED_SCREEN_WIDTH = 0x204;
    
    constexpr uint8_t CALIBRATED_SCREEN_HEIGHT = 0x205;

}
