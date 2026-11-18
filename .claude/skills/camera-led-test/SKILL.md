---
name: camera-led-test
description: Test the LED bars of the Core2 and CoreS3 with the laptop webcam (find the CoreS3 bar pin, check the colour of every layer, make contact sheets). Use when LED colours, pins or the Locks page lighting change, or when asked to look at the boards through the camera.
---

# LED bars through the webcam

Tool: `jbang tools/LedCam.java` (JBang, Java 27 with the Vector API; run as a single plain command, the camera and serial ports are invisible inside the sandbox).

## Set-up (what worked)
- Both boards in the camera view, bar edge towards the camera, at least 15 cm apart, a white surface in front of each bar, room dim or dark, shutter open.
- First capture one frame to find the boards: `ffmpeg -loglevel error -y -f v4l2 -video_size 1280x720 -i /dev/video0 -frames:v 40 -update 1 FRAME.jpg`, then read the image and choose the crop boxes (`x0,y0,x1,y1`, 8:5 ratio looks best).

## Layer colours
`jbang tools/LedCam.java colours --port /dev/ttyUSB0 --out DIR --crop x0,y0,x1,y1 --clean` (Core2) and `--port /dev/ttyACM0` (CoreS3). It cycles `LAYER 0..25`, films each, writes `DIR/montage.png` and prints a weak numeric verdict. **Judge the montage by eye**: expect distinct hues per layer, dim dark colours, blue-white pale colours (camera white balance). Create `DIR` first in a separate call.

## CoreS3 bar pin
Flash `tools/ledtest` env `cores3` (`pio run -d tools/ledtest -e cores3`, then `jbang tools/Flash.java cores3 --image tools/ledtest/.pio/build/cores3/firmware.bin`), run `jbang tools/LedCam.java pins --port /dev/ttyACM0 --out DIR --size 640x360`, then flash the real firmware again. Result so far: GPIO 5 (M-Bus pin 8), Core2 uses GPIO 25 (same M-Bus pin).

## Always
- Delete the frames when done (they show the room); keep only the contact sheets that go on the site.
- Boards back on layer 0 and the real firmware.
- Details and pitfalls: `docs/wiki/hardware-testing.md`, `docs/wiki/troubleshooting.md` (camera section).
