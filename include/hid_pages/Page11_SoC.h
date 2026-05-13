#pragma once
#include <stdint.h>

/**
 * HID Usage Page: SoC (0x11)
 */
namespace HID::SoC {
    constexpr uint8_t PAGE_ID = 0x11;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t SOCCONTROL = 0x01;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint8_t FIRMWARETRANSFER = 0x02;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint8_t FILEPAYLOADCONTAINSLASTBYTES = 0x08;
    
    constexpr uint8_t FILETRANSFERSTOP = 0x09;
    
    constexpr uint8_t FILETRANSFERTILLEND = 0x0A;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t FIRMWAREFILEID = 0x03;
    
    constexpr uint8_t FILEOFFSETINBYTES = 0x04;
    
    constexpr uint8_t FILETRANSFERSIZEMAXINBYTES = 0x05;
    
    constexpr uint8_t FILEPAYLOAD = 0x06;
    
    constexpr uint8_t FILEPAYLOADSIZEINBYTES = 0x07;

}
