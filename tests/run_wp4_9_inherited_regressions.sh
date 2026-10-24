#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

run() {
    name="$1"
    shift
    echo "=== $name ==="
    "$@" > "build/wp4_9_gate6_${name}.log"
    cat "build/wp4_9_gate6_${name}.log"
}

# Phase 2/3 component contracts that remain authoritative after D045. These
# contain no proof-sine full-app hash requirement.
run model sh tests/run_model_tests.sh
grep -F 'FMT_MODEL_TESTS_OK=1' build/wp4_9_gate6_model.log
run store sh tests/run_store_tests.sh
grep -F 'FMT_STORE_TESTS_OK=1' build/wp4_9_gate6_store.log
run transport sh tests/run_transport_tests.sh
grep -F 'FMT_TRANSPORT_TESTS_OK=1' build/wp4_9_gate6_transport.log
run playback sh tests/run_playback_tests.sh
grep -F 'FMT_PLAYBACK_TESTS_OK=1' build/wp4_9_gate6_playback.log
run pattern_input sh tests/run_pattern_input_tests.sh
grep -F 'FMT_PATTERN_INPUT_TESTS_OK=1' build/wp4_9_gate6_pattern_input.log
run step_record sh tests/run_step_record_tests.sh
grep -F 'FMT_STEP_RECORD_TESTS_OK=1' build/wp4_9_gate6_step_record.log
run tracker_screen sh tests/run_tracker_screen_tests.sh
grep -F 'FMT_TRACKER_SCREEN_TESTS_OK=1' build/wp4_9_gate6_tracker_screen.log
run param_registry sh tests/run_param_registry_tests.sh
grep -F 'FMT_PARAM_REGISTRY_TESTS_OK=1' build/wp4_9_gate6_param_registry.log
run lock_store sh tests/run_lock_store_tests.sh
grep -F 'FMT_LOCK_STORE_TESTS_OK=1' build/wp4_9_gate6_lock_store.log
run lock_editor sh tests/run_lock_editor_tests.sh
grep -F 'FMT_LOCK_EDITOR_TESTS_OK=1' build/wp4_9_gate6_lock_editor.log
run playback_params sh tests/run_playback_params_tests.sh
grep -F 'FMT_PLAYBACK_PARAMS_TESTS_OK=1' build/wp4_9_gate6_playback_params.log
run tracker_effect sh tests/run_tracker_effect_tests.sh
grep -F 'FMT_TRACKER_EFFECT_TESTS_OK=1' build/wp4_9_gate6_tracker_effect.log
run tracker_event sh tests/run_tracker_event_tests.sh
grep -F 'FMT_TRACKER_EVENT_TESTS_OK=1' build/wp4_9_gate6_tracker_event.log
run rt_queue sh tests/run_rt_queue_tests.sh
grep -F 'FMT_RT_QUEUE_TESTS_OK=1' build/wp4_9_gate6_rt_queue.log

# Context/UI behavior independent of tracker oscillator identity.
run pattern_context sh tests/run_pattern_context_host.sh
grep -F 'WP2_5_PATTERN_CONTEXT_HOST_OK=1' build/wp4_9_gate6_pattern_context.log
run track_context sh tests/run_track_selection_context_host.sh
grep -F 'WP2_6_TRACK_CONTEXT_HOST_OK=1' build/wp4_9_gate6_track_context.log
run step_record_context sh tests/run_step_rec_context_host.sh
grep -F 'WP2_7_STEP_REC_CONTEXT_HOST_OK=1' build/wp4_9_gate6_step_record_context.log
run lock_editor_context sh tests/run_lock_editor_context_host.sh
grep -F 'WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1' build/wp4_9_gate6_lock_editor_context.log
run lock_delete_context sh tests/run_lock_delete_context_host.sh
grep -F 'WP3_4_LOCK_DELETE_CONTEXT_HOST_OK=1' build/wp4_9_gate6_lock_delete_context.log
run visual sh tests/run_wp3_9_visual_host.sh
grep -F 'WP3_9_REVIEWED_SCREEN_HASHES_OK=1' build/wp4_9_gate6_visual.log
grep -F 'WP3_9_HOME_RETURN_PIXEL_IDENTICAL=1' build/wp4_9_gate6_visual.log

# D036's component-only stress/timing proofs remain valid and deliberately do
# not assert the obsolete proof-sine full-app WAV.
run worst_case sh tests/run_wp3_10_worst_case.sh
grep -F 'WP3_10_FOCUSED_WORST_CASE_OK=1' build/wp4_9_gate6_worst_case.log
run timing sh tests/run_wp3_10_timing_matrix.sh
grep -F 'WP3_10_TIMING_EXIT_OK=1' build/wp4_9_gate6_timing.log
grep -F 'tick_frames=919,1838,2757,3675,4594 next_row_frame=5513 row_boundary_wins=1 max_one_tick_per_frame=1' build/wp4_9_gate6_timing.log

