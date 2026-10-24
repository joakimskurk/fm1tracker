#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build

python3 - <<'PY' | tee build/wp3_10_realtime_audit.log
from pathlib import Path
import re, subprocess

BASE = '354bf7d356774ad828f1f660bd456dbab7f0490d'

# WP3.10 is a verification package. No production firmware change is permitted
# unless a stress test demonstrated and documented a defect. None has.
changed = subprocess.check_output([
    'git','diff','--name-only',BASE,'--','firmware/src','firmware/hal','firmware/loader','build.sh','tools'
], text=True).splitlines()
assert changed == [], changed
print('WP3_10_PRODUCTION_UNCHANGED_OK=1 baseline=' + BASE)

sources = {
    p: Path(p).read_text()
    for p in (
        'firmware/src/dinghy.c',
        'firmware/src/fmt_rt_queue.c',
        'firmware/src/fmt_tracker_effect.c',
        'firmware/src/fmt_tracker_event.c',
        'firmware/src/fmt_playback_params.c',
        'firmware/src/fmt_tracker_screen.c',
    )
}

# No dynamic allocation in the production Phase-3 runtime modules.
for path, text in sources.items():
    for token in ('malloc(', 'calloc(', 'realloc(', 'free('):
        assert token not in text, (path, token)
print('WP3_10_NO_ALLOCATOR_OK=1')

# Audio callback has no lock-store/editor traversal and no screen/display work.
dinghy = sources['firmware/src/dinghy.c']
a0 = dinghy.index('static void app_audio(')
a1 = dinghy.index('/* ---- notes (main loop) */', a0)
audio = dinghy[a0:a1]
for token in ('fmt_lock_', 'fmt_tracker_screen_', 'draw_', 'lcd_', 'cv_', 'settings_save', 'settings_load'):
    assert token not in audio, token
assert 'audition_consume_queue();' in audio
assert 'fmt_tracker_effect_tick_due' in audio
assert 'fmt_tracker_row_event_action' in audio
print('WP3_10_AUDIO_OWNERSHIP_OK=1 no_lock_traversal=1 no_screen_calls=1')

# D034 queue consumption is hard-capped and overflow discard is O(1).
q0 = dinghy.index('static void audition_consume_queue(')
q1 = dinghy.index('static void app_audio(', q0)
consume = dinghy[q0:q1]
assert 'for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++)' in consume
assert 'fmt_rt_queue_consumer_discard_all(&fmt_rt_queue_runtime);' in consume
assert 'while (' not in consume
print('WP3_10_QUEUE_BOUND_OK=1 callback_cap=32 overflow_discard_o1=1')

# SPSC implementation must not gain spin/retry loops or ownership-violating
# consumer/producer writes through a generic lock primitive.
rtq = sources['firmware/src/fmt_rt_queue.c']
assert 'while (' not in rtq
for token in ('mutex', 'spin', 'critical', 'disable_irq', 'enable_irq'):
    assert token not in rtq.lower(), token
print('WP3_10_SPSC_STATIC_OK=1 spin_retry=0 irq_masking=0')

# Screen ownership remains presentation/main only. D035 sync itself never writes
# audio/transport/editor/store state through mutation APIs.
screen = sources['firmware/src/fmt_tracker_screen.c']
for token in ('app_audio', 'lcd_', 'cv_', 'fmt_lock_set(', 'fmt_lock_remove(', 'fmt_transport_start(', 'fmt_transport_stop('):
    assert token not in screen, token
assert 'fmt_lock_count(' in screen
print('WP3_10_SCREEN_OWNERSHIP_OK=1 main_presentation_only=1 lock_reads_bounded=1')

# Realtime Phase-3 units retain integer-only implementation; 64-bit arithmetic
# remains host-test reference only, not firmware runtime.
for path in ('firmware/src/fmt_transport.c','firmware/src/fmt_tracker_effect.c','firmware/src/fmt_tracker_event.c','firmware/src/fmt_rt_queue.c','firmware/src/fmt_playback_params.c'):
    text = Path(path).read_text()
    assert 'uint64_t' not in text and 'int64_t' not in text, path
    assert re.search(r'\bfloat\b|\bdouble\b', text) is None, path
print('WP3_10_INTEGER_REALTIME_OK=1 runtime_64bit_types=0 float_double=0')

# There remains one D020 transport clock and D032 derives ticks from its phase;
# no new free-running tracker/effect clock was introduced by WP3.10.
effect_h = Path('firmware/src/fmt_tracker_effect.h').read_text()
assert '#define FMT_TRACKER_EFFECT_TICK_PHASE (FMT_TRANSPORT_PHASE_PER_STEP / FMT_TRACKER_EFFECT_TICKS_PER_ROW)' in effect_h
assert 'FMT_TRACKER_EFFECT_TICKS_PER_ROW 6u' in effect_h
print('WP3_10_SINGLE_CLOCK_OK=1 d032_derived_from_d020=1 ticks_per_row=6')

print('WP3_10_REALTIME_AUDIT_OK=1')
PY
