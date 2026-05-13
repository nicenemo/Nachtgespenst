#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Sensors (0x20)
 */
namespace HID::Sensors {
    constexpr uint8_t PAGE_ID = 0x20;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint16_t SENSOR = 0x01;
    
    constexpr uint16_t BIOMETRIC = 0x10;
    
    constexpr uint16_t BIOMETRIC:_HUMAN_PRESENCE = 0x11;
    
    constexpr uint16_t BIOMETRIC:_HUMAN_PROXIMITY = 0x12;
    
    constexpr uint16_t BIOMETRIC:_HUMAN_TOUCH = 0x13;
    
    constexpr uint16_t BIOMETRIC:_BLOOD_PRESSURE = 0x14;
    
    constexpr uint16_t BIOMETRIC:_BODY_TEMPERATURE = 0x15;
    
    constexpr uint16_t BIOMETRIC:_HEART_RATE = 0x16;
    
    constexpr uint16_t BIOMETRIC:_HEART_RATE_VARIABILITY = 0x17;
    
    constexpr uint16_t BIOMETRIC:_PERIPHERAL_OXYGEN_SATURATION = 0x18;
    
    constexpr uint16_t BIOMETRIC:_RESPIRATORY_RATE = 0x19;
    
    constexpr uint16_t ELECTRICAL = 0x20;
    
    constexpr uint16_t ELECTRICAL:_CAPACITANCE = 0x21;
    
    constexpr uint16_t ELECTRICAL:_CURRENT = 0x22;
    
    constexpr uint16_t ELECTRICAL:_POWER = 0x23;
    
    constexpr uint16_t ELECTRICAL:_INDUCTANCE = 0x24;
    
    constexpr uint16_t ELECTRICAL:_RESISTANCE = 0x25;
    
    constexpr uint16_t ELECTRICAL:_VOLTAGE = 0x26;
    
    constexpr uint16_t ELECTRICAL:_POTENTIOMETER = 0x27;
    
    constexpr uint16_t ELECTRICAL:_FREQUENCY = 0x28;
    
    constexpr uint16_t ELECTRICAL:_PERIOD = 0x29;
    
    constexpr uint16_t ENVIRONMENTAL = 0x30;
    
    constexpr uint16_t ENVIRONMENTAL:_ATMOSPHERIC_PRESSURE = 0x31;
    
    constexpr uint16_t ENVIRONMENTAL:_HUMIDITY = 0x32;
    
    constexpr uint16_t ENVIRONMENTAL:_TEMPERATURE = 0x33;
    
    constexpr uint16_t ENVIRONMENTAL:_WIND_DIRECTION = 0x34;
    
    constexpr uint16_t ENVIRONMENTAL:_WIND_SPEED = 0x35;
    
    constexpr uint16_t ENVIRONMENTAL:_AIR_QUALITY = 0x36;
    
    constexpr uint16_t ENVIRONMENTAL:_HEAT_INDEX = 0x37;
    
    constexpr uint16_t ENVIRONMENTAL:_SURFACE_TEMPERATURE = 0x38;
    
    constexpr uint16_t ENVIRONMENTAL:_VOLATILE_ORGANIC_COMPOUNDS = 0x39;
    
    constexpr uint16_t ENVIRONMENTAL:_OBJECT_PRESENCE = 0x3A;
    
    constexpr uint16_t ENVIRONMENTAL:_OBJECT_PROXIMITY = 0x3B;
    
    constexpr uint16_t LIGHT = 0x40;
    
    constexpr uint16_t LIGHT:_AMBIENT_LIGHT = 0x41;
    
    constexpr uint16_t LIGHT:_CONSUMER_INFRARED = 0x42;
    
    constexpr uint16_t LIGHT:_INFRARED_LIGHT = 0x43;
    
    constexpr uint16_t LIGHT:_VISIBLE_LIGHT = 0x44;
    
    constexpr uint16_t LIGHT:_ULTRAVIOLET_LIGHT = 0x45;
    
    constexpr uint16_t LOCATION = 0x50;
    
    constexpr uint16_t LOCATION:_BROADCAST = 0x51;
    
    constexpr uint16_t LOCATION:_DEAD_RECKONING = 0x52;
    
    constexpr uint16_t LOCATION:_GPS_GLOBAL_POSITIONING_SYSTEM = 0x53;
    
    constexpr uint16_t LOCATION:_LOOKUP = 0x54;
    
    constexpr uint16_t LOCATION:_OTHER = 0x55;
    
    constexpr uint16_t LOCATION:_STATIC = 0x56;
    
    constexpr uint16_t LOCATION:_TRIANGULATION = 0x57;
    
    constexpr uint16_t MECHANICAL = 0x60;
    
    constexpr uint16_t MECHANICAL:_BOOLEAN_SWITCH = 0x61;
    
    constexpr uint16_t MECHANICAL:_BOOLEAN_SWITCH_ARRAY = 0x62;
    
    constexpr uint16_t MECHANICAL:_MULTIVALUE_SWITCH = 0x63;
    
    constexpr uint16_t MECHANICAL:_FORCE = 0x64;
    
    constexpr uint16_t MECHANICAL:_PRESSURE = 0x65;
    
    constexpr uint16_t MECHANICAL:_STRAIN = 0x66;
    
    constexpr uint16_t MECHANICAL:_WEIGHT = 0x67;
    
    constexpr uint16_t MECHANICAL:_HAPTIC_VIBRATOR = 0x68;
    
    constexpr uint16_t MECHANICAL:_HALL_EFFECT_SWITCH = 0x69;
    
    constexpr uint16_t MOTION = 0x70;
    
    constexpr uint16_t MOTION:_ACCELEROMETER_1D = 0x71;
    
    constexpr uint16_t MOTION:_ACCELEROMETER_2D = 0x72;
    
    constexpr uint16_t MOTION:_ACCELEROMETER_3D = 0x73;
    
    constexpr uint16_t MOTION:_GYROMETER_1D = 0x74;
    
    constexpr uint16_t MOTION:_GYROMETER_2D = 0x75;
    
    constexpr uint16_t MOTION:_GYROMETER_3D = 0x76;
    
    constexpr uint16_t MOTION:_MOTION_DETECTOR = 0x77;
    
    constexpr uint16_t MOTION:_SPEEDOMETER = 0x78;
    
    constexpr uint16_t MOTION:_ACCELEROMETER = 0x79;
    
    constexpr uint16_t MOTION:_GYROMETER = 0x7A;
    
    constexpr uint16_t MOTION:_GRAVITY_VECTOR = 0x7B;
    
    constexpr uint16_t MOTION:_LINEAR_ACCELEROMETER = 0x7C;
    
    constexpr uint16_t ORIENTATION = 0x80;
    
    constexpr uint16_t ORIENTATION:_COMPASS_1D = 0x81;
    
    constexpr uint16_t ORIENTATION:_COMPASS_2D = 0x82;
    
    constexpr uint16_t ORIENTATION:_COMPASS_3D = 0x83;
    
    constexpr uint16_t ORIENTATION:_INCLINOMETER_1D = 0x84;
    
    constexpr uint16_t ORIENTATION:_INCLINOMETER_2D = 0x85;
    
    constexpr uint16_t ORIENTATION:_INCLINOMETER_3D = 0x86;
    
    constexpr uint16_t ORIENTATION:_DISTANCE_1D = 0x87;
    
    constexpr uint16_t ORIENTATION:_DISTANCE_2D = 0x88;
    
    constexpr uint16_t ORIENTATION:_DISTANCE_3D = 0x89;
    
