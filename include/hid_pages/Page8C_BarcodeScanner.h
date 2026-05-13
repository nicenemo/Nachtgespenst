#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Barcode Scanner (0x8C)
 */
namespace HID::BarcodeScanner {
    constexpr uint8_t PAGE_ID = 0x8C;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t BARCODE_BADGE_READER = 0x01;
    
    constexpr uint8_t BARCODE_SCANNER = 0x02;
    
    constexpr uint8_t DUMB_BAR_CODE_SCANNER = 0x03;
    
    constexpr uint8_t CORDLESS_SCANNER_BASE = 0x04;
    
    constexpr uint8_t BAR_CODE_SCANNER_CRADLE = 0x05;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t ATTRIBUTE_REPORT = 0x10;
    
    constexpr uint8_t SETTINGS_REPORT = 0x11;
    
    constexpr uint8_t SCANNED_DATA_REPORT = 0x12;
    
    constexpr uint8_t RAW_SCANNED_DATA_REPORT = 0x13;
    
    constexpr uint8_t TRIGGER_REPORT = 0x14;
    
    constexpr uint8_t STATUS_REPORT = 0x15;
    
    constexpr uint8_t UPC_EAN_CONTROL_REPORT = 0x16;
    
    constexpr uint8_t EAN_2_3_LABEL_CONTROL_REPORT = 0x17;
    
    constexpr uint8_t CODE_39_CONTROL_REPORT = 0x18;
    
    constexpr uint8_t INTERLEAVED_2_OF_5_CONTROL_REPORT = 0x19;
    
    constexpr uint8_t STANDARD_2_OF_5_CONTROL_REPORT = 0x1A;
    
    constexpr uint8_t MSI_PLESSEY_CONTROL_REPORT = 0x1B;
    
    constexpr uint8_t CODABAR_CONTROL_REPORT = 0x1C;
    
    constexpr uint8_t CODE_128_CONTROL_REPORT = 0x1D;
    
    constexpr uint8_t MISC_1D_CONTROL_REPORT = 0x1E;
    
    constexpr uint8_t ID_2D_CONTROL_REPORT = 0x1F;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t FRAGMENT_DECODING = 0x4D;
    
    constexpr uint8_t AIMING_LASER_PATTERN = 0x56;
    
    constexpr uint8_t POWER_ON_RESET_SCANNER = 0x5E;
    
    constexpr uint8_t PREVENT_READ_OF_BARCODES = 0x5F;
    
    constexpr uint8_t INITIATE_BARCODE_READ = 0x60;
    
    constexpr uint8_t COMMIT_PARAMETERS_TO_NVM = 0x6D;
    
    constexpr uint8_t PARAMETER_SCANNING = 0x6E;
    
    constexpr uint8_t SET_PARAMETER_DEFAULT_VALUES = 0x70;
    
    constexpr uint8_t GOOD_READ_LED = 0x7D;
    
    constexpr uint8_t NO_READ_MESSAGE = 0x82;
    
    constexpr uint8_t POWERUP_BEEP = 0x84;
    
    constexpr uint8_t SOUND_ERROR_BEEP = 0x85;
    
    constexpr uint8_t SOUND_GOOD_READ_BEEP = 0x86;
    
    constexpr uint8_t SOUND_NOT_ON_FILE_BEEP = 0x87;
    
    constexpr uint8_t BOOKLAND_EAN = 0x91;
    
    constexpr uint8_t CONVERT_EAN_8_TO_13_TYPE = 0x92;
    
    constexpr uint8_t CONVERT_UPC_A_TO_EAN_13 = 0x93;
    
    constexpr uint8_t CONVERT_UPC_E_TO_A = 0x94;
    
    constexpr uint8_t EAN_13 = 0x95;
    
    constexpr uint8_t EAN_8 = 0x96;
    
    constexpr uint8_t EAN_99_128_MANDATORY = 0x97;
    
    constexpr uint8_t EAN_99_P5_128_OPTIONAL = 0x98;
    
    constexpr uint8_t ENABLE_EAN_TWO_LABEL = 0x99;
    
    constexpr uint8_t UPC_EAN = 0x9A;
    
    constexpr uint8_t UPC_EAN_COUPON_CODE = 0x9B;
    
    constexpr uint8_t UPC_A = 0x9D;
    
