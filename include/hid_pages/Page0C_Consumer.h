#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Consumer (0x0C)
 */
namespace HID::Consumer {
    constexpr uint8_t PAGE_ID = 0x0C;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint16_t CONSUMER_CONTROL = 0x01;
    
    constexpr uint16_t MICROPHONE = 0x04;
    
    constexpr uint16_t HEADPHONE = 0x05;
    
    constexpr uint16_t GRAPHIC_EQUALIZER = 0x06;
    
    constexpr uint16_t BACKLIGHT = 0x07;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint16_t SPEAKER_SYSTEM = 0x160;
    
    constexpr uint16_t CHANNEL_LEFT = 0x161;
    
    constexpr uint16_t CHANNEL_RIGHT = 0x162;
    
    constexpr uint16_t CHANNEL_CENTER = 0x163;
    
    constexpr uint16_t CHANNEL_FRONT = 0x164;
    
    constexpr uint16_t CHANNEL_CENTER_FRONT = 0x165;
    
    constexpr uint16_t CHANNEL_SIDE = 0x166;
    
    constexpr uint16_t CHANNEL_SURROUND = 0x167;
    
    constexpr uint16_t CHANNEL_LOW_FREQUENCY_ENHANCEMENT = 0x168;
    
    constexpr uint16_t CHANNEL_TOP = 0x169;
    
    constexpr uint16_t CHANNEL_UNKNOWN = 0x16A;
    
    constexpr uint16_t EXTENDED_ATTRIBUTES_COLLECTION = 0x2C0;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint16_t CONTACT_INDEX = 0x503;
    
    constexpr uint16_t CONTACT_NICKNAME = 0x504;
    
    constexpr uint16_t CONTACT_FIRST_NAME = 0x505;
    
    constexpr uint16_t CONTACT_LAST_NAME = 0x506;
    
    constexpr uint16_t CONTACT_FULL_NAME = 0x507;
    
    constexpr uint16_t CONTACT_PHONE_NUMBER_PERSONAL = 0x508;
    
    constexpr uint16_t CONTACT_PHONE_NUMBER_BUSINESS = 0x509;
    
    constexpr uint16_t CONTACT_PHONE_NUMBER_MOBILE = 0x50A;
    
    constexpr uint16_t CONTACT_PHONE_NUMBER_PAGER = 0x50B;
    
    constexpr uint16_t CONTACT_PHONE_NUMBER_FAX = 0x50C;
    
    constexpr uint16_t CONTACT_PHONE_NUMBER_OTHER = 0x50D;
    
    constexpr uint16_t CONTACT_EMAIL_PERSONAL = 0x50E;
    
    constexpr uint16_t CONTACT_EMAIL_BUSINESS = 0x50F;
    
    constexpr uint16_t CONTACT_EMAIL_OTHER = 0x510;
    
    constexpr uint16_t CONTACT_EMAIL_MAIN = 0x511;
    
    constexpr uint16_t CONTACT_SPEED_DIAL_NUMBER = 0x512;
    
    constexpr uint16_t CONTACT_STATUS_FLAG = 0x513;
    
    constexpr uint16_t CONTACT_MISC = 0x514;

    // ==========================================
    // KIND: Linear Control (Relative/Range) (LC)
    // ==========================================
    
    constexpr uint16_t DISPLAY_BRIGHTNESS = 0x71;
    
    constexpr uint16_t BACKLIGHT_SET_LEVEL = 0x7B;
    
    constexpr uint16_t CHANNEL = 0x86;
    
    constexpr uint16_t TRACKING = 0xBD;
    
    constexpr uint16_t SLOW_TRACKING = 0xBF;
    
    constexpr uint16_t VOLUME = 0xE0;
    
    constexpr uint16_t BALANCE = 0xE1;
    
    constexpr uint16_t BASS = 0xE3;
    
    constexpr uint16_t TREBLE = 0xE4;
    
    constexpr uint16_t FAN_SPEED = 0x101;
    
    constexpr uint16_t LIGHT_ILLUMINATION_LEVEL = 0x103;
    
    constexpr uint16_t ROOM_TEMPERATURE = 0x105;
    
    constexpr uint16_t PROXIMITY = 0x109;
    
    constexpr uint16_t SUB_CHANNEL = 0x170;
    
    constexpr uint16_t AC_ZOOM = 0x22F;
    
    constexpr uint16_t AC_SCROLL = 0x235;
    
    constexpr uint16_t AC_PAN = 0x238;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint16_t RED_MENU_BUTTON = 0x69;
    
    constexpr uint16_t GREEN_MENU_BUTTON = 0x6A;
    
    constexpr uint16_t BLUE_MENU_BUTTON = 0x6B;
    