    constexpr uint16_t ORIENTATION:_DEVICE_ORIENTATION = 0x8A;
    
    constexpr uint16_t ORIENTATION:_COMPASS = 0x8B;
    
    constexpr uint16_t ORIENTATION:_INCLINOMETER = 0x8C;
    
    constexpr uint16_t ORIENTATION:_DISTANCE = 0x8D;
    
    constexpr uint16_t ORIENTATION:_RELATIVE_ORIENTATION = 0x8E;
    
    constexpr uint16_t ORIENTATION:_SIMPLE_ORIENTATION = 0x8F;
    
    constexpr uint16_t SCANNER = 0x90;
    
    constexpr uint16_t SCANNER:_BARCODE = 0x91;
    
    constexpr uint16_t SCANNER:_RFID = 0x92;
    
    constexpr uint16_t SCANNER:_NFC = 0x93;
    
    constexpr uint16_t TIME = 0xA0;
    
    constexpr uint16_t TIME:_ALARM_TIMER = 0xA1;
    
    constexpr uint16_t TIME:_REAL_TIME_CLOCK = 0xA2;
    
    constexpr uint16_t PERSONAL_ACTIVITY = 0xB0;
    
    constexpr uint16_t PERSONAL_ACTIVITY:_ACTIVITY_DETECTION = 0xB1;
    
    constexpr uint16_t PERSONAL_ACTIVITY:_DEVICE_POSITION = 0xB2;
    
    constexpr uint16_t PERSONAL_ACTIVITY:_FLOOR_TRACKER = 0xB3;
    
    constexpr uint16_t PERSONAL_ACTIVITY:_PEDOMETER = 0xB4;
    
    constexpr uint16_t PERSONAL_ACTIVITY:_STEP_DETECTION = 0xB5;
    
    constexpr uint16_t ORIENTATION_EXTENDED = 0xC0;
    
    constexpr uint16_t ORIENTATION_EXTENDED:_GEOMAGNETIC_ORIENTATION = 0xC1;
    
    constexpr uint16_t ORIENTATION_EXTENDED:_MAGNETOMETER = 0xC2;
    
    constexpr uint16_t GESTURE = 0xD0;
    
    constexpr uint16_t GESTURE:_CHASSIS_FLIP_GESTURE = 0xD1;
    
    constexpr uint16_t GESTURE:_HINGE_FOLD_GESTURE = 0xD2;
    
    constexpr uint16_t OTHER = 0xE0;
    
    constexpr uint16_t OTHER:_CUSTOM = 0xE1;
    
    constexpr uint16_t OTHER:_GENERIC = 0xE2;
    
    constexpr uint16_t OTHER:_GENERIC_ENUMERATOR = 0xE3;
    
    constexpr uint16_t OTHER:_HINGE_ANGLE = 0xE4;
    
    constexpr uint16_t VENDOR_RESERVED_1 = 0xF0;
    
    constexpr uint16_t VENDOR_RESERVED_2 = 0xF1;
    
    constexpr uint16_t VENDOR_RESERVED_3 = 0xF2;
    
    constexpr uint16_t VENDOR_RESERVED_4 = 0xF3;
    
    constexpr uint16_t VENDOR_RESERVED_5 = 0xF4;
    
    constexpr uint16_t VENDOR_RESERVED_6 = 0xF5;
    
    constexpr uint16_t VENDOR_RESERVED_7 = 0xF6;
    
    constexpr uint16_t VENDOR_RESERVED_8 = 0xF7;
    
    constexpr uint16_t VENDOR_RESERVED_9 = 0xF8;
    
    constexpr uint16_t VENDOR_RESERVED_10 = 0xF9;
    
    constexpr uint16_t VENDOR_RESERVED_11 = 0xFA;
    
    constexpr uint16_t VENDOR_RESERVED_12 = 0xFB;
    
    constexpr uint16_t VENDOR_RESERVED_13 = 0xFC;
    
    constexpr uint16_t VENDOR_RESERVED_14 = 0xFD;
    
    constexpr uint16_t VENDOR_RESERVED_15 = 0xFE;
    
    constexpr uint16_t VENDOR_RESERVED_16 = 0xFF;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint16_t PROPERTY:_FLUSH_FIFO_EVENTS = 0x31C;
    
    constexpr uint16_t PROPERTY:_IS_PRIMARY = 0x31E;
    
    constexpr uint16_t PROPERTY:_VIBRATION_STATE = 0x4A1;
    
    constexpr uint16_t PROPERTY:_AUTO_BRIGHTNESS_PREFERRED = 0x4E2;
    
    constexpr uint16_t PROPERTY:_AUTO_COLOR_PREFERRED = 0x4E3;
    
    constexpr uint16_t PROPERTY:_DAYLIGHT_SAVINGS_TIME_OBSERVED = 0x533;
    
    constexpr uint16_t PROPERTY:_ARM_ALARM = 0x535;
    
    constexpr uint16_t DATA_FIELD:_STEP_COUNT_RESET = 0x595;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint16_t EVENT = 0x200;
    
    constexpr uint16_t PROPERTY = 0x300;
    
    constexpr uint16_t PROPERTY:_PERSISTENT_UNIQUE_ID = 0x302;
    
    constexpr uint16_t PROPERTY:_SENSOR_STATUS = 0x303;
    
    constexpr uint16_t PROPERTY:_SENSOR_DEVICE_PATH = 0x30A;
    
    constexpr uint16_t PROPERTY:_REPORT_INTERVAL = 0x30E;
    
    constexpr uint16_t PROPERTY:_CHANGE_SENSITIVITY_ABSOLUTE = 0x30F;
    
    constexpr uint16_t PROPERTY:_CHANGE_SENSITIVITY_PERCENT_OF_RANGE = 0x310;
    
    constexpr uint16_t PROPERTY:_CHANGE_SENSITIVITY_PERCENT_RELATIVE = 0x311;
    
    constexpr uint16_t PROPERTY:_ACCURACY = 0x312;
    
    constexpr uint16_t PROPERTY:_RESOLUTION = 0x313;
    
    constexpr uint16_t PROPERTY:_MAXIMUM = 0x314;
    
    constexpr uint16_t PROPERTY:_MINIMUM = 0x315;
    
    constexpr uint16_t PROPERTY:_SAMPLING_RATE = 0x317;
    
    constexpr uint16_t PROPERTY:_RESPONSE_CURVE = 0x318;
    
    constexpr uint16_t PROPERTY:_REPORT_LATENCY = 0x31B;
    
    constexpr uint16_t PROPERTY:_MAXIMUM_POWER_CONSUMPTION = 0x31D;
    
    constexpr uint16_t DATA_FIELD:_LOCATION = 0x400;
    
    constexpr uint16_t PROPERTY:_LOCATION = 0x42A;
    
    constexpr uint16_t DATA_FIELD:_MOTION = 0x450;
    
    constexpr uint16_t DATA_FIELD:_ORIENTATION = 0x470;
    
    constexpr uint16_t DATA_FIELD:_MECHANICAL = 0x490;
    
    constexpr uint16_t PROPERTY:_MECHANICAL = 0x4A0;
    
    constexpr uint16_t PROPERTY:_FORWARD_VIBRATION_SPEED = 0x4A2;
    
    constexpr uint16_t PROPERTY:_BACKWARD_VIBRATION_SPEED = 0x4A3;
    
    constexpr uint16_t DATA_FIELD:_BIOMETRIC = 0x4B0;
    
