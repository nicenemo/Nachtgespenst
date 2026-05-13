#pragma once
#include "../HID_Types.h"

namespace HID::SportControls {
    constexpr uint8_t PAGE_ID = 0x0004;
    namespace OTHER {
        constexpr Usage STICK_TYPE = { 0x38, UsageKind::OTHER, "STICK_TYPE" };
    }
    namespace DV {
        constexpr Usage OAR = { 0x30, UsageKind::DV, "OAR" };
        constexpr Usage SLOPE = { 0x31, UsageKind::DV, "SLOPE" };
        constexpr Usage RATE = { 0x32, UsageKind::DV, "RATE" };
        constexpr Usage STICK_SPEED = { 0x33, UsageKind::DV, "STICK_SPEED" };
        constexpr Usage STICK_FACE_ANGLE = { 0x34, UsageKind::DV, "STICK_FACE_ANGLE" };
        constexpr Usage STICK_HEEL_TOE = { 0x35, UsageKind::DV, "STICK_HEEL_TOE" };
        constexpr Usage STICK_FOLLOW_THROUGH = { 0x36, UsageKind::DV, "STICK_FOLLOW_THROUGH" };
        constexpr Usage STICK_TEMPO = { 0x37, UsageKind::DV, "STICK_TEMPO" };
        constexpr Usage STICK_HEIGHT = { 0x39, UsageKind::DV, "STICK_HEIGHT" };
    }
    namespace Sel {
        constexpr Usage PUTTER = { 0x50, UsageKind::Sel, "PUTTER" };
        constexpr Usage 1_IRON = { 0x51, UsageKind::Sel, "1_IRON" };
        constexpr Usage 2_IRON = { 0x52, UsageKind::Sel, "2_IRON" };
        constexpr Usage 3_IRON = { 0x53, UsageKind::Sel, "3_IRON" };
        constexpr Usage 4_IRON = { 0x54, UsageKind::Sel, "4_IRON" };
        constexpr Usage 5_IRON = { 0x55, UsageKind::Sel, "5_IRON" };
        constexpr Usage 6_IRON = { 0x56, UsageKind::Sel, "6_IRON" };
        constexpr Usage 7_IRON = { 0x57, UsageKind::Sel, "7_IRON" };
        constexpr Usage 8_IRON = { 0x58, UsageKind::Sel, "8_IRON" };
        constexpr Usage 9_IRON = { 0x59, UsageKind::Sel, "9_IRON" };
        constexpr Usage 10_IRON = { 0x5A, UsageKind::Sel, "10_IRON" };
        constexpr Usage 11_IRON = { 0x5B, UsageKind::Sel, "11_IRON" };
        constexpr Usage SAND_WEDGE = { 0x5C, UsageKind::Sel, "SAND_WEDGE" };
        constexpr Usage LOFT_WEDGE = { 0x5D, UsageKind::Sel, "LOFT_WEDGE" };
        constexpr Usage POWER_WEDGE = { 0x5E, UsageKind::Sel, "POWER_WEDGE" };
        constexpr Usage 1_WOOD = { 0x5F, UsageKind::Sel, "1_WOOD" };
        constexpr Usage 3_WOOD = { 0x60, UsageKind::Sel, "3_WOOD" };
        constexpr Usage 5_WOOD = { 0x61, UsageKind::Sel, "5_WOOD" };
        constexpr Usage 7_WOOD = { 0x62, UsageKind::Sel, "7_WOOD" };
        constexpr Usage 9_WOOD = { 0x63, UsageKind::Sel, "9_WOOD" };
    }
    namespace CA {
        constexpr Usage BASEBALL_BAT = { 0x01, UsageKind::CA, "BASEBALL_BAT" };
        constexpr Usage GOLF_CLUB = { 0x02, UsageKind::CA, "GOLF_CLUB" };
        constexpr Usage ROWING_MACHINE = { 0x03, UsageKind::CA, "ROWING_MACHINE" };
        constexpr Usage TREADMILL = { 0x04, UsageKind::CA, "TREADMILL" };
    }
}