    constexpr uint16_t YELLOW_MENU_BUTTON = 0x6C;
    
    constexpr uint16_t ENTER_DISC = 0xBB;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint16_t NUMERIC_KEY_PAD = 0x02;
    
    constexpr uint16_t PROGRAMMABLE_BUTTONS = 0x03;
    
    constexpr uint16_t FUNCTION_BUTTONS = 0x36;
    
    constexpr uint16_t SELECTION = 0x80;
    
    constexpr uint16_t MEDIA_SELECTION = 0x87;
    
    constexpr uint16_t SELECT_DISC = 0xBA;
    
    constexpr uint16_t PLAYBACK_SPEED = 0xF1;
    
    constexpr uint16_t APPLICATION_LAUNCH_BUTTONS = 0x180;
    
    constexpr uint16_t GENERIC_GUI_APPLICATION_CONTROLS = 0x200;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint16_t POWER = 0x30;
    
    constexpr uint16_t ILLUMINATION = 0x35;
    
    constexpr uint16_t MENU = 0x40;
    
    constexpr uint16_t DATA_ON_SCREEN = 0x60;
    
    constexpr uint16_t CLOSED_CAPTION = 0x61;
    
    constexpr uint16_t VCR_TV = 0x63;
    
    constexpr uint16_t DISPLAY_BACKLIGHT_TOGGLE = 0x72;
    
    constexpr uint16_t DISPLAY_SET_AUTO_BRIGHTNESS = 0x75;
    
    constexpr uint16_t CAMERA_ACCESS_ENABLED = 0x76;
    
    constexpr uint16_t CAMERA_ACCESS_DISABLED = 0x77;
    
    constexpr uint16_t CAMERA_ACCESS_TOGGLE = 0x78;
    
    constexpr uint16_t BACKLIGHT_OOC = 0x7C;
    
    constexpr uint16_t BACKLIGHT_AUTO = 0x7F;
    
    constexpr uint16_t HELP = 0x95;
    
    constexpr uint16_t PLAY = 0xB0;
    
    constexpr uint16_t PAUSE = 0xB1;
    
    constexpr uint16_t RECORD = 0xB2;
    
    constexpr uint16_t FAST_FORWARD = 0xB3;
    
    constexpr uint16_t REWIND = 0xB4;
    
    constexpr uint16_t RANDOM_PLAY = 0xB9;
    
    constexpr uint16_t REPEAT_FROM_MARK = 0xC4;
    
    constexpr uint16_t START_OR_STOP_VOICE_DICTATION_SESSION = 0xD8;
    
    constexpr uint16_t INVOKE_DISMISS_EMOJI_PICKER = 0xD9;
    
    constexpr uint16_t MUTE = 0xE2;
    
    constexpr uint16_t BASS_BOOST = 0xE5;
    
    constexpr uint16_t LOUDNESS = 0xE7;
    
    constexpr uint16_t MPX = 0xE8;
    
    constexpr uint16_t FAN_ENABLE = 0x100;
    
    constexpr uint16_t LIGHT_ENABLE = 0x102;
    
    constexpr uint16_t CLIMATE_CONTROL_ENABLE = 0x104;
    
    constexpr uint16_t SECURITY_ENABLE = 0x106;
    
    constexpr uint16_t PRIVACY_SCREEN_TOGGLE = 0x2D0;
    
    constexpr uint16_t CONTACT_EDITED = 0x500;
    
    constexpr uint16_t CONTACT_ADDED = 0x501;
    
    constexpr uint16_t CONTACT_RECORD_ACTIVE = 0x502;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint16_t +10 = 0x20;
    
    constexpr uint16_t +100 = 0x21;
    
    constexpr uint16_t AM_PM = 0x22;
    
    constexpr uint16_t RESET = 0x31;
    
    constexpr uint16_t SLEEP = 0x32;
    
    constexpr uint16_t SLEEP_AFTER = 0x33;
    
    constexpr uint16_t MENU_PICK = 0x41;
    
    constexpr uint16_t MENU_UP = 0x42;
    
    constexpr uint16_t MENU_DOWN = 0x43;
    
    constexpr uint16_t MENU_LEFT = 0x44;
    
    constexpr uint16_t MENU_RIGHT = 0x45;
    
    constexpr uint16_t MENU_ESCAPE = 0x46;
    
    constexpr uint16_t MENU_VALUE_INCREASE = 0x47;
    
    constexpr uint16_t MENU_VALUE_DECREASE = 0x48;
    
    constexpr uint16_t CLOSED_CAPTION_SELECT = 0x62;
    
    constexpr uint16_t BROADCAST_MODE = 0x64;
    
    constexpr uint16_t SNAPSHOT = 0x65;
    
