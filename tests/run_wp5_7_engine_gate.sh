#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_7_engine
BASE=f9122f6b1ea7e842e12bf80345cadc749d0b21e0
mkdir -p "$OUT"

changed=$(git diff --name-only "$BASE" -- firmware/src firmware/gen tools)
expected='firmware/src/fmt_fm_algorithm.c
firmware/src/fmt_fm_algorithm.h
firmware/src/fmt_fm_engine.c
firmware/src/fmt_fm_engine.h'
[ "$changed" = "$expected" ]
printf '%s\n' 'WP5_7_ENGINE_SOURCE_BOUNDARY_OK=1 files=4 dinghy=unchanged d043=unchanged d049_d051_d052_d053_d054=unchanged' | tee "$OUT/source_boundary.txt"

cc -std=c11 -Wall -Wextra -Werror -Ifirmware/src \
  tests/wp5_7_engine_host.c \
  firmware/src/fmt_fm_operator.c \
  firmware/src/fmt_fm_algorithm.c \
  firmware/src/fmt_fm_ratio.c \
  firmware/src/fmt_fm_controls.c \
  firmware/src/fmt_fm_feedback.c \
  firmware/src/fmt_fm_envelope.c \
  firmware/src/fmt_fm_engine.c \
  firmware/src/fmt_engine_pitch.c \
  firmware/src/fmt_voice_allocator.c \
  -o "$OUT/wp5_7_engine_host"
"$OUT/wp5_7_engine_host" | tee "$OUT/engine.log"

grep -F 'WP5_7_D050_SEAM_EQUIVALENT_OK=1 algorithms=4 frames_each=4096 op4_advance=once' "$OUT/engine.log"
grep -F 'WP5_7_FM_ENGINE_OWNERSHIP_OK=1 slots=8 active_cross_track_steals=0 release_tail=reused malformed=safe_silence' "$OUT/engine.log"
grep -F 'WP5_7_FM_ENGINE_EVENT_OK=1 note=hard_retrigger retrig=hard_reset off_cut=detached_release' "$OUT/engine.log"
grep -F 'WP5_7_FM_ENGINE_HOST_OK=1 d050=compatible engine=788 voice=92 slots=8' "$OUT/engine.log"

# Re-run the accepted D050 host suite to prove the public renderer stayed exact.
sh tests/run_wp5_2_fm_algorithm_tests.sh | tee "$OUT/d050.log"
grep -F 'WP5_2_FM_ALGORITHM_HOST_GATE_OK=1' "$OUT/d050.log"

printf '%s\n' 'WP5_7_ENGINE_ISOLATED_GATE_OK=1 d050=exact engine=788 allocator=d043 integration=not_yet_linked' | tee "$OUT/summary.txt"
