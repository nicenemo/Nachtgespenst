#pragma once
#include <Arduino.h>

// Telemetry for the demo window over the game pad (HID report 11, the same 48-byte layout as the Disk demo): what the board shows or hears, as data the
// PC reads from the HID node instead of the serial port. TELEM BAR|WAVE|BOTH|OFF over serial only switches it on.
//   flags 0x04: the ten bar LEDs, 10 x (r, g, b) in the payload; offset counts the reports.
//   flags 0x08: a chunk of one microphone frame (offset = byte offset in the frame, flag 0x01 on the last chunk); the frame is a u16 divisor
//               followed by 512 signed bytes: the samples of the last 32 ms buffer divided by the divisor.
namespace Telem {
    enum Mode : uint8_t { Off = 0, Bar = 1, Wave = 2, Both = 3 };
    void set(uint8_t mode);
    uint8_t mode();
    bool wantsMic();                // the wave telemetry needs the microphone
    void loop();                    // from the effect task: sends what is due
}
