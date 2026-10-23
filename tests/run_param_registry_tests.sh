#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host

cc -std=c99 -O2 -Wall -Wextra -Werror \
    -Ifirmware/src \
    tests/test_fmt_param_registry.c \
    firmware/src/fmt_param_registry.c \
    -o build/host/test_fmt_param_registry

build/host/test_fmt_param_registry
