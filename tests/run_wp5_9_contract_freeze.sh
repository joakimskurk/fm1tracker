#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=849c8b27d8c1afa09f9c093b695985c64e0c6013
OUT=build/wp5_9_contract
mkdir -p "$OUT"

# WP5.9 starts as validation-only. Target firmware, generated DSP and build
# tooling must still be byte-for-byte the accepted D056 checkpoint.
git diff --exit-code "$BASE" -- firmware/src firmware/gen tools
printf '%s\n' 'WP5_9_SOURCE_FREEZE_OK=1 production_delta=0 base=d056'

# Verify why this package exists and freeze the exact mixed audio oracle.
python3 tests/wp5_9_reference.py > "$OUT/reference.log" 2>&1
cat "$OUT/reference.log"
grep -F 'WP5_9_REFERENCE_OK=1 production_mix=two_real_families existing_real_fixture=single_track_sequential host_observability=pre_fm oracle=saturated_sum target_identity=d056_exact' "$OUT/reference.log"

# Re-prove the complete accepted D056 host closeout on the untouched production
# tree. Do not use a pipe here: a failing inherited gate must remain failing.
sh tests/run_wp5_8_closeout_host.sh > "$OUT/d056.log" 2>&1
cat "$OUT/d056.log"
grep -F 'WP5_8_CLOSEOUT_HOST_OK=1 decision=D056 ids=27 pages=7 locks=4 cache=8704 base_bank=432 snapshot=232 editor=page_aware base_api=bounded d055=exact d054_d049=exact all8bit=wp5_0_identical realtime=pass hardware_margin_claim=none' "$OUT/d056.log"

# Normative contract must keep the validation-only boundary and exact parent
# identity visible before any host-harness implementation starts.
grep -F 'WP5.9 is a validation package' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'FM_ONLY' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'BIT8_ONLY' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'COMBINED == expected' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'FM4 NOTE -> 8-BIT NOTE -> FM4 NOTE' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'image                    = 79804 B' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'normal RAM               = 27792 B' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'FWSC SHA-256             = 4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
grep -F 'hardware_margin_claim=none' docs/WP5_9_MIXED_VALIDATION_CONTRACT.md >/dev/null
printf '%s\n' 'WP5_9_CONTRACT_DOCS_OK=1 simultaneous=required switch=required events=required plock_family_safe=required target_identity=d056_exact'

printf '%s\n' 'WP5_9_CONTRACT_FREEZE_OK=1 production_delta=0 real_simultaneous=required same_track_switch=required event_semantics=required plock_family_safe=required target_identity=d056_exact hardware_margin_claim=none' > "$OUT/summary.txt"
cat "$OUT/summary.txt"
