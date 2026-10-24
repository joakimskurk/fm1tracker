#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_3_ratio
mkdir -p "$OUT"

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_wp5_3_fm_ratio.c \
    firmware/src/fmt_engine_pitch.c \
    firmware/src/fmt_fm_ratio.c \
    -o "$OUT/test_wp5_3_fm_ratio"

"$OUT/test_wp5_3_fm_ratio" "$OUT/exhaustive.bin" | tee "$OUT/c_test.log"

test "$(wc -c < "$OUT/exhaustive.bin" | tr -d ' ')" = 1048576
sha256sum "$OUT/exhaustive.bin" | tee "$OUT/exhaustive_sha256.txt"
grep -F '5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c' "$OUT/exhaustive_sha256.txt"
grep -F 'WP5_3_FM_RATIO_C_ANCHORS_OK=1 ids=16 unity=1 map4=exact malformed=exact' "$OUT/c_test.log"
grep -F 'WP5_3_FM_RATIO_C_EXHAUSTIVE_OK=1 pitches=16384 ratios=16 outputs=262144 bytes=1048576 clipped=27615' "$OUT/c_test.log"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
import re
from pathlib import Path

source = Path('firmware/src/fmt_fm_ratio.c').read_text()
header = Path('firmware/src/fmt_fm_ratio.h').read_text()
code = re.sub(r'/\*.*?\*/', '', source, flags=re.S)
code = re.sub(r'//[^\n]*', '', code)

for forbidden in ('uint64_t', 'int64_t', 'long long', 'float', 'double', 'malloc', 'calloc', 'realloc', 'free'):
    assert forbidden not in code, forbidden
assert '/' not in code, 'runtime division operator found'
assert '%' not in code, 'runtime modulo operator found'
assert not re.search(r'\b(for|while|do)\b', code), 'loop found in production ratio source'
assert 'fmt_engine_pitch' not in source
assert 'fmt_fm_operator' not in source
assert 'fmt_fm_algorithm' not in source
assert 'dinghy' not in source
assert 'FMT_FM_RATIO_COUNT 16u' in header
assert 'FMT_FM_RATIO_BASE_INCREMENT_MAX 1293741834u' in header
assert 'FMT_FM_RATIO_INCREMENT_CEILING 2147483647u' in header
print('WP5_3_FM_RATIO_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 external_dsp_deps=0')
PY

grep -F 'WP5_3_FM_RATIO_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 external_dsp_deps=0' "$OUT/static_audit.txt"

# Re-prove all inherited D050/D049/WP5.0 host behavior.
sh tests/run_wp5_2_fm_algorithm_tests.sh | tee "$OUT/inherited_d050.log"
grep -F 'WP5_2_FM_ALGORITHM_HOST_GATE_OK=1 d050=exact algorithms=4 hashes=exact stress=pass d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d050.log"

printf '%s\n' 'WP5_3_FM_RATIO_HOST_GATE_OK=1 d051=exact exhaustive=exact d050=exact d049=exact full_app_wav=wp5_0_identical' | tee "$OUT/summary.txt"
