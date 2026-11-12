#pragma once
#include "../HID_Types.h"

namespace HID::BrailleDisplay {
    constexpr uint16_t PAGE_ID = 0x0041;
    namespace OTHER {
        constexpr Usage BRAILLE_DISPLAY = { 0x01, UsageKind::OTHER, "BRAILLE_DISPLAY" };
        constexpr Usage BRAILLE_ROW = { 0x02, UsageKind::OTHER, "BRAILLE_ROW" };
        constexpr Usage ID_8_DOT_BRAILLE_CELL = { 0x03, UsageKind::OTHER, "ID_8_DOT_BRAILLE_CELL" };
        constexpr Usage ID_6_DOT_BRAILLE_CELL = { 0x04, UsageKind::OTHER, "ID_6_DOT_BRAILLE_CELL" };
        constexpr Usage NUMBER_OF_BRAILLE_CELLS = { 0x05, UsageKind::OTHER, "NUMBER_OF_BRAILLE_CELLS" };
        constexpr Usage SCREEN_READER_CONTROL = { 0x06, UsageKind::OTHER, "SCREEN_READER_CONTROL" };
        constexpr Usage SCREEN_READER_IDENTIFIER = { 0x07, UsageKind::OTHER, "SCREEN_READER_IDENTIFIER" };
        constexpr Usage ROUTER_SET_1 = { 0xFA, UsageKind::OTHER, "ROUTER_SET_1" };
        constexpr Usage ROUTER_SET_2 = { 0xFB, UsageKind::OTHER, "ROUTER_SET_2" };
        constexpr Usage ROUTER_SET_3 = { 0xFC, UsageKind::OTHER, "ROUTER_SET_3" };
        constexpr Usage ROUTER_KEY = { 0x100, UsageKind::OTHER, "ROUTER_KEY" };
        constexpr Usage ROW_ROUTER_KEY = { 0x101, UsageKind::OTHER, "ROW_ROUTER_KEY" };
        constexpr Usage BRAILLE_BUTTONS = { 0x200, UsageKind::OTHER, "BRAILLE_BUTTONS" };
        constexpr Usage BRAILLE_DOT_1 = { 0x201, UsageKind::OTHER, "BRAILLE_DOT_1" };
        constexpr Usage BRAILLE_DOT_2 = { 0x202, UsageKind::OTHER, "BRAILLE_DOT_2" };
        constexpr Usage BRAILLE_DOT_3 = { 0x203, UsageKind::OTHER, "BRAILLE_DOT_3" };
        constexpr Usage BRAILLE_DOT_4 = { 0x204, UsageKind::OTHER, "BRAILLE_DOT_4" };
        constexpr Usage BRAILLE_DOT_5 = { 0x205, UsageKind::OTHER, "BRAILLE_DOT_5" };
        constexpr Usage BRAILLE_DOT_6 = { 0x206, UsageKind::OTHER, "BRAILLE_DOT_6" };
        constexpr Usage BRAILLE_DOT_7 = { 0x207, UsageKind::OTHER, "BRAILLE_DOT_7" };
        constexpr Usage BRAILLE_DOT_8 = { 0x208, UsageKind::OTHER, "BRAILLE_DOT_8" };
        constexpr Usage BRAILLE_SPACE = { 0x209, UsageKind::OTHER, "BRAILLE_SPACE" };
        constexpr Usage BRAILLE_LEFT_SPACE = { 0x20A, UsageKind::OTHER, "BRAILLE_LEFT_SPACE" };
        constexpr Usage BRAILLE_RIGHT_SPACE = { 0x20B, UsageKind::OTHER, "BRAILLE_RIGHT_SPACE" };
        constexpr Usage BRAILLE_FACE_CONTROLS = { 0x20C, UsageKind::OTHER, "BRAILLE_FACE_CONTROLS" };
        constexpr Usage BRAILLE_LEFT_CONTROLS = { 0x20D, UsageKind::OTHER, "BRAILLE_LEFT_CONTROLS" };
        constexpr Usage BRAILLE_RIGHT_CONTROLS = { 0x20E, UsageKind::OTHER, "BRAILLE_RIGHT_CONTROLS" };
        constexpr Usage BRAILLE_TOP_CONTROLS = { 0x20F, UsageKind::OTHER, "BRAILLE_TOP_CONTROLS" };
        constexpr Usage BRAILLE_JOYSTICK_CENTER = { 0x210, UsageKind::OTHER, "BRAILLE_JOYSTICK_CENTER" };
        constexpr Usage BRAILLE_JOYSTICK_UP = { 0x211, UsageKind::OTHER, "BRAILLE_JOYSTICK_UP" };
        constexpr Usage BRAILLE_JOYSTICK_DOWN = { 0x212, UsageKind::OTHER, "BRAILLE_JOYSTICK_DOWN" };
        constexpr Usage BRAILLE_JOYSTICK_LEFT = { 0x213, UsageKind::OTHER, "BRAILLE_JOYSTICK_LEFT" };
        constexpr Usage BRAILLE_JOYSTICK_RIGHT = { 0x214, UsageKind::OTHER, "BRAILLE_JOYSTICK_RIGHT" };
        constexpr Usage BRAILLE_D_PAD_CENTER = { 0x215, UsageKind::OTHER, "BRAILLE_D_PAD_CENTER" };
        constexpr Usage BRAILLE_D_PAD_UP = { 0x216, UsageKind::OTHER, "BRAILLE_D_PAD_UP" };
        constexpr Usage BRAILLE_D_PAD_DOWN = { 0x217, UsageKind::OTHER, "BRAILLE_D_PAD_DOWN" };
        constexpr Usage BRAILLE_D_PAD_LEFT = { 0x218, UsageKind::OTHER, "BRAILLE_D_PAD_LEFT" };
        constexpr Usage BRAILLE_D_PAD_RIGHT = { 0x219, UsageKind::OTHER, "BRAILLE_D_PAD_RIGHT" };
        constexpr Usage BRAILLE_PAN_LEFT = { 0x21A, UsageKind::OTHER, "BRAILLE_PAN_LEFT" };
        constexpr Usage BRAILLE_PAN_RIGHT = { 0x21B, UsageKind::OTHER, "BRAILLE_PAN_RIGHT" };
        constexpr Usage BRAILLE_ROCKER_UP = { 0x21C, UsageKind::OTHER, "BRAILLE_ROCKER_UP" };
        constexpr Usage BRAILLE_ROCKER_DOWN = { 0x21D, UsageKind::OTHER, "BRAILLE_ROCKER_DOWN" };
        constexpr Usage BRAILLE_ROCKER_PRESS = { 0x21E, UsageKind::OTHER, "BRAILLE_ROCKER_PRESS" };
    }
}
