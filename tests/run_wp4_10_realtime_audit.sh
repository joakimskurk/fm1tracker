#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/wp4_10

python3 - <<'PY' > build/wp4_10/realtime_audit.log
from pathlib import Path
import re
import subprocess

PRODUCTION = 'c93c39ad344760b4da0dfbc76829ca5fef886174'

# D046 remains verification-only unless a focused gate exposes a real defect.
changed = subprocess.check_output([
    'git', 'diff', '--name-only', PRODUCTION, '--',
    'firmware/src', 'firmware/hal', 'firmware/loader',
    'firmware/app.ld', 'build.sh', 'tools'
], text=True).splitlines()
assert changed == [], changed
print('WP4_10_PRODUCTION_FREEZE_OK=1 baseline=' + PRODUCTION)

paths = (
    'firmware/src/dinghy.c',
    'firmware/src/fmt_rt_queue.c',
    'firmware/src/fmt_rt_queue.h',
    'firmware/src/fmt_tracker_effect.c',
    'firmware/src/fmt_tracker_event.c',
    'firmware/src/fmt_playback_params.c',
    'firmware/src/fmt_tracker_screen.c',
    'firmware/src/fmt_8bit_engine.c',
    'firmware/src/fmt_8bit_engine.h',
    'firmware/src/fmt_8bit_linked_params.c',
    'firmware/src/fmt_envelope.c',
    'firmware/src/fmt_voice_allocator.c',
)
sources = {p: Path(p).read_text() for p in paths}

for path, text in sources.items():
    for token in ('malloc(', 'calloc(', 'realloc(', 'free('):
        assert token not in text, (path, token)
print('WP4_10_NO_ALLOCATOR_OK=1')

dinghy = sources['firmware/src/dinghy.c']
a0 = dinghy.index('static void app_audio(')
a1 = dinghy.index('/* ---- notes (main loop) */', a0)
audio = dinghy[a0:a1]

# Callback start owns exactly the one D034 consumer lane; tracker rendering is
# performed exactly once per rendered audio frame.
assert audio.count('audition_consume_queue();') == 1
assert audio.count('fmt_8bit_engine_render(&fmt_8bit_engine_runtime') == 1
for token in (
    'fmt_lock_set(', 'fmt_lock_remove(', 'fmt_lock_count(',
    'fmt_tracker_screen_', 'draw_', 'lcd_', 'cv_',
    'settings_save', 'settings_load', 'fmt_store_step('
):
    assert token not in audio, token
print('WP4_10_AUDIO_OWNERSHIP_OK=1 d034_consumers=1 engine_render_calls=1 editor_store_traversal=0 screen_calls=0')

# D045/D046 row ordering: an end-frame row boundary dispatches all eight tracks
# in ascending order. Only the else-if path may perform sub-row tick work, so a
# row boundary always wins and there is no second tracker/effect clock.
needle = '''if (fmt_playback_end_frame_tracks(&fmt_playback_runtime,
                                              &fmt_transport_runtime,
                                              event)) {
                for (track = 0u; track < FMT_TRACKS; track++)
                    tracker_apply_event(track, &event[track]);
            } else if (fmt_transport_runtime.running) {
                for (track = 0u; track < FMT_TRACKS; track++) {'''
assert needle in audio
assert audio.count('fmt_tracker_effect_tick_due(') == 1
assert audio.count('tracker_apply_tick(track);') == 1
print('WP4_10_TRACKER_FRAME_BOUND_OK=1 row_tracks=8 tick_due_checks_max=8 row_boundary_wins=1 second_tracker_clock=0')

# Row/tick helpers are fixed-state per-track operations. They may call bounded
# engine/effect/event/cache helpers but may not traverse queues, editor locks,
# song rows, screens, or contain local variable-length loops.
e0 = dinghy.index('static void tracker_apply_event(')
e1 = dinghy.index('static void tracker_apply_tick(', e0)
apply_event = dinghy[e0:e1]
t0 = e1
t1 = dinghy.index('static void audition_release_all(', t0)
apply_tick = dinghy[t0:t1]
for helper_name, helper in (('event', apply_event), ('tick', apply_tick)):
    assert 'while (' not in helper, helper_name
    assert 'for (' not in helper, helper_name
    for token in (
        'fmt_lock_', 'fmt_store_step(', 'fmt_tracker_screen_',
        'fmt_rt_queue_', 'draw_', 'lcd_', 'cv_'
    ):
        assert token not in helper, (helper_name, token)
assert 'fmt_tracker_row_event_action' in apply_event
assert 'fmt_tracker_row_event_action' in apply_tick
print('WP4_10_ROW_HELPERS_BOUND_OK=1 variable_loops=0 queue_traversal=0 lock_store_traversal=0')

