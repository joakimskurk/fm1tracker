#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp2_10 firmware/src/dinghy.c

build/host/fm1tracker_wp2_10 \
    build/host/fm1tracker_wp2_10_integration \
    tests/wp2_10_phase2_integration.script \
    > build/wp2_10_phase2_integration_host.log

build/host/fm1tracker_wp2_10 \
    build/host/fm1tracker_wp2_10_home_control \
    tests/wp2_10_home_equivalent_control.script \
    > build/wp2_10_home_control_host.log

# The 64 white-step traversal plus black-key selector must never leak audition
# MIDI in ordinary SEQ. Only the four deliberate STEP REC pitches may appear.
on_count="$(grep -c 'MIDI out 90' build/wp2_10_phase2_integration_host.log)"
off_count="$(grep -c 'MIDI out 80' build/wp2_10_phase2_integration_host.log)"
test "$on_count" -eq 4
test "$off_count" -eq 4
grep -F 'MIDI out 90 53 100' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 80 53 0' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 90 55 100' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 80 55 0' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 90 60 100' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 80 60 0' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 90 64 100' build/wp2_10_phase2_integration_host.log
grep -F 'MIDI out 80 64 0' build/wp2_10_phase2_integration_host.log

# The tracker-context run must relinquish every pixel to HOME. The control run
# has the same legitimate last-note state (E4) but never enters SEQ.
cmp build/host/fm1tracker_wp2_10_integration-3000.ppm \
    build/host/fm1tracker_wp2_10_home_control-3000.ppm

echo "WP2_10_HOME_RETURN_PIXEL_IDENTICAL=1"

tracker_sha="$(sha256sum build/host/fm1tracker_wp2_10_integration-1000.ppm | awk '{print $1}')"
home_sha="$(sha256sum build/host/fm1tracker_wp2_10_integration-3000.ppm | awk '{print $1}')"
control_sha="$(sha256sum build/host/fm1tracker_wp2_10_home_control-3000.ppm | awk '{print $1}')"

echo "WP2_10_INTEGRATION_TRACKER_SHA256=$tracker_sha"
echo "WP2_10_INTEGRATION_HOME_SHA256=$home_sha"
echo "WP2_10_HOME_CONTROL_SHA256=$control_sha"

test "$tracker_sha" = "a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7"
test "$home_sha" = "ea33024427340416c17a73ecfb2303e37262b8c61e2f2d30ca06265ba6e0b899"
test "$control_sha" = "$home_sha"

echo "WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1"
echo "WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8"
