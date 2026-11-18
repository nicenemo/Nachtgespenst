# 0002 Keep PlatformIO inside the project

Status: Accepted. The `PLATFORMIO_CORE_DIR` environment variable is replaced by `core_dir` in `platformio.ini` (see [0004](0004-plain-commands-and-wrapper-scripts.md)).

## Context
PlatformIO is not installed system-wide. A home-directory install (`~/.platformio`) would need sandbox exemptions, and `/tmp` is a small tmpfs.

## Decision
Install PlatformIO in a venv at `.pio/venv` and set `PLATFORMIO_CORE_DIR=$PWD/.pio/core`. `.pio/` is gitignored.

## Consequences
- No home-directory exemptions are needed for builds.
- Commands must set `PLATFORMIO_CORE_DIR` and use `.pio/venv/bin/pio`. See [tooling](../wiki/tooling.md).
- Two builds must not run at once in the same `.pio`.
