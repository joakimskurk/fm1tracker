#!/bin/sh
set -eu
mkdir -p build
cc -std=c11 -O2 -Wall -Wextra -Werror \
  -Ifirmware/src \
  tests/test_wp4_6_envelope.c \
  firmware/src/fmt_envelope.c \
  -o build/wp4_6_envelope_tests
./build/wp4_6_envelope_tests | tee build/wp4_6_envelope_tests.log
grep -F 'WP4_6_ENVELOPE_TESTS_OK=1' build/wp4_6_envelope_tests.log
grep -F 'long_hash=40a44a2e' build/wp4_6_envelope_tests.log
echo 'WP4_6_ENVELOPE_FOCUSED_GATE_OK=1'
