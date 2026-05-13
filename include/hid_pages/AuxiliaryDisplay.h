#pragma once
#include "../HID_Types.h"

namespace HID::AuxiliaryDisplay {
    constexpr uint8_t PAGE_ID = 0x0014;
    namespace OTHER {
        constexpr Usage ASCII_CHARACTER_SET = { 0x21, UsageKind::OTHER, "ASCII_CHARACTER_SET" };
        constexpr Usage DATA_READ_BACK = { 0x22, UsageKind::OTHER, "DATA_READ_BACK" };
        constexpr Usage FONT_READ_BACK = { 0x23, UsageKind::OTHER, "FONT_READ_BACK" };
        constexpr Usage CLEAR_DISPLAY = { 0x25, UsageKind::OTHER, "CLEAR_DISPLAY" };
        constexpr Usage DISPLAY_ENABLE = { 0x26, UsageKind::OTHER, "DISPLAY_ENABLE" };
        constexpr Usage SCREEN_SAVER_DELAY = { 0x27, UsageKind::OTHER, "SCREEN_SAVER_DELAY" };
        constexpr Usage SCREEN_SAVER_ENABLE = { 0x28, UsageKind::OTHER, "SCREEN_SAVER_ENABLE" };
        constexpr Usage VERTICAL_SCROLL = { 0x29, UsageKind::OTHER, "VERTICAL_SCROLL" };
        constexpr Usage HORIZONTAL_SCROLL = { 0x2A, UsageKind::OTHER, "HORIZONTAL_SCROLL" };
        constexpr Usage ROWS = { 0x35, UsageKind::OTHER, "ROWS" };
        constexpr Usage COLUMNS = { 0x36, UsageKind::OTHER, "COLUMNS" };
        constexpr Usage CURSOR_PIXEL_POSITIONING = { 0x37, UsageKind::OTHER, "CURSOR_PIXEL_POSITIONING" };
        constexpr Usage CURSOR_MODE = { 0x38, UsageKind::OTHER, "CURSOR_MODE" };
        constexpr Usage CURSOR_ENABLE = { 0x39, UsageKind::OTHER, "CURSOR_ENABLE" };
        constexpr Usage CURSOR_BLINK = { 0x3A, UsageKind::OTHER, "CURSOR_BLINK" };
        constexpr Usage FONT_DATA = { 0x3C, UsageKind::OTHER, "FONT_DATA" };
        constexpr Usage CHARACTER_WIDTH = { 0x3D, UsageKind::OTHER, "CHARACTER_WIDTH" };
        constexpr Usage CHARACTER_HEIGHT = { 0x3E, UsageKind::OTHER, "CHARACTER_HEIGHT" };
        constexpr Usage CHARACTER_SPACING_HORIZONTAL = { 0x3F, UsageKind::OTHER, "CHARACTER_SPACING_HORIZONTAL" };
        constexpr Usage CHARACTER_SPACING_VERTICAL = { 0x40, UsageKind::OTHER, "CHARACTER_SPACING_VERTICAL" };
        constexpr Usage UNICODE_CHARACTER_SET = { 0x41, UsageKind::OTHER, "UNICODE_CHARACTER_SET" };
        constexpr Usage FONT_7_SEGMENT = { 0x42, UsageKind::OTHER, "FONT_7_SEGMENT" };
        constexpr Usage 7_SEGMENT_DIRECT_MAP = { 0x43, UsageKind::OTHER, "7_SEGMENT_DIRECT_MAP" };
        constexpr Usage FONT_14_SEGMENT = { 0x44, UsageKind::OTHER, "FONT_14_SEGMENT" };
        constexpr Usage 14_SEGMENT_DIRECT_MAP = { 0x45, UsageKind::OTHER, "14_SEGMENT_DIRECT_MAP" };
        constexpr Usage ATTRIBUTE_READBACK = { 0x49, UsageKind::OTHER, "ATTRIBUTE_READBACK" };
        constexpr Usage BITMAP_SIZE_X = { 0x80, UsageKind::OTHER, "BITMAP_SIZE_X" };
        constexpr Usage BITMAP_SIZE_Y = { 0x81, UsageKind::OTHER, "BITMAP_SIZE_Y" };
        constexpr Usage MAX_BLIT_SIZE = { 0x82, UsageKind::OTHER, "MAX_BLIT_SIZE" };
        constexpr Usage BIT_DEPTH_FORMAT = { 0x83, UsageKind::OTHER, "BIT_DEPTH_FORMAT" };
        constexpr Usage PALETTE_DATA_SIZE = { 0x86, UsageKind::OTHER, "PALETTE_DATA_SIZE" };
        constexpr Usage PALETTE_DATA_OFFSET = { 0x87, UsageKind::OTHER, "PALETTE_DATA_OFFSET" };
        constexpr Usage PALETTE_DATA = { 0x88, UsageKind::OTHER, "PALETTE_DATA" };
        constexpr Usage BLIT_RECTANGLE_X1 = { 0x8B, UsageKind::OTHER, "BLIT_RECTANGLE_X1" };
        constexpr Usage BLIT_RECTANGLE_Y1 = { 0x8C, UsageKind::OTHER, "BLIT_RECTANGLE_Y1" };
        constexpr Usage BLIT_RECTANGLE_X2 = { 0x8D, UsageKind::OTHER, "BLIT_RECTANGLE_X2" };
        constexpr Usage BLIT_RECTANGLE_Y2 = { 0x8E, UsageKind::OTHER, "BLIT_RECTANGLE_Y2" };
        constexpr Usage BLIT_DATA = { 0x8F, UsageKind::OTHER, "BLIT_DATA" };
        constexpr Usage SOFT_BUTTON_ID = { 0x91, UsageKind::OTHER, "SOFT_BUTTON_ID" };
        constexpr Usage SOFT_BUTTON_SIDE = { 0x92, UsageKind::OTHER, "SOFT_BUTTON_SIDE" };
        constexpr Usage SOFT_BUTTON_OFFSET_1 = { 0x93, UsageKind::OTHER, "SOFT_BUTTON_OFFSET_1" };
        constexpr Usage SOFT_BUTTON_OFFSET_2 = { 0x94, UsageKind::OTHER, "SOFT_BUTTON_OFFSET_2" };
        constexpr Usage SOFT_BUTTON_REPORT = { 0x95, UsageKind::OTHER, "SOFT_BUTTON_REPORT" };
        constexpr Usage SOFT_KEYS = { 0xC2, UsageKind::OTHER, "SOFT_KEYS" };
        constexpr Usage DISPLAY_DATA_EXTENSIONS = { 0xCC, UsageKind::OTHER, "DISPLAY_DATA_EXTENSIONS" };
        constexpr Usage CHARACTER_MAPPING = { 0xCF, UsageKind::OTHER, "CHARACTER_MAPPING" };
        constexpr Usage UNICODE_EQUIVALENT = { 0xDD, UsageKind::OTHER, "UNICODE_EQUIVALENT" };
        constexpr Usage CHARACTER_PAGE_MAPPING = { 0xDF, UsageKind::OTHER, "CHARACTER_PAGE_MAPPING" };
    }
    namespace OOC {
        constexpr Usage CHAR_ATTR_ENHANCE = { 0x4B, UsageKind::OOC, "CHAR_ATTR_ENHANCE" };
        constexpr Usage CHAR_ATTR_UNDERLINE = { 0x4C, UsageKind::OOC, "CHAR_ATTR_UNDERLINE" };
        constexpr Usage CHAR_ATTR_BLINK = { 0x4D, UsageKind::OOC, "CHAR_ATTR_BLINK" };
    }
    namespace DV {
        constexpr Usage DISPLAY_DATA = { 0x2C, UsageKind::DV, "DISPLAY_DATA" };
        constexpr Usage ROW = { 0x33, UsageKind::DV, "ROW" };
        constexpr Usage COLUMN = { 0x34, UsageKind::DV, "COLUMN" };
        constexpr Usage DISPLAY_BRIGHTNESS = { 0x46, UsageKind::DV, "DISPLAY_BRIGHTNESS" };
        constexpr Usage DISPLAY_CONTRAST = { 0x47, UsageKind::DV, "DISPLAY_CONTRAST" };
        constexpr Usage ATTRIBUTE_DATA = { 0x4A, UsageKind::DV, "ATTRIBUTE_DATA" };
        constexpr Usage DISPLAY_ORIENTATION = { 0x84, UsageKind::DV, "DISPLAY_ORIENTATION" };
        constexpr Usage REQUEST_REPORT = { 0xFF, UsageKind::DV, "REQUEST_REPORT" };
    }
    namespace CL {
        constexpr Usage DISPLAY_ATTRIBUTES_REPORT = { 0x20, UsageKind::CL, "DISPLAY_ATTRIBUTES_REPORT" };
        constexpr Usage DISPLAY_CONTROL_REPORT = { 0x24, UsageKind::CL, "DISPLAY_CONTROL_REPORT" };
        constexpr Usage CHARACTER_REPORT = { 0x2B, UsageKind::CL, "CHARACTER_REPORT" };
        constexpr Usage DISPLAY_STATUS = { 0x2D, UsageKind::CL, "DISPLAY_STATUS" };
        constexpr Usage FONT_REPORT = { 0x3B, UsageKind::CL, "FONT_REPORT" };
        constexpr Usage CHARACTER_ATTRIBUTE = { 0x48, UsageKind::CL, "CHARACTER_ATTRIBUTE" };
        constexpr Usage PALETTE_REPORT = { 0x85, UsageKind::CL, "PALETTE_REPORT" };
        constexpr Usage BLIT_REPORT = { 0x8A, UsageKind::CL, "BLIT_REPORT" };
        constexpr Usage SOFT_BUTTON = { 0x90, UsageKind::CL, "SOFT_BUTTON" };
    }
    namespace Sel {
        constexpr Usage STAT_NOT_READY = { 0x2E, UsageKind::Sel, "STAT_NOT_READY" };
        constexpr Usage STAT_READY = { 0x2F, UsageKind::Sel, "STAT_READY" };
        constexpr Usage ERR_NOT_A_LOADABLE_CHARACTER = { 0x30, UsageKind::Sel, "ERR_NOT_A_LOADABLE_CHARACTER" };
        constexpr Usage ERR_FONT_DATA_CANNOT_BE_READ = { 0x31, UsageKind::Sel, "ERR_FONT_DATA_CANNOT_BE_READ" };
        constexpr Usage CURSOR_POSITION_REPORT = { 0x32, UsageKind::Sel, "CURSOR_POSITION_REPORT" };
    }
    namespace CA {
        constexpr Usage ALPHANUMERIC_DISPLAY = { 0x01, UsageKind::CA, "ALPHANUMERIC_DISPLAY" };
        constexpr Usage AUXILIARY_DISPLAY = { 0x02, UsageKind::CA, "AUXILIARY_DISPLAY" };
    }
}
