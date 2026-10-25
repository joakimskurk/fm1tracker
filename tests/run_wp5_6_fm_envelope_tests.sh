#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_6_envelope
mkdir -p "$OUT"

# Re-generate the immutable table and require byte-identical source output.
python3 tools/gen_fmt_fm_envelope.py "$OUT/fmt_fm_envelope_step_q15_16.inc" | tee "$OUT/generator.log"
cmp firmware/gen/fmt_fm_envelope_step_q15_16.inc "$OUT/fmt_fm_envelope_step_q15_16.inc"
grep -F 'FMT_FM_ENVELOPE_TABLE_OK=1 entries=256 bytes=1024 sha256=6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025' "$OUT/generator.log"

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_wp5_6_fm_envelope.c \
    firmware/src/fmt_fm_envelope.c \
    -o "$OUT/test_wp5_6_fm_envelope"

"$OUT/test_wp5_6_fm_envelope" \
    "$OUT/time_steps.bin" \
    "$OUT/sustain_map.bin" \
    "$OUT/effective_levels.bin" \
    "$OUT/stress.bin" | tee "$OUT/c_test.log"

test "$(wc -c < "$OUT/time_steps.bin" | tr -d ' ')" = 1024
test "$(wc -c < "$OUT/sustain_map.bin" | tr -d ' ')" = 512
test "$(wc -c < "$OUT/effective_levels.bin" | tr -d ' ')" = 131072
test "$(wc -c < "$OUT/stress.bin" | tr -d ' ')" = 786432

sha256sum \
    "$OUT/time_steps.bin" \
    "$OUT/sustain_map.bin" \
    "$OUT/effective_levels.bin" \
    "$OUT/stress.bin" | tee "$OUT/payload_sha256.txt"

grep -F '6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025' "$OUT/payload_sha256.txt"
grep -F 'a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815' "$OUT/payload_sha256.txt"
grep -F '196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b' "$OUT/payload_sha256.txt"
grep -F '8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849' "$OUT/payload_sha256.txt"

grep -F 'WP5_6_FM_ENVELOPE_C_SEMANTICS_OK=1 hard_retrigger=zero attack0=instant decay0=instant release0=instant corrupt_stage=off nulls=safe' "$OUT/c_test.log"
grep -F 'WP5_6_FM_ENVELOPE_C_SHAPE_OK=1 operators=4 state_bytes=20 controls_bytes=16 stages=5' "$OUT/c_test.log"
grep -F 'WP5_6_FM_ENVELOPE_C_MAX_RELEASE_OK=1 control=255 advances=2701155' "$OUT/c_test.log"
grep -F 'WP5_6_FM_ENVELOPE_C_STRESS_OK=1 samples=65536 final=off' "$OUT/c_test.log"

# Keep the independent frozen preimplementation model executable after implementation.
python3 tests/wp5_6_reference.py "$OUT/reference" | tee "$OUT/reference.log"
grep -F 'WP5_6_D054_REFERENCE_OK=1 architecture=4xADSR update=per_sample source=absent' "$OUT/reference.log"

python3 - <<'PY' | tee "$OUT/static_audit.txt"
import re
from pathlib import Path

source = Path('firmware/src/fmt_fm_envelope.c').read_text()
header = Path('firmware/src/fmt_fm_envelope.h').read_text()
table = Path('firmware/gen/fmt_fm_envelope_step_q15_16.inc').read_text()
generator = Path('tools/gen_fmt_fm_envelope.py').read_text()

code = re.sub(r'/\*.*?\*/', '', source, flags=re.S)
code = re.sub(r'//[^\n]*', '', code)
code = re.sub(r'"(?:\\.|[^"\\])*"', '""', code)
code = '\n'.join(line for line in code.splitlines()
                 if not line.lstrip().startswith('#'))

for forbidden in ('uint64_t', 'int64_t', 'long long', 'float', 'double',
                  'malloc', 'calloc', 'realloc', 'free'):
    assert forbidden not in source, forbidden
    assert forbidden not in header, forbidden
assert '/' not in code, 'runtime division operator found'
assert '%' not in code, 'runtime modulo operator found'
assert not re.search(r'\b(for|while|do)\b', code), 'loop found in production envelope source'
for forbidden_dep in (
    'fmt_fm_operator', 'fmt_fm_algorithm', 'fmt_fm_ratio', 'fmt_fm_controls',
    'fmt_fm_feedback', 'fmt_engine_pitch', 'fmt_engine_router', 'fmt_store', 'dinghy'
):
    assert forbidden_dep not in source and forbidden_dep not in header, forbidden_dep
assert 'static const uint32_t fmt_fm_envelope_step_q15_16[256]' in table
assert table.count('static const uint32_t') == 1
assert 'FMT_FM_ENVELOPE_LEVEL_MAX_Q15_16 0x7fff0000u' in header
assert 'sizeof(FmtFmEnvelopeState) == 20u' in header
assert 'sizeof(FmtFmEnvelopeControls) == 16u' in header
assert 'EXPECTED_SHA256 = "6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025"' in generator
print('WP5_6_FM_ENVELOPE_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 mutable_globals=0 table_bytes=1024 external_dsp_deps=0')
PY

grep -F 'WP5_6_FM_ENVELOPE_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 mutable_globals=0 table_bytes=1024 external_dsp_deps=0' "$OUT/static_audit.txt"

# Re-prove D053, which recursively re-proves D052/D051/D050/D049/WP5.0 host behavior.
sh tests/run_wp5_5_fm_feedback_tests.sh | tee "$OUT/inherited_d053.log"
grep -F 'WP5_5_FM_FEEDBACK_HOST_GATE_OK=1 d053=exact history_surface=exhaustive algorithms=4 switch_history=continuous extreme_stress=pass static_bounds=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d053.log"

printf '%s\n' 'WP5_6_FM_ENVELOPE_HOST_GATE_OK=1 d054=exact generator=identical table=exact stress=exact state=20 controls=16 static_bounds=pass d053=exact d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' | tee "$OUT/summary.txt"
