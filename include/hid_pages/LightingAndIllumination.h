#pragma once
#include "../HID_Types.h"

namespace HID::LightingAndIllumination {
    constexpr uint8_t PAGE_ID = 0x0059;
    namespace OTHER {
        constexpr Usage LAMPCOUNT = { 0x03, UsageKind::OTHER, "LAMPCOUNT" };
        constexpr Usage BOUNDINGBOXWIDTHINMICROMETERS = { 0x04, UsageKind::OTHER, "BOUNDINGBOXWIDTHINMICROMETERS" };
        constexpr Usage BOUNDINGBOXHEIGHTINMICROMETERS = { 0x05, UsageKind::OTHER, "BOUNDINGBOXHEIGHTINMICROMETERS" };
        constexpr Usage BOUNDINGBOXDEPTHINMICROMETERS = { 0x06, UsageKind::OTHER, "BOUNDINGBOXDEPTHINMICROMETERS" };
        constexpr Usage LAMPARRAYKIND = { 0x07, UsageKind::OTHER, "LAMPARRAYKIND" };
        constexpr Usage MINUPDATEINTERVALINMICROSECONDS = { 0x08, UsageKind::OTHER, "MINUPDATEINTERVALINMICROSECONDS" };
        constexpr Usage LAMPID = { 0x21, UsageKind::OTHER, "LAMPID" };
    }
    namespace DV {
        constexpr Usage POSITIONXINMICROMETERS = { 0x23, UsageKind::DV, "POSITIONXINMICROMETERS" };
        constexpr Usage POSITIONYINMICROMETERS = { 0x24, UsageKind::DV, "POSITIONYINMICROMETERS" };
        constexpr Usage POSITIONZINMICROMETERS = { 0x25, UsageKind::DV, "POSITIONZINMICROMETERS" };
        constexpr Usage LAMPPURPOSES = { 0x26, UsageKind::DV, "LAMPPURPOSES" };
        constexpr Usage UPDATELATENCYINMICROSECONDS = { 0x27, UsageKind::DV, "UPDATELATENCYINMICROSECONDS" };
        constexpr Usage REDLEVELCOUNT = { 0x28, UsageKind::DV, "REDLEVELCOUNT" };
        constexpr Usage GREENLEVELCOUNT = { 0x29, UsageKind::DV, "GREENLEVELCOUNT" };
        constexpr Usage BLUELEVELCOUNT = { 0x2A, UsageKind::DV, "BLUELEVELCOUNT" };
        constexpr Usage INTENSITYLEVELCOUNT = { 0x2B, UsageKind::DV, "INTENSITYLEVELCOUNT" };
        constexpr Usage ISPROGRAMMABLE = { 0x2C, UsageKind::DV, "ISPROGRAMMABLE" };
        constexpr Usage INPUTBINDING = { 0x2D, UsageKind::DV, "INPUTBINDING" };
        constexpr Usage REDUPDATECHANNEL = { 0x51, UsageKind::DV, "REDUPDATECHANNEL" };
        constexpr Usage GREENUPDATECHANNEL = { 0x52, UsageKind::DV, "GREENUPDATECHANNEL" };
        constexpr Usage BLUEUPDATECHANNEL = { 0x53, UsageKind::DV, "BLUEUPDATECHANNEL" };
        constexpr Usage INTENSITYUPDATECHANNEL = { 0x54, UsageKind::DV, "INTENSITYUPDATECHANNEL" };
        constexpr Usage LAMPUPDATEFLAGS = { 0x55, UsageKind::DV, "LAMPUPDATEFLAGS" };
        constexpr Usage LAMPIDSTART = { 0x61, UsageKind::DV, "LAMPIDSTART" };
        constexpr Usage LAMPIDEND = { 0x62, UsageKind::DV, "LAMPIDEND" };
        constexpr Usage AUTONOMOUSMODE = { 0x71, UsageKind::DV, "AUTONOMOUSMODE" };
    }
    namespace CL {
        constexpr Usage LAMPARRAYATTRIBUTESREPORT = { 0x02, UsageKind::CL, "LAMPARRAYATTRIBUTESREPORT" };
        constexpr Usage LAMPATTRIBUTESREQUESTREPORT = { 0x20, UsageKind::CL, "LAMPATTRIBUTESREQUESTREPORT" };
        constexpr Usage LAMPATTRIBUTESRESPONSEREPORT = { 0x22, UsageKind::CL, "LAMPATTRIBUTESRESPONSEREPORT" };
        constexpr Usage LAMPMULTIUPDATEREPORT = { 0x50, UsageKind::CL, "LAMPMULTIUPDATEREPORT" };
        constexpr Usage LAMPRANGEUPDATEREPORT = { 0x60, UsageKind::CL, "LAMPRANGEUPDATEREPORT" };
        constexpr Usage LAMPARRAYCONTROLREPORT = { 0x70, UsageKind::CL, "LAMPARRAYCONTROLREPORT" };
    }
    namespace CA {
        constexpr Usage LAMPARRAY = { 0x01, UsageKind::CA, "LAMPARRAY" };
    }
}
