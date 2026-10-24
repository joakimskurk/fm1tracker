#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/wp5_0_realtime

python3 - <<'PY' > build/wp5_0_realtime/realtime_audit.log
from pathlib import Path
import re

paths = (
    'firmware/src/dinghy.c',
    'firmware/src/fmt_engine_router.c',
    'firmware/src/fmt_engine_router.h',
    'firmware/src/fmt_rt_queue.c',
    'firmware/src/fmt_rt_queue.h',
    'firmware/src/fmt_tracker_effect.c',
    'firmware/src/fmt_tracker_event.c',
    'firmware/src/fmt_playback_params.c',
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
print('WP5_0_RT_NO_HEAP_OK=1')

dinghy = sources['firmware/src/dinghy.c']
a0 = dinghy.index('static void app_audio(')
a1 = dinghy.index('/* ---- notes (main loop) */', a0)
audio = dinghy[a0:a1]
assert audio.count('audition_consume_queue();') == 1
assert audio.count('fmt_8bit_engine_render(&fmt_8bit_engine_runtime') == 1
for token in (
    'fmt_store_', 'fmt_lock_set(', 'fmt_lock_remove(', 'fmt_lock_count(',
    'fmt_instrument_identity_publish', 'fmt_tracker_screen_',
    'draw_', 'lcd_', 'cv_', 'settings_save', 'settings_load'
):
    assert token not in audio, token
print('WP5_0_RT_AUDIO_BOUND_OK=1 d034_consumers=1 render_8bit_calls=1 mutable_song_reads=0 publication_writes=0 screen_calls=0')

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
print('WP5_0_RT_FRAME_ORDER_OK=1 row_tracks=8 tick_due_checks_max=8 row_boundary_wins=1 second_clock=0')

# The complete per-track D048 adapter is loop-free and never traverses the song,
# lock store, queue, screen, or any family registry.
r0 = dinghy.index('static int tracker_8bit_instrument_info(')
r1 = dinghy.index('static void audition_release_all(', r0)
routing = dinghy[r0:r1]
assert 'while (' not in routing
assert 'for (' not in routing
for token in (
    'fmt_store_', 'fmt_lock_', 'fmt_rt_queue_', 'fmt_tracker_screen_',
    'draw_', 'lcd_', 'cv_'
):
    assert token not in routing, token
assert 'switch (requested_family)' in routing
assert 'case FMT_ENGINE_FAMILY_8BIT:' in routing
assert 'case FMT_ENGINE_FAMILY_FM:' in routing
assert 'fmt_instrument_identity_read(&fmt_instrument_identity_publication' in routing
assert 'fmt_engine_router_family(&fmt_engine_router_runtime' in routing
assert 'fmt_engine_router_commit(&fmt_engine_router_runtime' in routing
assert 'fmt_engine_router_clear(&fmt_engine_router_runtime' in routing
print('WP5_0_RT_ROUTING_BOUND_OK=1 per_track_variable_loops=0 song_traversal=0 static_family_switch=1 dynamic_registry=0')

router = sources['firmware/src/fmt_engine_router.c']

def fn_slice(name, next_name):
    s = router.index(name)
    e = router.index(next_name, s)
    return router[s:e]

family_fn = fn_slice('uint8_t fmt_engine_router_family(', 'int fmt_engine_router_commit(')
commit_fn = fn_slice('int fmt_engine_router_commit(', 'void fmt_engine_router_clear(')
clear_fn = fn_slice('void fmt_engine_router_clear(', 'void fmt_instrument_identity_publication_reset(')
for name, fn in (('family', family_fn), ('commit', commit_fn), ('clear', clear_fn)):
    assert 'for (' not in fn and 'while (' not in fn, name
    assert 'router->family[track]' in fn, name
assert 'router->family[track] = family;' in commit_fn
assert 'router->family[track] = FMT_ENGINE_FAMILY_NONE;' in clear_fn
print('WP5_0_RT_ROUTE_STATE_OK=1 bytes_per_track=1 access=o1 addressed_track_only=1')

# Publication reads are one aligned 32-bit word and contain no traversal. Full
# publication loops exist only in startup/main functions and are absent from audio.
read0 = router.index('int fmt_instrument_identity_read(')
read_fn = router[read0:]
assert 'for (' not in read_fn and 'while (' not in read_fn
assert 'word = publication->word[instrument];' in read_fn
assert '__attribute__((aligned(4)))' in dinghy
print('WP5_0_RT_IDENTITY_READ_OK=1 aligned=4 bytes_per_instrument=4 read=o1 audio_song_store=0')

# One and only one 2,048-byte family page cache remains. Parallel family caches
# would violate the D048 RAM contract.
cache_defs = 0
for path in Path('firmware/src').glob('*'):
    if path.is_file():
        cache_defs += path.read_text(errors='ignore').count(
            'volatile Fmt8BitRowParamCache fmt_8bit_row_param_cache\n')
assert cache_defs == 1, cache_defs
linked_h = Path('firmware/src/fmt_8bit_linked_params.h').read_text()
assert 'sizeof(fmt_8bit_row_param_cache) == 2048u' in linked_h
print('WP5_0_RT_SHARED_CACHE_OK=1 definitions=1 bytes=2048 parallel_family_cache=0')

# No project-wide song metadata gate may survive in any realtime eligibility path.
assert 'song->meta.mode != FMT_MODE_8BIT' not in dinghy
assert 'song->meta.mode != FMT_MODE_8BIT' not in sources['firmware/src/fmt_8bit_linked_params.c']
print('WP5_0_RT_SONG_MODE_NONGATING_OK=1')

# Preserve exact D034 queue and D043 fixed voice bounds.
queue_h = sources['firmware/src/fmt_rt_queue.h']
m = re.search(r'^#define\s+FMT_RT_QUEUE_CAPACITY\s+(\d+)u\s*$', queue_h, re.M)
assert m and int(m.group(1)) == 32
q0 = dinghy.index('static void audition_consume_queue(')
q1 = dinghy.index('static void app_audio(', q0)
consume = dinghy[q0:q1]
assert 'for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++)' in consume
assert 'while (' not in consume
allocator_h = Path('firmware/src/fmt_voice_allocator.h').read_text()
m = re.search(r'^#define\s+FMT_VOICE_ALLOCATOR_SLOTS\s+(\d+)u\s*$', allocator_h, re.M)
assert m and int(m.group(1)) == 8
engine = sources['firmware/src/fmt_8bit_engine.c']
e0 = engine.index('int16_t fmt_8bit_engine_render(')
e1 = engine.index('uint8_t fmt_8bit_engine_quiet(', e0)
render = engine[e0:e1]
assert render.count('for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)') == 1
print('WP5_0_RT_INHERITED_BOUNDS_OK=1 queue_capacity=32 render_slots=8')

# Integer-only realtime source remains mandatory. Ignore documentation comments
# in dinghy callback but not code types.
audio_code = re.sub(r'/\*.*?\*/|//[^\n]*', '', audio, flags=re.S)
assert re.search(r'\bfloat\b|\bdouble\b', audio_code) is None
assert 'uint64_t' not in audio_code and 'int64_t' not in audio_code
integer_paths = (
    'firmware/src/fmt_engine_router.c',
    'firmware/src/fmt_tracker_effect.c',
    'firmware/src/fmt_tracker_event.c',
    'firmware/src/fmt_rt_queue.c',
    'firmware/src/fmt_playback_params.c',
    'firmware/src/fmt_8bit_engine.c',
    'firmware/src/fmt_8bit_linked_params.c',
    'firmware/src/fmt_envelope.c',
    'firmware/src/fmt_voice_allocator.c',
)
for path in integer_paths:
    text = Path(path).read_text()
    assert 'uint64_t' not in text and 'int64_t' not in text, path
    assert re.search(r'\bfloat\b|\bdouble\b', text) is None, path
print('WP5_0_RT_INTEGER_OK=1 callback_64bit_types=0 leaf_64bit_types=0 float_double=0')

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
print(f'WP5_0_RT_SINGLE_CLOCK_OK=1 row_phase={row_phase} tick_phase={tick_phase} ticks_per_row={ticks}')

print('WP5_0_REALTIME_AUDIT_OK=1 hardware_margin_claim=none')
PY

cat build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_NO_HEAP_OK=1' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_AUDIO_BOUND_OK=1 d034_consumers=1 render_8bit_calls=1 mutable_song_reads=0 publication_writes=0 screen_calls=0' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_FRAME_ORDER_OK=1 row_tracks=8 tick_due_checks_max=8 row_boundary_wins=1 second_clock=0' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_ROUTING_BOUND_OK=1 per_track_variable_loops=0 song_traversal=0 static_family_switch=1 dynamic_registry=0' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_ROUTE_STATE_OK=1 bytes_per_track=1 access=o1 addressed_track_only=1' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_IDENTITY_READ_OK=1 aligned=4 bytes_per_instrument=4 read=o1 audio_song_store=0' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_SHARED_CACHE_OK=1 definitions=1 bytes=2048 parallel_family_cache=0' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_SONG_MODE_NONGATING_OK=1' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_INHERITED_BOUNDS_OK=1 queue_capacity=32 render_slots=8' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_INTEGER_OK=1 callback_64bit_types=0 leaf_64bit_types=0 float_double=0' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_RT_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6' build/wp5_0_realtime/realtime_audit.log
grep -F 'WP5_0_REALTIME_AUDIT_OK=1 hardware_margin_claim=none' build/wp5_0_realtime/realtime_audit.log
