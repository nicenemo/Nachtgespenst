#pragma once
#include <Arduino.h>
#include <FastLED.h>

// HID LampArray test bench (ADR 0010, minimum viable test).
// One LampArray with BAR_LAMPS lamps for the LED bar (ids 0..9) followed by a grid of screen lamps
// (ids 10..), the grid size can be changed at run time (serial "LAMPS GRID c r") to see how far a host
// can go. The lamps only show on the two lamp pages; the other pages keep their own colours.
namespace Lamps {
    constexpr uint16_t BAR_LAMPS = 10;

    void begin();                                   // allocate lamp storage, default grid 16 x 12
    uint16_t count();                               // BAR_LAMPS + cols * rows
    uint16_t maxGridLamps();                        // largest grid this board can hold (PSRAM dependent)
    bool setGrid(uint16_t cols, uint16_t rows);     // false when too big; clears all screen lamps
    uint16_t cols();
    uint16_t rows();

    // Report handlers; the payload excludes the report ID. Safe to call from the USB and BLE tasks.
    void onOutput(uint8_t id, const uint8_t* d, uint16_t len);       // multi update, range update, control
    void onSetFeature(uint8_t id, const uint8_t* d, uint16_t len);   // attributes request
    uint16_t onGetFeature(uint8_t id, uint8_t* out, uint16_t maxLen); // attributes, attributes response; returns bytes

    bool autonomous();                              // host has not taken control (default true)
    CRGB barColor(uint8_t i);                       // lamp i (0..9) with its intensity applied
    bool barChanged();                              // main-loop path: true once after a frame touched the bar lamps (only while the worker is off)
    void barShown();                                // the main loop has shown the bar: records the latency of that frame
    void setBarSink(void (*fn)());                  // what the worker task calls to put lamps 0..9 on the LED bar (thread-safe, set by ui.cpp)
    void setWorker(bool on);                        // LAMPS WORKER ON|OFF: bar updates on their own task (the other core) or in the main loop
    bool workerOn();
    bool pending();                                 // a screen frame is waiting to be drawn: the main loop should not sleep
    void getLamp(uint16_t id, uint8_t rgbi[4]);
    const uint8_t* raw(uint16_t id);                // the four bytes r, g, b, intensity of lamp id and of the lamps after it (contiguous), or null
    void setFrameSink(void (*fn)());                // called from the HID task whenever a frame (update-complete report) has been stored

    void resetCounters();                           // also clears all lamp colours
    String status();                                // "LAMPS key=value ..." for the serial command
    void setBarBrightness(uint8_t level);           // FastLED global brightness for the bar (clamped)
    uint8_t barBrightness();

    // Pages (called from ui.cpp, main loop only). Rectangles are the free body area on the screen.
    enum class Page : uint8_t { None, Screen, Bar };
    void enterPage(Page p, int x, int y, int w, int h);  // full redraw from the stored lamps
    void leavePage();
    void tick();                                    // draw what changed since the last call (only while a lamp page is active)
}
