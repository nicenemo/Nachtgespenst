---
seconds: 45
---
# LampArray: the host paints the lights
- Lamps 0 to 9 are the LED bar, lamp 10 and up a grid on the screen
- Grid up to 320 x 190 lamps, kept in PSRAM
- The PC computes every picture and sends lamp updates
- The board only shows what it receives
- Windows supports this usage page itself (Dynamic Lighting); here our own PC program drives it
