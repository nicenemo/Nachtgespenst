#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Digitizers (0x0D)
 */
namespace HID::Digitizers {
    constexpr uint8_t PAGE_ID = 0x0D;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t DIGITIZER = 0x01;
    
    constexpr uint8_t PEN = 0x02;
    
    constexpr uint8_t LIGHT_PEN = 0x03;
    
    constexpr uint8_t TOUCH_SCREEN = 0x04;
    
    constexpr uint8_t TOUCH_PAD = 0x05;
    
    constexpr uint8_t WHITEBOARD = 0x06;
    
    constexpr uint8_t COORDINATE_MEASURING_MACHINE = 0x07;
    
    constexpr uint8_t ID_3D_DIGITIZER = 0x08;
    
    constexpr uint8_t STEREO_PLOTTER = 0x09;
    
    constexpr uint8_t ARTICULATED_ARM = 0x0A;
    
    constexpr uint8_t ARMATURE = 0x0B;
    
    constexpr uint8_t MULTIPLE_POINT_DIGITIZER = 0x0C;
    
    constexpr uint8_t FREE_SPACE_WAND = 0x0D;
    
    constexpr uint8_t DEVICE_CONFIGURATION = 0x0E;
    
    constexpr uint8_t CAPACITIVE_HEAT_MAP_DIGITIZER = 0x0F;
    
    constexpr uint8_t STYLUS = 0x20;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t PUCK = 0x21;
    
    constexpr uint8_t FINGER = 0x22;
    
    constexpr uint8_t DEVICE_SETTINGS = 0x23;
    
    constexpr uint8_t CHARACTER_GESTURE = 0x24;
    
    constexpr uint8_t TABLET_FUNCTION_KEYS = 0x39;
    
    constexpr uint8_t PROGRAM_CHANGE_KEYS = 0x3A;
    
    constexpr uint8_t DIGITIZER_DIAGNOSTIC = 0x80;
    
    constexpr uint8_t TRANSDUCER_SOFTWARE_INFO = 0x90;
    
    constexpr uint8_t TRANSDUCER_SWITCHES = 0xA5;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t SURFACE_SWITCH = 0x57;
    
    constexpr uint8_t BUTTON_SWITCH = 0x58;
    
    constexpr uint8_t LATENCY_MODE = 0x60;
    
    constexpr uint8_t GESTURE_CHARACTER_ENABLE = 0x6D;
    
    constexpr uint8_t NO_PREFERRED_COLOR = 0x6F;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t TIP_PRESSURE = 0x30;
    
    constexpr uint8_t BARREL_PRESSURE = 0x31;
    
    constexpr uint8_t QUALITY = 0x36;
    
    constexpr uint8_t TRANSDUCER_INDEX = 0x38;
    
    constexpr uint8_t BATTERY_STRENGTH = 0x3B;
    
    constexpr uint8_t X_TILT = 0x3D;
    
    constexpr uint8_t Y_TILT = 0x3E;
    
    constexpr uint8_t AZIMUTH = 0x3F;
    
    constexpr uint8_t ALTITUDE = 0x40;
    
    constexpr uint8_t TWIST = 0x41;
    
    constexpr uint8_t WIDTH = 0x48;
    
    constexpr uint8_t HEIGHT = 0x49;
    
    constexpr uint8_t CONTACT_IDENTIFIER = 0x51;
    
    constexpr uint8_t DEVICE_MODE = 0x52;
    
    constexpr uint8_t DEVICE_IDENTIFIER = 0x53;
    
    constexpr uint8_t CONTACT_COUNT = 0x54;
    
    constexpr uint8_t SCAN_TIME = 0x56;
    
    constexpr uint8_t PREFERRED_COLOR = 0x5C;
    
    constexpr uint8_t PREFERRED_LINE_WIDTH = 0x5E;
    
    constexpr uint8_t GESTURE_CHARACTER_QUALITY = 0x61;
    
    constexpr uint8_t CHARACTER_GESTURE_DATA_LENGTH = 0x62;
    
    constexpr uint8_t CHARACTER_GESTURE_DATA = 0x63;
    
    constexpr uint8_t CAPACITIVE_HEAT_MAP_FRAME_DATA = 0x6C;
    
    constexpr uint8_t REPORT_RATE = 0xA1;
    
    constexpr uint8_t TRANSDUCER_INDEX_SELECTOR = 0xA6;
    
