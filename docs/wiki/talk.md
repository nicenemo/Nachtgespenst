# The talk: slides, demos and cameras

The demo window (`jbang tools/DemoApp.java`, see [demo-app.md](demo-app.md)) doubles as a presentation: a ten-minute talk of automatic slides in which the demos run by themselves, and every demo can still be started alone from the sidebar.

## Slides are files

One Markdown file per slide in `tools/talk/`, played in file-name order (`01-title.md`, `02-idea.md`, ...). Edit, reorder or delete them with any editor; the files are read again every time you press **Start the talk**.

```
---
seconds: 45        how long the slide stays
demo: disk         optional: a demo that starts with the slide
---
# The title
- a bullet point
- another one
> a line for the speaker: it appears in the log, never on the slide
```

* `# ` is the title, `- ` or `* ` a bullet, anything else a plain line. Only this subset is read.
* The text is drawn as large as the panel allows, so a short slide is big and a long one shrinks to fit.
* A slide with a `demo:` waits for its demo to end, even when the seconds are over; the next slide stops a demo that is still running.
* The seconds add up to the length of the talk (now about ten minutes); the status bar shows the elapsed time against it.

Demos a slide can start: `pages`, `disk`, `diskfast`, `bars`, `bar:<effect>`, `board:<effect>`, `grid:<name>` (`grid:raster bars`, `grid:sine scroller`), `scope`, `vu`, `miccheck`, `photos`. The effect names are the ones in `Scenarios.java`. Without a board the slides still run and the demos are skipped.

Keys: space or right arrow next slide, left arrow previous, `P` pause, `H` hide the toolbar (a clean window to share), `F11` full screen (`Esc` leaves it).

## Cameras

The window can show two webcams: the one on the tripod above the board and the laptop camera. Open the camera popup (camera icon in the toolbar), tick a camera to open it and pick the device from the list; the icon buttons turn and mirror the picture; untick it and the device is released at once, so the laptop camera is free for Teams (or any other call) while the window is closed or the box is empty. A camera that is not there is not an error: the panel says so. The choice of devices is remembered.

Cameras are read through `ffmpeg` and Video4Linux (`/dev/video*`), the same route as `tools/LedCam.java`: `ffmpeg` must be installed, nothing else. If the camera is in use by another program the panel says "cannot open ... (in use?)".

## Giving the talk online

* Share the window of the demo app in Teams (or the whole screen). Press `H` to hide the toolbar and `F11` for full screen.
* Teams and the window cannot both hold the same camera. Either use two different cameras (tripod camera in the window, laptop camera in Teams), or let the window show the laptop camera and share the window instead of the camera. Sharing a camera between programs needs a virtual camera (`v4l2loopback`); that is not set up here.
* The tripod camera above the board shows the real lamps in the window, so viewers see the bar and the screen as well as the PC's own rendering of the lamps.

## The window

A row of icon buttons sits at the top (every button has a tooltip): start, pause, previous, next and stop of the talk, stop of a running demo, and the popups for the demos, the cameras, the microphone, the speaker and the log. The rest of the window belongs to the slide; the cameras, the board and the lamps share the right-hand column, the cameras getting most of it. The real metrics of a demo (speeds, bytes, hashes) appear in the bottom bar. Command line: `jbang tools/DemoApp.java --tripod 1 --laptop 0` switches cameras on at the start, `jbang tools/CamTool.java list|snap` works without a window.
