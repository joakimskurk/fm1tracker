#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_pattern_input.c \
    firmware/src/fmt_store.c firmware/src/fmt_pattern_input.c \
    -o build/tests/test_fmt_pattern_input
./build/tests/test_fmt_pattern_input
