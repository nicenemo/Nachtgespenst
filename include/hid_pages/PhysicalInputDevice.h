#pragma once
#include "../HID_Types.h"

namespace HID::PhysicalInputDevice {
    constexpr uint8_t PAGE_ID = 0x000F;
    namespace OTHER {
        constexpr Usage ROM_FLAG = { 0x24, UsageKind::OTHER, "ROM_FLAG" };
        constexpr Usage EFFECT_TYPE = { 0x25, UsageKind::OTHER, "EFFECT_TYPE" };
        constexpr Usage AXES_ENABLE = { 0x55, UsageKind::OTHER, "AXES_ENABLE" };
        constexpr Usage DIRECTION_ENABLE = { 0x56, UsageKind::OTHER, "DIRECTION_ENABLE" };
        constexpr Usage BLOCK_TYPE = { 0x59, UsageKind::OTHER, "BLOCK_TYPE" };
        constexpr Usage ISOCH_CUSTOM_FORCE_ENABLE = { 0x67, UsageKind::OTHER, "ISOCH_CUSTOM_FORCE_ENABLE" };
        constexpr Usage EFFECT_OPERATION = { 0x78, UsageKind::OTHER, "EFFECT_OPERATION" };
        constexpr Usage ROM_POOL_SIZE = { 0x81, UsageKind::OTHER, "ROM_POOL_SIZE" };
        constexpr Usage ROM_EFFECT_BLOCK_COUNT = { 0x82, UsageKind::OTHER, "ROM_EFFECT_BLOCK_COUNT" };
        constexpr Usage SIMULTANEOUS_EFFECTS_MAX = { 0x83, UsageKind::OTHER, "SIMULTANEOUS_EFFECTS_MAX" };
        constexpr Usage POOL_ALIGNMENT = { 0x84, UsageKind::OTHER, "POOL_ALIGNMENT" };
        constexpr Usage EFFECT_PARAMETER_BLOCK_LOAD_STATUS = { 0x8B, UsageKind::OTHER, "EFFECT_PARAMETER_BLOCK_LOAD_STATUS" };
        constexpr Usage EFFECT_PLAYING = { 0x94, UsageKind::OTHER, "EFFECT_PLAYING" };
        constexpr Usage PID_DEVICE_CONTROL = { 0x96, UsageKind::OTHER, "PID_DEVICE_CONTROL" };
        constexpr Usage DEVICE_PAUSED = { 0x9F, UsageKind::OTHER, "DEVICE_PAUSED" };
        constexpr Usage ACTUATORS_ENABLED = { 0xA0, UsageKind::OTHER, "ACTUATORS_ENABLED" };
        constexpr Usage SAFETY_SWITCH = { 0xA4, UsageKind::OTHER, "SAFETY_SWITCH" };
        constexpr Usage ACTUATOR_OVERRIDE_SWITCH = { 0xA5, UsageKind::OTHER, "ACTUATOR_OVERRIDE_SWITCH" };
        constexpr Usage DEVICE_MANAGED_POOL = { 0xA9, UsageKind::OTHER, "DEVICE_MANAGED_POOL" };
        constexpr Usage SHARED_PARAMETER_BLOCKS = { 0xAA, UsageKind::OTHER, "SHARED_PARAMETER_BLOCKS" };
    }
    namespace OOC {
        constexpr Usage ACTUATOR_POWER = { 0xA6, UsageKind::OOC, "ACTUATOR_POWER" };
    }
    namespace DV {
        constexpr Usage NORMAL = { 0x20, UsageKind::DV, "NORMAL" };
        constexpr Usage EFFECT_PARAMETER_BLOCK_INDEX = { 0x22, UsageKind::DV, "EFFECT_PARAMETER_BLOCK_INDEX" };
        constexpr Usage PARAMETER_BLOCK_OFFSET = { 0x23, UsageKind::DV, "PARAMETER_BLOCK_OFFSET" };
        constexpr Usage DURATION = { 0x50, UsageKind::DV, "DURATION" };
        constexpr Usage SAMPLE_PERIOD = { 0x51, UsageKind::DV, "SAMPLE_PERIOD" };
        constexpr Usage GAIN = { 0x52, UsageKind::DV, "GAIN" };
        constexpr Usage TRIGGER_BUTTON = { 0x53, UsageKind::DV, "TRIGGER_BUTTON" };
        constexpr Usage TRIGGER_REPEAT_INTERVAL = { 0x54, UsageKind::DV, "TRIGGER_REPEAT_INTERVAL" };
        constexpr Usage ATTACK_LEVEL = { 0x5B, UsageKind::DV, "ATTACK_LEVEL" };
        constexpr Usage ATTACK_TIME = { 0x5C, UsageKind::DV, "ATTACK_TIME" };
        constexpr Usage FADE_LEVEL = { 0x5D, UsageKind::DV, "FADE_LEVEL" };
        constexpr Usage FADE_TIME = { 0x5E, UsageKind::DV, "FADE_TIME" };
        constexpr Usage CENTER_POINT_OFFSET = { 0x60, UsageKind::DV, "CENTER_POINT_OFFSET" };
        constexpr Usage POSITIVE_COEFFICIENT = { 0x61, UsageKind::DV, "POSITIVE_COEFFICIENT" };
        constexpr Usage NEGATIVE_COEFFICIENT = { 0x62, UsageKind::DV, "NEGATIVE_COEFFICIENT" };
        constexpr Usage POSITIVE_SATURATION = { 0x63, UsageKind::DV, "POSITIVE_SATURATION" };
        constexpr Usage NEGATIVE_SATURATION = { 0x64, UsageKind::DV, "NEGATIVE_SATURATION" };
        constexpr Usage DEAD_BAND = { 0x65, UsageKind::DV, "DEAD_BAND" };
        constexpr Usage CUSTOM_FORCE_DATA = { 0x69, UsageKind::DV, "CUSTOM_FORCE_DATA" };
        constexpr Usage CUSTOM_FORCE_VENDOR_DEFINED_DATA = { 0x6A, UsageKind::DV, "CUSTOM_FORCE_VENDOR_DEFINED_DATA" };
        constexpr Usage CUSTOM_FORCE_DATA_OFFSET = { 0x6C, UsageKind::DV, "CUSTOM_FORCE_DATA_OFFSET" };
        constexpr Usage SAMPLE_COUNT = { 0x6D, UsageKind::DV, "SAMPLE_COUNT" };
        constexpr Usage OFFSET = { 0x6F, UsageKind::DV, "OFFSET" };
        constexpr Usage MAGNITUDE = { 0x70, UsageKind::DV, "MAGNITUDE" };
        constexpr Usage PHASE = { 0x71, UsageKind::DV, "PHASE" };
        constexpr Usage PERIOD = { 0x72, UsageKind::DV, "PERIOD" };
        constexpr Usage RAMP_START = { 0x75, UsageKind::DV, "RAMP_START" };
        constexpr Usage RAMP_END = { 0x76, UsageKind::DV, "RAMP_END" };
        constexpr Usage LOOP_COUNT = { 0x7C, UsageKind::DV, "LOOP_COUNT" };
        constexpr Usage DEVICE_GAIN = { 0x7E, UsageKind::DV, "DEVICE_GAIN" };
        constexpr Usage RAM_POOL_SIZE = { 0x80, UsageKind::DV, "RAM_POOL_SIZE" };
        constexpr Usage MOVE_SOURCE = { 0x86, UsageKind::DV, "MOVE_SOURCE" };
        constexpr Usage MOVE_DESTINATION = { 0x87, UsageKind::DV, "MOVE_DESTINATION" };
        constexpr Usage MOVE_LENGTH = { 0x88, UsageKind::DV, "MOVE_LENGTH" };
        constexpr Usage BLOCK_HANDLE = { 0x8F, UsageKind::DV, "BLOCK_HANDLE" };
        constexpr Usage START_DELAY = { 0xA7, UsageKind::DV, "START_DELAY" };
        constexpr Usage RAM_POOL_AVAILABLE = { 0xAC, UsageKind::DV, "RAM_POOL_AVAILABLE" };
    }
    namespace CL {
        constexpr Usage SET_EFFECT_REPORT = { 0x21, UsageKind::CL, "SET_EFFECT_REPORT" };
        constexpr Usage DIRECTION = { 0x57, UsageKind::CL, "DIRECTION" };
        constexpr Usage TYPE_SPECIFIC_BLOCK_OFFSET = { 0x58, UsageKind::CL, "TYPE_SPECIFIC_BLOCK_OFFSET" };
        constexpr Usage SET_ENVELOPE_REPORT = { 0x5A, UsageKind::CL, "SET_ENVELOPE_REPORT" };
        constexpr Usage SET_CONDITION_REPORT = { 0x5F, UsageKind::CL, "SET_CONDITION_REPORT" };
        constexpr Usage DOWNLOAD_FORCE_SAMPLE = { 0x66, UsageKind::CL, "DOWNLOAD_FORCE_SAMPLE" };
        constexpr Usage CUSTOM_FORCE_DATA_REPORT = { 0x68, UsageKind::CL, "CUSTOM_FORCE_DATA_REPORT" };
        constexpr Usage SET_CUSTOM_FORCE_REPORT = { 0x6B, UsageKind::CL, "SET_CUSTOM_FORCE_REPORT" };
        constexpr Usage SET_PERIODIC_REPORT = { 0x6E, UsageKind::CL, "SET_PERIODIC_REPORT" };
        constexpr Usage SET_CONSTANT_FORCE_REPORT = { 0x73, UsageKind::CL, "SET_CONSTANT_FORCE_REPORT" };
        constexpr Usage SET_RAMP_FORCE_REPORT = { 0x74, UsageKind::CL, "SET_RAMP_FORCE_REPORT" };
        constexpr Usage EFFECT_OPERATION_REPORT = { 0x77, UsageKind::CL, "EFFECT_OPERATION_REPORT" };
        constexpr Usage DEVICE_GAIN_REPORT = { 0x7D, UsageKind::CL, "DEVICE_GAIN_REPORT" };
        constexpr Usage PARAMETER_BLOCK_POOLS_REPORT = { 0x7F, UsageKind::CL, "PARAMETER_BLOCK_POOLS_REPORT" };
        constexpr Usage PARAMETER_BLOCK_MOVE_REPORT = { 0x85, UsageKind::CL, "PARAMETER_BLOCK_MOVE_REPORT" };
        constexpr Usage EFFECT_PARAMETER_BLOCK_LOAD_REPORT = { 0x89, UsageKind::CL, "EFFECT_PARAMETER_BLOCK_LOAD_REPORT" };
        constexpr Usage EFFECT_PARAMETER_BLOCK_FREE_REPORT = { 0x90, UsageKind::CL, "EFFECT_PARAMETER_BLOCK_FREE_REPORT" };
        constexpr Usage TYPE_SPECIFIC_BLOCK_HANDLE = { 0x91, UsageKind::CL, "TYPE_SPECIFIC_BLOCK_HANDLE" };
        constexpr Usage PID_STATE_REPORT = { 0x92, UsageKind::CL, "PID_STATE_REPORT" };
        constexpr Usage PID_DEVICE_CONTROL_REPORT = { 0x95, UsageKind::CL, "PID_DEVICE_CONTROL_REPORT" };
        constexpr Usage PARAMETER_BLOCK_SIZE = { 0xA8, UsageKind::CL, "PARAMETER_BLOCK_SIZE" };
        constexpr Usage CREATE_NEW_EFFECT_PARAMETER_BLOCK_REPORT = { 0xAB, UsageKind::CL, "CREATE_NEW_EFFECT_PARAMETER_BLOCK_REPORT" };
    }
    namespace Sel {
        constexpr Usage ET_CONSTANT_FORCE = { 0x26, UsageKind::Sel, "ET_CONSTANT_FORCE" };
        constexpr Usage ET_RAMP = { 0x27, UsageKind::Sel, "ET_RAMP" };
        constexpr Usage ET_CUSTOM_FORCE = { 0x28, UsageKind::Sel, "ET_CUSTOM_FORCE" };
        constexpr Usage ET_SQUARE = { 0x30, UsageKind::Sel, "ET_SQUARE" };
        constexpr Usage ET_SINE = { 0x31, UsageKind::Sel, "ET_SINE" };
        constexpr Usage ET_TRIANGLE = { 0x32, UsageKind::Sel, "ET_TRIANGLE" };
        constexpr Usage ET_SAWTOOTH_UP = { 0x33, UsageKind::Sel, "ET_SAWTOOTH_UP" };
        constexpr Usage ET_SAWTOOTH_DOWN = { 0x34, UsageKind::Sel, "ET_SAWTOOTH_DOWN" };
        constexpr Usage ET_SPRING = { 0x40, UsageKind::Sel, "ET_SPRING" };
        constexpr Usage ET_DAMPER = { 0x41, UsageKind::Sel, "ET_DAMPER" };
        constexpr Usage ET_INERTIA = { 0x42, UsageKind::Sel, "ET_INERTIA" };
        constexpr Usage ET_FRICTION = { 0x43, UsageKind::Sel, "ET_FRICTION" };
        constexpr Usage OP_EFFECT_START = { 0x79, UsageKind::Sel, "OP_EFFECT_START" };
        constexpr Usage OP_EFFECT_START_SOLO = { 0x7A, UsageKind::Sel, "OP_EFFECT_START_SOLO" };
        constexpr Usage OP_EFFECT_STOP = { 0x7B, UsageKind::Sel, "OP_EFFECT_STOP" };
        constexpr Usage BLOCK_LOAD_SUCCESS = { 0x8C, UsageKind::Sel, "BLOCK_LOAD_SUCCESS" };
        constexpr Usage BLOCK_LOAD_FULL = { 0x8D, UsageKind::Sel, "BLOCK_LOAD_FULL" };
        constexpr Usage BLOCK_LOAD_ERROR = { 0x8E, UsageKind::Sel, "BLOCK_LOAD_ERROR" };
        constexpr Usage DC_ENABLE_ACTUATORS = { 0x97, UsageKind::Sel, "DC_ENABLE_ACTUATORS" };
        constexpr Usage DC_DISABLE_ACTUATORS = { 0x98, UsageKind::Sel, "DC_DISABLE_ACTUATORS" };
        constexpr Usage DC_STOP_ALL_EFFECTS = { 0x99, UsageKind::Sel, "DC_STOP_ALL_EFFECTS" };
        constexpr Usage DC_RESET = { 0x9A, UsageKind::Sel, "DC_RESET" };
        constexpr Usage DC_PAUSE = { 0x9B, UsageKind::Sel, "DC_PAUSE" };
        constexpr Usage DC_CONTINUE = { 0x9C, UsageKind::Sel, "DC_CONTINUE" };
    }
    namespace CA {
        constexpr Usage PHYSICAL_INPUT_DEVICE = { 0x01, UsageKind::CA, "PHYSICAL_INPUT_DEVICE" };
    }
}
