#!/usr/bin/env bash
# Send one or more serial commands to a board and print the one-line replies.
#
#   tools/serial.sh S3 "LAYER 26" "STATE?"      # S3 or C2 (core2/cores3 also work)
#
# Text commands only (PING, INFO, STATE?, LAYER n, TAP x y, KEY l i, PREFER ..., ONLY ...); SHOT is binary,
# use `jbang tools/SerialTest.java --shot DIR` for screenshots.
# Run it as ONE plain command (no pipe, cd, &&, ;, VAR=x prefix) so that it matches "tools/serial.sh*" in
# sandbox.excludedCommands and runs outside the Claude Code sandbox.
set -euo pipefail
here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=tools/lib-ports.sh
source "$here/lib-ports.sh"

[ $# -ge 2 ] || die "usage: tools/serial.sh S3|C2 \"COMMAND\" [\"COMMAND\" ...]" 2
board="$1"; shift
id=$(usb_id_for_board "$board")
port=$(find_port "$id") || die "no $board board on USB"
die_if_unusable "$port"

stty -F "$port" 115200 raw -echo -hupcl
out=$(mktemp); trap 'rm -f "$out"; kill "$reader" 2>/dev/null || true' EXIT
timeout $((${#} * 2 + 3)) cat "$port" > "$out" &
reader=$!
sleep 0.3
for cmd in "$@"; do
  printf '%s\n' "$cmd" > "$port"
  sleep 0.6
done
sleep 0.5
tr -d '\r' < "$out" | grep -a -E '^(OK|ERR)' || echo "(no reply: is the firmware running, and is another program using $port?)"
