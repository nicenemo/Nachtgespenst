---
seconds: 50
---
# Data over the game pad buttons
- Host to board: lamp updates. Board to host: 48 bytes per report
- A block: offset, flags, length, up to 38 bytes of data, CRC32
- The host acknowledges with F13 to F24 key presses
- Blocks carry their byte offset, so a repeated block does no harm
- Over USB it works; over Bluetooth it still stalls (open point)
