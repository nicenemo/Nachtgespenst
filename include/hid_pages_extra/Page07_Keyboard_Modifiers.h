#pragma once
#include <stdint.h>

/**
 * HID Keyboard Modifiers (0x07)
 * Manual Bitmask definitions for HID Report ID 1
 */
namespace HID::Keyboard::Modifiers {
    enum Mask : uint8_t {
        NONE      = 0x00,
        
        // --- Core Masks ---
        L_CTRL    = (1 << 0), // 0x01
        L_SHIFT   = (1 << 1), // 0x02
        L_ALT     = (1 << 2), // 0x04
        L_GUI     = (1 << 3), // 0x08
        
        R_CTRL    = (1 << 4), // 0x10
        R_SHIFT   = (1 << 5), // 0x20
        R_ALT     = (1 << 6), // 0x40
        R_GUI     = (1 << 7)  // 0x80
    };

    // ==========================================
    // BOTH SIDE SYMMETRY
    // ==========================================
    constexpr uint8_t BOTH_CTRL      = L_CTRL  | R_CTRL;
    constexpr uint8_t BOTH_SHIFT     = L_SHIFT | R_SHIFT;
    constexpr uint8_t BOTH_ALT       = L_ALT   | R_ALT;
    constexpr uint8_t BOTH_GUI       = L_GUI   | R_GUI;

    // ==========================================
    // MEH COMBINATIONS (Ctrl + Shift + Alt)
    // ==========================================
    constexpr uint8_t L_MEH          = L_CTRL | L_SHIFT | L_ALT;
    constexpr uint8_t R_MEH          = R_CTRL | R_SHIFT | R_ALT;
    constexpr uint8_t BOTH_MEH       = BOTH_CTRL | BOTH_SHIFT | BOTH_ALT;

    // ==========================================
    // HYPER COMBINATIONS (Ctrl + Shift + Alt + GUI)
    // ==========================================
    constexpr uint8_t L_HYPER        = L_CTRL | L_SHIFT | L_ALT | L_GUI;
    constexpr uint8_t R_HYPER        = R_CTRL | R_SHIFT | R_ALT | R_GUI;
    constexpr uint8_t BOTH_HYPER     = BOTH_CTRL | BOTH_SHIFT | BOTH_ALT | BOTH_GUI;

    // ==========================================
    // DUAL MODIFIER PERMUTATIONS
    // ==========================================
    
    // --- Ctrl + Shift ---
    constexpr uint8_t L_CTRL_SHIFT   = L_CTRL  | L_SHIFT;
    constexpr uint8_t R_CTRL_SHIFT   = R_CTRL  | R_SHIFT;
    constexpr uint8_t BOTH_CTRL_SHIFT = BOTH_CTRL | BOTH_SHIFT;

    // --- Ctrl + Alt ---
    constexpr uint8_t L_CTRL_ALT     = L_CTRL  | L_ALT;
    constexpr uint8_t R_CTRL_ALT     = R_CTRL  | R_ALT;
    constexpr uint8_t BOTH_CTRL_ALT   = BOTH_CTRL | BOTH_ALT;

    // --- Ctrl + GUI ---
    constexpr uint8_t L_CTRL_GUI     = L_CTRL  | L_GUI;
    constexpr uint8_t R_CTRL_GUI     = R_CTRL  | R_GUI;
    constexpr uint8_t BOTH_CTRL_GUI   = BOTH_CTRL | BOTH_GUI;

    // --- Shift + Alt ---
    constexpr uint8_t L_SHIFT_ALT    = L_SHIFT | L_ALT;
    constexpr uint8_t R_SHIFT_ALT    = R_SHIFT | R_ALT;
    constexpr uint8_t BOTH_SHIFT_ALT  = BOTH_SHIFT | BOTH_ALT;

    // --- Shift + GUI ---
    constexpr uint8_t L_SHIFT_GUI    = L_SHIFT | L_GUI;
    constexpr uint8_t R_SHIFT_GUI    = R_SHIFT | R_GUI;
    constexpr uint8_t BOTH_SHIFT_GUI  = BOTH_SHIFT | BOTH_GUI;

    // --- Alt + GUI ---
    constexpr uint8_t L_ALT_GUI      = L_ALT   | L_GUI;
    constexpr uint8_t R_ALT_GUI      = R_ALT   | R_GUI;
    constexpr uint8_t BOTH_ALT_GUI    = BOTH_ALT | BOTH_GUI;

    // ==========================================
    // TRIPLE MODIFIER PERMUTATIONS (Non-MEH)
    // ==========================================

    // --- Ctrl + Shift + GUI ---
    constexpr uint8_t L_CTRL_SHIFT_GUI    = L_CTRL | L_SHIFT | L_GUI;
    constexpr uint8_t R_CTRL_SHIFT_GUI    = R_CTRL | R_SHIFT | R_GUI;
    constexpr uint8_t BOTH_CTRL_SHIFT_GUI = BOTH_CTRL | BOTH_SHIFT | BOTH_GUI;

    // --- Ctrl + Alt + GUI ---
    constexpr uint8_t L_CTRL_ALT_GUI      = L_CTRL | L_ALT | L_GUI;
    constexpr uint8_t R_CTRL_ALT_GUI      = R_CTRL | R_ALT | R_GUI;
    constexpr uint8_t BOTH_CTRL_ALT_GUI   = BOTH_CTRL | BOTH_ALT | BOTH_GUI;

    // --- Shift + Alt + GUI ---
    constexpr uint8_t L_SHIFT_ALT_GUI     = L_SHIFT | L_ALT | L_GUI;
    constexpr uint8_t R_SHIFT_ALT_GUI     = R_SHIFT | R_ALT | R_GUI;
    constexpr uint8_t BOTH_SHIFT_ALT_GUI  = BOTH_SHIFT | BOTH_ALT | BOTH_GUI;
}
