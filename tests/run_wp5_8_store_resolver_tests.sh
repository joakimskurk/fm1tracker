#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/wp5_8_store_resolver

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    -include firmware/src/fmt_playback.h \
    tests/test_fmt_fm_store_resolver.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_fm_params.c \
    firmware/src/fmt_fm_linked_params.c \
    firmware/src/fmt_8bit_params.c \
    firmware/src/fmt_8bit_linked_params.c \
    -o build/host/test_fmt_fm_store_resolver

build/host/test_fmt_fm_store_resolver > build/wp5_8_store_resolver/resolver.log
cat build/wp5_8_store_resolver/resolver.log
grep -F 'WP5_8_STORE_RESOLVER_OK=1 note_inherit=exact locks=4 uint16_filter=no_truncate wrong_family=ignored structural=failsafe shared_low=protected' build/wp5_8_store_resolver/resolver.log

# The generic D028 sparse store remains authoritative and unchanged. Do not pipe
# this command through tee: a failing historical regression must fail this gate.
sh tests/run_lock_store_tests.sh > build/wp5_8_store_resolver/lock_store.log 2>&1
cat build/wp5_8_store_resolver/lock_store.log
grep -F 'FMT_LOCK_STORE_TESTS_OK=1' build/wp5_8_store_resolver/lock_store.log

# Re-prove the accepted physical cache and the complete parameter-model
# regression, including exact D055 behavior and the all-8BIT WP5.0 WAV.
sh tests/run_wp5_8_row_cache_tests.sh > build/wp5_8_store_resolver/row_cache.log 2>&1
cat build/wp5_8_store_resolver/row_cache.log
grep -F 'WP5_8_ROW_CACHE_GATE_OK=1 decision=D056 total=8704 legacy=2048 fm_base=432 double_buffer=exact preemption=deterministic all8bit=wp5_0_identical hardware_margin_claim=none' build/wp5_8_store_resolver/row_cache.log

printf '%s\n' 'WP5_8_STORE_RESOLVER_GATE_OK=1 decision=D056 note_inherit=exact locks=4 uint16_filter=no_truncate wrong_family=ignored structural=failsafe shared_cache=protected d028=exact all8bit=wp5_0_identical hardware_margin_claim=none' > build/wp5_8_store_resolver/summary.txt
cat build/wp5_8_store_resolver/summary.txt
