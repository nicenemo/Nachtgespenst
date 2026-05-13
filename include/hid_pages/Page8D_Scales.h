#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Scales (0x8D)
 */
namespace HID::Scales {
    constexpr uint8_t PAGE_ID = 0x8D;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t SCALES = 0x01;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t SCALE_DEVICE = 0x20;
    
    constexpr uint8_t SCALE_ATTRIBUTE_REPORT = 0x30;
    
    constexpr uint8_t SCALE_CONTROL_REPORT = 0x31;
    
    constexpr uint8_t SCALE_DATA_REPORT = 0x32;
    
    constexpr uint8_t SCALE_STATUS_REPORT = 0x33;
    
    constexpr uint8_t SCALE_WEIGHT_LIMIT_REPORT = 0x34;
    
    constexpr uint8_t SCALE_STATISTICS_REPORT = 0x35;
    
    constexpr uint8_t WEIGHT_UNIT = 0x50;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t DATA_WEIGHT = 0x40;
    
    constexpr uint8_t DATA_SCALING = 0x41;
    
    constexpr uint8_t CALIBRATION_COUNT = 0x60;
    
    constexpr uint8_t RE_ZERO_COUNT = 0x61;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint8_t SCALE_CLASS = 0x21;
    
    constexpr uint8_t SCALE_STATUS = 0x70;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t ZERO_SCALE = 0x80;
    
    constexpr uint8_t ENFORCED_ZERO_RETURN = 0x81;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint8_t SCALE_CLASS_I_METRIC = 0x22;
    
    constexpr uint8_t SCALE_CLASS_II_METRIC = 0x23;
    
    constexpr uint8_t SCALE_CLASS_III_METRIC = 0x24;
    
    constexpr uint8_t SCALE_CLASS_IIIL_METRIC = 0x25;
    
    constexpr uint8_t SCALE_CLASS_IV_METRIC = 0x26;
    
    constexpr uint8_t SCALE_CLASS_III_ENGLISH = 0x27;
    
    constexpr uint8_t SCALE_CLASS_IIIL_ENGLISH = 0x28;
    
    constexpr uint8_t SCALE_CLASS_IV_ENGLISH = 0x29;
    
    constexpr uint8_t SCALE_CLASS_GENERIC = 0x2A;
    
    constexpr uint8_t WEIGHT_UNIT_MILLIGRAM = 0x51;
    
    constexpr uint8_t WEIGHT_UNIT_GRAM = 0x52;
    
    constexpr uint8_t WEIGHT_UNIT_KILOGRAM = 0x53;
    
    constexpr uint8_t WEIGHT_UNIT_CARATS = 0x54;
    
    constexpr uint8_t WEIGHT_UNIT_TAELS = 0x55;
    
    constexpr uint8_t WEIGHT_UNIT_GRAINS = 0x56;
    
    constexpr uint8_t WEIGHT_UNIT_PENNYWEIGHTS = 0x57;
    
    constexpr uint8_t WEIGHT_UNIT_METRIC_TON = 0x58;
    
    constexpr uint8_t WEIGHT_UNIT_AVOIR_TON = 0x59;
    
    constexpr uint8_t WEIGHT_UNIT_TROY_OUNCE = 0x5A;
    
    constexpr uint8_t WEIGHT_UNIT_OUNCE = 0x5B;
    
    constexpr uint8_t WEIGHT_UNIT_POUND = 0x5C;
    
    constexpr uint8_t SCALE_STATUS_FAULT = 0x71;
    
    constexpr uint8_t SCALE_STATUS_STABLE_AT_CENTER_OF_ZERO = 0x72;
    
    constexpr uint8_t SCALE_STATUS_IN_MOTION = 0x73;
    
    constexpr uint8_t SCALE_STATUS_WEIGHT_STABLE = 0x74;
    
    constexpr uint8_t SCALE_STATUS_UNDER_ZERO = 0x75;
    
    constexpr uint8_t SCALE_STATUS_OVER_WEIGHT_LIMIT = 0x76;
    
    constexpr uint8_t SCALE_STATUS_REQUIRES_CALIBRATION = 0x77;
    
    constexpr uint8_t SCALE_STATUS_REQUIRES_REZEROING = 0x78;

}
