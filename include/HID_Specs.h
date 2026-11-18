#pragma once
#include <stdint.h>
#include <vector>
#include <string>

namespace HID {

    // --- MODIFIERS (byte 0 of the keyboard report) ---
    namespace Modifiers {
        // Individual bits (USB HID spec, page 07)
        constexpr uint8_t L_CTRL   = 0x01; // Left Control
        constexpr uint8_t L_SHIFT  = 0x02; // Left Shift
        constexpr uint8_t L_ALT    = 0x04; // Left Alt
        constexpr uint8_t L_GUI    = 0x08; // Left Win / Cmd
        
        constexpr uint8_t R_CTRL   = 0x10; // Right Control
        constexpr uint8_t R_SHIFT  = 0x20; // Right Shift
        constexpr uint8_t R_ALT    = 0x40; // Right Alt (AltGr)
        constexpr uint8_t R_GUI    = 0x80; // Right Win / Cmd

        // --- Combinations ---
        constexpr uint8_t NONE      = 0x00;
        
        // Left and right pairs
        constexpr uint8_t L_CTL_SFT = L_CTRL | L_SHIFT;
        constexpr uint8_t L_ALT_SFT = L_ALT  | L_SHIFT;
        constexpr uint8_t R_CTL_SFT = R_CTRL | R_SHIFT;
        constexpr uint8_t R_ALT_SFT = R_ALT  | R_SHIFT;

        // The "über" modifiers (left)
        constexpr uint8_t L_MEH     = L_CTRL | L_SHIFT | L_ALT;
        constexpr uint8_t L_HYPER   = L_CTRL | L_SHIFT | L_ALT | L_GUI;

        // The "über" modifiers (right)
        constexpr uint8_t R_MEH     = R_CTRL | R_SHIFT | R_ALT;
        constexpr uint8_t R_HYPER   = R_CTRL | R_SHIFT | R_ALT | R_GUI;

        // Default aliases (point to the left-hand set)
        constexpr uint8_t MEH       = L_MEH;
        constexpr uint8_t HYPER     = L_HYPER;
    }

    // --- REPORT DESCRIPTOR ENGINE ---
    namespace Reports {
        enum ID : uint8_t {
            KEYBOARD = 0x01,
            CONSUMER = 0x02,
            SYSTEM   = 0x03,
            HAPTICS  = 0x04,
            LIGHTING = 0x05
        };

        // --- PAGE 0x07 & 0x08: Keyboard + LEDs ---
        inline constexpr uint8_t KeyboardBlock[] = {
            0x05, 0x01,         // Usage Page (Generic Desktop)
            0x09, 0x06,         // Usage (Keyboard)
            0xA1, 0x01,         // Collection (Application)
            0x85, ID::KEYBOARD, //   REPORT ID (1)
            0x05, 0x07,         //   Usage Page (Keyboard)
            0x19, 0xE0,         //   Usage Minimum (224 - L_Ctrl)
            0x29, 0xE7,         //   Usage Maximum (231 - R_GUI)
            0x15, 0x00,         //   Logical Minimum (0)
            0x25, 0x01,         //   Logical Maximum (1)
            0x75, 0x01,         //   Report Size (1)
            0x95, 0x08,         //   Report Count (8 bits: 1 per modifier)
            0x81, 0x02,         //   Input (Data, Variable, Absolute) -> Modifier Byte
            0x95, 0x01,         //   Report Count (1)
            0x75, 0x08,         //   Report Size (8)
            0x81, 0x01,         //   Input (Constant) -> Reserved Byte
            0x95, 0x06,         //   Report Count (6)
            0x75, 0x08,         //   Report Size (8 bits per key)
            0x15, 0x00,         //   Logical Minimum (0)
            0x26, 0xFF, 0x00,   //   Logical Maximum (255)
            0x05, 0x07,         //   Usage Page (Keyboard)
            0x19, 0x00,         //   Usage Minimum (0)
            0x29, 0xDD,         //   Usage Maximum (221)
            0x81, 0x00,         //   Input (Data, Array, Absolute) -> Key Array (6KRO)
            0x15, 0x00,         //   Logical Minimum (0)
            0x25, 0x01,         //   Logical Maximum (1)
            0x05, 0x08,         //   Usage Page (LEDs)
            0x19, 0x01,         //   Usage Minimum (1 - Num Lock)
            0x29, 0x05,         //   Usage Maximum (5 - Kana)
            0x95, 0x05,         //   Report Count (5 LEDs)
            0x75, 0x01,         //   Report Size (1 bit per LED)
            0x91, 0x02,         //   Output (Data, Variable, Absolute) -> LED Status
            0x95, 0x01,         //   Report Count (1)
            0x75, 0x03,         //   Report Size (3 bits)
            0x91, 0x01,         //   Output (Constant) -> Padding
            0xC0                // End Collection
        };

