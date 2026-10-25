#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_7_integration
BASE=f9122f6b1ea7e842e12bf80345cadc749d0b21e0
EXPECTED_WAV=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
mkdir -p "$OUT" build/host

actual="$(git diff --name-only "$BASE" -- firmware/src firmware/gen tools | sort)"
expected="$(printf '%s\n' \
  firmware/src/dinghy.c \
  firmware/src/fmt_fm_algorithm.c \
  firmware/src/fmt_fm_algorithm.h \
  firmware/src/fmt_fm_engine.c \
  firmware/src/fmt_fm_engine.h | sort)"
test "$actual" = "$expected"

git diff --exit-code "$BASE" -- \
  firmware/src/fmt_voice_allocator.c \
  firmware/src/fmt_voice_allocator.h \
  firmware/src/fmt_fm_operator.c \
  firmware/src/fmt_fm_operator.h \
  firmware/src/fmt_fm_ratio.c \
  firmware/src/fmt_fm_ratio.h \
  firmware/src/fmt_fm_controls.c \
  firmware/src/fmt_fm_controls.h \
  firmware/src/fmt_fm_feedback.c \
  firmware/src/fmt_fm_feedback.h \
  firmware/src/fmt_fm_envelope.c \
  firmware/src/fmt_fm_envelope.h \
  firmware/gen/fmt_fm_sine_q15.inc \
  firmware/gen/fmt_fm_envelope_step_q15_16.inc
printf '%s\n' 'WP5_7_FINAL_SOURCE_BOUNDARY_OK=1 production_files=5 d043=unchanged d049=unchanged d051=unchanged d052=unchanged d053=unchanged d054=unchanged generated=unchanged' | tee "$OUT/source_boundary.txt"

# Focused engine and D050 compatibility gate, without the historical pre-dinghy
# source-boundary assertion in run_wp5_7_engine_gate.sh.
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/src \
  tests/wp5_7_engine_host.c \
  firmware/src/fmt_fm_operator.c \
  firmware/src/fmt_fm_algorithm.c \
  firmware/src/fmt_fm_ratio.c \
  firmware/src/fmt_fm_controls.c \
  firmware/src/fmt_fm_feedback.c \
  firmware/src/fmt_fm_envelope.c \
  firmware/src/fmt_fm_engine.c \
  firmware/src/fmt_engine_pitch.c \
  firmware/src/fmt_voice_allocator.c \
  -o "$OUT/wp5_7_engine_host"
"$OUT/wp5_7_engine_host" | tee "$OUT/engine.log"
grep -F 'WP5_7_D050_SEAM_EQUIVALENT_OK=1 algorithms=4 frames_each=4096 op4_advance=once' "$OUT/engine.log"
grep -F 'WP5_7_FM_ENGINE_OWNERSHIP_OK=1 slots=8 active_cross_track_steals=0 release_tail=reused malformed=safe_silence' "$OUT/engine.log"
grep -F 'WP5_7_FM_ENGINE_EVENT_OK=1 note=hard_retrigger retrig=hard_reset off_cut=detached_release' "$OUT/engine.log"
grep -F 'WP5_7_FM_ENGINE_HOST_OK=1 d050=compatible engine=788 voice=92 slots=8' "$OUT/engine.log"

# Default production startup remains all-8BIT and must retain the exact WP5.0
# full-app stream even though both family renderers are now linked.
sh tests/run_host.sh | tee "$OUT/default_host.log"
DEFAULT_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$DEFAULT_SHA" = "$EXPECTED_WAV"
DEFAULT_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav','rb') as f:
    print(f.getnframes())
PY
)"
test "$DEFAULT_FRAMES" = 286464
printf 'WP5_7_ALL8BIT_REGRESSION_OK=1 sha256=%s frames=%s\n' "$DEFAULT_SHA" "$DEFAULT_FRAMES" | tee "$OUT/all8bit.txt"

# Build an actual-dinghy mixed-family fixture in a dedicated source directory.
# Only startup fixture data changes: instrument 0 becomes FM4, then track 0
# switches to 8-BIT instrument 1 on step 16 and back to FM4 on step 32.
MIXSRC=build/wp5_7_mixed_src
rm -rf "$MIXSRC"
mkdir -p "$MIXSRC"
python3 - <<'PY'
from pathlib import Path
src = Path('firmware/src/dinghy.c').read_text()
anchor = '    fmt_playback_seed_proof_pattern();\n'
assert src.count(anchor) == 1
fixture = '''    fmt_playback_seed_proof_pattern();
    {
        FmtSong *wp5_7_song = fmt_store_song();
        FmtStep *wp5_7_row;
        if (wp5_7_song) {
            wp5_7_song->instruments[0].mode = FMT_MODE_16BIT;
            wp5_7_song->instruments[0].engine = FMT_FM_ENGINE_FM4;
            wp5_7_row = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 16u);
            if (wp5_7_row)
                wp5_7_row->instrument = 1u;
            wp5_7_row = fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 32u);
            if (wp5_7_row)
                wp5_7_row->instrument = 0u;
        }
    }
'''
Path('build/wp5_7_mixed_src/dinghy.c').write_text(src.replace(anchor, fixture, 1))
PY
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
  -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp5.7-mixed"' \
  -Ifirmware/src -Ifirmware/gen \
  -o "$OUT/fm1tracker_mixed" "$MIXSRC/dinghy.c"
