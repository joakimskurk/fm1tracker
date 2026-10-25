#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME="\"FM1TRACKER\"" -DAPP_VER="\"wp5.11\"" \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp5_11 firmware/src/dinghy.c

build/host/fm1tracker_wp5_11 \
    build/host/fm1tracker_wp5_11 \
    tests/wp5_11_runtime_editor.script \
    > build/wp5_11_runtime_editor.log

grep -F "400 ms  WP5_11_EDITOR active=1 track=1 instrument=4 family=1 engine=0 page=3 k1=10" \
    build/wp5_11_runtime_editor.log
grep -F "550 ms  WP5_11_EDITOR active=0 track=1 instrument=4 family=1 engine=0 page=3 k1=10" \
    build/wp5_11_runtime_editor.log

echo "WP5_11_RUNTIME_EDITOR_HOST_OK=1"
