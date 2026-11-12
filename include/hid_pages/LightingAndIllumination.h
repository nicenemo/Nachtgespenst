#pragma once
#include "../HID_Types.h"

namespace HID::LightingAndIllumination {
    constexpr uint16_t PAGE_ID = 0x0059;
    namespace OTHER {
        constexpr Usage LAMPARRAY = { 0x01, UsageKind::OTHER, "LAMPARRAY" };
        constexpr Usage LAMPARRAYATTRIBUTESREPORT = { 0x02, UsageKind::OTHER, "LAMPARRAYATTRIBUTESREPORT" };
        constexpr Usage LAMPCOUNT = { 0x03, UsageKind::OTHER, "LAMPCOUNT" };
        constexpr Usage BOUNDINGBOXWIDTHINMICROMETERS = { 0x04, UsageKind::OTHER, "BOUNDINGBOXWIDTHINMICROMETERS" };
        constexpr Usage BOUNDINGBOXHEIGHTINMICROMETERS = { 0x05, UsageKind::OTHER, "BOUNDINGBOXHEIGHTINMICROMETERS" };
        constexpr Usage BOUNDINGBOXDEPTHINMICROMETERS = { 0x06, UsageKind::OTHER, "BOUNDINGBOXDEPTHINMICROMETERS" };
        constexpr Usage LAMPARRAYKIND = { 0x07, UsageKind::OTHER, "LAMPARRAYKIND" };
        constexpr Usage MINUPDATEINTERVALINMICROSECONDS = { 0x08, UsageKind::OTHER, "MINUPDATEINTERVALINMICROSECONDS" };
        constexpr Usage LAMPATTRIBUTESREQUESTREPORT = { 0x20, UsageKind::OTHER, "LAMPATTRIBUTESREQUESTREPORT" };
        constexpr Usage LAMPID = { 0x21, UsageKind::OTHER, "LAMPID" };
        constexpr Usage LAMPATTRIBUTESRESPONSEREPORT = { 0x22, UsageKind::OTHER, "LAMPATTRIBUTESRESPONSEREPORT" };
        constexpr Usage POSITIONXINMICROMETERS = { 0x23, UsageKind::OTHER, "POSITIONXINMICROMETERS" };
        constexpr Usage POSITIONYINMICROMETERS = { 0x24, UsageKind::OTHER, "POSITIONYINMICROMETERS" };
        constexpr Usage POSITIONZINMICROMETERS = { 0x25, UsageKind::OTHER, "POSITIONZINMICROMETERS" };
        constexpr Usage LAMPPURPOSES = { 0x26, UsageKind::OTHER, "LAMPPURPOSES" };
        constexpr Usage UPDATELATENCYINMICROSECONDS = { 0x27, UsageKind::OTHER, "UPDATELATENCYINMICROSECONDS" };
        constexpr Usage REDLEVELCOUNT = { 0x28, UsageKind::OTHER, "REDLEVELCOUNT" };
        constexpr Usage GREENLEVELCOUNT = { 0x29, UsageKind::OTHER, "GREENLEVELCOUNT" };
        constexpr Usage BLUELEVELCOUNT = { 0x2A, UsageKind::OTHER, "BLUELEVELCOUNT" };
        constexpr Usage INTENSITYLEVELCOUNT = { 0x2B, UsageKind::OTHER, "INTENSITYLEVELCOUNT" };
        constexpr Usage ISPROGRAMMABLE = { 0x2C, UsageKind::OTHER, "ISPROGRAMMABLE" };
        constexpr Usage INPUTBINDING = { 0x2D, UsageKind::OTHER, "INPUTBINDING" };
        constexpr Usage LAMPMULTIUPDATEREPORT = { 0x50, UsageKind::OTHER, "LAMPMULTIUPDATEREPORT" };
        constexpr Usage REDUPDATECHANNEL = { 0x51, UsageKind::OTHER, "REDUPDATECHANNEL" };
        constexpr Usage GREENUPDATECHANNEL = { 0x52, UsageKind::OTHER, "GREENUPDATECHANNEL" };
        constexpr Usage BLUEUPDATECHANNEL = { 0x53, UsageKind::OTHER, "BLUEUPDATECHANNEL" };
        constexpr Usage INTENSITYUPDATECHANNEL = { 0x54, UsageKind::OTHER, "INTENSITYUPDATECHANNEL" };
        constexpr Usage LAMPUPDATEFLAGS = { 0x55, UsageKind::OTHER, "LAMPUPDATEFLAGS" };
        constexpr Usage LAMPRANGEUPDATEREPORT = { 0x60, UsageKind::OTHER, "LAMPRANGEUPDATEREPORT" };
        constexpr Usage LAMPIDSTART = { 0x61, UsageKind::OTHER, "LAMPIDSTART" };
        constexpr Usage LAMPIDEND = { 0x62, UsageKind::OTHER, "LAMPIDEND" };
        constexpr Usage LAMPARRAYCONTROLREPORT = { 0x70, UsageKind::OTHER, "LAMPARRAYCONTROLREPORT" };
        constexpr Usage AUTONOMOUSMODE = { 0x71, UsageKind::OTHER, "AUTONOMOUSMODE" };
    }
}
