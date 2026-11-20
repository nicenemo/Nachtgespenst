#pragma once
#include <Arduino.h>

// The Core2 as a presentation clicker and air mouse over Bluetooth (docs/wiki/presenter.md). The clicker types the keys every presenter
// program and every third-party clicker uses (Page Up, Page Down, F5, B, Esc, Ctrl+L). The pointer turns the rotation of the board
// (gyroscope) into mouse movement: it can be switched off, and while the board is held still it falls asleep by itself, until it is moved again.
namespace Presenter {
    enum class Action : uint8_t { Prev, Next, Start, Blank, End, Laser };
    enum class Ptr : uint8_t { Off, Asleep, Active };

    void begin();                          // after HIDManager::begin(): reads the stored settings
    void loop();                           // every loop(): the pointer, and the release of a mouse click
    bool send(Action a);                   // type the key of the action; false when it cannot be sent
    bool click(uint8_t button = 1);        // a mouse click (1 left, 2 right, 4 middle): press now, release a few ms later
    void scroll(int ticks);                // vertical scroll wheel: positive scrolls up (away from you), negative down
    // Sticky modifiers for the mouse: a modifier stays held until it is switched off (Ctrl+click, Ctrl+scroll to zoom, Shift+click). Released at once when the host goes away.
    constexpr uint8_t M_CTRL = 1, M_SHIFT = 2, M_ALT = 4, M_WIN = 8, M_CS = M_CTRL | M_SHIFT, M_AS = M_ALT | M_SHIFT, M_MEH = M_CTRL | M_SHIFT | M_ALT, M_HYPER = M_MEH | M_WIN;
    uint8_t modifiers();                   // the modifiers that are held now (bit mask of M_*)
    void toggleModifiers(uint8_t mask);    // hold the combination, or let it go when it is held
    void releaseModifiers();
    const char* name(Action a);

    Ptr pointer();                         // Off (switched off), Asleep (held still) or Active (moving)
    bool pointerEnabled();
    void setPointerEnabled(bool on);
    void togglePointer();
    const char* pointerName();             // "off", "still" or "moving"
    uint32_t reports();                    // mouse movement reports sent
    uint32_t clicks();
    uint32_t scrolls();

    bool set(const char* key, const char* value);   // gain, idle, hold, flipx, flipy (stored)
    String status();                       // one line of key=value pairs
    void feed(float yawDps, float pitchDps, uint32_t ms);   // test: pretend the board turns at this rate for ms milliseconds
    void feedRaw(float gx, float gy, float gz, uint32_t ms); // test: the same with the three axes of the gyroscope in degrees per second
}
