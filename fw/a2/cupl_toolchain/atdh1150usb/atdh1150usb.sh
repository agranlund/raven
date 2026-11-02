#!/usr/bin/env bash
# Flash an SVF to an ATF150x CPLD via the Atmel ATDH1150USB + OpenOCD.
#
# Usage: atdh1150usb.sh [-d <device>] <file.svf>
#   device: ATF1502AS | ATF1504AS | ATF1508AS (default)
set -euo pipefail

HERE="$(dirname "$(readlink -f "$0")")"
CFG="$HERE/atdh1150usb.cfg"

DEVICE=ATF1508AS
usage() { echo "usage: $(basename "$0") [-d ATF1502AS|ATF1504AS|ATF1508AS] <file.svf>" >&2; exit 1; }
while getopts "d:h" opt; do
  case "$opt" in
    d) DEVICE="$OPTARG" ;;
    *) usage ;;
  esac
done
shift $((OPTIND - 1))
[ $# -eq 1 ] || usage
SVF="$1"

case "$(echo "$DEVICE" | tr '[:lower:]' '[:upper:]')" in
  ATF1502AS|1502|1502AS) IDCODE=0x0150203f ;;
  ATF1504AS|1504|1504AS) IDCODE=0x0150403f ;;
  ATF1508AS|1508|1508AS) IDCODE=0x0150803f ;;
  *) echo "unknown device: $DEVICE" >&2; usage ;;
esac

DIR=0x0ffb             # all JTAG + ACBUS0-3 + AD4-7 outputs; TDO input
# layout_init data
AMBER=0x02f8           # RED on  + GREEN on
GREEN=0x06f8           # GREEN on (RED off)  -> success
RED=0x0af8             # RED on   (GREEN off) -> failure

led() {  # $1 = layout_init data
  openocd -f "$CFG" -c "ftdi layout_init $1 $DIR" \
    -c "jtag newtap atf tap -irlen 10" \
    -c "catch { init }" -c shutdown >/dev/null 2>&1 || true
}

log="$(mktemp)"
trap 'rm -f "$log"' EXIT

echo ">>> Programming $(basename "$SVF")..."
openocd -f "$CFG" -c "ftdi layout_init $AMBER $DIR" \
  -c "jtag newtap atf tap -irlen 10 -expected-id $IDCODE" \
  -c init -c "svf $SVF" -c shutdown 2>&1 | tee "$log" || true

if grep -q "programmed successfully" "$log" && grep -q "with 0 errors" "$log"; then
  echo ">>> SUCCESS"
  led "$GREEN"
  exit 0
else
  echo ">>> FAILED" >&2
  led "$RED"
  exit 1
fi