    constexpr uint16_t DATA_FIELD:_LIGHT = 0x4D0;
    
    constexpr uint16_t PROPERTY:_LIGHT = 0x4DF;
    
    constexpr uint16_t PROPERTY:_CONSUMER_IR_SENTENCE_SEND = 0x4E0;
    
    constexpr uint16_t DATA_FIELD:_SCANNER = 0x4F0;
    
    constexpr uint16_t PROPERTY:_SCANNER = 0x4F8;
    
    constexpr uint16_t DATA_FIELD:_TIME = 0x520;
    
    constexpr uint16_t PROPERTY:_TIME = 0x530;
    
    constexpr uint16_t PROPERTY:_TIME_ZONE_OFFSET_FROM_UTC = 0x531;
    
    constexpr uint16_t PROPERTY:_TIME_ZONE_NAME = 0x532;
    
    constexpr uint16_t PROPERTY:_TIME_TRIM_ADJUSTMENT = 0x534;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM = 0x540;
    
    constexpr uint16_t DATA_FIELD:_GENERIC = 0x560;
    
    constexpr uint16_t PROPERTY:_GENERIC = 0x580;
    
    constexpr uint16_t PROPERTY:_ENUMERATOR_TABLE_ROW_INDEX = 0x581;
    
    constexpr uint16_t DATA_FIELD:_PERSONAL_ACTIVITY = 0x590;
    
    constexpr uint16_t PROPERTY:_MINIMUM_ACTIVITY_DETECTION_INTERVAL = 0x5A0;
    
    constexpr uint16_t PROPERTY:_FLOOR_HEIGHT = 0x5A5;
    
    constexpr uint16_t PROPERTY:_CUSTOM = 0x5C0;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_1 = 0x5C1;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_2 = 0x5C2;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_3 = 0x5C3;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_4 = 0x5C4;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_5 = 0x5C5;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_6 = 0x5C6;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_7 = 0x5C7;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_8 = 0x5C8;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_9 = 0x5C9;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_10 = 0x5CA;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_11 = 0x5CB;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_12 = 0x5CC;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_13 = 0x5CD;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_14 = 0x5CE;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_15 = 0x5CF;
    
    constexpr uint16_t PROPERTY:_CUSTOM_VALUE_16 = 0x5D0;
    
    constexpr uint16_t DATA_FIELD:_GESTURE_SENSOR = 0x5F0;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint16_t DATA_FIELD:_HUMAN_ATTENTION_DETECTED = 0x4BD;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint16_t EVENT:_SENSOR_STATE = 0x201;
    
    constexpr uint16_t EVENT:_SENSOR_EVENT = 0x202;
    
    constexpr uint16_t PROPERTY:_SENSOR_CONNECTION_TYPE = 0x309;
    
    constexpr uint16_t PROPERTY:_REPORTING_STATE = 0x316;
    
    constexpr uint16_t PROPERTY:_POWER_STATE = 0x319;
    
    constexpr uint16_t PROPERTY:_HUMAN_PRESENCE_DETECTION_TYPE = 0x31F;
    
    constexpr uint16_t DATA_FIELD:_FIX_QUALITY = 0x40A;
    
    constexpr uint16_t DATA_FIELD:_FIX_TYPE = 0x40B;
    
    constexpr uint16_t DATA_FIELD:_GPS_OPERATION_MODE = 0x40D;
    
    constexpr uint16_t DATA_FIELD:_GPS_SELECTION_MODE = 0x40E;
    
    constexpr uint16_t DATA_FIELD:_GPS_STATUS = 0x40F;
    
    constexpr uint16_t PROPERTY:_LOCATION_DESIRED_ACCURACY = 0x42B;
    
    constexpr uint16_t DATA_FIELD:_MAGNETOMETER_ACCURACY = 0x488;
    
    constexpr uint16_t DATA_FIELD:_SIMPLE_ORIENTATION_DIRECTION = 0x489;
    
    constexpr uint16_t DATA_FIELD:_DAY_OF_WEEK = 0x524;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_GUID_OR_PROPERTYKEY_KIND = 0x56C;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_FIRMWARE_VARTYPE = 0x572;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_UNIT_OF_MEASURE = 0x573;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_UNIT_EXPONENT = 0x574;
    
    constexpr uint16_t DATA_FIELD:_ACTIVITY_TYPE = 0x591;
    
    constexpr uint16_t DATA_FIELD:_ACTIVITY_STATE = 0x592;
    
    constexpr uint16_t DATA_FIELD:_DEVICE_POSITION = 0x593;
    
    constexpr uint16_t DATA_FIELD:_STEP_TYPE = 0x597;
    
    constexpr uint16_t PROPERTY:_SUPPORTED_ACTIVITY_TYPES = 0x5A1;
    
    constexpr uint16_t PROPERTY:_SUBSCRIBED_ACTIVITY_TYPES = 0x5A2;
    
    constexpr uint16_t PROPERTY:_SUPPORTED_STEP_TYPES = 0x5A3;
    
    constexpr uint16_t PROPERTY:_SUBSCRIBED_STEP_TYPES = 0x5A4;
    
    constexpr uint16_t DATA_FIELD:_GESTURE_STATE = 0x5F1;
    
    constexpr uint16_t DATA_FIELD:_HINGE_FOLD_CONTRIBUTING_PANEL = 0x5F4;
    
    constexpr uint16_t DATA_FIELD:_HINGE_FOLD_TYPE = 0x5F5;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint16_t DATA_FIELD:_OBJECT_PRESENCE = 0x43A;
    
    constexpr uint16_t DATA_FIELD:_OBJECT_PROXIMITY_OUT_OF_RANGE = 0x43C;
    
    constexpr uint16_t DATA_FIELD:_MOTION_STATE = 0x451;
    
    constexpr uint16_t DATA_FIELD:_DISTANCE_OUT_OF_RANGE = 0x47D;
    
    constexpr uint16_t DATA_FIELD:_BOOLEAN_SWITCH_STATE = 0x491;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_PRESENCE = 0x4B1;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_PROXIMITY_OUT_OF_RANGE = 0x4B3;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_TOUCH_STATE = 0x4B4;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint16_t PROPERTY:_FRIENDLY_NAME = 0x301;
    
    constexpr uint16_t PROPERTY:_MINIMUM_REPORT_INTERVAL = 0x304;
    
    constexpr uint16_t PROPERTY:_SENSOR_MANUFACTURER = 0x305;
    
    constexpr uint16_t PROPERTY:_SENSOR_MODEL = 0x306;
    
    constexpr uint16_t PROPERTY:_SENSOR_SERIAL_NUMBER = 0x307;
    
    constexpr uint16_t PROPERTY:_SENSOR_DESCRIPTION = 0x308;
    
    constexpr uint16_t PROPERTY:_HARDWARE_REVISION = 0x30B;
    
    constexpr uint16_t PROPERTY:_FIRMWARE_VERSION = 0x30C;
    
    constexpr uint16_t PROPERTY:_RELEASE_DATE = 0x30D;
    
    constexpr uint16_t PROPERTY:_MAXIMUM_FIFO_EVENTS = 0x31A;
    
    constexpr uint16_t DATA_FIELD:_ALTITUDE_ANTENNA_SEA_LEVEL = 0x402;
    
    constexpr uint16_t DATA_FIELD:_DIFFERENTIAL_REFERENCE_STATION_ID = 0x403;
    
    constexpr uint16_t DATA_FIELD:_ALTITUDE_ELLIPSOID_ERROR = 0x404;
    
