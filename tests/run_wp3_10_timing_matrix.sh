#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests build

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp3_10_timing_matrix.c \
    firmware/src/fmt_transport.c \
    firmware/src/fmt_tracker_effect.c \
    firmware/src/fmt_tracker_event.c \
    firmware/src/fmt_store.c \
    -o build/tests/test_wp3_10_timing_matrix

./build/tests/test_wp3_10_timing_matrix | tee build/wp3_10_timing_matrix.log

grep -F 'WP3_10_LONG_TICK_MATRIX_OK=1 rows=4096 wraps=64' build/wp3_10_timing_matrix.log
grep -F 'effect_ticks=20480' build/wp3_10_timing_matrix.log
grep -F 'final_step=0 final_phase=0' build/wp3_10_timing_matrix.log
grep -F 'WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1' build/wp3_10_timing_matrix.log

# Retain the independent six-hour D020 proof and exact 120-BPM frame anchors.
sh tests/run_transport_tests.sh | tee build/wp3_10_transport_long.log
grep -F 'FMT_TRANSPORT_TESTS_OK=1' build/wp3_10_transport_long.log
grep -F 'long_run_hours=6 max_bpm=65535' build/wp3_10_transport_long.log

sh tests/run_wp3_6_tick_grid_tests.sh | tee build/wp3_10_tick_grid.log
grep -F 'WP3_6_TICK_GRID_TESTS_OK=1' build/wp3_10_tick_grid.log
grep -F 'tick_frames=919,1838,2757,3675,4594 next_row_frame=5513 row_boundary_wins=1 max_one_tick_per_frame=1' build/wp3_10_tick_grid.log

echo 'WP3_10_TIMING_EXIT_OK=1'
