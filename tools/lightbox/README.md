# Lightbox

Two static pages for the webcam tests, shown full screen on the laptop:

- `white.html`: lights the boards for a photo (the screen is the lamp).
- `black.html`: no light from the laptop screen while the LED bars are filmed.

`jbang tools/Light.java white|black|off` opens them in a Firefox kiosk window with a throw-away profile (and closes the window again with `off`). See [hardware-testing](../../docs/wiki/hardware-testing.md).
