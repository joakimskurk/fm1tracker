#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests build/wp4_9_gate3

BASE=c990305379332e1d12ed3d1abdde0a923fe03bb1
EXPECTED_REPLAY=c2a8d4df

# Gate 3 may activate only the D044 registry/editor/publication surface. The
# linked engine itself and dinghy migration remain frozen until Gate 4.
actual="$(git diff --name-only "$BASE" -- firmware/src | sort)"
expected="$(printf '%s\n' \
    firmware/src/fmt_8bit_linked_params.c \
    firmware/src/fmt_8bit_linked_params.h \
    firmware/src/fmt_8bit_params.h \
    firmware/src/fmt_lock_editor.c \
    firmware/src/fmt_lock_editor.h \
    firmware/src/fmt_model.h \
    firmware/src/fmt_param_registry.h | sort)"
test "$actual" = "$expected"
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_8bit_engine.c firmware/src/fmt_8bit_engine.h \
    firmware/src/fmt_playback.c firmware/src/fmt_playback.h \
    firmware/src/fmt_playback_params.c firmware/src/fmt_playback_params.h \
    firmware/src/fmt_tracker_effect.c firmware/src/fmt_tracker_effect.h \
    firmware/src/fmt_tracker_event.c firmware/src/fmt_tracker_event.h \
    firmware/src/fmt_engine_pitch.c firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_osc_pulse.c firmware/src/fmt_osc_pulse.h \
    firmware/src/fmt_osc_noise.c firmware/src/fmt_osc_noise.h \
    firmware/src/fmt_osc_triangle.c firmware/src/fmt_osc_triangle.h \
    firmware/src/fmt_osc_wavetable8.c firmware/src/fmt_osc_wavetable8.h \
    firmware/src/fmt_envelope.c firmware/src/fmt_envelope.h \
    firmware/src/fmt_voice_allocator.c firmware/src/fmt_voice_allocator.h
printf '%s\n' 'WP4_9_GATE3_SOURCE_BOUNDARY_OK=1 production_files=7 dinghy_delta=0 engine_delta=0 gate1_delta=0 primitives_delta=0'

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp4_9_gate3.c \
    firmware/src/fmt_8bit_linked_params.c \
    firmware/src/fmt_8bit_params.c \
    firmware/src/fmt_lock_editor.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_playback_params.c \
    firmware/src/fmt_store.c \
    -o build/tests/test_wp4_9_gate3
./build/tests/test_wp4_9_gate3 | tee build/wp4_9_gate3/focused.log
grep -F 'WP4_9_GATE3_TESTS_OK=1' build/wp4_9_gate3/focused.log
grep -F 'registry_ids=10 bit8_ids=6 base_bank_bytes=96 row_cache_bytes=2048 tracks=8 rows=64 page_slots=4 inheritance=note_only editor=page_aware refresh=d031+d044 d032_refresh=0 dinghy_migration=0' build/wp4_9_gate3/focused.log
printf '%s\n' 'WP4_9_GATE3_FOCUSED_OK=1'

# Main/editor publication may bounded-scan fixed rows/locks, but must stay out of
# audio ownership and must never pull D032 effect refresh into p-lock mutation.
files='firmware/src/fmt_8bit_linked_params.h firmware/src/fmt_8bit_linked_params.c firmware/src/fmt_lock_editor.h firmware/src/fmt_lock_editor.c firmware/src/fmt_param_registry.h'
! grep -nE '\b(float|double|malloc|calloc|realloc|free)\b' $files
! grep -nE '\b(uint64_t|int64_t|long[[:space:]]+long)\b' $files
! grep -nE '\b(while|goto|qsort|bsearch)\b' firmware/src/fmt_8bit_linked_params.c
grep -F 'row < FMT_STEPS_PER_PATTERN' firmware/src/fmt_8bit_linked_params.c >/dev/null
grep -F 'ordinal < count' firmware/src/fmt_8bit_linked_params.c >/dev/null
! grep -n 'fmt_tracker_effect' firmware/src/fmt_8bit_linked_params.c
grep -F 'fmt_playback_params_refresh_track_step' firmware/src/fmt_8bit_linked_params.c >/dev/null
grep -F 'fmt_8bit_linked_params_refresh_track_row' firmware/src/fmt_8bit_linked_params.c >/dev/null
printf '%s\n' 'WP4_9_GATE3_STATIC_AUDIT_OK=1 alloc=0 float=0 int64=0 unbounded=0 d032_refresh=0 main_only_publication=1'

