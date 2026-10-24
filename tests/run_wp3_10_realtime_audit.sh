#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build

# Do not pipe Python directly through tee here: the audit must propagate a
# nonzero Python status rather than accidentally returning tee's success.
python3 - <<'PY' > build/wp3_10_realtime_audit.log
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

for path, text in sources.items():
    for token in ('malloc(', 'calloc(', 'realloc(', 'free('):
        assert token not in text, (path, token)
print('WP3_10_NO_ALLOCATOR_OK=1')

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

q0 = dinghy.index('static void audition_consume_queue(')
q1 = dinghy.index('static void app_audio(', q0)
consume = dinghy[q0:q1]
assert 'for (i = 0u; i < FMT_RT_QUEUE_CAPACITY; i++)' in consume
assert 'fmt_rt_queue_consumer_discard_all(&fmt_rt_queue_runtime);' in consume
assert 'while (' not in consume
print('WP3_10_QUEUE_BOUND_OK=1 callback_cap=32 overflow_discard_o1=1')

rtq = sources['firmware/src/fmt_rt_queue.c']
assert 'while (' not in rtq
for token in ('mutex', 'spin', 'critical', 'disable_irq', 'enable_irq'):
    assert token not in rtq.lower(), token
print('WP3_10_SPSC_STATIC_OK=1 spin_retry=0 irq_masking=0')

screen = sources['firmware/src/fmt_tracker_screen.c']
for token in ('app_audio', 'lcd_', 'cv_', 'fmt_lock_set(', 'fmt_lock_remove(', 'fmt_transport_start(', 'fmt_transport_stop('):
    assert token not in screen, token
assert 'fmt_lock_count(' in screen
print('WP3_10_SCREEN_OWNERSHIP_OK=1 main_presentation_only=1 lock_reads_bounded=1')

for path in ('firmware/src/fmt_transport.c','firmware/src/fmt_tracker_effect.c','firmware/src/fmt_tracker_event.c','firmware/src/fmt_rt_queue.c','firmware/src/fmt_playback_params.c'):
    text = Path(path).read_text()
    assert 'uint64_t' not in text and 'int64_t' not in text, path
    assert re.search(r'\bfloat\b|\bdouble\b', text) is None, path
print('WP3_10_INTEGER_REALTIME_OK=1 runtime_64bit_types=0 float_double=0')

# Parse the frozen integer constants and prove the D032 tick grid exactly
# divides D020's row phase; do not depend on a particular macro spelling.
transport_h = Path('firmware/src/fmt_transport.h').read_text()
effect_h = Path('firmware/src/fmt_tracker_effect.h').read_text()
def define_u(text, name):
    m = re.search(r'^#define\s+' + re.escape(name) + r'\s+(\d+)u\s*$', text, re.M)
    assert m, name
    return int(m.group(1))
row_phase = define_u(transport_h, 'FMT_TRANSPORT_PHASE_PER_STEP')
tick_phase = define_u(effect_h, 'FMT_TRACKER_EFFECT_TICK_PHASE')
ticks = define_u(effect_h, 'FMT_TRACKER_EFFECT_TICKS_PER_ROW')
assert row_phase == tick_phase * ticks == 661500
assert 'tracker_effect_tick_divides_row_exactly' in effect_h
print(f'WP3_10_SINGLE_CLOCK_OK=1 row_phase={row_phase} tick_phase={tick_phase} ticks_per_row={ticks}')

print('WP3_10_REALTIME_AUDIT_OK=1')
PY

cat build/wp3_10_realtime_audit.log
grep -F 'WP3_10_PRODUCTION_UNCHANGED_OK=1' build/wp3_10_realtime_audit.log
grep -F 'WP3_10_AUDIO_OWNERSHIP_OK=1 no_lock_traversal=1 no_screen_calls=1' build/wp3_10_realtime_audit.log
grep -F 'WP3_10_QUEUE_BOUND_OK=1 callback_cap=32 overflow_discard_o1=1' build/wp3_10_realtime_audit.log
grep -F 'WP3_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6' build/wp3_10_realtime_audit.log
grep -F 'WP3_10_REALTIME_AUDIT_OK=1' build/wp3_10_realtime_audit.log
