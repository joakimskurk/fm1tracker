#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Run the unchanged Dinghy baseline app as FM1Tracker on this computer (tests/host.c):
# build/host/fm1tracker.wav and the screens as PNG.
set -e
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"${APP_VER:-dev}\"" -Ifirmware/src -Ifirmware/gen -o build/host/fm1tracker firmware/src/dinghy.c
build/host/fm1tracker build/host/fm1tracker
python3 - <<'PY'
from pathlib import Path
from PIL import Image
for p in sorted(Path("build/host").glob("fm1tracker-*.ppm")):
    Image.open(p).resize((480, 480), Image.NEAREST).save(p.with_suffix(".png"))
    p.unlink()
    print("screen ->", p.with_suffix(".png"))
PY
