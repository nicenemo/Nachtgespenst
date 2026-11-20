#pragma once
#include <lvgl.h>

// The console look of the device: black background, rounded blocks in amber, peach, lavender and coral, Geist Sans (bold) for
// headings and buttons and Geist Mono for live data. The same palette as the demo window (tools/ConsoleTheme.java); see docs/wiki/design-system.md.
namespace Console {
constexpr uint32_t INK = 0x000000, AMBER = 0xFF9900, PEACH = 0xFFCC99, LAVENDER = 0x9999FF, CORAL = 0xCC6666, DIM = 0x2A2A44;
constexpr int RADIUS = 10;      // blocks; the demo window uses 14 on a screen that is about three times as large
constexpr int PILL = 12;        // bars (half their height)

void begin();                                   // creates the fonts and the styles; call once after lv_init()
const lv_font_t* sans(int px);                  // Geist Sans Bold at 12, 16, 20 or 28 px (nearest), with Montserrat as the fallback for missing glyphs
const lv_font_t* mono(int px);                  // Geist Mono at 12 or 14 px
void block(lv_obj_t* o, uint32_t colour);       // rounded block: colour, pressed = amber with black text, focused = peach ring
void pill(lv_obj_t* o, uint32_t colour);        // the same with a full radius, for the bars of the header
}
