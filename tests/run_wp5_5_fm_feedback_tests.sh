#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_5_feedback
mkdir -p "$OUT"

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_wp5_5_fm_feedback.c \
    firmware/src/fmt_fm_feedback.c \
    firmware/src/fmt_fm_operator.c \
    -o "$OUT/test_wp5_5_fm_feedback"

"$OUT/test_wp5_5_fm_feedback" "$OUT" | tee "$OUT/c_test.log"

# Independent frozen source-absent reference remains executable after implementation.
python3 tests/wp5_5_reference.py | tee "$OUT/reference.log"

test "$(wc -c < "$OUT/feedback_map.bin" | tr -d ' ')" = 512
test "$(wc -c < "$OUT/op4_4096.bin" | tr -d ' ')" = 16384
test "$(wc -c < "$OUT/algo_stack4_4096.bin" | tr -d ' ')" = 16384
test "$(wc -c < "$OUT/algo_fan3_4096.bin" | tr -d ' ')" = 16384
test "$(wc -c < "$OUT/algo_dual2_4096.bin" | tr -d ' ')" = 16384
test "$(wc -c < "$OUT/algo_parallel4_4096.bin" | tr -d ' ')" = 16384
test "$(wc -c < "$OUT/switch_4096.bin" | tr -d ' ')" = 16384
test "$(wc -c < "$OUT/extreme_stack4_65536.bin" | tr -d ' ')" = 262144
test "$(wc -c < "$OUT/extreme_fan3_65536.bin" | tr -d ' ')" = 262144
test "$(wc -c < "$OUT/extreme_dual2_65536.bin" | tr -d ' ')" = 262144
test "$(wc -c < "$OUT/extreme_parallel4_65536.bin" | tr -d ' ')" = 262144

sha256sum \
    "$OUT/feedback_map.bin" \
    "$OUT/op4_4096.bin" \
    "$OUT/algo_stack4_4096.bin" \
    "$OUT/algo_fan3_4096.bin" \
    "$OUT/algo_dual2_4096.bin" \
    "$OUT/algo_parallel4_4096.bin" \
    "$OUT/switch_4096.bin" \
    "$OUT/extreme_stack4_65536.bin" \
    "$OUT/extreme_fan3_65536.bin" \
    "$OUT/extreme_dual2_65536.bin" \
    "$OUT/extreme_parallel4_65536.bin" | tee "$OUT/payload_sha256.txt"

grep -F 'cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7' "$OUT/payload_sha256.txt"
grep -F '6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49' "$OUT/payload_sha256.txt"
grep -F '25f86f13f10413f5be3ff4b50160cafeac97d758933fb5d660225525a4eb9077' "$OUT/payload_sha256.txt"
grep -F '02e8b4556fb26a4134a6c257f1030b03c6808b8da2e597351fd19ef6ba9920fa' "$OUT/payload_sha256.txt"
grep -F 'f23d8e23e09ee1f4c4fd5d7247c125627b0624ac98fceb9bf98c8b1a6ff55a93' "$OUT/payload_sha256.txt"
grep -F '60ff90a19ccfc517213aa4fcce1a209240d546bbd3db5eecde9c695612501050' "$OUT/payload_sha256.txt"
grep -F '5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67' "$OUT/payload_sha256.txt"
grep -F 'd91af85e62f5791881a7b2b3bc560228ab820268c3fe7ca0762f9ae6d6922c0d' "$OUT/payload_sha256.txt"
grep -F '1037755115ed94cc460da484851a8f86fc49f8d19b6b9e76f55fa00c9bd65406' "$OUT/payload_sha256.txt"
grep -F '9da3125c952f75280296266236fd7a61afe93f6816354ce145d4d433fc0c6e46' "$OUT/payload_sha256.txt"
grep -F 'ed1c7ae3b47c83773e455757267da3692454d988e6c9e01dcba09acd02ea8d7a' "$OUT/payload_sha256.txt"

