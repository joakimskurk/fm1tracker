#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_6_contract
BASE=216c7e34b6d761cadb683bd1fc356c216b98cc27
mkdir -p "$OUT"

# Production envelope source/generated table must not exist before D054 acceptance.
test ! -e firmware/src/fmt_fm_envelope.h
test ! -e firmware/src/fmt_fm_envelope.c
test ! -e firmware/gen/fmt_fm_envelope_step_q15_16.inc
test ! -e tools/gen_fmt_fm_envelope.py
printf '%s\n' 'WP5_6_D054_SOURCE_ABSENT_OK=1 envelope_source=absent generated_table=absent generator=absent' | tee "$OUT/source_absent.txt"

# D049-D053 and live application integration remain exactly at the accepted WP5.5 closeout.
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_engine_pitch.c \
    firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_fm_operator.c \
    firmware/src/fmt_fm_operator.h \
    firmware/src/fmt_fm_algorithm.c \
    firmware/src/fmt_fm_algorithm.h \
    firmware/src/fmt_fm_ratio.c \
    firmware/src/fmt_fm_ratio.h \
    firmware/src/fmt_fm_controls.c \
    firmware/src/fmt_fm_controls.h \
    firmware/src/fmt_fm_feedback.c \
    firmware/src/fmt_fm_feedback.h \
    firmware/gen/fmt_fm_sine_q15.inc
printf '%s\n' 'WP5_6_D054_PARENT_FREEZE_OK=1 d049=unchanged d050=unchanged d051=unchanged d052=unchanged d053=unchanged dinghy=unchanged' | tee "$OUT/parent_freeze.txt"

python3 tests/wp5_6_reference.py "$OUT" | tee "$OUT/reference.log"

grep -F 'WP5_6_D054_MODEL_SHAPE_OK=1 operators=4 state_bytes=20 controls_bytes=16 stages=5 q15_16_max=0x7fff0000' "$OUT/reference.log"
grep -F 'WP5_6_D054_TIME_MAP_OK=1 controls=256 bytes=1024 sha256=6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025 max_release_steps=2701155' "$OUT/reference.log"
grep -F 'WP5_6_D054_SUSTAIN_MAP_OK=1 controls=256 sha256=a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815 malformed=saturate' "$OUT/reference.log"
grep -F 'WP5_6_D054_EFFECTIVE_LEVEL_OK=1 pairs=65536 bytes=131072 sha256=196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b full_env=identity' "$OUT/reference.log"
grep -F 'WP5_6_D054_ZERO_RETRIGGER_OK=1 hard_retrigger=zero attack0=instant decay0=instant release0=instant corrupt_stage=off null_controls=silence' "$OUT/reference.log"
grep -F 'WP5_6_D054_MAX_RELEASE_OK=1 control=255 advances=2701155' "$OUT/reference.log"
grep -F 'WP5_6_D054_STRESS_OK=1 samples=65536 bytes=786432 sha256=8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849 final=off' "$OUT/reference.log"
grep -F 'WP5_6_D054_REFERENCE_OK=1 architecture=4xADSR update=per_sample source=absent' "$OUT/reference.log"

# Re-prove D053, which recursively re-proves D052/D051/D050/D049/WP5.0 host behavior.
sh tests/run_wp5_5_fm_feedback_tests.sh | tee "$OUT/inherited_d053.log"
grep -F 'WP5_5_FM_FEEDBACK_HOST_GATE_OK=1 d053=exact history_surface=exhaustive algorithms=4 switch_history=continuous extreme_stress=pass static_bounds=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d053.log"

printf '%s\n' 'WP5_6_D054_CONTRACT_GATE_OK=1 source=absent architecture=4xADSR state=20 controls=16 time_map=exact d053=exact full_app_wav=wp5_0_identical' | tee "$OUT/summary.txt"