    constexpr uint8_t UPC_A_WITH_128_MANDATORY = 0x9E;
    
    constexpr uint8_t UPC_A_WITH_128_OPTIONAL = 0x9F;
    
    constexpr uint8_t UPC_A_WITH_P5_OPTIONAL = 0xA0;
    
    constexpr uint8_t UPC_E = 0xA1;
    
    constexpr uint8_t UPC_E1 = 0xA2;
    
    constexpr uint8_t EAN_TWO_LABEL = 0xB7;
    
    constexpr uint8_t EAN_THREE_LABEL = 0xB8;
    
    constexpr uint8_t ADD_EAN_2_3_LABEL_DEFINITION = 0xBF;
    
    constexpr uint8_t CLEAR_ALL_EAN_2_3_LABEL_DEFINITIONS = 0xC0;
    
    constexpr uint8_t CODABAR = 0xC3;
    
    constexpr uint8_t CODE_128 = 0xC4;
    
    constexpr uint8_t CODE_39 = 0xC7;
    
    constexpr uint8_t CODE_93 = 0xC8;
    
    constexpr uint8_t FULL_ASCII_CONVERSION = 0xC9;
    
    constexpr uint8_t INTERLEAVED_2_OF_5 = 0xCA;
    
    constexpr uint8_t ITALIAN_PHARMACY_CODE = 0xCB;
    
    constexpr uint8_t MSI_PLESSEY = 0xCC;
    
    constexpr uint8_t STANDARD_2_OF_5_IATA = 0xCD;
    
    constexpr uint8_t STANDARD_2_OF_5 = 0xCE;
    
    constexpr uint8_t TRANSMIT_START_STOP = 0xD3;
    
    constexpr uint8_t TRI_OPTIC = 0xD4;
    
    constexpr uint8_t UCC_EAN_128 = 0xD5;
    
    constexpr uint8_t DECODE_DATA_CONTINUED = 0xFF;
    
    constexpr uint8_t AZTEC_CODE = 0x110;
    
    constexpr uint8_t BC412 = 0x111;
    
    constexpr uint8_t CHANNEL_CODE = 0x112;
    
    constexpr uint8_t CODE_16 = 0x113;
    
    constexpr uint8_t CODE_32 = 0x114;
    
    constexpr uint8_t CODE_49 = 0x115;
    
    constexpr uint8_t CODE_ONE = 0x116;
    
    constexpr uint8_t COLORCODE = 0x117;
    
    constexpr uint8_t DATA_MATRIX = 0x118;
    
    constexpr uint8_t MAXICODE = 0x119;
    
    constexpr uint8_t MICROPDF = 0x11A;
    
    constexpr uint8_t PDF_417 = 0x11B;
    
    constexpr uint8_t POSICODE = 0x11C;
    
    constexpr uint8_t QR_CODE = 0x11D;
    
    constexpr uint8_t SUPERCODE = 0x11E;
    
    constexpr uint8_t ULTRACODE = 0x11F;
    
    constexpr uint8_t USD_5_SLUG_CODE = 0x120;
    
    constexpr uint8_t VERICODE = 0x121;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t SCANNER_READ_CONFIDENCE = 0x4E;
    
    constexpr uint8_t ACTIVE_TIME = 0x55;
    
    constexpr uint8_t LASER_ON_TIME = 0x59;
    
    constexpr uint8_t LOCKOUT_TIME = 0x5B;
    
    constexpr uint8_t MOTOR_TIMEOUT = 0x5D;
    
    constexpr uint8_t AIM_DURATION = 0x7A;
    
    constexpr uint8_t GOOD_READ_LAMP_DURATION = 0x7B;
    
    constexpr uint8_t GOOD_READ_LAMP_INTENSITY = 0x7C;
    
    constexpr uint8_t GOOD_READ_TONE_FREQUENCY = 0x7E;
    
    constexpr uint8_t GOOD_READ_TONE_LENGTH = 0x7F;
    
    constexpr uint8_t GOOD_READ_TONE_VOLUME = 0x80;
    
    constexpr uint8_t NOT_ON_FILE_VOLUME = 0x83;
    
    constexpr uint8_t UPC_EAN_PERIODICALS = 0x9C;
    
    constexpr uint8_t EAN_8_FLAG_DIGIT_1 = 0xB9;
    
