#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp2_5 firmware/src/dinghy.c

build/host/fm1tracker_wp2_5 \
    build/host/fm1tracker_wp2_5 \
    tests/wp2_5_pattern_context.script \
    > build/wp2_5_pattern_context_host.log

# Exactly two performance note gestures should reach MIDI out. The first proves
# baseline performance audition; the second proves a release still terminates a
# note after SEQ is entered while that physical key remains held.
test "$(grep -c 'MIDI out' build/wp2_5_pattern_context_host.log)" -eq 4
grep -F "   100 ms  MIDI out 90 53 100" build/wp2_5_pattern_context_host.log
grep -F "   150 ms  MIDI out 80 53 0" build/wp2_5_pattern_context_host.log
grep -F "   600 ms  MIDI out 90 53 100" build/wp2_5_pattern_context_host.log
grep -F "   700 ms  MIDI out 80 53 0" build/wp2_5_pattern_context_host.log

# The white key at 250 ms and black key at 350 ms occurred in SEQ context and
# therefore must not audition or emit MIDI.
! grep -E '^[[:space:]]*(250|300|350|400) ms  MIDI out' build/wp2_5_pattern_context_host.log

echo "WP2_5_PATTERN_CONTEXT_HOST_OK=1"
