# Architecture decision records

One file per decision, numbered `NNNN-title.md`, with Status, Context, Decision, Consequences. Do not rewrite an accepted record: add a new one that supersedes it.

| ADR | Title | Status |
|---|---|---|
| [0001](0001-sandbox-claude-code.md) | Run Claude Code in a filesystem and network sandbox | Accepted, pending verification |
| [0002](0002-project-local-platformio.md) | Keep PlatformIO inside the project | Accepted |
| [0003](0003-jbang-pinned-jdk.md) | Pin the JDK per script with `//JAVA` | Accepted |
| [0004](0004-plain-commands-and-wrapper-scripts.md) | Run hardware and network commands as single plain commands, backed by wrapper scripts | Accepted (owner adds the `excludedCommands` entries) |
| [0005](0005-project-site-on-gh-pages.md) | Publish the project site from `site/` through the `gh-pages` branch | Accepted |
| [0006](0006-ui-lvgl-or-classic.md) | LVGL or the classic direct-drawing UI | Proposed (leaning LVGL) |
| [0007](0007-page-switching-speed-and-prerendering.md) | Page switching speed: coalesce, slim down, then pre-render neighbours | Proposed |
| [0008](0008-jbang-for-project-tooling.md) | Write project tooling in JBang (Java), not in shell scripts | Accepted |
