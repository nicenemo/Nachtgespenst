#pragma once
#include <lvgl.h>
#include "Bindings.h"

// Tile presentation derived from a Binding (see src/Icons.cpp for the lookup table).
// Icons come from a Material Icons subset (Apache-2.0) rendered via LVGL Tiny TTF.
struct TileLook {
    const char* icon;     // UTF-8 icon glyph (IC_* from IconCodes.h) or nullptr
    char text[20];        // main text when there is no icon
    const char* caption;  // optional short caption (only drawn when the tile is tall enough)
    char badge[20];       // modifier hint, e.g. "Ctrl+Shift", "+Shift", "Hyper"; "" if none
};

void icons_init();                                  // create the icon fonts (call after lv_init)
const lv_font_t* icons_font(int px);                // px: 20, 28 or 36
TileLook icons_look(const char* layerTitle, const Binding& b);
