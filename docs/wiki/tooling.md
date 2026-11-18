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
