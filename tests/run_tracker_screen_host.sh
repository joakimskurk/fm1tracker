#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp2_9 firmware/src/dinghy.c

build/host/fm1tracker_wp2_9 \
    build/host/fm1tracker_wp2_9_tracker \
    tests/wp2_9_tracker_screen.script \
    > build/wp2_9_tracker_screen_host.log

build/host/fm1tracker_wp2_9 \
    build/host/fm1tracker_wp2_9_home_return \
    tests/wp2_9_home_return.script \
    > build/wp2_9_home_return_host.log

build/host/fm1tracker_wp2_9 \
    build/host/fm1tracker_wp2_9_home_control \
    tests/wp2_9_home_control.script \
    > build/wp2_9_home_control_host.log

# Returning from SEQ must clear every tracker pixel and restore the same HOME
# surface as a run that never left HOME.
cmp build/host/fm1tracker_wp2_9_home_return-3000.ppm \
    build/host/fm1tracker_wp2_9_home_control-3000.ppm

echo "WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1"

# These raw PPM hashes were visually reviewed from run 40009374414. Freeze the
# exact framebuffer output rather than the PNG encoder output.
seq_sha="$(sha256sum build/host/fm1tracker_wp2_9_tracker-1000.ppm | awk '{print $1}')"
rec_sha="$(sha256sum build/host/fm1tracker_wp2_9_tracker-3000.ppm | awk '{print $1}')"
test "$seq_sha" = "e67d0cf9aa2fa391464237a40ed795bb6ba881e15aff40340e1b92a4a8eccb7a"
test "$rec_sha" = "4b0192b084e6ee483d7a8f98add2ae95229dcc914583d3cbe957944824cb6424"
echo "WP2_9_SEQ_SCREEN_SHA256=$seq_sha"
echo "WP2_9_STEP_REC_SCREEN_SHA256=$rec_sha"
echo "WP2_9_REVIEWED_SCREEN_HASHES_OK=1"

python3 - <<'PY'
from pathlib import Path
from PIL import Image
import hashlib

paths = [
    Path('build/host/fm1tracker_wp2_9_tracker-1000.ppm'),
    Path('build/host/fm1tracker_wp2_9_tracker-3000.ppm'),
    Path('build/host/fm1tracker_wp2_9_home_return-3000.ppm'),
]
for p in paths:
    assert p.is_file() and p.stat().st_size > 0, p
    png = p.with_suffix('.png')
    Image.open(p).save(png)
    print(f'{p.name}_sha256={hashlib.sha256(p.read_bytes()).hexdigest()}')
    print(f'{png.name}_sha256={hashlib.sha256(png.read_bytes()).hexdigest()}')
PY

test -s build/host/fm1tracker_wp2_9_tracker-1000.png
test -s build/host/fm1tracker_wp2_9_tracker-3000.png
test -s build/host/fm1tracker_wp2_9_home_return-3000.png

echo "WP2_9_TRACKER_SCREEN_HOST_OK=1"
