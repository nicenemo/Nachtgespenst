# 0005 Publish the project site from `site/` through the `gh-pages` branch

Status: Accepted

## Context
The project needs one `index.html` that explains it, with screenshots, camera test results and the set-up, under the account's custom domain like `hanskruse.eu/mikmakmuk/`. Repository settings cannot be changed from the command line in this environment (`gh` is not logged in, no GitHub API tools), so a solution that needs a settings change per iteration is awkward.

## Decision
- The source lives on `develop` in `site/` (`index.html` plus `img/`). A workflow, `.github/workflows/pages.yml`, copies `site/` to the `gh-pages` branch on every push to `develop` or `main` that touches `site/**`, using only `GITHUB_TOKEN` (`contents: write`), `actions/checkout` and plain `git`.
- GitHub Pages serves the `gh-pages` branch, root folder. Through the account's custom domain it appears at **https://hanskruse.eu/Nachtgespenst/**.
- The page is one self-contained HTML file (inline CSS, no JavaScript, no build step); images are separate files under `site/img/`.

## Consequences
- The first publish worked: the workflow produced a `gh-pages` commit identical to a manual one. Publishing is "commit and push `site/`".
- The path is case-sensitive and follows the repository name (`/Nachtgespenst/`); `/nachtgespenst/` returns 404. Renaming the repository to lowercase would change that (and break the capitalised URL); only the owner can do it.
- `gh-pages` is owned by the workflow: never commit to it by hand; a rejected push means the workflow got there first.
- Images add about 1 MB to the repository and to `gh-pages`.
- The page was generated once by a throw-away script that is not kept; later edits are made in the HTML directly, screenshots are refreshed with `SerialTest --shot` (skill `publish-site`).
- Alternatives considered: `docs/` on `main` (mixes the site with the markdown documentation, Jekyll rendering); the official "deploy-pages" action (needs the Pages source set to "GitHub Actions" in the settings, which cannot be done from here); a separate repository (more places to keep in sync).

## Update 2026-11-18
The repository was renamed to lowercase `nachtgespenst`, so the site is now served at **https://hanskruse.eu/nachtgespenst/** (still case-sensitive). The old capitalised path no longer exists: GitHub redirects the repository and git remotes, but not Pages paths, so links to the old site address must be changed. The statements above about `/Nachtgespenst/` are historic.
