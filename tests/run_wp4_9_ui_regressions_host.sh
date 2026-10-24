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

lock_editor_1000="$(sha256sum build/host/wp4_9_lock_editor-1000.ppm | awk '{print $1}')"
lock_editor_3000="$(sha256sum build/host/wp4_9_lock_editor-3000.ppm | awk '{print $1}')"
lock_delete_1000="$(sha256sum build/host/wp4_9_lock_delete-1000.ppm | awk '{print $1}')"
lock_delete_3000="$(sha256sum build/host/wp4_9_lock_delete-3000.ppm | awk '{print $1}')"
visual_1000="$(sha256sum build/host/wp4_9_visual-1000.ppm | awk '{print $1}')"
visual_3000="$(sha256sum build/host/wp4_9_visual-3000.ppm | awk '{print $1}')"

test "$lock_editor_1000" = 'c9972b1edac7a0c96203ce6a7b4b668c5a8aafdc21c0c45e9f6cec61d1a4feda'
test "$lock_editor_3000" = '16641a96750ecdfa40aa897467643ccdea5cfc666a6a01b4e176663e953765ff'
test "$lock_delete_1000" = '7daeebe59622467fc49bbecf6607db24649996e90c2e531bab06d2ea0644c150'
test "$lock_delete_3000" = 'd66a634d1e8a185c22ddb1605b05d1a162e89f71b85b1e0813bffb2cc17d14cf'
test "$visual_1000" = 'dab7d3f60b23289dd0fc39a8071d13c301b2108164b96fc54681a8b1558b8441'
test "$visual_3000" = 'ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6'

echo "WP4_9_UI_LOCK_EDITOR_1000_SHA256=$lock_editor_1000"
echo "WP4_9_UI_LOCK_EDITOR_3000_SHA256=$lock_editor_3000"
echo "WP4_9_UI_LOCK_DELETE_1000_SHA256=$lock_delete_1000"
echo "WP4_9_UI_LOCK_DELETE_3000_SHA256=$lock_delete_3000"
echo "WP4_9_UI_VISUAL_1000_SHA256=$visual_1000"
echo "WP4_9_UI_VISUAL_3000_SHA256=$visual_3000"
echo 'WP4_9_UI_HOME_RETURN_PIXEL_IDENTICAL=1'
echo 'WP4_9_UI_CONTEXT_REGRESSIONS_OK=1 lock_editor_midi=0 lock_delete_midi=0 bounded_visual_frames=1'
echo 'WP4_9_UI_CANONICAL_HASHES_OK=1'
