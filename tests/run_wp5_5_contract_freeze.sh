#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_5_contract
BASE=d3501eca952f3e903d4dcc4faab2851d439f7ad0
mkdir -p "$OUT"

# D053 must be frozen before any feedback production source.
grep -F 'Status: **FROZEN BY D053 BEFORE FM FEEDBACK PRODUCTION SOURCE**' docs/WP5_5_FM_FEEDBACK_CONTRACT.md
test ! -e firmware/src/fmt_fm_feedback.h
test ! -e firmware/src/fmt_fm_feedback.c
! grep -R -F 'fmt_fm_feedback' firmware/src firmware/gen

# Frozen DSP layers and running application remain untouched at contract freeze.
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_fm_operator.h \
    firmware/src/fmt_fm_operator.c \
    firmware/src/fmt_fm_algorithm.h \
    firmware/src/fmt_fm_algorithm.c \
    firmware/src/fmt_fm_ratio.h \
    firmware/src/fmt_fm_ratio.c \
    firmware/src/fmt_fm_controls.h \
    firmware/src/fmt_fm_controls.c \
    firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_engine_pitch.c \
    firmware/gen/fmt_fm_sine_q15.inc
printf '%s\n' 'WP5_5_D053_PRODUCTION_ABSENT_OK=1 feedback_source=absent d049_delta=0 d050_delta=0 d051_delta=0 d052_delta=0 dinghy_delta=0'

python3 tests/wp5_5_reference.py | tee "$OUT/reference.log"
grep -F 'WP5_5_D053_D050_REFERENCE_OK=1 algorithms=4 renders_each=4096 hashes=exact' "$OUT/reference.log"
grep -F 'WP5_5_D053_CONTROL_MAP_OK=1 controls=256 q14=0..16383 same_numeric_map_as_d052_index=1' "$OUT/reference.log"
grep -F 'WP5_5_D053_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216' "$OUT/reference.log"
grep -F 'WP5_5_D053_OP4_FIXTURE_OK=1 delay_samples=1 renders=4096 final_history=13151' "$OUT/reference.log"
grep -F 'WP5_5_D053_ALGORITHM_FIXTURES_OK=1 algorithms=4 renders_each=4096 history=continuous op4_first=1' "$OUT/reference.log"
grep -F 'WP5_5_D053_SWITCH_CONTINUITY_OK=1 renders=4096 switches=3 reset_on_switch=0' "$OUT/reference.log"
grep -F 'WP5_5_D053_EXTREME_STRESS_OK=1 algorithms=4 renders_each=65536 feedback=255 index=16383 output=q15_bounded final_history=2517' "$OUT/reference.log"

# Re-prove D052, which recursively re-proves D051/D050/D049/WP5.0 host behavior.
sh tests/run_wp5_4_fm_controls_tests.sh | tee "$OUT/d052_inherited.log"
grep -F 'WP5_4_FM_CONTROLS_HOST_GATE_OK=1 d052=exact exhaustive=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/d052_inherited.log"

printf '%s\n' 'WP5_5_D053_CONTRACT_FREEZE_OK=1 production=absent topology=op4_self delay=one_sample state_bytes=2 control=byte_q14 history_surface=exhaustive switch_history=continuous extreme_stress=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
