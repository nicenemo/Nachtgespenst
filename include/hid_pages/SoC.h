#pragma once
#include "../HID_Types.h"

namespace HID::SoC {
    constexpr uint8_t PAGE_ID = 0x0011;
    namespace OTHER {
        constexpr Usage FILEPAYLOADCONTAINSLASTBYTES = { 0x08, UsageKind::OTHER, "FILEPAYLOADCONTAINSLASTBYTES" };
        constexpr Usage FILETRANSFERSTOP = { 0x09, UsageKind::OTHER, "FILETRANSFERSTOP" };
        constexpr Usage FILETRANSFERTILLEND = { 0x0A, UsageKind::OTHER, "FILETRANSFERTILLEND" };
    }
    namespace DV {
        constexpr Usage FIRMWAREFILEID = { 0x03, UsageKind::DV, "FIRMWAREFILEID" };
        constexpr Usage FILEOFFSETINBYTES = { 0x04, UsageKind::DV, "FILEOFFSETINBYTES" };
        constexpr Usage FILETRANSFERSIZEMAXINBYTES = { 0x05, UsageKind::DV, "FILETRANSFERSIZEMAXINBYTES" };
        constexpr Usage FILEPAYLOAD = { 0x06, UsageKind::DV, "FILEPAYLOAD" };
        constexpr Usage FILEPAYLOADSIZEINBYTES = { 0x07, UsageKind::DV, "FILEPAYLOADSIZEINBYTES" };
    }
    namespace CL {
        constexpr Usage FIRMWARETRANSFER = { 0x02, UsageKind::CL, "FIRMWARETRANSFER" };
    }
    namespace CA {
        constexpr Usage SOCCONTROL = { 0x01, UsageKind::CA, "SOCCONTROL" };
    }
}
