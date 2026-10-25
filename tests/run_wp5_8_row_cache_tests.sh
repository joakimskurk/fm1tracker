#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/wp5_8_row_cache

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_fmt_fm_row_cache.c \
    firmware/src/fmt_fm_linked_params.c \
    firmware/src/fmt_fm_params.c \
    -o build/host/test_fmt_fm_row_cache

build/host/test_fmt_fm_row_cache | tee build/wp5_8_row_cache/row_cache.log
grep -F 'WP5_8_ROW_CACHE_OK=1 total=8704 legacy=2048 base_bank=432 slots=4 banks=2 selector=byte preemption=deterministic' build/wp5_8_row_cache/row_cache.log

# Re-prove the complete first D056 production slice and exact all-8BIT stream on
# the generalized physical cache allocation.
sh tests/run_wp5_8_param_model_tests.sh | tee build/wp5_8_row_cache/param_model.log
grep -F 'WP5_8_PARAM_MODEL_GATE_OK=1 decision=D056 ids=27 pages=7 registry=37 defaults=d055_exact legacy8bit=unchanged hardware_margin_claim=none' build/wp5_8_row_cache/param_model.log

printf '%s\n' 'WP5_8_ROW_CACHE_GATE_OK=1 decision=D056 total=8704 legacy=2048 fm_base=432 double_buffer=exact preemption=deterministic all8bit=wp5_0_identical hardware_margin_claim=none' | tee build/wp5_8_row_cache/summary.txt
