#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/tests build/wp4_9_fixtures

BIN=build/tests/wp4_9_linked_host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
  -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp4.9-gate5"' \
  -Ifirmware/src -Ifirmware/gen -o "$BIN" firmware/src/dinghy.c

write_fixture() {
  name="$1"
  cat > "build/wp4_9_fixtures/$name.txt"
}
run_fixture() {
  name="$1"
  "$BIN" "build/host/wp4_9_$name" "build/wp4_9_fixtures/$name.txt" \
    > "build/wp4_9_${name}.log"
  test -s "build/host/wp4_9_$name.wav"
  hash="$(sha256sum "build/host/wp4_9_$name.wav" | awk '{print $1}')"
  upper="$(printf '%s' "$name" | tr '[:lower:]' '[:upper:]')"
  printf 'WP4_9_%s_WAV_SHA256=%s\n' "$upper" "$hash"
}

# A. One deterministic fixture per linked engine. Attack/decay are immediate,
# sustain is full, and OFF is immediate so the waveform identity is isolated.
write_fixture pulse <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 step 0 0 60 0 0 0
1 step 0 1 254 255 0 0
10 probe
130 probe
300 end
EOF
write_fixture noise <<'EOF'
1 clear
1 base 1 0 0 255 0 96 0
1 step 0 0 60 1 0 0
1 step 0 1 254 255 0 0
10 probe
130 probe
300 end
EOF
write_fixture triangle <<'EOF'
1 clear
1 base 2 0 0 255 0 96 0
1 step 0 0 60 2 0 0
1 step 0 1 254 255 0 0
10 probe
130 probe
300 end
EOF
write_fixture wavetable <<'EOF'
1 clear
1 base 3 0 0 255 0 96 192
1 step 0 0 60 3 0 0
1 step 0 1 254 255 0 0
10 probe
130 probe
300 end
EOF

# B. Four engines together, then TIE/EMPTY and staggered OFF rows.
write_fixture mixed <<'EOF'
1 clear
1 base 0 0 0 255 96 80 0
1 base 1 0 0 255 96 96 0
1 base 2 0 0 255 96 96 0
1 base 3 0 0 255 96 96 160
1 step 0 0 60 0 0 0
1 step 1 0 64 1 0 0
1 step 2 0 67 2 0 0
1 step 3 0 72 3 0 0
1 step 0 1 253 255 0 0
1 step 1 1 255 255 0 0
1 step 2 1 253 255 0 0
1 step 3 1 255 255 0 0
1 step 0 2 254 255 0 0
1 step 1 3 254 255 0 0
1 step 2 4 254 255 0 0
1 step 3 5 254 255 0 0
10 probe
140 probe
270 probe
650 probe
850 end
EOF

# C. Eight logical tracks fill voices 0..7 in dispatch order. Tracks 0..3 then
# detach into long release tails; row 2 re-notes them while tracks 4..7 stay
# active, so only detached tails may be stolen.
write_fixture ownership <<'EOF'
1 clear
1 base 0 0 0 255 220 80 0
1 base 1 0 0 255 220 96 0
1 base 2 0 0 255 220 96 0
1 base 3 0 0 255 220 96 160
1 step 0 0 60 0 0 0
1 step 1 0 61 1 0 0
1 step 2 0 62 2 0 0
1 step 3 0 63 3 0 0
1 step 4 0 64 0 0 0
1 step 5 0 65 1 0 0
1 step 6 0 66 2 0 0
1 step 7 0 67 3 0 0
1 step 0 1 254 255 0 0
1 step 1 1 254 255 0 0
1 step 2 1 254 255 0 0
1 step 3 1 254 255 0 0
1 step 4 1 253 255 0 0
1 step 5 1 253 255 0 0
1 step 6 1 253 255 0 0
1 step 7 1 253 255 0 0
1 step 0 2 72 0 0 0
1 step 1 2 73 1 0 0
1 step 2 2 74 2 0 0
1 step 3 2 75 3 0 0
1 step 4 2 253 255 0 0
1 step 5 2 253 255 0 0
1 step 6 2 253 255 0 0
1 step 7 2 253 255 0 0
10 probe
135 probe
265 probe
400 end
EOF

# D. Exact D033 timing pairs. The canonical gate will freeze the first PCM
# difference/non-zero frame after discovery.
write_fixture delay_control <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 step 0 0 60 0 0 0
100 end
EOF
write_fixture delay3 <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 step 0 0 60 0 7 3
100 end
EOF
write_fixture retrig_control <<'EOF'
1 clear
1 base 2 0 0 255 0 96 0
1 step 0 0 60 2 0 0
100 end
EOF
write_fixture retrig2 <<'EOF'
1 clear
1 base 2 0 0 255 0 96 0
1 step 0 0 60 2 6 2
100 end
EOF
write_fixture cut_control <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 step 0 0 60 0 0 0
100 end
EOF
write_fixture cut3 <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 step 0 0 60 0 8 3
100 end
EOF