    constexpr uint8_t EAN_8_FLAG_DIGIT_2 = 0xBA;
    
    constexpr uint8_t EAN_8_FLAG_DIGIT_3 = 0xBB;
    
    constexpr uint8_t EAN_13_FLAG_DIGIT_1 = 0xBC;
    
    constexpr uint8_t EAN_13_FLAG_DIGIT_2 = 0xBD;
    
    constexpr uint8_t EAN_13_FLAG_DIGIT_3 = 0xBE;
    
    constexpr uint8_t SYMBOLOGY_IDENTIFIER_1 = 0xFB;
    
    constexpr uint8_t SYMBOLOGY_IDENTIFIER_2 = 0xFC;
    
    constexpr uint8_t SYMBOLOGY_IDENTIFIER_3 = 0xFD;
    
    constexpr uint8_t DECODED_DATA = 0xFE;
    
    constexpr uint8_t BAR_SPACE_DATA = 0x100;
    
    constexpr uint8_t SCANNER_DATA_ACCURACY = 0x101;
    
    constexpr uint8_t MINIMUM_LENGTH_TO_DECODE = 0x106;
    
    constexpr uint8_t MAXIMUM_LENGTH_TO_DECODE = 0x107;
    
    constexpr uint8_t DISCRETE_LENGTH_TO_DECODE_1 = 0x108;
    
    constexpr uint8_t DISCRETE_LENGTH_TO_DECODE_2 = 0x109;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t DATA_PREFIX = 0x4F;
    
    constexpr uint8_t TRIGGER_MODE = 0x62;
    
    constexpr uint8_t GOOD_READ_WHEN_TO_WRITE = 0x88;
    
    constexpr uint8_t PERIODICAL = 0xA9;
    
    constexpr uint8_t CHECK = 0xB0;
    
    constexpr uint8_t CHECK_DIGIT = 0xD6;
    
    constexpr uint8_t TRANSMIT_CHECK_DIGIT = 0xF0;
    
    constexpr uint8_t RAW_DATA_POLARITY = 0x102;
    
    constexpr uint8_t DATA_LENGTH_METHOD = 0x10A;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t BAR_CODE_PRESENT = 0x57;
    
    constexpr uint8_t BEEPER_STATE = 0x58;
    
    constexpr uint8_t LASER_STATE = 0x5A;
    
    constexpr uint8_t MOTOR_STATE = 0x5C;
    
    constexpr uint8_t TRIGGER_STATE = 0x61;
    
    constexpr uint8_t PARAMETERS_CHANGED = 0x6F;
    
    constexpr uint8_t SCANNER_IN_CRADLE = 0x75;
    
    constexpr uint8_t SCANNER_IN_RANGE = 0x76;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint8_t AIMING_POINTER_MODE = 0x30;
    
    constexpr uint8_t BAR_CODE_PRESENT_SENSOR = 0x31;
    
    constexpr uint8_t CLASS_1A_LASER = 0x32;
    
    constexpr uint8_t CLASS_2_LASER = 0x33;
    
    constexpr uint8_t HEATER_PRESENT = 0x34;
    
    constexpr uint8_t CONTACT_SCANNER = 0x35;
    
    constexpr uint8_t ELECTRONIC_ARTICLE_SURVEILLANCE_NOTIFICATION = 0x36;
    
    constexpr uint8_t CONSTANT_ELECTRONIC_ARTICLE_SURVEILLANCE = 0x37;
    
    constexpr uint8_t ERROR_INDICATION = 0x38;
    
    constexpr uint8_t FIXED_BEEPER = 0x39;
    
    constexpr uint8_t GOOD_DECODE_INDICATION = 0x3A;
    
    constexpr uint8_t HANDS_FREE_SCANNING = 0x3B;
    
    constexpr uint8_t INTRINSICALLY_SAFE = 0x3C;
    
    constexpr uint8_t KLASSE_EINS_LASER = 0x3D;
    
    constexpr uint8_t LONG_RANGE_SCANNER = 0x3E;
    
    constexpr uint8_t MIRROR_SPEED_CONTROL = 0x3F;
    
    constexpr uint8_t NOT_ON_FILE_INDICATION = 0x40;
    
    constexpr uint8_t PROGRAMMABLE_BEEPER = 0x41;
    
    constexpr uint8_t TRIGGERLESS = 0x42;
    
