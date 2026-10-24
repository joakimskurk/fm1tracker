#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_8 firmware/src/dinghy.c

cat > build/wp3_8_control.script <<'EOF'
900 end
EOF
cat > build/wp3_8_fifo.script <<'EOF'
20 midi 144 60 100
20 midi 128 60 0
900 end
EOF
cat > build/wp3_8_hardware.script <<'EOF'
20 keys 128
100 keys 0
900 end
EOF
cat > build/wp3_8_midi.script <<'EOF'
20 midi 144 64 100
100 midi 128 64 0
900 end
EOF
cat > build/wp3_8_overflow.script <<'EOF'
20 midi 144 60 100
EOF
# More than the frozen 32-event capacity in one main-loop poll. Notes are
# deliberately left on; only the overflow fail-to-silence rule can settle them.
i=0
while [ "$i" -lt 40 ]; do
    note=$((60 + (i % 40)))
    printf '60 midi 144 %u 100\n' "$note" >> build/wp3_8_overflow.script
    i=$((i + 1))
done
cat >> build/wp3_8_overflow.script <<'EOF'
900 end
EOF

cat > build/wp3_8_recovery.script <<'EOF'
20 midi 144 60 100
EOF
i=0
while [ "$i" -lt 40 ]; do
    note=$((60 + (i % 40)))
    printf '60 midi 144 %u 100\n' "$note" >> build/wp3_8_recovery.script
    i=$((i + 1))
done
cat >> build/wp3_8_recovery.script <<'EOF'
220 midi 144 72 100
300 midi 128 72 0
900 end
EOF

for name in control fifo hardware midi overflow recovery; do
    build/host/fm1tracker_wp3_8 \
        "build/host/fm1tracker_wp3_8_${name}" \
        "build/wp3_8_${name}.script" \
        > "build/wp3_8_${name}.log"
    test -s "build/host/fm1tracker_wp3_8_${name}.wav"
done

# Queue consumption itself must never emit MIDI. Only the physical hardware-key
# fixture has the historical explicit USB-MIDI echo, exactly NOTE ON + NOTE OFF.
test "$(grep -c 'MIDI out' build/wp3_8_control.log || true)" -eq 0
test "$(grep -c 'MIDI out' build/wp3_8_fifo.log || true)" -eq 0
test "$(grep -c 'MIDI out' build/wp3_8_midi.log || true)" -eq 0
test "$(grep -c 'MIDI out' build/wp3_8_overflow.log || true)" -eq 0
test "$(grep -c 'MIDI out' build/wp3_8_recovery.log || true)" -eq 0
test "$(grep -c 'MIDI out' build/wp3_8_hardware.log || true)" -eq 2

python3 - <<'PY' | tee build/wp3_8_queue_audio_analysis.txt
from hashlib import sha256
from pathlib import Path
import wave

FS = 44100
NAMES = ('control', 'fifo', 'hardware', 'midi', 'overflow', 'recovery')


def read(name):
    p = Path(f'build/host/fm1tracker_wp3_8_{name}.wav')
    with wave.open(str(p), 'rb') as w:
        assert w.getnchannels() == 2
        assert w.getsampwidth() == 2
        assert w.getframerate() == FS
        frames = w.getnframes()
        raw = w.readframes(frames)
    return p, frames, raw


def frame(ms):
    return (ms * FS + 999) // 1000


def differs(a, b, start_ms, end_ms):
    a0 = frame(start_ms) * 4
    b0 = frame(end_ms) * 4
    return a[a0:b0] != b[a0:b0]

loaded = {n: read(n) for n in NAMES}
frame_counts = {v[1] for v in loaded.values()}
assert len(frame_counts) == 1, frame_counts
frames = frame_counts.pop()
control = loaded['control'][2]

# Same-callback NOTE ON then NOTE OFF must consume FIFO before the first render
# and therefore be fully silent relative to the tracker-only control.
assert loaded['fifo'][2] == control

# Both real producer paths must make audible manual output, then settle back to
# the byte-identical tracker proof once their existing click-safe release ends.
for name in ('hardware', 'midi'):
    raw = loaded[name][2]
    assert differs(raw, control, 20, 150), name
    assert raw[frame(250) * 4:] == control[frame(250) * 4:], name

# The 40-event burst is larger than the frozen queue. Because the batch contains
# no NOTE OFF events, a reject-newest implementation without overflow recovery
# would leave audition voices held indefinitely. D034 must instead fail to
# silence and become byte-identical to the tracker-only control after the release.
overflow = loaded['overflow'][2]
assert differs(overflow, control, 20, 150)
assert overflow[frame(200) * 4:] == control[frame(200) * 4:]

# After that forced overflow, a later normal NOTE ON/OFF must still work; this
# proves the consumer discard is a recovery operation rather than a dead queue.
recovery = loaded['recovery'][2]
assert differs(recovery, control, 220, 340)
assert recovery[frame(450) * 4:] == control[frame(450) * 4:]

for name in NAMES:
    p, _, _ = loaded[name]
    print(f'WP3_8_{name.upper()}_WAV_SHA256={sha256(p.read_bytes()).hexdigest()}')
print(f'WP3_8_QUEUE_AUDIO_FRAMES={frames}')
print('WP3_8_QUEUE_AUDIO_BEHAVIOR_OK=1 fifo=1 hardware=1 midi=1 overflow_silence=1 recovery=1 queue_midi_neutral=1')
PY