    constexpr uint16_t DATA_FIELD:_ALTITUDE_ELLIPSOID = 0x405;
    
    constexpr uint16_t DATA_FIELD:_ALTITUDE_SEA_LEVEL_ERROR = 0x406;
    
    constexpr uint16_t DATA_FIELD:_ALTITUDE_SEA_LEVEL = 0x407;
    
    constexpr uint16_t DATA_FIELD:_DIFFERENTIAL_GPS_DATA_AGE = 0x408;
    
    constexpr uint16_t DATA_FIELD:_ERROR_RADIUS = 0x409;
    
    constexpr uint16_t DATA_FIELD:_GEOIDAL_SEPARATION = 0x40C;
    
    constexpr uint16_t DATA_FIELD:_POSITION_DILUTION_OF_PRECISION = 0x410;
    
    constexpr uint16_t DATA_FIELD:_HORIZONTAL_DILUTION_OF_PRECISION = 0x411;
    
    constexpr uint16_t DATA_FIELD:_VERTICAL_DILUTION_OF_PRECISION = 0x412;
    
    constexpr uint16_t DATA_FIELD:_LATITUDE = 0x413;
    
    constexpr uint16_t DATA_FIELD:_LONGITUDE = 0x414;
    
    constexpr uint16_t DATA_FIELD:_TRUE_HEADING = 0x415;
    
    constexpr uint16_t DATA_FIELD:_MAGNETIC_HEADING = 0x416;
    
    constexpr uint16_t DATA_FIELD:_MAGNETIC_VARIATION = 0x417;
    
    constexpr uint16_t DATA_FIELD:_SPEED = 0x418;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_IN_VIEW = 0x419;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_IN_VIEW_AZIMUTH = 0x41A;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_IN_VIEW_ELEVATION = 0x41B;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_IN_VIEW_IDS = 0x41C;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_IN_VIEW_PRNS = 0x41D;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_IN_VIEW_S_N_RATIOS = 0x41E;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_USED_COUNT = 0x41F;
    
    constexpr uint16_t DATA_FIELD:_SATELLITES_USED_PRNS = 0x420;
    
    constexpr uint16_t DATA_FIELD:_NMEA_SENTENCE = 0x421;
    
    constexpr uint16_t DATA_FIELD:_ADDRESS_LINE_1 = 0x422;
    
    constexpr uint16_t DATA_FIELD:_ADDRESS_LINE_2 = 0x423;
    
    constexpr uint16_t DATA_FIELD:_CITY = 0x424;
    
    constexpr uint16_t DATA_FIELD:_STATE_OR_PROVINCE = 0x425;
    
    constexpr uint16_t DATA_FIELD:_COUNTRY_OR_REGION = 0x426;
    
    constexpr uint16_t DATA_FIELD:_POSTAL_CODE = 0x427;
    
    constexpr uint16_t DATA_FIELD:_ENVIRONMENTAL = 0x430;
    
    constexpr uint16_t DATA_FIELD:_ATMOSPHERIC_PRESSURE = 0x431;
    
    constexpr uint16_t DATA_FIELD:_RELATIVE_HUMIDITY = 0x433;
    
    constexpr uint16_t DATA_FIELD:_TEMPERATURE = 0x434;
    
    constexpr uint16_t DATA_FIELD:_WIND_DIRECTION = 0x435;
    
    constexpr uint16_t DATA_FIELD:_WIND_SPEED = 0x436;
    
    constexpr uint16_t DATA_FIELD:_AIR_QUALITY_INDEX = 0x437;
    
    constexpr uint16_t DATA_FIELD:_EQUIVALENT_CO2 = 0x438;
    
    constexpr uint16_t DATA_FIELD:_VOLATILE_ORGANIC_COMPOUND_CONCENTRATION = 0x439;
    
    constexpr uint16_t DATA_FIELD:_OBJECT_PROXIMITY_RANGE = 0x43B;
    
    constexpr uint16_t PROPERTY:_ENVIRONMENTAL = 0x440;
    
    constexpr uint16_t PROPERTY:_REFERENCE_PRESSURE = 0x441;
    
    constexpr uint16_t DATA_FIELD:_ACCELERATION = 0x452;
    
    constexpr uint16_t DATA_FIELD:_ACCELERATION_AXIS_X = 0x453;
    
    constexpr uint16_t DATA_FIELD:_ACCELERATION_AXIS_Y = 0x454;
    
    constexpr uint16_t DATA_FIELD:_ACCELERATION_AXIS_Z = 0x455;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_VELOCITY = 0x456;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_VELOCITY_ABOUT_X_AXIS = 0x457;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_VELOCITY_ABOUT_Y_AXIS = 0x458;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_VELOCITY_ABOUT_Z_AXIS = 0x459;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_POSITION = 0x45A;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_POSITION_ABOUT_X_AXIS = 0x45B;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_POSITION_ABOUT_Y_AXIS = 0x45C;
    
    constexpr uint16_t DATA_FIELD:_ANGULAR_POSITION_ABOUT_Z_AXIS = 0x45D;
    
    constexpr uint16_t DATA_FIELD:_MOTION_SPEED = 0x45E;
    
    constexpr uint16_t DATA_FIELD:_MOTION_INTENSITY = 0x45F;
    
    constexpr uint16_t DATA_FIELD:_HEADING = 0x471;
    
    constexpr uint16_t DATA_FIELD:_HEADING_X_AXIS = 0x472;
    
    constexpr uint16_t DATA_FIELD:_HEADING_Y_AXIS = 0x473;
    
    constexpr uint16_t DATA_FIELD:_HEADING_Z_AXIS = 0x474;
    
    constexpr uint16_t DATA_FIELD:_HEADING_COMPENSATED_MAGNETIC_NORTH = 0x475;
    
    constexpr uint16_t DATA_FIELD:_HEADING_COMPENSATED_TRUE_NORTH = 0x476;
    
    constexpr uint16_t DATA_FIELD:_HEADING_MAGNETIC_NORTH = 0x477;
    
    constexpr uint16_t DATA_FIELD:_HEADING_TRUE_NORTH = 0x478;
    
    constexpr uint16_t DATA_FIELD:_DISTANCE = 0x479;
    
    constexpr uint16_t DATA_FIELD:_DISTANCE_X_AXIS = 0x47A;
    
    constexpr uint16_t DATA_FIELD:_DISTANCE_Y_AXIS = 0x47B;
    
    constexpr uint16_t DATA_FIELD:_DISTANCE_Z_AXIS = 0x47C;
    
    constexpr uint16_t DATA_FIELD:_TILT = 0x47E;
    
    constexpr uint16_t DATA_FIELD:_TILT_X_AXIS = 0x47F;
    
    constexpr uint16_t DATA_FIELD:_TILT_Y_AXIS = 0x480;
    
    constexpr uint16_t DATA_FIELD:_TILT_Z_AXIS = 0x481;
    
    constexpr uint16_t DATA_FIELD:_ROTATION_MATRIX = 0x482;
    
    constexpr uint16_t DATA_FIELD:_QUATERNION = 0x483;
    
    constexpr uint16_t DATA_FIELD:_MAGNETIC_FLUX = 0x484;
    
    constexpr uint16_t DATA_FIELD:_MAGNETIC_FLUX_X_AXIS = 0x485;
    
    constexpr uint16_t DATA_FIELD:_MAGNETIC_FLUX_Y_AXIS = 0x486;
    
    constexpr uint16_t DATA_FIELD:_MAGNETIC_FLUX_Z_AXIS = 0x487;
    
