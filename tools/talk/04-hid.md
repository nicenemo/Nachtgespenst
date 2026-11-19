---
seconds: 40
---
# One device, several HID functions
- Keyboard with LEDs (report 1), consumer control (2), system control (3)
- LampArray, usage page 0x59: lamp attributes and updates (5 to 10)
- Game pad with 384 buttons (11): a data channel
- Mouse (12), only in the Core2 presenter build
- The host needs no driver: all of it is standard HID
