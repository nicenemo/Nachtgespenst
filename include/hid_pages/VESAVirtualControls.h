#pragma once
#include "../HID_Types.h"

namespace HID::VESAVirtualControls {
    constexpr uint8_t PAGE_ID = 0x0082;
    namespace OTHER {
        constexpr Usage DEGAUSS = { 0x01, UsageKind::OTHER, "DEGAUSS" };
        constexpr Usage BRIGHTNESS = { 0x10, UsageKind::OTHER, "BRIGHTNESS" };
        constexpr Usage CONTRAST = { 0x12, UsageKind::OTHER, "CONTRAST" };
        constexpr Usage RED_VIDEO_GAIN = { 0x16, UsageKind::OTHER, "RED_VIDEO_GAIN" };
        constexpr Usage GREEN_VIDEO_GAIN = { 0x18, UsageKind::OTHER, "GREEN_VIDEO_GAIN" };
        constexpr Usage BLUE_VIDEO_GAIN = { 0x1A, UsageKind::OTHER, "BLUE_VIDEO_GAIN" };
        constexpr Usage FOCUS = { 0x1C, UsageKind::OTHER, "FOCUS" };
        constexpr Usage HORIZONTAL_POSITION = { 0x20, UsageKind::OTHER, "HORIZONTAL_POSITION" };
        constexpr Usage HORIZONTAL_SIZE = { 0x22, UsageKind::OTHER, "HORIZONTAL_SIZE" };
        constexpr Usage HORIZONTAL_PINCUSHION = { 0x24, UsageKind::OTHER, "HORIZONTAL_PINCUSHION" };
        constexpr Usage HORIZONTAL_PINCUSHION_BALANCE = { 0x26, UsageKind::OTHER, "HORIZONTAL_PINCUSHION_BALANCE" };
        constexpr Usage HORIZONTAL_MISCONVERGENCE = { 0x28, UsageKind::OTHER, "HORIZONTAL_MISCONVERGENCE" };
        constexpr Usage HORIZONTAL_LINEARITY = { 0x2A, UsageKind::OTHER, "HORIZONTAL_LINEARITY" };
        constexpr Usage HORIZONTAL_LINEARITY_BALANCE = { 0x2C, UsageKind::OTHER, "HORIZONTAL_LINEARITY_BALANCE" };
        constexpr Usage VERTICAL_POSITION = { 0x30, UsageKind::OTHER, "VERTICAL_POSITION" };
        constexpr Usage VERTICAL_SIZE = { 0x32, UsageKind::OTHER, "VERTICAL_SIZE" };
        constexpr Usage VERTICAL_PINCUSHION = { 0x34, UsageKind::OTHER, "VERTICAL_PINCUSHION" };
        constexpr Usage VERTICAL_PINCUSHION_BALANCE = { 0x36, UsageKind::OTHER, "VERTICAL_PINCUSHION_BALANCE" };
        constexpr Usage VERTICAL_MISCONVERGENCE = { 0x38, UsageKind::OTHER, "VERTICAL_MISCONVERGENCE" };
        constexpr Usage VERTICAL_LINEARITY = { 0x3A, UsageKind::OTHER, "VERTICAL_LINEARITY" };
        constexpr Usage VERTICAL_LINEARITY_BALANCE = { 0x3C, UsageKind::OTHER, "VERTICAL_LINEARITY_BALANCE" };
        constexpr Usage PARALLELOGRAM_DISTORTION_KEY_BALANCE = { 0x40, UsageKind::OTHER, "PARALLELOGRAM_DISTORTION_KEY_BALANCE" };
        constexpr Usage TRAPEZOIDAL_DISTORTION_KEY = { 0x42, UsageKind::OTHER, "TRAPEZOIDAL_DISTORTION_KEY" };
        constexpr Usage TILT_ROTATION = { 0x44, UsageKind::OTHER, "TILT_ROTATION" };
        constexpr Usage TOP_CORNER_DISTORTION_CONTROL = { 0x46, UsageKind::OTHER, "TOP_CORNER_DISTORTION_CONTROL" };
        constexpr Usage TOP_CORNER_DISTORTION_BALANCE = { 0x48, UsageKind::OTHER, "TOP_CORNER_DISTORTION_BALANCE" };
        constexpr Usage BOTTOM_CORNER_DISTORTION_CONTROL = { 0x4A, UsageKind::OTHER, "BOTTOM_CORNER_DISTORTION_CONTROL" };
        constexpr Usage BOTTOM_CORNER_DISTORTION_BALANCE = { 0x4C, UsageKind::OTHER, "BOTTOM_CORNER_DISTORTION_BALANCE" };
        constexpr Usage HORIZONTAL_MOIRE = { 0x56, UsageKind::OTHER, "HORIZONTAL_MOIRE" };
        constexpr Usage VERTICAL_MOIRE = { 0x58, UsageKind::OTHER, "VERTICAL_MOIRE" };
        constexpr Usage INPUT_LEVEL_SELECT = { 0x5E, UsageKind::OTHER, "INPUT_LEVEL_SELECT" };
        constexpr Usage INPUT_SOURCE_SELECT = { 0x60, UsageKind::OTHER, "INPUT_SOURCE_SELECT" };
        constexpr Usage RED_VIDEO_BLACK_LEVEL = { 0x6C, UsageKind::OTHER, "RED_VIDEO_BLACK_LEVEL" };
        constexpr Usage GREEN_VIDEO_BLACK_LEVEL = { 0x6E, UsageKind::OTHER, "GREEN_VIDEO_BLACK_LEVEL" };
        constexpr Usage BLUE_VIDEO_BLACK_LEVEL = { 0x70, UsageKind::OTHER, "BLUE_VIDEO_BLACK_LEVEL" };
        constexpr Usage AUTO_SIZE_CENTER = { 0xA2, UsageKind::OTHER, "AUTO_SIZE_CENTER" };
        constexpr Usage POLARITY_HORIZONTAL_SYNCHRONIZATION = { 0xA4, UsageKind::OTHER, "POLARITY_HORIZONTAL_SYNCHRONIZATION" };
        constexpr Usage POLARITY_VERTICAL_SYNCHRONIZATION = { 0xA6, UsageKind::OTHER, "POLARITY_VERTICAL_SYNCHRONIZATION" };
        constexpr Usage SYNCHRONIZATION_TYPE = { 0xA8, UsageKind::OTHER, "SYNCHRONIZATION_TYPE" };
        constexpr Usage SCREEN_ORIENTATION = { 0xAA, UsageKind::OTHER, "SCREEN_ORIENTATION" };
        constexpr Usage HORIZONTAL_FREQUENCY = { 0xAC, UsageKind::OTHER, "HORIZONTAL_FREQUENCY" };
        constexpr Usage VERTICAL_FREQUENCY = { 0xAE, UsageKind::OTHER, "VERTICAL_FREQUENCY" };
        constexpr Usage SETTINGS = { 0xB0, UsageKind::OTHER, "SETTINGS" };
        constexpr Usage ON_SCREEN_DISPLAY = { 0xCA, UsageKind::OTHER, "ON_SCREEN_DISPLAY" };
        constexpr Usage STEREO_MODE = { 0xD4, UsageKind::OTHER, "STEREO_MODE" };
    }
}
