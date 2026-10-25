#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_7_contract
BASE=886cb496f4e73a91c4ded16bf9e722b770a040f6
mkdir -p "$OUT"

# D055 production integration source/seam must not exist before acceptance.
test ! -e firmware/src/fmt_fm_engine.h
test ! -e firmware/src/fmt_fm_engine.c
! grep -F 'fmt_fm_algorithm_render_from_op4' firmware/src/fmt_fm_algorithm.h
! grep -F 'fmt_fm_algorithm_render_from_op4' firmware/src/fmt_fm_algorithm.c
printf '%s\n' 'WP5_7_D055_SOURCE_ABSENT_OK=1 fm_engine=absent d050_composition_seam=absent' | tee "$OUT/source_absent.txt"

# Production tree is still exactly the accepted WP5.6 closeout.
git diff --exit-code "$BASE" -- firmware/src firmware/gen tools
printf '%s\n' 'WP5_7_D055_PARENT_FREEZE_OK=1 d043=unchanged d048=unchanged d049=unchanged d050=unchanged d051=unchanged d052=unchanged d053=unchanged d054=unchanged dinghy=unchanged' | tee "$OUT/parent_freeze.txt"

python3 tests/wp5_7_reference.py "$OUT/reference" | tee "$OUT/reference.log"

grep -F 'WP5_7_D055_MODEL_SHAPE_OK=1 slots=8 tracks=8 voice_bytes=92 allocator_bytes=52 engine_bytes=788 patch_bytes=28' "$OUT/reference.log"
grep -F 'WP5_7_D055_DEFAULT_PATCH_OK=1 bytes=28 sha256=3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52 algorithm=DUAL2 index=64 feedback=0' "$OUT/reference.log"
grep -F 'WP5_7_D055_STAGE_VIEW_OK=1 idle=all_off release=off_or_release active=otherwise' "$OUT/reference.log"
grep -F 'WP5_7_D055_ALLOCATOR_STRESS_OK=1 events=4096 trace_bytes=262144 sha256=97b87ba4b287c4bdc6cbb41021e3f5d75d9f62897f5aa5d63d5ebdc5927280af reuse=1896 fresh=565 steal_tail=124 active_cross_track_steals=0' "$OUT/reference.log"
grep -F 'WP5_7_D055_ROUTE_MODEL_OK=1 cross_family=silence_clear_attempt_commit failure=none tie_empty_retrig=no_route_change off_cut=release_then_clear' "$OUT/reference.log"
grep -F 'WP5_7_D055_RESOURCE_MODEL_OK=1 engine_ram=788 ram_increment_ceiling=1024 total_ram_ceiling=20700 image_increment_ceiling=8192 total_image_ceiling=80984 pool=113000' "$OUT/reference.log"
grep -F 'WP5_7_D055_REFERENCE_OK=1 source=absent allocator=d043-compatible family_router=d048-compatible' "$OUT/reference.log"

# D054 recursively includes the D053/D052/D051/D050/D049/WP5.0 host chain.
sh tests/run_wp5_6_fm_envelope_tests.sh | tee "$OUT/inherited_d054.log"
grep -F 'WP5_6_FM_ENVELOPE_HOST_GATE_OK=1 d054=exact generator=identical table=exact stress=exact state=20 controls=16 static_bounds=pass d053=exact d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/inherited_d054.log"

printf '%s\n' 'WP5_7_D055_CONTRACT_GATE_OK=1 source=absent slots=8 voice=92 engine=788 allocator=d043 route=d048 d054=exact full_app_wav=wp5_0_identical' | tee "$OUT/summary.txt"
