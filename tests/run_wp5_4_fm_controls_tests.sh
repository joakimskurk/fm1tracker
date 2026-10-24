#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_4_controls
mkdir -p "$OUT"

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_wp5_4_fm_controls.c \
    firmware/src/fmt_fm_controls.c \
    -o "$OUT/test_wp5_4_fm_controls"

"$OUT/test_wp5_4_fm_controls" \
    "$OUT/mappings.bin" \
    "$OUT/edge_surface.bin" \
    "$OUT/composition.bin" | tee "$OUT/c_test.log"

test "$(wc -c < "$OUT/mappings.bin" | tr -d ' ')" = 1024
test "$(wc -c < "$OUT/edge_surface.bin" | tr -d ' ')" = 262144
test "$(wc -c < "$OUT/composition.bin" | tr -d ' ')" = 78

sha256sum "$OUT/mappings.bin" "$OUT/edge_surface.bin" "$OUT/composition.bin" | tee "$OUT/payload_sha256.txt"
grep -F '26e55d6a5fcacb5f432ecc3620b41a1c18e23f277aea6a13cb72d988a1929b38' "$OUT/payload_sha256.txt"
grep -F 'cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0' "$OUT/payload_sha256.txt"
grep -F 'e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c' "$OUT/payload_sha256.txt"

grep -F 'WP5_4_FM_CONTROLS_C_MAP_OK=1 controls=256 struct_bytes=10 malformed=saturate null_levels=silence' "$OUT/c_test.log"
grep -F 'WP5_4_FM_CONTROLS_C_EDGE_OK=1 pairs=65536 max=2147287044 int32_margin=196603' "$OUT/c_test.log"
grep -F 'WP5_4_FM_CONTROLS_C_COMPOSITION_OK=1 vectors=3 bytes=78' "$OUT/c_test.log"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
import re
from pathlib import Path

source = Path('firmware/src/fmt_fm_controls.c').read_text()
header = Path('firmware/src/fmt_fm_controls.h').read_text()
code = re.sub(r'/\*.*?\*/', '', source, flags=re.S)
code = re.sub(r'//[^\n]*', '', code)

for forbidden in ('uint64_t', 'int64_t', 'long long', 'float', 'double', 'malloc', 'calloc', 'realloc', 'free'):
    assert forbidden not in code, forbidden
assert '/' not in code, 'runtime division operator found'
assert '%' not in code, 'runtime modulo operator found'
assert not re.search(r'\b(for|while|do)\b', code), 'loop found in production controls source'
assert not re.search(r'\bstatic\b', code), 'static runtime state or table found'
for forbidden_dep in (
    'fmt_fm_operator', 'fmt_fm_algorithm', 'fmt_fm_ratio',
    'fmt_engine_pitch', 'fmt_engine_router', 'fmt_store', 'dinghy'
):
    assert forbidden_dep not in source, forbidden_dep
assert '#define FMT_FM_CONTROL_MAX 255u' in header
assert 'uint16_t level_q15[4];' in header
assert 'uint16_t mod_depth_q14;' in header
assert 'fmt_fm_control_sanitize(uint32_t control)' in header
assert 'fmt_fm_level_q15(uint32_t control)' in header
assert 'fmt_fm_index_q14(uint32_t control)' in header
assert 'fmt_fm_controls_map(' in header
print('WP5_4_FM_CONTROLS_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 tables=0 external_dsp_deps=0')
PY

grep -F 'WP5_4_FM_CONTROLS_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 tables=0 external_dsp_deps=0' "$OUT/static_audit.txt"

# Re-prove D051, which recursively re-proves D050/D049/WP5.0 host behavior.
sh tests/run_wp5_3_fm_ratio_tests.sh | tee "$OUT/inherited_d051.log"
grep -F 'WP5_3_FM_RATIO_HOST_GATE_OK=1 d051=exact exhaustive=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d051.log"

printf '%s\n' 'WP5_4_FM_CONTROLS_HOST_GATE_OK=1 d052=exact exhaustive=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' | tee "$OUT/summary.txt"
