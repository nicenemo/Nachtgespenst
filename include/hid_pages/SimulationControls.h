#pragma once
#include "../HID_Types.h"

namespace HID::SimulationControls {
    constexpr uint8_t PAGE_ID = 0x0002;
    namespace OOC {
        constexpr Usage AUTOPILOT_ENABLE = { 0xB3, UsageKind::OOC, "AUTOPILOT_ENABLE" };
        constexpr Usage ELECTRONIC_COUNTERMEASURES = { 0xB7, UsageKind::OOC, "ELECTRONIC_COUNTERMEASURES" };
        constexpr Usage FLIGHT_COMMUNICATIONS = { 0xBC, UsageKind::OOC, "FLIGHT_COMMUNICATIONS" };
        constexpr Usage LANDING_GEAR = { 0xBE, UsageKind::OOC, "LANDING_GEAR" };
        constexpr Usage WEAPONS_ARM = { 0xC1, UsageKind::OOC, "WEAPONS_ARM" };
    }
    namespace DV {
        constexpr Usage AILERON = { 0xB0, UsageKind::DV, "AILERON" };
        constexpr Usage AILERON_TRIM = { 0xB1, UsageKind::DV, "AILERON_TRIM" };
        constexpr Usage ANTI_TORQUE_CONTROL = { 0xB2, UsageKind::DV, "ANTI_TORQUE_CONTROL" };
        constexpr Usage COLLECTIVE_CONTROL = { 0xB5, UsageKind::DV, "COLLECTIVE_CONTROL" };
        constexpr Usage DIVE_BRAKE = { 0xB6, UsageKind::DV, "DIVE_BRAKE" };
        constexpr Usage ELEVATOR = { 0xB8, UsageKind::DV, "ELEVATOR" };
        constexpr Usage ELEVATOR_TRIM = { 0xB9, UsageKind::DV, "ELEVATOR_TRIM" };
        constexpr Usage RUDDER = { 0xBA, UsageKind::DV, "RUDDER" };
        constexpr Usage THROTTLE = { 0xBB, UsageKind::DV, "THROTTLE" };
        constexpr Usage TOE_BRAKE = { 0xBF, UsageKind::DV, "TOE_BRAKE" };
        constexpr Usage WING_FLAPS = { 0xC3, UsageKind::DV, "WING_FLAPS" };
        constexpr Usage ACCELERATOR = { 0xC4, UsageKind::DV, "ACCELERATOR" };
        constexpr Usage BRAKE = { 0xC5, UsageKind::DV, "BRAKE" };
        constexpr Usage CLUTCH = { 0xC6, UsageKind::DV, "CLUTCH" };
        constexpr Usage SHIFTER = { 0xC7, UsageKind::DV, "SHIFTER" };
        constexpr Usage STEERING = { 0xC8, UsageKind::DV, "STEERING" };
        constexpr Usage TURRET_DIRECTION = { 0xC9, UsageKind::DV, "TURRET_DIRECTION" };
        constexpr Usage BARREL_ELEVATION = { 0xCA, UsageKind::DV, "BARREL_ELEVATION" };
        constexpr Usage DIVE_PLANE = { 0xCB, UsageKind::DV, "DIVE_PLANE" };
        constexpr Usage BALLAST = { 0xCC, UsageKind::DV, "BALLAST" };
        constexpr Usage BICYCLE_CRANK = { 0xCD, UsageKind::DV, "BICYCLE_CRANK" };
        constexpr Usage HANDLE_BARS = { 0xCE, UsageKind::DV, "HANDLE_BARS" };
        constexpr Usage FRONT_BRAKE = { 0xCF, UsageKind::DV, "FRONT_BRAKE" };
        constexpr Usage REAR_BRAKE = { 0xD0, UsageKind::DV, "REAR_BRAKE" };
    }
    namespace MC {
        constexpr Usage TRIGGER = { 0xC0, UsageKind::MC, "TRIGGER" };
    }
    namespace CP {
        constexpr Usage CYCLIC_CONTROL = { 0x22, UsageKind::CP, "CYCLIC_CONTROL" };
        constexpr Usage CYCLIC_TRIM = { 0x23, UsageKind::CP, "CYCLIC_TRIM" };
        constexpr Usage TRACK_CONTROL = { 0x25, UsageKind::CP, "TRACK_CONTROL" };
    }
    namespace OSC {
        constexpr Usage CHAFF_RELEASE = { 0xB4, UsageKind::OSC, "CHAFF_RELEASE" };
        constexpr Usage FLARE_RELEASE = { 0xBD, UsageKind::OSC, "FLARE_RELEASE" };
        constexpr Usage WEAPONS_SELECT = { 0xC2, UsageKind::OSC, "WEAPONS_SELECT" };
    }
    namespace CA {
        constexpr Usage FLIGHT_SIMULATION_DEVICE = { 0x01, UsageKind::CA, "FLIGHT_SIMULATION_DEVICE" };
        constexpr Usage AUTOMOBILE_SIMULATION_DEVICE = { 0x02, UsageKind::CA, "AUTOMOBILE_SIMULATION_DEVICE" };
        constexpr Usage TANK_SIMULATION_DEVICE = { 0x03, UsageKind::CA, "TANK_SIMULATION_DEVICE" };
        constexpr Usage SPACESHIP_SIMULATION_DEVICE = { 0x04, UsageKind::CA, "SPACESHIP_SIMULATION_DEVICE" };
        constexpr Usage SUBMARINE_SIMULATION_DEVICE = { 0x05, UsageKind::CA, "SUBMARINE_SIMULATION_DEVICE" };
        constexpr Usage SAILING_SIMULATION_DEVICE = { 0x06, UsageKind::CA, "SAILING_SIMULATION_DEVICE" };
        constexpr Usage MOTORCYCLE_SIMULATION_DEVICE = { 0x07, UsageKind::CA, "MOTORCYCLE_SIMULATION_DEVICE" };
        constexpr Usage SPORTS_SIMULATION_DEVICE = { 0x08, UsageKind::CA, "SPORTS_SIMULATION_DEVICE" };
        constexpr Usage AIRPLANE_SIMULATION_DEVICE = { 0x09, UsageKind::CA, "AIRPLANE_SIMULATION_DEVICE" };
        constexpr Usage HELICOPTER_SIMULATION_DEVICE = { 0x0A, UsageKind::CA, "HELICOPTER_SIMULATION_DEVICE" };
        constexpr Usage MAGIC_CARPET_SIMULATION_DEVICE = { 0x0B, UsageKind::CA, "MAGIC_CARPET_SIMULATION_DEVICE" };
        constexpr Usage BICYCLE_SIMULATION_DEVICE = { 0x0C, UsageKind::CA, "BICYCLE_SIMULATION_DEVICE" };
        constexpr Usage FLIGHT_CONTROL_STICK = { 0x20, UsageKind::CA, "FLIGHT_CONTROL_STICK" };
        constexpr Usage FLIGHT_STICK = { 0x21, UsageKind::CA, "FLIGHT_STICK" };
        constexpr Usage FLIGHT_YOKE = { 0x24, UsageKind::CA, "FLIGHT_YOKE" };
    }
}
