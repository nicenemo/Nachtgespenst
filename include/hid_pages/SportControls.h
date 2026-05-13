#pragma once
#include "../HID_Types.h"

namespace HID::SportControls {
    constexpr uint8_t PAGE_ID = 0x0004;
    namespace OTHER {
        constexpr Usage BASEBALL_BAT = { 0x01, UsageKind::OTHER, "BASEBALL_BAT" };
        constexpr Usage GOLF_CLUB = { 0x02, UsageKind::OTHER, "GOLF_CLUB" };
        constexpr Usage ROWING_MACHINE = { 0x03, UsageKind::OTHER, "ROWING_MACHINE" };
        constexpr Usage TREADMILL = { 0x04, UsageKind::OTHER, "TREADMILL" };
        constexpr Usage OAR = { 0x30, UsageKind::OTHER, "OAR" };
        constexpr Usage SLOPE = { 0x31, UsageKind::OTHER, "SLOPE" };
        constexpr Usage RATE = { 0x32, UsageKind::OTHER, "RATE" };
        constexpr Usage STICK_SPEED = { 0x33, UsageKind::OTHER, "STICK_SPEED" };
        constexpr Usage STICK_FACE_ANGLE = { 0x34, UsageKind::OTHER, "STICK_FACE_ANGLE" };
        constexpr Usage STICK_HEEL_TOE = { 0x35, UsageKind::OTHER, "STICK_HEEL_TOE" };
        constexpr Usage STICK_FOLLOW_THROUGH = { 0x36, UsageKind::OTHER, "STICK_FOLLOW_THROUGH" };
        constexpr Usage STICK_TEMPO = { 0x37, UsageKind::OTHER, "STICK_TEMPO" };
        constexpr Usage STICK_TYPE = { 0x38, UsageKind::OTHER, "STICK_TYPE" };
        constexpr Usage STICK_HEIGHT = { 0x39, UsageKind::OTHER, "STICK_HEIGHT" };
        constexpr Usage PUTTER = { 0x50, UsageKind::OTHER, "PUTTER" };
        constexpr Usage 1_IRON = { 0x51, UsageKind::OTHER, "1_IRON" };
        constexpr Usage 2_IRON = { 0x52, UsageKind::OTHER, "2_IRON" };
        constexpr Usage 3_IRON = { 0x53, UsageKind::OTHER, "3_IRON" };
        constexpr Usage 4_IRON = { 0x54, UsageKind::OTHER, "4_IRON" };
        constexpr Usage 5_IRON = { 0x55, UsageKind::OTHER, "5_IRON" };
        constexpr Usage 6_IRON = { 0x56, UsageKind::OTHER, "6_IRON" };
        constexpr Usage 7_IRON = { 0x57, UsageKind::OTHER, "7_IRON" };
        constexpr Usage 8_IRON = { 0x58, UsageKind::OTHER, "8_IRON" };
        constexpr Usage 9_IRON = { 0x59, UsageKind::OTHER, "9_IRON" };
        constexpr Usage 10_IRON = { 0x5A, UsageKind::OTHER, "10_IRON" };
        constexpr Usage 11_IRON = { 0x5B, UsageKind::OTHER, "11_IRON" };
        constexpr Usage SAND_WEDGE = { 0x5C, UsageKind::OTHER, "SAND_WEDGE" };
        constexpr Usage LOFT_WEDGE = { 0x5D, UsageKind::OTHER, "LOFT_WEDGE" };
        constexpr Usage POWER_WEDGE = { 0x5E, UsageKind::OTHER, "POWER_WEDGE" };
        constexpr Usage 1_WOOD = { 0x5F, UsageKind::OTHER, "1_WOOD" };
        constexpr Usage 3_WOOD = { 0x60, UsageKind::OTHER, "3_WOOD" };
        constexpr Usage 5_WOOD = { 0x61, UsageKind::OTHER, "5_WOOD" };
        constexpr Usage 7_WOOD = { 0x62, UsageKind::OTHER, "7_WOOD" };
        constexpr Usage 9_WOOD = { 0x63, UsageKind::OTHER, "9_WOOD" };
    }
}
