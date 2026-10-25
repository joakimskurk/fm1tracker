#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_8_closeout_host
BASE=80587f12786122dcec819260ddad5dd5db4b1005
mkdir -p "$OUT"

# D056 production boundary. Keep this exact rather than merely checking that
# every changed file is allowed: closeout evidence must describe the tree that
# was actually verified.
actual="$(git diff --name-only "$BASE" -- firmware/src firmware/gen | sort)"
expected="$(printf '%s\n' \
  firmware/src/dinghy.c \
  firmware/src/fmt_8bit_linked_params.c \
  firmware/src/fmt_8bit_linked_params.h \
  firmware/src/fmt_fm_linked_params.c \
  firmware/src/fmt_fm_linked_params.h \
  firmware/src/fmt_fm_params.c \
  firmware/src/fmt_fm_params.h \
  firmware/src/fmt_lock_editor.c \
  firmware/src/fmt_param_registry.h | sort)"
test "$actual" = "$expected"

# D049-D055 synthesis/voice/router and generated DSP data remain frozen.
git diff --exit-code "$BASE" -- \
  firmware/src/fmt_fm_operator.c firmware/src/fmt_fm_operator.h \
  firmware/src/fmt_fm_algorithm.c firmware/src/fmt_fm_algorithm.h \
  firmware/src/fmt_fm_ratio.c firmware/src/fmt_fm_ratio.h \
  firmware/src/fmt_fm_controls.c firmware/src/fmt_fm_controls.h \
  firmware/src/fmt_fm_feedback.c firmware/src/fmt_fm_feedback.h \
  firmware/src/fmt_fm_envelope.c firmware/src/fmt_fm_envelope.h \
  firmware/src/fmt_fm_engine.c firmware/src/fmt_fm_engine.h \
  firmware/src/fmt_voice_allocator.c firmware/src/fmt_voice_allocator.h \
  firmware/src/fmt_engine_router.c firmware/src/fmt_engine_router.h \
  firmware/gen/fmt_fm_sine_q15.inc \
  firmware/gen/fmt_fm_envelope_step_q15_16.inc
printf '%s\n' 'WP5_8_CLOSEOUT_SOURCE_OK=1 decision=D056 production_delta=9 d049_d055=frozen generated=frozen' | tee "$OUT/source_boundary.txt"

# Complete D056 stack: base API -> editor -> row snapshot -> store resolver ->
# generalized cache -> parameter model -> focused D055 behavior -> exact 8BIT.
sh tests/run_wp5_8_base_api_tests.sh > "$OUT/d056_stack.log" 2>&1
cat "$OUT/d056_stack.log"
grep -F 'WP5_8_FM_BASE_API_GATE_OK=1 decision=D056 bytes=27 instruments=16 cache_rebuild=none plock_override=wins editor=exact all8bit=wp5_0_identical hardware_margin_claim=none' "$OUT/d056_stack.log"

# Independently re-prove D054. This recursively re-proves D053, D052, D051,
# D050, D049 and the accepted full-app all-8BIT stream.
sh tests/run_wp5_6_fm_envelope_tests.sh > "$OUT/inherited_d054.log" 2>&1
cat "$OUT/inherited_d054.log"
grep -F 'WP5_6_FM_ENVELOPE_HOST_GATE_OK=1 d054=exact generator=identical table=exact stress=exact state=20 controls=16 static_bounds=pass d053=exact d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d054.log"

# Closeout realtime seam audit. D056 composition is bounded to row dispatch and
# no mutable sparse store/editor read may enter the per-sample FM renderer.
python3 - <<'PY' | tee "$OUT/realtime_audit.txt"
import re
from pathlib import Path

dinghy = Path('firmware/src/dinghy.c').read_text()
engine = Path('firmware/src/fmt_fm_engine.c').read_text()
linked = Path('firmware/src/fmt_fm_linked_params.c').read_text()

lo = dinghy.index('static void tracker_apply_effect_pitch')
hi = dinghy.index('static void audition_release_all')
seam = dinghy[lo:hi]
assert 'fmt_fm_linked_params_read(track, row, &overrides)' not in seam
assert 'fmt_lock_' not in seam
assert 'fmt_store_' not in seam
assert 'tracker_fm_lock_page' not in seam
assert seam.count('tracker_fm_snapshot_row(') == 1

for forbidden in ('fmt_lock_', 'fmt_store_', 'fmt_lock_editor',
                  'malloc', 'calloc', 'realloc', 'free',
                  'uint64_t', 'int64_t', 'long long', 'float', 'double'):
    assert forbidden not in engine, forbidden

setter = linked[linked.index('int fmt_fm_linked_params_set_base('):
                linked.index('\nconst FmtFmParamSet *fmt_fm_linked_params_base')]
for forbidden in ('fmt_lock_', 'fmt_store_', 'fmt_engine_row_param_cache',
                  'malloc', 'calloc', 'realloc', 'free'):
    assert forbidden not in setter, forbidden

print('WP5_8_CLOSEOUT_REALTIME_OK=1 row_compose=bounded per_sample_store=0 per_sample_editor=0 page_state_audio=0 heap=0 float=0 int64=0 hardware_margin_claim=none')
PY

printf '%s\n' 'WP5_8_CLOSEOUT_HOST_OK=1 decision=D056 ids=27 pages=7 locks=4 cache=8704 base_bank=432 snapshot=232 editor=page_aware base_api=bounded d055=exact d054_d049=exact all8bit=wp5_0_identical realtime=pass hardware_margin_claim=none' | tee "$OUT/summary.txt"
