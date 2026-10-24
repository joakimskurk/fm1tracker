#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/wp5_0_inherited

# The D045 inherited suite already excludes obsolete proof-sine hashes and runs
# every component/context/UI/stress/timing contract that remains authoritative.
# D048 has its own stronger router realtime audit, but the inherited D045 audit
# must also continue to pass so migration does not silently weaken old bounds.
sh tests/run_wp4_9_inherited_regressions.sh | tee build/wp5_0_inherited/inherited.log

grep -F 'FMT_MODEL_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_STORE_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_TRANSPORT_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_PLAYBACK_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_PATTERN_INPUT_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_STEP_RECORD_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_TRACKER_SCREEN_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_PARAM_REGISTRY_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_LOCK_STORE_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_LOCK_EDITOR_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_PLAYBACK_PARAMS_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_TRACKER_EFFECT_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_TRACKER_EVENT_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'FMT_RT_QUEUE_TESTS_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP2_5_PATTERN_CONTEXT_HOST_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP2_6_TRACK_CONTEXT_HOST_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP2_7_STEP_REC_CONTEXT_HOST_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP4_9_UI_CONTEXT_REGRESSIONS_OK=1 lock_editor_midi=0 lock_delete_midi=0 bounded_visual_frames=1' build/wp5_0_inherited/inherited.log
grep -F 'WP3_10_FOCUSED_WORST_CASE_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP3_10_TIMING_EXIT_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP4_9_GATE6_REALTIME_AUDIT_OK=1' build/wp5_0_inherited/inherited.log
grep -F 'WP4_9_GATE6_INHERITED_REGRESSIONS_OK=1 obsolete_proof_sine_hashes_excluded=1' build/wp5_0_inherited/inherited.log

printf '%s\n' 'WP5_0_INHERITED_REGRESSIONS_OK=1 phase1_4=applicable contracts=pass obsolete_proof_sine=excluded' | tee build/wp5_0_inherited/summary.txt
