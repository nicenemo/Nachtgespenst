#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Arcade (0x91)
 */
namespace HID::Arcade {
    constexpr uint8_t PAGE_ID = 0x91;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t GENERAL_PURPOSE_IO_CARD = 0x01;
    
    constexpr uint8_t COIN_DOOR = 0x02;
    
    constexpr uint8_t WATCHDOG_TIMER = 0x03;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t GENERAL_PURPOSE_ANALOG_INPUT_STATE = 0x30;
    
    constexpr uint8_t GENERAL_PURPOSE_DIGITAL_INPUT_STATE = 0x31;
    
    constexpr uint8_t GENERAL_PURPOSE_OPTICAL_INPUT_STATE = 0x32;
    
    constexpr uint8_t GENERAL_PURPOSE_DIGITAL_OUTPUT_STATE = 0x33;
    
    constexpr uint8_t NUMBER_OF_COIN_DOORS = 0x34;
    
    constexpr uint8_t COIN_DRAWER_DROP_COUNT = 0x35;
    
    constexpr uint8_t WATCHDOG_TIMEOUT = 0x41;
    
    constexpr uint8_t ALARM_INPUT = 0x45;
    
    constexpr uint8_t I_O_DIRECTION_MAPPING = 0x47;
    
    constexpr uint8_t SET_I_O_DIRECTION_MAPPING = 0x48;
    
    constexpr uint8_t EXTENDED_OPTICAL_INPUT_STATE = 0x49;
    
    constexpr uint8_t PIN_PAD_INPUT_STATE = 0x4A;
    
    constexpr uint8_t PIN_PAD_STATUS = 0x4B;
    
    constexpr uint8_t PIN_PAD_COMMAND = 0x4D;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t WATCHDOG_ACTION = 0x42;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t COIN_DRAWER_START = 0x36;
    
    constexpr uint8_t COIN_DRAWER_SERVICE = 0x37;
    
    constexpr uint8_t COIN_DRAWER_TILT = 0x38;
    
    constexpr uint8_t COIN_DOOR_TEST = 0x39;
    
    constexpr uint8_t COIN_DOOR_LOCKOUT = 0x40;
    
    constexpr uint8_t COIN_DOOR_COUNTER = 0x46;
    
    constexpr uint8_t PIN_PAD_OUTPUT = 0x4C;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t WATCHDOG_REBOOT = 0x43;
    
    constexpr uint8_t WATCHDOG_RESTART = 0x44;

}
