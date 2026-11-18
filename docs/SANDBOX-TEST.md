# Sandbox test plan

Resume with: "continue with docs/SANDBOX-TEST.md". Run `git status` first. Do not commit unless asked.

Goal: verify that Claude Code's sandbox is enabled and that every tool this project needs works inside it, with reads and writes outside the project limited to the exemptions in `.claude/settings.json`.

Tick each step and write down the result and any fix under "Results" at the bottom.

## 0. Install the settings (user does this once, then restarts Claude Code)

The proposed file was prepared at:
`/tmp/claude-1000/-home-kruse-Projects-Nachtgespenst/4ed53e2f-70f7-4b3e-af26-ba24eabd541b/scratchpad/settings.json`
(the scratchpad may be gone after a reboot). If it is gone, rebuild it from the intent below.

```sh
cp <scratchpad>/settings.json /home/kruse/Projects/Nachtgespenst/.claude/settings.json
```

Intended content, compared with the previous file:
- `sandbox.enabled: true`
- `allowRead`: `.`, `~/jbinst`, `~/.jbang`, `~/.m2`, `~/.sdkman`, `~/.gitconfig`, `~/.config/git`
- `allowWrite`: `.`, `/tmp`, `~/.jbang`, `~/.m2`
- `denyRead`: `~/` (unchanged)
- `allowedDomains`: `dl.registry.platformio.org`, `registry.platformio.org`, `api.registry.platformio.org`, `github.com`, `api.github.com`, `*.githubusercontent.com`, `pypi.org`, `files.pythonhosted.org`, `repo.maven.apache.org`, `repo1.maven.org`, `jitpack.io`, `api.foojay.io`, `www.usb.org`
- `excludedCommands`: `*pio run*upload*`, `*pio device*`, `*pio test*`, `*esptool*`, `*jbang*SerialTest*`, `gh *`, `git push*`, `git pull*`, `git fetch*`, `git ls-remote*`

Check after restart: run `/sandbox` (or look at the status) and confirm it reports enabled.

## 1. Sandbox containment (negative tests, should FAIL)

Run each through the Bash tool without disabling the sandbox. Every one must be blocked.

- [ ] Read outside the allowlist: `ls ~/Documenten` and `cat ~/.bash_history`
- [ ] Read of `~/.ssh`: `ls ~/.ssh`
- [ ] Write in home outside the allowlist: `touch ~/sandbox-test-file`
- [ ] Write to `~/.sdkman` (must be blocked, it is read-only): `touch ~/.sdkman/x`
- [ ] Network to a host not on the list: `curl -sI https://example.com`

If any of these succeed, stop and fix the settings before going further.

## 2. Allowed access (positive tests, should WORK)

- [ ] Read project files; write `touch /home/kruse/Projects/Nachtgespenst/.pio/sandbox-probe && rm` the same file
- [ ] Write to `/tmp` and to the scratchpad
- [ ] `git status`, `git diff --stat`, `git log --oneline | head -3` (needs `~/.gitconfig` read)
- [ ] `ls ~/.jbang ~/.m2 ~/jbinst ~/.sdkman` (read works)
- [ ] `curl -sI https://github.com` and `curl -sI https://pypi.org` (domains allowed)

## 3. jbang, inside the sandbox

Run from inside `tools/`.

- [ ] `jbang --version`
- [ ] `jbang jdk list --available | head` (needs `api.foojay.io`)
- [ ] `jbang GenerateHeaders.jsh` prints "Generation complete." (JDK 27 from `~/.jbang/cache/jdks`, jars from `~/.m2`)
- [ ] `git status --short` shows no unexpected changes. `tools/include/` is gitignored.
- [ ] `jbang FetchHut.jsh` (needs `www.usb.org`). Afterwards check that `tools/data/hut.json` is unchanged with `git diff --stat tools/data`. If the PDF fetch changed it, restore with `git checkout tools/data`.
- [ ] Optional: try a JDK that is not cached (`jbang jdk install 28` or `//JAVA 28` on a scratch script) to confirm the download path works inside the sandbox, then `jbang jdk uninstall 28`.

## 4. PlatformIO build, inside the sandbox

Always use the project-local install, never plain `pio`:

```sh
cd /home/kruse/Projects/Nachtgespenst
.pio/venv/bin/pio --version        # core dir comes from core_dir in platformio.ini
.pio/venv/bin/pio run -e core2     # ~12 min, run in the background
.pio/venv/bin/pio run -e cores3    # ~12 min, never run two builds at once in the same .pio
```

- [ ] `pio --version` works
- [ ] `pio run -e core2` ends in SUCCESS. If it fails on a blocked host, note the domain, add it to `allowedDomains`, and retry.
- [ ] `pio run -e cores3` ends in SUCCESS
- [ ] Optional fresh-install check: `python3 -m venv /tmp/venvtest && /tmp/venvtest/bin/pip install platformio` (needs `pypi.org` and `files.pythonhosted.org`). Delete `/tmp/venvtest` afterwards.
- [ ] Optional library fetch check: move `.pio/libdeps` aside, rebuild, and confirm libraries come back via `dl.registry.platformio.org` and `github.com`. Restore it if it fails.

## 5. Excluded commands (hardware, runs OUTSIDE the sandbox)

These need a device on USB. Skip if no board is connected. Confirm the exclusion patterns actually match: the command should run unsandboxed, but it should still prompt for permission as usual.

- [ ] `ls -l /dev/ttyUSB0 /dev/ttyACM0` (Core2 is ttyUSB0, S3 is ttyACM0)
- [ ] `.pio/venv/bin/pio device list`
- [ ] Flash through the documented esptool command (see `docs/OVERNIGHT.md` step 2/3) or `pio run -e cores3 -t upload`
- [ ] `jbang tools/SerialTest.java --board S3` (or `--board C2`)
- [ ] `git fetch` (matches `git fetch*`)

Known issue: Core2 flashing is flaky (see `docs/OVERNIGHT.md`). A failed Core2 flash is hardware, not a sandbox failure. Check S3 first.

If an excluded command still fails because of the sandbox, the call is not a single plain command: a pipe, `cd`, `&&`, `;`, a `VAR=x` prefix or a redirect keeps the whole call sandboxed. The patterns themselves (`*pio run*upload*`, `*jbang*SerialTest*`, `git push*`) were verified to match plain calls; see [claude-code-workflow](wiki/claude-code-workflow.md) and [ADR 0004](adr/0004-plain-commands-and-wrapper-scripts.md).

## 6. Wrap-up

- [ ] Update `.claude/settings.json` with every fix found above
- [ ] Update `CLAUDE.md` and `README.md` if any instruction changed
- [ ] Record the final results below, and delete the scratchpad copy of the settings

## Results

(fill in while testing)
