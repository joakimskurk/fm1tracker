#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=81d3464059270130747e26fcc950b7a2e1bb03e1
OUT=build/wp5_10_realtime
mkdir -p "$OUT"

git diff --exit-code "$BASE" -- \
    firmware/src firmware/gen firmware/hal firmware/loader \
    firmware/app.ld build.sh tools

python3 - <<'PY' > build/wp5_10_realtime/audit.txt
from pathlib import Path
import re

src = Path('firmware/src')
dinghy = (src / 'dinghy.c').read_text()
bit = (src / 'fmt_8bit_engine.c').read_text()
fm = (src / 'fmt_fm_engine.c').read_text()
algo = (src / 'fmt_fm_algorithm.c').read_text()
feedback = (src / 'fmt_fm_feedback.c').read_text()
operator = (src / 'fmt_fm_operator.c').read_text()
envelope = (src / 'fmt_fm_envelope.c').read_text()
queue_h = (src / 'fmt_rt_queue.h').read_text()
alloc_h = (src / 'fmt_voice_allocator.h').read_text()
fm_h = (src / 'fmt_fm_engine.h').read_text()

# No heap in the complete realtime DSP/control source set.
rt = '\n'.join([dinghy, bit, fm, algo, feedback, operator, envelope])
for token in ('malloc(', 'calloc(', 'realloc(', 'free('):
    assert token not in rt, token

# Final linked callback calls both engines exactly once in the sample body and
# retains three fixed 8-track dispatch loops: start, row and tick.
a0 = dinghy.index('static void app_audio(')
a1 = dinghy.index('/* ---- notes (main loop) */', a0)
audio = dinghy[a0:a1]
assert audio.count('fmt_8bit_engine_render(&fmt_8bit_engine_runtime)') == 1
assert audio.count('fmt_fm_engine_render(&fmt_fm_engine_runtime)') == 1
assert audio.count('for (track = 0u; track < FMT_TRACKS; track++)') == 3
assert 'audition_consume_queue();' in audio
assert '#define NV 4u' in dinghy
assert re.search(r'#define\s+FMT_RT_QUEUE_CAPACITY\s+32u', queue_h)
assert re.search(r'#define\s+FMT_VOICE_ALLOCATOR_SLOTS\s+8u', alloc_h)
assert re.search(r'#define\s+FMT_FM_ENGINE_SLOTS\s+8u', fm_h)

# Both engine renderers remain fixed-size slot scans with no while/goto.
b0 = bit.index('int16_t fmt_8bit_engine_render(')
b1 = bit.index('uint8_t fmt_8bit_engine_quiet(', b0)
br = bit[b0:b1]
f0 = fm.index('int16_t fmt_fm_engine_render(')
f1 = fm.index('uint8_t fmt_fm_engine_quiet(', f0)
fr = fm[f0:f1]
assert br.count('for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)') == 1
assert fr.count('for (i = 0u; i < FMT_FM_ENGINE_SLOTS; i++)') == 1
for body in (br, fr):
    assert 'while (' not in body
    assert 'goto ' not in body

# FM render composition is bounded to four operators/topology math and contains
# no store/editor/screen traversal.
assert 'fmt_fm_feedback_render_op4' in fr
assert 'fmt_fm_algorithm_render_from_op4' in fr
assert 'fmt_fm_envelope_advance' in fr
for token in ('fmt_store_', 'fmt_lock_', 'fmt_tracker_screen_', 'fmt_lock_editor_'):
    assert token not in fr

# Realtime code stays integer-only by source contract. This checks the final DSP
# family files for floating literals/types rather than host benchmark helpers.
for name, text in [('bit', bit), ('fm', fm), ('algo', algo), ('feedback', feedback), ('operator', operator), ('envelope', envelope)]:
    assert not re.search(r'\b(float|double)\b', text), name
    assert not re.search(r'\d+\.\d+[fF]?', text), name

print('WP5_10_REALTIME_AUDIT_OK=1 bit_slots=8 fm_slots=8 tracks=8 dispatch_loops=3 queue=32 audition=4 dual_render=1 heap=0 variable_slot_loops=0 float=0 production_delta=0 hardware_margin_claim=none')
PY
cat "$OUT/audit.txt"
grep -F 'WP5_10_REALTIME_AUDIT_OK=1 bit_slots=8 fm_slots=8 tracks=8 dispatch_loops=3 queue=32 audition=4 dual_render=1 heap=0 variable_slot_loops=0 float=0 production_delta=0 hardware_margin_claim=none' "$OUT/audit.txt"
