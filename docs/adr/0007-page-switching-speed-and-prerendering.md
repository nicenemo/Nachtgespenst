# 0007 Page switching speed: coalesce, slim down, then pre-render neighbours

Status: Accepted 2026-11-18, implementation starts with stage 1 (coalescing) and stage 2 (slimmer pages), pre-rendering only if still needed. Follows [0006](0006-ui-lvgl-or-classic.md), which chose LVGL.

## Context
Paging works, but it gets sluggish when pages are changed quickly (Core2 buttons A/C pressed repeatedly, CoreS3 arrows tapped). Measured with `SerialTest --latency` on 2026-11-18: LVGL needs about 360 ms on average per page on the CoreS3, about 700 ms on the dense 6x4 pages, and 450 to 630 ms on the Core2; the classic UI needs about 92 ms and 100 ms. At 5 presses per second (a 200 ms interval) a 400 to 700 ms page cannot keep up, so the screen lags behind the fingers.

Where the time goes (read from `src/ui.cpp` and `include/lv_conf.h`; the split between building and rendering is partly measured: `LAYER` is acknowledged after about 140 to 230 ms on the CoreS3, the rest is rendering):
1. **Every press builds a page, synchronously.** `ui_set_layer()` calls `buildPage()` inline: `lv_obj_clean()` plus up to 24 new buttons with labels. Five quick presses build five pages; four of them are never seen. Nothing is coalesced.
2. **Icon glyphs are rasterised at run time.** Icons come from Tiny TTF (`LV_USE_TINY_TTF`), three sizes, a 96-glyph cache each: every glyph that is not cached is rendered by stb_truetype inside the draw path.
3. **Heavy styling per tile.** Rounded corners, borders, partial opacity, shadows disabled but pressed-state transforms set on each of up to 24 buttons.
4. **Rendering is partial, software, in 24-row strips** (`LV_DISPLAY_RENDER_MODE_PARTIAL`, two buffers of 15 kB), flushed synchronously with `pushImage`.

## Measured (2026-11-18)
Firmware timers (`PERF?`, medians over a sparse page, two dense 6x4 pages and the Locks page) and a burst of ten page changes 150 ms apart (`SerialTest --latency`, LVGL):

| | CoreS3 | Core2 |
|---|---|---|
| Build of a page (`buildPage`) | 221 ms | 280 ms |
| Render | 315 ms | 413 ms |
| of which flush to the panel (SPI) | 55 ms | 73 ms |
| so LVGL's own drawing | about 260 ms | about 340 ms |
| Page on screen (`LAYER` + `SHOT` minus read-out) | 695 ms | 838 ms |
| Burst: last page on screen after the first press | 2009 ms | 2095 ms |
| Burst: lag after the last press | 509 ms | 595 ms |

Reading: the panel transfer is only about a tenth of a page; building and LVGL's software drawing are the cost, in roughly equal measure. A cached page costs about one flush, 55 to 75 ms, which confirms the estimate for option 3. The burst shows that light pages keep up (10 pages in about 2 s) but the last page still arrives half a second after the last press; dense pages are worse.

## Options
| Option | What it does | Expected effect on a page | Cost and risks |
|---|---|---|---|
| 1. Coalesce page changes | a press only updates the target page and the header counter; the page is built and rendered once, after presses stop for about 30 ms (or once per loop tick) | a burst costs one page instead of N; sustained fast paging shows only the last page | small change in `ui_set_layer`/`ui_loop`; must keep `LAYER` + `SHOT` semantics (build before screenshot) |
| 2. Cut the per-page cost | pre-rendered bitmap icon fonts (`lv_font_conv`) instead of Tiny TTF; a pool of 24 tile widgets that are restyled and hidden instead of recreated; flatter tile style | estimate: build plus render from about 360 ms towards 100 to 150 ms (to be measured) | font tooling in the build, more code in `ui.cpp`; the look should stay the same |
| 3. Pre-render neighbours off-screen | idle time renders the pages around the current one into RGB565 buffers in PSRAM; a switch is one blit (about 30 to 40 ms, estimate) | a switch to a cached page is almost instant regardless of the live render cost | see below; the largest change |
| 4. Slide transition | blit old and new page at shifting offsets for a few frames | adds time (one full-screen push per frame, about 30 ms each) | cosmetic; only worth it when paging slowly |

