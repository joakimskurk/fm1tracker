#!/bin/sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cd "$ROOT"
mkdir -p build

${CC:-cc} -std=c11 -O2 -Wall -Wextra -Werror \
  -Ifirmware/src \
  tests/test_wp4_7_allocator.c firmware/src/fmt_voice_allocator.c \
  -o build/test_wp4_7_allocator

./build/test_wp4_7_allocator | tee build/wp4_7_allocator_test.log

grep -F 'WP4_7_ALLOCATOR_TESTS_OK=1' build/wp4_7_allocator_test.log >/dev/null
grep -F 'allocator_bytes=52 result_bytes=4 slots=8 masks=256 active_cross_track_steal=0 release_tail_steal=1' build/wp4_7_allocator_test.log >/dev/null
grep -F 'long_hash=c1138b39 long_ops=4096 final_clock=2064' build/wp4_7_allocator_test.log >/dev/null

echo 'WP4_7_ALLOCATOR_FOCUSED_GATE_OK=1'
