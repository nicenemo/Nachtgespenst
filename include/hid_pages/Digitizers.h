#pragma once
#include "../HID_Types.h"

namespace HID::Digitizers {
    constexpr uint8_t PAGE_ID = 0x000D;
    namespace OTHER {
        constexpr Usage CONTACT_COUNT_MAXIMUM = { 0x55, UsageKind::OTHER, "CONTACT_COUNT_MAXIMUM" };
        constexpr Usage SURFACE_SWITCH = { 0x57, UsageKind::OTHER, "SURFACE_SWITCH" };
        constexpr Usage BUTTON_SWITCH = { 0x58, UsageKind::OTHER, "BUTTON_SWITCH" };
        constexpr Usage PAD_TYPE = { 0x59, UsageKind::OTHER, "PAD_TYPE" };
        constexpr Usage TRANSDUCER_SERIAL_NUMBER = { 0x5B, UsageKind::OTHER, "TRANSDUCER_SERIAL_NUMBER" };
        constexpr Usage LATENCY_MODE = { 0x60, UsageKind::OTHER, "LATENCY_MODE" };
        constexpr Usage GESTURE_CHARACTER_ENCODING = { 0x64, UsageKind::OTHER, "GESTURE_CHARACTER_ENCODING" };
        constexpr Usage CAPACITIVE_HEAT_MAP_PROTOCOL_VENDOR_ID = { 0x6A, UsageKind::OTHER, "CAPACITIVE_HEAT_MAP_PROTOCOL_VENDOR_ID" };
        constexpr Usage CAPACITIVE_HEAT_MAP_PROTOCOL_VERSION = { 0x6B, UsageKind::OTHER, "CAPACITIVE_HEAT_MAP_PROTOCOL_VERSION" };
        constexpr Usage GESTURE_CHARACTER_ENABLE = { 0x6D, UsageKind::OTHER, "GESTURE_CHARACTER_ENABLE" };
        constexpr Usage TRANSDUCER_SERIAL_NUMBER_PART_2 = { 0x6E, UsageKind::OTHER, "TRANSDUCER_SERIAL_NUMBER_PART_2" };
        constexpr Usage NO_PREFERRED_COLOR = { 0x6F, UsageKind::OTHER, "NO_PREFERRED_COLOR" };
        constexpr Usage PREFERRED_LINE_STYLE = { 0x70, UsageKind::OTHER, "PREFERRED_LINE_STYLE" };
        constexpr Usage DIGITIZER_ERROR = { 0x81, UsageKind::OTHER, "DIGITIZER_ERROR" };
        constexpr Usage TRANSDUCER_VENDOR_ID = { 0x91, UsageKind::OTHER, "TRANSDUCER_VENDOR_ID" };
        constexpr Usage TRANSDUCER_PRODUCT_ID = { 0x92, UsageKind::OTHER, "TRANSDUCER_PRODUCT_ID" };
        constexpr Usage DEVICE_SUPPORTED_PROTOCOLS = { 0x93, UsageKind::OTHER, "DEVICE_SUPPORTED_PROTOCOLS" };
        constexpr Usage TRANSDUCER_SUPPORTED_PROTOCOLS = { 0x94, UsageKind::OTHER, "TRANSDUCER_SUPPORTED_PROTOCOLS" };
        constexpr Usage SUPPORTED_REPORT_RATES = { 0xA0, UsageKind::OTHER, "SUPPORTED_REPORT_RATES" };
        constexpr Usage TRANSDUCER_CONNECTED = { 0xA2, UsageKind::OTHER, "TRANSDUCER_CONNECTED" };
    }
    namespace DV {
        constexpr Usage TIP_PRESSURE = { 0x30, UsageKind::DV, "TIP_PRESSURE" };
        constexpr Usage BARREL_PRESSURE = { 0x31, UsageKind::DV, "BARREL_PRESSURE" };
        constexpr Usage QUALITY = { 0x36, UsageKind::DV, "QUALITY" };
        constexpr Usage TRANSDUCER_INDEX = { 0x38, UsageKind::DV, "TRANSDUCER_INDEX" };
        constexpr Usage BATTERY_STRENGTH = { 0x3B, UsageKind::DV, "BATTERY_STRENGTH" };
        constexpr Usage X_TILT = { 0x3D, UsageKind::DV, "X_TILT" };
        constexpr Usage Y_TILT = { 0x3E, UsageKind::DV, "Y_TILT" };
        constexpr Usage AZIMUTH = { 0x3F, UsageKind::DV, "AZIMUTH" };
        constexpr Usage ALTITUDE = { 0x40, UsageKind::DV, "ALTITUDE" };
        constexpr Usage TWIST = { 0x41, UsageKind::DV, "TWIST" };
        constexpr Usage WIDTH = { 0x48, UsageKind::DV, "WIDTH" };
        constexpr Usage HEIGHT = { 0x49, UsageKind::DV, "HEIGHT" };
        constexpr Usage CONTACT_IDENTIFIER = { 0x51, UsageKind::DV, "CONTACT_IDENTIFIER" };
        constexpr Usage DEVICE_MODE = { 0x52, UsageKind::DV, "DEVICE_MODE" };
        constexpr Usage DEVICE_IDENTIFIER = { 0x53, UsageKind::DV, "DEVICE_IDENTIFIER" };
        constexpr Usage CONTACT_COUNT = { 0x54, UsageKind::DV, "CONTACT_COUNT" };
        constexpr Usage SCAN_TIME = { 0x56, UsageKind::DV, "SCAN_TIME" };
        constexpr Usage PREFERRED_COLOR = { 0x5C, UsageKind::DV, "PREFERRED_COLOR" };
        constexpr Usage PREFERRED_LINE_WIDTH = { 0x5E, UsageKind::DV, "PREFERRED_LINE_WIDTH" };
        constexpr Usage GESTURE_CHARACTER_QUALITY = { 0x61, UsageKind::DV, "GESTURE_CHARACTER_QUALITY" };
        constexpr Usage CHARACTER_GESTURE_DATA_LENGTH = { 0x62, UsageKind::DV, "CHARACTER_GESTURE_DATA_LENGTH" };
        constexpr Usage CHARACTER_GESTURE_DATA = { 0x63, UsageKind::DV, "CHARACTER_GESTURE_DATA" };
        constexpr Usage CAPACITIVE_HEAT_MAP_FRAME_DATA = { 0x6C, UsageKind::DV, "CAPACITIVE_HEAT_MAP_FRAME_DATA" };
        constexpr Usage REPORT_RATE = { 0xA1, UsageKind::DV, "REPORT_RATE" };
        constexpr Usage TRANSDUCER_INDEX_SELECTOR = { 0xA6, UsageKind::DV, "TRANSDUCER_INDEX_SELECTOR" };
        constexpr Usage BUTTON_PRESS_THRESHOLD = { 0xB0, UsageKind::DV, "BUTTON_PRESS_THRESHOLD" };
    }
    namespace MC {
        constexpr Usage IN_RANGE = { 0x32, UsageKind::MC, "IN_RANGE" };
        constexpr Usage TOUCH = { 0x33, UsageKind::MC, "TOUCH" };
        constexpr Usage DATA_VALID = { 0x37, UsageKind::MC, "DATA_VALID" };
        constexpr Usage INVERT = { 0x3C, UsageKind::MC, "INVERT" };
        constexpr Usage TIP_SWITCH = { 0x42, UsageKind::MC, "TIP_SWITCH" };
        constexpr Usage SECONDARY_TIP_SWITCH = { 0x43, UsageKind::MC, "SECONDARY_TIP_SWITCH" };
        constexpr Usage BARREL_SWITCH = { 0x44, UsageKind::MC, "BARREL_SWITCH" };
        constexpr Usage ERASER = { 0x45, UsageKind::MC, "ERASER" };
        constexpr Usage TABLET_PICK = { 0x46, UsageKind::MC, "TABLET_PICK" };
        constexpr Usage TOUCH_VALID = { 0x47, UsageKind::MC, "TOUCH_VALID" };
        constexpr Usage SECONDARY_BARREL_SWITCH = { 0x5A, UsageKind::MC, "SECONDARY_BARREL_SWITCH" };
        constexpr Usage PREFERRED_COLOR_IS_LOCKED = { 0x5D, UsageKind::MC, "PREFERRED_COLOR_IS_LOCKED" };
        constexpr Usage PREFERRED_LINE_WIDTH_IS_LOCKED = { 0x5F, UsageKind::MC, "PREFERRED_LINE_WIDTH_IS_LOCKED" };
        constexpr Usage PREFERRED_LINE_STYLE_IS_LOCKED = { 0x71, UsageKind::MC, "PREFERRED_LINE_STYLE_IS_LOCKED" };
    }
    namespace CL {
        constexpr Usage PUCK = { 0x21, UsageKind::CL, "PUCK" };
        constexpr Usage FINGER = { 0x22, UsageKind::CL, "FINGER" };
        constexpr Usage DEVICE_SETTINGS = { 0x23, UsageKind::CL, "DEVICE_SETTINGS" };
        constexpr Usage CHARACTER_GESTURE = { 0x24, UsageKind::CL, "CHARACTER_GESTURE" };
        constexpr Usage TABLET_FUNCTION_KEYS = { 0x39, UsageKind::CL, "TABLET_FUNCTION_KEYS" };
        constexpr Usage PROGRAM_CHANGE_KEYS = { 0x3A, UsageKind::CL, "PROGRAM_CHANGE_KEYS" };
        constexpr Usage DIGITIZER_DIAGNOSTIC = { 0x80, UsageKind::CL, "DIGITIZER_DIAGNOSTIC" };
        constexpr Usage TRANSDUCER_SOFTWARE_INFO = { 0x90, UsageKind::CL, "TRANSDUCER_SOFTWARE_INFO" };
        constexpr Usage TRANSDUCER_SWITCHES = { 0xA5, UsageKind::CL, "TRANSDUCER_SWITCHES" };
    }
    namespace Sel {
        constexpr Usage UTF8_CHARACTER_GESTURE_ENCODING = { 0x65, UsageKind::Sel, "UTF8_CHARACTER_GESTURE_ENCODING" };
        constexpr Usage UTF16_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING = { 0x66, UsageKind::Sel, "UTF16_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING" };
        constexpr Usage UTF16_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING = { 0x67, UsageKind::Sel, "UTF16_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING" };
        constexpr Usage UTF32_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING = { 0x68, UsageKind::Sel, "UTF32_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING" };
        constexpr Usage UTF32_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING = { 0x69, UsageKind::Sel, "UTF32_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING" };
        constexpr Usage INK = { 0x72, UsageKind::Sel, "INK" };
        constexpr Usage PENCIL = { 0x73, UsageKind::Sel, "PENCIL" };
        constexpr Usage HIGHLIGHTER = { 0x74, UsageKind::Sel, "HIGHLIGHTER" };
        constexpr Usage CHISEL_MARKER = { 0x75, UsageKind::Sel, "CHISEL_MARKER" };
        constexpr Usage BRUSH = { 0x76, UsageKind::Sel, "BRUSH" };
        constexpr Usage NO_PREFERENCE = { 0x77, UsageKind::Sel, "NO_PREFERENCE" };
        constexpr Usage ERR_NORMAL_STATUS = { 0x82, UsageKind::Sel, "ERR_NORMAL_STATUS" };
        constexpr Usage ERR_TRANSDUCERS_EXCEEDED = { 0x83, UsageKind::Sel, "ERR_TRANSDUCERS_EXCEEDED" };
        constexpr Usage ERR_FULL_TRANS_FEATURES_UNAVAILABLE = { 0x84, UsageKind::Sel, "ERR_FULL_TRANS_FEATURES_UNAVAILABLE" };
        constexpr Usage ERR_CHARGE_LOW = { 0x85, UsageKind::Sel, "ERR_CHARGE_LOW" };
        constexpr Usage NO_PROTOCOL = { 0x95, UsageKind::Sel, "NO_PROTOCOL" };
        constexpr Usage WACOM_AES_PROTOCOL = { 0x96, UsageKind::Sel, "WACOM_AES_PROTOCOL" };
        constexpr Usage USI_PROTOCOL = { 0x97, UsageKind::Sel, "USI_PROTOCOL" };
        constexpr Usage MICROSOFT_PEN_PROTOCOL = { 0x98, UsageKind::Sel, "MICROSOFT_PEN_PROTOCOL" };
        constexpr Usage SWITCH_DISABLED = { 0xA3, UsageKind::Sel, "SWITCH_DISABLED" };
        constexpr Usage SWITCH_UNIMPLEMENTED = { 0xA4, UsageKind::Sel, "SWITCH_UNIMPLEMENTED" };
    }
    namespace OSC {
        constexpr Usage UNTOUCH = { 0x34, UsageKind::OSC, "UNTOUCH" };
        constexpr Usage TAP = { 0x35, UsageKind::OSC, "TAP" };
    }
    namespace CA {
        constexpr Usage DIGITIZER = { 0x01, UsageKind::CA, "DIGITIZER" };
        constexpr Usage PEN = { 0x02, UsageKind::CA, "PEN" };
        constexpr Usage LIGHT_PEN = { 0x03, UsageKind::CA, "LIGHT_PEN" };
        constexpr Usage TOUCH_SCREEN = { 0x04, UsageKind::CA, "TOUCH_SCREEN" };
        constexpr Usage TOUCH_PAD = { 0x05, UsageKind::CA, "TOUCH_PAD" };
        constexpr Usage WHITEBOARD = { 0x06, UsageKind::CA, "WHITEBOARD" };
        constexpr Usage COORDINATE_MEASURING_MACHINE = { 0x07, UsageKind::CA, "COORDINATE_MEASURING_MACHINE" };
        constexpr Usage 3D_DIGITIZER = { 0x08, UsageKind::CA, "3D_DIGITIZER" };
        constexpr Usage STEREO_PLOTTER = { 0x09, UsageKind::CA, "STEREO_PLOTTER" };
        constexpr Usage ARTICULATED_ARM = { 0x0A, UsageKind::CA, "ARTICULATED_ARM" };
        constexpr Usage ARMATURE = { 0x0B, UsageKind::CA, "ARMATURE" };
        constexpr Usage MULTIPLE_POINT_DIGITIZER = { 0x0C, UsageKind::CA, "MULTIPLE_POINT_DIGITIZER" };
        constexpr Usage FREE_SPACE_WAND = { 0x0D, UsageKind::CA, "FREE_SPACE_WAND" };
        constexpr Usage DEVICE_CONFIGURATION = { 0x0E, UsageKind::CA, "DEVICE_CONFIGURATION" };
        constexpr Usage CAPACITIVE_HEAT_MAP_DIGITIZER = { 0x0F, UsageKind::CA, "CAPACITIVE_HEAT_MAP_DIGITIZER" };
        constexpr Usage STYLUS = { 0x20, UsageKind::CA, "STYLUS" };
    }
}
