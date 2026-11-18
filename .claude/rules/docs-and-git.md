---
paths:
  - "docs/**"
  - "*.md"
  - "site/**"
  - ".github/**"
---

# Documentation, the site and git

- Documentation and code comments are British English (colour, licence, behaviour). Identifiers and file names keep their spelling (`LAYER_COLORS`, `LICENSE`).
- One fact, one place: put how-to material in `docs/wiki/`, decisions in `docs/adr/` (never rewrite an accepted ADR, add a superseding one), open work in `docs/TODO.md`, history in `CHANGELOG.md`. Link instead of copying.
- The project site is `site/index.html` (single page) plus `site/img/`. Pushing a change under `site/` to `develop` or `main` publishes it through `.github/workflows/pages.yml` to the `gh-pages` branch; it is served at https://hanskruse.eu/Nachtgespenst/ (case-sensitive). Skill: `publish-site`. Do not commit to `gh-pages` by hand; if a push is rejected, `git fetch origin gh-pages` and compare.
- Stage explicit paths with `git add <paths>`; never `git add -A` or `git add .` (sandbox placeholder files show up as untracked). Group commits by topic. Trailer: `Co-Authored-By: Claude <noreply@anthropic.com>` (no model name). Push with a plain `git push`.
- No pull requests, merges to `main` or repository settings changes unless asked; list what the owner has to do instead (GitHub is not reachable through `gh` or an API here).
- Screenshots and camera frames: keep only what the site or an ADR needs, delete the rest (they show the room).