    constexpr uint16_t STILL = 0x66;
    
    constexpr uint16_t PICTURE_IN_PICTURE_TOGGLE = 0x67;
    
    constexpr uint16_t PICTURE_IN_PICTURE_SWAP = 0x68;
    
    constexpr uint16_t ASPECT = 0x6D;
    
    constexpr uint16_t ID_3D_MODE_SELECT = 0x6E;
    
    constexpr uint16_t DISPLAY_SET_BRIGHTNESS_TO_MINIMUM = 0x73;
    
    constexpr uint16_t DISPLAY_SET_BRIGHTNESS_TO_MAXIMUM = 0x74;
    
    constexpr uint16_t BRIGHTNESS_INCREMENT = 0x79;
    
    constexpr uint16_t BRIGHTNESS_DECREMENT = 0x7A;
    
    constexpr uint16_t BACKLIGHT_SET_MINIMUM = 0x7D;
    
    constexpr uint16_t BACKLIGHT_SET_MAXIMUM = 0x7E;
    
    constexpr uint16_t ASSIGN_SELECTION = 0x81;
    
    constexpr uint16_t MODE_STEP = 0x82;
    
    constexpr uint16_t RECALL_LAST = 0x83;
    
    constexpr uint16_t ENTER_CHANNEL = 0x84;
    
    constexpr uint16_t ORDER_MOVIE = 0x85;
    
    constexpr uint16_t QUIT = 0x94;
    
    constexpr uint16_t CHANNEL_INCREMENT = 0x9C;
    
    constexpr uint16_t CHANNEL_DECREMENT = 0x9D;
    
    constexpr uint16_t VCR_PLUS = 0xA0;
    
    constexpr uint16_t ONCE = 0xA1;
    
    constexpr uint16_t DAILY = 0xA2;
    
    constexpr uint16_t WEEKLY = 0xA3;
    
    constexpr uint16_t MONTHLY = 0xA4;
    
    constexpr uint16_t SCAN_NEXT_TRACK = 0xB5;
    
    constexpr uint16_t SCAN_PREVIOUS_TRACK = 0xB6;
    
    constexpr uint16_t STOP = 0xB7;
    
    constexpr uint16_t EJECT = 0xB8;
    
    constexpr uint16_t REPEAT = 0xBC;
    
    constexpr uint16_t TRACK_NORMAL = 0xBE;
    
    constexpr uint16_t MARK = 0xC2;
    
    constexpr uint16_t CLEAR_MARK = 0xC3;
    
    constexpr uint16_t RETURN_TO_MARK = 0xC5;
    
    constexpr uint16_t SEARCH_MARK_FORWARD = 0xC6;
    
    constexpr uint16_t SEARCH_MARK_BACKWARDS = 0xC7;
    
    constexpr uint16_t COUNTER_RESET = 0xC8;
    
    constexpr uint16_t SHOW_COUNTER = 0xC9;
    
    constexpr uint16_t STOP_EJECT = 0xCC;
    
    constexpr uint16_t PLAY_PAUSE = 0xCD;
    
    constexpr uint16_t PLAY_SKIP = 0xCE;
    
    constexpr uint16_t VOICE_COMMAND = 0xCF;
    
    constexpr uint16_t SURROUND_MODE = 0xE6;
    
    constexpr uint16_t SPEED_SELECT = 0xF0;
    
    constexpr uint16_t SLOW = 0xF5;
    
    constexpr uint16_t FIRE_ALARM = 0x107;
    
    constexpr uint16_t POLICE_ALARM = 0x108;
    
    constexpr uint16_t MOTION = 0x10A;
    
    constexpr uint16_t DURESS_ALARM = 0x10B;
    
    constexpr uint16_t HOLDUP_ALARM = 0x10C;
    
    constexpr uint16_t MEDICAL_ALARM = 0x10D;
    
    constexpr uint16_t SUB_CHANNEL_INCREMENT = 0x171;
    
    constexpr uint16_t SUB_CHANNEL_DECREMENT = 0x172;
    
    constexpr uint16_t ALTERNATE_AUDIO_INCREMENT = 0x173;
    
    constexpr uint16_t ALTERNATE_AUDIO_DECREMENT = 0x174;
    
    constexpr uint16_t PRIVACY_SCREEN_LEVEL_MINIMUM = 0x2D3;
    
    constexpr uint16_t PRIVACY_SCREEN_LEVEL_MAXIMUM = 0x2D4;
    
    constexpr uint16_t BRIGHTNESS_NEXT = 0x515;
    
    constexpr uint16_t BRIGHTNESS_PREVIOUS = 0x516;

    // ==========================================
    // KIND: Re-trigger Control (RTC)
    // ==========================================
    
