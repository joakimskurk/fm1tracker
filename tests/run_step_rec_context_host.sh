#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp2_7 firmware/src/dinghy.c

build/host/fm1tracker_wp2_7 \
    build/host/fm1tracker_wp2_7 \
    tests/wp2_7_step_rec_context.script \
    > build/wp2_7_step_rec_context_host.log

# REC while transport runs is ignored, so the first key remains normal F3
# performance audition. Later HOME gestures use the STEP REC octave (+1).
grep -F "   150 ms  MIDI out 90 53 100" build/wp2_7_step_rec_context_host.log
grep -F "   200 ms  MIDI out 80 53 0" build/wp2_7_step_rec_context_host.log
grep -F "   850 ms  MIDI out 90 65 100" build/wp2_7_step_rec_context_host.log
grep -F "   900 ms  MIDI out 80 65 0" build/wp2_7_step_rec_context_host.log
grep -F "  1300 ms  MIDI out 90 65 100" build/wp2_7_step_rec_context_host.log
grep -F "  1350 ms  MIDI out 80 65 0" build/wp2_7_step_rec_context_host.log

# STEP REC auditions every physical pitch key. OCT+ therefore makes key 0/2
# MIDI 65/67, and the following key 3 gesture auditions MIDI 68.
grep -F "   450 ms  MIDI out 90 65 100" build/wp2_7_step_rec_context_host.log
grep -F "   450 ms  MIDI out 90 67 100" build/wp2_7_step_rec_context_host.log
grep -F "   500 ms  MIDI out 80 65 0" build/wp2_7_step_rec_context_host.log
grep -F "   500 ms  MIDI out 80 67 0" build/wp2_7_step_rec_context_host.log
grep -F "   550 ms  MIDI out 90 68 100" build/wp2_7_step_rec_context_host.log
grep -F "   600 ms  MIDI out 80 68 0" build/wp2_7_step_rec_context_host.log

# PLAY exits STEP REC into ordinary SEQ before resuming, and REC exit while
# stopped also remains in SEQ. These white-key gestures must therefore select
# steps instead of auditioning notes.
! grep -E '^[[:space:]]*(700|750|1150|1200) ms  MIDI out' build/wp2_7_step_rec_context_host.log

# No other unexpected audition events are allowed.
test "$(grep -c 'MIDI out' build/wp2_7_step_rec_context_host.log)" -eq 12

echo "WP2_7_STEP_REC_CONTEXT_HOST_OK=1"