# D034 queue bounds: exact 32-event capacity, callback consumes at most 32,
# overflow recovery is O(1), and the SPSC implementation has no retry/spin or
# IRQ-masking ownership escape hatch.
queue_h = sources['firmware/src/fmt_rt_queue.h']
m = re.search(r'^#define\s+FMT_RT_QUEUE_CAPACITY\s+(\d+)u\s*$', queue_h, re.M)
assert m and int(m.group(1)) == 32
q0 = dinghy.index('static void audition_consume_queue(')
q1 = dinghy.index('static void app_audio(', q0)
consume = dinghy[q0:q1]
assert 'for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++)' in consume
assert 'fmt_rt_queue_consumer_discard_all(&fmt_rt_queue_runtime);' in consume
assert 'while (' not in consume
rtq = sources['firmware/src/fmt_rt_queue.c']
assert 'while (' not in rtq
for token in ('mutex', 'spin', 'critical', 'disable_irq', 'enable_irq'):
    assert token not in rtq.lower(), token
print('WP4_10_QUEUE_BOUND_OK=1 capacity=32 callback_cap=32 overflow_discard_o1=1 spin_retry=0 irq_masking=0')

# The linked renderer scans the exact eight fixed slots and never reaches back
# into mutable song/lock/cache/editor state.
engine_h = sources['firmware/src/fmt_8bit_engine.h']
# The allocator contract owns the slot count; accept either a literal define in
# this header or the included allocator header, but prove the authoritative one.
allocator_h = Path('firmware/src/fmt_voice_allocator.h').read_text()
m = re.search(r'^#define\s+FMT_VOICE_ALLOCATOR_SLOTS\s+(\d+)u\s*$', allocator_h, re.M)
assert m and int(m.group(1)) == 8
engine = sources['firmware/src/fmt_8bit_engine.c']
r0 = engine.index('int16_t fmt_8bit_engine_render(')
r1 = engine.index('uint8_t fmt_8bit_engine_quiet(', r0)
render = engine[r0:r1]
assert render.count('for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)') == 1
assert 'while (' not in render
for token in ('fmt_store_', 'fmt_lock_', 'fmt_8bit_linked_params_', 'fmt_tracker_screen_', 'draw_', 'lcd_'):
    assert token not in render, token
print('WP4_10_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0 lock_list_reads=0')

# Main/editor presentation may inspect bounded lock/cache state; audio may not.
screen = sources['firmware/src/fmt_tracker_screen.c']
for token in (
    'app_audio', 'lcd_', 'cv_', 'fmt_lock_set(', 'fmt_lock_remove(',
    'fmt_transport_start(', 'fmt_transport_stop('
):
    assert token not in screen, token
assert 'fmt_lock_count(' in screen
print('WP4_10_SCREEN_OWNERSHIP_OK=1 main_presentation_only=1 bounded_lock_reads=1')

# Final realtime components stay integer-only and do not gain runtime 64-bit
# arithmetic/division. dinghy.c is included because D046 audits the final linked
# app_audio path itself, not only leaf DSP units.
for path, text in sources.items():
    assert re.search(r'\bfloat\b|\bdouble\b', text) is None, path
    assert 'uint64_t' not in text and 'int64_t' not in text, path
print('WP4_10_INTEGER_REALTIME_OK=1 runtime_64bit_types=0 float_double=0')

# D032 remains tied to the single exact D020 row phase.
transport_h = Path('firmware/src/fmt_transport.h').read_text()
effect_h = Path('firmware/src/fmt_tracker_effect.h').read_text()
def define_u(text, name):
    mm = re.search(r'^#define\s+' + re.escape(name) + r'\s+(\d+)u\s*$', text, re.M)
    assert mm, name
    return int(mm.group(1))
row_phase = define_u(transport_h, 'FMT_TRANSPORT_PHASE_PER_STEP')
tick_phase = define_u(effect_h, 'FMT_TRACKER_EFFECT_TICK_PHASE')
ticks = define_u(effect_h, 'FMT_TRACKER_EFFECT_TICKS_PER_ROW')
assert row_phase == tick_phase * ticks == 661500
assert ticks == 6
assert 'tracker_effect_tick_divides_row_exactly' in effect_h
print(f'WP4_10_SINGLE_CLOCK_OK=1 row_phase={row_phase} tick_phase={tick_phase} ticks_per_row={ticks}')

print('WP4_10_REALTIME_AUDIT_OK=1')
PY

cat build/wp4_10/realtime_audit.log
grep -F 'WP4_10_PRODUCTION_FREEZE_OK=1' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_AUDIO_OWNERSHIP_OK=1 d034_consumers=1 engine_render_calls=1 editor_store_traversal=0 screen_calls=0' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_TRACKER_FRAME_BOUND_OK=1 row_tracks=8 tick_due_checks_max=8 row_boundary_wins=1 second_tracker_clock=0' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_ROW_HELPERS_BOUND_OK=1 variable_loops=0 queue_traversal=0 lock_store_traversal=0' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_QUEUE_BOUND_OK=1 capacity=32 callback_cap=32 overflow_discard_o1=1 spin_retry=0 irq_masking=0' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0 lock_list_reads=0' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6' build/wp4_10/realtime_audit.log
grep -F 'WP4_10_REALTIME_AUDIT_OK=1' build/wp4_10/realtime_audit.log
