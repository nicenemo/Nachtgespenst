#!/usr/bin/env bash
# Shared helpers for tools/flash.sh and tools/serial.sh (source this file, do not run it).
#
# The boards are found by USB vendor:product ID through sysfs, so it does not matter whether the CoreS3
# is /dev/ttyACM0 or ttyACM1 today. sysfs is visible inside the Claude Code sandbox, the /dev nodes
# are not: that is how `die_if_sandboxed` recognises the sandbox.

usb_id_for_board() {
  case "$1" in
    C2|c2|core2)   echo "10c4:ea60" ;;   # CP2104 UART bridge
    S3|s3|cores3)  echo "303a:1001" ;;   # ESP32-S3 native USB (firmware, and ROM bootloader after the 1200-baud touch)
    *) echo "unknown board '$1' (use core2|cores3 or C2|S3)" >&2; return 1 ;;
  esac
}

# Prints /dev/ttyXXX for the first serial port whose USB device has the wanted vendor:product ID.
find_port() {
  local want="$1" t d vid pid
  for t in /sys/class/tty/ttyACM* /sys/class/tty/ttyUSB*; do
    [ -e "$t" ] || continue
    d=$(readlink -f "$t")
    while [ "$d" != "/" ] && [ ! -e "$d/idVendor" ]; do d=$(dirname "$d"); done
    [ -e "$d/idVendor" ] || continue
    vid=$(cat "$d/idVendor"); pid=$(cat "$d/idProduct")
    if [ "$vid:$pid" = "$want" ]; then echo "/dev/$(basename "$t")"; return 0; fi
  done
  return 1
}

# Waits until the board shows up as a serial port (default 25 s). Prints the port.
wait_for_port() {
  local want="$1" limit="${2:-25}" i port
  for ((i = 0; i < limit * 2; i++)); do
    if port=$(find_port "$want"); then echo "$port"; return 0; fi
    sleep 0.5
  done
  return 1
}

die() { echo "error: $1" >&2; exit "${2:-1}"; }

# The port is listed in sysfs but cannot be opened: the command runs inside the sandbox (or lacks uucp rights).
die_if_unusable() {
  local port="$1"
  if [ ! -e "$port" ]; then
    die "$port is listed by the kernel but not visible here: this command is running inside the Claude Code sandbox.
       Run it as a single plain command that matches an entry in sandbox.excludedCommands (see docs/wiki/claude-code-workflow.md)." 3
  fi
  if [ ! -r "$port" ] || [ ! -w "$port" ]; then
    die "cannot read/write $port: is your user in the uucp group?" 3
  fi
}