    constexpr uint16_t SLEEP_MODE = 0x34;
    
    constexpr uint16_t DISPLAY_BRIGHTNESS_INCREMENT = 0x6F;
    
    constexpr uint16_t DISPLAY_BRIGHTNESS_DECREMENT = 0x70;
    
    constexpr uint16_t FRAME_FORWARD = 0xC0;
    
    constexpr uint16_t FRAME_BACK = 0xC1;
    
    constexpr uint16_t TRACKING_INCREMENT = 0xCA;
    
    constexpr uint16_t TRACKING_DECREMENT = 0xCB;
    
    constexpr uint16_t VOLUME_INCREMENT = 0xE9;
    
    constexpr uint16_t VOLUME_DECREMENT = 0xEA;
    
    constexpr uint16_t BALANCE_RIGHT = 0x150;
    
    constexpr uint16_t BALANCE_LEFT = 0x151;
    
    constexpr uint16_t BASS_INCREMENT = 0x152;
    
    constexpr uint16_t BASS_DECREMENT = 0x153;
    
    constexpr uint16_t TREBLE_INCREMENT = 0x154;
    
    constexpr uint16_t TREBLE_DECREMENT = 0x155;
    
    constexpr uint16_t PRIVACY_SCREEN_LEVEL_DECREMENT = 0x2D1;
    
    constexpr uint16_t PRIVACY_SCREEN_LEVEL_INCREMENT = 0x2D2;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint16_t FORM_FACTOR = 0x2C1;
    
    constexpr uint16_t KEY_TYPE = 0x2C2;
    
    constexpr uint16_t PHYSICAL_LAYOUT = 0x2C3;
    
    constexpr uint16_t VENDOR_SPECIFIC_PHYSICAL_LAYOUT = 0x2C4;
    
    constexpr uint16_t IETF_LANGUAGE_TAG_INDEX = 0x2C5;
    
    constexpr uint16_t IMPLEMENTED_INPUT_ASSIST_CONTROLS = 0x2C6;
    
    constexpr uint16_t BACKLIGHT_LEVEL_SUGGESTION = 0x517;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint16_t MEDIA_SELECT_COMPUTER = 0x88;
    
    constexpr uint16_t MEDIA_SELECT_TV = 0x89;
    
    constexpr uint16_t MEDIA_SELECT_WWW = 0x8A;
    
    constexpr uint16_t MEDIA_SELECT_DVD = 0x8B;
    
    constexpr uint16_t MEDIA_SELECT_TELEPHONE = 0x8C;
    
    constexpr uint16_t MEDIA_SELECT_PROGRAM_GUIDE = 0x8D;
    
    constexpr uint16_t MEDIA_SELECT_VIDEO_PHONE = 0x8E;
    
    constexpr uint16_t MEDIA_SELECT_GAMES = 0x8F;
    
    constexpr uint16_t MEDIA_SELECT_MESSAGES = 0x90;
    
    constexpr uint16_t MEDIA_SELECT_CD = 0x91;
    
    constexpr uint16_t MEDIA_SELECT_VCR = 0x92;
    
    constexpr uint16_t MEDIA_SELECT_TUNER = 0x93;
    
    constexpr uint16_t MEDIA_SELECT_TAPE = 0x96;
    
    constexpr uint16_t MEDIA_SELECT_CABLE = 0x97;
    
    constexpr uint16_t MEDIA_SELECT_SATELLITE = 0x98;
    
    constexpr uint16_t MEDIA_SELECT_SECURITY = 0x99;
    
    constexpr uint16_t MEDIA_SELECT_HOME = 0x9A;
    
    constexpr uint16_t MEDIA_SELECT_CALL = 0x9B;
    
    constexpr uint16_t MEDIA_SELECT_SAP = 0x9E;
    
    constexpr uint16_t INVOKE_CAPTURE_INTERFACE = 0xD0;
    
    constexpr uint16_t START_OR_STOP_GAME_RECORDING = 0xD1;
    
    constexpr uint16_t HISTORICAL_GAME_CAPTURE = 0xD2;
    
    constexpr uint16_t CAPTURE_GAME_SCREENSHOT = 0xD3;
    
    constexpr uint16_t SHOW_OR_HIDE_RECORDING_INDICATOR = 0xD4;
    
    constexpr uint16_t START_OR_STOP_MICROPHONE_CAPTURE = 0xD5;
    
    constexpr uint16_t START_OR_STOP_CAMERA_CAPTURE = 0xD6;
    
    constexpr uint16_t START_OR_STOP_GAME_BROADCAST = 0xD7;
    
    constexpr uint16_t STANDARD_PLAY = 0xF2;
    
    constexpr uint16_t LONG_PLAY = 0xF3;
    