    constexpr uint8_t WAND = 0x43;
    
    constexpr uint8_t WATER_RESISTANT = 0x44;
    
    constexpr uint8_t MULTI_RANGE_SCANNER = 0x45;
    
    constexpr uint8_t PROXIMITY_SENSOR = 0x46;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t PREFIX_AIMI = 0x50;
    
    constexpr uint8_t PREFIX_NONE = 0x51;
    
    constexpr uint8_t PREFIX_PROPRIETARY = 0x52;
    
    constexpr uint8_t TRIGGER_MODE_BLINKING_LASER_ON = 0x63;
    
    constexpr uint8_t TRIGGER_MODE_CONTINUOUS_LASER_ON = 0x64;
    
    constexpr uint8_t TRIGGER_MODE_LASER_ON_WHILE_PULLED = 0x65;
    
    constexpr uint8_t TRIGGER_MODE_LASER_STAYS_ON_AFTER_RELEASE = 0x66;
    
    constexpr uint8_t GRWTI_AFTER_DECODE = 0x89;
    
    constexpr uint8_t GRWTI_BEEP_LAMP_AFTER_TRANSMIT = 0x8A;
    
    constexpr uint8_t GRWTI_NO_BEEP_LAMP_USE_AT_ALL = 0x8B;
    
    constexpr uint8_t PERIODICAL_AUTO_DISCRIMINATE_+2 = 0xAA;
    
    constexpr uint8_t PERIODICAL_ONLY_DECODE_WITH_+2 = 0xAB;
    
    constexpr uint8_t PERIODICAL_IGNORE_+2 = 0xAC;
    
    constexpr uint8_t PERIODICAL_AUTO_DISCRIMINATE_+5 = 0xAD;
    
    constexpr uint8_t PERIODICAL_ONLY_DECODE_WITH_+5 = 0xAE;
    
    constexpr uint8_t PERIODICAL_IGNORE_+5 = 0xAF;
    
    constexpr uint8_t CHECK_DISABLE_PRICE = 0xB1;
    
    constexpr uint8_t CHECK_ENABLE_4_DIGIT_PRICE = 0xB2;
    
    constexpr uint8_t CHECK_ENABLE_5_DIGIT_PRICE = 0xB3;
    
    constexpr uint8_t CHECK_ENABLE_EUROPEAN_4_DIGIT_PRICE = 0xB4;
    
    constexpr uint8_t CHECK_ENABLE_EUROPEAN_5_DIGIT_PRICE = 0xB5;
    
    constexpr uint8_t CHECK_DIGIT_DISABLE = 0xD7;
    
    constexpr uint8_t CHECK_DIGIT_ENABLE_INTERLEAVED_2_OF_5_OPCC = 0xD8;
    
    constexpr uint8_t CHECK_DIGIT_ENABLE_INTERLEAVED_2_OF_5_USS = 0xD9;
    
    constexpr uint8_t CHECK_DIGIT_ENABLE_STANDARD_2_OF_5_OPCC = 0xDA;
    
    constexpr uint8_t CHECK_DIGIT_ENABLE_STANDARD_2_OF_5_USS = 0xDB;
    
    constexpr uint8_t CHECK_DIGIT_ENABLE_ONE_MSI_PLESSEY = 0xDC;
    
    constexpr uint8_t CHECK_DIGIT_ENABLE_TWO_MSI_PLESSEY = 0xDD;
    
    constexpr uint8_t CHECK_DIGIT_CODABAR_ENABLE = 0xDE;
    
    constexpr uint8_t CHECK_DIGIT_CODE_39_ENABLE = 0xDF;
    
    constexpr uint8_t DISABLE_CHECK_DIGIT_TRANSMIT = 0xF1;
    
    constexpr uint8_t ENABLE_CHECK_DIGIT_TRANSMIT = 0xF2;
    
    constexpr uint8_t POLARITY_INVERTED_BAR_CODE = 0x103;
    
    constexpr uint8_t POLARITY_NORMAL_BAR_CODE = 0x104;
    
    constexpr uint8_t DL_METHOD_READ_ANY = 0x10B;
    
    constexpr uint8_t DL_METHOD_CHECK_IN_RANGE = 0x10C;
    
    constexpr uint8_t DL_METHOD_CHECK_FOR_DISCRETE = 0x10D;

}
