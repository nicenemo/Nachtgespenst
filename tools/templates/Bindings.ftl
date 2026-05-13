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

// --- Symmetric Wrappers ---
#define M_BCTL (M_LCTL | M_RCTL)
#define M_BSFT (M_LSFT | M_RSFT)
#define M_BALT (M_LALT | M_RALT)
#define M_BGUI (M_LGUI | M_RGUI)

struct Binding {
    HID::Page page;
    HID::Usage usage;
    uint8_t modifier;
    const char* label;
    HID::UsageKind kind;

    constexpr Binding() 
        : page((HID::Page)0), usage({0, HID::UsageKind::OTHER, " "}), 
          modifier(M_NONE), label(nullptr), kind(HID::UsageKind::OTHER) {}

    constexpr Binding(HID::Page p, HID::Usage u, uint8_t m = M_NONE, 
                      const char* l = nullptr, HID::UsageKind k = HID::UsageKind::OTHER)
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
#define lhyp(b) ((b) | M_LHYP)

// --- Right Wrapper Macros ---
#define rct(b)  ((b) | M_RCTL)
#define rsf(b)  ((b) | M_RSFT)
#define ral(b)  ((b) | M_RALT)
#define rgui(b) ((b) | M_RGUI)
#define rcs(b)  ((b) | M_RCS)
#define rca(b)  ((b) | M_RCA)
#define rcg(b)  ((b) | M_RCG)
#define rsa(b)  ((b) | M_RSA)
#define rsg(b)  ((b) | M_RSG)
#define rag(b)  ((b) | M_RAG)
#define rmeh(b) ((b) | M_RMEH)
#define rhyp(b) ((b) | M_RHYP)

// --- Both Hand Macros ---
#define bct(b)  ((b) | M_BCTL)
#define bsf(b)  ((b) | M_BSFT)
#define bal(b)  ((b) | M_BALT)
#define bgui(b) ((b) | M_BGUI)

#define none Binding()

// Fixed: Added base helper macros
#define kp(u, ...) Binding(HID::Page::Keyboard, u, M_NONE, ##__VA_ARGS__)
#define cp(u, ...) Binding(HID::Page::Consumer, u, M_NONE, ##__VA_ARGS__)
#define gp(u, ...) Binding(HID::Page::GenericDesktop, u, M_NONE, ##__VA_ARGS__)

// --- Shortcut Includes ---
<#list pages as page>
#include "hid_shortcuts/${page.filename}"
</#list>