grep -F 'WP5_5_FM_FEEDBACK_C_STATE_OK=1 state_bytes=2 reset=zero retrigger=zero nulls=safe feedback_zero_tracks_history=1' "$OUT/c_test.log"
grep -F 'WP5_5_FM_FEEDBACK_C_MAP_OK=1 controls=256 malformed=saturate q14=0..16383' "$OUT/c_test.log"
grep -F 'WP5_5_FM_FEEDBACK_C_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216 min=-2147352576 max=2147287044' "$OUT/c_test.log"
grep -F 'WP5_5_FM_FEEDBACK_C_OP4_OK=1 renders=4096 delay_samples=1 final_history=13151' "$OUT/c_test.log"
grep -F 'WP5_5_FM_FEEDBACK_C_ALGORITHMS_OK=1 algorithms=4 renders_each=4096 op4_first=1 history=continuous' "$OUT/c_test.log"
grep -F 'WP5_5_FM_FEEDBACK_C_SWITCH_OK=1 renders=4096 switches=3 reset_on_switch=0 final_history=13151' "$OUT/c_test.log"
grep -F 'WP5_5_FM_FEEDBACK_C_EXTREME_OK=1 algorithms=4 renders_each=65536 output=q15_bounded final_history=2517' "$OUT/c_test.log"

grep -F 'WP5_5_D053_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216' "$OUT/reference.log"
grep -F 'WP5_5_D053_EXTREME_STRESS_OK=1 algorithms=4 renders_each=65536 feedback=255 index=16383 output=q15_bounded final_history=2517' "$OUT/reference.log"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
import re
from pathlib import Path

source = Path('firmware/src/fmt_fm_feedback.c').read_text()
header = Path('firmware/src/fmt_fm_feedback.h').read_text()
code = re.sub(r'/\*.*?\*/', '', source, flags=re.S)
code = re.sub(r'//[^\n]*', '', code)

for forbidden in ('uint64_t', 'int64_t', 'long long', 'float', 'double',
                  'malloc', 'calloc', 'realloc', 'free'):
    assert forbidden not in code, forbidden
assert '/' not in code, 'runtime division operator found'
assert '%' not in code, 'runtime modulo operator found'
assert not re.search(r'\b(for|while|do)\b', code), 'loop found in production feedback source'
assert not re.search(r'\bstatic\b', code), 'static runtime state/table found'
for forbidden_dep in (
    'fmt_fm_algorithm', 'fmt_fm_ratio', 'fmt_fm_controls',
    'fmt_engine_pitch', 'fmt_engine_router', 'fmt_store', 'dinghy'
):
    assert forbidden_dep not in source and forbidden_dep not in header, forbidden_dep
assert '#include "fmt_fm_operator.h"' in header
assert '#define FMT_FM_FEEDBACK_CONTROL_MAX 255u' in header
assert 'int16_t previous_op4_q15;' in header
assert 'fmt_fm_feedback_render_op4(' in header
assert 'fmt_fm_operator_render(' in source
print('WP5_5_FM_FEEDBACK_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 tables=0 mutable_globals=0 deps=d049_only')
PY

grep -F 'WP5_5_FM_FEEDBACK_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 tables=0 mutable_globals=0 deps=d049_only' "$OUT/static_audit.txt"

# Re-prove D052, which recursively re-proves D051/D050/D049/WP5.0 host behavior.
sh tests/run_wp5_4_fm_controls_tests.sh | tee "$OUT/inherited_d052.log"
grep -F 'WP5_4_FM_CONTROLS_HOST_GATE_OK=1 d052=exact exhaustive=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d052.log"

printf '%s\n' 'WP5_5_FM_FEEDBACK_HOST_GATE_OK=1 d053=exact history_surface=exhaustive algorithms=4 switch_history=continuous extreme_stress=pass static_bounds=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' | tee "$OUT/summary.txt"
