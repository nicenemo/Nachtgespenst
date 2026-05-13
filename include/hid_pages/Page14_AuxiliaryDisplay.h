#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Auxiliary Display (0x14)
 */
namespace HID::AuxiliaryDisplay {
    constexpr uint8_t PAGE_ID = 0x14;

    // ==========================================
    // KIND: BufferedBytes (BufferedBytes)
    // ==========================================
    
    constexpr uint8_t FONT_DATA = 0x3C;
    
    constexpr uint8_t PALETTE_DATA = 0x88;
    
    constexpr uint8_t BLIT_DATA = 0x8F;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t ALPHANUMERIC_DISPLAY = 0x01;
    
    constexpr uint8_t AUXILIARY_DISPLAY = 0x02;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t DISPLAY_ATTRIBUTES_REPORT = 0x20;
    
    constexpr uint8_t DISPLAY_CONTROL_REPORT = 0x24;
    
    constexpr uint8_t CHARACTER_REPORT = 0x2B;
    
    constexpr uint8_t DISPLAY_STATUS = 0x2D;
    
    constexpr uint8_t FONT_REPORT = 0x3B;
    
    constexpr uint8_t CHARACTER_ATTRIBUTE = 0x48;
    
    constexpr uint8_t PALETTE_REPORT = 0x85;
    
    constexpr uint8_t BLIT_REPORT = 0x8A;
    
    constexpr uint8_t SOFT_BUTTON = 0x90;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t CLEAR_DISPLAY = 0x25;
    
    constexpr uint8_t DISPLAY_ENABLE = 0x26;
    
    constexpr uint8_t SCREEN_SAVER_ENABLE = 0x28;
    
    constexpr uint8_t CURSOR_MODE = 0x38;
    
    constexpr uint8_t CURSOR_ENABLE = 0x39;
    
    constexpr uint8_t CURSOR_BLINK = 0x3A;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t DISPLAY_DATA = 0x2C;
    
    constexpr uint8_t ROW = 0x33;
    
    constexpr uint8_t COLUMN = 0x34;
    
    constexpr uint8_t DISPLAY_BRIGHTNESS = 0x46;
    
    constexpr uint8_t DISPLAY_CONTRAST = 0x47;
    
    constexpr uint8_t ATTRIBUTE_DATA = 0x4A;
    
    constexpr uint8_t DISPLAY_ORIENTATION = 0x84;
    
    constexpr uint8_t REQUEST_REPORT = 0xFF;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t CHAR_ATTR_ENHANCE = 0x4B;
    
    constexpr uint8_t CHAR_ATTR_UNDERLINE = 0x4C;
    
    constexpr uint8_t CHAR_ATTR_BLINK = 0x4D;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint8_t ASCII_CHARACTER_SET = 0x21;
    
    constexpr uint8_t DATA_READ_BACK = 0x22;
    
    constexpr uint8_t FONT_READ_BACK = 0x23;
    
    constexpr uint8_t VERTICAL_SCROLL = 0x29;
    
    constexpr uint8_t HORIZONTAL_SCROLL = 0x2A;
    
    constexpr uint8_t CURSOR_PIXEL_POSITIONING = 0x37;
    
    constexpr uint8_t UNICODE_CHARACTER_SET = 0x41;
    
    constexpr uint8_t FONT_7_SEGMENT = 0x42;
    
    constexpr uint8_t ID_7_SEGMENT_DIRECT_MAP = 0x43;
    
    constexpr uint8_t FONT_14_SEGMENT = 0x44;
    
    constexpr uint8_t ID_14_SEGMENT_DIRECT_MAP = 0x45;
    
    constexpr uint8_t ATTRIBUTE_READBACK = 0x49;
    
    constexpr uint8_t DISPLAY_DATA_EXTENSIONS = 0xCC;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t SCREEN_SAVER_DELAY = 0x27;
    
    constexpr uint8_t ROWS = 0x35;
    
    constexpr uint8_t COLUMNS = 0x36;
    
    constexpr uint8_t CHARACTER_WIDTH = 0x3D;
    
    constexpr uint8_t CHARACTER_HEIGHT = 0x3E;
    
    constexpr uint8_t CHARACTER_SPACING_HORIZONTAL = 0x3F;
    
    constexpr uint8_t CHARACTER_SPACING_VERTICAL = 0x40;
    
    constexpr uint8_t BITMAP_SIZE_X = 0x80;
    
    constexpr uint8_t BITMAP_SIZE_Y = 0x81;
    
    constexpr uint8_t MAX_BLIT_SIZE = 0x82;
    
    constexpr uint8_t BIT_DEPTH_FORMAT = 0x83;
    
    constexpr uint8_t PALETTE_DATA_SIZE = 0x86;
    
    constexpr uint8_t PALETTE_DATA_OFFSET = 0x87;
    
    constexpr uint8_t BLIT_RECTANGLE_X1 = 0x8B;
    
    constexpr uint8_t BLIT_RECTANGLE_Y1 = 0x8C;
    
    constexpr uint8_t BLIT_RECTANGLE_X2 = 0x8D;
    
    constexpr uint8_t BLIT_RECTANGLE_Y2 = 0x8E;
    
    constexpr uint8_t SOFT_BUTTON_ID = 0x91;
    
    constexpr uint8_t SOFT_BUTTON_SIDE = 0x92;
    
    constexpr uint8_t SOFT_BUTTON_OFFSET_1 = 0x93;
    
    constexpr uint8_t SOFT_BUTTON_OFFSET_2 = 0x94;
    
    constexpr uint8_t SOFT_BUTTON_REPORT = 0x95;
    
    constexpr uint8_t SOFT_KEYS = 0xC2;
    
    constexpr uint8_t CHARACTER_MAPPING = 0xCF;
    
    constexpr uint8_t UNICODE_EQUIVALENT = 0xDD;
    
    constexpr uint8_t CHARACTER_PAGE_MAPPING = 0xDF;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t STAT_NOT_READY = 0x2E;
    
    constexpr uint8_t STAT_READY = 0x2F;
    
    constexpr uint8_t ERR_NOT_A_LOADABLE_CHARACTER = 0x30;
    
    constexpr uint8_t ERR_FONT_DATA_CANNOT_BE_READ = 0x31;
    
    constexpr uint8_t CURSOR_POSITION_REPORT = 0x32;

}
