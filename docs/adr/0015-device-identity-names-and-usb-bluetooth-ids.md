# 0015 Device identity: short names, own USB ID and a Bluetooth PnP ID

Status: Accepted 2026-11-18 with a test ID. Replace the test ID before anything is released or shared.

## Context
Windows identifies and caches devices by their IDs, and composite devices (several functions behind one USB device, or several top-level HID collections behind one HID interface) are the least forgiving. The firmware had three weaknesses:
- **Bluetooth:** no PnP ID in the Device Information Service, which HID over GATT requires. The host saw vendor 0000, product 0000 for both boards, so a Windows or BlueZ cache could not tell the boards or a changed report map apart.
- **USB:** the CoreS3 used Espressif's shared `303a:1001`, the same ID as every ESP32-S3 in its ROM bootloader or debug mode, so driver and COM port associations could mix up.
- **Names:** `Nachtgespenst-S3` and `-C2`, manufacturer `BKA - KDA` (with spaces). The 31-byte advertising payload was 29 bytes full.

## Decision
- **Names:** device name `NG-S3` and `NG-C2` (USB product, `INFO name=`, HID names), manufacturer `Nachtgespenst`; ASCII only, no spaces. The Bluetooth name is `🌙👻-cs3` (CoreS3) and `🌙👻-c2` (Core2), the owner's choice, written as UTF-8 bytes in `K_BLE_NAME` (`Config.h`). A moon and a ghost are 4 bytes each in UTF-8, so the advertising payload still has room. Emoji stay out of the USB strings (UTF-16 surrogates, Windows port and HID names). Hosts that cannot render emoji show boxes; if a host refuses to pair, fall back to the ASCII name.
- **IDs:** the pid.codes test ID **1209:0001** for the USB device of the CoreS3 and as the Bluetooth PnP ID (vendor ID source 0x02, "USB Implementers Forum") of both boards. It is a shared test ID: fine on the owner's machines, not for releases. The PnP version is `0xBBRR`, board in the high byte (1 = CoreS3, 2 = Core2) and the report map revision in the low byte; raise the low byte whenever the report map changes so hosts re-read it (to be verified).
- **USB serial number:** derived from the chip's MAC address (`__MAC__`) instead of a fixed string, so a second board is a second device to Windows.
- **Own variant folder:** the stock CoreS3 variant (`pins_arduino.h`) hard-codes the Espressif ID and cannot be overridden by a build flag. `variants/m5stack_cores3/pins_arduino.h` is a copy with the two IDs changed, selected with `board_build.variants_dir`. Compare it with the framework's header when the framework is updated.
- **Tools:** the CoreS3 has two IDs now: the application (1209:0001) and the ROM bootloader (303a:1001, also the ID of firmware older than this change). `Boards.java` knows both; `Flash.java` waits for the bootloader after the 1200-baud touch and for the application ID after flashing.
- **udev:** one rule matches the kernel's HID device name `000[35]:1209:0001.*` (USB and Bluetooth), so no other Bluetooth keyboard or mouse is opened up. Ansible playbooks: `udev-setup.yml` and `udev-teardown.yml` (kept in the owner's kos project).

## Consequences
- Every bonded host must remove and pair the boards again (the PnP ID and the names changed, and so did the report map); on Windows also delete the old device entries.
- A real vendor and product ID is still to be arranged: request PIDs from Espressif's open-source PID list (`github.com/espressif/usb-pids`, VID 0x303A) or from pid.codes; one per board model is cleaner. The PnP version distinguishes the boards until then.
- Windows behaviour with the composite device (serial port plus HID with three top-level collections) is not tested yet; the owner's Windows machine is the test.
