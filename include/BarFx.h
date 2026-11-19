#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "Settings.h"

// Autonomous effects of the LED bar (docs/PLAN-lamp-effects.md): they run on the layer pages in the colour of the layer, without the host.
// The ten LEDs are one chain in the shape of a U (measured with the webcam): lamps 0 to 4 run down the right side, lamps 5 to 9 run up the left side.
// The microphone feeds the "vu" effect and the MIC? command.
namespace BarFx {
    inline int rightOf(int row) { return row; }          // row 0..4 from the top
    inline int leftOf(int row) { return 9 - row; }

    // Fills leds[0..9] for the effect at time ms; base is the colour of the layer.
    void render(Settings::Effect e, CRGB* leds, uint32_t ms, CRGB base);

    // Microphone: sampling runs from the effect task while wanted (the "vu" effect, or MIC ON). The speaker and the microphone share the I2S port.
    void micCommand(bool on);                             // MIC ON|OFF over serial: sample without the vu effect
    bool micCommandOn();
    void micWanted(bool on);                              // switches sampling on or off (starts or stops M5.Mic, gives the speaker back when off)
    void micLoop();                                       // call every few milliseconds while wanted: collects a buffer and analyses it
    struct MicStats { bool active; uint16_t level; uint16_t peak; uint16_t freq; uint32_t buffers; };
    MicStats micStats();                                  // RMS and peak (0..32767), the dominant frequency estimated from zero crossings (Hz)
    constexpr int WAVE_SAMPLES = 512;                     // one buffer: 32 ms at 16 kHz
    void micWave(int16_t out[WAVE_SAMPLES]);              // the last buffer, mean removed (for the oscilloscope on the lamps)
    String micStatus();                                   // "MIC active=1 level=... peak=... freq=... buffers=..."
}