    constexpr uint16_t DATA_FIELD:_BOOLEAN_SWITCH_ARRAY_STATES = 0x492;
    
    constexpr uint16_t DATA_FIELD:_MULTIVALUE_SWITCH_VALUE = 0x493;
    
    constexpr uint16_t DATA_FIELD:_FORCE = 0x494;
    
    constexpr uint16_t DATA_FIELD:_ABSOLUTE_PRESSURE = 0x495;
    
    constexpr uint16_t DATA_FIELD:_GAUGE_PRESSURE = 0x496;
    
    constexpr uint16_t DATA_FIELD:_STRAIN = 0x497;
    
    constexpr uint16_t DATA_FIELD:_WEIGHT = 0x498;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_PROXIMITY_RANGE = 0x4B2;
    
    constexpr uint16_t DATA_FIELD:_BLOOD_PRESSURE = 0x4B5;
    
    constexpr uint16_t DATA_FIELD:_BLOOD_PRESSURE_DIASTOLIC = 0x4B6;
    
    constexpr uint16_t DATA_FIELD:_BLOOD_PRESSURE_SYSTOLIC = 0x4B7;
    
    constexpr uint16_t DATA_FIELD:_HEART_RATE = 0x4B8;
    
    constexpr uint16_t DATA_FIELD:_RESTING_HEART_RATE = 0x4B9;
    
    constexpr uint16_t DATA_FIELD:_HEARTBEAT_INTERVAL = 0x4BA;
    
    constexpr uint16_t DATA_FIELD:_RESPIRATORY_RATE = 0x4BB;
    
    constexpr uint16_t DATA_FIELD:_SPO2 = 0x4BC;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_HEAD_AZIMUTH = 0x4BE;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_HEAD_ALTITUDE = 0x4BF;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_HEAD_ROLL = 0x4C0;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_HEAD_PITCH = 0x4C1;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_HEAD_YAW = 0x4C2;
    
    constexpr uint16_t DATA_FIELD:_HUMAN_CORRELATION_ID = 0x4C3;
    
    constexpr uint16_t DATA_FIELD:_ILLUMINANCE = 0x4D1;
    
    constexpr uint16_t DATA_FIELD:_COLOR_TEMPERATURE = 0x4D2;
    
    constexpr uint16_t DATA_FIELD:_CHROMATICITY = 0x4D3;
    
    constexpr uint16_t DATA_FIELD:_CHROMATICITY_X = 0x4D4;
    
    constexpr uint16_t DATA_FIELD:_CHROMATICITY_Y = 0x4D5;
    
    constexpr uint16_t DATA_FIELD:_CONSUMER_IR_SENTENCE_RECEIVE = 0x4D6;
    
    constexpr uint16_t DATA_FIELD:_INFRARED_LIGHT = 0x4D7;
    
    constexpr uint16_t DATA_FIELD:_RED_LIGHT = 0x4D8;
    
    constexpr uint16_t DATA_FIELD:_GREEN_LIGHT = 0x4D9;
    
    constexpr uint16_t DATA_FIELD:_BLUE_LIGHT = 0x4DA;
    
    constexpr uint16_t DATA_FIELD:_ULTRAVIOLET_A_LIGHT = 0x4DB;
    
    constexpr uint16_t DATA_FIELD:_ULTRAVIOLET_B_LIGHT = 0x4DC;
    
    constexpr uint16_t DATA_FIELD:_ULTRAVIOLET_INDEX = 0x4DD;
    
    constexpr uint16_t DATA_FIELD:_NEAR_INFRARED_LIGHT = 0x4DE;
    
    constexpr uint16_t DATA_FIELD:_RFID_TAG_40_BIT = 0x4F1;
    
    constexpr uint16_t DATA_FIELD:_NFC_SENTENCE_RECEIVE = 0x4F2;
    
    constexpr uint16_t PROPERTY:_NFC_SENTENCE_SEND = 0x4F9;
    
    constexpr uint16_t DATA_FIELD:_ELECTRICAL = 0x500;
    
    constexpr uint16_t DATA_FIELD:_CAPACITANCE = 0x501;
    
    constexpr uint16_t DATA_FIELD:_CURRENT = 0x502;
    
    constexpr uint16_t DATA_FIELD:_ELECTRICAL_POWER = 0x503;
    
    constexpr uint16_t DATA_FIELD:_INDUCTANCE = 0x504;
    
    constexpr uint16_t DATA_FIELD:_RESISTANCE = 0x505;
    
    constexpr uint16_t DATA_FIELD:_VOLTAGE = 0x506;
    
    constexpr uint16_t DATA_FIELD:_FREQUENCY = 0x507;
    
    constexpr uint16_t DATA_FIELD:_PERIOD = 0x508;
    
    constexpr uint16_t DATA_FIELD:_PERCENT_OF_RANGE = 0x509;
    
    constexpr uint16_t DATA_FIELD:_YEAR = 0x521;
    
    constexpr uint16_t DATA_FIELD:_MONTH = 0x522;
    
    constexpr uint16_t DATA_FIELD:_DAY = 0x523;
    
    constexpr uint16_t DATA_FIELD:_HOUR = 0x525;
    
    constexpr uint16_t DATA_FIELD:_MINUTE = 0x526;
    
    constexpr uint16_t DATA_FIELD:_SECOND = 0x527;
    
    constexpr uint16_t DATA_FIELD:_MILLISECOND = 0x528;
    
    constexpr uint16_t DATA_FIELD:_TIMESTAMP = 0x529;
    
    constexpr uint16_t DATA_FIELD:_JULIAN_DAY_OF_YEAR = 0x52A;
    
    constexpr uint16_t DATA_FIELD:_TIME_SINCE_SYS_BOOT = 0x52B;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_USAGE = 0x541;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_BOOLEAN_ARRAY = 0x542;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE = 0x543;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_1 = 0x544;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_2 = 0x545;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_3 = 0x546;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_4 = 0x547;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_5 = 0x548;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_6 = 0x549;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_7 = 0x54A;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_8 = 0x54B;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_9 = 0x54C;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_10 = 0x54D;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_11 = 0x54E;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_12 = 0x54F;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_13 = 0x550;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_14 = 0x551;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_15 = 0x552;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_16 = 0x553;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_17 = 0x554;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_18 = 0x555;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_19 = 0x556;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_20 = 0x557;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_21 = 0x558;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_22 = 0x559;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_23 = 0x55A;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_24 = 0x55B;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_25 = 0x55C;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_26 = 0x55D;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_27 = 0x55E;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_VALUE_28 = 0x55F;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_GUID_OR_PROPERTYKEY = 0x561;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_CATEGORY_GUID = 0x562;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_TYPE_GUID = 0x563;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_EVENT_PROPERTYKEY = 0x564;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_PROPERTY_PROPERTYKEY = 0x565;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_DATA_FIELD_PROPERTYKEY = 0x566;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_EVENT = 0x567;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_PROPERTY = 0x568;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_DATA_FIELD = 0x569;
    
    constexpr uint16_t DATA_FIELD:_ENUMERATOR_TABLE_ROW_INDEX = 0x56A;
    
    constexpr uint16_t DATA_FIELD:_ENUMERATOR_TABLE_ROW_COUNT = 0x56B;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_GUID = 0x56D;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_PROPERTYKEY = 0x56E;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_TOP_LEVEL_COLLECTION_ID = 0x56F;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_REPORT_ID = 0x570;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_REPORT_ITEM_POSITION_INDEX = 0x571;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_REPORT_SIZE = 0x575;
    
