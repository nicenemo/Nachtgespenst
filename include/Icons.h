#pragma once
#include "Bindings.h"

// Tile presentation derived from a Binding (see src/Icons.cpp for the lookup table).
// The classic UI has no icon font: it draws arrows as shapes and shows the caption (or text) of every
// other tile. `icon` keeps the IC_* glyph string from IconCodes.h so the table stays shared with the
// LVGL UI on the other branches.
struct TileLook {
    const char* icon;     // IC_* glyph string (IconCodes.h) or nullptr
    char text[20];        // main text when there is no icon
    const char* caption;  // short caption, shown instead of the icon
    char badge[20];       // modifier hint, e.g. "Ctrl+Shift", "+Shift", "Hyper"; "" if none
};

TileLook icons_look(const char* layerTitle, const Binding& b);
