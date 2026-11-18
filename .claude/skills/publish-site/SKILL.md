---
name: publish-site
description: Edit and publish the Nachtgespenst project site (site/index.html on GitHub Pages at hanskruse.eu/Nachtgespenst/). Use when the site content, screenshots or camera test sheets change, or when asked to check that the site is live.
---

# Project site

- Source: `site/index.html` (one self-contained page, inline CSS, no JavaScript) and `site/img/{lvgl,classic,camera}/`. Edit on `develop`.
- Publishing: a push to `develop` or `main` that touches `site/**` runs `.github/workflows/pages.yml`, which copies `site/` to the `gh-pages` branch; GitHub Pages serves that branch. Never commit to `gh-pages` by hand (a rejected push means the workflow got there first: `git fetch origin gh-pages` and compare, do not force).
- URL: **https://hanskruse.eu/Nachtgespenst/** (the path is case-sensitive; `/nachtgespenst/` is 404 unless the repository is renamed). Decision record: `docs/adr/0005-project-site-on-gh-pages.md`.

## Update screenshots
1. Boards on the firmware you want to show; `jbang tools/SerialTest.java --shot DIR` (plain command; `DIR` created earlier).
2. Copy `DIR/<board>/pageNN.png` to `site/img/<lvgl|classic>/<c2|s3>-NN.png`; palette-reduce with ffmpeg (`palettegen` + `paletteuse`) to about 3 to 6 kB each.
3. Page descriptions live in the HTML, one `<article class="page">` per page.

## Check before pushing
- Render a copy in headless Firefox (absolute paths, own profile directory, `loading="lazy"` stripped in the temporary copy) at 1280 px and at 390 px width; delete the copy afterwards. See `docs/wiki/claude-code-workflow.md`.
- British English; no personal data; camera pictures only if needed.

## Push and verify
- `git add site <other paths>`, commit with the `Co-Authored-By: Claude <noreply@anthropic.com>` trailer, plain `git push origin develop`.
- Wait a minute, then one unsandboxed `curl -sS -m 15 -o /dev/null -w "%{http_code} %{size_download}\n" https://hanskruse.eu/Nachtgespenst/` should print `200` and a size matching `site/index.html`. Check one image URL as well.
