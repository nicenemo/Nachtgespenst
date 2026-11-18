#!/usr/bin/env bash
# Flash an already built Nachtgespenst firmware image to a board and wait until it is back.
#
#   tools/flash.sh core2|cores3 [--image FILE] [--dry-run]
#
# Build first (about 12 minutes per environment):  .pio/venv/bin/pio run -e cores3
# - core2:  the CP2104 bridge resets the chip into the bootloader by itself.
# - cores3: native USB (TinyUSB) cannot do that, so a 1200-baud touch puts the ROM bootloader on the port first.
# Only the application is written (0x10000); bootloader and partition table stay as they are.
#
# Run it as ONE plain command (no pipe, cd, &&, ;, VAR=x prefix) so that it matches the entry
# "tools/flash.sh*" in sandbox.excludedCommands and runs outside the Claude Code sandbox.
set -euo pipefail
here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
root=$(dirname "$here")
# shellcheck source=tools/lib-ports.sh
source "$here/lib-ports.sh"

env_name="${1:-}"; [ -n "$env_name" ] || die "usage: tools/flash.sh core2|cores3 [--image FILE] [--dry-run]" 2
shift
image=""; dry=0
while [ $# -gt 0 ]; do
  case "$1" in
    --image) image="$2"; shift 2 ;;
    --dry-run) dry=1; shift ;;
    *) die "unknown option $1" 2 ;;
  esac
done

case "$env_name" in
  core2|C2|c2)   env_name=core2;  chip=esp32;   baud=460800; id=$(usb_id_for_board C2) ;;
  cores3|S3|s3)  env_name=cores3; chip=esp32s3; baud=921600; id=$(usb_id_for_board S3) ;;
  *) die "unknown board '$env_name'" 2 ;;
esac

[ -n "$image" ] || image="$root/.pio/build/$env_name/firmware.bin"
[ -f "$image" ] || die "no image at $image (build it first: .pio/venv/bin/pio run -e $env_name)"

# A stale image is how the wrong UI got flashed once: say clearly what is about to happen.
echo "branch : $(git -C "$root" branch --show-current 2>/dev/null || echo '?')"
echo "image  : $image ($(stat -c '%s bytes, %y' "$image" | cut -c1-40))"
stale=$(find "$root/src" "$root/include" "$root/platformio.ini" -newer "$image" -type f -not -path '*/hid_*' -print -quit 2>/dev/null || true)
[ -z "$stale" ] || echo "WARNING: $stale is newer than the image: rebuild before flashing unless that is intended"

esptool="$root/.pio/core/packages/tool-esptool/esptool"
[ -x "$esptool" ] || esptool=$(command -v esptool || true)
[ -n "$esptool" ] || die "esptool not found (run a pio build once so PlatformIO installs it)"

port=$(find_port "$id") || die "no $env_name board on USB (looked for $id): plugged in? cable carries data? drivers loaded (cp210x, cdc_acm)?"
echo "port   : $port"
if [ "$dry" = 1 ]; then echo "dry run: would flash $image to $port with $esptool --chip $chip --baud $baud write-flash 0x10000"; exit 0; fi
die_if_unusable "$port"

if [ "$env_name" = cores3 ]; then
  echo "step   : 1200-baud touch (ROM bootloader)"
  stty -F "$port" 1200 || true
  sleep 3
  port=$(wait_for_port "$id" 15) || die "the CoreS3 did not come back after the 1200-baud touch (hold the side button ~3 s for download mode)"
fi

echo "step   : writing"
"$esptool" --chip "$chip" --port "$port" --baud "$baud" write-flash --no-progress 0x10000 "$image"

echo "step   : waiting for the board to boot"
sleep 3
port=$(wait_for_port "$id" 25) || die "the board did not reappear after flashing"
sleep 4   # firmware start + CDC enumeration; serial tests started earlier lose their first reply
echo "done   : $env_name is up on $port"
