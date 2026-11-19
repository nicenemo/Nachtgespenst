---
seconds: 40
---
# The firmware
- PlatformIO, Arduino framework, C++17
- LVGL 9.2 for the screen, NimBLE-Arduino for Bluetooth, M5Unified, FastLED
- The HID names (usage pages, keys) are generated from the USB HID Usage Tables PDF by a Java program, not typed by hand
- Layers of keys are plain C++ arrays, in the style of the ZMK keyboard firmware
