---
seconds: 30
---
# Is the file intact?
- CRC32 per block catches transmission errors
- SHA-256 over the whole file, computed on the board and on the PC
- The board shows both hashes and whether they match
- Mismatch: the screen says so and the bar turns red
