#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp3_6_tick_grid.c \
    firmware/src/fmt_tracker_effect.c \
    firmware/src/fmt_transport.c \
    firmware/src/fmt_store.c \
    -o build/tests/test_wp3_6_tick_grid
./build/tests/test_wp3_6_tick_grid
