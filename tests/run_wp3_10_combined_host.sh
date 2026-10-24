#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_10 firmware/src/dinghy.c

python3 - <<'PY'
from pathlib import Path

common = [
    '100 btn 11',                    # SEQ
    '120 effect 0 1 55',
    '120 effect 1 6 2',
    '120 effect 2 7 3',
    '120 effect 3 8 0',
    '120 effect 4 5 47',
    '120 effect 5 2 4',
    '120 effect 6 3 165',
    '120 effect 7 4 16',
    '200 keys 1',                    # hold step 1
    '220 enc 2 -10',                 # VOICE
    '230 enc 3 20',                  # FINE
    '240 enc 4 -10',                 # TRACK LEVEL
    '250 enc 5 1',                   # PAN storage-only
    '360 keys 0',
    '420 keys 4',                    # another white step
    '440 enc 2 -5',
    '450 enc 4 -5',
    '460 enc 0 -1',                  # browse/clamp
    '700 btndown 6',                 # EDIT arm
    '720 btn 3',                     # later SEL confirm
    '740 btnup 6',
    '760 keys 0',
    # Dirty all eight visible FX rows in one poll.
    '900 effect 0 1 56',
    '900 effect 1 6 3',
    '900 effect 2 7 4',
    '900 effect 3 8 1',
    '900 effect 4 5 48',
    '900 effect 5 2 5',
    '900 effect 6 3 166',
    '900 effect 7 4 17',
    '1200 btn 1',                    # page next
    '1250 keys 1',
    '1280 enc 2 -20',
    '1290 enc 5 -10',
    '1350 btndown 6',
    '1400 btn 12',                   # PLAY cancels armed delete before toggle
    '1420 btnup 6',
    '1450 keys 0',
    '1600 btn 8',                    # HOME clears tracker/editor presentation
    '1800 btn 11',                   # back SEQ
    '1850 btn 12',                   # resume requested transport
    '1900 btn 0',                    # page previous
    '2000 effect 0 5 63',
    '2000 effect 1 1 18',
    '2000 effect 2 6 1',
    '2000 effect 3 7 5',
    '2000 effect 4 8 5',
    '2000 effect 5 2 255',
    '2000 effect 6 3 255',
    '2000 effect 7 4 1',
    '3200 end',
]

# Stress adds two >32-event bursts plus normal recovery notes. Incoming MIDI is
# intentionally not reflected in the tracker UI; the control keeps all editor,
# effect and transport actions identical.
stress_extra = []
for base_ms, start_note in ((300, 48), (2200, 60)):
    for i in range(40):
        stress_extra.append(f'{base_ms} midi 144 {start_note + (i % 40)} 100')
stress_extra += [
    '520 midi 144 72 100',
    '600 midi 128 72 0',
    '2400 midi 144 76 100',
    '2480 midi 128 76 0',
]

# Preserve timestamp ordering while keeping same-time insertion deterministic.
def write(name, extras):
    indexed = [(int(line.split()[0]), 0, i, line) for i, line in enumerate(common)]
    indexed += [(int(line.split()[0]), 1, i, line) for i, line in enumerate(extras)]
    indexed.sort(key=lambda x: (x[0], x[1], x[2]))
    # end must remain last regardless of sort class.
    lines = [x[3] for x in indexed if ' end' not in x[3]] + ['3200 end']
    Path(name).write_text('\n'.join(lines) + '\n')

write('build/wp3_10_combined_control.script', [])
write('build/wp3_10_combined_stress.script', stress_extra)
PY

for name in control stress_a stress_b; do
    script=build/wp3_10_combined_control.script
    case "$name" in stress_*) script=build/wp3_10_combined_stress.script ;; esac
    build/host/fm1tracker_wp3_10 \
        "build/host/fm1tracker_wp3_10_${name}" \
        "$script" \
        > "build/wp3_10_combined_${name}.log"
    test -s "build/host/fm1tracker_wp3_10_${name}.wav"
    test -s "build/host/fm1tracker_wp3_10_${name}-1000.ppm"
    test -s "build/host/fm1tracker_wp3_10_${name}-3000.ppm"
done

# p-lock editor activity and queue consumption must not emit outbound MIDI.
for name in control stress_a stress_b; do
    test "$(grep -c 'MIDI out' "build/wp3_10_combined_${name}.log" || true)" -eq 0
done

# Same input must be byte-deterministic; queue activity must not affect screen.
cmp build/host/fm1tracker_wp3_10_stress_a.wav build/host/fm1tracker_wp3_10_stress_b.wav
cmp build/host/fm1tracker_wp3_10_stress_a-1000.ppm build/host/fm1tracker_wp3_10_stress_b-1000.ppm
cmp build/host/fm1tracker_wp3_10_stress_a-3000.ppm build/host/fm1tracker_wp3_10_stress_b-3000.ppm
cmp build/host/fm1tracker_wp3_10_stress_a-1000.ppm build/host/fm1tracker_wp3_10_control-1000.ppm
cmp build/host/fm1tracker_wp3_10_stress_a-3000.ppm build/host/fm1tracker_wp3_10_control-3000.ppm

python3 - <<'PY' | tee build/wp3_10_combined_analysis.txt
from hashlib import sha256
from pathlib import Path
import wave

FS = 44100

def load(name):
    p = Path(f'build/host/fm1tracker_wp3_10_{name}.wav')
    with wave.open(str(p), 'rb') as w:
        assert w.getnchannels() == 2 and w.getsampwidth() == 2 and w.getframerate() == FS
        n = w.getnframes()
        raw = w.readframes(n)
    return p, n, raw

def frame(ms):
    return (ms * FS + 999) // 1000

control_p, control_n, control = load('control')
stress_p, stress_n, stress = load('stress_a')
stress2_p, stress2_n, stress2 = load('stress_b')
assert control_n == stress_n == stress2_n
assert stress == stress2

# Before the first queue burst, the combined UI/effect workload is identical.
assert stress[:frame(280)*4] == control[:frame(280)*4]
# Overflow/audition activity must be audible/different, proving the path ran.
assert stress[frame(300)*4:frame(700)*4] != control[frame(300)*4:frame(700)*4]
assert stress[frame(2200)*4:frame(2600)*4] != control[frame(2200)*4:frame(2600)*4]
# Both overflow epochs plus later normal NOTE ON/OFF must recover completely.
assert stress[frame(2750)*4:] == control[frame(2750)*4:]

for name, p in [('control', control_p), ('stress', stress_p)]:
    print(f'WP3_10_COMBINED_{name.upper()}_WAV_SHA256={sha256(p.read_bytes()).hexdigest()}')
for ms in (1000, 3000):
    p = Path(f'build/host/fm1tracker_wp3_10_stress_a-{ms}.ppm')
    print(f'WP3_10_COMBINED_SCREEN_{ms}_SHA256={sha256(p.read_bytes()).hexdigest()}')
print(f'WP3_10_COMBINED_FRAMES={stress_n}')
print('WP3_10_COMBINED_STRESS_OK=1 deterministic=1 two_overflow_epochs=1 recovery=1 midi_neutral=1 screen_queue_neutral=1')
PY
