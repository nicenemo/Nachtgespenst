#pragma once
#include <Arduino.h>

// LVGL based UI. Pages 0..LAYER_COUNT-1 are the Keymap layers,
// page LAYER_COUNT is the "Locks" page (Caps/Num/Scroll toggle tiles).
// All functions are safe to call from loop() (single threaded, no LVGL locking).

void ui_begin();                         // after M5.begin() and HIDManager::begin()
uint32_t ui_loop();                      // call every loop(): LVGL tick + host LED sync; returns ms the caller may sleep
uint8_t ui_page_count();                 // LAYER_COUNT + 1
uint8_t ui_locks_page();                 // index of the Locks page
int ui_layer();                          // current page
void ui_toggle_locks();                  // Locks page <-> previous page
void ui_set_layer(int n);                // switch page (wraps around)
bool ui_press(int layer, int idx, bool fromSerial = false);   // activate tile idx of page (sends HID), no touch needed; from the serial port Power Down and Sleep are refused
bool ui_tap(int x, int y);               // inject a ~120 ms touch at screen coords; refused (false) outside the Locks page, so a test cannot hit Power Down or Sleep
bool ui_touch(int x, int y, bool down); // hold a touch (down) until it is released (up, x and y ignored); Locks page or header only; the release cancels the click
bool ui_button(int i, bool down);        // Core2 button A, B, C = 0..2: down runs its action and lights its indicator, up puts it out; false on the CoreS3
String ui_state();                       // one-line "key=value" state summary
void ui_wake();                          // restart the idle timers: backlight and LED bar come back, BL override cleared
void ui_backlight(int level);            // fixed backlight level 0..255, -1 = automatic
void ui_bar_colors(uint32_t out[10]);  // the colours the ten bar LEDs were last set to (0xRRGGBB, before the global brightness)
void ui_leds_refresh();                  // recompute the LED bar (after a lamp reset or a brightness change)
String ui_perf();                        // "PERF build_ms=.. render_ms=.. flush_ms=.. strips=.. page=..": timing of the last page
void ui_flush();                         // build a page that is still waiting (its set-up may print log lines): call before the reply of SHOT starts
void ui_screenshot(Stream& out);         // "SHOT w h\n" + RGB888 rows + "\nEND\n"
