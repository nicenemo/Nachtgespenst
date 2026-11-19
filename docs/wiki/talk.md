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
* `![alt](img/photo.jpg)` shows a picture, the path relative to the slide file (`../../site/img/camera/cores3-colours.jpg` reaches the colour test sheet of the site). With bullets the picture sits at the right (60 % of the width), without bullets it fills the slide. A missing file is reported on the slide instead of leaving a hole. Put your photos in `tools/talk/img/`; slide `02b-cores3.md` waits for `img/cores3.jpg`, for instance made with `jbang tools/CamTool.java snap tools/talk/img/cores3.jpg 1` (keep the room out of the frame: the repository is public).
* The text is drawn as large as the panel allows, so a short slide is big and a long one shrinks to fit.
* A slide with a `demo:` waits for its demo to end, even when the seconds are over; the next slide stops a demo that is still running.
* The seconds add up to the length of the talk (now about ten minutes); the status bar shows the elapsed time against it.

Demos a slide can start: `pages`, `disk`, `diskfast`, `bars`, `bar:<effect>`, `board:<effect>`, `grid:<name>` (`grid:raster bars`, `grid:sine scroller`), `scope`, `vu`, `miccheck`, `photos`. The effect names are the ones in `Scenarios.java`. Without a board the slides still run and the demos are skipped.

Keys: space or right arrow next slide, left arrow previous, `P` pause, `H` hide the toolbar (a clean window to share), `F11` full screen (`Esc` leaves it).

## Cameras

The window can show two webcams: the one on the tripod above the board and the laptop camera. Open the camera settings (gear in the toolbar), pick the device from the list; the icon buttons turn and mirror the picture; untick it and the device is released at once, so the laptop camera is free for Teams (or any other call) while the window is closed or the box is empty. A camera that is not there is not an error: the panel says so. The choice of devices is remembered.

Cameras are read in plain Java through Video4Linux (`tools/V4l2.java`, the foreign function API: `open`, `ioctl`, `mmap` of libc), so nothing has to be installed. The picture is asked as YUYV (raw); a camera with only MJPEG is decoded with the Java image library. The user needs access to `/dev/video*` (a normal desktop session has it). If another program holds the camera the panel says so ("the camera is in use by another program"); a camera that answers with an input/output error is probably a bad USB connection. The same camera can be shown in both panels.

## Giving the talk online

* Share the window of the demo app in Teams (or the whole screen). Press `H` to hide the toolbar and `F11` for full screen.
* Teams and the window cannot both hold the same camera. Either use two different cameras (tripod camera in the window, laptop camera in Teams), or let the window show the laptop camera and share the window instead of the camera. Sharing a camera between programs needs a virtual camera (`v4l2loopback`); that is not set up here.
* The tripod camera above the board shows the real lamps in the window, so viewers see the bar and the screen as well as the PC's own rendering of the lamps.

## The window

The title of the slide is shown large in the header. A row of icon buttons sits at the bottom left (every button has a tooltip): start, pause, previous, next and stop of the talk, stop of a running demo, and the popups for the demos, the cameras, the microphone, the speaker and the log. The rest of the window belongs to the slide; the cameras, the board and the lamps share the right-hand column, the cameras getting most of it. The real metrics of a demo (speeds, hash check) and the progress of the talk appear in the display next to the buttons; what is running is shown in the info bar at the top right. Command line: `jbang tools/DemoApp.java --tripod 1 --laptop 0` switches cameras on at the start, `jbang tools/CamTool.java list|snap` works without a window.

## Driving the window from tests and scripts

`jbang tools/DemoApp.java --control` reads commands on standard input and answers with one line each (`OK ...` or `ERR ...`); `--headless` runs the same application without a window (add `--no-board` to skip the connection to the board). Commands: `state`, `talk start|stop|pause|next|prev|goto N`, `demo ID`, `wait SECONDS`, `camera off|tripod|laptop`, `camera turn left|right|half`, `camera mirror|flip`, `blank`, `toolbar`, `shot FILE [WxH]`, `quit`. `jbang tools/DemoTest.java [--board] [--shot DIR]` uses it: it checks the talk (start, next, previous, pause, goto, stop), the black screen, the toolbar, the camera rule (one stream at a time), a screenshot and, with `--board`, a short demo on the real board.
