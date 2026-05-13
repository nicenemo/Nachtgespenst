#pragma once
#include "../HID_Types.h"

namespace HID::VESAVirtualControls {
    constexpr uint8_t PAGE_ID = 0x0082;
    namespace OTHER {
        constexpr Usage INPUT_LEVEL_SELECT = { 0x5E, UsageKind::OTHER, "INPUT_LEVEL_SELECT" };
        constexpr Usage INPUT_SOURCE_SELECT = { 0x60, UsageKind::OTHER, "INPUT_SOURCE_SELECT" };
        constexpr Usage AUTO_SIZE_CENTER = { 0xA2, UsageKind::OTHER, "AUTO_SIZE_CENTER" };
        constexpr Usage POLARITY_HORIZONTAL_SYNCHRONIZATION = { 0xA4, UsageKind::OTHER, "POLARITY_HORIZONTAL_SYNCHRONIZATION" };
        constexpr Usage POLARITY_VERTICAL_SYNCHRONIZATION = { 0xA6, UsageKind::OTHER, "POLARITY_VERTICAL_SYNCHRONIZATION" };
        constexpr Usage SYNCHRONIZATION_TYPE = { 0xA8, UsageKind::OTHER, "SYNCHRONIZATION_TYPE" };
        constexpr Usage SCREEN_ORIENTATION = { 0xAA, UsageKind::OTHER, "SCREEN_ORIENTATION" };
        constexpr Usage SETTINGS = { 0xB0, UsageKind::OTHER, "SETTINGS" };
        constexpr Usage ON_SCREEN_DISPLAY = { 0xCA, UsageKind::OTHER, "ON_SCREEN_DISPLAY" };
        constexpr Usage STEREO_MODE = { 0xD4, UsageKind::OTHER, "STEREO_MODE" };
    }
    namespace DV {
        constexpr Usage DEGAUSS = { 0x01, UsageKind::DV, "DEGAUSS" };
        constexpr Usage BRIGHTNESS = { 0x10, UsageKind::DV, "BRIGHTNESS" };
        constexpr Usage CONTRAST = { 0x12, UsageKind::DV, "CONTRAST" };
        constexpr Usage RED_VIDEO_GAIN = { 0x16, UsageKind::DV, "RED_VIDEO_GAIN" };
        constexpr Usage GREEN_VIDEO_GAIN = { 0x18, UsageKind::DV, "GREEN_VIDEO_GAIN" };
        constexpr Usage BLUE_VIDEO_GAIN = { 0x1A, UsageKind::DV, "BLUE_VIDEO_GAIN" };
        constexpr Usage FOCUS = { 0x1C, UsageKind::DV, "FOCUS" };
        constexpr Usage HORIZONTAL_POSITION = { 0x20, UsageKind::DV, "HORIZONTAL_POSITION" };
        constexpr Usage HORIZONTAL_SIZE = { 0x22, UsageKind::DV, "HORIZONTAL_SIZE" };
        constexpr Usage HORIZONTAL_PINCUSHION = { 0x24, UsageKind::DV, "HORIZONTAL_PINCUSHION" };
        constexpr Usage HORIZONTAL_PINCUSHION_BALANCE = { 0x26, UsageKind::DV, "HORIZONTAL_PINCUSHION_BALANCE" };
        constexpr Usage HORIZONTAL_MISCONVERGENCE = { 0x28, UsageKind::DV, "HORIZONTAL_MISCONVERGENCE" };
        constexpr Usage HORIZONTAL_LINEARITY = { 0x2A, UsageKind::DV, "HORIZONTAL_LINEARITY" };
        constexpr Usage HORIZONTAL_LINEARITY_BALANCE = { 0x2C, UsageKind::DV, "HORIZONTAL_LINEARITY_BALANCE" };
        constexpr Usage VERTICAL_POSITION = { 0x30, UsageKind::DV, "VERTICAL_POSITION" };
        constexpr Usage VERTICAL_SIZE = { 0x32, UsageKind::DV, "VERTICAL_SIZE" };
        constexpr Usage VERTICAL_PINCUSHION = { 0x34, UsageKind::DV, "VERTICAL_PINCUSHION" };
        constexpr Usage VERTICAL_PINCUSHION_BALANCE = { 0x36, UsageKind::DV, "VERTICAL_PINCUSHION_BALANCE" };
        constexpr Usage VERTICAL_MISCONVERGENCE = { 0x38, UsageKind::DV, "VERTICAL_MISCONVERGENCE" };
        constexpr Usage VERTICAL_LINEARITY = { 0x3A, UsageKind::DV, "VERTICAL_LINEARITY" };
        constexpr Usage VERTICAL_LINEARITY_BALANCE = { 0x3C, UsageKind::DV, "VERTICAL_LINEARITY_BALANCE" };
        constexpr Usage PARALLELOGRAM_DISTORTION_KEY_BALANCE = { 0x40, UsageKind::DV, "PARALLELOGRAM_DISTORTION_KEY_BALANCE" };
        constexpr Usage TRAPEZOIDAL_DISTORTION_KEY = { 0x42, UsageKind::DV, "TRAPEZOIDAL_DISTORTION_KEY" };
        constexpr Usage TILT_ROTATION = { 0x44, UsageKind::DV, "TILT_ROTATION" };
        constexpr Usage TOP_CORNER_DISTORTION_CONTROL = { 0x46, UsageKind::DV, "TOP_CORNER_DISTORTION_CONTROL" };
        constexpr Usage TOP_CORNER_DISTORTION_BALANCE = { 0x48, UsageKind::DV, "TOP_CORNER_DISTORTION_BALANCE" };
        constexpr Usage BOTTOM_CORNER_DISTORTION_CONTROL = { 0x4A, UsageKind::DV, "BOTTOM_CORNER_DISTORTION_CONTROL" };
        constexpr Usage BOTTOM_CORNER_DISTORTION_BALANCE = { 0x4C, UsageKind::DV, "BOTTOM_CORNER_DISTORTION_BALANCE" };
        constexpr Usage HORIZONTAL_MOIRÉ = { 0x56, UsageKind::DV, "HORIZONTAL_MOIRÉ" };
        constexpr Usage VERTICAL_MOIRÉ = { 0x58, UsageKind::DV, "VERTICAL_MOIRÉ" };
        constexpr Usage RED_VIDEO_BLACK_LEVEL = { 0x6C, UsageKind::DV, "RED_VIDEO_BLACK_LEVEL" };
        constexpr Usage GREEN_VIDEO_BLACK_LEVEL = { 0x6E, UsageKind::DV, "GREEN_VIDEO_BLACK_LEVEL" };
        constexpr Usage BLUE_VIDEO_BLACK_LEVEL = { 0x70, UsageKind::DV, "BLUE_VIDEO_BLACK_LEVEL" };
        constexpr Usage HORIZONTAL_FREQUENCY = { 0xAC, UsageKind::DV, "HORIZONTAL_FREQUENCY" };
        constexpr Usage VERTICAL_FREQUENCY = { 0xAE, UsageKind::DV, "VERTICAL_FREQUENCY" };
    }
}
