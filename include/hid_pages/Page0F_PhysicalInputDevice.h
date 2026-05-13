#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Physical Input Device (0x0F)
 */
namespace HID::PhysicalInputDevice {
    constexpr uint8_t PAGE_ID = 0x0F;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint16_t PHYSICAL_INPUT_DEVICE = 0x01;

    // ==========================================
    // KIND: Logical Collection (CL)
    // ==========================================
    
    constexpr uint16_t SET_EFFECT_REPORT = 0x21;
    
    constexpr uint16_t DIRECTION = 0x57;
    
    constexpr uint16_t TYPE_SPECIFIC_BLOCK_OFFSET = 0x58;
    
    constexpr uint16_t SET_ENVELOPE_REPORT = 0x5A;
    
    constexpr uint16_t SET_CONDITION_REPORT = 0x5F;
    
    constexpr uint16_t DOWNLOAD_FORCE_SAMPLE = 0x66;
    
    constexpr uint16_t CUSTOM_FORCE_DATA_REPORT = 0x68;
    
    constexpr uint16_t SET_CUSTOM_FORCE_REPORT = 0x6B;
    
    constexpr uint16_t SET_PERIODIC_REPORT = 0x6E;
    
    constexpr uint16_t SET_CONSTANT_FORCE_REPORT = 0x73;
    
    constexpr uint16_t SET_RAMP_FORCE_REPORT = 0x74;
    
    constexpr uint16_t EFFECT_OPERATION_REPORT = 0x77;
    
    constexpr uint16_t DEVICE_GAIN_REPORT = 0x7D;
    
    constexpr uint16_t PARAMETER_BLOCK_POOLS_REPORT = 0x7F;
    
    constexpr uint16_t PARAMETER_BLOCK_MOVE_REPORT = 0x85;
    
    constexpr uint16_t EFFECT_PARAMETER_BLOCK_LOAD_REPORT = 0x89;
    
    constexpr uint16_t EFFECT_PARAMETER_BLOCK_FREE_REPORT = 0x90;
    
    constexpr uint16_t TYPE_SPECIFIC_BLOCK_HANDLE = 0x91;
    
    constexpr uint16_t PID_STATE_REPORT = 0x92;
    
    constexpr uint16_t PID_DEVICE_CONTROL_REPORT = 0x95;
    
    constexpr uint16_t PARAMETER_BLOCK_SIZE = 0xA8;
    
    constexpr uint16_t CREATE_NEW_EFFECT_PARAMETER_BLOCK_REPORT = 0xAB;

    // ==========================================
    // KIND: DF (DF)
    // ==========================================
    
    constexpr uint16_t ROM_FLAG = 0x24;
    
    constexpr uint16_t DIRECTION_ENABLE = 0x56;
    
    constexpr uint16_t ISOCH_CUSTOM_FORCE_ENABLE = 0x67;
    
    constexpr uint16_t EFFECT_PLAYING = 0x94;
    
    constexpr uint16_t DEVICE_PAUSED = 0x9F;
    
    constexpr uint16_t ACTUATORS_ENABLED = 0xA0;
    
    constexpr uint16_t SAFETY_SWITCH = 0xA4;
    
    constexpr uint16_t ACTUATOR_OVERRIDE_SWITCH = 0xA5;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint16_t NORMAL = 0x20;
    
    constexpr uint16_t EFFECT_PARAMETER_BLOCK_INDEX = 0x22;
    
    constexpr uint16_t PARAMETER_BLOCK_OFFSET = 0x23;
    
    constexpr uint16_t DURATION = 0x50;
    
    constexpr uint16_t SAMPLE_PERIOD = 0x51;
    
    constexpr uint16_t GAIN = 0x52;
    
    constexpr uint16_t TRIGGER_BUTTON = 0x53;
    
    constexpr uint16_t TRIGGER_REPEAT_INTERVAL = 0x54;
    
    constexpr uint16_t ATTACK_LEVEL = 0x5B;
    
    constexpr uint16_t ATTACK_TIME = 0x5C;
    
    constexpr uint16_t FADE_LEVEL = 0x5D;
    
    constexpr uint16_t FADE_TIME = 0x5E;
    
    constexpr uint16_t CENTER_POINT_OFFSET = 0x60;
    
    constexpr uint16_t POSITIVE_COEFFICIENT = 0x61;
    
    constexpr uint16_t NEGATIVE_COEFFICIENT = 0x62;
    
    constexpr uint16_t POSITIVE_SATURATION = 0x63;
    
    constexpr uint16_t NEGATIVE_SATURATION = 0x64;
    
    constexpr uint16_t DEAD_BAND = 0x65;
    
    constexpr uint16_t CUSTOM_FORCE_DATA = 0x69;
    