    constexpr uint16_t EXTENDED_PLAY = 0xF4;
    
    constexpr uint16_t AL_LAUNCH_BUTTON_CONFIGURATION_TOOL = 0x181;
    
    constexpr uint16_t AL_PROGRAMMABLE_BUTTON_CONFIGURATION = 0x182;
    
    constexpr uint16_t AL_CONSUMER_CONTROL_CONFIGURATION = 0x183;
    
    constexpr uint16_t AL_WORD_PROCESSOR = 0x184;
    
    constexpr uint16_t AL_TEXT_EDITOR = 0x185;
    
    constexpr uint16_t AL_SPREADSHEET = 0x186;
    
    constexpr uint16_t AL_GRAPHICS_EDITOR = 0x187;
    
    constexpr uint16_t AL_PRESENTATION_APP = 0x188;
    
    constexpr uint16_t AL_DATABASE_APP = 0x189;
    
    constexpr uint16_t AL_EMAIL_READER = 0x18A;
    
    constexpr uint16_t AL_NEWSREADER = 0x18B;
    
    constexpr uint16_t AL_VOICEMAIL = 0x18C;
    
    constexpr uint16_t AL_CONTACTS_ADDRESS_BOOK = 0x18D;
    
    constexpr uint16_t AL_CALENDAR_SCHEDULE = 0x18E;
    
    constexpr uint16_t AL_TASK_PROJECT_MANAGER = 0x18F;
    
    constexpr uint16_t AL_LOG_JOURNAL_TIMECARD = 0x190;
    
    constexpr uint16_t AL_CHECKBOOK_FINANCE = 0x191;
    
    constexpr uint16_t AL_CALCULATOR = 0x192;
    
    constexpr uint16_t AL_A_V_CAPTURE_PLAYBACK = 0x193;
    
    constexpr uint16_t AL_LOCAL_MACHINE_BROWSER = 0x194;
    
    constexpr uint16_t AL_LAN_WAN_BROWSER = 0x195;
    
    constexpr uint16_t AL_INTERNET_BROWSER = 0x196;
    
    constexpr uint16_t AL_REMOTE_NETWORKING_ISP_CONNECT = 0x197;
    
    constexpr uint16_t AL_NETWORK_CONFERENCE = 0x198;
    
    constexpr uint16_t AL_NETWORK_CHAT = 0x199;
    
    constexpr uint16_t AL_TELEPHONY_DIALER = 0x19A;
    
    constexpr uint16_t AL_LOGON = 0x19B;
    
    constexpr uint16_t AL_LOGOFF = 0x19C;
    
    constexpr uint16_t AL_LOGON_LOGOFF = 0x19D;
    
    constexpr uint16_t AL_TERMINAL_LOCK_SCREENSAVER = 0x19E;
    
    constexpr uint16_t AL_CONTROL_PANEL = 0x19F;
    
    constexpr uint16_t AL_COMMAND_LINE_PROCESSOR_RUN = 0x1A0;
    
    constexpr uint16_t AL_PROCESS_TASK_MANAGER = 0x1A1;
    
    constexpr uint16_t AL_SELECT_TASK_APPLICATION = 0x1A2;
    
    constexpr uint16_t AL_NEXT_TASK_APPLICATION = 0x1A3;
    
    constexpr uint16_t AL_PREVIOUS_TASK_APPLICATION = 0x1A4;
    
    constexpr uint16_t AL_PREEMPTIVE_HALT_TASK_APPLICATION = 0x1A5;
    
    constexpr uint16_t AL_INTEGRATED_HELP_CENTER = 0x1A6;
    
    constexpr uint16_t AL_DOCUMENTS = 0x1A7;
    
    constexpr uint16_t AL_THESAURUS = 0x1A8;
    
    constexpr uint16_t AL_DICTIONARY = 0x1A9;
    
    constexpr uint16_t AL_DESKTOP = 0x1AA;
    
    constexpr uint16_t AL_SPELL_CHECK = 0x1AB;
    
    constexpr uint16_t AL_GRAMMAR_CHECK = 0x1AC;
    
    constexpr uint16_t AL_WIRELESS_STATUS = 0x1AD;
    
    constexpr uint16_t AL_LAYOUT = 0x1AE;
    
    constexpr uint16_t AL_VIRUS_PROTECTION = 0x1AF;
    
    constexpr uint16_t AL_ENCRYPTION = 0x1B0;
    
    constexpr uint16_t AL_SCREEN_SAVER = 0x1B1;
    
    constexpr uint16_t AL_ALARMS = 0x1B2;
    
    constexpr uint16_t AL_CLOCK = 0x1B3;
    
    constexpr uint16_t AL_FILE_BROWSER = 0x1B4;
    
