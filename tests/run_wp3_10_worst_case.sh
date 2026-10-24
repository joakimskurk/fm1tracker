#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests build

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp3_10_worst_case.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_playback_params.c \
    firmware/src/fmt_tracker_effect.c \
    firmware/src/fmt_rt_queue.c \
    firmware/src/fmt_transport.c \
    firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_step_record.c \
    firmware/src/fmt_lock_editor.c \
    firmware/src/fmt_tracker_screen.c \
    -o build/tests/test_wp3_10_worst_case

./build/tests/test_wp3_10_worst_case | tee build/wp3_10_worst_case.log

grep -F 'WP3_10_LOCK_POOL_STRESS_OK=1 records=2048 owners=512' build/wp3_10_worst_case.log
grep -F 'WP3_10_CACHE_STRESS_OK=1 rows=64 locks_per_row=4 playback_cache=256 effect_cache=256' build/wp3_10_worst_case.log
grep -F 'WP3_10_QUEUE_STRESS_OK=1 cycles=2048 rejected=6144' build/wp3_10_worst_case.log
grep -F 'WP3_10_SCREEN_STRESS_OK=1 visible_rows=8 locks_inspected_max=32 full_polls=5 row_backlog_polls=4 converged=1' build/wp3_10_worst_case.log
grep -F 'WP3_10_FOCUSED_WORST_CASE_OK=1' build/wp3_10_worst_case.log