    constexpr uint16_t CUSTOM_FORCE_VENDOR_DEFINED_DATA = 0x6A;
    
    constexpr uint16_t CUSTOM_FORCE_DATA_OFFSET = 0x6C;
    
    constexpr uint16_t SAMPLE_COUNT = 0x6D;
    
    constexpr uint16_t OFFSET = 0x6F;
    
    constexpr uint16_t MAGNITUDE = 0x70;
    
    constexpr uint16_t PHASE = 0x71;
    
    constexpr uint16_t PERIOD = 0x72;
    
    constexpr uint16_t RAMP_START = 0x75;
    
    constexpr uint16_t RAMP_END = 0x76;
    
    constexpr uint16_t LOOP_COUNT = 0x7C;
    
    constexpr uint16_t DEVICE_GAIN = 0x7E;
    
    constexpr uint16_t RAM_POOL_SIZE = 0x80;
    
    constexpr uint16_t MOVE_SOURCE = 0x86;
    
    constexpr uint16_t MOVE_DESTINATION = 0x87;
    
    constexpr uint16_t MOVE_LENGTH = 0x88;
    
    constexpr uint16_t BLOCK_HANDLE = 0x8F;
    
    constexpr uint16_t START_DELAY = 0xA7;
    
    constexpr uint16_t RAM_POOL_AVAILABLE = 0xAC;

    // ==========================================
    // KIND: NAry (NAry)
    // ==========================================
    
    constexpr uint16_t EFFECT_TYPE = 0x25;
    
    constexpr uint16_t BLOCK_TYPE = 0x59;
    
    constexpr uint16_t EFFECT_OPERATION = 0x78;
    
    constexpr uint16_t EFFECT_PARAMETER_BLOCK_LOAD_STATUS = 0x8B;
    
    constexpr uint16_t PID_DEVICE_CONTROL = 0x96;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint16_t ACTUATOR_POWER = 0xA6;

    // ==========================================
    // KIND: SF (SF)
    // ==========================================
    
    constexpr uint16_t DEVICE_MANAGED_POOL = 0xA9;
    
    constexpr uint16_t SHARED_PARAMETER_BLOCKS = 0xAA;

    // ==========================================
    // KIND: SV (SV)
    // ==========================================
    
    constexpr uint16_t ROM_POOL_SIZE = 0x81;
    
    constexpr uint16_t ROM_EFFECT_BLOCK_COUNT = 0x82;
    
    constexpr uint16_t SIMULTANEOUS_EFFECTS_MAX = 0x83;
    
    constexpr uint16_t POOL_ALIGNMENT = 0x84;

    // ==========================================
    // KIND: Selector (Menu/Choice) (Sel)
    // ==========================================
    
    constexpr uint16_t ET_CONSTANT_FORCE = 0x26;
    
    constexpr uint16_t ET_RAMP = 0x27;
    
    constexpr uint16_t ET_CUSTOM_FORCE = 0x28;
    
    constexpr uint16_t ET_SQUARE = 0x30;
    
    constexpr uint16_t ET_SINE = 0x31;
    
    constexpr uint16_t ET_TRIANGLE = 0x32;
    
    constexpr uint16_t ET_SAWTOOTH_UP = 0x33;
    
    constexpr uint16_t ET_SAWTOOTH_DOWN = 0x34;
    
    constexpr uint16_t ET_SPRING = 0x40;
    
    constexpr uint16_t ET_DAMPER = 0x41;
    
    constexpr uint16_t ET_INERTIA = 0x42;
    
    constexpr uint16_t ET_FRICTION = 0x43;
    
    constexpr uint16_t OP_EFFECT_START = 0x79;
    
    constexpr uint16_t OP_EFFECT_START_SOLO = 0x7A;
    
    constexpr uint16_t OP_EFFECT_STOP = 0x7B;
    
    constexpr uint16_t BLOCK_LOAD_SUCCESS = 0x8C;
    
    constexpr uint16_t BLOCK_LOAD_FULL = 0x8D;
    
    constexpr uint16_t BLOCK_LOAD_ERROR = 0x8E;
    
    constexpr uint16_t DC_ENABLE_ACTUATORS = 0x97;
    
    constexpr uint16_t DC_DISABLE_ACTUATORS = 0x98;
    
    constexpr uint16_t DC_STOP_ALL_EFFECTS = 0x99;
    
    constexpr uint16_t DC_RESET = 0x9A;
    
    constexpr uint16_t DC_PAUSE = 0x9B;
    
    constexpr uint16_t DC_CONTINUE = 0x9C;

    // ==========================================
    // KIND: US (US)
    // ==========================================
    
    constexpr uint16_t AXES_ENABLE = 0x55;

}