    constexpr uint8_t BUTTON_PRESS_THRESHOLD = 0xB0;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint8_t IN_RANGE = 0x32;
    
    constexpr uint8_t TOUCH = 0x33;
    
    constexpr uint8_t DATA_VALID = 0x37;
    
    constexpr uint8_t INVERT = 0x3C;
    
    constexpr uint8_t TIP_SWITCH = 0x42;
    
    constexpr uint8_t SECONDARY_TIP_SWITCH = 0x43;
    
    constexpr uint8_t BARREL_SWITCH = 0x44;
    
    constexpr uint8_t ERASER = 0x45;
    
    constexpr uint8_t TABLET_PICK = 0x46;
    
    constexpr uint8_t TOUCH_VALID = 0x47;
    
    constexpr uint8_t SECONDARY_BARREL_SWITCH = 0x5A;
    
    constexpr uint8_t PREFERRED_COLOR_IS_LOCKED = 0x5D;
    
    constexpr uint8_t PREFERRED_LINE_WIDTH_IS_LOCKED = 0x5F;
    
    constexpr uint8_t PREFERRED_LINE_STYLE_IS_LOCKED = 0x71;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t GESTURE_CHARACTER_ENCODING = 0x64;
    
    constexpr uint8_t PREFERRED_LINE_STYLE = 0x70;
    
    constexpr uint8_t DIGITIZER_ERROR = 0x81;
    
    constexpr uint8_t DEVICE_SUPPORTED_PROTOCOLS = 0x93;
    
    constexpr uint8_t TRANSDUCER_SUPPORTED_PROTOCOLS = 0x94;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t UNTOUCH = 0x34;
    
    constexpr uint8_t TAP = 0x35;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint8_t PAD_TYPE = 0x59;
    
    constexpr uint8_t TRANSDUCER_CONNECTED = 0xA2;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t CONTACT_COUNT_MAXIMUM = 0x55;
    
    constexpr uint8_t TRANSDUCER_SERIAL_NUMBER = 0x5B;
    
    constexpr uint8_t CAPACITIVE_HEAT_MAP_PROTOCOL_VENDOR_ID = 0x6A;
    
    constexpr uint8_t CAPACITIVE_HEAT_MAP_PROTOCOL_VERSION = 0x6B;
    
    constexpr uint8_t TRANSDUCER_SERIAL_NUMBER_PART_2 = 0x6E;
    
    constexpr uint8_t TRANSDUCER_VENDOR_ID = 0x91;
    
    constexpr uint8_t TRANSDUCER_PRODUCT_ID = 0x92;
    
    constexpr uint8_t SUPPORTED_REPORT_RATES = 0xA0;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t UTF8_CHARACTER_GESTURE_ENCODING = 0x65;
    
    constexpr uint8_t UTF16_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING = 0x66;
    
    constexpr uint8_t UTF16_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING = 0x67;
    
    constexpr uint8_t UTF32_LITTLE_ENDIAN_CHARACTER_GESTURE_ENCODING = 0x68;
    
    constexpr uint8_t UTF32_BIG_ENDIAN_CHARACTER_GESTURE_ENCODING = 0x69;
    
    constexpr uint8_t INK = 0x72;
    
    constexpr uint8_t PENCIL = 0x73;
    
    constexpr uint8_t HIGHLIGHTER = 0x74;
    
    constexpr uint8_t CHISEL_MARKER = 0x75;
    
    constexpr uint8_t BRUSH = 0x76;
    
    constexpr uint8_t NO_PREFERENCE = 0x77;
    
    constexpr uint8_t ERR_NORMAL_STATUS = 0x82;
    
    constexpr uint8_t ERR_TRANSDUCERS_EXCEEDED = 0x83;
    
    constexpr uint8_t ERR_FULL_TRANS_FEATURES_UNAVAILABLE = 0x84;
    
    constexpr uint8_t ERR_CHARGE_LOW = 0x85;
    
    constexpr uint8_t NO_PROTOCOL = 0x95;
    
    constexpr uint8_t WACOM_AES_PROTOCOL = 0x96;
    
    constexpr uint8_t USI_PROTOCOL = 0x97;
    
    constexpr uint8_t MICROSOFT_PEN_PROTOCOL = 0x98;
    
    constexpr uint8_t SWITCH_DISABLED = 0xA3;
    
    constexpr uint8_t SWITCH_UNIMPLEMENTED = 0xA4;

}
