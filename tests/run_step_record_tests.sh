#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests
cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_step_record.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_transport.c \
    firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_step_record.c \
    -o build/tests/test_fmt_step_record
./build/tests/test_fmt_step_record
