#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=80587f12786122dcec819260ddad5dd5db4b1005
mkdir -p build/wp5_8_contract

# D056 must be frozen while production is still byte-for-byte the accepted
# WP5.7 closeout. No parameter registry activation or FM linked source exists.
git diff --exit-code "$BASE" -- firmware/src firmware/gen
test ! -e firmware/src/fmt_fm_params.h
test ! -e firmware/src/fmt_fm_params.c
test ! -e firmware/src/fmt_fm_linked_params.h
test ! -e firmware/src/fmt_fm_linked_params.c
printf '%s\n' 'WP5_8_SOURCE_ABSENT_OK=1 firmware_delta=0 new_fm_param_sources=0'

python3 tests/wp5_8_reference.py | tee build/wp5_8_contract/reference.log
grep -F 'WP5_8_REFERENCE_OK=1 ids=27 pages=7 base_bytes=27 bank_bytes=432 legacy_cache=2048 generalized_cache=8704 net_cache_growth=6656 known_ram_growth=7088' build/wp5_8_contract/reference.log

# The source-absent state still has the exact inherited D027 registry runtime.
sh tests/run_param_registry_tests.sh > build/wp5_8_contract/param_registry.log
grep -F 'FMT_PARAM_REGISTRY_TESTS_OK=1' build/wp5_8_contract/param_registry.log

# Re-run the accepted *final integrated* D055 host gate. Do not call the older
# pre-integration run_wp5_7_engine_gate.sh here: that gate intentionally requires
# dinghy.c to be unchanged and is therefore invalid after D055 integration.
sh tests/run_wp5_7_integration_tests.sh > build/wp5_8_contract/d055.log
cat build/wp5_8_contract/d055.log
grep -F 'WP5_7_HOST_GATE_OK=1 d055=integrated engine=788 slots=8 d043=exact d048=integrated d054=exact all8bit=wp5_0_identical mixed=deterministic hardware_margin_claim=none' build/wp5_8_contract/d055.log

# Normative docs must explicitly contain the frozen IDs/resource/source rules.
grep -F '`0x60` | `FM_ALGORITHM`' docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md >/dev/null
grep -F '`0x7B` | `FM_OP4_RELEASE`' docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md >/dev/null
grep -F 'total              8704 B' docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md >/dev/null
grep -F 'normal RAM  <= 28656 B' docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md >/dev/null
grep -F 'firmware/src/fmt_fm_engine.*' docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md >/dev/null
printf '%s\n' 'WP5_8_CONTRACT_DOCS_OK=1 decision=D056 ids=27 pages=7 source_boundary=frozen'

printf '%s\n' 'WP5_8_CONTRACT_FREEZE_OK=1 decision=D056 production_authorized_after_green=1 hardware_margin_claim=none' | tee build/wp5_8_contract/summary.txt
