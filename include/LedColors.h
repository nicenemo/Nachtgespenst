#pragma once
#include <FastLED.h>

const CRGB LAYER_COLORS[] = {
    CRGB::Blue,         CRGB::Red,          CRGB::Orange,       CRGB::Green,
    CRGB::Purple,       CRGB::Cyan,         CRGB::Yellow,       CRGB::White,
    CRGB::Magenta,      CRGB::Lime,
    CRGB::Maroon,       CRGB::Navy,         CRGB::Olive,        CRGB::Teal,
    CRGB::DeepPink,     CRGB::Gold,         CRGB::Indigo,       CRGB::DarkGreen,
    CRGB::DarkOrange,   CRGB::DarkViolet,
    CRGB::SkyBlue,      CRGB::LightCoral,   CRGB::PaleGreen,    CRGB::Khaki,
    CRGB::Lavender,     CRGB::MistyRose,    CRGB::Aquamarine,   CRGB::PeachPuff,
    CRGB::PowderBlue,   CRGB::LemonChiffon,
    CRGB::Tomato,       CRGB::Salmon,       CRGB::IndianRed,    CRGB::Chocolate,
    CRGB::SaddleBrown,  CRGB::Sienna,       CRGB::Peru,         CRGB::BurlyWood,
    CRGB::Tan,          CRGB::RosyBrown,
    CRGB::CadetBlue,    CRGB::SteelBlue,    CRGB::DodgerBlue,   CRGB::RoyalBlue,
    CRGB::MediumSlateBlue, CRGB::SlateBlue, CRGB::MediumBlue,   CRGB::MidnightBlue,
    CRGB::DarkSlateBlue, CRGB::CornflowerBlue,
    CRGB::ForestGreen,  CRGB::SeaGreen,     CRGB::MediumSeaGreen, CRGB::SpringGreen,
    CRGB::MediumSpringGreen, CRGB::LawnGreen, CRGB::Chartreuse,  CRGB::GreenYellow,
    CRGB::YellowGreen,  CRGB::DarkOliveGreen,
    CRGB::Orchid,       CRGB::MediumOrchid, CRGB::DarkOrchid,   CRGB::Violet,
    CRGB::Plum,         CRGB::Thistle,      CRGB::HotPink,      CRGB::LightPink,
    CRGB::DeepPink,     CRGB::MediumVioletRed,
    CRGB::DarkCyan,     CRGB::LightSeaGreen, CRGB::DarkTurquoise, CRGB::Turquoise,
    CRGB::MediumTurquoise, CRGB::PaleTurquoise, CRGB::DarkSlateGrey, CRGB::LightCyan,
    CRGB::Azure,        CRGB::AliceBlue,
    CRGB::OrangeRed,    CRGB::DarkGoldenrod, CRGB::Goldenrod,    CRGB::PaleGoldenrod,
    CRGB::DarkKhaki,    CRGB::Wheat,        CRGB::NavajoWhite,  CRGB::AntiqueWhite,
    CRGB::BlanchedAlmond, CRGB::PapayaWhip,
    CRGB::LightGrey,    CRGB::Silver,       CRGB::DarkGrey,     CRGB::Grey,
    CRGB::SlateGrey,    CRGB::LightSlateGrey, CRGB::Black,      CRGB::Snow,
    CRGB::FloralWhite,  CRGB::OldLace
};

// ---------- Lock indicator colours (Core2 LED bar + Locks page) ----------
enum LockId : uint8_t { LK_CAPS = 0, LK_NUM = 1, LK_SCROLL = 2, LK_NONE = 255 };
// activeMask bits: caps=1, num=2, scroll=4 (not the HID LED bits)
const CRGB LOCK_COLORS[3] = { CRGB(255, 80, 0), CRGB(0, 220, 60), CRGB(40, 80, 255) };

// Additive mix of n colours (saturating)
inline CRGB mixColors(const CRGB* c, uint8_t n) {
    CRGB out(0, 0, 0);
    for (uint8_t i = 0; i < n; i++) {
        out.r = qadd8(out.r, c[i].r); out.g = qadd8(out.g, c[i].g); out.b = qadd8(out.b, c[i].b);
    }
    return out;
}

// Bar in segments 4/3/3 (caps/num/scroll). One active lock fills its segment; with several active
// the colours add up over the whole bar. Nothing active: faint white.
inline void ledBarLocks(CRGB* leds, uint8_t count, uint8_t activeMask) {
    CRGB act[3]; uint8_t n = 0;
    for (uint8_t i = 0; i < 3; i++) if (activeMask & (1u << i)) act[n++] = LOCK_COLORS[i];
    if (n == 0) { fill_solid(leds, count, CRGB(6, 6, 6)); return; }
    if (n > 1)  { fill_solid(leds, count, mixColors(act, n)); return; }
    fill_solid(leds, count, CRGB::Black);
    uint8_t seg = (activeMask & 1) ? 0 : (activeMask & 2) ? 1 : 2;
    static const uint8_t from[3] = {0, 4, 7}, to[3] = {4, 7, 10};
    for (uint8_t i = from[seg]; i < to[seg] && i < count; i++) leds[i] = act[0];
}
