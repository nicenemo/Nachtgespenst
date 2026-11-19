# 17. Disk drive demo: data channels

* **Status:** Accepted (2026-11-19)

## Context

The LampArray bench ([ADR 0010](0010-lamparray-lighting-and-picture-pages.md)) showed what the lamps can carry in a host-to-board direction (1.5 to 1.9 million lamps per second of intake, 960 reports per second over USB). The owner wanted a demo that moves a file with that channel, in the spirit of a fast loader with many wires: Twenty Thousand Leagues under the Sea (Project Gutenberg #164), compressed, with error detection, acknowledged by function keys, and sent back to the PC.

## Decision

* **Way in (SAVE):** the PC writes blocks into 1,024 lamps (ids 10 to 1,033) with multi updates; the "update complete" flag is the clock edge; every block has a CRC-32. Status lines go back as the keys F13 to F24 (all of them reach the host on Linux over USB and Bluetooth). The file is raw deflate, unpacked with the ROM inflate of the ESP32 and written to the SD card.
* **Way back (LOAD):** a new HID game pad collection (report ID 11) with 384 buttons, 48 bytes per report, 38 bytes of data. The host answers with credits written into the lamps: that is the pushback and the resend request.
* **Variable bit rate** on both ways: block size or payload, window and gap grow after clean stretches and are cut back after errors.
* **SHA-256** on both ends (hardware accelerated on the ESP32 and ESP32-S3): the board computes it and shows it next to the PC's, with the measured speeds and a MATCH or DIFFERENT. The test compares the hash of the received file with the original.
* **Modes:** show (progress, modem-like sounds) and fast (no display, LED or sound during the transfer).
* The reasons for 384 buttons and 1,024 lamps are in [disk-drive](../wiki/disk-drive.md).

## Consequences

* The descriptor grows by a game pad collection, so the Bluetooth pairing has to be renewed once and the PnP version moved to 0x0203 (Core2) and 0x0103 (CoreS3). A desktop may see a game pad with 384 buttons; a udev rule can hide it if that gets in the way.
* The throughput is limited by report counts (about 30 kB/s each way over USB), not by block or report size. Bluetooth is about 30 reports per second from the host side.
* Not covered: a vendor-defined report or USB CDC would be faster; the game pad channel was chosen on request and for the idea of parallel data lines. Force feedback and vibration are open work ([TODO](../TODO.md)).
