#pragma once
#include "../HID_Types.h"

namespace HID::SportControls {
    constexpr uint16_t PAGE_ID = 0x0004;
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
        constexpr Usage ID_1_IRON = { 0x51, UsageKind::OTHER, "ID_1_IRON" };
        constexpr Usage ID_2_IRON = { 0x52, UsageKind::OTHER, "ID_2_IRON" };
        constexpr Usage ID_3_IRON = { 0x53, UsageKind::OTHER, "ID_3_IRON" };
        constexpr Usage ID_4_IRON = { 0x54, UsageKind::OTHER, "ID_4_IRON" };
        constexpr Usage ID_5_IRON = { 0x55, UsageKind::OTHER, "ID_5_IRON" };
        constexpr Usage ID_6_IRON = { 0x56, UsageKind::OTHER, "ID_6_IRON" };
        constexpr Usage ID_7_IRON = { 0x57, UsageKind::OTHER, "ID_7_IRON" };
        constexpr Usage ID_8_IRON = { 0x58, UsageKind::OTHER, "ID_8_IRON" };
        constexpr Usage ID_9_IRON = { 0x59, UsageKind::OTHER, "ID_9_IRON" };
        constexpr Usage ID_10_IRON = { 0x5A, UsageKind::OTHER, "ID_10_IRON" };
        constexpr Usage ID_11_IRON = { 0x5B, UsageKind::OTHER, "ID_11_IRON" };
        constexpr Usage SAND_WEDGE = { 0x5C, UsageKind::OTHER, "SAND_WEDGE" };
        constexpr Usage LOFT_WEDGE = { 0x5D, UsageKind::OTHER, "LOFT_WEDGE" };
        constexpr Usage POWER_WEDGE = { 0x5E, UsageKind::OTHER, "POWER_WEDGE" };
        constexpr Usage ID_1_WOOD = { 0x5F, UsageKind::OTHER, "ID_1_WOOD" };
        constexpr Usage ID_3_WOOD = { 0x60, UsageKind::OTHER, "ID_3_WOOD" };
        constexpr Usage ID_5_WOOD = { 0x61, UsageKind::OTHER, "ID_5_WOOD" };
        constexpr Usage ID_7_WOOD = { 0x62, UsageKind::OTHER, "ID_7_WOOD" };
        constexpr Usage ID_9_WOOD = { 0x63, UsageKind::OTHER, "ID_9_WOOD" };
    }
}
