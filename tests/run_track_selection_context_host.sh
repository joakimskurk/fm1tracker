#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp2_6 firmware/src/dinghy.c

build/host/fm1tracker_wp2_6 \
    build/host/fm1tracker_wp2_6 \
    tests/wp2_6_track_context.script \
    > build/wp2_6_track_context_host.log

# Exactly two PERFORMANCE black-key note gestures should reach MIDI out.
test "$(grep -c 'MIDI out' build/wp2_6_track_context_host.log)" -eq 4
grep -F "   100 ms  MIDI out 90 54 100" build/wp2_6_track_context_host.log
grep -F "   150 ms  MIDI out 80 54 0" build/wp2_6_track_context_host.log
grep -F "   600 ms  MIDI out 90 54 100" build/wp2_6_track_context_host.log
grep -F "   700 ms  MIDI out 80 54 0" build/wp2_6_track_context_host.log

# Designated black, white-step and reserved-black gestures occurred in SEQ and
# therefore must not audition or emit MIDI.
! grep -E '^[[:space:]]*(250|300|350|400|450|500) ms  MIDI out' build/wp2_6_track_context_host.log

echo "WP2_6_TRACK_CONTEXT_HOST_OK=1"
