# Tooling

## JBang, SDKMAN and Java
- Get JBang with SDKMAN: `sdk install jbang`. Here it currently lives in `~/jbinst`.
- JBang downloads the JDK named by `//JAVA <n>` in each `tools/*.jsh` into `~/.jbang/cache/jdks`. No manual Java install is needed.
- To bump Java, edit that line in `tools/GenerateHeaders.jsh` and `tools/FetchHut.jsh`. List versions with `jbang jdk list --available`. `-ea` means not released.
- Run the scripts from inside `tools/`. Output goes to `tools/include/`; copy it with `cp -r tools/include/. include/`.
- `FetchHut.jsh` downloads the HUT PDF to `tools/data/hut_spec.pdf` (gitignored; delete it to force a re-download) and extracts `tools/data/hut.json`.

## PlatformIO
- Project-local, see [ADR 0002](../adr/0002-project-local-platformio.md). Recreate with `python3 -m venv .pio/venv && .pio/venv/bin/pip install platformio`.
- Invoke as `.pio/venv/bin/pio ...`. The core directory is set by `core_dir` in `platformio.ini` (an environment prefix such as `PLATFORMIO_CORE_DIR=...` would keep the command inside the Claude Code sandbox, see [claude-code-workflow](claude-code-workflow.md)).

## Sandbox
- Reasoning in [ADR 0001](../adr/0001-sandbox-claude-code.md); actual lists in `.claude/settings.json`; test plan in [SANDBOX-TEST](../SANDBOX-TEST.md).
- Hosts used: `dl.registry.platformio.org`, `registry.platformio.org`, `github.com` (PlatformIO); `pypi.org`, `files.pythonhosted.org` (venv); `repo.maven.apache.org`, `api.foojay.io` (jbang); `www.usb.org` (HUT PDF).

## Project tools (JBang)
Decision: [ADR 0008](../adr/0008-jbang-for-project-tooling.md). Every tool is a single-file Java program started as `jbang tools/<Name>.java args` (a plain command by construction; `jbang` is on the PATH).

| Tool | Purpose |
|---|---|
| `Flash.java` | flash a built image to a board and wait for it (`core2\|cores3 [--image F] [--dry-run]`) |
| `Serial.java` | send text commands to a board and print the replies (`S3\|C2 "CMD" ...`) |
| `SerialTest.java` | functional tests, `--shot DIR` screenshots, `--latency` page timing |
| `LedCam.java` | webcam LED tests (`pins`, `colours`) |
| `Boards.java` | shared helper (board USB IDs, port lookup); not run directly |
| `GenerateHeaders.jsh`, `FetchHut.jsh` | HID header generator and HUT download |

Exit codes of `Flash.java` and `Serial.java`: 0 done, 1 failed or no reply, 2 usage, 3 the port cannot be opened (inside the sandbox, or no uucp rights).

Adding a tool:
1. One class per file, file name equals class name, first line `///usr/bin/env jbang "$0" "$@" ; exit $?`.
2. `//DEPS com.fazecast:jSerialComm:2.11.0` for serial access, `//SOURCES Boards.java` for the shared helper, `//JAVA 17+` (pin it, [ADR 0003](../adr/0003-jbang-pinned-jdk.md)), `//JAVA_OPTIONS --enable-native-access=ALL-UNNAMED` (silences the jSerialComm warning).
3. Child programs (esptool, git) through `ProcessBuilder`; serial ports through jSerialComm, found by USB ID with `Boards.find`.
4. If it touches hardware, the network or the camera, add one `*jbang*<Name>*` entry to `sandbox.excludedCommands` (owner edits `.claude/settings.json`) and make it exit with code 3 and a clear message when the port cannot be opened.
5. Compile check: `jbang build tools/<Name>.java`; then test it on the boards.