# E1. FINE + SLIDE UP + VOICE/TRACK gain + WIDTH lock in one row.
write_fixture precedence_slide <<'EOF'
1 clear
1 base 0 0 0 255 72 96 0
1 step 0 0 60 0 2 8
1 lock 0 0 2 160
1 lock 0 0 1 128
1 lock 0 0 160 64
1 lock 0 0 36 200
10 probe
25 probe
50 probe
100 end
EOF
# E2. Portamento retains the old pitch on the NOTE row, then moves one bounded
# tick at a time; FINE remains added after the effect state.
write_fixture precedence_porta <<'EOF'
1 clear
1 base 0 0 0 255 72 96 0
1 step 0 0 60 0 0 0
1 step 0 1 64 0 4 16
1 lock 0 1 2 160
130 probe
155 probe
180 end
EOF
# E3. VIBRATO tick 1 with speed=4/depth=15 has a deterministic +15 Q7
# transient; FINE=160 contributes +32 independently.
write_fixture precedence_vibrato <<'EOF'
1 clear
1 base 0 0 0 255 72 96 0
1 step 0 0 60 0 5 79
1 lock 0 0 2 160
10 probe
25 probe
60 end
EOF
# E4. DECAY/SUSTAIN are NOTE snapshots. A TIE row publishes a different page
# but must not retime the envelope already in flight.
write_fixture envelope_snapshot <<'EOF'
1 clear
1 base 0 0 16 100 72 96 0
1 step 0 0 60 0 0 0
1 lock 0 0 33 32
1 lock 0 0 34 120
1 step 0 1 253 255 0 0
1 lock 0 1 33 200
1 lock 0 1 34 50
10 probe
130 probe
180 end
EOF
# E5. OFF row RELEASE p-lock is snapped into the detached tail.
write_fixture release_slow <<'EOF'
1 clear
1 base 0 0 0 255 0 96 0
1 step 0 0 60 0 0 0
1 step 0 1 254 255 0 0
1 lock 0 1 35 220
130 probe
180 end
EOF
write_fixture release_fast <<'EOF'
1 clear
1 base 0 0 0 255 220 96 0
1 step 0 0 60 0 0 0
1 step 0 1 254 255 0 0
1 lock 0 1 35 0
130 probe
180 end
EOF
# E6/E7. TABLE affects WAVETABLE slot 0; an unrelated FINE lock does not alter
# the PULSE page bytes.
write_fixture wavetable_lock <<'EOF'
1 clear
1 base 3 0 0 255 72 96 0
1 step 0 0 60 3 0 0
1 lock 0 0 37 224
10 probe
60 end
EOF
write_fixture unrelated_lock <<'EOF'
1 clear
1 base 0 0 0 255 72 96 0
1 step 0 0 60 0 0 0
1 lock 0 0 2 160
10 probe
60 end
EOF

for f in pulse noise triangle wavetable mixed ownership \
         delay_control delay3 retrig_control retrig2 cut_control cut3 \
         precedence_slide precedence_porta precedence_vibrato envelope_snapshot \
         release_slow release_fast wavetable_lock unrelated_lock; do
  run_fixture "$f"
done | tee build/wp4_9_gate5_hashes.txt

python3 - <<'PY' | tee build/wp4_9_gate5_discovery.txt
from pathlib import Path
import hashlib, re, struct, wave

ROOT = Path('build')

def wav_frames(name):
    p = ROOT/'host'/f'wp4_9_{name}.wav'
    with wave.open(str(p), 'rb') as f:
        assert f.getnchannels() == 2 and f.getsampwidth() == 2 and f.getframerate() == 44100
        raw = f.readframes(f.getnframes())
        frames = list(struct.iter_unpack('<hh', raw))
    return frames

def sha(name):
    return hashlib.sha256((ROOT/'host'/f'wp4_9_{name}.wav').read_bytes()).hexdigest()

def first_nonzero(frames):
    for i, (l, r) in enumerate(frames):
        if l or r:
            return i
    return None

def first_diff(a, b):
    assert len(a) == len(b), (len(a), len(b))
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i
    return None

engines = ['pulse','noise','triangle','wavetable']
engine_hash = [sha(x) for x in engines]
assert len(set(engine_hash)) == 4, engine_hash
for name in engines:
    assert first_nonzero(wav_frames(name)) is not None, name

own = (ROOT/'wp4_9_ownership.log').read_text()
probe10 = next(x for x in own.splitlines() if '    10 ms  fixture probe tracks' in x)
for t in range(8):
    expect = f't{t}=v{t}/e{t % 4 + 1}/i{t % 4}/s3'
    assert expect in probe10, (expect, probe10)
