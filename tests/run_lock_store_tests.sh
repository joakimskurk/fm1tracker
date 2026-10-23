#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_lock_store.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_param_registry.c \
    -o build/tests/test_fmt_lock_store
./build/tests/test_fmt_lock_store

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_lock_limit.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_lock_store.c \
    -o build/tests/test_fmt_lock_limit
./build/tests/test_fmt_lock_limit
