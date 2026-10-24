#!/bin/sh
set -eu

mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
  tests/test_wp4_4_triangle.c \
  firmware/src/fmt_engine_pitch.c \
  firmware/src/fmt_osc_triangle.c \
  -o build/tests/test_wp4_4_triangle

build/tests/test_wp4_4_triangle

echo 'WP4_4_TRIANGLE_FOCUSED_GATE_OK=1'
