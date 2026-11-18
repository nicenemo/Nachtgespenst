# 0006 LVGL or the classic direct-drawing UI

Status: Accepted 2026-11-18 (option D): LVGL is the UI. `feature/classic-ui` is kept as a maintained branch for communication purposes and a possible later rollback; it is not merged and not deleted. The page speed work of [0007](0007-page-switching-speed-and-prerendering.md) follows.

## Context
Two complete UI implementations exist behind the same `ui.h`: LVGL 9 on `develop`/`main` (icons, anti-aliased text, rounded tiles, partial redraws) and direct drawing on `feature/classic-ui` (plain tiles, text labels, arrow shapes, bottom navigation bar on the CoreS3). Both pass the same serial test suite on both boards. Measured on the boards on 2026-11-18:

| | LVGL | Classic |
|---|---|---|
| Flash used (CoreS3) | 2,327,535 B (35.5 %) | 1,651,631 B (25.2 %) |
| RAM used (CoreS3) | 104,796 B (32.0 %) | 73,380 B (22.4 %) |
| Page on screen, CoreS3 | about 360 ms (260 to 480 ms), about 700 ms on the dense 6x4 pages | about 92 ms |
| Page on screen, Core2 | about 450 to 630 ms | about 100 ms |
| Source | `ui.cpp` 429 lines, `Icons.cpp` 194, 40 kB icon font data, `lv_conf.h` | `ui.cpp` 378 lines, `Icons.cpp` 182 |
| Build time | about 13 min | about 11.5 min (dependency scan dominates) |

Method: `jbang tools/SerialTest.java --latency` (page on screen = `LAYER` + `SHOT` minus a plain `SHOT`, because LVGL answers `LAYER` before it renders).

Quality notes. Classic: whole-page redraw (about 90 ms, a visible repaint on the CoreS3), no icons (captions instead, which reads better for some tiles), flat tiles, no pressed animation beyond a 120 ms highlight, touch handled by geometry. LVGL: richer look and room for widgets, but a page is built from scratch (up to 24 buttons and labels) and icon glyphs are rasterised at run time by Tiny TTF.

## Options
- A. LVGL only (as now).
- B. Classic only (smaller and faster, plainer).
- C. Both behind a build flag in one tree (two UI files, LVGL still a dependency; more to maintain).
- D. LVGL on `develop`, classic kept as a maintained branch, and the LVGL page speed fixed ([0007](0007-page-switching-speed-and-prerendering.md)).

## Recommendation
D. The page-switch cost is a fixable implementation issue (build and render strategy), not a property of LVGL; the classic branch is a useful fast, small fallback and a baseline to measure against. Revisit if LVGL cannot reach about 150 ms per page after the steps in 0007.

## Consequences (if D)
- `feature/classic-ui` is not merged; keep it compiling when `ui.h` or the keymap structure changes, or retire it explicitly.
- Both implementations must keep the same layout constants (header 24 px on the Core2, 38 px bottom bar on the CoreS3, gap 3) because `SerialTest` taps by coordinates.
