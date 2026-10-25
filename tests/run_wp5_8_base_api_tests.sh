#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_8_base_api
mkdir -p "$OUT"

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_fm_base_api.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_fm_params.c \
    firmware/src/fmt_fm_linked_params.c \
    -o "$OUT/test_fmt_fm_base_api"
"$OUT/test_fmt_fm_base_api" > "$OUT/base_api.log" 2>&1
cat "$OUT/base_api.log"
grep -F 'WP5_8_FM_BASE_API_OK=1 bytes=27 instruments=16 mutable=bounded cache_rebuild=none instrument_isolation=exact plock_override=wins invalid=no_mutation' "$OUT/base_api.log"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
from pathlib import Path
s = Path('firmware/src/fmt_fm_linked_params.c').read_text()
start = s.index('int fmt_fm_linked_params_set_base(')
end = s.index('\nconst FmtFmParamSet *fmt_fm_linked_params_base', start)
body = s[start:end]
assert 'fmt_fm_instrument_base[instrument] = *base;' in body
for forbidden in ('fmt_store_', 'fmt_lock_', 'fmt_engine_row_param_cache',
                  'malloc', 'calloc', 'realloc', 'free'):
    assert forbidden not in body, forbidden
print('WP5_8_FM_BASE_STATIC_OK=1 setter=main_bounded store_reads=0 lock_reads=0 cache_writes=0 heap=0')
PY

# Re-prove the full accepted editor/row-snapshot stack on the same tree.
sh tests/run_wp5_8_fm_editor_tests.sh > "$OUT/fm_editor.log" 2>&1
cat "$OUT/fm_editor.log"
grep -F 'WP5_8_FM_EDITOR_GATE_OK=1 decision=D056 pages=7 gesture=oct_under_fm_hold base_reference=exact one_row_refresh=exact generic_editor=unchanged row_snapshot=exact all8bit=wp5_0_identical hardware_margin_claim=none' "$OUT/fm_editor.log"

printf '%s\n' 'WP5_8_FM_BASE_API_GATE_OK=1 decision=D056 bytes=27 instruments=16 cache_rebuild=none plock_override=wins editor=exact all8bit=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
