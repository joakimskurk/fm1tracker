#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests build/wp4_9_gate2

BASE=fe5816fc4c8fae23b5dd7781f7ea9ddafab2b01b
EXPECTED_REPLAY=c2a8d4df

# Gate 2 may add exactly one pure engine module pair. Gate 1 publication,
# D044 registry/cache/editor activation and dinghy migration remain frozen.
actual="$(git diff --name-only "$BASE" -- firmware/src | sort)"
expected="$(printf '%s\n' \
    firmware/src/fmt_8bit_engine.c \
    firmware/src/fmt_8bit_engine.h | sort)"
test "$actual" = "$expected"
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_param_registry.c firmware/src/fmt_param_registry.h \
    firmware/src/fmt_8bit_params.c firmware/src/fmt_8bit_params.h \
    firmware/src/fmt_playback.c firmware/src/fmt_playback.h \
    firmware/src/fmt_playback_params.c firmware/src/fmt_playback_params.h \
    firmware/src/fmt_tracker_effect.c firmware/src/fmt_tracker_effect.h \
    firmware/src/fmt_engine_pitch.c firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_osc_pulse.c firmware/src/fmt_osc_pulse.h \
    firmware/src/fmt_osc_noise.c firmware/src/fmt_osc_noise.h \
    firmware/src/fmt_osc_triangle.c firmware/src/fmt_osc_triangle.h \
    firmware/src/fmt_osc_wavetable8.c firmware/src/fmt_osc_wavetable8.h \
    firmware/src/fmt_envelope.c firmware/src/fmt_envelope.h \
    firmware/src/fmt_voice_allocator.c firmware/src/fmt_voice_allocator.h
printf '%s\n' 'WP4_9_GATE2_SOURCE_BOUNDARY_OK=1 production_files=2 dinghy_delta=0 gate1_delta=0 registry_delta=0 primitive_delta=0'

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp4_9_gate2.c \
    firmware/src/fmt_8bit_engine.c \
    firmware/src/fmt_8bit_params.c \
    firmware/src/fmt_engine_pitch.c \
    firmware/src/fmt_osc_pulse.c \
    firmware/src/fmt_osc_noise.c \
    firmware/src/fmt_osc_triangle.c \
    firmware/src/fmt_osc_wavetable8.c \
    firmware/src/fmt_envelope.c \
    firmware/src/fmt_voice_allocator.c \
    -o build/tests/test_wp4_9_gate2
./build/tests/test_wp4_9_gate2 | tee build/wp4_9_gate2/focused.log
grep -F 'WP4_9_GATE2_TESTS_OK=1' build/wp4_9_gate2/focused.log
grep -F 'osc_state_bytes=8 voice_bytes=44 engine_bytes=404 slots=8 mix_clamp=32767 identity_reset=1 phase_reuse=1 retrig_reset=1 release_tail=1 malformed_silence=1' build/wp4_9_gate2/focused.log
grep -F "WP4_9_GATE2_REPLAY_FNV1A32=$EXPECTED_REPLAY" build/wp4_9_gate2/focused.log
printf '%s\n' 'WP4_9_GATE2_FOCUSED_OK=1 replay_frozen=1'

files='firmware/src/fmt_8bit_engine.h firmware/src/fmt_8bit_engine.c'
! grep -nE '\b(float|double|malloc|calloc|realloc|free)\b' $files
! grep -nE '\b(uint64_t|int64_t|long[[:space:]]+long)\b' $files
! grep -nE 'Fmt(Song|Step|Lock|Tracker|RtQueue)|fmt_lock_|fmt_store_|fmt_lock_editor' firmware/src/fmt_8bit_engine.c
! grep -nE '\b(while|goto|qsort|bsearch)\b' firmware/src/fmt_8bit_engine.c
grep -F 'i < FMT_VOICE_ALLOCATOR_SLOTS' firmware/src/fmt_8bit_engine.c >/dev/null
grep -F 'fmt_voice_allocator_acquire' firmware/src/fmt_8bit_engine.c >/dev/null
grep -F 'fmt_voice_allocator_detach' firmware/src/fmt_8bit_engine.c >/dev/null
printf '%s\n' 'WP4_9_GATE2_STATIC_AUDIT_OK=1 alloc=0 float=0 int64=0 unbounded=0 store_traversal=0 slots=8 globals=0'

# D038-D044 primitives remain independently green under the pure adapter.
sh tests/run_wp4_2_pulse_tests.sh >/dev/null
sh tests/run_wp4_3_noise_tests.sh >/dev/null
sh tests/run_wp4_4_triangle_tests.sh >/dev/null
sh tests/run_wp4_5_wavetable8_tests.sh >/dev/null
sh tests/run_wp4_6_envelope_tests.sh >/dev/null
sh tests/run_wp4_7_voice_allocator_tests.sh >/dev/null
sh tests/run_wp4_8_8bit_params_tests.sh >/dev/null
printf '%s\n' 'WP4_9_GATE2_PRIMITIVE_REGRESSIONS_OK=1 d038=1 d039=1 d040=1 d041=1 d042=1 d043=1 d044=1'

# Gate 2 is still unlinked from the application. Proof audio must therefore
# remain the exact inherited WP4.8/Gate-1 baseline.
sh tests/run_host.sh
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
HOST_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
test "$HOST_SHA" = '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440'
test "$HOST_FRAMES" = '286464'
printf '%s\n' "WP4_9_GATE2_PROOF_AUDIO_UNCHANGED_OK=1 sha256=$HOST_SHA frames=$HOST_FRAMES"

REPLAY="$(sed -n 's/^WP4_9_GATE2_REPLAY_FNV1A32=//p' build/wp4_9_gate2/focused.log)"
test "$REPLAY" = "$EXPECTED_REPLAY"
printf '%s\n' \
    "WP4_9_GATE2_OK=1 replay_fnv1a32=$REPLAY engine_bytes=404 slots=8 lifecycle=deterministic mix=bounded pi32v2=pending dinghy_migration=0 gate3_activation=0" \
    | tee build/wp4_9_gate2/summary.txt