    constexpr uint16_t AL_POWER_STATUS = 0x1B5;
    
    constexpr uint16_t AL_IMAGE_BROWSER = 0x1B6;
    
    constexpr uint16_t AL_AUDIO_BROWSER = 0x1B7;
    
    constexpr uint16_t AL_MOVIE_BROWSER = 0x1B8;
    
    constexpr uint16_t AL_DIGITAL_RIGHTS_MANAGER = 0x1B9;
    
    constexpr uint16_t AL_DIGITAL_WALLET = 0x1BA;
    
    constexpr uint16_t AL_INSTANT_MESSAGING = 0x1BC;
    
    constexpr uint16_t AL_OEM_FEATURES_TIPS_TUTORIAL_BROWSER = 0x1BD;
    
    constexpr uint16_t AL_OEM_HELP = 0x1BE;
    
    constexpr uint16_t AL_ONLINE_COMMUNITY = 0x1BF;
    
    constexpr uint16_t AL_ENTERTAINMENT_CONTENT_BROWSER = 0x1C0;
    
    constexpr uint16_t AL_ONLINE_SHOPPING_BROWSER = 0x1C1;
    
    constexpr uint16_t AL_SMARTCARD_INFORMATION_HELP = 0x1C2;
    
    constexpr uint16_t AL_MARKET_MONITOR_FINANCE_BROWSER = 0x1C3;
    
    constexpr uint16_t AL_CUSTOMIZED_CORPORATE_NEWS_BROWSER = 0x1C4;
    
    constexpr uint16_t AL_ONLINE_ACTIVITY_BROWSER = 0x1C5;
    
    constexpr uint16_t AL_RESEARCH_SEARCH_BROWSER = 0x1C6;
    
    constexpr uint16_t AL_AUDIO_PLAYER = 0x1C7;
    
    constexpr uint16_t AL_MESSAGE_STATUS = 0x1C8;
    
    constexpr uint16_t AL_CONTACT_SYNC = 0x1C9;
    
    constexpr uint16_t AL_NAVIGATION = 0x1CA;
    
    constexpr uint16_t AL_CONTEXT_AWARE_DESKTOP_ASSISTANT = 0x1CB;
    
    constexpr uint16_t AC_NEW = 0x201;
    
    constexpr uint16_t AC_OPEN = 0x202;
    
    constexpr uint16_t AC_CLOSE = 0x203;
    
    constexpr uint16_t AC_EXIT = 0x204;
    
    constexpr uint16_t AC_MAXIMIZE = 0x205;
    
    constexpr uint16_t AC_MINIMIZE = 0x206;
    
    constexpr uint16_t AC_SAVE = 0x207;
    
    constexpr uint16_t AC_PRINT = 0x208;
    
    constexpr uint16_t AC_PROPERTIES = 0x209;
    
    constexpr uint16_t AC_UNDO = 0x21A;
    
    constexpr uint16_t AC_COPY = 0x21B;
    
    constexpr uint16_t AC_CUT = 0x21C;
    
    constexpr uint16_t AC_PASTE = 0x21D;
    
    constexpr uint16_t AC_SELECT_ALL = 0x21E;
    
    constexpr uint16_t AC_FIND = 0x21F;
    
    constexpr uint16_t AC_FIND_AND_REPLACE = 0x220;
    
    constexpr uint16_t AC_SEARCH = 0x221;
    
    constexpr uint16_t AC_GO_TO = 0x222;
    
    constexpr uint16_t AC_HOME = 0x223;
    
    constexpr uint16_t AC_BACK = 0x224;
    
    constexpr uint16_t AC_FORWARD = 0x225;
    
    constexpr uint16_t AC_STOP = 0x226;
    
    constexpr uint16_t AC_REFRESH = 0x227;
    
    constexpr uint16_t AC_PREVIOUS_LINK = 0x228;
    
    constexpr uint16_t AC_NEXT_LINK = 0x229;
    
    constexpr uint16_t AC_BOOKMARKS = 0x22A;
    
    constexpr uint16_t AC_HISTORY = 0x22B;
    
    constexpr uint16_t AC_SUBSCRIPTIONS = 0x22C;
    
    constexpr uint16_t AC_ZOOM_IN = 0x22D;
    
    constexpr uint16_t AC_ZOOM_OUT = 0x22E;
    
    constexpr uint16_t AC_FULL_SCREEN_VIEW = 0x230;
    
    constexpr uint16_t AC_NORMAL_VIEW = 0x231;
    
    constexpr uint16_t AC_VIEW_TOGGLE = 0x232;
    
    constexpr uint16_t AC_SCROLL_UP = 0x233;
    
