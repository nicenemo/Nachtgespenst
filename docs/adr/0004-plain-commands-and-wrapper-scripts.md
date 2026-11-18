# 0004 Run hardware and network commands as single plain commands, backed by wrapper scripts

Status: Accepted. The wrapper scripts described below were first written in bash and then replaced by JBang tools (`Flash.java`, `Serial.java`, `Boards.java`) by [0008](0008-jbang-for-project-tooling.md); the rule and the intent stand. The owner still has to add the new `excludedCommands` entries (see Consequences and 0008). Supersedes the `PLATFORMIO_CORE_DIR` clause of [0002](0002-project-local-platformio.md).

## Context
[0001](0001-sandbox-claude-code.md) runs Claude Code in a sandbox that hides serial ports, the camera, input devices, `~/.ssh` and D-Bus. A list of `excludedCommands` runs outside it, but a call is excluded only when every command in it matches an entry. Across the first long session the same mistakes were made again and again, each costing minutes and sometimes a 10-minute build:

- A pipe, `cd`, `&&`, `;`, a `VAR=x` prefix or a redirect in front of or after an excluded command kept it sandboxed (`git push | tail`, `PLATFORMIO_CORE_DIR=... pio ...`, `S=...; jbang ...`). Symptoms looked like hardware faults (`cannot open /dev/ttyACM0`, `Host key verification failed`, `no Nachtgespenst board found`).
- A plain `pio upload` on the CoreS3 (TinyUSB, no auto-reset) failed after a full rebuild.
- Builds were started while `platformio.ini` was being edited or another build ran, which cleans the build folder and kills the build.
- A stale `firmware.bin` from another branch was flashed, so tests ran against the wrong UI.
- zsh quirks (`"$c:path"`, unmatched globs, a persistent `cd`), sandbox placeholder files staged by accident, Pages URLs in the wrong case.

## Decision
1. **Rule:** hardware, camera, Bluetooth, `git push|pull|fetch` and upload commands are issued as one plain command: no pipe, `cd`, `&&`, `;`, `VAR=x` prefix, redirect or `$(...)`. Absolute paths replace `cd`; `sleep` and `mkdir` get their own calls.
2. **PlatformIO needs no environment prefix:** `[platformio] core_dir = ${PROJECT_DIR}/.pio/core` in `platformio.ini`; `.pio/venv/bin/pio ...` is called directly.
3. **Wrapper scripts** make the fragile sequences one command:
   - `tools/flash.sh core2|cores3 [--image F] [--dry-run]`: finds the board by USB ID, does the CoreS3 1200-baud touch, flashes the application only, waits for the board, warns about stale images and about running inside the sandbox.
   - `tools/serial.sh S3|C2 "CMD" ...`: sends text commands and prints the replies.
   - `tools/lib-ports.sh`: shared port lookup (sysfs is visible in the sandbox, `/dev` nodes are not, which is how the sandbox is detected).
   - `jbang tools/SerialTest.java --latency` and `tools/LedCam.java` replace throw-away scripts.
4. **Knowledge is stored where it is read:** rules in `.claude/rules/`, procedures in `.claude/skills/` (`flash-and-test`, `camera-led-test`, `publish-site`), roles in `.claude/agents/` (`hardware-tester`, `firmware-builder`), explanations in `docs/wiki/` ([claude-code-workflow](../wiki/claude-code-workflow.md), [hardware-testing](../wiki/hardware-testing.md), [troubleshooting](../wiki/troubleshooting.md)), and a short summary in `CLAUDE.md`.

## Consequences
- Fewer repeated errors and one documented way to flash and test; the scripts print what is wrong when they are sandboxed.
- The owner must add `tools/flash.sh*`, `tools/serial.sh*` and `*jbang*LedCam*` to `sandbox.excludedCommands` in `.claude/settings.json` (it is write-protected inside the sandbox). Until then these commands run with the sandbox disabled for that single call and pass through the normal permission prompt.
- `.claude/agents`, `skills`, `commands`, `hooks` and `settings*.json` are write-protected in the sandbox; changing them needs an unsandboxed step.
- Editing `platformio.ini` (including this change) invalidates the build once.
- Alternatives rejected: disabling the sandbox for hardware sessions (loses the protection the sandbox exists for); allowing `/dev/tty*` inside the sandbox (does not cover `~/.ssh`, D-Bus or the camera and widens the sandbox); a hook that rejects bad command shapes (a good later addition, but hooks are protected paths and the rules already cover it).
