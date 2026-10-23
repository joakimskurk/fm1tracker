#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp2_8 firmware/src/dinghy.c

build/host/fm1tracker_wp2_8 \
    build/host/fm1tracker_wp2_8 \
    tests/wp2_8_special_events_context.script \
    > build/wp2_8_special_events_context_host.log

# Special-event buttons never audition notes. The only manual MIDI gesture in
# the fixture is the ordinary F3 note written immediately after NOTE OFF.
grep -F "   550 ms  MIDI out 90 53 100" build/wp2_8_special_events_context_host.log
grep -F "   600 ms  MIDI out 80 53 0" build/wp2_8_special_events_context_host.log
test "$(grep -c 'MIDI out' build/wp2_8_special_events_context_host.log)" -eq 2

# Playback resumes with rows REST, TIE, OFF, F3. REST/TIE leave the already
# sounding proof note alone, OFF releases it to exact silence, and the following
# F3 row re-opens the proof gate. This also catches an accidental second REST on
# FX release after the FX+SELECT OFF gesture: that bug would move F3 to step 4
# and the post-OFF window below would still be silent.
python3 - <<'PY'
import array
import wave
from pathlib import Path

path = Path("build/host/fm1tracker_wp2_8.wav")
assert path.is_file() and path.stat().st_size > 44
with wave.open(str(path), "rb") as w:
    assert w.getnchannels() == 2
    assert w.getsampwidth() == 2
    rate = w.getframerate()
    frames = w.getnframes()
    data = array.array("h", w.readframes(frames))

def nonzero(ms0, ms1):
    a = int(rate * ms0 / 1000) * 2
    b = int(rate * ms1 / 1000) * 2
    return sum(1 for x in data[a:b] if x != 0)

before_off = nonzero(1220, 1270)
quiet_after_off = nonzero(1340, 1400)
after_next_note = nonzero(1460, 1510)
assert before_off > 0, before_off
assert quiet_after_off == 0, quiet_after_off
assert after_next_note > 0, after_next_note
print(f"WP2_8_OFF_AUDIO_BEHAVIOR_OK=1 before={before_off} quiet={quiet_after_off} after={after_next_note}")
PY

echo "WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1"