"$OUT/fm1tracker_mixed" "$OUT/mixed_a" > "$OUT/mixed_a.log"
"$OUT/fm1tracker_mixed" "$OUT/mixed_b" > "$OUT/mixed_b.log"
test -s "$OUT/mixed_a.wav"
test -s "$OUT/mixed_b.wav"
cmp -s "$OUT/mixed_a.wav" "$OUT/mixed_b.wav"
MIXED_SHA="$(sha256sum "$OUT/mixed_a.wav" | awk '{print $1}')"
test "$MIXED_SHA" != "$EXPECTED_WAV"
python3 - <<'PY' | tee "$OUT/mixed_analysis.txt"
import wave
from pathlib import Path
p = Path('build/wp5_7_integration/mixed_a.wav')
with wave.open(str(p), 'rb') as f:
    assert f.getnchannels() == 2
    assert f.getframerate() == 44100
    frames = f.getnframes()
    width = f.getsampwidth()
    data = f.readframes(frames)
assert frames == 286464
assert width in (2, 4)
stride = width * 2
def left_energy(lo, hi):
    total = 0
    count = 0
    for frame in range(lo, min(hi, frames)):
        off = frame * stride
        v = int.from_bytes(data[off:off+width], 'little', signed=True)
        total += abs(v)
        count += v != 0
    return total, count
# At 120 BPM, step 16 is near frame 88,200 and step 32 near frame 176,400.
# Stay away from exact boundaries and require audible activity in each family era.
fm_a = left_energy(8000, 80000)
bit = left_energy(96000, 168000)
fm_b = left_energy(184000, 260000)
assert fm_a[0] > 0 and fm_a[1] > 0, fm_a
assert bit[0] > 0 and bit[1] > 0, bit
assert fm_b[0] > 0 and fm_b[1] > 0, fm_b
print(f'WP5_7_MIXED_AUDIO_WINDOWS_OK=1 frames={frames} fm_a_nonzero={fm_a[1]} bit_nonzero={bit[1]} fm_b_nonzero={fm_b[1]}')
PY
printf 'WP5_7_MIXED_DINGHY_OK=1 runs=2 wav=byte_identical sha256=%s switches=fm_to_8bit_to_fm\n' "$MIXED_SHA" | tee "$OUT/mixed_identity.txt"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
import re
from pathlib import Path
engine = Path('firmware/src/fmt_fm_engine.c').read_text()
header = Path('firmware/src/fmt_fm_engine.h').read_text()
dinghy = Path('firmware/src/dinghy.c').read_text()
algorithm = Path('firmware/src/fmt_fm_algorithm.c').read_text()

def strip(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    text = re.sub(r'//[^\n]*', '', text)
    text = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
    return '\n'.join(line for line in text.splitlines()
                     if not line.lstrip().startswith('#'))
code = strip(engine)
for forbidden in ('uint64_t','int64_t','long long','float','double',
                  'malloc','calloc','realloc','free'):
    assert forbidden not in engine and forbidden not in header, forbidden
assert '/' not in code, 'runtime division in FM engine'
assert '%' not in code, 'runtime modulo in FM engine'
for forbidden_dep in ('fmt_store_song','fmt_store_step','fmt_lock','fmt_editor','fmt_8bit_row_param_cache'):
    assert forbidden_dep not in engine, forbidden_dep
assert 'sizeof(FmtFmVoice) == 92u' in header
assert 'sizeof(FmtFmEngine) == 788u' in header
assert 'FmtFmEngine fmt_fm_engine_runtime;' in dinghy
assert dinghy.count('fmt_fm_engine_render(&fmt_fm_engine_runtime)') == 1
assert 'fmt_fm_engine_note(&fmt_fm_engine_runtime,' in dinghy
assert 'fmt_fm_engine_release(&fmt_fm_engine_runtime,' in dinghy
assert 'fmt_fm_engine_retrigger(&fmt_fm_engine_runtime,' in dinghy
assert 'fmt_fm_engine_update_row(' in dinghy
assert 'fmt_fm_algorithm_render_from_op4' in algorithm
assert algorithm.count('fmt_fm_operator_render(&state->op[3]') == 1
print('WP5_7_REALTIME_STATIC_AUDIT_OK=1 fm_slots=fixed8 family_render_calls=1 heap=0 float=0 int64=0 runtime_divmod=0 store_editor_cache_reads=0 d050_op4_seam=single')
PY

# Re-prove D054; it recursively re-proves D053/D052/D051/D050/D049 and the
# full default WP5.0 WAV on the now-integrated production tree.
sh tests/run_wp5_6_fm_envelope_tests.sh | tee "$OUT/inherited_d054.log"
grep -F 'WP5_6_FM_ENVELOPE_HOST_GATE_OK=1 d054=exact generator=identical table=exact stress=exact state=20 controls=16 static_bounds=pass d053=exact d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d054.log"

printf 'WP5_7_HOST_GATE_OK=1 d055=integrated engine=788 slots=8 d043=exact d048=integrated d054=exact all8bit=wp5_0_identical mixed=deterministic hardware_margin_claim=none\n' | tee "$OUT/summary.txt"
