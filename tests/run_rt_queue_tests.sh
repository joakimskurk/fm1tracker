#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests
cc -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_rt_queue.c firmware/src/fmt_rt_queue.c \
    -o build/tests/test_fmt_rt_queue
build/tests/test_fmt_rt_queue