    constexpr uint16_t DATA_FIELD:_GENERIC_REPORT_COUNT = 0x576;
    
    constexpr uint16_t PROPERTY:_ENUMERATOR_TABLE_ROW_COUNT = 0x582;
    
    constexpr uint16_t DATA_FIELD:_STEP_COUNT = 0x594;
    
    constexpr uint16_t DATA_FIELD:_STEP_DURATION = 0x596;
    
    constexpr uint16_t DATA_FIELD:_CUSTOM_TYPE_ID = 0x5B0;
    
    constexpr uint16_t DATA_FIELD:_HINGE = 0x5E0;
    
    constexpr uint16_t DATA_FIELD:_HINGE_ANGLE = 0x5E1;
    
    constexpr uint16_t DATA_FIELD:_HINGE_FOLD_INITIAL_ANGLE = 0x5F2;
    
    constexpr uint16_t DATA_FIELD:_HINGE_FOLD_FINAL_ANGLE = 0x5F3;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint16_t SENSOR_STATE:_UNDEFINED = 0x800;
    
    constexpr uint16_t SENSOR_STATE:_READY = 0x801;
    
    constexpr uint16_t SENSOR_STATE:_NOT_AVAILABLE = 0x802;
    
    constexpr uint16_t SENSOR_STATE:_NO_DATA = 0x803;
    
    constexpr uint16_t SENSOR_STATE:_INITIALIZING = 0x804;
    
    constexpr uint16_t SENSOR_STATE:_ACCESS_DENIED = 0x805;
    
    constexpr uint16_t SENSOR_STATE:_ERROR = 0x806;
    
    constexpr uint16_t SENSOR_EVENT:_UNKNOWN = 0x810;
    
    constexpr uint16_t SENSOR_EVENT:_STATE_CHANGED = 0x811;
    
    constexpr uint16_t SENSOR_EVENT:_PROPERTY_CHANGED = 0x812;
    
    constexpr uint16_t SENSOR_EVENT:_DATA_UPDATED = 0x813;
    
    constexpr uint16_t SENSOR_EVENT:_POLL_RESPONSE = 0x814;
    
    constexpr uint16_t SENSOR_EVENT:_CHANGE_SENSITIVITY = 0x815;
    
    constexpr uint16_t SENSOR_EVENT:_RANGE_MAXIMUM_REACHED = 0x816;
    
    constexpr uint16_t SENSOR_EVENT:_RANGE_MINIMUM_REACHED = 0x817;
    
    constexpr uint16_t SENSOR_EVENT:_HIGH_THRESHOLD_CROSS_UPWARD = 0x818;
    
    constexpr uint16_t SENSOR_EVENT:_HIGH_THRESHOLD_CROSS_DOWNWARD = 0x819;
    
    constexpr uint16_t SENSOR_EVENT:_LOW_THRESHOLD_CROSS_UPWARD = 0x81A;
    
    constexpr uint16_t SENSOR_EVENT:_LOW_THRESHOLD_CROSS_DOWNWARD = 0x81B;
    
    constexpr uint16_t SENSOR_EVENT:_ZERO_THRESHOLD_CROSS_UPWARD = 0x81C;
    
    constexpr uint16_t SENSOR_EVENT:_ZERO_THRESHOLD_CROSS_DOWNWARD = 0x81D;
    
    constexpr uint16_t SENSOR_EVENT:_PERIOD_EXCEEDED = 0x81E;
    
    constexpr uint16_t SENSOR_EVENT:_FREQUENCY_EXCEEDED = 0x81F;
    
    constexpr uint16_t SENSOR_EVENT:_COMPLEX_TRIGGER = 0x820;
    
    constexpr uint16_t CONNECTION_TYPE:_PC_INTEGRATED = 0x830;
    
    constexpr uint16_t CONNECTION_TYPE:_PC_ATTACHED = 0x831;
    
    constexpr uint16_t CONNECTION_TYPE:_PC_EXTERNAL = 0x832;
    
    constexpr uint16_t REPORTING_STATE:_REPORT_NO_EVENTS = 0x840;
    
    constexpr uint16_t REPORTING_STATE:_REPORT_ALL_EVENTS = 0x841;
    
    constexpr uint16_t REPORTING_STATE:_REPORT_THRESHOLD_EVENTS = 0x842;
    
    constexpr uint16_t REPORTING_STATE:_WAKE_ON_NO_EVENTS = 0x843;
    
    constexpr uint16_t REPORTING_STATE:_WAKE_ON_ALL_EVENTS = 0x844;
    
    constexpr uint16_t REPORTING_STATE:_WAKE_ON_THRESHOLD_EVENTS = 0x845;
    
    constexpr uint16_t REPORTING_STATE:_ANYTIME = 0x846;
    
    constexpr uint16_t POWER_STATE:_UNDEFINED = 0x850;
    
    constexpr uint16_t POWER_STATE:_D0_FULL_POWER = 0x851;
    
    constexpr uint16_t POWER_STATE:_D1_LOW_POWER = 0x852;
    
    constexpr uint16_t POWER_STATE:_D2_STANDBY_POWER_WITH_WAKEUP = 0x853;
    
    constexpr uint16_t POWER_STATE:_D3_SLEEP_WITH_WAKEUP = 0x854;
    
    constexpr uint16_t POWER_STATE:_D4_POWER_OFF = 0x855;
    
    constexpr uint16_t ACCURACY:_DEFAULT = 0x860;
    
    constexpr uint16_t ACCURACY:_HIGH = 0x861;
    
    constexpr uint16_t ACCURACY:_MEDIUM = 0x862;
    
    constexpr uint16_t ACCURACY:_LOW = 0x863;
    
    constexpr uint16_t FIX_QUALITY:_NO_FIX = 0x870;
    
    constexpr uint16_t FIX_QUALITY:_GPS = 0x871;
    
    constexpr uint16_t FIX_QUALITY:_DGPS = 0x872;
    
    constexpr uint16_t FIX_TYPE:_NO_FIX = 0x880;
    
    constexpr uint16_t FIX_TYPE:_GPS_SPS_MODE,_FIX_VALID = 0x881;
    
    constexpr uint16_t FIX_TYPE:_DGPS_SPS_MODE,_FIX_VALID = 0x882;
    
    constexpr uint16_t FIX_TYPE:_GPS_PPS_MODE,_FIX_VALID = 0x883;
    
    constexpr uint16_t FIX_TYPE:_REAL_TIME_KINEMATIC = 0x884;
    
    constexpr uint16_t FIX_TYPE:_FLOAT_RTK = 0x885;
    
    constexpr uint16_t FIX_TYPE:_ESTIMATED_DEAD_RECKONED = 0x886;
    
    constexpr uint16_t FIX_TYPE:_MANUAL_INPUT_MODE = 0x887;
    
    constexpr uint16_t FIX_TYPE:_SIMULATOR_MODE = 0x888;
    
    constexpr uint16_t GPS_OPERATION_MODE:_MANUAL = 0x890;
    
    constexpr uint16_t GPS_OPERATION_MODE:_AUTOMATIC = 0x891;
    
    constexpr uint16_t GPS_SELECTION_MODE:_AUTONOMOUS = 0x8A0;
    
    constexpr uint16_t GPS_SELECTION_MODE:_DGPS = 0x8A1;
    
    constexpr uint16_t GPS_SELECTION_MODE:_ESTIMATED_DEAD_RECKONED = 0x8A2;
    
    constexpr uint16_t GPS_SELECTION_MODE:_MANUAL_INPUT = 0x8A3;
    
