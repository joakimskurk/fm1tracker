#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/wp4_10

# WP4.10 / D046 is an exit-verification package. Production source stays frozen
# at the final WP4.9 source-hygiene checkpoint unless a focused gate proves a
# real defect. Each child gate independently verifies that invariant as well.
PRODUCTION=c93c39ad344760b4da0dfbc76829ca5fef886174
git diff --exit-code "$PRODUCTION" -- \
    firmware/src firmware/hal firmware/loader firmware/app.ld build.sh tools
printf '%s\n' 'WP4_10_EXIT_PRODUCTION_FREEZE_OK=1 baseline=c93c39ad344760b4da0dfbc76829ca5fef886174'

# D046 representative worst-case evidence: real linked adapter, all eight tracks,
# all four 8-bit engines, complete D028 lock pool, concurrent D034 pressure and a
# 4,096-row / 64-wrap deterministic run.
sh tests/run_wp4_10_representative_host.sh | tee build/wp4_10/exit_representative.log
grep -F 'WP4_10_FULL_POOL_LINKED_OK=1 tracks=8 rows=64 owners=512 locks=2048 engines=4' build/wp4_10/exit_representative.log
grep -F 'WP4_10_QUEUE_PRESSURE_OK=1 short_overflow=16 long_overflow=40 recovery=1 tracker_continues=1' build/wp4_10/exit_representative.log
grep -F 'WP4_10_LONG_RUN_OK=1 rows=4096 wraps=64 ticks=163840' build/wp4_10/exit_representative.log

# Focused structural proof for bounded callback ownership and exact single-clock
# sequencing. This is intentionally separate from the behavioral benchmark.
sh tests/run_wp4_10_realtime_audit.sh | tee build/wp4_10/exit_realtime.log
grep -F 'WP4_10_REALTIME_AUDIT_OK=1' build/wp4_10/exit_realtime.log
grep -F 'WP4_10_QUEUE_BOUND_OK=1 capacity=32 callback_cap=32 overflow_discard_o1=1 spin_retry=0 irq_masking=0' build/wp4_10/exit_realtime.log
grep -F 'WP4_10_ENGINE_BOUND_OK=1 render_slots=8 mutable_store_reads=0 lock_list_reads=0' build/wp4_10/exit_realtime.log
grep -F 'WP4_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6' build/wp4_10/exit_realtime.log

# Reuse the authoritative WP4.9 closeout gate verbatim for every inherited
# non-obsolete Phase 2/3 regression, canonical linked-audio hashes, pinned JieLi
# toolchain/AC79 SDK target build, resource ceilings, package identity and exact
# target artifact hashes. Do not weaken these checks by duplicating them here.
sh tests/run_wp4_9_closeout_ci.sh | tee build/wp4_10/exit_inherited_target.log
grep -F 'WP4_9_CLOSEOUT_OK=1 gates=5,6 host=linked_8bit target=pinned d037_ceiling=pass hardware_margin_claim=none' build/wp4_10/exit_inherited_target.log
grep -F 'FM1Tracker package' build/wp4_9_closeout_device.log
grep -F 'identity FM-1_987' build/wp4_9_closeout_device.log
grep -F 'fm1tracker_bin_sha256=214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215' build/wp4_9_closeout_metrics.txt
grep -F 'fm1tracker_fwsc_sha256=20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119' build/wp4_9_closeout_metrics.txt

# Compact machine-readable evidence consumed by the workflow and closeout docs.
REP_SHA="$(sed -n 's/^WP4_10_REPRESENTATIVE_WAV_SHA256=\([^ ]*\).*/\1/p' build/wp4_10/exit_representative.log | tail -n 1)"
test -n "$REP_SHA"
printf 'WP4_10_EXIT_OK=1 representative_wav_sha256=%s inherited_regressions=pass realtime=pass target=pinned identity=FM-1_987 phase4=exit_ready\n' "$REP_SHA" \
    | tee build/wp4_10/exit_summary.txt
