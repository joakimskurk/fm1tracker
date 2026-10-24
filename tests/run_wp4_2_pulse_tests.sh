#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp4_2_pulse.c \
    firmware/src/fmt_engine_pitch.c \
    firmware/src/fmt_osc_pulse.c \
    -o build/tests/test_wp4_2_pulse

./build/tests/test_wp4_2_pulse | tee build/wp4_2_pulse_tests.log

grep -F 'WP4_2_PULSE_TESTS_OK=1' build/wp4_2_pulse_tests.log
grep -F 'state_bytes=4 width=32..224 phase_order=sample_before_advance hard_edge=1' build/wp4_2_pulse_tests.log
grep -F 'pitch_q7=0..16383 strict_monotonic=1 max_inc=1293741834' build/wp4_2_pulse_tests.log
grep -F 'long_hash_fnv1a32=8ab03ee1 samples=65536 pitch_q7=8832 width=96 final_phase=dfb90000' build/wp4_2_pulse_tests.log

echo 'WP4_2_PULSE_FOCUSED_GATE_OK=1'
