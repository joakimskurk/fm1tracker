#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_5 firmware/src/dinghy.c

build/host/fm1tracker_wp3_5 \
    build/host/fm1tracker_wp3_5_control \
    tests/wp3_5_locked_audio_control.script \
    > build/wp3_5_locked_audio_control.log

build/host/fm1tracker_wp3_5 \
    build/host/fm1tracker_wp3_5_locked \
    tests/wp3_5_locked_audio.script \
    > build/wp3_5_locked_audio.log

test "$(grep -c 'MIDI out' build/wp3_5_locked_audio_control.log || true)" -eq 0
test "$(grep -c 'MIDI out' build/wp3_5_locked_audio.log || true)" -eq 0

test -s build/host/fm1tracker_wp3_5_control.wav
test -s build/host/fm1tracker_wp3_5_locked.wav

python3 - <<'PY' | tee build/wp3_5_locked_audio_analysis.txt
from array import array
from hashlib import sha256
from pathlib import Path
import wave

CONTROL_SHA = '5545ea2322ff2f01b77bce0c94ed8ae379386eec1baea0f712b6daf000baf3fd'
LOCKED_SHA = '8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8'
FROZEN_FRAMES = 52736


def read_left(path):
    with wave.open(str(path), 'rb') as f:
        assert f.getnchannels() == 2
        assert f.getsampwidth() == 2
        assert f.getframerate() == 44100
        n = f.getnframes()
        raw = f.readframes(n)
    samples = array('h')
    samples.frombytes(raw)
    return n, samples[0::2]


def crossings(values):
    prev = 0
    count = 0
    for x in values:
        sign = 1 if x > 0 else -1 if x < 0 else 0
        if not sign:
            continue
        if prev and sign != prev:
            count += 1
        prev = sign
    return count

control_path = Path('build/host/fm1tracker_wp3_5_control.wav')
locked_path = Path('build/host/fm1tracker_wp3_5_locked.wav')
cn, control = read_left(control_path)
ln, locked = read_left(locked_path)
assert cn == ln == FROZEN_FRAMES, (cn, ln)

# The first changed row is zero-based step 4, boundary frame 22050 at 120 BPM.
assert control[:22050] == locked[:22050]

# Late in row 5 the combined 128/128 level lock is settled and FINE=192
# must raise the zero-crossing count relative to the unshifted control.
lo, hi = 24500, 27000
control_abs = sum(abs(x) for x in control[lo:hi])
locked_abs = sum(abs(x) for x in locked[lo:hi])
control_zc = crossings(control[lo:hi])
locked_zc = crossings(locked[lo:hi])
assert locked_abs * 2 < control_abs, (locked_abs, control_abs)
assert locked_zc > control_zc, (locked_zc, control_zc)

# Step 5 is EMPTY plus VOICE=0. After its click-safe fade the proof lane is
# exactly silent, while the control continues sustaining the note.
lo5, hi5 = 28000, 32500
assert max(abs(x) for x in locked[lo5:hi5]) == 0
assert max(abs(x) for x in control[lo5:hi5]) > 100

# Step 6 is unlocked EMPTY. D031 row-local restore must reopen the still-active
# note at default level without a new NOTE event.
lo6, hi6 = 33500, 38000
assert max(abs(x) for x in locked[lo6:hi6]) > 100

locked_sha = sha256(locked_path.read_bytes()).hexdigest()
control_sha = sha256(control_path.read_bytes()).hexdigest()
assert control_sha == CONTROL_SHA, control_sha
assert locked_sha == LOCKED_SHA, locked_sha
print(f'WP3_5_LOCKED_AUDIO_CONTROL_SHA256={control_sha}')
print(f'WP3_5_LOCKED_AUDIO_SHA256={locked_sha}')
print(f'WP3_5_LOCKED_AUDIO_FRAMES={ln}')
print(f'WP3_5_ROW4_CONTROL_ABS={control_abs}')
print(f'WP3_5_ROW4_LOCKED_ABS={locked_abs}')
print(f'WP3_5_ROW4_CONTROL_ZC={control_zc}')
print(f'WP3_5_ROW4_LOCKED_ZC={locked_zc}')
print('WP3_5_LOCKED_AUDIO_BEHAVIOR_OK=1 pre_equal=1 combined_level=1 fine_pitch=1 empty_mute=1 unlocked_restore=1 midi_events=0')
PY
