#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_4 firmware/src/dinghy.c

build/host/fm1tracker_wp3_4 \
    build/host/fm1tracker_wp3_4_lock_delete \
    tests/wp3_4_lock_delete_context.script \
    > build/wp3_4_lock_delete_context_host.log

# Ordinary SEQ editing/browse/delete must never audition note keys.
midi_count="$(grep -c 'MIDI out' build/wp3_4_lock_delete_context_host.log || true)"
test "$midi_count" -eq 0

test -s build/host/fm1tracker_wp3_4_lock_delete-1000.ppm
test -s build/host/fm1tracker_wp3_4_lock_delete-3000.ppm

sha_1000="$(sha256sum build/host/fm1tracker_wp3_4_lock_delete-1000.ppm | awk '{print $1}')"
sha_3000="$(sha256sum build/host/fm1tracker_wp3_4_lock_delete-3000.ppm | awk '{print $1}')"

echo "WP3_4_LOCK_DELETE_1000_SHA256=$sha_1000"
echo "WP3_4_LOCK_DELETE_3000_SHA256=$sha_3000"

test "$sha_1000" = "6ce56a09073b4d70ec5ff7001ae787966f17d6bf610f1db2e239df152e14205d"
test "$sha_3000" = "f0185637136d5b609ebae1adbfbba7a62989a9246cb63093e6263242040e6cc6"
echo "WP3_4_REVIEWED_LOCK_DELETE_SCREENS_OK=1"
echo "WP3_4_LOCK_DELETE_CONTEXT_HOST_OK=1 midi_events=0 same_poll_confirm=fixture play_cancel=fixture unrelated_key_cancel=fixture valid_delete=fixture"

# CI installs Pillow for reviewable images. Local execution remains useful when
# Pillow is absent.
python3 - <<'PY' || true
from pathlib import Path
try:
    from PIL import Image
except Exception:
    raise SystemExit(0)
for stem in ('1000', '3000'):
    src = Path(f'build/host/fm1tracker_wp3_4_lock_delete-{stem}.ppm')
    dst = src.with_suffix('.png')
    Image.open(src).save(dst)
PY

if test -s build/host/fm1tracker_wp3_4_lock_delete-1000.png && \
   test -s build/host/fm1tracker_wp3_4_lock_delete-3000.png; then
    png_1000="$(sha256sum build/host/fm1tracker_wp3_4_lock_delete-1000.png | awk '{print $1}')"
    png_3000="$(sha256sum build/host/fm1tracker_wp3_4_lock_delete-3000.png | awk '{print $1}')"
    echo "WP3_4_LOCK_DELETE_1000_PNG_SHA256=$png_1000"
    echo "WP3_4_LOCK_DELETE_3000_PNG_SHA256=$png_3000"
    test "$png_1000" = "9ef8b51b7666e362f32f6d9f5abd57392c778198f09bc06e99ba8ca9067d091e"
    test "$png_3000" = "85de320da9fdbd2069c6b59d18a0173da9674877f7c00f3e90d2daf3388b6032"
fi
