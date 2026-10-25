#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_8_fm_editor
EXPECTED_WAV=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
mkdir -p "$OUT" build/host

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_fm_linked_editor.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_lock_editor.c \
    firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_playback_params.c \
    firmware/src/fmt_8bit_params.c \
    firmware/src/fmt_8bit_linked_params.c \
    firmware/src/fmt_fm_params.c \
    firmware/src/fmt_fm_linked_params.c \
    -o "$OUT/test_fmt_fm_linked_editor"
"$OUT/test_fmt_fm_linked_editor" > "$OUT/editor.log" 2>&1
cat "$OUT/editor.log"
grep -F 'WP5_8_FM_EDITOR_OK=1 pages=7 slots=4 base_reference=exact existing_lock=exact limit=4 delete_refresh=one_row footer=semantic other_family=unchanged' "$OUT/editor.log"

# Historical generic editor grammar remains unchanged.
sh tests/run_lock_editor_tests.sh > "$OUT/lock_editor.log" 2>&1
cat "$OUT/lock_editor.log"
grep -F 'FMT_LOCK_EDITOR_TESTS_OK=1' "$OUT/lock_editor.log"

# The accepted row-boundary integration and all lower D056 layers remain green.
sh tests/run_wp5_8_row_snapshot_tests.sh > "$OUT/row_snapshot.log" 2>&1
cat "$OUT/row_snapshot.log"
grep -F 'WP5_8_ROW_SNAPSHOT_GATE_OK=1 decision=D056 snapshot_bytes=232 row_boundary=bounded retrig=current_patch off_cut=row_release store_traversal_audio=0 resolver=exact all8bit=wp5_0_identical hardware_margin_claim=none' "$OUT/row_snapshot.log"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
from pathlib import Path
s = Path('firmware/src/dinghy.c').read_text()
e = Path('firmware/src/fmt_lock_editor.c').read_text()

assert 'static uint8_t tracker_fm_lock_page;' in s
assert 'tracker_fm_lock_page = FMT_FM_PAGE_CORE;' in s
assert 'fmt_fm_linked_editor_is_fm(&fmt_lock_editor_runtime)' in s
assert 'fmt_fm_linked_editor_apply_delta(&fmt_lock_editor_runtime,' in s
assert 'fmt_fm_linked_editor_confirm_delete(&fmt_lock_editor_runtime)' in s
assert 'tracker_fm_lock_page + 1u < FMT_FM_PAGE_COUNT' in s
assert 'fmt_pattern_input_page_prev(&fmt_pattern_input_runtime)' in s
assert 'fmt_pattern_input_page_next(&fmt_pattern_input_runtime)' in s

# Provisional page navigation is main/UI only. No FM page-state reference may
# enter the bounded audio/event seam.
lo = s.index('static void tracker_apply_effect_pitch')
hi = s.index('static void audition_release_all')
assert 'tracker_fm_lock_page' not in s[lo:hi]

for token in ('FMT_PARAM_FM_ALGORITHM', 'FMT_PARAM_FM_INDEX',
              'FMT_PARAM_FM_FEEDBACK', 'FMT_PARAM_FM_OP1_RATIO',
              'FMT_PARAM_FM_OP1_LEVEL', 'FMT_PARAM_FM_OP1_ATTACK',
              'FMT_PARAM_FM_OP1_DECAY', 'FMT_PARAM_FM_OP1_SUSTAIN',
              'FMT_PARAM_FM_OP1_RELEASE'):
    assert token in e, token
print('WP5_8_FM_EDITOR_STATIC_OK=1 page_state=main_only oct_gesture=fm_only family_dispatch=1 footer_semantics=27 audio_page_reads=0')
PY

# Default production startup remains exact all-8BIT after UI/editor integration.
sh tests/run_host.sh > "$OUT/default_host.log" 2>&1
DEFAULT_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$DEFAULT_SHA" = "$EXPECTED_WAV"
printf '%s\n' "WP5_8_FM_EDITOR_ALL8BIT_OK=1 sha256=$DEFAULT_SHA" | tee "$OUT/all8bit.txt"

printf '%s\n' 'WP5_8_FM_EDITOR_GATE_OK=1 decision=D056 pages=7 gesture=oct_under_fm_hold base_reference=exact one_row_refresh=exact generic_editor=unchanged row_snapshot=exact all8bit=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
