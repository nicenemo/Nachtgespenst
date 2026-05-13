#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Lighting And Illumination (0x59)
 */
namespace HID::LightingAndIllumination {
    constexpr uint8_t PAGE_ID = 0x59;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t LAMPARRAY = 0x01;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t LAMPARRAYATTRIBUTESREPORT = 0x02;
    
    constexpr uint8_t LAMPATTRIBUTESREQUESTREPORT = 0x20;
    
    constexpr uint8_t LAMPATTRIBUTESRESPONSEREPORT = 0x22;
    
    constexpr uint8_t LAMPMULTIUPDATEREPORT = 0x50;
    
    constexpr uint8_t LAMPRANGEUPDATEREPORT = 0x60;
    
    constexpr uint8_t LAMPARRAYCONTROLREPORT = 0x70;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t POSITIONXINMICROMETERS = 0x23;
    
    constexpr uint8_t POSITIONYINMICROMETERS = 0x24;
    
    constexpr uint8_t POSITIONZINMICROMETERS = 0x25;
    
    constexpr uint8_t LAMPPURPOSES = 0x26;
    
    constexpr uint8_t UPDATELATENCYINMICROSECONDS = 0x27;
    
    constexpr uint8_t REDLEVELCOUNT = 0x28;
    
    constexpr uint8_t GREENLEVELCOUNT = 0x29;
    
    constexpr uint8_t BLUELEVELCOUNT = 0x2A;
    
    constexpr uint8_t INTENSITYLEVELCOUNT = 0x2B;
    
    constexpr uint8_t ISPROGRAMMABLE = 0x2C;
    
    constexpr uint8_t INPUTBINDING = 0x2D;
    
    constexpr uint8_t REDUPDATECHANNEL = 0x51;
    
    constexpr uint8_t GREENUPDATECHANNEL = 0x52;
    
    constexpr uint8_t BLUEUPDATECHANNEL = 0x53;
    
    constexpr uint8_t INTENSITYUPDATECHANNEL = 0x54;
    
    constexpr uint8_t LAMPUPDATEFLAGS = 0x55;
    
    constexpr uint8_t LAMPIDSTART = 0x61;
    
    constexpr uint8_t LAMPIDEND = 0x62;
    
    constexpr uint8_t AUTONOMOUSMODE = 0x71;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint8_t LAMPCOUNT = 0x03;
    
    constexpr uint8_t BOUNDINGBOXWIDTHINMICROMETERS = 0x04;
    
    constexpr uint8_t BOUNDINGBOXHEIGHTINMICROMETERS = 0x05;
    
    constexpr uint8_t BOUNDINGBOXDEPTHINMICROMETERS = 0x06;
    
    constexpr uint8_t LAMPARRAYKIND = 0x07;
    
    constexpr uint8_t MINUPDATEINTERVALINMICROSECONDS = 0x08;
    
    constexpr uint8_t LAMPID = 0x21;

}
