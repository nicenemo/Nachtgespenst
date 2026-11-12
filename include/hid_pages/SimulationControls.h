#pragma once
#include "../HID_Types.h"

namespace HID::SimulationControls {
    constexpr uint16_t PAGE_ID = 0x0002;
    namespace OTHER {
        constexpr Usage FLIGHT_SIMULATION_DEVICE = { 0x01, UsageKind::OTHER, "FLIGHT_SIMULATION_DEVICE" };
        constexpr Usage AUTOMOBILE_SIMULATION_DEVICE = { 0x02, UsageKind::OTHER, "AUTOMOBILE_SIMULATION_DEVICE" };
        constexpr Usage TANK_SIMULATION_DEVICE = { 0x03, UsageKind::OTHER, "TANK_SIMULATION_DEVICE" };
        constexpr Usage SPACESHIP_SIMULATION_DEVICE = { 0x04, UsageKind::OTHER, "SPACESHIP_SIMULATION_DEVICE" };
        constexpr Usage SUBMARINE_SIMULATION_DEVICE = { 0x05, UsageKind::OTHER, "SUBMARINE_SIMULATION_DEVICE" };
        constexpr Usage SAILING_SIMULATION_DEVICE = { 0x06, UsageKind::OTHER, "SAILING_SIMULATION_DEVICE" };
        constexpr Usage MOTORCYCLE_SIMULATION_DEVICE = { 0x07, UsageKind::OTHER, "MOTORCYCLE_SIMULATION_DEVICE" };
        constexpr Usage SPORTS_SIMULATION_DEVICE = { 0x08, UsageKind::OTHER, "SPORTS_SIMULATION_DEVICE" };
        constexpr Usage AIRPLANE_SIMULATION_DEVICE = { 0x09, UsageKind::OTHER, "AIRPLANE_SIMULATION_DEVICE" };
        constexpr Usage HELICOPTER_SIMULATION_DEVICE = { 0x0A, UsageKind::OTHER, "HELICOPTER_SIMULATION_DEVICE" };
        constexpr Usage MAGIC_CARPET_SIMULATION_DEVICE = { 0x0B, UsageKind::OTHER, "MAGIC_CARPET_SIMULATION_DEVICE" };
        constexpr Usage BICYCLE_SIMULATION_DEVICE = { 0x0C, UsageKind::OTHER, "BICYCLE_SIMULATION_DEVICE" };
        constexpr Usage FLIGHT_CONTROL_STICK = { 0x20, UsageKind::OTHER, "FLIGHT_CONTROL_STICK" };
        constexpr Usage FLIGHT_STICK = { 0x21, UsageKind::OTHER, "FLIGHT_STICK" };
        constexpr Usage CYCLIC_CONTROL = { 0x22, UsageKind::OTHER, "CYCLIC_CONTROL" };
        constexpr Usage CYCLIC_TRIM = { 0x23, UsageKind::OTHER, "CYCLIC_TRIM" };
        constexpr Usage FLIGHT_YOKE = { 0x24, UsageKind::OTHER, "FLIGHT_YOKE" };
        constexpr Usage TRACK_CONTROL = { 0x25, UsageKind::OTHER, "TRACK_CONTROL" };
        constexpr Usage AILERON = { 0xB0, UsageKind::OTHER, "AILERON" };
        constexpr Usage AILERON_TRIM = { 0xB1, UsageKind::OTHER, "AILERON_TRIM" };
        constexpr Usage ANTI_TORQUE_CONTROL = { 0xB2, UsageKind::OTHER, "ANTI_TORQUE_CONTROL" };
        constexpr Usage AUTOPILOT_ENABLE = { 0xB3, UsageKind::OTHER, "AUTOPILOT_ENABLE" };
        constexpr Usage CHAFF_RELEASE = { 0xB4, UsageKind::OTHER, "CHAFF_RELEASE" };
        constexpr Usage COLLECTIVE_CONTROL = { 0xB5, UsageKind::OTHER, "COLLECTIVE_CONTROL" };
        constexpr Usage DIVE_BRAKE = { 0xB6, UsageKind::OTHER, "DIVE_BRAKE" };
        constexpr Usage ELECTRONIC_COUNTERMEASURES = { 0xB7, UsageKind::OTHER, "ELECTRONIC_COUNTERMEASURES" };
        constexpr Usage ELEVATOR = { 0xB8, UsageKind::OTHER, "ELEVATOR" };
        constexpr Usage ELEVATOR_TRIM = { 0xB9, UsageKind::OTHER, "ELEVATOR_TRIM" };
        constexpr Usage RUDDER = { 0xBA, UsageKind::OTHER, "RUDDER" };
        constexpr Usage THROTTLE = { 0xBB, UsageKind::OTHER, "THROTTLE" };
        constexpr Usage FLIGHT_COMMUNICATIONS = { 0xBC, UsageKind::OTHER, "FLIGHT_COMMUNICATIONS" };
        constexpr Usage FLARE_RELEASE = { 0xBD, UsageKind::OTHER, "FLARE_RELEASE" };
        constexpr Usage LANDING_GEAR = { 0xBE, UsageKind::OTHER, "LANDING_GEAR" };
        constexpr Usage TOE_BRAKE = { 0xBF, UsageKind::OTHER, "TOE_BRAKE" };
        constexpr Usage TRIGGER = { 0xC0, UsageKind::OTHER, "TRIGGER" };
        constexpr Usage WEAPONS_ARM = { 0xC1, UsageKind::OTHER, "WEAPONS_ARM" };
        constexpr Usage WEAPONS_SELECT = { 0xC2, UsageKind::OTHER, "WEAPONS_SELECT" };
        constexpr Usage WING_FLAPS = { 0xC3, UsageKind::OTHER, "WING_FLAPS" };
        constexpr Usage ACCELERATOR = { 0xC4, UsageKind::OTHER, "ACCELERATOR" };
        constexpr Usage BRAKE = { 0xC5, UsageKind::OTHER, "BRAKE" };
        constexpr Usage CLUTCH = { 0xC6, UsageKind::OTHER, "CLUTCH" };
        constexpr Usage SHIFTER = { 0xC7, UsageKind::OTHER, "SHIFTER" };
        constexpr Usage STEERING = { 0xC8, UsageKind::OTHER, "STEERING" };
        constexpr Usage TURRET_DIRECTION = { 0xC9, UsageKind::OTHER, "TURRET_DIRECTION" };
        constexpr Usage BARREL_ELEVATION = { 0xCA, UsageKind::OTHER, "BARREL_ELEVATION" };
        constexpr Usage DIVE_PLANE = { 0xCB, UsageKind::OTHER, "DIVE_PLANE" };
        constexpr Usage BALLAST = { 0xCC, UsageKind::OTHER, "BALLAST" };
        constexpr Usage BICYCLE_CRANK = { 0xCD, UsageKind::OTHER, "BICYCLE_CRANK" };
        constexpr Usage HANDLE_BARS = { 0xCE, UsageKind::OTHER, "HANDLE_BARS" };
        constexpr Usage FRONT_BRAKE = { 0xCF, UsageKind::OTHER, "FRONT_BRAKE" };
        constexpr Usage REAR_BRAKE = { 0xD0, UsageKind::OTHER, "REAR_BRAKE" };
    }
}
