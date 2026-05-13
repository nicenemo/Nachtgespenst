#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Generic Desktop (0x01)
 */
namespace HID::Desktop {
    constexpr uint8_t PAGE_ID = 0x01;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t MOUSE = 0x02;
    
    constexpr uint8_t JOYSTICK = 0x04;
    
    constexpr uint8_t GAMEPAD = 0x05;
    
    constexpr uint8_t KEYBOARD = 0x06;
    
    constexpr uint8_t KEYPAD = 0x07;
    
    constexpr uint8_t MULTI_AXIS_CONTROLLER = 0x08;
    
    constexpr uint8_t TABLET_PC_SYS_CONTROLS = 0x09;
    
    constexpr uint8_t WATER_COOLING_DEVICE = 0x0A;
    
    constexpr uint8_t COMPUTER_CHASSIS_DEVICE = 0x0B;
    
    constexpr uint8_t WIRELESS_RADIO_CONTROLS = 0x0C;
    
    constexpr uint8_t PORTABLE_DEVICE_CONTROL = 0x0D;
    
    constexpr uint8_t SYS_MULTI_AXIS_CONTROLLER = 0x0E;
    
    constexpr uint8_t SPATIAL_CONTROLLER = 0x0F;
    
    constexpr uint8_t ASSISTIVE_CONTROL = 0x10;
    
    constexpr uint8_t DEVICE_DOCK = 0x11;
    
    constexpr uint8_t DOCKABLE_DEVICE = 0x12;
    
    constexpr uint8_t CALL_STATE_MANAGEMENT_CONTROL = 0x13;
    
    constexpr uint8_t SYS_CONTROL = 0x80;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t COUNTED_BUFFER = 0x3A;
    
    constexpr uint8_t SENSOR_ZONE = 0xC0;
    
    constexpr uint8_t CHASSIS_ENCLOSURE = 0xC5;
    
    constexpr uint8_t DOCKABLE_DEVICE_DISPLAY_OCCLUSION = 0xD5;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t POINTER = 0x01;
    
    constexpr uint8_t THUMBSTICK = 0x96;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t CONTROL_ENABLE = 0xCB;
    
    constexpr uint8_t DOCKABLE_DEVICE_DOCKING_STATE = 0xD4;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t X = 0x30;
    
    constexpr uint8_t Y = 0x31;
    
    constexpr uint8_t Z = 0x32;
    
    constexpr uint8_t RX = 0x33;
    
    constexpr uint8_t RY = 0x34;
    
    constexpr uint8_t RZ = 0x35;
    
    constexpr uint8_t SLIDER = 0x36;
    
    constexpr uint8_t DIAL = 0x37;
    
    constexpr uint8_t WHEEL = 0x38;
    
    constexpr uint8_t HAT_SWITCH = 0x39;
    
    constexpr uint8_t BYTE_COUNT = 0x3B;
    
    constexpr uint8_t VX = 0x40;
    
    constexpr uint8_t VY = 0x41;
    
    constexpr uint8_t VZ = 0x42;
    
    constexpr uint8_t VBRX = 0x43;
    
    constexpr uint8_t VBRY = 0x44;
    
    constexpr uint8_t VBRZ = 0x45;
    
    constexpr uint8_t VNO = 0x46;
    
    constexpr uint8_t FEATURE_NOTIFICATION = 0x47;
    
    constexpr uint8_t RESOLUTION_MULTIPLIER = 0x48;
    
    constexpr uint8_t QX = 0x49;
    
    constexpr uint8_t QY = 0x4A;
    
    constexpr uint8_t QZ = 0x4B;
    
    constexpr uint8_t QW = 0x4C;
    
    constexpr uint8_t SYS_FUNCTION_SHIFT_LOCK_INDICATOR = 0x99;
    
    constexpr uint8_t RPM = 0xC1;
    
    constexpr uint8_t COOLANT_LEVEL = 0xC2;
    
    constexpr uint8_t DOCKABLE_DEVICE_UNIQUE_ID = 0xD0;
    
    constexpr uint8_t DOCKABLE_DEVICE_VENDOR_ID = 0xD1;
    
    constexpr uint8_t DOCKABLE_DEVICE_PRIMARY_USAGE_PAGE = 0xD2;
    
    constexpr uint8_t DOCKABLE_DEVICE_PRIMARY_USAGE_ID = 0xD3;
    
    constexpr uint8_t DOCKABLE_DEVICE_OBJECT_TYPE = 0xD6;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint8_t INDEX_TRIGGER = 0x94;
    
    constexpr uint8_t PALM_TRIGGER = 0x95;
    
    constexpr uint8_t SYS_FUNCTION_SHIFT = 0x97;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t START = 0x3D;
    