        // --- PAGE 0x0C: Consumer Control (Media) ---
        inline constexpr uint8_t ConsumerBlock[] = {
            0x05, 0x0C,         // Usage Page (Consumer)
            0x09, 0x01,         // Usage (Consumer Control)
            0xA1, 0x01,         // Collection (Application)
            0x85, ID::CONSUMER, //   REPORT ID (2)
            0x19, 0x00,         //   Usage Minimum (0)
            0x2A, 0xFF, 0x03,   //   Usage Maximum (1023)
            0x15, 0x00,         //   Logical Minimum (0)
            0x26, 0xFF, 0x03,   //   Logical Maximum (1023)
            0x95, 0x01,         //   Report Count (1)
            0x75, 0x10,         //   Report Size (16 bits)
            0x81, 0x00,         //   Input (Data, Array, Absolute)
            0xC0                // End Collection
        };

        // --- PAGE 0x01: System Control (Power/Sleep) ---
        inline constexpr uint8_t SystemBlock[] = {
            0x05, 0x01,         // Usage Page (Generic Desktop)
            0x09, 0x80,         // Usage (System Control)
            0xA1, 0x01,         // Collection (Application)
            0x85, ID::SYSTEM,   //   REPORT ID (3)
            0x19, 0x81,         //   Usage Minimum (System Power Down)
            0x29, 0x83,         //   Usage Maximum (System Wake Up)
            0x15, 0x00,         //   Logical Minimum (0)
            0x25, 0x01,         //   Logical Maximum (1)
            0x75, 0x01,         //   Report Size (1 bit)
            0x95, 0x03,         //   Report Count (3 functions)
            0x81, 0x02,         //   Input (Data, Variable, Absolute)
            0x95, 0x01,         //   Report Count (1)
            0x75, 0x05,         //   Report Size (5 bits)
            0x81, 0x01,         //   Input (Constant) -> Padding
            0xC0                // End Collection
        };

        // --- PAGE 0x0F: Haptics (Physical Input) ---
        inline constexpr uint8_t HapticsBlock[] = {
            0x05, 0x0F,         // Usage Page (Haptics)
            0x09, 0x01,         // Usage (Simple Haptic Controller)
            0xA1, 0x01,         // Collection (Application)
            0x85, ID::HAPTICS,  //   REPORT ID (4)
            0x09, 0x11,         //   Usage (Haptic Waveform Select)
            0x15, 0x00,         //   Logical Minimum (0)
            0x25, 0x08,         //   Logical Maximum (8 Waveforms)
            0x75, 0x08,         //   Report Size (8 bits)
            0x95, 0x01,         //   Report Count (1)
            0xB1, 0x02,         //   Feature (Data, Variable, Absolute)
            0xC0                // End Collection
        };

        // --- PAGE 0x59: Lighting (RGB) ---
        inline constexpr uint8_t LightingBlock[] = {
            0x05, 0x59,         // Usage Page (Lighting)
            0x09, 0x01,         // Usage (Lamp Array)
            0xA1, 0x01,         // Collection (Application)
            0x85, ID::LIGHTING, //   REPORT ID (5)
            0x09, 0x05,         //   Usage (Lamp Multi Update)
            0x15, 0x00,         //   Logical Minimum (0)
            0x25, 0xFF,         //   Logical Maximum (255)
            0x75, 0x08,         //   Report Size (8 bits)
            0x95, 0x04,         //   Report Count (4: ID, R, G, B)
            0x91, 0x02,         //   Output (Data, Variable, Absolute)
            0xC0                // End Collection
        };
    }

    // --- The mapper engine (builds the final vector) ---
    inline std::vector<uint8_t> GetReportMap() {
        std::vector<uint8_t> map;
        auto add = [&](const uint8_t* b, size_t s) { map.insert(map.end(), b, b + s); };

        add(Reports::KeyboardBlock, sizeof(Reports::KeyboardBlock));
        add(Reports::ConsumerBlock, sizeof(Reports::ConsumerBlock));
        // System/Haptics/Lighting blocks are defined above but not advertised:
        // the host would expect reports the firmware does not provide.
        
        return map;
    }
}
