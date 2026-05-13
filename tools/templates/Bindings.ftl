#pragma once
#include "HID_Shortcuts.h"

// --- Modifier Bitmasks ---
#define M_NONE 0x00
#define M_LCTL 0x01
#define M_LSFT 0x02
#define M_LALT 0x04
#define M_LGUI 0x08
#define M_RCTL 0x10
#define M_RSFT 0x20
#define M_RALT 0x40
#define M_RGUI 0x80

// --- Exhaustive Chords (Left) ---
#define M_LCS  (M_LCTL | M_LSFT)
#define M_LCA  (M_LCTL | M_LALT)
#define M_LCG  (M_LCTL | M_LGUI)
#define M_LSA  (M_LSFT | M_LALT)
#define M_LSG  (M_LSFT | M_LGUI)
#define M_LAG  (M_LALT | M_LGUI)
#define M_LMEH (M_LCTL | M_LSFT | M_LALT)
#define M_LHYP (M_LCTL | M_LSFT | M_LALT | M_LGUI)

// --- Exhaustive Chords (Right) ---
#define M_RCS  (M_RCTL | M_RSFT)
#define M_RCA  (M_RCTL | M_RALT)
#define M_RCG  (M_RCTL | M_RGUI)
#define M_RSA  (M_RSFT | M_RALT)
#define M_RSG  (M_RSFT | M_RGUI)
#define M_RAG  (M_RALT | M_RGUI)
#define M_RMEH (M_RCTL | M_RSFT | M_RALT)
#define M_RHYP (M_RCTL | M_RSFT | M_RALT | M_RGUI)

struct Binding {
    HID::Page page;
    HID::Usage usage;
    uint8_t modifier;
    const char* label;
    HID::UsageKind kind;

    // Constructor for generated macros (e.g., cp_MUTE)
    constexpr Binding(HID::Page p, HID::Usage u) 
        : page(p), usage(u), modifier(0), label(nullptr), kind(u.kind) {}

    // Full constructor for manual bindings with labels/modifiers
    constexpr Binding(HID::Page p, HID::Usage u, uint8_t m, const char* l, HID::UsageKind k)
        : page(p), usage(u), modifier(m), label(l), kind(k) {}

    constexpr Binding operator|(uint8_t mod) const {
        return Binding(page, usage, (uint8_t)(modifier | mod), label, kind);
    }
};

// --- Left Wrapper Macros ---
#define lct(b)  ((b) | M_LCTL)
#define lsf(b)  ((b) | M_LSFT)
#define lal(b)  ((b) | M_LALT)
#define lgui(b) ((b) | M_LGUI)
#define lcs(b)  ((b) | M_LCS)
#define lca(b)  ((b) | M_LCA)
#define lcg(b)  ((b) | M_LCG)
#define lsa(b)  ((b) | M_LSA)
#define lsg(b)  ((b) | M_LSG)
#define lag(b)  ((b) | M_LAG)
#define lmeh(b) ((b) | M_LMEH)
#define lhyp(b) ((b) | M_LMEH | M_LGUI)

// --- Right Wrapper Macros ---
#define rct(b)  ((b) | M_RCTL)
#define rsf(b)  ((b) | M_RSFT)
#define ral(b)  ((b) | M_RALT)
#define rgui(b) ((b) | M_RGUI)
#define rcs(b)  ((b) | M_RCS)
#define rca(b)  ((b) | M_RCA)
#define rcg(b)  ((b) | M_RCG)
#define rsa(b)  ((b) | M_RSFT | M_RALT)
#define rsg(b)  ((b) | M_RSFT | M_RGUI)
#define rag(b)  ((b) | M_RALT | M_RGUI)
#define rmeh(b) ((b) | M_RMEH)
#define rhyp(b) ((b) | M_RMEH | M_RGUI)

// Static none binding for empty slots
static constexpr Binding none = Binding(HID::Page::UNDEFINED, {0, HID::UsageKind::OTHER, "NONE"});