probe265 = next(x for x in own.splitlines() if '   265 ms  fixture probe tracks' in x)
for t in range(8):
    expect = f't{t}=v{t}/e{t % 4 + 1}/i{t % 4}/s3'
    assert expect in probe265, (expect, probe265)

# Sample-accurate D033 discovery values. Delay uses onset; retrig/cut use the
# first PCM delta from otherwise-identical controls.
delay_onset = first_nonzero(wav_frames('delay3'))
retrig_diff = first_diff(wav_frames('retrig_control'), wav_frames('retrig2'))
cut_diff = first_diff(wav_frames('cut_control'), wav_frames('cut3'))
assert delay_onset is not None and retrig_diff is not None and cut_diff is not None

# Parse a specific detailed voice line from a probe block.
def voice_at(name, ms, slot=0):
    text = (ROOT/f'wp4_9_{name}.log').read_text().splitlines()
    prefix = f'{ms:6d} ms  fixture voice v{slot} '
    line = next(x for x in text if x.startswith(prefix))
    m = re.search(r'owner(\d+) eng(\d+) inst(\d+) stage(\d+) pitch(\d+) gain(\d+) '
                  r'page([0-9a-fA-F]{8}) level(\d+) rate(\d+) decay(\d+) sustain(\d+) '
                  r'phase([0-9a-fA-F]{8}) lfsr(\d+)', line)
    assert m, line
    vals = list(m.groups())
    return {
        'owner': int(vals[0]), 'engine': int(vals[1]), 'instrument': int(vals[2]),
        'stage': int(vals[3]), 'pitch': int(vals[4]), 'gain': int(vals[5]),
        'page': int(vals[6], 16), 'level': int(vals[7]), 'rate': int(vals[8]),
        'decay': int(vals[9]), 'sustain': int(vals[10]),
        'phase': int(vals[11], 16), 'lfsr': int(vals[12])
    }

slide0 = voice_at('precedence_slide', 10)
slide1 = voice_at('precedence_slide', 25)
assert slide0['pitch'] == 7712, slide0
assert slide1['pitch'] == 7720, slide1
assert slide0['gain'] == 4128, slide0
assert slide0['page'] == 0x48ff00c8, hex(slide0['page'])

porta0 = voice_at('precedence_porta', 130)
porta1 = voice_at('precedence_porta', 155)
assert porta0['pitch'] == 7712, porta0
assert porta1['pitch'] == 7728, porta1

vib0 = voice_at('precedence_vibrato', 10)
vib1 = voice_at('precedence_vibrato', 25)
assert vib0['pitch'] == 7712, vib0
assert vib1['pitch'] == 7727, vib1

env0 = voice_at('envelope_snapshot', 10)
env1 = voice_at('envelope_snapshot', 130)
assert env0['page'] == 0x48782060, hex(env0['page'])
assert env1['page'] == 0x4832c860, hex(env1['page'])
assert env0['decay'] == env1['decay'], (env0, env1)
assert env0['sustain'] == env1['sustain'], (env0, env1)

slow = voice_at('release_slow', 130)
fast = voice_at('release_fast', 130)
assert slow['owner'] == 255 and slow['stage'] == 4, slow
assert slow['page'] == 0xdcff0060, hex(slow['page'])
assert fast['owner'] == 255 and fast['stage'] == 0, fast

wt = voice_at('wavetable_lock', 10)
unrelated = voice_at('unrelated_lock', 10)
assert wt['engine'] == 4 and wt['page'] == 0x48ff00e0, wt
assert unrelated['page'] == 0x48ff0060, unrelated

print('WP4_9_GATE5_DISCOVERY_OK=1')
for name in engines + ['mixed','ownership']:
    print(f'{name}_sha256={sha(name)}')
print(f'delay3_first_nonzero_frame={delay_onset}')
print(f'retrig2_first_diff_frame={retrig_diff}')
print(f'cut3_first_diff_frame={cut_diff}')
print(f'slide_tick0_pitch={slide0["pitch"]} slide_tick1_pitch={slide1["pitch"]} gain={slide0["gain"]} page={slide0["page"]:08x}')
print(f'porta_row_tick0_pitch={porta0["pitch"]} porta_tick1_pitch={porta1["pitch"]}')
print(f'vibrato_tick0_pitch={vib0["pitch"]} vibrato_tick1_pitch={vib1["pitch"]}')
print(f'envelope_note_page={env0["page"]:08x} tie_page={env1["page"]:08x} decay_snapshot={env0["decay"]} sustain_snapshot={env0["sustain"]}')
print(f'release_slow_stage={slow["stage"]} release_fast_stage={fast["stage"]}')
print(f'wavetable_page={wt["page"]:08x} unrelated_page={unrelated["page"]:08x}')
PY

echo 'WP4_9_GATE5_HOST_MATRIX_DISCOVERY_OK=1'