    constexpr uint8_t SELECT = 0x3E;
    
    constexpr uint8_t D_PAD_UP = 0x90;
    
    constexpr uint8_t D_PAD_DOWN = 0x91;
    
    constexpr uint8_t D_PAD_RIGHT = 0x92;
    
    constexpr uint8_t D_PAD_LEFT = 0x93;
    
    constexpr uint8_t SYS_FUNCTION_SHIFT_LOCK = 0x98;
    
    constexpr uint8_t SYS_DO_NOT_DISTURB = 0x9B;
    
    constexpr uint8_t SYS_MICROPHONE_MUTE = 0xA9;
    
    constexpr uint8_t SYS_ACCESSIBILITY_BINDING = 0xAA;
    
    constexpr uint8_t WIRELESS_RADIO_BUTTON = 0xC6;
    
    constexpr uint8_t WIRELESS_RADIO_LED = 0xC7;
    
    constexpr uint8_t WIRELESS_RADIO_SLIDER_SWITCH = 0xC8;
    
    constexpr uint8_t SYS_DISPLAY_ROTATION_LOCK_BUTTON = 0xC9;
    
    constexpr uint8_t SYS_DISPLAY_ROTATION_LOCK_SLIDER_SWITCH = 0xCA;
    
    constexpr uint8_t CALL_ACTIVE_LED = 0xE0;
    
    constexpr uint8_t CALL_MUTE_LED = 0xE2;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t MOTION_WAKEUP = 0x3C;
    
    constexpr uint8_t SYS_POWER_DOWN = 0x81;
    
    constexpr uint8_t SYS_SLEEP = 0x82;
    
    constexpr uint8_t SYS_WAKE_UP = 0x83;
    
    constexpr uint8_t SYS_CONTEXT_MENU = 0x84;
    
    constexpr uint8_t SYS_MAIN_MENU = 0x85;
    
    constexpr uint8_t SYS_APP_MENU = 0x86;
    
    constexpr uint8_t SYS_MENU_HELP = 0x87;
    
    constexpr uint8_t SYS_MENU_EXIT = 0x88;
    
    constexpr uint8_t SYS_MENU_SELECT = 0x89;
    
    constexpr uint8_t SYS_COLD_RESTART = 0x8E;
    
    constexpr uint8_t SYS_WARM_RESTART = 0x8F;
    
    constexpr uint8_t SYS_DISMISS_NOTIFICATION = 0x9A;
    
    constexpr uint8_t SYS_DOCK = 0xA0;
    
    constexpr uint8_t SYS_UNDOCK = 0xA1;
    
    constexpr uint8_t SYS_SETUP = 0xA2;
    
    constexpr uint8_t SYS_BREAK = 0xA3;
    
    constexpr uint8_t SYS_DEBUGGER_BREAK = 0xA4;
    
    constexpr uint8_t APPLICATION_BREAK = 0xA5;
    
    constexpr uint8_t APPLICATION_DEBUGGER_BREAK = 0xA6;
    
    constexpr uint8_t SYS_SPEAKER_MUTE = 0xA7;
    
    constexpr uint8_t SYS_HIBERNATE = 0xA8;
    
    constexpr uint8_t SYS_DISPLAY_INVERT = 0xB0;
    
    constexpr uint8_t SYS_DISPLAY_INTERNAL = 0xB1;
    
    constexpr uint8_t SYS_DISPLAY_EXTERNAL = 0xB2;
    
    constexpr uint8_t SYS_DISPLAY_BOTH = 0xB3;
    
    constexpr uint8_t SYS_DISPLAY_DUAL = 0xB4;
    
    constexpr uint8_t SYS_DISPLAY_TOGGLE_INT_EXT_MODE = 0xB5;
    
    constexpr uint8_t SYS_DISPLAY_SWAP_PRIMARY_SECONDARY = 0xB6;
    
    constexpr uint8_t SYS_DISPLAY_TOGGLE_LCD_AUTOSCALE = 0xB7;
    
    constexpr uint8_t CALL_MUTE_TOGGLE = 0xE1;

    // ==========================================
    // KIND: Re-trigger Control (RTC)
    // ==========================================
    
    constexpr uint8_t SYS_MENU_RIGHT = 0x8A;
    
    constexpr uint8_t SYS_MENU_LEFT = 0x8B;
    
    constexpr uint8_t SYS_MENU_UP = 0x8C;
    
    constexpr uint8_t SYS_MENU_DOWN = 0x8D;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t COOLANT_CRITICAL_LEVEL = 0xC3;

    // ==========================================
    // KIND: US (US)
    // ==========================================
    
    constexpr uint8_t COOLANT_PUMP = 0xC4;

}
