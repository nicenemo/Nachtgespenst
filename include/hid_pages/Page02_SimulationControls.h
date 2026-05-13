#pragma once
#include <stdint.h>

/**
 * HID Usage Page: Simulation Controls (0x02)
 */
namespace HID::SimulationControls {
    constexpr uint8_t PAGE_ID = 0x02;

    // ==========================================
    // KIND: Application Collection (CA)
    // ==========================================
    
    constexpr uint8_t FLIGHT_SIMULATION_DEVICE = 0x01;
    
    constexpr uint8_t AUTOMOBILE_SIMULATION_DEVICE = 0x02;
    
    constexpr uint8_t TANK_SIMULATION_DEVICE = 0x03;
    
    constexpr uint8_t SPACESHIP_SIMULATION_DEVICE = 0x04;
    
    constexpr uint8_t SUBMARINE_SIMULATION_DEVICE = 0x05;
    
    constexpr uint8_t SAILING_SIMULATION_DEVICE = 0x06;
    
    constexpr uint8_t MOTORCYCLE_SIMULATION_DEVICE = 0x07;
    
    constexpr uint8_t SPORTS_SIMULATION_DEVICE = 0x08;
    
    constexpr uint8_t AIRPLANE_SIMULATION_DEVICE = 0x09;
    
    constexpr uint8_t HELICOPTER_SIMULATION_DEVICE = 0x0A;
    
    constexpr uint8_t MAGIC_CARPET_SIMULATION_DEVICE = 0x0B;
    
    constexpr uint8_t BICYCLE_SIMULATION_DEVICE = 0x0C;
    
    constexpr uint8_t FLIGHT_CONTROL_STICK = 0x20;
    
    constexpr uint8_t FLIGHT_STICK = 0x21;
    
    constexpr uint8_t FLIGHT_YOKE = 0x24;

    // ==========================================
    // KIND: Physical Collection (CP)
    // ==========================================
    
    constexpr uint8_t CYCLIC_CONTROL = 0x22;
    
    constexpr uint8_t CYCLIC_TRIM = 0x23;
    
    constexpr uint8_t TRACK_CONTROL = 0x25;

    // ==========================================
    // KIND: Dynamic Value (Absolute/Variable) (DV)
    // ==========================================
    
    constexpr uint8_t AILERON = 0xB0;
    
    constexpr uint8_t AILERON_TRIM = 0xB1;
    
    constexpr uint8_t ANTI_TORQUE_CONTROL = 0xB2;
    
    constexpr uint8_t COLLECTIVE_CONTROL = 0xB5;
    
    constexpr uint8_t DIVE_BRAKE = 0xB6;
    
    constexpr uint8_t ELEVATOR = 0xB8;
    
    constexpr uint8_t ELEVATOR_TRIM = 0xB9;
    
    constexpr uint8_t RUDDER = 0xBA;
    
    constexpr uint8_t THROTTLE = 0xBB;
    
    constexpr uint8_t TOE_BRAKE = 0xBF;
    
    constexpr uint8_t WING_FLAPS = 0xC3;
    
    constexpr uint8_t ACCELERATOR = 0xC4;
    
    constexpr uint8_t BRAKE = 0xC5;
    
    constexpr uint8_t CLUTCH = 0xC6;
    
    constexpr uint8_t SHIFTER = 0xC7;
    
    constexpr uint8_t STEERING = 0xC8;
    
    constexpr uint8_t TURRET_DIRECTION = 0xC9;
    
    constexpr uint8_t BARREL_ELEVATION = 0xCA;
    
    constexpr uint8_t DIVE_PLANE = 0xCB;
    
    constexpr uint8_t BALLAST = 0xCC;
    
    constexpr uint8_t BICYCLE_CRANK = 0xCD;
    
    constexpr uint8_t HANDLE_BARS = 0xCE;
    
    constexpr uint8_t FRONT_BRAKE = 0xCF;
    
    constexpr uint8_t REAR_BRAKE = 0xD0;

    // ==========================================
    // KIND: Momentary Control (Push) (MC)
    // ==========================================
    
    constexpr uint8_t TRIGGER = 0xC0;

    // ==========================================
    // KIND: On/Off Control (Toggle) (OOC)
    // ==========================================
    
    constexpr uint8_t AUTOPILOT_ENABLE = 0xB3;
    
    constexpr uint8_t ELECTRONIC_COUNTERMEASURES = 0xB7;
    
    constexpr uint8_t FLIGHT_COMMUNICATIONS = 0xBC;
    
    constexpr uint8_t LANDING_GEAR = 0xBE;
    
    constexpr uint8_t WEAPONS_ARM = 0xC1;

    // ==========================================
    // KIND: One Shot Control (OSC)
    // ==========================================
    
    constexpr uint8_t CHAFF_RELEASE = 0xB4;
    
    constexpr uint8_t FLARE_RELEASE = 0xBD;
    
    constexpr uint8_t WEAPONS_SELECT = 0xC2;

}
