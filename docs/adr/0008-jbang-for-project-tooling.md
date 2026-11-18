# 0008 Write project tooling in JBang (Java), not in shell scripts

Status: Accepted. Replaces the bash implementation of the wrapper scripts in [0004](0004-plain-commands-and-wrapper-scripts.md); the rule of 0004 (hardware and network commands are single plain commands) and its intent stand.

## Context
[0004](0004-plain-commands-and-wrapper-scripts.md) introduced `tools/flash.sh`, `tools/serial.sh` and `tools/lib-ports.sh`. They worked, but the project already has its own tool chain: single-file Java programs run with JBang ([0003](0003-jbang-pinned-jdk.md)): `SerialTest.java`, `LedCam.java`, `GenerateHeaders.jsh`, `FetchHut.jsh`. The bash scripts were the odd ones out:
- Port lookup was scraped from sysfs, the 1200-baud touch relied on the side effect of `stty`, and `SerialTest.java` already had the same port logic in Java with a real serial library (jSerialComm).
- Shell quoting, `set -e` subtleties and the zsh/fish login-shell differences that caused several of the earlier mistakes are exactly the constructs a Java tool does not have.
- Two languages for one job, no shared code, no structured parsing of arguments or exit codes.

## Decision
Project tools are JBang single-file Java programs in `tools/`:
- Header: `///usr/bin/env jbang "$0" "$@" ; exit $?`, `//DEPS` (jSerialComm 2.11.0 for serial), `//JAVA 17+` (pinned per script as in 0003), `//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED` (silences the jSerialComm warning) and `//SOURCES` for shared code.
- `tools/Boards.java` is the shared helper (board definitions by USB ID, port lookup, waiting for a port, repository root, the "you are sandboxed" message). It is pulled in with `//SOURCES` and is not run itself.
- `tools/Flash.java` flashes a built image (1200-baud touch through jSerialComm, esptool and git as child processes, stale-image warning, waits for the board); `tools/Serial.java` sends text commands and prints replies; `SerialTest.java` and `LedCam.java` stay as they are.
- Exit codes: 0 done, 1 failed or no reply, 2 usage, 3 the port cannot be opened (sandbox or missing uucp rights).
- One class per tool, file name equals class name, one sandbox exclusion per tool: `*jbang*Flash.java*`, `*jbang*Serial.java*` (and the existing `*jbang*SerialTest*`, `*jbang*LedCam*`).
- No new shell scripts in `tools/`; the three bash scripts are removed.

## Consequences
- One language and one way to run tools (`jbang tools/X.java args`, a plain command by construction); shared code instead of copies; typed argument handling; no `stty` or sysfs parsing.
- The first run of a tool compiles it (a few seconds, cached afterwards) and needs a JDK (JBang downloads it); `jbang` must be on the PATH (it is: `~/jbinst/jbang/bin`).
- The owner has to replace the two bash entries in `sandbox.excludedCommands` by `*jbang*Flash.java*` and `*jbang*Serial.java*` (see [claude-code-workflow](../wiki/claude-code-workflow.md)); until then these tools are refused inside the sandbox with exit code 3 and a clear message.
- esptool stays an external program (it is part of the PlatformIO packages).
- Alternatives rejected: keeping bash (reasons above); Python (the only Python in the project is the PlatformIO venv and `tools/icons/gen_icons.py`, which needs fontTools and has no Java equivalent; a serial library would be a new dependency); Make or just (no serial support, awkward quoting); Kotlin scripts (not used anywhere else).

## Verification (2026-11-18, on the real boards)
- Compile, usage errors (exit 2), missing image (exit 1), `--dry-run` on both boards, stale-image warning, refusal inside the sandbox (exit 3).
- Flash of the CoreS3 (1200-baud touch from Java) and the Core2, each followed by `SerialTest` on both boards: 40 of 40 checks.
- A flash of the CoreS3 starting from the ROM bootloader state (after a deliberate touch), which also exercised `Serial.java`'s "no reply" path (exit 1).
- `Serial.java` against both boards: normal replies, `ERR unknown`, the refused `SHOT`, a bad board name and the sandboxed call.
