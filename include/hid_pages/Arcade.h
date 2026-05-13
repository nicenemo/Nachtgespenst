#pragma once
#include "../HID_Types.h"

namespace HID::Arcade {
    constexpr uint8_t PAGE_ID = 0x0091;
    namespace OTHER {
        constexpr Usage WATCHDOG_ACTION = { 0x42, UsageKind::OTHER, "WATCHDOG_ACTION" };
    }
    namespace OOC {
        constexpr Usage COIN_DRAWER_START = { 0x36, UsageKind::OOC, "COIN_DRAWER_START" };
        constexpr Usage COIN_DRAWER_SERVICE = { 0x37, UsageKind::OOC, "COIN_DRAWER_SERVICE" };
        constexpr Usage COIN_DRAWER_TILT = { 0x38, UsageKind::OOC, "COIN_DRAWER_TILT" };
        constexpr Usage COIN_DOOR_TEST = { 0x39, UsageKind::OOC, "COIN_DOOR_TEST" };
        constexpr Usage COIN_DOOR_LOCKOUT = { 0x40, UsageKind::OOC, "COIN_DOOR_LOCKOUT" };
        constexpr Usage COIN_DOOR_COUNTER = { 0x46, UsageKind::OOC, "COIN_DOOR_COUNTER" };
        constexpr Usage PIN_PAD_OUTPUT = { 0x4C, UsageKind::OOC, "PIN_PAD_OUTPUT" };
    }
    namespace DV {
        constexpr Usage GENERAL_PURPOSE_ANALOG_INPUT_STATE = { 0x30, UsageKind::DV, "GENERAL_PURPOSE_ANALOG_INPUT_STATE" };
        constexpr Usage GENERAL_PURPOSE_DIGITAL_INPUT_STATE = { 0x31, UsageKind::DV, "GENERAL_PURPOSE_DIGITAL_INPUT_STATE" };
        constexpr Usage GENERAL_PURPOSE_OPTICAL_INPUT_STATE = { 0x32, UsageKind::DV, "GENERAL_PURPOSE_OPTICAL_INPUT_STATE" };
        constexpr Usage GENERAL_PURPOSE_DIGITAL_OUTPUT_STATE = { 0x33, UsageKind::DV, "GENERAL_PURPOSE_DIGITAL_OUTPUT_STATE" };
        constexpr Usage NUMBER_OF_COIN_DOORS = { 0x34, UsageKind::DV, "NUMBER_OF_COIN_DOORS" };
        constexpr Usage COIN_DRAWER_DROP_COUNT = { 0x35, UsageKind::DV, "COIN_DRAWER_DROP_COUNT" };
        constexpr Usage WATCHDOG_TIMEOUT = { 0x41, UsageKind::DV, "WATCHDOG_TIMEOUT" };
        constexpr Usage ALARM_INPUT = { 0x45, UsageKind::DV, "ALARM_INPUT" };
        constexpr Usage I_O_DIRECTION_MAPPING = { 0x47, UsageKind::DV, "I_O_DIRECTION_MAPPING" };
        constexpr Usage SET_I_O_DIRECTION_MAPPING = { 0x48, UsageKind::DV, "SET_I_O_DIRECTION_MAPPING" };
        constexpr Usage EXTENDED_OPTICAL_INPUT_STATE = { 0x49, UsageKind::DV, "EXTENDED_OPTICAL_INPUT_STATE" };
        constexpr Usage PIN_PAD_INPUT_STATE = { 0x4A, UsageKind::DV, "PIN_PAD_INPUT_STATE" };
        constexpr Usage PIN_PAD_STATUS = { 0x4B, UsageKind::DV, "PIN_PAD_STATUS" };
        constexpr Usage PIN_PAD_COMMAND = { 0x4D, UsageKind::DV, "PIN_PAD_COMMAND" };
    }
    namespace Sel {
        constexpr Usage WATCHDOG_REBOOT = { 0x43, UsageKind::Sel, "WATCHDOG_REBOOT" };
        constexpr Usage WATCHDOG_RESTART = { 0x44, UsageKind::Sel, "WATCHDOG_RESTART" };
    }
    namespace CA {
        constexpr Usage GENERAL_PURPOSE_IO_CARD = { 0x01, UsageKind::CA, "GENERAL_PURPOSE_IO_CARD" };
        constexpr Usage COIN_DOOR = { 0x02, UsageKind::CA, "COIN_DOOR" };
        constexpr Usage WATCHDOG_TIMER = { 0x03, UsageKind::CA, "WATCHDOG_TIMER" };
    }
}
