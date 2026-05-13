#pragma once
#include "../HID_Types.h"

namespace HID::GameControls {
    constexpr uint8_t PAGE_ID = 0x0005;
    namespace OTHER {
        constexpr Usage 3D_GAME_CONTROLLER = { 0x01, UsageKind::OTHER, "3D_GAME_CONTROLLER" };
        constexpr Usage PINBALL_DEVICE = { 0x02, UsageKind::OTHER, "PINBALL_DEVICE" };
        constexpr Usage GUN_DEVICE = { 0x03, UsageKind::OTHER, "GUN_DEVICE" };
        constexpr Usage POINT_OF_VIEW = { 0x20, UsageKind::OTHER, "POINT_OF_VIEW" };
        constexpr Usage TURN_RIGHT_LEFT = { 0x21, UsageKind::OTHER, "TURN_RIGHT_LEFT" };
        constexpr Usage PITCH_FORWARD_BACKWARD = { 0x22, UsageKind::OTHER, "PITCH_FORWARD_BACKWARD" };
        constexpr Usage ROLL_RIGHT_LEFT = { 0x23, UsageKind::OTHER, "ROLL_RIGHT_LEFT" };
        constexpr Usage MOVE_RIGHT_LEFT = { 0x24, UsageKind::OTHER, "MOVE_RIGHT_LEFT" };
        constexpr Usage MOVE_FORWARD_BACKWARD = { 0x25, UsageKind::OTHER, "MOVE_FORWARD_BACKWARD" };
        constexpr Usage MOVE_UP_DOWN = { 0x26, UsageKind::OTHER, "MOVE_UP_DOWN" };
        constexpr Usage LEAN_RIGHT_LEFT = { 0x27, UsageKind::OTHER, "LEAN_RIGHT_LEFT" };
        constexpr Usage LEAN_FORWARD_BACKWARD = { 0x28, UsageKind::OTHER, "LEAN_FORWARD_BACKWARD" };
        constexpr Usage HEIGHT_OF_POV = { 0x29, UsageKind::OTHER, "HEIGHT_OF_POV" };
        constexpr Usage FLIPPER = { 0x2A, UsageKind::OTHER, "FLIPPER" };
        constexpr Usage SECONDARY_FLIPPER = { 0x2B, UsageKind::OTHER, "SECONDARY_FLIPPER" };
        constexpr Usage BUMP = { 0x2C, UsageKind::OTHER, "BUMP" };
        constexpr Usage NEW_GAME = { 0x2D, UsageKind::OTHER, "NEW_GAME" };
        constexpr Usage SHOOT_BALL = { 0x2E, UsageKind::OTHER, "SHOOT_BALL" };
        constexpr Usage PLAYER = { 0x2F, UsageKind::OTHER, "PLAYER" };
        constexpr Usage GUN_BOLT = { 0x30, UsageKind::OTHER, "GUN_BOLT" };
        constexpr Usage GUN_CLIP = { 0x31, UsageKind::OTHER, "GUN_CLIP" };
        constexpr Usage GUN_SELECTOR = { 0x32, UsageKind::OTHER, "GUN_SELECTOR" };
        constexpr Usage GUN_SINGLE_SHOT = { 0x33, UsageKind::OTHER, "GUN_SINGLE_SHOT" };
        constexpr Usage GUN_BURST = { 0x34, UsageKind::OTHER, "GUN_BURST" };
        constexpr Usage GUN_AUTOMATIC = { 0x35, UsageKind::OTHER, "GUN_AUTOMATIC" };
        constexpr Usage GUN_SAFETY = { 0x36, UsageKind::OTHER, "GUN_SAFETY" };
        constexpr Usage GAMEPAD_FIRE_JUMP = { 0x37, UsageKind::OTHER, "GAMEPAD_FIRE_JUMP" };
        constexpr Usage GAMEPAD_TRIGGER = { 0x39, UsageKind::OTHER, "GAMEPAD_TRIGGER" };
        constexpr Usage FORM_FITTING_GAMEPAD = { 0x3A, UsageKind::OTHER, "FORM_FITTING_GAMEPAD" };
    }
}
