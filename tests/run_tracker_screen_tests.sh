#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/tests

COMMON_SRC="firmware/src/fmt_store.c firmware/src/fmt_lock_store.c firmware/src/fmt_param_registry.c firmware/src/fmt_lock_editor.c firmware/src/fmt_transport.c firmware/src/fmt_pattern_input.c firmware/src/fmt_step_record.c firmware/src/fmt_tracker_screen.c"

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_fmt_tracker_screen.c $COMMON_SRC \
    -o build/tests/test_fmt_tracker_screen
./build/tests/test_fmt_tracker_screen

cc -std=c11 -O2 -Wall -Wextra -Werror -Ifirmware/src \
    tests/test_wp3_9_visual.c $COMMON_SRC \
    -o build/tests/test_wp3_9_visual
./build/tests/test_wp3_9_visual
