# 0011 PC performance monitor page and a device status page

Status: Proposed. Nothing is implemented. Shares the PC tool and the serial picture transport of [0010](0010-lamparray-lighting-and-picture-pages.md) and the page menu of [0009](0009-unicode-pages-and-host-os-awareness.md).

## Context
A macro pad that sits next to the keyboard is a good place for a small system monitor: CPU, memory, disk and network of the computer it is attached to, with the LED bar as a meter. The owner suggested it as another use of the screen and the bar.

## Idea
1. **Host monitor page.** Gauges and sparklines for CPU (total, optionally per core), memory (and swap), disk (used per volume, read and write rate) and network (download and upload rate), coloured by threshold (green, amber, red: software legends, [zmk-comparison](../wiki/zmk-comparison.md)). Optional where the OS provides them: temperatures and GPU load.
2. **Device status page** (no PC needed): battery level and charging state (the Core2 has a battery), free heap and PSRAM, loop rate, uptime, USB and BLE link state, the routing mode, the firmware build, and the host OS detection state of [0009](0009-unicode-pages-and-host-os-awareness.md). It doubles as the on-device diagnostics view that 0009's experiments need.
3. **LED bar as a meter.** Ten LEDs as a level indicator for the chosen metric, colour by threshold. Driven over the LampArray of 0010 once that exists, or by a plain serial command until then.

## Design
- **Data, not pictures.** The PC sends numbers, not images: a short text line about once per second, for example `MON cpu=37 mem=62 swap=3 disk=C:71,D:40 dio=12.3,4.5 net=1.2,0.4 temp=61` (roughly 100 to 150 bytes, easy to read in a terminal), over the existing serial link (CoreS3: USB CDC; Core2: the UART). The firmware keeps a ring buffer of about 60 samples per metric for the sparklines and draws gauges natively. This is far lighter than pushing JPEGs and works on the Core2's slow UART.
- **Rendering.** The page is built once and updated incrementally (a chart point, a bar value); there is no page rebuild per sample, so the slow page build of [0007](0007-page-switching-speed-and-prerendering.md) is not a problem here. With LVGL the `lv_chart`, `lv_bar` and `lv_arc` widgets fit; the classic UI draws lines and rectangles directly.
- **PC tool** `tools/Monitor.java` (JBang, [ADR 0008](0008-jbang-for-project-tooling.md)) using OSHI (`com.github.oshi:oshi-core`), which reads CPU load, memory, swap, disks and their I/O, network interfaces and, where available, sensors on Windows, Linux and macOS. Options: `--port`, `--interval`, thresholds, which interface and volumes. It needs its own `*jbang*Monitor*` sandbox exclusion because it opens the serial port.
- **The tool tells the board which OS it runs on.** When the tool runs on the computer the board is cabled to, it can send `OS <name>` as well. That is the most reliable host OS signal for 0009 (it even reports the guest OS if the tool runs inside a virtual machine, which is the OS that interprets the keys), and it complements the USB and BLE detection.
- **Thresholds and alerts.** Above a configurable level the metric's tile turns amber or red and the bar can pulse; no sound, no popups.
- **Backlight.** Serial data does not count as activity today, so the screen would dim after a minute even while it shows live data. Proposal: the monitor page keeps the backlight on while samples arrive and dims normally after they stop.

## btop-style views, threads and sensors
The host monitor becomes a small set of views, switched by tapping the left or right edge (CoreS3) or with buttons A and C (Core2), like the page switching of the keys.

| View | Content |
|---|---|
| Overview | the gauges and sparklines above, plus the hottest temperature and the fan speed |
| CPU threads | one mini graph per logical CPU (a 60-sample sparkline with a green, amber, red gradient and the current percentage); up to 16 threads as a 4 x 4 grid of tiles, more as thin bars; frequency per thread where the OS reports it |
| Processes | the top 7 or 8 by CPU or by memory (name, percentage, memory), a tap on the header toggles the sort order, a tap on a row shows that process's threads (OSHI exposes thread details) |
| Sensors | list of temperatures and fan speeds with their labels, coloured by threshold; fans as rpm and a small bar |
| Disk and network | per volume used and rates, per interface rates, sparklines |

- **Data volume.** Per-thread loads are at most 64 numbers, the process list about 320 bytes, sensors about 200 bytes, sent at 0.5 to 1 Hz as extra line types (`CPUS`, `PROC`, `SENS`) beside `MON`: well under 1 kB/s, fine for the Core2's UART. The firmware keeps the 60-sample ring buffers; names are truncated to the tile width.
- **Sensor sources.** Linux: `/sys/class/hwmon/*/temp*_input` (millidegrees) and `fan*_input` (rpm) with the `name` and `*_label` files, readable without root. The owner's Linux laptop exposes `thinkpad` (8 temperatures and one fan, 3940 rpm when probed), `coretemp` (3), `acpitz`, `pch_wildcat_point` and `iwlwifi_1` (1 each), and has 4 logical CPUs, so the thread view would show 4 tiles there. Windows: OSHI returns zeros for most sensors, so real values need LibreHardwareMonitor (its web server JSON or WMI) running next to the tool; optional. macOS: not in the first round. Whatever is missing is simply not shown.
- **Thresholds.** Per sensor class (CPU 75 and 90 degrees for amber and red, fans only shown), configurable in the tool; a red sensor can pulse the LED bar.
- **Testing.** `Monitor.java --fake` produces 4, 16 and 64 threads, a missing sensor set and an overheating sensor so every layout can be compared with `SHOT`; the real values are checked against `sensors` or the hwmon files on the Linux laptop.

## Tests
- `Monitor.java --fake` sends deterministic synthetic samples so `SHOT` can be compared with an expected picture; `SerialTest` checks the `MON` parser (valid and malformed lines, missing fields, stale data timeout) and the status page.
- End to end on the Linux laptop: a CPU burner raises the CPU gauge, a large `curl` download raises the network gauge, a file copy raises the disk rate. The webcam test of `LedCam` can check the bar meter.

## Phasing
1. Device status page (standalone, also useful for 0009).
2. `MON` protocol, host monitor page with CPU and memory, `Monitor.java` with OSHI.
3. Disk and network, sparklines, thresholds.
3b. CPU threads, processes and sensors views (Linux hwmon first, Windows through LibreHardwareMonitor later).
4. The LED bar as a meter (needs a serial `LEDS` command or the LampArray of 0010).
5. `OS` reporting from the tool; optional BLE transport so it also works without a cable.

## Risks and open questions
- Sensors (temperatures, GPU) differ per OS and are often missing in Java (especially on Windows); treat them as optional.
- OSHI adds a dependency with native access (JNA); the JBang header needs the usual native-access option, and the first run downloads it.
- The tool has to keep running; starting it with the session is the owner's choice (a login entry), not part of the repository.
- A continuous serial stream must not disturb tests and other tools that use the port (only one program can hold it); the tool should release the port on a signal.
- Metrics of a remote or virtual machine are not available unless the tool runs there.

## Decisions needed
1. Host monitor, device status page, or both (suggested: both, status page first)?
2. Which metrics matter beyond CPU, memory, disk and network (temperatures, GPU, per-core view)?
3. Numbers drawn on the device as proposed (not images)?
4. Should the bar act as the meter, and is the backlight rule acceptable?
5. Are the btop-style views (threads, processes, sensors) wanted from the start, or after the basic gauges? Is Linux hwmon enough for sensors in the first round?
