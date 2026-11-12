#pragma once
#include "../HID_Types.h"

namespace HID::SoC {
    constexpr uint16_t PAGE_ID = 0x0011;
    namespace OTHER {
        constexpr Usage SOCCONTROL = { 0x01, UsageKind::OTHER, "SOCCONTROL" };
        constexpr Usage FIRMWARETRANSFER = { 0x02, UsageKind::OTHER, "FIRMWARETRANSFER" };
        constexpr Usage FIRMWAREFILEID = { 0x03, UsageKind::OTHER, "FIRMWAREFILEID" };
        constexpr Usage FILEOFFSETINBYTES = { 0x04, UsageKind::OTHER, "FILEOFFSETINBYTES" };
        constexpr Usage FILETRANSFERSIZEMAXINBYTES = { 0x05, UsageKind::OTHER, "FILETRANSFERSIZEMAXINBYTES" };
        constexpr Usage FILEPAYLOAD = { 0x06, UsageKind::OTHER, "FILEPAYLOAD" };
        constexpr Usage FILEPAYLOADSIZEINBYTES = { 0x07, UsageKind::OTHER, "FILEPAYLOADSIZEINBYTES" };
        constexpr Usage FILEPAYLOADCONTAINSLASTBYTES = { 0x08, UsageKind::OTHER, "FILEPAYLOADCONTAINSLASTBYTES" };
        constexpr Usage FILETRANSFERSTOP = { 0x09, UsageKind::OTHER, "FILETRANSFERSTOP" };
        constexpr Usage FILETRANSFERTILLEND = { 0x0A, UsageKind::OTHER, "FILETRANSFERTILLEND" };
    }
}