    constexpr uint16_t AC_SCROLL_DOWN = 0x234;
    
    constexpr uint16_t AC_PAN_LEFT = 0x236;
    
    constexpr uint16_t AC_PAN_RIGHT = 0x237;
    
    constexpr uint16_t AC_NEW_WINDOW = 0x239;
    
    constexpr uint16_t AC_TILE_HORIZONTALLY = 0x23A;
    
    constexpr uint16_t AC_TILE_VERTICALLY = 0x23B;
    
    constexpr uint16_t AC_FORMAT = 0x23C;
    
    constexpr uint16_t AC_EDIT = 0x23D;
    
    constexpr uint16_t AC_BOLD = 0x23E;
    
    constexpr uint16_t AC_ITALICS = 0x23F;
    
    constexpr uint16_t AC_UNDERLINE = 0x240;
    
    constexpr uint16_t AC_STRIKETHROUGH = 0x241;
    
    constexpr uint16_t AC_SUBSCRIPT = 0x242;
    
    constexpr uint16_t AC_SUPERSCRIPT = 0x243;
    
    constexpr uint16_t AC_ALL_CAPS = 0x244;
    
    constexpr uint16_t AC_ROTATE = 0x245;
    
    constexpr uint16_t AC_RESIZE = 0x246;
    
    constexpr uint16_t AC_FLIP_HORIZONTAL = 0x247;
    
    constexpr uint16_t AC_FLIP_VERTICAL = 0x248;
    
    constexpr uint16_t AC_MIRROR_HORIZONTAL = 0x249;
    
    constexpr uint16_t AC_MIRROR_VERTICAL = 0x24A;
    
    constexpr uint16_t AC_FONT_SELECT = 0x24B;
    
    constexpr uint16_t AC_FONT_COLOR = 0x24C;
    
    constexpr uint16_t AC_FONT_SIZE = 0x24D;
    
    constexpr uint16_t AC_JUSTIFY_LEFT = 0x24E;
    
    constexpr uint16_t AC_JUSTIFY_CENTER_H = 0x24F;
    
    constexpr uint16_t AC_JUSTIFY_RIGHT = 0x250;
    
    constexpr uint16_t AC_JUSTIFY_BLOCK_H = 0x251;
    
    constexpr uint16_t AC_JUSTIFY_TOP = 0x252;
    
    constexpr uint16_t AC_JUSTIFY_CENTER_V = 0x253;
    
    constexpr uint16_t AC_JUSTIFY_BOTTOM = 0x254;
    
    constexpr uint16_t AC_JUSTIFY_BLOCK_V = 0x255;
    
    constexpr uint16_t AC_INDENT_DECREASE = 0x256;
    
    constexpr uint16_t AC_INDENT_INCREASE = 0x257;
    
    constexpr uint16_t AC_NUMBERED_LIST = 0x258;
    
    constexpr uint16_t AC_RESTART_NUMBERING = 0x259;
    
    constexpr uint16_t AC_BULLETED_LIST = 0x25A;
    
    constexpr uint16_t AC_PROMOTE = 0x25B;
    
    constexpr uint16_t AC_DEMOTE = 0x25C;
    
    constexpr uint16_t AC_YES = 0x25D;
    
    constexpr uint16_t AC_NO = 0x25E;
    
    constexpr uint16_t AC_CANCEL = 0x25F;
    
    constexpr uint16_t AC_CATALOG = 0x260;
    
    constexpr uint16_t AC_BUY_CHECKOUT = 0x261;
    
    constexpr uint16_t AC_ADD_TO_CART = 0x262;
    
    constexpr uint16_t AC_EXPAND = 0x263;
    
    constexpr uint16_t AC_EXPAND_ALL = 0x264;
    
    constexpr uint16_t AC_COLLAPSE = 0x265;
    
    constexpr uint16_t AC_COLLAPSE_ALL = 0x266;
    
    constexpr uint16_t AC_PRINT_PREVIEW = 0x267;
    
    constexpr uint16_t AC_PASTE_SPECIAL = 0x268;
    
    constexpr uint16_t AC_INSERT_MODE = 0x269;
    
    constexpr uint16_t AC_DELETE = 0x26A;
    
    constexpr uint16_t AC_LOCK = 0x26B;
    
    constexpr uint16_t AC_UNLOCK = 0x26C;
    
    constexpr uint16_t AC_PROTECT = 0x26D;
    
    constexpr uint16_t AC_UNPROTECT = 0x26E;
    
    constexpr uint16_t AC_ATTACH_COMMENT = 0x26F;
    
    constexpr uint16_t AC_DELETE_COMMENT = 0x270;
    
    constexpr uint16_t AC_VIEW_COMMENT = 0x271;
    
