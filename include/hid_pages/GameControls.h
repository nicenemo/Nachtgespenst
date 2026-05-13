#pragma once
#include "../HID_Types.h"

namespace HID::GameControls {
    constexpr uint8_t PAGE_ID = 0x0005;
    namespace OTHER {
        constexpr Usage GUN_SELECTOR = { 0x32, UsageKind::OTHER, "GUN_SELECTOR" };
        constexpr Usage FORM_FITTING_GAMEPAD = { 0x3A, UsageKind::OTHER, "FORM_FITTING_GAMEPAD" };
    }
    namespace OOC {
        constexpr Usage GUN_BOLT = { 0x30, UsageKind::OOC, "GUN_BOLT" };
        constexpr Usage GUN_CLIP = { 0x31, UsageKind::OOC, "GUN_CLIP" };
        constexpr Usage GUN_SAFETY = { 0x36, UsageKind::OOC, "GUN_SAFETY" };
    }
    namespace DV {
        constexpr Usage TURN_RIGHT_LEFT = { 0x21, UsageKind::DV, "TURN_RIGHT_LEFT" };
        constexpr Usage PITCH_FORWARD_BACKWARD = { 0x22, UsageKind::DV, "PITCH_FORWARD_BACKWARD" };
        constexpr Usage ROLL_RIGHT_LEFT = { 0x23, UsageKind::DV, "ROLL_RIGHT_LEFT" };
        constexpr Usage MOVE_RIGHT_LEFT = { 0x24, UsageKind::DV, "MOVE_RIGHT_LEFT" };
        constexpr Usage MOVE_FORWARD_BACKWARD = { 0x25, UsageKind::DV, "MOVE_FORWARD_BACKWARD" };
        constexpr Usage MOVE_UP_DOWN = { 0x26, UsageKind::DV, "MOVE_UP_DOWN" };
        constexpr Usage LEAN_RIGHT_LEFT = { 0x27, UsageKind::DV, "LEAN_RIGHT_LEFT" };
        constexpr Usage LEAN_FORWARD_BACKWARD = { 0x28, UsageKind::DV, "LEAN_FORWARD_BACKWARD" };
        constexpr Usage HEIGHT_OF_POV = { 0x29, UsageKind::DV, "HEIGHT_OF_POV" };
    }
    namespace MC {
        constexpr Usage FLIPPER = { 0x2A, UsageKind::MC, "FLIPPER" };
        constexpr Usage SECONDARY_FLIPPER = { 0x2B, UsageKind::MC, "SECONDARY_FLIPPER" };
        constexpr Usage BUMP = { 0x2C, UsageKind::MC, "BUMP" };
    }
    namespace CL {
        constexpr Usage GAMEPAD_FIRE_JUMP = { 0x37, UsageKind::CL, "GAMEPAD_FIRE_JUMP" };
        constexpr Usage GAMEPAD_TRIGGER = { 0x39, UsageKind::CL, "GAMEPAD_TRIGGER" };
    }
    namespace Sel {
        constexpr Usage GUN_SINGLE_SHOT = { 0x33, UsageKind::Sel, "GUN_SINGLE_SHOT" };
        constexpr Usage GUN_BURST = { 0x34, UsageKind::Sel, "GUN_BURST" };
        constexpr Usage GUN_AUTOMATIC = { 0x35, UsageKind::Sel, "GUN_AUTOMATIC" };
    }
    namespace CP {
        constexpr Usage POINT_OF_VIEW = { 0x20, UsageKind::CP, "POINT_OF_VIEW" };
    }
    namespace OSC {
        constexpr Usage NEW_GAME = { 0x2D, UsageKind::OSC, "NEW_GAME" };
        constexpr Usage SHOOT_BALL = { 0x2E, UsageKind::OSC, "SHOOT_BALL" };
        constexpr Usage PLAYER = { 0x2F, UsageKind::OSC, "PLAYER" };
    }
    namespace CA {
        constexpr Usage 3D_GAME_CONTROLLER = { 0x01, UsageKind::CA, "3D_GAME_CONTROLLER" };
        constexpr Usage PINBALL_DEVICE = { 0x02, UsageKind::CA, "PINBALL_DEVICE" };
        constexpr Usage GUN_DEVICE = { 0x03, UsageKind::CA, "GUN_DEVICE" };
    }
}