    constexpr uint16_t GPS_SELECTION_MODE:_SIMULATOR = 0x8A4;
    
    constexpr uint16_t GPS_SELECTION_MODE:_DATA_NOT_VALID = 0x8A5;
    
    constexpr uint16_t GPS_STATUS_DATA:_VALID = 0x8B0;
    
    constexpr uint16_t GPS_STATUS_DATA:_NOT_VALID = 0x8B1;
    
    constexpr uint16_t DAY_OF_WEEK:_SUNDAY = 0x8C0;
    
    constexpr uint16_t DAY_OF_WEEK:_MONDAY = 0x8C1;
    
    constexpr uint16_t DAY_OF_WEEK:_TUESDAY = 0x8C2;
    
    constexpr uint16_t DAY_OF_WEEK:_WEDNESDAY = 0x8C3;
    
    constexpr uint16_t DAY_OF_WEEK:_THURSDAY = 0x8C4;
    
    constexpr uint16_t DAY_OF_WEEK:_FRIDAY = 0x8C5;
    
    constexpr uint16_t DAY_OF_WEEK:_SATURDAY = 0x8C6;
    
    constexpr uint16_t KIND:_CATEGORY = 0x8D0;
    
    constexpr uint16_t KIND:_TYPE = 0x8D1;
    
    constexpr uint16_t KIND:_EVENT = 0x8D2;
    
    constexpr uint16_t KIND:_PROPERTY = 0x8D3;
    
    constexpr uint16_t KIND:_DATA_FIELD = 0x8D4;
    
    constexpr uint16_t MAGNETOMETER_ACCURACY:_LOW = 0x8E0;
    
    constexpr uint16_t MAGNETOMETER_ACCURACY:_MEDIUM = 0x8E1;
    
    constexpr uint16_t MAGNETOMETER_ACCURACY:_HIGH = 0x8E2;
    
    constexpr uint16_t SIMPLE_ORIENTATION_DIRECTION:_NOT_ROTATED = 0x8F0;
    
    constexpr uint16_t SIMPLE_ORIENTATION_DIRECTION:_ROTATED_90_DEGREES_CCW = 0x8F1;
    
    constexpr uint16_t SIMPLE_ORIENTATION_DIRECTION:_ROTATED_180_DEGREES_CCW = 0x8F2;
    
    constexpr uint16_t SIMPLE_ORIENTATION_DIRECTION:_ROTATED_270_DEGREES_CCW = 0x8F3;
    
    constexpr uint16_t SIMPLE_ORIENTATION_DIRECTION:_FACE_UP = 0x8F4;
    
    constexpr uint16_t SIMPLE_ORIENTATION_DIRECTION:_FACE_DOWN = 0x8F5;
    
    constexpr uint16_t VT\_NULL = 0x900;
    
    constexpr uint16_t VT\_BOOL = 0x901;
    
    constexpr uint16_t VT\_UI1 = 0x902;
    
    constexpr uint16_t VT\_I1 = 0x903;
    
    constexpr uint16_t VT\_UI2 = 0x904;
    
    constexpr uint16_t VT\_I2 = 0x905;
    
    constexpr uint16_t VT\_UI4 = 0x906;
    
    constexpr uint16_t VT\_I4 = 0x907;
    
    constexpr uint16_t VT\_UI8 = 0x908;
    
    constexpr uint16_t VT\_I8 = 0x909;
    
    constexpr uint16_t VT\_R4 = 0x90A;
    
    constexpr uint16_t VT\_R8 = 0x90B;
    
    constexpr uint16_t VT\_WSTR = 0x90C;
    
    constexpr uint16_t VT\_STR = 0x90D;
    
    constexpr uint16_t VT\_CLSID = 0x90E;
    
    constexpr uint16_t VT\_VECTOR_VT\_UI1 = 0x90F;
    
    constexpr uint16_t VT\_F16E0 = 0x910;
    
    constexpr uint16_t VT\_F16E1 = 0x911;
    
    constexpr uint16_t VT\_F16E2 = 0x912;
    
    constexpr uint16_t VT\_F16E3 = 0x913;
    
    constexpr uint16_t VT\_F16E4 = 0x914;
    
    constexpr uint16_t VT\_F16E5 = 0x915;
    
    constexpr uint16_t VT\_F16E6 = 0x916;
    
    constexpr uint16_t VT\_F16E7 = 0x917;
    
    constexpr uint16_t VT\_F16E8 = 0x918;
    
    constexpr uint16_t VT\_F16E9 = 0x919;
    
    constexpr uint16_t VT\_F16EA = 0x91A;
    
    constexpr uint16_t VT\_F16EB = 0x91B;
    
    constexpr uint16_t VT\_F16EC = 0x91C;
    
    constexpr uint16_t VT\_F16ED = 0x91D;
    
    constexpr uint16_t VT\_F16EE = 0x91E;
    
    constexpr uint16_t VT\_F16EF = 0x91F;
    
    constexpr uint16_t VT\_F32E0 = 0x920;
    
    constexpr uint16_t VT\_F32E1 = 0x921;
    
    constexpr uint16_t VT\_F32E2 = 0x922;
    
    constexpr uint16_t VT\_F32E3 = 0x923;
    
    constexpr uint16_t VT\_F32E4 = 0x924;
    
    constexpr uint16_t VT\_F32E5 = 0x925;
    
    constexpr uint16_t VT\_F32E6 = 0x926;
    
    constexpr uint16_t VT\_F32E7 = 0x927;
    
    constexpr uint16_t VT\_F32E8 = 0x928;
    
    constexpr uint16_t VT\_F32E9 = 0x929;
    
    constexpr uint16_t VT\_F32EA = 0x92A;
    
    constexpr uint16_t VT\_F32EB = 0x92B;
    
    constexpr uint16_t VT\_F32EC = 0x92C;
    
    constexpr uint16_t VT\_F32ED = 0x92D;
    
    constexpr uint16_t VT\_F32EE = 0x92E;
    
    constexpr uint16_t VT\_F32EF = 0x92F;
    
    constexpr uint16_t ACTIVITY_TYPE:_UNKNOWN = 0x930;
    
    constexpr uint16_t ACTIVITY_TYPE:_STATIONARY = 0x931;
    
    constexpr uint16_t ACTIVITY_TYPE:_FIDGETING = 0x932;
    
    constexpr uint16_t ACTIVITY_TYPE:_WALKING = 0x933;
    
    constexpr uint16_t ACTIVITY_TYPE:_RUNNING = 0x934;
    
    constexpr uint16_t ACTIVITY_TYPE:_IN_VEHICLE = 0x935;
    
    constexpr uint16_t ACTIVITY_TYPE:_BIKING = 0x936;
    
    constexpr uint16_t ACTIVITY_TYPE:_IDLE = 0x937;
    
    constexpr uint16_t UNIT:_NOT_SPECIFIED = 0x940;
    
    constexpr uint16_t UNIT:_LUX = 0x941;
    
    constexpr uint16_t UNIT:_DEGREES_KELVIN = 0x942;
    
    constexpr uint16_t UNIT:_DEGREES_CELSIUS = 0x943;
    
    constexpr uint16_t UNIT:_PASCAL = 0x944;
    
    constexpr uint16_t UNIT:_NEWTON = 0x945;
    
    constexpr uint16_t UNIT:_METERS_SECOND = 0x946;
    
    constexpr uint16_t UNIT:_KILOGRAM = 0x947;
    
    constexpr uint16_t UNIT:_METER = 0x948;
    
