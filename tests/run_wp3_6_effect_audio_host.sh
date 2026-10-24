#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_6 firmware/src/dinghy.c

for name in control arp slide porta vibrato; do
    build/host/fm1tracker_wp3_6 \
        "build/host/fm1tracker_wp3_6_${name}" \
        "tests/wp3_6_effect_${name}.script" \
        > "build/wp3_6_effect_${name}.log"
    test "$(grep -c 'MIDI out' "build/wp3_6_effect_${name}.log" || true)" -eq 0
    test -s "build/host/fm1tracker_wp3_6_${name}.wav"
done

python3 - <<'PY' | tee build/wp3_6_effect_audio_analysis.txt
from array import array
from hashlib import sha256
from pathlib import Path
import wave

NAMES = ('control', 'arp', 'slide', 'porta', 'vibrato')


def read_left(name):
    path = Path(f'build/host/fm1tracker_wp3_6_{name}.wav')
    with wave.open(str(path), 'rb') as f:
        assert f.getnchannels() == 2
        assert f.getsampwidth() == 2
        assert f.getframerate() == 44100
        n = f.getnframes()
        raw = f.readframes(n)
    x = array('h')
    x.frombytes(raw)
    return path, n, x[0::2]


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


def first_diff(a, b):
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i
    return -1

loaded = {name: read_left(name) for name in NAMES}
frames = {v[1] for v in loaded.values()}
assert len(frames) == 1, frames
frame_count = frames.pop()
assert frame_count > 39000, frame_count
control = loaded['control'][2]

# D032 internal effect tick starts for 120 BPM. Changes become audible on the
# next rendered frame after the transport phase crosses each threshold.
tick_starts = [0, 919, 1838, 2757, 3675, 4594, 5513]

def tick_crossings(samples):
    out = []
    for a, b in zip(tick_starts[:-1], tick_starts[1:]):
        out.append(crossings(samples[a + 100:b - 100]))
    return out

arp = loaded['arp'][2]
slide = loaded['slide'][2]
porta = loaded['porta'][2]
vibrato = loaded['vibrato'][2]

# ARP/slide/vibrato tick 0 is deliberately unmodulated. Their first sample
# difference from control must therefore occur no earlier than tick 1.
for name, samples in (('arp', arp), ('slide', slide), ('vibrato', vibrato)):
    assert samples[:tick_starts[1]] == control[:tick_starts[1]], name
    fd = first_diff(control, samples)
    assert tick_starts[1] <= fd < tick_starts[2], (name, fd)

arp_zc = tick_crossings(arp)
assert arp_zc[1] > arp_zc[0], arp_zc
assert arp_zc[2] > arp_zc[1], arp_zc
assert abs(arp_zc[3] - arp_zc[0]) <= 1, arp_zc
assert arp_zc[4] > arp_zc[3] and arp_zc[5] > arp_zc[4], arp_zc

slide_zc = tick_crossings(slide)
assert slide_zc[-1] >= slide_zc[0] + 1, slide_zc
assert slide != control

# Step 4 begins at frame 22050 at 120 BPM. PORTAMENTO preserves the previous C4
# center on the E4 NOTE row, then advances by one semitone at each nonzero tick.
row4 = 22050
row_tick = [row4 + x for x in tick_starts]
assert porta[:row4] == control[:row4]
fd_porta = first_diff(control, porta)
assert row4 <= fd_porta < row4 + 256, fd_porta
porta_zc = []
for a, b in zip(row_tick[:-1], row_tick[1:]):
    porta_zc.append(crossings(porta[a + 100:b - 100]))
assert porta_zc[3] > porta_zc[0], porta_zc
assert porta_zc[4] >= porta_zc[3], porta_zc

# Full-app vibrato proof: exact phase/depth math is unit-tested; here require
# that the modulation reaches audio only after tick 1 and produces substantial
# deterministic sample divergence without changing gate/MIDI behavior.
vib_diff = sum(abs(int(a) - int(b)) for a, b in zip(vibrato[tick_starts[1]:tick_starts[6]],
                                                     control[tick_starts[1]:tick_starts[6]]))
assert vib_diff > 100000, vib_diff

for name in NAMES:
    path, n, _ = loaded[name]
    print(f'WP3_6_{name.upper()}_WAV_SHA256={sha256(path.read_bytes()).hexdigest()}')
print(f'WP3_6_EFFECT_AUDIO_FRAMES={frame_count}')
print('WP3_6_ARP_ZC=' + ','.join(map(str, arp_zc)))
print('WP3_6_SLIDE_ZC=' + ','.join(map(str, slide_zc)))
print('WP3_6_PORTA_ZC=' + ','.join(map(str, porta_zc)))
print(f'WP3_6_VIBRATO_DIFF_ABS={vib_diff}')
print('WP3_6_EFFECT_AUDIO_BEHAVIOR_OK=1 arp_ticks=1 slide_up=1 porta_glide=1 vibrato=1 midi_events=0')
PY
