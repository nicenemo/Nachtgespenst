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

const uint8_t COLOR_COUNT = sizeof(LAYER_COLORS) / sizeof(LAYER_COLORS[0]);
