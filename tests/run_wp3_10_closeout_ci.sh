#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

WP3_9=354bf7d356774ad828f1f660bd456dbab7f0490d

# D036 found no production defect. Phase 3 closeout therefore requires exact
# production-source identity with the final WP3.9 verified firmware baseline.
git diff --exit-code "$WP3_9" -- firmware/src firmware/hal firmware/loader firmware/app.ld build.sh tools
echo "WP3_10_PRODUCTION_IDENTICAL_OK=1 baseline=$WP3_9"

# Re-run the complete WP3.9 inherited regression + pinned JieLi/AC79 device
# build as the authoritative production/device baseline. Do not pipe through
# tee here: preserve the script's exit status.
sh tests/run_wp3_9_closeout_ci.sh > build/wp3_10_inherited_phase3.log
cat build/wp3_10_inherited_phase3.log
grep -F 'WP3_9_CLOSEOUT_OK=1' build/wp3_10_inherited_phase3.log

# D036 focused and combined worst cases.
sh tests/run_wp3_10_worst_case.sh > build/wp3_10_worst_case_final.log
cat build/wp3_10_worst_case_final.log
grep -F 'WP3_10_FOCUSED_WORST_CASE_OK=1' build/wp3_10_worst_case_final.log

a="build/wp3_10_combined_final.log"
sh tests/run_wp3_10_combined_host.sh > "$a"
cat "$a"
grep -F 'WP3_10_COMBINED_STRESS_OK=1 deterministic=1 two_overflow_epochs=1 recovery=1 midi_neutral=1 screen_queue_neutral=1' "$a"
grep -F 'WP3_10_COMBINED_CONTROL_WAV_SHA256=7bdc491d929013bbf0e95005c95d4a21ac500960d418ba4ca0812a59991624ee' build/wp3_10_combined_analysis.txt
grep -F 'WP3_10_COMBINED_STRESS_WAV_SHA256=d471a5fa3a9fcf4f7a826b8afb8c13ba5c2b15a8d8df7a28ec96889b4f711c83' build/wp3_10_combined_analysis.txt
grep -F 'WP3_10_COMBINED_SCREEN_1000_SHA256=09e2041ae6f9133a0e1401d2b9da08797515c70dac1eb8ea46503cb1f9ea8cd5' build/wp3_10_combined_analysis.txt
grep -F 'WP3_10_COMBINED_SCREEN_3000_SHA256=c0362e8ca91d36240682c8c0845d4ac739fb14015601def788a6f4d6825ee916' build/wp3_10_combined_analysis.txt

sh tests/run_wp3_10_timing_matrix.sh > build/wp3_10_timing_final.log
cat build/wp3_10_timing_final.log
grep -F 'WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1 hash=364d0c2c' build/wp3_10_timing_final.log
grep -F 'WP3_10_LONG_TICK_MATRIX_OK=1 rows=4096 wraps=64 frames=22579200 effect_ticks=20480 actions=512 final_step=0 final_phase=0' build/wp3_10_timing_final.log
grep -F 'WP3_10_TIMING_EXIT_OK=1' build/wp3_10_timing_final.log

sh tests/run_wp3_10_realtime_audit.sh > build/wp3_10_realtime_final.log
cat build/wp3_10_realtime_final.log
grep -F 'WP3_10_REALTIME_AUDIT_OK=1' build/wp3_10_realtime_final.log

# Since production source is byte-identical to WP3.9, the final target artifacts
# and linked resources must also be exactly the frozen WP3.9 values.
BIN_SHA="$(sha256sum build/fm1tracker.bin | awk '{print $1}')"
FWSC_SHA="$(sha256sum build/fm1tracker-0.1.fwsc | awk '{print $1}')"
test "$BIN_SHA" = 'b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27'
test "$FWSC_SHA" = '7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c'

grep -F 'image_bytes=66960' build/wp3_9_measured_metrics.txt
grep -F 'normal_ram_bytes=13296' build/wp3_9_measured_metrics.txt
grep -F 'pool_ram_bytes=113000' build/wp3_9_measured_metrics.txt
grep -F 'tracker_screen_bytes=62' build/wp3_9_measured_metrics.txt
grep -F 'default_wav_sha256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440' build/wp3_9_measured_metrics.txt
grep -F 'd031_locked_wav_sha256=8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8' build/wp3_9_measured_metrics.txt

printf '%s\n' \
    'WP3_10_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 production_delta=0' \
    "WP3_10_FINAL_BIN_SHA256=$BIN_SHA" \
    "WP3_10_FINAL_FWSC_SHA256=$FWSC_SHA" \
    'WP3_10_PHASE3_EXIT_OK=1 wp3_1_through_wp3_10=complete host_device_build=1 physical_hardware_pending=1' \
    'WP3_10_CLOSEOUT_OK=1' \
    | tee build/wp3_10_final_summary.txt
