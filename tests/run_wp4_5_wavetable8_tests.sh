#!/bin/sh
set -eu

mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
  tests/test_wp4_5_wavetable8.c \
  firmware/src/fmt_engine_pitch.c \
  firmware/src/fmt_osc_wavetable8.c \
  -o build/tests/test_wp4_5_wavetable8

build/tests/test_wp4_5_wavetable8
python3 tests/verify_wp4_5_table_data.py

echo 'WP4_5_WAVETABLE8_FOCUSED_GATE_OK=1'
