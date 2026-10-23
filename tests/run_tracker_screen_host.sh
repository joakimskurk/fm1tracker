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
