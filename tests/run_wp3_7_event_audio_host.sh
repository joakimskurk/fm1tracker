#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_7 firmware/src/dinghy.c

cat > build/wp3_7_control.script <<'EOF'
900 end
EOF
cat > build/wp3_7_delay3.script <<'EOF'
1 effect 0 7 3
900 end
EOF
cat > build/wp3_7_cut3.script <<'EOF'
1 effect 0 8 3
900 end
EOF
cat > build/wp3_7_cut0.script <<'EOF'
1 effect 0 8 0
900 end
EOF
cat > build/wp3_7_retrig2.script <<'EOF'
1 effect 0 6 2
900 end
EOF
cat > build/wp3_7_delay6.script <<'EOF'
1 effect 0 7 6
900 end
EOF
cat > build/wp3_7_cut6.script <<'EOF'
1 effect 0 8 6
900 end
EOF
cat > build/wp3_7_retrig6.script <<'EOF'
1 effect 0 6 6
900 end
EOF

for name in control delay3 cut3 cut0 retrig2 delay6 cut6 retrig6; do
    build/host/fm1tracker_wp3_7 \
        "build/host/fm1tracker_wp3_7_${name}" \
        "build/wp3_7_${name}.script" \
        > "build/wp3_7_${name}.log"
    test "$(grep -c 'MIDI out' "build/wp3_7_${name}.log" || true)" -eq 0
    test -s "build/host/fm1tracker_wp3_7_${name}.wav"
done

python3 - <<'PY' | tee build/wp3_7_event_audio_analysis.txt
from array import array
from hashlib import sha256
from pathlib import Path
import wave

FS = 44100
BPM = 120
ROW_PHASE = 661500
TICK_PHASE = 110250
NAMES = ('control', 'delay3', 'cut3', 'cut0', 'retrig2', 'delay6', 'cut6', 'retrig6')


def ceil_div(a, b):
    return (a + b - 1) // b


def row_tick_starts(row):
    base = row * ROW_PHASE
    return [ceil_div(base + tick * TICK_PHASE, BPM) for tick in range(7)]


def read_left(name):
    path = Path(f'build/host/fm1tracker_wp3_7_{name}.wav')
    with wave.open(str(path), 'rb') as f:
        assert f.getnchannels() == 2
        assert f.getsampwidth() == 2
        assert f.getframerate() == FS
        n = f.getnframes()
        raw = f.readframes(n)
    x = array('h')
    x.frombytes(raw)
    return path, n, x[0::2]


def first_diff(a, b):
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i
    return -1


def first_nonzero(x, start=0):
    for i in range(start, len(x)):
        if x[i] != 0:
            return i
    return -1

loaded = {name: read_left(name) for name in NAMES}
frames = {v[1] for v in loaded.values()}
assert len(frames) == 1, frames
frame_count = frames.pop()
assert frame_count > 39000, frame_count
control = loaded['control'][2]
delay3 = loaded['delay3'][2]
cut3 = loaded['cut3'][2]
cut0 = loaded['cut0'][2]
retrig2 = loaded['retrig2'][2]

ticks0 = row_tick_starts(0)
row4 = row_tick_starts(4)[0]

# NOTE DELAY 3: row-0 NOTE is withheld from silence until the exact D032 tick.
assert all(v == 0 for v in delay3[:ticks0[3]]), first_nonzero(delay3)
fd_delay = first_nonzero(delay3)
assert ticks0[3] <= fd_delay < ticks0[4], (fd_delay, ticks0)

# NOTE CUT 3: ordinary row dispatch is unchanged through tick 3, then the
# synthetic OFF starts a click-safe release and cannot resurrect before row 4.
assert cut3[:ticks0[3]] == control[:ticks0[3]]
fd_cut = first_diff(control, cut3)
assert ticks0[3] <= fd_cut < ticks0[4], (fd_cut, ticks0)
assert all(v == 0 for v in cut3[ticks0[5]:row4]), 'cut3 did not settle before row4'

# NOTE CUT 0 owns tick 0 after the ordinary NOTE, so no row-0 sample may sound.
assert all(v == 0 for v in cut0[:row4]), first_nonzero(cut0)
assert first_nonzero(cut0, row4) >= row4

# RETRIG 2 resets phase on ticks 2 and 4. It must be sample-identical before
# tick 2 and diverge before tick 3 without changing gate or MIDI behavior.
assert retrig2[:ticks0[2]] == control[:ticks0[2]]
fd_retrig = first_diff(control, retrig2)
assert ticks0[2] <= fd_retrig < ticks0[3], (fd_retrig, ticks0)

# Values > 5 are explicit scheduling no-ops: the ordinary row event survives
# unchanged and the full render remains byte-identical to the control.
for name in ('delay6', 'cut6', 'retrig6'):
    path = loaded[name][0]
    assert loaded[name][2] == control, name
    assert path.read_bytes() == loaded['control'][0].read_bytes(), name

for name in NAMES:
    path, _, _ = loaded[name]
    print(f'WP3_7_{name.upper()}_WAV_SHA256={sha256(path.read_bytes()).hexdigest()}')
print(f'WP3_7_EVENT_AUDIO_FRAMES={frame_count}')
print(f'WP3_7_DELAY3_FIRST_NONZERO={fd_delay}')
print(f'WP3_7_CUT3_FIRST_DIFF={fd_cut}')
print(f'WP3_7_RETRIG2_FIRST_DIFF={fd_retrig}')
print('WP3_7_INVALID_VALUE_AUDIO_IDENTICAL=1')
print('WP3_7_EVENT_AUDIO_BEHAVIOR_OK=1 delay=1 cut=1 cut0=1 retrig=1 invalid_noop=1 midi_events=0')
PY
