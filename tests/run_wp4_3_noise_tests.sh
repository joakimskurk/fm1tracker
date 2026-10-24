#!/bin/sh
set -eu

mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
  tests/test_wp4_3_noise.c \
  firmware/src/fmt_engine_pitch.c \
  firmware/src/fmt_osc_noise.c \
  -o build/tests/test_wp4_3_noise

build/tests/test_wp4_3_noise

echo 'WP4_3_NOISE_FOCUSED_GATE_OK=1'
