#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Telephony Device (0x0B)
 */
namespace HID::TelephonyDevice {
    constexpr uint8_t PAGE_ID = 0x0B;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t PHONE = 0x01;
    
    constexpr uint8_t ANSWERING_MACHINE = 0x02;
    
    constexpr uint8_t DUAL_MODE_PHONE = 0x14B;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t MESSAGE_CONTROLS = 0x03;
    
    constexpr uint8_t HANDSET = 0x04;
    
    constexpr uint8_t HEADSET = 0x05;
    
    constexpr uint8_t INCOMING_CALL_HISTORY = 0x112;
    
    constexpr uint8_t OUTGOING_CALL_HISTORY = 0x113;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t INCOMING_CALL_HISTORY_COUNT = 0x110;
    
    constexpr uint8_t OUTGOING_CALL_HISTORY_COUNT = 0x111;
    
    constexpr uint8_t PHONE_LOCALE = 0x114;
    
    constexpr uint8_t PHONE_TIME_SECOND = 0x140;
    
    constexpr uint8_t PHONE_TIME_MINUTE = 0x141;
    
    constexpr uint8_t PHONE_TIME_HOUR = 0x142;
    
    constexpr uint8_t PHONE_DATE_DAY = 0x143;
    
    constexpr uint8_t PHONE_DATE_MONTH = 0x144;
    
    constexpr uint8_t PHONE_DATE_YEAR = 0x145;
    
    constexpr uint8_t HANDSET_NICKNAME = 0x146;
    
    constexpr uint8_t ADDRESS_BOOK_ID = 0x147;
    
    constexpr uint8_t CALL_DURATION = 0x14A;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint8_t FLASH = 0x21;
    
    constexpr uint8_t ALTERNATE_FUNCTION = 0x29;
    
    constexpr uint8_t CALLER_ID = 0x30;
    
    constexpr uint8_t INSIDE_DIAL_TONE = 0x90;
    
    constexpr uint8_t OUTSIDE_DIAL_TONE = 0x91;
    
    constexpr uint8_t INSIDE_RING_TONE = 0x92;
    
    constexpr uint8_t OUTSIDE_RING_TONE = 0x93;
    
    constexpr uint8_t PRIORITY_RING_TONE = 0x94;
    
    constexpr uint8_t INSIDE_RINGBACK = 0x95;
    
    constexpr uint8_t PRIORITY_RINGBACK = 0x96;
    
    constexpr uint8_t LINE_BUSY_TONE = 0x97;
    
    constexpr uint8_t REORDER_TONE = 0x98;
    
    constexpr uint8_t CALL_WAITING_TONE = 0x99;
    
    constexpr uint8_t CONFIRMATION_TONE_1 = 0x9A;
    
    constexpr uint8_t CONFIRMATION_TONE_2 = 0x9B;
    
    constexpr uint8_t OUTSIDE_RINGBACK = 0x9D;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t TELEPHONY_KEY_PAD = 0x06;
    
    constexpr uint8_t PROGRAMMABLE_BUTTON = 0x07;
    
    constexpr uint8_t RING_TYPE = 0xF4;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t HOOK_SWITCH = 0x20;
    
    constexpr uint8_t HOLD = 0x23;
    
    constexpr uint8_t PARK = 0x27;
    
    constexpr uint8_t FORWARD_CALLS = 0x28;
    
    constexpr uint8_t SPEAKER_PHONE = 0x2B;
    
    constexpr uint8_t CONFERENCE = 0x2C;
    
    constexpr uint8_t RING_ENABLE = 0x2D;
    
    constexpr uint8_t PHONE_MUTE = 0x2F;
    
    constexpr uint8_t SEND = 0x31;
    
    constexpr uint8_t PHONE_DIRECTORY = 0x53;
    
    constexpr uint8_t VOICE_MAIL = 0x70;
    
    constexpr uint8_t SCREEN_CALLS = 0x71;
    
    constexpr uint8_t DO_NOT_DISTURB = 0x72;
    
    constexpr uint8_t ANSWER_ON_OFF = 0x74;
    
    constexpr uint8_t TONES_OFF = 0x9C;
    
    constexpr uint8_t RINGER = 0x9E;
    
    constexpr uint8_t HOST_CONTROL = 0xF0;
    
    constexpr uint8_t HOST_AVAILABLE = 0xF1;
    
    constexpr uint8_t HOST_CALL_ACTIVE = 0xF2;
    
    constexpr uint8_t ACTIVATE_HANDSET_AUDIO = 0xF3;
    
    constexpr uint8_t RE_DIALABLE_PHONE_NUMBER = 0xF5;
    
    constexpr uint8_t EMAIL_MESSAGE_WAITING = 0x108;
    
    constexpr uint8_t VOICEMAIL_MESSAGE_WAITING = 0x109;
    
    constexpr uint8_t HOST_HOLD = 0x10A;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t FEATURE = 0x22;
    
    constexpr uint8_t REDIAL = 0x24;
    
    constexpr uint8_t TRANSFER = 0x25;
    
    constexpr uint8_t DROP = 0x26;
    
    constexpr uint8_t LINE = 0x2A;
    
    constexpr uint8_t RING_SELECT = 0x2E;
    
    constexpr uint8_t SPEED_DIAL = 0x50;
    
    constexpr uint8_t STORE_NUMBER = 0x51;
    
    constexpr uint8_t RECALL_NUMBER = 0x52;
    
    constexpr uint8_t MESSAGE = 0x73;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t PHONE_KEY_0 = 0xB0;
    
    constexpr uint8_t PHONE_KEY_1 = 0xB1;
    
    constexpr uint8_t PHONE_KEY_2 = 0xB2;
    
    constexpr uint8_t PHONE_KEY_3 = 0xB3;
    
    constexpr uint8_t PHONE_KEY_4 = 0xB4;
    
    constexpr uint8_t PHONE_KEY_5 = 0xB5;
    
    constexpr uint8_t PHONE_KEY_6 = 0xB6;
    
    constexpr uint8_t PHONE_KEY_7 = 0xB7;
    
    constexpr uint8_t PHONE_KEY_8 = 0xB8;
    
    constexpr uint8_t PHONE_KEY_9 = 0xB9;
    
    constexpr uint8_t PHONE_KEY_STAR = 0xBA;
    
    constexpr uint8_t PHONE_KEY_POUND = 0xBB;
    
    constexpr uint8_t PHONE_KEY_A = 0xBC;
    
    constexpr uint8_t PHONE_KEY_B = 0xBD;
    
    constexpr uint8_t PHONE_KEY_C = 0xBE;
    
    constexpr uint8_t PHONE_KEY_D = 0xBF;
    
    constexpr uint8_t PHONE_CALL_HISTORY_KEY = 0xC0;
    
    constexpr uint8_t PHONE_CALLER_ID_KEY = 0xC1;
    
    constexpr uint8_t PHONE_SETTINGS_KEY = 0xC2;
    
    constexpr uint8_t STOP_RING_TONE = 0xF8;
    
    constexpr uint8_t PSTN_RING_TONE = 0xF9;
    
    constexpr uint8_t HOST_RING_TONE = 0xFA;
    
    constexpr uint8_t ALERT_SOUND_ERROR = 0xFB;
    
    constexpr uint8_t ALERT_SOUND_CONFIRM = 0xFC;
    
    constexpr uint8_t ALERT_SOUND_NOTIFICATION = 0xFD;
    
    constexpr uint8_t SILENT_RING = 0xFE;

}
