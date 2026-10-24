#!/bin/sh
set -eu

mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
  tests/test_wp4_7_voice_allocator.c \
  firmware/src/fmt_voice_allocator.c \
  -o build/tests/test_wp4_7_voice_allocator

build/tests/test_wp4_7_voice_allocator | tee build/tests/wp4_7_allocator.log
grep -F 'WP4_7_ALLOCATOR_TESTS_OK=1' build/tests/wp4_7_allocator.log
grep -F 'long_hash_fnv1a32=c6baff38 events=4096 final_clock=2658' build/tests/wp4_7_allocator.log

echo 'WP4_7_ALLOCATOR_FOCUSED_GATE_OK=1'
