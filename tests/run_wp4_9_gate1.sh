#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests build/wp4_9_gate1

BASE=53317d36bd6527f7177d6849db606d23ef49c4f0

# Gate 1 production delta is deliberately limited to eight-track row dispatch
# plus track-aware D031/D032 publication. The linked engine boundary stays shut.
actual="$(git diff --name-only "$BASE" -- firmware/src | sort)"
expected="$(printf '%s\n' \
    firmware/src/fmt_playback.c \
    firmware/src/fmt_playback.h \
    firmware/src/fmt_playback_params.c \
    firmware/src/fmt_playback_params.h \
    firmware/src/fmt_tracker_effect.c \
    firmware/src/fmt_tracker_effect.h | sort)"
test "$actual" = "$expected"
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_param_registry.c firmware/src/fmt_param_registry.h \
    firmware/src/fmt_8bit_params.c firmware/src/fmt_8bit_params.h \
    firmware/src/fmt_voice_allocator.c firmware/src/fmt_voice_allocator.h \
    firmware/src/fmt_envelope.c firmware/src/fmt_envelope.h \
    firmware/src/fmt_osc_pulse.c firmware/src/fmt_osc_pulse.h \
    firmware/src/fmt_osc_noise.c firmware/src/fmt_osc_noise.h \
    firmware/src/fmt_osc_triangle.c firmware/src/fmt_osc_triangle.h \
    firmware/src/fmt_osc_wavetable8.c firmware/src/fmt_osc_wavetable8.h
printf '%s\n' 'WP4_9_GATE1_SOURCE_BOUNDARY_OK=1 production_files=6 dinghy_delta=0 registry_delta=0 engine_delta=0'

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp4_9_gate1.c \
    firmware/src/fmt_playback.c \
    firmware/src/fmt_transport.c \
    firmware/src/fmt_playback_params.c \
    firmware/src/fmt_tracker_effect.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_store.c \
    -o build/tests/test_wp4_9_gate1
./build/tests/test_wp4_9_gate1 | tee build/wp4_9_gate1/focused.log
grep -F 'WP4_9_GATE1_TESTS_OK=1' build/wp4_9_gate1/focused.log
grep -F 'tracks=8 playback_event_bytes=4 track_event_bytes=4 d031_legacy=256 d031_extra=1792 d032_legacy=256 d032_extra=1792 boundary_step1_frame=5513' build/wp4_9_gate1/focused.log
grep -F 'legacy_track0_wrappers=1 track_isolation=1 invalid_address_fallback=1 dinghy_migration=0' build/wp4_9_gate1/focused.log

# Inherited component regressions must remain exact before Gate 2.
./tests/run_playback_tests.sh | tee build/wp4_9_gate1/playback_legacy.log
./tests/run_playback_params_tests.sh | tee build/wp4_9_gate1/d031_legacy.log
./tests/run_tracker_effect_tests.sh | tee build/wp4_9_gate1/d032_legacy.log
grep -F 'FMT_PLAYBACK_TESTS_OK=1' build/wp4_9_gate1/playback_legacy.log
grep -F 'FMT_PLAYBACK_PARAMS_TESTS_OK=1' build/wp4_9_gate1/d031_legacy.log
grep -F 'FMT_TRACKER_EFFECT_TESTS_OK=1' build/wp4_9_gate1/d032_legacy.log
printf '%s\n' 'WP4_9_GATE1_INHERITED_COMPONENTS_OK=1 playback=1 d031=1 d032=1'

# Gate 1 must not change the inherited proof-sine output because dinghy.c is
# intentionally untouched until Gate 4.
./tests/run_host.sh
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
HOST_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
test "$HOST_SHA" = '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440'
test "$HOST_FRAMES" = '286464'
printf '%s\n' "WP4_9_GATE1_PROOF_AUDIO_UNCHANGED_OK=1 sha256=$HOST_SHA frames=$HOST_FRAMES"

printf '%s\n' \
    'WP4_9_GATE1_OK=1 tracks=8 d031_track_aware=1 d032_track_aware=1 legacy_track0=1 proof_audio_identical=1 dinghy_migration=0 registry_activation=0 engine_link=0' \
    | tee build/wp4_9_gate1/summary.txt
