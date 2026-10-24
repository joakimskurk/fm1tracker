#!/bin/sh
set -eu

mkdir -p build/wp4_8_params

CC="${CC:-cc}"
"$CC" -std=c11 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_wp4_8_8bit_params.c \
    firmware/src/fmt_8bit_params.c \
    firmware/src/fmt_param_registry.c \
    -o build/wp4_8_params/test_wp4_8_8bit_params

build/wp4_8_params/test_wp4_8_8bit_params | tee build/wp4_8_params/host_summary.txt

grep -F 'WP4_8_D044_TESTS_OK=1' build/wp4_8_params/host_summary.txt >/dev/null
grep -F 'WP4_8_REPLAY_FNV1A32=635311c5' build/wp4_8_params/host_summary.txt >/dev/null

echo 'WP4_8_D044_FOCUSED_GATE_OK=1'
