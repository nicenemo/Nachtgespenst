#pragma once
#include "../HID_Types.h"

namespace HID::LED {
    constexpr uint8_t PAGE_ID = 0x0008;
    namespace OTHER {
        constexpr Usage USAGE_SELECTED_INDICATOR = { 0x3A, UsageKind::OTHER, "USAGE_SELECTED_INDICATOR" };
        constexpr Usage USAGE_IN_USE_INDICATOR = { 0x3B, UsageKind::OTHER, "USAGE_IN_USE_INDICATOR" };
        constexpr Usage USAGE_MULTI_MODE_INDICATOR = { 0x3C, UsageKind::OTHER, "USAGE_MULTI_MODE_INDICATOR" };
        constexpr Usage USAGE_INDICATOR_COLOR = { 0x47, UsageKind::OTHER, "USAGE_INDICATOR_COLOR" };
        constexpr Usage PLAYER_INDICATOR = { 0x60, UsageKind::OTHER, "PLAYER_INDICATOR" };
    }
    namespace DV {
        constexpr Usage FLASH_ON_TIME = { 0x42, UsageKind::DV, "FLASH_ON_TIME" };
        constexpr Usage SLOW_BLINK_ON_TIME = { 0x43, UsageKind::DV, "SLOW_BLINK_ON_TIME" };
        constexpr Usage SLOW_BLINK_OFF_TIME = { 0x44, UsageKind::DV, "SLOW_BLINK_OFF_TIME" };
        constexpr Usage FAST_BLINK_ON_TIME = { 0x45, UsageKind::DV, "FAST_BLINK_ON_TIME" };
        constexpr Usage FAST_BLINK_OFF_TIME = { 0x46, UsageKind::DV, "FAST_BLINK_OFF_TIME" };
        constexpr Usage RED_LED_CHANNEL = { 0x53, UsageKind::DV, "RED_LED_CHANNEL" };
        constexpr Usage BLUE_LED_CHANNEL = { 0x54, UsageKind::DV, "BLUE_LED_CHANNEL" };
        constexpr Usage GREEN_LED_CHANNEL = { 0x55, UsageKind::DV, "GREEN_LED_CHANNEL" };
        constexpr Usage LED_INTENSITY = { 0x56, UsageKind::DV, "LED_INTENSITY" };
    }
    namespace OOC {
        constexpr Usage NUM_LOCK = { 0x01, UsageKind::OOC, "NUM_LOCK" };
        constexpr Usage CAPS_LOCK = { 0x02, UsageKind::OOC, "CAPS_LOCK" };
        constexpr Usage SCROLL_LOCK = { 0x03, UsageKind::OOC, "SCROLL_LOCK" };
        constexpr Usage COMPOSE = { 0x04, UsageKind::OOC, "COMPOSE" };
        constexpr Usage KANA = { 0x05, UsageKind::OOC, "KANA" };
        constexpr Usage POWER = { 0x06, UsageKind::OOC, "POWER" };
        constexpr Usage SHIFT = { 0x07, UsageKind::OOC, "SHIFT" };
        constexpr Usage DO_NOT_DISTURB = { 0x08, UsageKind::OOC, "DO_NOT_DISTURB" };
        constexpr Usage MUTE = { 0x09, UsageKind::OOC, "MUTE" };
        constexpr Usage TONE_ENABLE = { 0x0A, UsageKind::OOC, "TONE_ENABLE" };
        constexpr Usage HIGH_CUT_FILTER = { 0x0B, UsageKind::OOC, "HIGH_CUT_FILTER" };
        constexpr Usage LOW_CUT_FILTER = { 0x0C, UsageKind::OOC, "LOW_CUT_FILTER" };
        constexpr Usage EQUALIZER_ENABLE = { 0x0D, UsageKind::OOC, "EQUALIZER_ENABLE" };
        constexpr Usage SOUND_FIELD_ON = { 0x0E, UsageKind::OOC, "SOUND_FIELD_ON" };
        constexpr Usage SURROUND_ON = { 0x0F, UsageKind::OOC, "SURROUND_ON" };
        constexpr Usage REPEAT = { 0x10, UsageKind::OOC, "REPEAT" };
        constexpr Usage STEREO = { 0x11, UsageKind::OOC, "STEREO" };
        constexpr Usage SAMPLING_RATE_DETECT = { 0x12, UsageKind::OOC, "SAMPLING_RATE_DETECT" };
        constexpr Usage SPINNING = { 0x13, UsageKind::OOC, "SPINNING" };
        constexpr Usage CAV = { 0x14, UsageKind::OOC, "CAV" };
        constexpr Usage CLV = { 0x15, UsageKind::OOC, "CLV" };
        constexpr Usage RECORDING_FORMAT_DETECT = { 0x16, UsageKind::OOC, "RECORDING_FORMAT_DETECT" };
        constexpr Usage OFF_HOOK = { 0x17, UsageKind::OOC, "OFF_HOOK" };
        constexpr Usage RING = { 0x18, UsageKind::OOC, "RING" };
        constexpr Usage MESSAGE_WAITING = { 0x19, UsageKind::OOC, "MESSAGE_WAITING" };
        constexpr Usage DATA_MODE = { 0x1A, UsageKind::OOC, "DATA_MODE" };
        constexpr Usage BATTERY_OPERATION = { 0x1B, UsageKind::OOC, "BATTERY_OPERATION" };
        constexpr Usage BATTERY_OK = { 0x1C, UsageKind::OOC, "BATTERY_OK" };
        constexpr Usage BATTERY_LOW = { 0x1D, UsageKind::OOC, "BATTERY_LOW" };
        constexpr Usage SPEAKER = { 0x1E, UsageKind::OOC, "SPEAKER" };
        constexpr Usage HEADSET = { 0x1F, UsageKind::OOC, "HEADSET" };
        constexpr Usage HOLD = { 0x20, UsageKind::OOC, "HOLD" };
        constexpr Usage MICROPHONE = { 0x21, UsageKind::OOC, "MICROPHONE" };
        constexpr Usage COVERAGE = { 0x22, UsageKind::OOC, "COVERAGE" };
        constexpr Usage NIGHT_MODE = { 0x23, UsageKind::OOC, "NIGHT_MODE" };
        constexpr Usage SEND_CALLS = { 0x24, UsageKind::OOC, "SEND_CALLS" };
        constexpr Usage CALL_PICKUP = { 0x25, UsageKind::OOC, "CALL_PICKUP" };
        constexpr Usage CONFERENCE = { 0x26, UsageKind::OOC, "CONFERENCE" };
        constexpr Usage STAND_BY = { 0x27, UsageKind::OOC, "STAND_BY" };
        constexpr Usage CAMERA_ON = { 0x28, UsageKind::OOC, "CAMERA_ON" };
        constexpr Usage CAMERA_OFF = { 0x29, UsageKind::OOC, "CAMERA_OFF" };
        constexpr Usage ON_LINE = { 0x2A, UsageKind::OOC, "ON_LINE" };
        constexpr Usage OFF_LINE = { 0x2B, UsageKind::OOC, "OFF_LINE" };
        constexpr Usage BUSY = { 0x2C, UsageKind::OOC, "BUSY" };
        constexpr Usage READY = { 0x2D, UsageKind::OOC, "READY" };
        constexpr Usage PAPER_OUT = { 0x2E, UsageKind::OOC, "PAPER_OUT" };
        constexpr Usage PAPER_JAM = { 0x2F, UsageKind::OOC, "PAPER_JAM" };
        constexpr Usage REMOTE = { 0x30, UsageKind::OOC, "REMOTE" };
        constexpr Usage FORWARD = { 0x31, UsageKind::OOC, "FORWARD" };
        constexpr Usage REVERSE = { 0x32, UsageKind::OOC, "REVERSE" };
        constexpr Usage STOP = { 0x33, UsageKind::OOC, "STOP" };
        constexpr Usage REWIND = { 0x34, UsageKind::OOC, "REWIND" };
        constexpr Usage FAST_FORWARD = { 0x35, UsageKind::OOC, "FAST_FORWARD" };
        constexpr Usage PLAY = { 0x36, UsageKind::OOC, "PLAY" };
        constexpr Usage PAUSE = { 0x37, UsageKind::OOC, "PAUSE" };
        constexpr Usage RECORD = { 0x38, UsageKind::OOC, "RECORD" };
        constexpr Usage ERROR = { 0x39, UsageKind::OOC, "ERROR" };
        constexpr Usage GENERIC_INDICATOR = { 0x4B, UsageKind::OOC, "GENERIC_INDICATOR" };
        constexpr Usage SYS_SUSPEND = { 0x4C, UsageKind::OOC, "SYS_SUSPEND" };
        constexpr Usage EXTERNAL_POWER_CONNECTED = { 0x4D, UsageKind::OOC, "EXTERNAL_POWER_CONNECTED" };
        constexpr Usage GOOD_STATUS = { 0x50, UsageKind::OOC, "GOOD_STATUS" };
        constexpr Usage WARNING_STATUS = { 0x51, UsageKind::OOC, "WARNING_STATUS" };
        constexpr Usage SYS_MICROPHONE_MUTE = { 0x57, UsageKind::OOC, "SYS_MICROPHONE_MUTE" };
    }
    namespace CL {
        constexpr Usage RGB_LED = { 0x52, UsageKind::CL, "RGB_LED" };
    }
    namespace Sel {
        constexpr Usage INDICATOR_ON = { 0x3D, UsageKind::Sel, "INDICATOR_ON" };
        constexpr Usage INDICATOR_FLASH = { 0x3E, UsageKind::Sel, "INDICATOR_FLASH" };
        constexpr Usage INDICATOR_SLOW_BLINK = { 0x3F, UsageKind::Sel, "INDICATOR_SLOW_BLINK" };
        constexpr Usage INDICATOR_FAST_BLINK = { 0x40, UsageKind::Sel, "INDICATOR_FAST_BLINK" };
        constexpr Usage INDICATOR_OFF = { 0x41, UsageKind::Sel, "INDICATOR_OFF" };
        constexpr Usage INDICATOR_RED = { 0x48, UsageKind::Sel, "INDICATOR_RED" };
        constexpr Usage INDICATOR_GREEN = { 0x49, UsageKind::Sel, "INDICATOR_GREEN" };
        constexpr Usage INDICATOR_AMBER = { 0x4A, UsageKind::Sel, "INDICATOR_AMBER" };
        constexpr Usage INDICATOR_BLUE = { 0x4E, UsageKind::Sel, "INDICATOR_BLUE" };
        constexpr Usage INDICATOR_ORANGE = { 0x4F, UsageKind::Sel, "INDICATOR_ORANGE" };
        constexpr Usage PLAYER_1 = { 0x61, UsageKind::Sel, "PLAYER_1" };
        constexpr Usage PLAYER_2 = { 0x62, UsageKind::Sel, "PLAYER_2" };
        constexpr Usage PLAYER_3 = { 0x63, UsageKind::Sel, "PLAYER_3" };
        constexpr Usage PLAYER_4 = { 0x64, UsageKind::Sel, "PLAYER_4" };
        constexpr Usage PLAYER_5 = { 0x65, UsageKind::Sel, "PLAYER_5" };
        constexpr Usage PLAYER_6 = { 0x66, UsageKind::Sel, "PLAYER_6" };
        constexpr Usage PLAYER_7 = { 0x67, UsageKind::Sel, "PLAYER_7" };
        constexpr Usage PLAYER_8 = { 0x68, UsageKind::Sel, "PLAYER_8" };
    }
}
