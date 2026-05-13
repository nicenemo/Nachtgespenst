#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Power (0x84)
 */
namespace HID::Power {
    constexpr uint8_t PAGE_ID = 0x84;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t UPS = 0x04;
    
    constexpr uint8_t POWER_SUPPLY = 0x05;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t PRESENT_STATUS = 0x02;
    
    constexpr uint8_t CHANGED_STATUS = 0x03;
    
    constexpr uint8_t GANG = 0x22;
    
    constexpr uint8_t POWER_SUMMARY = 0x24;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t BATTERY_SYSTEM = 0x10;
    
    constexpr uint8_t BATTERY = 0x12;
    
    constexpr uint8_t CHARGER = 0x14;
    
    constexpr uint8_t POWER_CONVERTER = 0x16;
    
    constexpr uint8_t OUTLET_SYSTEM = 0x18;
    
    constexpr uint8_t INPUT = 0x1A;
    
    constexpr uint8_t OUTPUT = 0x1C;
    
    constexpr uint8_t FLOW = 0x1E;
    
    constexpr uint8_t OUTLET = 0x20;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t PRESENT = 0x60;
    
    constexpr uint8_t GOOD = 0x61;
    
    constexpr uint8_t INTERNAL_FAILURE = 0x62;
    
    constexpr uint8_t VOLTAG_OUT_OF_RANGE = 0x63;
    
    constexpr uint8_t FREQUENCY_OUT_OF_RANGE = 0x64;
    
    constexpr uint8_t OVERLOAD = 0x65;
    
    constexpr uint8_t OVER_CHARGED = 0x66;
    
    constexpr uint8_t OVER_TEMPERATURE = 0x67;
    
    constexpr uint8_t SHUTDOWN_REQUESTED = 0x68;
    
    constexpr uint8_t SHUTDOWN_IMMINENT = 0x69;
    
    constexpr uint8_t SWITCH_ON_OFF = 0x6B;
    
    constexpr uint8_t SWITCHABLE = 0x6C;
    
    constexpr uint8_t USED = 0x6D;
    
    constexpr uint8_t BOOST = 0x6E;
    
    constexpr uint8_t BUCK = 0x6F;
    
    constexpr uint8_t INITIALIZED = 0x70;
    
    constexpr uint8_t TESTED = 0x71;
    
    constexpr uint8_t AWAITING_POWER = 0x72;
    
    constexpr uint8_t COMMUNICATION_LOST = 0x73;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t VOLTAGE = 0x30;
    
    constexpr uint8_t CURRENT = 0x31;
    
    constexpr uint8_t FREQUENCY = 0x32;
    
    constexpr uint8_t APPARENT_POWER = 0x33;
    
    constexpr uint8_t ACTIVE_POWER = 0x34;
    
    constexpr uint8_t PERCENT_LOAD = 0x35;
    
    constexpr uint8_t TEMPERATURE = 0x36;
    
    constexpr uint8_t HUMIDITY = 0x37;
    
    constexpr uint8_t BAD_COUNT = 0x38;
    
    constexpr uint8_t SWITCH_ON_CONTROL = 0x50;
    
    constexpr uint8_t SWITCH_OFF_CONTROL = 0x51;
    
    constexpr uint8_t TOGGLE_CONTROL = 0x52;
    
    constexpr uint8_t LOW_VOLTAGE_TRANSFER = 0x53;
    
    constexpr uint8_t HIGH_VOLTAGE_TRANSFER = 0x54;
    
    constexpr uint8_t DELAY_BEFORE_REBOOT = 0x55;
    
    constexpr uint8_t DELAY_BEFORE_STARTUP = 0x56;
    
    constexpr uint8_t DELAY_BEFORE_SHUTDOWN = 0x57;
    
    constexpr uint8_t TEST = 0x58;
    
    constexpr uint8_t MODULE_RESET = 0x59;
    
    constexpr uint8_t AUDIBLE_ALARM_CONTROL = 0x5A;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t INAME = 0x01;
    
    constexpr uint8_t BATTERY_SYS_ID = 0x11;
    
    constexpr uint8_t BATTERY_ID = 0x13;
    
    constexpr uint8_t CHARGER_ID = 0x15;
    
    constexpr uint8_t POWER_CONVERTER_ID = 0x17;
    
    constexpr uint8_t OUTLET_SYS_ID = 0x19;
    
    constexpr uint8_t INPUT_ID = 0x1B;
    
    constexpr uint8_t OUTPUT_ID = 0x1D;
    
    constexpr uint8_t FLOW_ID = 0x1F;
    
    constexpr uint8_t OUTLET_ID = 0x21;
    
    constexpr uint8_t GANG_ID = 0x23;
    
    constexpr uint8_t POWER_SUMMARY_ID = 0x25;
    
    constexpr uint8_t CONFIG_VOLTAGE = 0x40;
    
    constexpr uint8_t CONFIG_CURRENT = 0x41;
    
    constexpr uint8_t CONFIG_FREQUENCY = 0x42;
    
    constexpr uint8_t CONFIG_APPARENT_POWER = 0x43;
    
    constexpr uint8_t CONFIG_ACTIVE_POWER = 0x44;
    
    constexpr uint8_t CONFIG_PERCENT_LOAD = 0x45;
    
    constexpr uint8_t CONFIG_TEMPERATURE = 0x46;
    
    constexpr uint8_t CONFIG_HUMIDITY = 0x47;
    
    constexpr uint8_t IMANUFACTURER = 0xFD;
    
    constexpr uint8_t IPRODUCT = 0xFE;
    
    constexpr uint8_t ISERIALNUMBER = 0xFF;

}
