#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build

cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_9 firmware/src/dinghy.c

build/host/fm1tracker_wp3_9 \
    build/host/fm1tracker_wp3_9_visual \
    tests/wp3_9_visual_feedback.script \
    > build/wp3_9_visual_host.log

# D035 must still preserve the historical HOME cleanup behavior. Use the same
# old transition/control fixtures, but do not freeze old tracker hashes because
# WP3.9 intentionally changes tracker pixels.
build/host/fm1tracker_wp3_9 \
    build/host/fm1tracker_wp3_9_home_return \
    tests/wp2_9_home_return.script \
    > build/wp3_9_home_return_host.log

build/host/fm1tracker_wp3_9 \
    build/host/fm1tracker_wp3_9_home_control \
    tests/wp2_9_home_control.script \
    > build/wp3_9_home_control_host.log

cmp build/host/fm1tracker_wp3_9_home_return-3000.ppm \
    build/host/fm1tracker_wp3_9_home_control-3000.ppm

echo "WP3_9_HOME_RETURN_PIXEL_IDENTICAL=1"

python3 - <<'PY'
from pathlib import Path
from PIL import Image
import hashlib

paths = [
    Path('build/host/fm1tracker_wp3_9_visual-1000.ppm'),
    Path('build/host/fm1tracker_wp3_9_visual-3000.ppm'),
    Path('build/host/fm1tracker_wp3_9_home_return-3000.ppm'),
]
for p in paths:
    assert p.is_file() and p.stat().st_size > 0, p
    png = p.with_suffix('.png')
    Image.open(p).save(png)
    print(f'{p.name}_sha256={hashlib.sha256(p.read_bytes()).hexdigest()}')
    print(f'{png.name}_sha256={hashlib.sha256(png.read_bytes()).hexdigest()}')
PY

active_sha="$(sha256sum build/host/fm1tracker_wp3_9_visual-1000.ppm | awk '{print $1}')"
released_sha="$(sha256sum build/host/fm1tracker_wp3_9_visual-3000.ppm | awk '{print $1}')"
test "$active_sha" = "d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3"
test "$released_sha" = "ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6"
echo "WP3_9_ACTIVE_SCREEN_SHA256=$active_sha"
echo "WP3_9_RELEASED_SCREEN_SHA256=$released_sha"
echo "WP3_9_REVIEWED_SCREEN_HASHES_OK=1"

test -s build/host/fm1tracker_wp3_9_visual-1000.png
test -s build/host/fm1tracker_wp3_9_visual-3000.png

echo "WP3_9_VISUAL_HOST_OK=1"
