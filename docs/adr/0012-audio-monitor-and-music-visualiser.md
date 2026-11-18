# 0012 Audio monitor and music visualiser

Status: Proposed. Nothing is implemented. Uses the PC tool and the serial link of [0010](0010-lamparray-lighting-and-picture-pages.md) and [0011](0011-performance-monitor-and-device-status.md); the ledbar rules of 0010 apply.

## Context
The owner asked for an audio monitor for the speakers and the microphone and for a music visualiser. Both boards have what is needed on the device itself: the Core2 has a PDM microphone and a speaker, the CoreS3 has microphones and a speaker, and M5Unified wraps them (`M5.Mic`, `M5.Speaker`). Today `main.cpp` switches the internal microphone and speaker off (no beeps, no listening). The 320 x 240 screen and the ten-LED bar are the output.

## Idea
1. **PC audio monitor.** Levels (left and right) and a spectrum of what the computer plays (the speaker output, captured as loopback) and of the microphone, with peak hold, shown on the screen and on the bar. Optionally the microphone's mute state as a red "you are muted" indication on the bar (useful next to the Teams page).
2. **Music visualiser.** Spectrum bars, VU meters, an oscilloscope trace, a scrolling spectrogram, and beat flashes on the bar, driven by the music playing on the PC.
3. **Standalone sound-reactive mode.** The board listens with its own microphone and visualises the room's sound, with no PC and no cable. Off by default, started explicitly from a page.

## Design
- **PC tool** `tools/Audio.java` (JBang, [ADR 0008](0008-jbang-for-project-tooling.md)): captures a source, runs an FFT in plain Java (no dependency needed for a 1024-point transform), and sends one short line per frame at 30 Hz, for example `AUD l=118 r=96 b=0a1f3c...` (left and right peak, then 32 log-spaced bands from about 30 Hz to 16 kHz as hex digits): about 70 bytes, so about 2 kB/s, trivial for the serial link of either board. A second line type carries 128 down-sampled waveform samples for the oscilloscope. Over a cable first; BLE later.
- **Capture per OS is the hard part.** The microphone is easy everywhere (`javax.sound.sampled`). The speaker output (loopback) is not: Linux works with a PulseAudio or PipeWire monitor source (for example `ffmpeg -f pulse` on `<sink>.monitor`); Windows has WASAPI loopback, which plain Java and ffmpeg cannot do, so it needs a small native helper or a virtual audio device; macOS needs a virtual device such as BlackHole. The first round targets Linux (the owner's control machine) plus the microphone on any OS.
- **Firmware drawing.** The page draws directly, with changed rectangles only (32 bars, a few meters), like the Photo and monitor pages; no widget tree and no page rebuild per frame, so the slow page build of [0007](0007-page-switching-speed-and-prerendering.md) does not matter. Redraw cost is a few hundred microseconds per bar; the panel flush of a full screen (55 ms on the CoreS3, 73 ms on the Core2) is avoided by partial updates. The spectrogram scrolls by shifting the previous image and drawing one new column.
- **LED bar.** The ten bars of the spectrum (or a VU level) on the ten LEDs; beat detection flashes it. Through the LampArray of 0010 once that exists, until then through a serial command.
- **Standalone mode.** `M5.Mic` at 16 kHz, a 512-sample window every 32 ms, an FFT on the ESP32 (esp-dsp or arduinoFFT, a few milliseconds), the same drawing code. The microphone is only started while that page is open, a visible indicator shows that the microphone is active, and no audio leaves the RAM.
- **Backlight.** Like the monitor page: the page keeps the screen on while frames arrive.

## Tests
- A synthetic source (`Audio.java --test-tone 1000`, a sine and a sweep) must light the expected band; `SHOT` compared with the expected bar heights; `SerialTest` checks the `AUD` parser (valid and malformed lines, stale data timeout).
- End to end on the Linux laptop: play a known tone and check the bars; mute the microphone and check the indicator. The webcam test of `LedCam` can check the bar.
- Standalone mode: a tone from a phone speaker next to the board; the visible peak band must match the tone.

## Phasing
1. Standalone visualiser with the device microphone (no PC needed): enables the microphone, FFT, the drawing code, one style.
2. `Audio.java` with the microphone and the `AUD` protocol, PC audio page with levels and spectrum.
3. Speaker loopback on Linux, more styles (VU, oscilloscope, spectrogram), the bar as a meter and beat flash.
4. Windows and macOS loopback (helper or virtual device), mute-state indicator, optional BLE transport.

## Risks and open questions
- Loopback capture on Windows and macOS needs native help or a virtual device; the tool cannot do it with the JDK alone.
- Privacy and trust: the microphone must never be on silently; the standalone page is explicit, and the PC tool only runs when started.
- CPU and memory on the Core2: FFT plus drawing at 30 Hz next to the BLE stack; measure. The Core2's microphone and speaker share an I2S port, which is fine while the speaker stays off.
- A continuous serial stream must not disturb tests and other tools that use the port (only one program can hold it).
- Microphone quality and sensitivity of the small on-board microphones differ per board; the display scale needs a gain setting.

## Decisions needed
1. Both the PC audio monitor and the standalone device-microphone mode, or only the PC side (suggested: both, the standalone mode first because it needs no tool)?
2. Which visual styles first (spectrum bars and VU meters are the cheapest; spectrogram and oscilloscope later)?
3. Is Linux loopback plus microphone on any OS enough for the first round, with Windows and macOS loopback as a follow-up?
4. Should the microphone mute state be shown on the bar?
