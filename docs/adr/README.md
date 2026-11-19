# Architecture decision records

One file per decision, numbered `NNNN-title.md`, with Status, Context, Decision, Consequences. Do not rewrite an accepted record: add a new one that supersedes it.

| ADR | Title | Status |
|---|---|---|
| [0001](0001-sandbox-claude-code.md) | Run Claude Code in a filesystem and network sandbox | Accepted, pending verification |
| [0002](0002-project-local-platformio.md) | Keep PlatformIO inside the project | Accepted |
| [0003](0003-jbang-pinned-jdk.md) | Pin the JDK per script with `//JAVA` | Accepted |
| [0004](0004-plain-commands-and-wrapper-scripts.md) | Run hardware and network commands as single plain commands, backed by wrapper scripts | Accepted (owner adds the `excludedCommands` entries) |
| [0005](0005-project-site-on-gh-pages.md) | Publish the project site from `site/` through the `gh-pages` branch | Accepted |
| [0006](0006-ui-lvgl-or-classic.md) | LVGL or the classic direct-drawing UI | Accepted (LVGL; classic branch kept) |
| [0007](0007-page-switching-speed-and-prerendering.md) | Page switching speed: coalesce, slim down, then pre-render neighbours | Accepted, in progress |
| [0008](0008-jbang-for-project-tooling.md) | Write project tooling in JBang (Java), not in shell scripts | Accepted |
| [0009](0009-unicode-pages-and-host-os-awareness.md) | Unicode pages, host OS awareness and a settings page | Accepted, implementation parked |
| [0010](0010-lamparray-lighting-and-picture-pages.md) | HID LampArray lighting, a photo and webcam page, and a JBang tool on the PC | Proposed |
| [0011](0011-performance-monitor-and-device-status.md) | PC performance monitor page and a device status page | Proposed |
| [0012](0012-audio-monitor-and-music-visualiser.md) | Audio monitor and music visualiser | Proposed |
| [0013](0013-hid-features-for-core2-and-cores3.md) | Which parts of the HID specification fit the Core2 and the CoreS3 | Proposed |
| [0014](0014-settings-page-and-leds-off.md) | A Settings page, switching the LEDs off, and persistent settings | Proposed |
| [0015](0015-device-identity-names-and-usb-bluetooth-ids.md) | Device identity: short names, own USB ID and a Bluetooth PnP ID | Accepted (test ID) |
| [0016](0016-secret-scanning-with-trufflehog-hooks.md) | Scan for secrets with trufflehog in git hooks | Accepted |
| [0017](0017-disk-drive-data-channels.md) | Disk drive demo: lamps in, game pad buttons back, SHA-256 on both ends | Accepted |