    constexpr uint16_t AC_SELECT_WORD = 0x272;
    
    constexpr uint16_t AC_SELECT_SENTENCE = 0x273;
    
    constexpr uint16_t AC_SELECT_PARAGRAPH = 0x274;
    
    constexpr uint16_t AC_SELECT_COLUMN = 0x275;
    
    constexpr uint16_t AC_SELECT_ROW = 0x276;
    
    constexpr uint16_t AC_SELECT_TABLE = 0x277;
    
    constexpr uint16_t AC_SELECT_OBJECT = 0x278;
    
    constexpr uint16_t AC_REDO_REPEAT = 0x279;
    
    constexpr uint16_t AC_SORT = 0x27A;
    
    constexpr uint16_t AC_SORT_ASCENDING = 0x27B;
    
    constexpr uint16_t AC_SORT_DESCENDING = 0x27C;
    
    constexpr uint16_t AC_FILTER = 0x27D;
    
    constexpr uint16_t AC_SET_CLOCK = 0x27E;
    
    constexpr uint16_t AC_VIEW_CLOCK = 0x27F;
    
    constexpr uint16_t AC_SELECT_TIME_ZONE = 0x280;
    
    constexpr uint16_t AC_EDIT_TIME_ZONES = 0x281;
    
    constexpr uint16_t AC_SET_ALARM = 0x282;
    
    constexpr uint16_t AC_CLEAR_ALARM = 0x283;
    
    constexpr uint16_t AC_SNOOZE_ALARM = 0x284;
    
    constexpr uint16_t AC_RESET_ALARM = 0x285;
    
    constexpr uint16_t AC_SYNCHRONIZE = 0x286;
    
    constexpr uint16_t AC_SEND_RECEIVE = 0x287;
    
    constexpr uint16_t AC_SEND_TO = 0x288;
    
    constexpr uint16_t AC_REPLY = 0x289;
    
    constexpr uint16_t AC_REPLY_ALL = 0x28A;
    
    constexpr uint16_t AC_FORWARD_MSG = 0x28B;
    
    constexpr uint16_t AC_SEND = 0x28C;
    
    constexpr uint16_t AC_ATTACH_FILE = 0x28D;
    
    constexpr uint16_t AC_UPLOAD = 0x28E;
    
    constexpr uint16_t AC_DOWNLOAD_SAVE_TARGET_AS = 0x28F;
    
    constexpr uint16_t AC_SET_BORDERS = 0x290;
    
    constexpr uint16_t AC_INSERT_ROW = 0x291;
    
    constexpr uint16_t AC_INSERT_COLUMN = 0x292;
    
    constexpr uint16_t AC_INSERT_FILE = 0x293;
    
    constexpr uint16_t AC_INSERT_PICTURE = 0x294;
    
    constexpr uint16_t AC_INSERT_OBJECT = 0x295;
    
    constexpr uint16_t AC_INSERT_SYMBOL = 0x296;
    
    constexpr uint16_t AC_SAVE_AND_CLOSE = 0x297;
    
    constexpr uint16_t AC_RENAME = 0x298;
    
    constexpr uint16_t AC_MERGE = 0x299;
    
    constexpr uint16_t AC_SPLIT = 0x29A;
    
    constexpr uint16_t AC_DISRIBUTE_HORIZONTALLY = 0x29B;
    
    constexpr uint16_t AC_DISTRIBUTE_VERTICALLY = 0x29C;
    
    constexpr uint16_t AC_NEXT_LAYOUT_SELECT = 0x29D;
    
    constexpr uint16_t AC_NAVIGATION_GUIDANCE = 0x29E;
    
    constexpr uint16_t AC_DESKTOP_SHOW_ALL_WINDOWS = 0x29F;
    
    constexpr uint16_t AC_SOFT_KEY_LEFT = 0x2A0;
    
    constexpr uint16_t AC_SOFT_KEY_RIGHT = 0x2A1;
    
    constexpr uint16_t AC_DESKTOP_SHOW_ALL_APPLICATIONS = 0x2A2;
    
    constexpr uint16_t AC_IDLE_KEEP_ALIVE = 0x2B0;
    
    constexpr uint16_t INPUT_ASSIST_PREVIOUS = 0x2C7;
    
    constexpr uint16_t INPUT_ASSIST_NEXT = 0x2C8;
    
    constexpr uint16_t INPUT_ASSIST_PREVIOUS_GROUP = 0x2C9;
    
    constexpr uint16_t INPUT_ASSIST_NEXT_GROUP = 0x2CA;
    
    constexpr uint16_t INPUT_ASSIST_ACCEPT = 0x2CB;
    
    constexpr uint16_t INPUT_ASSIST_CANCEL = 0x2CC;

}
