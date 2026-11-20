# The Core2 as a presentation clicker and air mouse

The Core2 has one page and talks to the computer over Bluetooth only (it has no USB data connection). It types the keys of a presentation clicker and moves the mouse pointer when you turn it. It works in PowerPoint, Keynote, Google Slides and in the demo window of this project, because the keys are the ones every third-party clicker sends.

## The page

| Block | Key it types | Physical button |
|---|---|---|
| Previous | Page Up | A |
| Next | Page Down | C |
| Start | F5 (start the slide show) | |
| Blank | B (black screen, again to come back) | |
| End | Esc | |
| Pointer | switches the air mouse on and off | B |
| Laser | Ctrl+L (the laser pointer of PowerPoint) | |
| Click | left mouse button | |

The header shows the title, whether a host is connected (`BLE` or `no BLE`) and three indicators A, B, C that light up while the buttons are held. A held block turns amber.

## The air mouse

* The pointer is off until you switch it on (block or button B); the setting is remembered.
* While on, turning the board moves the pointer: turn left and it moves left, tilt the nose up and it moves up. Hold the board flat with the screen up, like a remote.
* Held still for a few seconds (`idle`, default 2 s) the pointer falls asleep: it sends nothing, so it cannot drift, and the block turns lavender. A deliberate movement wakes it (amber). Small bumps do not.
* The gyroscope learns its own zero point while the board is held still.
* Tuning over the serial shell (`jbang tools/Shell.java C2`): `pointer gain 20` (pixels per degree), `pointer idle 2`, `pointer upright on` (hold the board upright like a phone instead of flat), `pointer flipx on`, `pointer flipy on`. If a direction is wrong on your board, flip it.

## Pairing

The report map changed (a mouse was added), so a host that knew the Core2 before has to forget it and pair again: on Linux `bluetoothctl remove <address>`, on Windows remove the device in the Bluetooth settings. The Core2 advertises as the moon and ghost followed by `-c2-clicker`.

## Tests

`jbang tools/SerialTest.java --board C2` checks the page, the pressed states (`down`, `up`, `btn b down`), and the pointer with `pointer feed` (a pretend rotation): it wakes, falls asleep by itself, sends nothing while asleep or off. It does not type on the host. `clicker <action>` and `pointer click` do type or click on a connected host; use them deliberately. The movement of a real board and the keys in PowerPoint have to be tried by hand.