    constexpr uint16_t UNIT:_METERS_SECOND_SECOND = 0x949;
    
    constexpr uint16_t UNIT:_FARAD = 0x94A;
    
    constexpr uint16_t UNIT:_AMPERE = 0x94B;
    
    constexpr uint16_t UNIT:_WATT = 0x94C;
    
    constexpr uint16_t UNIT:_HENRY = 0x94D;
    
    constexpr uint16_t UNIT:_OHM = 0x94E;
    
    constexpr uint16_t UNIT:_VOLT = 0x94F;
    
    constexpr uint16_t UNIT:_HERTZ = 0x950;
    
    constexpr uint16_t UNIT:_BAR = 0x951;
    
    constexpr uint16_t UNIT:_DEGREES_ANTI_CLOCKWISE = 0x952;
    
    constexpr uint16_t UNIT:_DEGREES_CLOCKWISE = 0x953;
    
    constexpr uint16_t UNIT:_DEGREES = 0x954;
    
    constexpr uint16_t UNIT:_DEGREES_SECOND = 0x955;
    
    constexpr uint16_t UNIT:_DEGREES_SECOND_SECOND = 0x956;
    
    constexpr uint16_t UNIT:_KNOT = 0x957;
    
    constexpr uint16_t UNIT:_PERCENT = 0x958;
    
    constexpr uint16_t UNIT:_SECOND = 0x959;
    
    constexpr uint16_t UNIT:_MILLISECOND = 0x95A;
    
    constexpr uint16_t UNIT:_G = 0x95B;
    
    constexpr uint16_t UNIT:_BYTES = 0x95C;
    
    constexpr uint16_t UNIT:_MILLIGAUSS = 0x95D;
    
    constexpr uint16_t UNIT:_BITS = 0x95E;
    
    constexpr uint16_t ACTIVITY_STATE:_NO_STATE_CHANGE = 0x960;
    
    constexpr uint16_t ACTIVITY_STATE:_START_ACTIVITY = 0x961;
    
    constexpr uint16_t ACTIVITY_STATE:_END_ACTIVITY = 0x962;
    
    constexpr uint16_t EXPONENT_0 = 0x970;
    
    constexpr uint16_t EXPONENT_1 = 0x971;
    
    constexpr uint16_t EXPONENT_2 = 0x972;
    
    constexpr uint16_t EXPONENT_3 = 0x973;
    
    constexpr uint16_t EXPONENT_4 = 0x974;
    
    constexpr uint16_t EXPONENT_5 = 0x975;
    
    constexpr uint16_t EXPONENT_6 = 0x976;
    
    constexpr uint16_t EXPONENT_7 = 0x977;
    
    constexpr uint16_t EXPONENT_8 = 0x978;
    
    constexpr uint16_t EXPONENT_9 = 0x979;
    
    constexpr uint16_t EXPONENT_A = 0x97A;
    
    constexpr uint16_t EXPONENT_B = 0x97B;
    
    constexpr uint16_t EXPONENT_C = 0x97C;
    
    constexpr uint16_t EXPONENT_D = 0x97D;
    
    constexpr uint16_t EXPONENT_E = 0x97E;
    
    constexpr uint16_t EXPONENT_F = 0x97F;
    
    constexpr uint16_t DEVICE_POSITION:_UNKNOWN = 0x980;
    
    constexpr uint16_t DEVICE_POSITION:_UNCHANGED = 0x981;
    
    constexpr uint16_t DEVICE_POSITION:_ON_DESK = 0x982;
    
    constexpr uint16_t DEVICE_POSITION:_IN_HAND = 0x983;
    
    constexpr uint16_t DEVICE_POSITION:_MOVING_IN_BAG = 0x984;
    
    constexpr uint16_t DEVICE_POSITION:_STATIONARY_IN_BAG = 0x985;
    
    constexpr uint16_t STEP_TYPE:_UNKNOWN = 0x990;
    
    constexpr uint16_t STEP_TYPE:_WALKING = 0x991;
    
    constexpr uint16_t STEP_TYPE:_RUNNING = 0x992;
    
    constexpr uint16_t GESTURE_STATE:_UNKNOWN = 0x9A0;
    
    constexpr uint16_t GESTURE_STATE:_STARTED = 0x9A1;
    
    constexpr uint16_t GESTURE_STATE:_COMPLETED = 0x9A2;
    
    constexpr uint16_t GESTURE_STATE:_CANCELLED = 0x9A3;
    
    constexpr uint16_t HINGE_FOLD_CONTRIBUTING_PANEL:_UNKNOWN = 0x9B0;
    
    constexpr uint16_t HINGE_FOLD_CONTRIBUTING_PANEL:_PANEL_1 = 0x9B1;
    
    constexpr uint16_t HINGE_FOLD_CONTRIBUTING_PANEL:_PANEL_2 = 0x9B2;
    
    constexpr uint16_t HINGE_FOLD_CONTRIBUTING_PANEL:_BOTH = 0x9B3;
    
    constexpr uint16_t HINGE_FOLD_TYPE:_UNKNOWN = 0x9B4;
    
    constexpr uint16_t HINGE_FOLD_TYPE:_INCREASING = 0x9B5;
    
    constexpr uint16_t HINGE_FOLD_TYPE:_DECREASING = 0x9B6;
    
    constexpr uint16_t HUMAN_PRESENCE_DETECTION_TYPE:_VENDOR_DEFINED_NON_BIOMETRIC = 0x9C0;
    
    constexpr uint16_t HUMAN_PRESENCE_DETECTION_TYPE:_VENDOR_DEFINED_BIOMETRIC = 0x9C1;
    
    constexpr uint16_t HUMAN_PRESENCE_DETECTION_TYPE:_FACIAL_BIOMETRIC = 0x9C2;
    
    constexpr uint16_t HUMAN_PRESENCE_DETECTION_TYPE:_AUDIO_BIOMETRIC = 0x9C3;

    // ==========================================
    // KIND: US (US)
    // ==========================================
    
    constexpr uint16_t MODIFIER:_CHANGE_SENSITIVITY_ABSOLUTE = 0x1000;
    
    constexpr uint16_t MODIFIER:_MAXIMUM = 0x2000;
    
    constexpr uint16_t MODIFIER:_MINIMUM = 0x3000;
    
    constexpr uint16_t MODIFIER:_ACCURACY = 0x4000;
    
    constexpr uint16_t MODIFIER:_RESOLUTION = 0x5000;
    
    constexpr uint16_t MODIFIER:_THRESHOLD_HIGH = 0x6000;
    
    constexpr uint16_t MODIFIER:_THRESHOLD_LOW = 0x7000;
    
    constexpr uint16_t MODIFIER:_CALIBRATION_OFFSET = 0x8000;
    
    constexpr uint16_t MODIFIER:_CALIBRATION_MULTIPLIER = 0x9000;
    
    constexpr uint16_t MODIFIER:_REPORT_INTERVAL = 0xA000;
    
    constexpr uint16_t MODIFIER:_FREQUENCY_MAX = 0xB000;
    
    constexpr uint16_t MODIFIER:_PERIOD_MAX = 0xC000;
    
    constexpr uint16_t MODIFIER:_CHANGE_SENSITIVITY_PERCENT_OF_RANGE = 0xD000;
    
    constexpr uint16_t MODIFIER:_CHANGE_SENSITIVITY_PERCENT_RELATIVE = 0xE000;
    
    constexpr uint16_t MODIFIER:_VENDOR_RESERVED = 0xF000;

}
