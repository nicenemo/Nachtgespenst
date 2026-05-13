#pragma once
#include <stdint.h>

/**
 * HID Usage Page: LED (0x08)
 */
namespace HID::LEDs {
    constexpr uint8_t PAGE_ID = 0x08;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t RGB_LED = 0x52;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t FLASH_ON_TIME = 0x42;
    
    constexpr uint8_t SLOW_BLINK_ON_TIME = 0x43;
    
    constexpr uint8_t SLOW_BLINK_OFF_TIME = 0x44;
    
    constexpr uint8_t FAST_BLINK_ON_TIME = 0x45;
    
    constexpr uint8_t FAST_BLINK_OFF_TIME = 0x46;
    
    constexpr uint8_t RED_LED_CHANNEL = 0x53;
    
    constexpr uint8_t BLUE_LED_CHANNEL = 0x54;
    
    constexpr uint8_t GREEN_LED_CHANNEL = 0x55;
    
    constexpr uint8_t LED_INTENSITY = 0x56;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t PLAYER_INDICATOR = 0x60;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t NUM_LOCK = 0x01;
    
    constexpr uint8_t CAPS_LOCK = 0x02;
    
    constexpr uint8_t SCROLL_LOCK = 0x03;
    
    constexpr uint8_t COMPOSE = 0x04;
    
    constexpr uint8_t KANA = 0x05;
    
    constexpr uint8_t POWER = 0x06;
    
    constexpr uint8_t SHIFT = 0x07;
    
    constexpr uint8_t DO_NOT_DISTURB = 0x08;
    
    constexpr uint8_t MUTE = 0x09;
    
    constexpr uint8_t TONE_ENABLE = 0x0A;
    
    constexpr uint8_t HIGH_CUT_FILTER = 0x0B;
    
    constexpr uint8_t LOW_CUT_FILTER = 0x0C;
    
    constexpr uint8_t EQUALIZER_ENABLE = 0x0D;
    
    constexpr uint8_t SOUND_FIELD_ON = 0x0E;
    
    constexpr uint8_t SURROUND_ON = 0x0F;
    
    constexpr uint8_t REPEAT = 0x10;
    
    constexpr uint8_t STEREO = 0x11;
    
    constexpr uint8_t SAMPLING_RATE_DETECT = 0x12;
    
    constexpr uint8_t SPINNING = 0x13;
    
    constexpr uint8_t CAV = 0x14;
    
    constexpr uint8_t CLV = 0x15;
    
    constexpr uint8_t RECORDING_FORMAT_DETECT = 0x16;
    
    constexpr uint8_t OFF_HOOK = 0x17;
    
    constexpr uint8_t RING = 0x18;
    
    constexpr uint8_t MESSAGE_WAITING = 0x19;
    
    constexpr uint8_t DATA_MODE = 0x1A;
    
    constexpr uint8_t BATTERY_OPERATION = 0x1B;
    
    constexpr uint8_t BATTERY_OK = 0x1C;
    
    constexpr uint8_t BATTERY_LOW = 0x1D;
    
    constexpr uint8_t SPEAKER = 0x1E;
    
    constexpr uint8_t HEADSET = 0x1F;
    
    constexpr uint8_t HOLD = 0x20;
    
    constexpr uint8_t MICROPHONE = 0x21;
    
    constexpr uint8_t COVERAGE = 0x22;
    
    constexpr uint8_t NIGHT_MODE = 0x23;
    
    constexpr uint8_t SEND_CALLS = 0x24;
    
    constexpr uint8_t CALL_PICKUP = 0x25;
    
    constexpr uint8_t CONFERENCE = 0x26;
    
    constexpr uint8_t STAND_BY = 0x27;
    
    constexpr uint8_t CAMERA_ON = 0x28;
    
    constexpr uint8_t CAMERA_OFF = 0x29;
    
    constexpr uint8_t ON_LINE = 0x2A;
    
    constexpr uint8_t OFF_LINE = 0x2B;
    
    constexpr uint8_t BUSY = 0x2C;
    
    constexpr uint8_t READY = 0x2D;
    
    constexpr uint8_t PAPER_OUT = 0x2E;
    
    constexpr uint8_t PAPER_JAM = 0x2F;
    
    constexpr uint8_t REMOTE = 0x30;
    
    constexpr uint8_t FORWARD = 0x31;
    
    constexpr uint8_t REVERSE = 0x32;
    
    constexpr uint8_t STOP = 0x33;
    
    constexpr uint8_t REWIND = 0x34;
    
    constexpr uint8_t FAST_FORWARD = 0x35;
    
    constexpr uint8_t PLAY = 0x36;
    
    constexpr uint8_t PAUSE = 0x37;
    
    constexpr uint8_t RECORD = 0x38;
    
    constexpr uint8_t ERROR = 0x39;
    
    constexpr uint8_t GENERIC_INDICATOR = 0x4B;
    
    constexpr uint8_t SYS_SUSPEND = 0x4C;
    
    constexpr uint8_t EXTERNAL_POWER_CONNECTED = 0x4D;
    
    constexpr uint8_t GOOD_STATUS = 0x50;
    
    constexpr uint8_t WARNING_STATUS = 0x51;
    
    constexpr uint8_t SYS_MICROPHONE_MUTE = 0x57;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t INDICATOR_ON = 0x3D;
    
    constexpr uint8_t INDICATOR_FLASH = 0x3E;
    
    constexpr uint8_t INDICATOR_SLOW_BLINK = 0x3F;
    
    constexpr uint8_t INDICATOR_FAST_BLINK = 0x40;
    
    constexpr uint8_t INDICATOR_OFF = 0x41;
    
    constexpr uint8_t INDICATOR_RED = 0x48;
    
    constexpr uint8_t INDICATOR_GREEN = 0x49;
    
    constexpr uint8_t INDICATOR_AMBER = 0x4A;
    
    constexpr uint8_t INDICATOR_BLUE = 0x4E;
    
    constexpr uint8_t INDICATOR_ORANGE = 0x4F;
    
    constexpr uint8_t PLAYER_1 = 0x61;
    
    constexpr uint8_t PLAYER_2 = 0x62;
    
    constexpr uint8_t PLAYER_3 = 0x63;
    
    constexpr uint8_t PLAYER_4 = 0x64;
    
    constexpr uint8_t PLAYER_5 = 0x65;
    
    constexpr uint8_t PLAYER_6 = 0x66;
    
    constexpr uint8_t PLAYER_7 = 0x67;
    
    constexpr uint8_t PLAYER_8 = 0x68;

    // ==========================================
    // KIND: UM (UM)
    // ==========================================
    
    constexpr uint8_t USAGE_MULTI_MODE_INDICATOR = 0x3C;
    
    constexpr uint8_t USAGE_INDICATOR_COLOR = 0x47;

    // ==========================================
    // KIND: US (US)
    // ==========================================
    
    constexpr uint8_t USAGE_SELECTED_INDICATOR = 0x3A;
    
    constexpr uint8_t USAGE_IN_USE_INDICATOR = 0x3B;

}
