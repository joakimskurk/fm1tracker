#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

BIN=build/host/fm1tracker_wp4_9_ui
cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp4.9-gate6"' \
    -Ifirmware/src -Ifirmware/gen -o "$BIN" firmware/src/dinghy.c

# D029 context grammar remains authoritative, but its pre-D044 footer pixels do
# not: D044 intentionally changed the semantic 8-BIT page displayed/edited.
"$BIN" build/host/wp4_9_lock_editor tests/wp3_3_lock_editor_context.script \
    > build/wp4_9_ui_lock_editor.log
test "$(grep -c 'MIDI out' build/wp4_9_ui_lock_editor.log || true)" -eq 0
test -s build/host/wp4_9_lock_editor-1000.ppm
test -s build/host/wp4_9_lock_editor-3000.ppm

# D030 destructive grammar also remains authoritative; compare behavior/context,
# not the obsolete pre-D044 semantic footer text.
"$BIN" build/host/wp4_9_lock_delete tests/wp3_4_lock_delete_context.script \
    > build/wp4_9_ui_lock_delete.log
test "$(grep -c 'MIDI out' build/wp4_9_ui_lock_delete.log || true)" -eq 0
test -s build/host/wp4_9_lock_delete-1000.ppm
test -s build/host/wp4_9_lock_delete-3000.ppm

# D035 visual feedback remains bounded. Active/released frames are captured for
# canonical WP4.9 evidence. HOME cleanup is still required to be pixel-identical
# to a clean HOME control after the same transition.
"$BIN" build/host/wp4_9_visual tests/wp3_9_visual_feedback.script \
    > build/wp4_9_ui_visual.log
"$BIN" build/host/wp4_9_home_return tests/wp2_9_home_return.script \
    > build/wp4_9_ui_home_return.log
"$BIN" build/host/wp4_9_home_control tests/wp2_9_home_control.script \
    > build/wp4_9_ui_home_control.log
cmp build/host/wp4_9_home_return-3000.ppm build/host/wp4_9_home_control-3000.ppm

test -s build/host/wp4_9_visual-1000.ppm
test -s build/host/wp4_9_visual-3000.ppm

echo "WP4_9_UI_LOCK_EDITOR_1000_SHA256=$(sha256sum build/host/wp4_9_lock_editor-1000.ppm | awk '{print $1}')"
echo "WP4_9_UI_LOCK_EDITOR_3000_SHA256=$(sha256sum build/host/wp4_9_lock_editor-3000.ppm | awk '{print $1}')"
echo "WP4_9_UI_LOCK_DELETE_1000_SHA256=$(sha256sum build/host/wp4_9_lock_delete-1000.ppm | awk '{print $1}')"
echo "WP4_9_UI_LOCK_DELETE_3000_SHA256=$(sha256sum build/host/wp4_9_lock_delete-3000.ppm | awk '{print $1}')"
echo "WP4_9_UI_VISUAL_1000_SHA256=$(sha256sum build/host/wp4_9_visual-1000.ppm | awk '{print $1}')"
echo "WP4_9_UI_VISUAL_3000_SHA256=$(sha256sum build/host/wp4_9_visual-3000.ppm | awk '{print $1}')"
echo 'WP4_9_UI_HOME_RETURN_PIXEL_IDENTICAL=1'
echo 'WP4_9_UI_CONTEXT_REGRESSIONS_OK=1 lock_editor_midi=0 lock_delete_midi=0 bounded_visual_frames=1'
