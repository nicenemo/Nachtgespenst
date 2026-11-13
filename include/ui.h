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
bool ui_press(int layer, int idx);       // activate tile idx of page (sends HID), no touch needed
void ui_tap(int x, int y);               // inject a ~120 ms touch at screen coords
String ui_state();                       // one-line "key=value" state summary
void ui_screenshot(Stream& out);         // "SHOT w h\n" + RGB888 rows + "\nEND\n"