# D045 realtime ownership audit. This intentionally supersedes the old
# WP3.10 "production unchanged since WP3.9" assertion while preserving its
# realtime invariants and extending them to the linked 8-BIT engine.
python3 - <<'PY' > build/wp4_9_gate6_realtime_audit.log
from pathlib import Path
import re

paths = [
    'firmware/src/dinghy.c',
    'firmware/src/fmt_rt_queue.c',
    'firmware/src/fmt_tracker_effect.c',
    'firmware/src/fmt_tracker_event.c',
    'firmware/src/fmt_playback_params.c',
    'firmware/src/fmt_tracker_screen.c',
    'firmware/src/fmt_8bit_engine.c',
    'firmware/src/fmt_8bit_linked_params.c',
    'firmware/src/fmt_envelope.c',
]
sources = {p: Path(p).read_text() for p in paths}
for path, text in sources.items():
    for token in ('malloc(', 'calloc(', 'realloc(', 'free('):
        assert token not in text, (path, token)
print('WP4_9_GATE6_NO_ALLOCATOR_OK=1')

dinghy = sources['firmware/src/dinghy.c']
a0 = dinghy.index('static void app_audio(')
a1 = dinghy.index('/* ---- notes (main loop) */', a0)
audio = dinghy[a0:a1]
for token in ('fmt_lock_set(', 'fmt_lock_remove(', 'fmt_lock_count(',
              'fmt_tracker_screen_', 'draw_', 'lcd_', 'cv_',
              'settings_save', 'settings_load', 'fmt_store_step('):
    assert token not in audio, token
for required in ('audition_consume_queue();', 'fmt_8bit_engine_render(&fmt_8bit_engine_runtime)',
                 'fmt_tracker_effect_tick_due', 'fmt_tracker_row_event_action'):
    assert required in audio, required
print('WP4_9_GATE6_AUDIO_OWNERSHIP_OK=1 editor_store_traversal=0 screen_calls=0 linked_engine=1')

q0 = dinghy.index('static void audition_consume_queue(')
q1 = dinghy.index('static void app_audio(', q0)
consume = dinghy[q0:q1]
assert 'for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++)' in consume
assert 'fmt_rt_queue_consumer_discard_all(&fmt_rt_queue_runtime);' in consume
assert 'while (' not in consume
print('WP4_9_GATE6_QUEUE_BOUND_OK=1 callback_cap=32 overflow_discard_o1=1')

engine = sources['firmware/src/fmt_8bit_engine.c']
r0 = engine.index('int16_t fmt_8bit_engine_render(')
r1 = engine.index('uint8_t fmt_8bit_engine_quiet(', r0)
render = engine[r0:r1]
assert 'for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)' in render
assert 'while (' not in render
for token in ('fmt_store_', 'fmt_lock_', 'fmt_8bit_linked_params_'):
    assert token not in render, token
print('WP4_9_GATE6_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0')

rtq = sources['firmware/src/fmt_rt_queue.c']
assert 'while (' not in rtq
for token in ('mutex', 'spin', 'critical', 'disable_irq', 'enable_irq'):
    assert token not in rtq.lower(), token
print('WP4_9_GATE6_SPSC_STATIC_OK=1 spin_retry=0 irq_masking=0')

screen = sources['firmware/src/fmt_tracker_screen.c']
for token in ('app_audio', 'lcd_', 'cv_', 'fmt_lock_set(', 'fmt_lock_remove(',
              'fmt_transport_start(', 'fmt_transport_stop('):
    assert token not in screen, token
assert 'fmt_lock_count(' in screen
print('WP4_9_GATE6_SCREEN_OWNERSHIP_OK=1 main_presentation_only=1 lock_reads_bounded=1')

for path, text in sources.items():
    if path == 'firmware/src/dinghy.c':
        continue
    assert 'uint64_t' not in text and 'int64_t' not in text, path
    assert re.search(r'\bfloat\b|\bdouble\b', text) is None, path
print('WP4_9_GATE6_INTEGER_REALTIME_OK=1 phase4_runtime_64bit_types=0 float_double=0')

print('WP4_9_GATE6_REALTIME_AUDIT_OK=1')
PY
cat build/wp4_9_gate6_realtime_audit.log
grep -F 'WP4_9_GATE6_REALTIME_AUDIT_OK=1' build/wp4_9_gate6_realtime_audit.log

echo 'WP4_9_GATE6_INHERITED_REGRESSIONS_OK=1 obsolete_proof_sine_hashes_excluded=1'
