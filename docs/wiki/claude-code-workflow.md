# Working with Claude Code in this repository

Everything here was learned the hard way. Reasoning: [ADR 0001](../adr/0001-sandbox-claude-code.md) (the sandbox) and [ADR 0004](../adr/0004-plain-commands-and-wrapper-scripts.md) (plain commands and board tools). Symptoms and fixes: [troubleshooting](troubleshooting.md).

## The one rule: hardware and network commands are single plain commands

Claude Code runs every Bash call inside a sandbox that hides `/dev/tty*`, `/dev/video*`, `/dev/input/*`, `~/.ssh`, D-Bus and most of the network. Commands listed in `sandbox.excludedCommands` (in `.claude/settings.json`) run outside it. A call only counts as excluded when **every** command in it matches an entry, so all of these silently stay sandboxed:

| Do not | Why | Do instead |
|---|---|---|
| `git push \| tail -3` | the pipe makes `tail` part of the call | `git push` alone |
| `PLATFORMIO_CORE_DIR=... pio ...` | the `VAR=` prefix is part of the text that is matched | `.pio/venv/bin/pio ...` (core directory is set in `platformio.ini`) |
| `cd tools && jbang SerialTest.java` | `cd` keeps the whole call sandboxed | absolute path: `jbang /home/kruse/Projects/Nachtgespenst/tools/SerialTest.java` |
| `S=...; mkdir -p $S; jbang ...` | `;` chains a non-excluded command | create folders in an earlier call |
| `sleep 8; jbang ...` | same | `sleep` in its own call |
| `cmd 2>&1 \| head` | pipe | run plain; filter in a second step or read the saved output file |
| `jbang ... > log.txt` | redirect to a file | let the tool print; use `run_in_background` for long jobs |

Typical symptoms when this goes wrong: `cannot open /dev/ttyACM0`, `Host key verification failed`, `Failed to connect to system D-Bus`, `No such file or directory: '/dev/ttyACM0'` from esptool, `no Nachtgespenst board found`, `curl` returning `000`.

### Entries that must exist in `sandbox.excludedCommands`

Only the repository owner can edit `.claude/settings.json` (it is write-protected inside the sandbox). Needed entries:

```json
"*pio run*upload*", "*pio device*", "*pio test*", "*esptool*",
"*jbang*tools/*.java*",
"git push*", "git pull*", "git fetch*", "git ls-remote*", "gh *",
"bluetoothctl list*", "bluetoothctl show*", "bluetoothctl devices*", "bluetoothctl paired-devices*", "bluetoothctl info*"
```

One generic entry, `*jbang*tools/*.java*`, covers every JBang tool in `tools/` (`Flash`, `Serial`, `SerialTest`, `LedCam`, `LampTest` and any later one), so a new tool needs no new entry. Before 2026-11-18 there was one entry per tool (`*jbang*SerialTest*`, `*jbang*LedCam*`, `*jbang*Flash.java*`, `*jbang*Serial.java*`). The pattern matches both `jbang tools/X.java` and `jbang /abs/path/tools/X.java`. It also covers `jbang build tools/X.java` (then unsandboxed, harmless), but not the `.jsh` generators, which stay sandboxed. Keep tools that must not touch hardware out of `tools/`, or give them another folder.

## Use the board tools, not hand-made commands

| Task | Command |
|---|---|
| Build | `.pio/venv/bin/pio run -e core2` / `-e cores3` (background, about 12 minutes) |
| Flash | `jbang tools/Flash.java core2` / `jbang tools/Flash.java cores3` |
| One-off serial commands | `jbang tools/Serial.java S3 "LAYER 26" "STATE?"` |
| Functional tests | `jbang tools/SerialTest.java [--board S3\|C2]` |
| Screenshots of every page | `jbang tools/SerialTest.java --shot DIR` |
| Page-switch timing | `jbang tools/SerialTest.java --latency` |
| LED bar camera tests | `jbang tools/LedCam.java colours --port PORT --out DIR --crop x0,y0,x1,y1` |

`jbang` is on the PATH (`~/jbinst/jbang/bin`), so `jbang tools/X.java ...` is a plain command. The tools exit with code 3 and say so when they are run inside the sandbox. See [hardware-testing](hardware-testing.md) and [tooling](tooling.md).

## Long jobs

- A build takes 11 to 13 minutes on either board (the dependency scan dominates). Start it with `run_in_background`; you are notified when it ends. Check progress by reading the output file, not by polling.
- **One `pio` build at a time**, and no edits to `platformio.ini` and no `git switch` while a build runs: PlatformIO cleans the build folder when the configuration changes and the running build dies with `.sconsign...dblite: No such file or directory`. The build also compiles whatever is in the working tree, so switching branches mid-build builds a mixture.
- Build the branch you intend to flash. `jbang tools/Flash.java` prints the branch and warns when sources are newer than the image.
- Cancel a background job with `TaskStop`, not `pkill`.

## Protected paths

`.claude/agents`, `.claude/skills`, `.claude/commands`, `.claude/hooks`, `.claude/workflows`, `.claude/settings.json` and `settings.local.json` are read-only inside the sandbox. Inside it they show up as empty `/dev/null` placeholder files (and as untracked entries in `git status`). They do not exist on disk outside the sandbox until someone creates them there. Create and edit them with an unsandboxed call and review the result; never edit `settings.json` on someone's behalf without asking.

## Shell quirks (zsh, login shell fish, Dutch locale)

- `"$c:src/main.cpp"` is a zsh history-style modifier and fails with `bad substitution`: write `"${c}:src/main.cpp"`.
- A glob without a match is an error in zsh: quote it (`--include='*.cpp'`) or use `find`.
- `cd` persists between calls and changes the "primary working directory". Prefer absolute paths; if you must `cd`, `cd` back.
- Error messages are Dutch: `Toegang geweigerd` = permission denied, `Bestand of map bestaat niet` = no such file or directory, `Actie is niet toegestaan` = operation not permitted (sandbox).
- Python is only for throw-away scripts and image tweaks; run it with `python3 -I`. Project tooling is Java (JBang) or shell.

## Git and GitHub

- Stage explicit paths. Placeholder files from the sandbox (`.bashrc`, `.profile`, `.gitconfig`, `.idea`, `.vscode`, `.claude/agents`, `tools/.claude`, ...) appear as untracked; never `git add -A` or `git add .`.
- Commit trailer: `Co-Authored-By: Claude <noreply@anthropic.com>`. No model name in commits, PR text or code.
- `git push` / `pull` / `fetch` must be plain commands (see above). `gh` is not logged in and the GitHub MCP tools are not available, so repository settings cannot be changed from here; list what the owner has to click instead.
- Creating a branch that already has uncommitted work: commit first, then branch; a build is running? wait.
- GitHub Pages paths are case-sensitive and follow the repository name: [ADR 0005](../adr/0005-project-site-on-gh-pages.md).

## Camera and browser checks

- `ffmpeg -f v4l2 -i /dev/video0 ...` needs the sandbox off. Read 25 frames and keep the last (`-frames:v 25 -update 1`): the first frames are black while the exposure settles.
- Delete frames afterwards; they show the room.
- Check a page in a browser without Playwright: `firefox --headless --profile /abs/dir --screenshot /abs/out.png --window-size=1280,2600 file:///abs/index.html` (absolute paths, own profile directory). One-shot screenshots do not load `loading="lazy"` images below the fold: strip the attribute in a temporary copy before rendering and delete the copy.
