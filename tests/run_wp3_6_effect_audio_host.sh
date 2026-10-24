#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_6 firmware/src/dinghy.c

for name in control arp slide slide_down porta vibrato arp_fine invalid; do
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
import math
import wave

FS = 44100
BPM = 120
ROW_PHASE = 661500
TICK_PHASE = 110250
NAMES = ('control', 'arp', 'slide', 'slide_down', 'porta', 'vibrato', 'arp_fine', 'invalid')


def read_left(name):
    path = Path(f'build/host/fm1tracker_wp3_6_{name}.wav')
    with wave.open(str(path), 'rb') as f:
        assert f.getnchannels() == 2
        assert f.getsampwidth() == 2
        assert f.getframerate() == FS
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


def positive_crossings(values, start, end):
    out = []
    start = max(start, 1)
    end = min(end, len(values))
    for i in range(start, end):
        a, b = values[i - 1], values[i]
        if a <= 0 < b and b != a:
            out.append((i - 1) + (-float(a) / float(b - a)))
    return out


def frequency(values, start, end):
    xs = positive_crossings(values, start, end)
    assert len(xs) >= 3, (start, end, xs)
    return FS * (len(xs) - 1) / (xs[-1] - xs[0])


def first_diff(a, b):
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i
    return -1


def ceil_div(a, b):
    return (a + b - 1) // b


def row_tick_starts(row):
    base = row * ROW_PHASE
    return [ceil_div(base + tick * TICK_PHASE, BPM) for tick in range(7)]


def tick_crossings(samples, row=0):
    starts = row_tick_starts(row)
    out = []
    for a, b in zip(starts[:-1], starts[1:]):
        out.append(crossings(samples[a + 100:b - 100]))
    return out


def tick_frequencies(samples, row=0):
    starts = row_tick_starts(row)
    out = []
    for a, b in zip(starts[:-1], starts[1:]):
        out.append(frequency(samples, a + 60, b - 60))
    return out

loaded = {name: read_left(name) for name in NAMES}
frames = {v[1] for v in loaded.values()}
assert len(frames) == 1, frames
frame_count = frames.pop()
assert frame_count > 39000, frame_count
control = loaded['control'][2]
arp = loaded['arp'][2]
slide = loaded['slide'][2]
slide_down = loaded['slide_down'][2]
porta = loaded['porta'][2]
vibrato = loaded['vibrato'][2]
arp_fine = loaded['arp_fine'][2]
invalid = loaded['invalid'][2]

row0_ticks = row_tick_starts(0)

# ARP/slide/vibrato tick 0 is deliberately unmodulated. Their first sample
# difference from control must therefore occur no earlier than internal tick 1.
for name, samples in (('arp', arp), ('slide', slide), ('slide_down', slide_down), ('vibrato', vibrato)):
    assert samples[:row0_ticks[1]] == control[:row0_ticks[1]], name
    fd = first_diff(control, samples)
    assert row0_ticks[1] <= fd < row0_ticks[2], (name, fd)

arp_zc = tick_crossings(arp)
assert arp_zc[1] > arp_zc[0], arp_zc
assert arp_zc[2] > arp_zc[1], arp_zc
assert abs(arp_zc[3] - arp_zc[0]) <= 1, arp_zc
assert arp_zc[4] > arp_zc[3] and arp_zc[5] > arp_zc[4], arp_zc

slide_zc = tick_crossings(slide)
slide_freq = tick_frequencies(slide)
assert slide_zc[-1] >= slide_zc[0] + 1, slide_zc
assert slide_freq[-1] > slide_freq[0] * 1.10, slide_freq

slide_down_zc = tick_crossings(slide_down)
slide_down_freq = tick_frequencies(slide_down)
assert slide_down_freq[-1] < slide_down_freq[0] * 0.90, slide_down_freq
assert slide_down != control

# Step 4 is the next proof NOTE (E4). PORTAMENTO preserves the previous C4
# center on the NOTE row, then advances by one semitone at each nonzero tick.
row4_ticks = row_tick_starts(4)
row4 = row4_ticks[0]
assert porta[:row4] == control[:row4]
fd_porta = first_diff(control, porta)
assert row4 <= fd_porta < row4 + 256, fd_porta
porta_zc = tick_crossings(porta, 4)
porta_freq = tick_frequencies(porta, 4)
assert porta_freq[3] > porta_freq[0] * 1.15, porta_freq
assert porta_freq[4] >= porta_freq[3], porta_freq

# Full-app vibrato proof: exact phase/depth math is unit-tested; here require
# audio divergence only after tick 1 without gate/MIDI changes.
vib_diff = sum(abs(int(a) - int(b)) for a, b in zip(vibrato[row0_ticks[1]:row0_ticks[6]],
                                                     control[row0_ticks[1]:row0_ticks[6]]))
assert vib_diff > 100000, vib_diff

# D031 FINE composition through the real D029 editor. Row 1 gets FINE=255
# (+127 Q7) and ARP +4/+7. Tick 0 must be about one semitone above the control
# C4; later ticks must add the ARP intervals on top of that fine offset.
control_row1_freq = tick_frequencies(control, 1)
arp_fine_freq = tick_frequencies(arp_fine, 1)
ratio0 = arp_fine_freq[0] / control_row1_freq[0]
ratio1 = arp_fine_freq[1] / control_row1_freq[1]
ratio2 = arp_fine_freq[2] / control_row1_freq[2]
assert 1.04 < ratio0 < 1.08, (ratio0, arp_fine_freq, control_row1_freq)
assert ratio1 > 1.30, ratio1
assert ratio2 > 1.54, ratio2
assert ratio2 > ratio1 > ratio0, (ratio0, ratio1, ratio2)

# Unknown command is published as safe NONE and is audio-identical to control.
assert invalid == control
assert loaded['invalid'][0].read_bytes() == loaded['control'][0].read_bytes()

for name in NAMES:
    path, n, _ = loaded[name]
    print(f'WP3_6_{name.upper()}_WAV_SHA256={sha256(path.read_bytes()).hexdigest()}')
print(f'WP3_6_EFFECT_AUDIO_FRAMES={frame_count}')
print('WP3_6_ARP_ZC=' + ','.join(map(str, arp_zc)))
print('WP3_6_SLIDE_UP_ZC=' + ','.join(map(str, slide_zc)))
print('WP3_6_SLIDE_DOWN_ZC=' + ','.join(map(str, slide_down_zc)))
print('WP3_6_PORTA_ZC=' + ','.join(map(str, porta_zc)))
print('WP3_6_SLIDE_UP_FREQ=' + ','.join(f'{x:.3f}' for x in slide_freq))
print('WP3_6_SLIDE_DOWN_FREQ=' + ','.join(f'{x:.3f}' for x in slide_down_freq))
print('WP3_6_PORTA_FREQ=' + ','.join(f'{x:.3f}' for x in porta_freq))
print(f'WP3_6_FINE_ARP_RATIOS={ratio0:.6f},{ratio1:.6f},{ratio2:.6f}')
print(f'WP3_6_VIBRATO_DIFF_ABS={vib_diff}')
print('WP3_6_INVALID_COMMAND_AUDIO_IDENTICAL=1')
print('WP3_6_EFFECT_AUDIO_BEHAVIOR_OK=1 arp_ticks=1 slide_up=1 slide_down=1 porta_glide=1 vibrato=1 fine_composition=1 invalid_none=1 midi_events=0')
PY
