#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Battery System (0x85)
 */
namespace HID::BatterySystem {
    constexpr uint8_t PAGE_ID = 0x85;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t SMART_BATTERY_BATTERY_MODE = 0x01;
    
    constexpr uint8_t SMART_BATTERY_CHARGER_MODE = 0x04;
    
    constexpr uint8_t SMART_BATTERY_CHARGER_STATUS = 0x05;
    
    constexpr uint8_t SMART_BATTERY_CHARGER_SPEC_INFO = 0x06;
    
    constexpr uint8_t SMART_BATTERY_SELECTOR_STATE = 0x07;
    
    constexpr uint8_t SMART_BATTERY_SELECTOR_PRESETS = 0x08;
    
    constexpr uint8_t SMART_BATTERY_SELECTOR_INFO = 0x09;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t CONNECTION_TO_SM_BUS = 0x15;
    
    constexpr uint8_t OUTPUT_CONNECTION = 0x16;
    
    constexpr uint8_t CHARGER_CONNECTION = 0x17;
    
    constexpr uint8_t BATTERY_INSERTION = 0x18;
    
    constexpr uint8_t USE_NEXT = 0x19;
    
    constexpr uint8_t OK_TO_USE = 0x1A;
    
    constexpr uint8_t BATTERY_SUPPORTED = 0x1B;
    
    constexpr uint8_t SELECTOR_REVISION = 0x1C;
    
    constexpr uint8_t CHARGING_INDICATOR = 0x1D;
    
    constexpr uint8_t CONDITIONING_FLAG = 0x48;
    
    constexpr uint8_t AT_RATE_OK = 0x49;
    
    constexpr uint8_t NEED_REPLACEMENT = 0x4B;
    
    constexpr uint8_t INHIBIT_CHARGE = 0xC0;
    
    constexpr uint8_t ENABLE_POLLING = 0xC1;
    
    constexpr uint8_t RESET_TO_ZERO = 0xC2;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t OPTIONAL_MFG_FUNCTION_1 = 0x10;
    
    constexpr uint8_t OPTIONAL_MFG_FUNCTION_2 = 0x11;
    
    constexpr uint8_t OPTIONAL_MFG_FUNCTION_3 = 0x12;
    
    constexpr uint8_t OPTIONAL_MFG_FUNCTION_4 = 0x13;
    
    constexpr uint8_t OPTIONAL_MFG_FUNCTION_5 = 0x14;
    
    constexpr uint8_t MANUFACTURER_ACCESS = 0x28;
    
    constexpr uint8_t REMAINING_CAPACITY_LIMIT = 0x29;
    
    constexpr uint8_t REMAINING_TIME_LIMIT = 0x2A;
    
    constexpr uint8_t AT_RATE = 0x2B;
    
    constexpr uint8_t CAPACITY_MODE = 0x2C;
    
    constexpr uint8_t BROADCAST_TO_CHARGER = 0x2D;
    
    constexpr uint8_t PRIMARY_BATTERY = 0x2E;
    
    constexpr uint8_t CHARGE_CONTROLLER = 0x2F;
    
    constexpr uint8_t SMART_BATTERY_ERROR_CODE = 0x4A;
    
    constexpr uint8_t AT_RATE_TIME_TO_FULL = 0x60;
    
    constexpr uint8_t AT_RATE_TIME_TO_EMPTY = 0x61;
    
    constexpr uint8_t AVERAGE_CURRENT = 0x62;
    
    constexpr uint8_t MAX_ERROR = 0x63;
    
    constexpr uint8_t RELATIVE_STATE_OF_CHARGE = 0x64;
    
    constexpr uint8_t ABSOLUTE_STATE_OF_CHARGE = 0x65;
    
    constexpr uint8_t REMAINING_CAPACITY = 0x66;
    
    constexpr uint8_t FULL_CHARGE_CAPACITY = 0x67;
    
    constexpr uint8_t RUN_TIME_TO_EMPTY = 0x68;
    
    constexpr uint8_t AVERAGE_TIME_TO_EMPTY = 0x69;
    
    constexpr uint8_t AVERAGE_TIME_TO_FULL = 0x6A;
    
    constexpr uint8_t CYCLE_COUNT = 0x6B;
    
    constexpr uint8_t AC_PRESENT = 0xD0;
    
    constexpr uint8_t BATTERY_PRESENT = 0xD1;
    
    constexpr uint8_t POWER_FAIL = 0xD2;
    
    constexpr uint8_t ALARM_INHIBITED = 0xD3;
    
    constexpr uint8_t THERMISTOR_UNDER_RANGE = 0xD4;
    
    constexpr uint8_t THERMISTOR_HOT = 0xD5;
    
    constexpr uint8_t THERMISTOR_COLD = 0xD6;
    
    constexpr uint8_t THERMISTOR_OVER_RANGE = 0xD7;
    
    constexpr uint8_t VOLTAGE_OUT_OF_RANGE = 0xD8;
    
    constexpr uint8_t CURRENT_OUT_OF_RANGE = 0xD9;
    
    constexpr uint8_t CURRENT_NOT_REGULATED = 0xDA;
    
    constexpr uint8_t VOLTAGE_NOT_REGULATED = 0xDB;
    
    constexpr uint8_t MASTER_MODE = 0xDC;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t SMART_BATTERY_BATTERY_STATUS = 0x02;
    
    constexpr uint8_t SMART_BATTERY_ALARM_WARNING = 0x03;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint8_t INTERNAL_CHARGE_CONTROLLER = 0x81;
    
    constexpr uint8_t PRIMARY_BATTERY_SUPPORT = 0x82;
    
    constexpr uint8_t CHARGER_SELECTOR_SUPPORT = 0xF0;
    
    constexpr uint8_t LEVEL_2 = 0xF2;
    
    constexpr uint8_t LEVEL_3 = 0xF3;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t BATTERY_PACK_MODEL_LEVEL = 0x80;
    
    constexpr uint8_t DESIGN_CAPACITY = 0x83;
    
    constexpr uint8_t SPECIFICATION_INFO = 0x84;
    
    constexpr uint8_t MANUFACTURE_DATE = 0x85;
    
    constexpr uint8_t SERIAL_NUMBER = 0x86;
    
    constexpr uint8_t IMANUFACTURER_NAME = 0x87;
    
    constexpr uint8_t IDEVICE_NAME = 0x88;
    
    constexpr uint8_t IDEVICE_CHEMISTRY = 0x89;
    
    constexpr uint8_t MANUFACTURER_DATA = 0x8A;
    
    constexpr uint8_t RECHARGABLE = 0x8B;
    
    constexpr uint8_t WARNING_CAPACITY_LIMIT = 0x8C;
    
    constexpr uint8_t CAPACITY_GRANULARITY_1 = 0x8D;
    
    constexpr uint8_t CAPACITY_GRANULARITY_2 = 0x8E;
    
    constexpr uint8_t IOEM_INFORMATION = 0x8F;
    
    constexpr uint8_t CHARGER_SPEC = 0xF1;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t TERMINATE_CHARGE = 0x40;
    
    constexpr uint8_t TERMINATE_DISCHARGE = 0x41;
    
    constexpr uint8_t BELOW_REMAINING_CAPACITY_LIMIT = 0x42;
    
    constexpr uint8_t REMAINING_TIME_LIMIT_EXPIRED = 0x43;
    
    constexpr uint8_t CHARGING = 0x44;
    
    constexpr uint8_t DISCHARGING = 0x45;
    
    constexpr uint8_t FULLY_CHARGED = 0x46;
    
    constexpr uint8_t FULLY_DISCHARGED = 0x47;

}
