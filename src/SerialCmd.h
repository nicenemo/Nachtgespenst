#pragma once
#include <Arduino.h>

// Line protocol on Serial (115200, '\n' terminated, '\r' ignored). Replies start with OK or ERR.
//   PING                 -> OK PONG
//   INFO                 -> OK INFO board=.. name=.. fw=.. w=.. h=.. pages=.. locks=..
//   STATE?               -> OK STATE page=.. title=.. caps= num= scroll= last= presses= conn=
//   LAYER <n>            -> OK LAYER <n>            (switch page, 0..pages-1)
//   TAP <x> <y>          -> OK TAP                  (injected ~120 ms touch)
//   KEY <layer> <idx>    -> OK KEY | ERR            (sends the real HID key!)
//   SHOT                 -> OK SHOT w h\n<RGB888>\nEND\n
// WARNING: KEY/TAP on keyboard-page tiles really type on the host. For tests use the
// Locks page (index = pages-1; Caps/Num/Scroll) and prefer Scroll Lock (idx 2).
void serialcmd_poll();   // non-blocking; call every loop()