### Pre-rendering in detail (option 3)
- **Memory.** One page is 320 x 240 x 2 bytes = 153,600 bytes. Current plus previous and next: 460,800 bytes. Plus or minus three pages: 1.07 MB. All 27 pages: 4.15 MB. Both boards are built with `BOARD_HAS_PSRAM`; the CoreS3 has 8 MB. On the Core2 PlatformIO counts 4.5 MB in total (internal RAM plus about 4 MB of mapped PSRAM), so a full cache does not fit there; a ring of plus or minus three does. LVGL's allocator is `LV_STDLIB_CLIB` (plain `malloc`), so the buffers have to be allocated explicitly with `heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`.
- **How to render off-screen.** Either `lv_snapshot_take_to_buf()` (needs `LV_USE_SNAPSHOT` in `lv_conf.h`, a `lv_draw_buf` in PSRAM and a page built under a hidden parent), or by temporarily redirecting `flushCb` to copy strips into a cache buffer instead of the panel and calling `lv_refr_now()`. Either way the page is built and rendered once, then its widgets are deleted. LVGL is not thread-safe, so this runs in `loop()`; a render of 100 to 700 ms is one uninterruptible step.
- **Scheduling.** Only when the UI has been idle for a moment (no touch or button for about 300 ms), nearest page first. With the live render at 360 to 700 ms a fast burst still outruns the prefetch (page n+2 is not ready 200 ms after the switch to n+1); that is why option 2 should come first: at 100 to 150 ms per page the prefetch keeps up and a larger look-ahead is cheap.
- **Touch while showing a cached image.** There are no widgets behind the bitmap. Taps must be resolved from the layout geometry (as the classic UI does, the layout constants are shared with `SerialTest`) and the pressed feedback drawn directly on the panel, or the live widgets are rebuilt right after the blit (and taps in that window are handled by geometry).
- **Dynamic content.** The Locks page (ON/OFF state) is never cached; its three tiles render live. A theme change would invalidate the cache.
- **Unaffected.** `SHOT` reads the panel, the LED bar follows `g_page`, serial tests are unchanged.
- **Slide.** A frame of a slide is a full-screen push (about 30 ms at 40 MHz SPI, estimate), so 4 frames cost about 120 ms and 8 frames about 250 ms: slower than an instant switch. If a transition is wanted: 3 to 4 steps, only when the previous page change was more than about 400 ms ago, never during fast paging. For a macro pad whose job is fast key presses, instant switching is the better default.
- The classic UI can do the same with `M5Canvas` sprites (its draw functions would target a canvas); at 92 ms per page it hardly needs it.

## Stage 1 measured (2026-11-19, coalescing)
`ui_set_layer()` now records the target and updates the header; the body is built once in `ui_loop()` after 25 ms, or earlier when a screenshot or an injected tap needs it. On the CoreS3: the `LAYER` acknowledgement went from 140 to 230 ms to 11 ms, the burst of ten changes 150 ms apart now has the last page on screen after 1,803 ms (was 2,009) and 303 ms after the last press (was 509). A single page costs the same as before (about 160 to 240 ms build and 290 to 320 ms render, also when the same page is shown a second time, so a cold glyph cache is not the cost): coalescing helps bursts, not the base cost. Next: compiler optimisation (-O2 instead of -Os) and taller strips (48 rows instead of 24), then fewer widgets per page.

## Recommendation
Stage it and measure after each stage (`SerialTest --latency`, plus a quick-paging test: send `LAYER n+1` ten times at 150 ms intervals and time until the last page is on screen):
1. **Coalesce** page changes (option 1). Cheap, helps fast paging immediately.
2. **Slim the page** (option 2): bitmap icon fonts, widget pool, flatter style. Target at most about 150 ms per page on both boards.
3. **Pre-render only if still needed**: plus or minus one page in PSRAM (option 3), widened when step 2 made renders cheap; touch by geometry; Locks live.
4. **Slide** only as an optional, adaptive extra; default off.

## Measurements
- Done: `PERF?` (build, render and flush times of the last page) and the burst test, see "Measured".
- The flush of a full page is 55 ms (CoreS3) and 73 ms (Core2), taken from the strips LVGL pushes; a single 153,600-byte `pushImage` from PSRAM has not been timed separately yet.
- Still to measure: free PSRAM on each board at run time (`heap_caps_get_free_size(MALLOC_CAP_SPIRAM)`).
- One more observation to investigate: a touch or button press that arrives while a page is being built (about 220 to 280 ms) or rendered is only seen if it is still held when the main loop polls again; a short tap can be missed. This has not been measured (the serial `TAP` command is injected and cannot lose a tap).

## Consequences
- Until implemented, fast paging remains sluggish on LVGL; the classic branch does not have the problem.
- Steps 1 and 2 are independent of PSRAM and keep RAM use flat; step 3 adds up to about 1 MB of PSRAM and complexity in `ui.cpp`.
