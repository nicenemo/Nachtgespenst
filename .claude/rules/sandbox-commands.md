# Commands, the sandbox and long jobs

Details and symptoms: `docs/wiki/claude-code-workflow.md`, `docs/wiki/troubleshooting.md`. Reasoning: `docs/adr/0004-plain-commands-and-wrapper-scripts.md`.

- Hardware (serial, USB, camera, Bluetooth), `git push|pull|fetch` and `pio ... upload` only work as **one plain command** that matches `sandbox.excludedCommands`. A pipe, `cd`, `&&`, `;`, `VAR=x` prefix, a redirect to a file or `$(...)` keeps the whole call inside the sandbox, where the ports, `~/.ssh` and D-Bus are invisible. Use absolute paths instead of `cd`; put `sleep` and `mkdir` in separate calls.
- `pio` needs no `PLATFORMIO_CORE_DIR` prefix: `core_dir` is set in `platformio.ini`. Call `.pio/venv/bin/pio ...` directly.
- Use the board tools: `jbang tools/Flash.java core2|cores3`, `jbang tools/Serial.java S3|C2 "CMD"...`, `jbang tools/SerialTest.java`, `jbang tools/LedCam.java`. Do not re-invent `stty`/`esptool`/`printf > /dev/tty*` one-liners.
- A failure that says `cannot open /dev/tty...`, `Host key verification failed`, `Failed to connect to system D-Bus`, `No such file or directory` for a port that sysfs lists, or `no Nachtgespenst board found` means the call was sandboxed. Fix the shape of the command first; only then consider `dangerouslyDisableSandbox` for that single call.
- Builds take 11 to 13 minutes: always `run_in_background`, one `pio` build at a time, no edits to `platformio.ini` and no `git switch` while it runs. Never poll or sleep-loop; the harness notifies on exit.
- Check what is on a board before trusting a test: `jbang tools/Serial.java S3 "INFO"` (`fw=` is the build time). Build the branch you flash.
- `.claude/agents`, `skills`, `commands`, `hooks` and `settings*.json` are write-protected in the sandbox. Do not edit `settings.json` on the owner's behalf; list the entries to add.
- zsh: write `"${c}:path"` (not `"$c:path"`), quote globs, avoid `cd`. Error messages are Dutch.
