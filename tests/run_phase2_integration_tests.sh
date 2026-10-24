#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_phase2_integration.c \
    firmware/src/fmt_store.c \
    firmware/src/fmt_param_registry.c \
    firmware/src/fmt_lock_store.c \
    firmware/src/fmt_lock_editor.c \
    firmware/src/fmt_transport.c \
    firmware/src/fmt_playback.c \
    firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_step_record.c \
    firmware/src/fmt_tracker_screen.c \
    -o build/tests/test_phase2_integration

./build/tests/test_phase2_integration
