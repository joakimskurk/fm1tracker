#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_3 firmware/src/dinghy.c

build/host/fm1tracker_wp3_3 \
    build/host/fm1tracker_wp3_3_lock_editor \
    tests/wp3_3_lock_editor_context.script \
    > build/wp3_3_lock_editor_context_host.log

# Ordinary SEQ white/black keys are editor controls. The entire fixture must
# therefore emit no audition MIDI even though note keys are physically held.
midi_count="$(grep -c 'MIDI out' build/wp3_3_lock_editor_context_host.log || true)"
test "$midi_count" -eq 0

test -s build/host/fm1tracker_wp3_3_lock_editor-1000.ppm
test -s build/host/fm1tracker_wp3_3_lock_editor-3000.ppm

sha_1000="$(sha256sum build/host/fm1tracker_wp3_3_lock_editor-1000.ppm | awk '{print $1}')"
sha_3000="$(sha256sum build/host/fm1tracker_wp3_3_lock_editor-3000.ppm | awk '{print $1}')"

echo "WP3_3_LOCK_EDITOR_1000_SHA256=$sha_1000"
echo "WP3_3_LOCK_EDITOR_3000_SHA256=$sha_3000"
echo "WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1 midi_events=0 stale_begin_release_deltas=fixture focus_suppression=fixture"

# CI installs Pillow so reviewed images can be uploaded. Keep local execution
# useful even when Pillow is absent.
python3 - <<'PY' || true
from pathlib import Path
try:
    from PIL import Image
except Exception:
    raise SystemExit(0)
for stem in ('1000', '3000'):
    src = Path(f'build/host/fm1tracker_wp3_3_lock_editor-{stem}.ppm')
    dst = src.with_suffix('.png')
    Image.open(src).save(dst)
    print(f'{dst}_sha256=')
PY
