# The shell

The serial port of both boards (USB, 115200 baud) is a small shell: commands and arguments in lower case, `help` for the list and `help <command>` for one command. Every reply starts with `OK` or `ERR`; some commands print more lines after the first.

This page is generated from `docs/shell/commands.txt` by `tools/Manual.java`, which also writes the built-in help of the firmware (`include/ShellHelp.h`) and the manual on the site. The same file is `help.txt` on the SD card of the CoreS3 (`jbang tools/Shell.java install-help`), where `help <command>` reads its text from.

Try it: `jbang tools/Shell.java S3` (an interactive shell) or `jbang tools/Serial.java S3 "help"`.

| Command | Usage | What it does |
|---|---|---|
| [`help`](#help) | `help [command]` | List the commands, or explain one. |
| [`ping`](#ping) | `ping` | Check that the board answers. |
| [`info`](#info) | `info` | Board, name, firmware build time, screen size, number of pages. |
| [`state`](#state) | `state` | The current page, the host lock LEDs, the last key, the connections. |
| [`layer`](#layer) | `layer <n>` | Show page n (0 is the first key layer). |
| [`key`](#key) | `key <layer> <tile>` | Press a tile as if it was touched. It really types on the host! |
| [`tap`](#tap) | `tap <x> <y>` | Touch the screen at x, y (Locks page only). |
| [`down`](#down) | `down <x> <y>` | Hold a touch at x, y until up (Locks page or the header bar only). |
| [`up`](#up) | `up` | Release the touch that down is holding. |
| [`btn`](#btn) | `btn <a|b|c> <down|up>` | Press or release a button of the Core2 (A previous page, B Locks, C next page). |
| [`clicker`](#clicker) | `clicker <prev|next|start|blank|end|laser>` | Type the key of a presenter button (Core2 only). It really types on the host! |
| [`pointer`](#pointer) | `pointer [on|off|toggle|click [left|right|middle]|scroll <n>|<setting> <value>|feed <yaw> <pitch>]` | The air mouse of the presenter (Core2 only): switch it, tune it, click. |
| [`imu`](#imu) | `imu <x> <y> <z>` | Pretend the gyroscope turns around its three axes (degrees per second) for 0.6 s (Core2 only, for tests). |
| [`mod`](#mod) | `mod <ctrl|shift|alt|win|cs|as|meh|hyper|off>` | Hold or release a sticky modifier of the mouse page (Core2 only). |
| [`shot`](#shot) | `shot` | Screenshot of the screen, 24 bits per pixel. |
| [`perf`](#perf) | `perf` | How long the last page took to build, render and flush. |
| [`wake`](#wake) | `wake` | Restart the idle timers: screen and LED bar on again. |
| [`bl`](#bl) | `bl <16..255|auto>` | Fixed backlight, or auto for the stored dim and off times. |
| [`leds`](#leds) | `leds [off|locks|on]` | The LED bar: off (also cuts the 5 V rail of the base), only the host lock state, or the layer colours. |
| [`set`](#set) | `set [key value]` | Show or change the stored settings. |
| [`prefer`](#prefer) | `prefer [usb|ble|both]` | Which link carries the keys when both are connected. |
| [`only`](#only) | `only <usb|ble>` | Never use the other link, even if the chosen one is down. |
| [`lamps`](#lamps) | `lamps [reset | grid <cols> <rows> | get <id> | bright <n> | worker on|off]` | The LampArray: counters, the lamp grid, one lamp, the bar brightness, the bar worker task. |
| [`disk`](#disk) | `disk [reset | dir | sound on|off]` | The Disk demo: the state of the transfer, forget it, list the SD card, mute the modem sounds. |
| [`sd`](#sd) | `sd` | Is there an SD card, which type, how big, how much is used. |
| [`mic`](#mic) | `mic [on|off]` | Switch the microphone on or off, or show what it hears. |
| [`wave`](#wave) | `wave` | The last microphone buffer: 512 samples of 16 bits as hex (for tests). |
| [`bar`](#bar) | `bar` | The colours of the ten bar LEDs as hex (for tests). |
| [`telem`](#telem) | `telem [bar|wave|both|off]` | Send the bar colours and/or the microphone waveform to the PC as game pad reports. |

## help

`help [command]`

List the commands, or explain one.

Without an argument the commands are listed with a one-line summary. With a command name the full text of that command is printed: from help.txt on the SD card when there is one, otherwise the usage line and the summary that are built into the firmware.

* Reply: `OK HELP, then the lines | ERR help unknown`

```
help layer
```

## ping

`ping`

Check that the board answers.

* Reply: `OK PONG`

```
ping
```

## info

`info`

Board, name, firmware build time, screen size, number of pages.

The firmware build time (fw=) is the first thing to check before trusting a test: it tells which build is on the board.

* Reply: `OK INFO board=S3 name=NG-S3 fw=Nov 19 2026_17:30:28 w=320 h=240 pages=30 locks=26`

```
info
```

## state

`state`

The current page, the host lock LEDs, the last key, the connections.

Shows the page number and title, the Caps, Num and Scroll Lock state the host reports, the last key pressed, the number of presses, and whether USB and Bluetooth are connected (conn, usb, ble).

* Reply: `OK STATE page=0/30 title=Maven Tools caps=0 num=0 scroll=1 last=- presses=0 conn=1 usb=1 ble=1`

```
state
```

## layer

`layer <n>`

Show page n (0 is the first key layer).

The pages are the key layers, then the Locks page, the Disk page and the two lamp pages (the last two; the test tools count from the end). Changes in a burst are merged: only the last page is built.

* Reply: `OK LAYER n | ERR range`

```
layer 26
```

## key

`key <layer> <tile>`

Press a tile as if it was touched. It really types on the host!

The tile is sent to the host like a touch would. Power Down and Sleep (tiles of the System and Media pages) are refused (ERR key). Only use it on the Locks page (Scroll Lock) and on the Wake Up tile of the Media page; never script a loop over a whole page.

* Reply: `OK KEY | ERR key`

```
key 26 2
```

## tap

`tap <x> <y>`

Touch the screen at x, y (Locks page only).

Refused outside the Locks page (ERR tap), so that a stray coordinate cannot press a key on another page.

* Reply: `OK TAP | ERR tap`

```
tap 160 120
```

## down

`down <x> <y>`

Hold a touch at x, y until up (Locks page or the header bar only).

For tests of the pressed state: the control under the finger changes colour while the touch is held. Nothing is sent to the host: up first moves the touch off the control, so no click fires. state shows touch=1 while it is held.

* Reply: `OK DOWN | ERR down`

```
down 100 120
```

## up

`up`

Release the touch that down is holding.

* Reply: `OK UP | ERR up`

```
up
```

## btn

`btn <a|b|c> <down|up>`

Press or release a button of the Core2 (A previous page, B Locks, C next page).

down does what the button does and lights its indicator in the header; up puts the indicator out. state shows btn=<bit mask> while held. The CoreS3 has no buttons (ERR btn).

* Reply: `OK BTN DOWN | OK BTN UP | ERR btn`

```
btn b down
```

## clicker

`clicker <prev|next|start|blank|end|laser>`

Type the key of a presenter button (Core2 only). It really types on the host!

prev is Page Up, next Page Down, start F5, blank B, end Esc and laser Ctrl+L: the keys of every presentation program and every third-party clicker. Needs a Bluetooth host.

* Reply: `OK CLICKER | ERR clicker`

```
clicker next
```

## pointer

`pointer [on|off|toggle|click [left|right|middle]|scroll <n>|<setting> <value>|feed <yaw> <pitch>]`

The air mouse of the presenter (Core2 only): switch it, tune it, click.

Without arguments it prints the state. The pointer is off until it is switched on. While on, it moves the mouse pointer of the host when the board is turned, and falls asleep by itself after a few seconds of holding still, until it is moved again. Settings: gain 1 to 100 (pixels per degree), idle 0.5 to 30 (seconds still before it sleeps), upright on|off (hold the board upright instead of flat), flipx on|off, flipy on|off. feed pretends the board turns at yaw and pitch degrees per second for 0.6 s (for tests). click presses a mouse button (left by default). scroll turns the vertical wheel n ticks (positive up, negative down).

* Reply: `OK PRESENTER pointer=off|still|moving gain=.. idle=.. | OK POINTER SET | ERR pointer`

```
pointer on
```

## imu

`imu <x> <y> <z>`

Pretend the gyroscope turns around its three axes (degrees per second) for 0.6 s (Core2 only, for tests).

The pointer uses yaw (z when the board is held flat, y when upright) and pitch (x), so imu 0 0 60 is a turn to the side and imu 40 0 0 a tilt.

* Reply: `OK IMU`

```
imu 40 0 60
```

## mod

`mod <ctrl|shift|alt|win|cs|as|meh|hyper|off>`

Hold or release a sticky modifier of the mouse page (Core2 only).

The modifiers stay held until you switch them off, so Ctrl+click and Ctrl+scroll work with the mouse. cs is Ctrl+Shift, as Alt+Shift, meh Ctrl+Shift+Alt, hyper Ctrl+Shift+Alt+Win. The same combination again lets it go; off releases all. They are released when the host goes away.

* Reply: `OK MOD <bit mask> | ERR mod`

```
mod ctrl
```

## shot

`shot`

Screenshot of the screen, 24 bits per pixel.

Prints OK SHOT w h and then w * h * 3 bytes (RGB). The test tools save it as a PNG.

* Reply: `OK SHOT 320 240, then the bytes`

```
shot
```

## perf

`perf`

How long the last page took to build, render and flush.

* Reply: `OK PERF build_us=... render_us=... flush_us=...`

```
perf
```

## wake

`wake`

Restart the idle timers: screen and LED bar on again.

* Reply: `OK WAKE`

```
wake
```

## bl

`bl <16..255|auto>`

Fixed backlight, or auto for the stored dim and off times.

* Reply: `OK BL n | OK BL AUTO | ERR bl`

```
bl 100
```

## leds

`leds [off|locks|on]`

The LED bar: off (also cuts the 5 V rail of the base), only the host lock state, or the layer colours.

Stored in the settings. Without an argument the settings are shown.

* Reply: `OK LEDS OFF | OK LEDS led=on bright=40 ...`

```
leds locks
```

## set

`set [key value]`

Show or change the stored settings.

Keys: led (off|locks|on), effect (none|nightrider|breath|rainbow|comet|sparkle|vu), bright 1..120, ledidle minutes, bl 16..255, dim seconds, off seconds, prefer (usb|ble|both), defaults (any value restores everything). A valid value is stored at once.

* Reply: `OK SET led=on bright=40 ledidle=0 bl=128 dim=60 off=300 prefer=usb effect=none | ERR set`

```
set effect nightrider
```

## prefer

`prefer [usb|ble|both]`

Which link carries the keys when both are connected.

usb: while USB is mounted keys go over USB only (default). ble: while Bluetooth is connected keys go over Bluetooth only. both: to every connected link. Without an argument the choice and the connections are shown.

* Reply: `OK PREFER USB | OK PREFER USB usb=1 ble=1`

```
prefer ble
```

## only

`only <usb|ble>`

Never use the other link, even if the chosen one is down.

* Reply: `OK ONLY USB | OK ONLY BLE | ERR only`

```
only usb
```

## lamps

`lamps [reset | grid <cols> <rows> | get <id> | bright <n> | worker on|off]`

The LampArray: counters, the lamp grid, one lamp, the bar brightness, the bar worker task.

Without an argument: the counters (reports, lamps set, frames, drawing and latency of the bar and the screen). reset clears them and all lamps. grid sets the screen lamp grid (at most 60,800 lamps). get shows the colour of one lamp. bright overrides the bar brightness for a test. worker chooses whether the bar is shown by a task on the second core (default) or by the main loop.

* Reply: `OK LAMPS mode=auto lamps=202 grid=16x12 ... | OK LAMPS GRID c r lamps=n max=n | OK LAMP id r g b i | ERR lamps`

```
lamps grid 160 95
```

## disk

`disk [reset | dir | sound on|off]`

The Disk demo: the state of the transfer, forget it, list the SD card, mute the modem sounds.

Without an argument the state of the transfer is shown: blocks, speeds, the SHA-256 of the board and of the PC, whether the file was saved. See the manual of the Disk demo.

* Reply: `OK DISK state=done ok=74 ... | OK DISK RESET | OK DIR, then the files | OK DISK SOUND ON`

```
disk dir
```

## sd

`sd`

Is there an SD card, which type, how big, how much is used.

The CoreS3 only; the Core2 does not use its card.

* Reply: `OK SD present=1 type=SDHC size_mb=30436 used_mb=1`

```
sd
```

## mic

`mic [on|off]`

Switch the microphone on or off, or show what it hears.

Without an argument: level and peak (0 to 32767), the dominant frequency in Hz from zero crossings, and the number of buffers. The speaker is off while the microphone listens (they share the I2S port on the Core2).

* Reply: `OK MIC active=1 level=... peak=... freq=... buffers=... | OK MIC ON`

```
mic on
```

## wave

`wave`

The last microphone buffer: 512 samples of 16 bits as hex (for tests).

The demo window gets the waveform over the game pad (telem), not from here.

* Reply: `OK WAVE <2048 hex characters>`

```
wave
```

## bar

`bar`

The colours of the ten bar LEDs as hex (for tests).

The demo window gets them over the game pad (telem), not from here.

* Reply: `OK BAR rrggbb rrggbb ... (ten values)`

```
bar
```

## telem

`telem [bar|wave|both|off]`

Send the bar colours and/or the microphone waveform to the PC as game pad reports.

The PC reads them from the HID node (report 11, 48 bytes); this command only switches them on or off. The demo window uses it.

* Reply: `OK TELEM BAR | ERR telem`

```
telem both
```