# D027-D030 remain inherited. D044's resolver/history stays byte-for-byte under
# test, but its one WP4.8-only assertion that 0x20..0x25 were unassigned is
# intentionally superseded by D045 Gate 3. Adapt only that historical assertion
# in a build-directory copy; never rewrite the archived WP4.8 test source.
sh tests/run_param_registry_tests.sh >/dev/null
sh tests/run_lock_store_tests.sh >/dev/null
sh tests/run_lock_editor_tests.sh >/dev/null
python3 - <<'PY'
from pathlib import Path
src = Path('tests/test_wp4_8_8bit_params.c').read_text()
old = '''        /* D044 freezes the IDs but WP4.8 deliberately does not activate the\n         * already-linked D027 registry. That migration belongs to WP4.9. */\n        assert(fmt_param_lookup(ids[i]) == NULL);\n        assert(fmt_param_is_assigned(ids[i]) == 0);\n        assert(fmt_param_is_lockable(ids[i]) == 0);\n'''
new = '''        /* D045 Gate 3 deliberately activates the six frozen D044 IDs. */\n        assert(fmt_param_lookup(ids[i]) != NULL);\n        assert(fmt_param_is_assigned(ids[i]) == 1);\n        assert(fmt_param_is_lockable(ids[i]) == 1);\n'''
assert old in src
Path('build/wp4_9_gate3/test_wp4_8_gate3_compat.c').write_text(src.replace(old, new, 1))
PY
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    build/wp4_9_gate3/test_wp4_8_gate3_compat.c \
    firmware/src/fmt_8bit_params.c \
    firmware/src/fmt_param_registry.c \
    -o build/tests/test_wp4_8_under_gate3
./build/tests/test_wp4_8_under_gate3 > build/wp4_9_gate3/d044_compat.log
grep -F 'WP4_8_D044_TESTS_OK=1' build/wp4_9_gate3/d044_compat.log
grep -F 'WP4_8_REPLAY_FNV1A32=635311c5' build/wp4_9_gate3/d044_compat.log
printf '%s\n' 'WP4_9_GATE3_INHERITED_EDITOR_REGRESSIONS_OK=1 d027=1 d028=1 d029_d030=1 d044_resolver=1 d044_registry_isolation_superseded=1'

# Re-run the pure Gate 2 engine test directly, intentionally bypassing its old
# source-boundary assertion now that Gate 3 is allowed to change registry/editor.
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
    -o build/tests/test_wp4_9_gate2_under_gate3
./build/tests/test_wp4_9_gate2_under_gate3 > build/wp4_9_gate3/gate2_focused.log
grep -F 'WP4_9_GATE2_TESTS_OK=1' build/wp4_9_gate3/gate2_focused.log
grep -F "WP4_9_GATE2_REPLAY_FNV1A32=$EXPECTED_REPLAY" build/wp4_9_gate3/gate2_focused.log
printf '%s\n' 'WP4_9_GATE3_GATE2_REGRESSION_OK=1 replay_fnv1a32=c2a8d4df'

# Gate 4 has not migrated dinghy.c, so tracker proof audio remains exact.
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
printf '%s\n' "WP4_9_GATE3_PROOF_AUDIO_UNCHANGED_OK=1 sha256=$HOST_SHA frames=$HOST_FRAMES"

printf '%s\n' 'WP4_9_GATE3_OK=1 registry_ids=10 base_bank_bytes=96 row_cache_bytes=2048 page_aware_editor=1 pi32v2=pending dinghy_migration=0' \
    | tee build/wp4_9_gate3/summary.txt
