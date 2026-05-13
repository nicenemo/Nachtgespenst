#pragma once
#include "../HID_Types.h"

namespace HID::TelephonyDevice {
    constexpr uint8_t PAGE_ID = 0x000B;
    namespace OTHER {
        constexpr Usage TELEPHONY_KEY_PAD = { 0x06, UsageKind::OTHER, "TELEPHONY_KEY_PAD" };
        constexpr Usage PROGRAMMABLE_BUTTON = { 0x07, UsageKind::OTHER, "PROGRAMMABLE_BUTTON" };
        constexpr Usage RING_TYPE = { 0xF4, UsageKind::OTHER, "RING_TYPE" };
    }
    namespace DV {
        constexpr Usage INCOMING_CALL_HISTORY_COUNT = { 0x110, UsageKind::DV, "INCOMING_CALL_HISTORY_COUNT" };
        constexpr Usage OUTGOING_CALL_HISTORY_COUNT = { 0x111, UsageKind::DV, "OUTGOING_CALL_HISTORY_COUNT" };
        constexpr Usage PHONE_LOCALE = { 0x114, UsageKind::DV, "PHONE_LOCALE" };
        constexpr Usage PHONE_TIME_SECOND = { 0x140, UsageKind::DV, "PHONE_TIME_SECOND" };
        constexpr Usage PHONE_TIME_MINUTE = { 0x141, UsageKind::DV, "PHONE_TIME_MINUTE" };
        constexpr Usage PHONE_TIME_HOUR = { 0x142, UsageKind::DV, "PHONE_TIME_HOUR" };
        constexpr Usage PHONE_DATE_DAY = { 0x143, UsageKind::DV, "PHONE_DATE_DAY" };
        constexpr Usage PHONE_DATE_MONTH = { 0x144, UsageKind::DV, "PHONE_DATE_MONTH" };
        constexpr Usage PHONE_DATE_YEAR = { 0x145, UsageKind::DV, "PHONE_DATE_YEAR" };
        constexpr Usage HANDSET_NICKNAME = { 0x146, UsageKind::DV, "HANDSET_NICKNAME" };
        constexpr Usage ADDRESS_BOOK_ID = { 0x147, UsageKind::DV, "ADDRESS_BOOK_ID" };
        constexpr Usage CALL_DURATION = { 0x14A, UsageKind::DV, "CALL_DURATION" };
    }
    namespace OOC {
        constexpr Usage HOOK_SWITCH = { 0x20, UsageKind::OOC, "HOOK_SWITCH" };
        constexpr Usage HOLD = { 0x23, UsageKind::OOC, "HOLD" };
        constexpr Usage PARK = { 0x27, UsageKind::OOC, "PARK" };
        constexpr Usage FORWARD_CALLS = { 0x28, UsageKind::OOC, "FORWARD_CALLS" };
        constexpr Usage SPEAKER_PHONE = { 0x2B, UsageKind::OOC, "SPEAKER_PHONE" };
        constexpr Usage CONFERENCE = { 0x2C, UsageKind::OOC, "CONFERENCE" };
        constexpr Usage RING_ENABLE = { 0x2D, UsageKind::OOC, "RING_ENABLE" };
        constexpr Usage PHONE_MUTE = { 0x2F, UsageKind::OOC, "PHONE_MUTE" };
        constexpr Usage SEND = { 0x31, UsageKind::OOC, "SEND" };
        constexpr Usage PHONE_DIRECTORY = { 0x53, UsageKind::OOC, "PHONE_DIRECTORY" };
        constexpr Usage VOICE_MAIL = { 0x70, UsageKind::OOC, "VOICE_MAIL" };
        constexpr Usage SCREEN_CALLS = { 0x71, UsageKind::OOC, "SCREEN_CALLS" };
        constexpr Usage DO_NOT_DISTURB = { 0x72, UsageKind::OOC, "DO_NOT_DISTURB" };
        constexpr Usage ANSWER_ON_OFF = { 0x74, UsageKind::OOC, "ANSWER_ON_OFF" };
        constexpr Usage TONES_OFF = { 0x9C, UsageKind::OOC, "TONES_OFF" };
        constexpr Usage RINGER = { 0x9E, UsageKind::OOC, "RINGER" };
        constexpr Usage HOST_CONTROL = { 0xF0, UsageKind::OOC, "HOST_CONTROL" };
        constexpr Usage HOST_AVAILABLE = { 0xF1, UsageKind::OOC, "HOST_AVAILABLE" };
        constexpr Usage HOST_CALL_ACTIVE = { 0xF2, UsageKind::OOC, "HOST_CALL_ACTIVE" };
        constexpr Usage ACTIVATE_HANDSET_AUDIO = { 0xF3, UsageKind::OOC, "ACTIVATE_HANDSET_AUDIO" };
        constexpr Usage RE_DIALABLE_PHONE_NUMBER = { 0xF5, UsageKind::OOC, "RE_DIALABLE_PHONE_NUMBER" };
        constexpr Usage EMAIL_MESSAGE_WAITING = { 0x108, UsageKind::OOC, "EMAIL_MESSAGE_WAITING" };
        constexpr Usage VOICEMAIL_MESSAGE_WAITING = { 0x109, UsageKind::OOC, "VOICEMAIL_MESSAGE_WAITING" };
        constexpr Usage HOST_HOLD = { 0x10A, UsageKind::OOC, "HOST_HOLD" };
    }
    namespace MC {
        constexpr Usage FLASH = { 0x21, UsageKind::MC, "FLASH" };
        constexpr Usage ALTERNATE_FUNCTION = { 0x29, UsageKind::MC, "ALTERNATE_FUNCTION" };
        constexpr Usage CALLER_ID = { 0x30, UsageKind::MC, "CALLER_ID" };
        constexpr Usage INSIDE_DIAL_TONE = { 0x90, UsageKind::MC, "INSIDE_DIAL_TONE" };
        constexpr Usage OUTSIDE_DIAL_TONE = { 0x91, UsageKind::MC, "OUTSIDE_DIAL_TONE" };
        constexpr Usage INSIDE_RING_TONE = { 0x92, UsageKind::MC, "INSIDE_RING_TONE" };
        constexpr Usage OUTSIDE_RING_TONE = { 0x93, UsageKind::MC, "OUTSIDE_RING_TONE" };
        constexpr Usage PRIORITY_RING_TONE = { 0x94, UsageKind::MC, "PRIORITY_RING_TONE" };
        constexpr Usage INSIDE_RINGBACK = { 0x95, UsageKind::MC, "INSIDE_RINGBACK" };
        constexpr Usage PRIORITY_RINGBACK = { 0x96, UsageKind::MC, "PRIORITY_RINGBACK" };
        constexpr Usage LINE_BUSY_TONE = { 0x97, UsageKind::MC, "LINE_BUSY_TONE" };
        constexpr Usage REORDER_TONE = { 0x98, UsageKind::MC, "REORDER_TONE" };
        constexpr Usage CALL_WAITING_TONE = { 0x99, UsageKind::MC, "CALL_WAITING_TONE" };
        constexpr Usage CONFIRMATION_TONE_1 = { 0x9A, UsageKind::MC, "CONFIRMATION_TONE_1" };
        constexpr Usage CONFIRMATION_TONE_2 = { 0x9B, UsageKind::MC, "CONFIRMATION_TONE_2" };
        constexpr Usage OUTSIDE_RINGBACK = { 0x9D, UsageKind::MC, "OUTSIDE_RINGBACK" };
    }
    namespace CL {
        constexpr Usage MESSAGE_CONTROLS = { 0x03, UsageKind::CL, "MESSAGE_CONTROLS" };
        constexpr Usage HANDSET = { 0x04, UsageKind::CL, "HANDSET" };
        constexpr Usage HEADSET = { 0x05, UsageKind::CL, "HEADSET" };
        constexpr Usage INCOMING_CALL_HISTORY = { 0x112, UsageKind::CL, "INCOMING_CALL_HISTORY" };
        constexpr Usage OUTGOING_CALL_HISTORY = { 0x113, UsageKind::CL, "OUTGOING_CALL_HISTORY" };
    }
    namespace Sel {
        constexpr Usage PHONE_KEY_0 = { 0xB0, UsageKind::Sel, "PHONE_KEY_0" };
        constexpr Usage PHONE_KEY_1 = { 0xB1, UsageKind::Sel, "PHONE_KEY_1" };
        constexpr Usage PHONE_KEY_2 = { 0xB2, UsageKind::Sel, "PHONE_KEY_2" };
        constexpr Usage PHONE_KEY_3 = { 0xB3, UsageKind::Sel, "PHONE_KEY_3" };
        constexpr Usage PHONE_KEY_4 = { 0xB4, UsageKind::Sel, "PHONE_KEY_4" };
        constexpr Usage PHONE_KEY_5 = { 0xB5, UsageKind::Sel, "PHONE_KEY_5" };
        constexpr Usage PHONE_KEY_6 = { 0xB6, UsageKind::Sel, "PHONE_KEY_6" };
        constexpr Usage PHONE_KEY_7 = { 0xB7, UsageKind::Sel, "PHONE_KEY_7" };
        constexpr Usage PHONE_KEY_8 = { 0xB8, UsageKind::Sel, "PHONE_KEY_8" };
        constexpr Usage PHONE_KEY_9 = { 0xB9, UsageKind::Sel, "PHONE_KEY_9" };
        constexpr Usage PHONE_KEY_STAR = { 0xBA, UsageKind::Sel, "PHONE_KEY_STAR" };
        constexpr Usage PHONE_KEY_POUND = { 0xBB, UsageKind::Sel, "PHONE_KEY_POUND" };
        constexpr Usage PHONE_KEY_A = { 0xBC, UsageKind::Sel, "PHONE_KEY_A" };
        constexpr Usage PHONE_KEY_B = { 0xBD, UsageKind::Sel, "PHONE_KEY_B" };
        constexpr Usage PHONE_KEY_C = { 0xBE, UsageKind::Sel, "PHONE_KEY_C" };
        constexpr Usage PHONE_KEY_D = { 0xBF, UsageKind::Sel, "PHONE_KEY_D" };
        constexpr Usage PHONE_CALL_HISTORY_KEY = { 0xC0, UsageKind::Sel, "PHONE_CALL_HISTORY_KEY" };
        constexpr Usage PHONE_CALLER_ID_KEY = { 0xC1, UsageKind::Sel, "PHONE_CALLER_ID_KEY" };
        constexpr Usage PHONE_SETTINGS_KEY = { 0xC2, UsageKind::Sel, "PHONE_SETTINGS_KEY" };
        constexpr Usage STOP_RING_TONE = { 0xF8, UsageKind::Sel, "STOP_RING_TONE" };
        constexpr Usage PSTN_RING_TONE = { 0xF9, UsageKind::Sel, "PSTN_RING_TONE" };
        constexpr Usage HOST_RING_TONE = { 0xFA, UsageKind::Sel, "HOST_RING_TONE" };
        constexpr Usage ALERT_SOUND_ERROR = { 0xFB, UsageKind::Sel, "ALERT_SOUND_ERROR" };
        constexpr Usage ALERT_SOUND_CONFIRM = { 0xFC, UsageKind::Sel, "ALERT_SOUND_CONFIRM" };
        constexpr Usage ALERT_SOUND_NOTIFICATION = { 0xFD, UsageKind::Sel, "ALERT_SOUND_NOTIFICATION" };
        constexpr Usage SILENT_RING = { 0xFE, UsageKind::Sel, "SILENT_RING" };
    }
    namespace OSC {
        constexpr Usage FEATURE = { 0x22, UsageKind::OSC, "FEATURE" };
        constexpr Usage REDIAL = { 0x24, UsageKind::OSC, "REDIAL" };
        constexpr Usage TRANSFER = { 0x25, UsageKind::OSC, "TRANSFER" };
        constexpr Usage DROP = { 0x26, UsageKind::OSC, "DROP" };
        constexpr Usage LINE = { 0x2A, UsageKind::OSC, "LINE" };
        constexpr Usage RING_SELECT = { 0x2E, UsageKind::OSC, "RING_SELECT" };
        constexpr Usage SPEED_DIAL = { 0x50, UsageKind::OSC, "SPEED_DIAL" };
        constexpr Usage STORE_NUMBER = { 0x51, UsageKind::OSC, "STORE_NUMBER" };
        constexpr Usage RECALL_NUMBER = { 0x52, UsageKind::OSC, "RECALL_NUMBER" };
        constexpr Usage MESSAGE = { 0x73, UsageKind::OSC, "MESSAGE" };
    }
    namespace CA {
        constexpr Usage PHONE = { 0x01, UsageKind::CA, "PHONE" };
        constexpr Usage ANSWERING_MACHINE = { 0x02, UsageKind::CA, "ANSWERING_MACHINE" };
        constexpr Usage DUAL_MODE_PHONE = { 0x14B, UsageKind::CA, "DUAL_MODE_PHONE" };
    }
}
