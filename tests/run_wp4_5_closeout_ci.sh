#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/tests build/wp4_5_target

PRE_SOURCE=64c9603795668b002cf2075bfaa72305384eb0fa

# WP4.5 may add exactly two isolated WAVETABLE-8 production files.
actual="$(git diff --name-only "$PRE_SOURCE" -- firmware/src | sort)"
expected="$(printf '%s\n' \
    firmware/src/fmt_osc_wavetable8.c \
    firmware/src/fmt_osc_wavetable8.h | sort)"
test "$actual" = "$expected"
git diff --exit-code "$PRE_SOURCE" -- firmware/src/dinghy.c
git diff --exit-code "$PRE_SOURCE" -- \
    firmware/src/fmt_engine_pitch.c firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_osc_pulse.c firmware/src/fmt_osc_pulse.h \
    firmware/src/fmt_osc_noise.c firmware/src/fmt_osc_noise.h \
    firmware/src/fmt_osc_triangle.c firmware/src/fmt_osc_triangle.h
echo 'WP4_5_ISOLATED_PRODUCTION_DELTA_OK=1 files=2 linked_delta=0 prior_engine_delta=0'

# Focused D041 all-table, blob-SHA and deterministic signature proof.
sh tests/run_wp4_5_wavetable8_tests.sh | tee build/wp4_5_focused_closeout.log
grep -F 'WP4_5_WAVETABLE8_FOCUSED_GATE_OK=1' build/wp4_5_focused_closeout.log
grep -F 'all_table_samples=256' build/wp4_5_focused_closeout.log
grep -F 'WP4_5_TABLE_DATA_SHA_OK=1 bytes=256 sha256=f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee' build/wp4_5_focused_closeout.log
grep -F 'long_hashes=40a7ddab,7c509a67,a573d449,1d6f9fcb,b8048a69,a3661307,27204689,48f2989f' build/wp4_5_focused_closeout.log

files='firmware/src/fmt_osc_wavetable8.h firmware/src/fmt_osc_wavetable8.c'
! grep -nE '\b(float|double|malloc|calloc|realloc|free)\b' $files
! grep -nE '\b(uint64_t|int64_t|long[[:space:]]+long)\b' $files
! grep -nE '\b(for|while|do)[[:space:]]*\(' firmware/src/fmt_osc_wavetable8.c
! grep -nE 'Fmt(Song|Step|Lock|Tracker|RtQueue)|fmt_lock_|fmt_store_' firmware/src/fmt_osc_wavetable8.c
grep -F 'index = osc->phase >> FMT_WAVETABLE8_INDEX_SHIFT;' firmware/src/fmt_osc_wavetable8.c
grep -F '* FMT_WAVETABLE8_Q15_SCALE;' firmware/src/fmt_osc_wavetable8.c
grep -F 'osc->phase += phase_increment;' firmware/src/fmt_osc_wavetable8.c
echo 'WP4_5_STATIC_AUDIT_OK=1 wavetable_loops=0 alloc=0 float=0 int64=0 store_traversal=0 immutable_table_bytes=256'

# Running host app must remain exact baseline because WAVETABLE-8 is unlinked.
./tests/run_host.sh
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
HOST_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
test "$HOST_SHA" = '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440'
test "$HOST_FRAMES" = '286464'
echo "WP4_5_FULL_APP_HOST_BASELINE_OK=1 sha256=$HOST_SHA frames=$HOST_FRAMES"

: "${RUNNER_TEMP:?RUNNER_TEMP is required for pinned target closeout}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
CC="$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
test -x "$CC"
"$CC" --version | head -n 2

"$CC" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall -Werror \
    -Ifirmware/src -c firmware/src/fmt_engine_pitch.c \
    -o build/wp4_5_target/fmt_engine_pitch.o
"$CC" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall -Werror \
    -Ifirmware/src -c firmware/src/fmt_osc_wavetable8.c \
    -o build/wp4_5_target/fmt_osc_wavetable8.o
PITCH_OBJ_SHA="$(sha256sum build/wp4_5_target/fmt_engine_pitch.o | awk '{print $1}')"
WAVETABLE_OBJ_SHA="$(sha256sum build/wp4_5_target/fmt_osc_wavetable8.o | awk '{print $1}')"
test "$PITCH_OBJ_SHA" = '6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20'
test "$WAVETABLE_OBJ_SHA" = '4924fc7ff9069d51ab7cf8259af109b32da16758d25504e22b183af7e29c1194'
echo "WP4_5_TARGET_OBJECTS_OK=1 pitch_sha=$PITCH_OBJ_SHA wavetable8_sha=$WAVETABLE_OBJ_SHA"

SDK="$RUNNER_TEMP/ac79-sdk"
rm -rf "$SDK"
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
    https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git "$SDK"
ACTUAL_SDK="$(git -C "$SDK" rev-parse HEAD)"
test "$ACTUAL_SDK" = 'd179b4484759423312073f5fbb232501aa491047'
test "$(sha256sum "$SDK/cpu/wl82/tools/uboot.boot" | awk '{print $1}')" = \
    '4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4'
test "$(sha256sum "$SDK/cpu/wl82/tools/cfg_tool.bin" | awk '{print $1}')" = \
    '276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b'
test "$(sha256sum "$SDK/cpu/wl82/tools/cfg/eq_cfg_hw.bin" | awk '{print $1}')" = \
    '41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d'
echo 'WP4_5_AC79_INPUTS_OK=1'

JIELI_TOOLCHAIN="$RUNNER_TEMP/jieli/toolchain" \
AC79_SDK="$SDK" \
./build.sh --release 0.1 | tee build/wp4_5_device_build.log

python3 - <<'PY' | tee build/wp4_5_device_metrics.txt
import hashlib, re
from pathlib import Path
log = Path('build/wp4_5_device_build.log').read_text()
m = re.search(r"image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064", log)
assert m, 'resource line not found'
image, image_limit, ram, pool = map(int, m.groups())
assert image == 66960, image
assert ram == 13296, ram
assert pool == 113000, pool

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()
bin_sha = sha('build/fm1tracker.bin')
fwsc_sha = sha('build/fm1tracker-0.1.fwsc')
assert bin_sha == 'b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27', bin_sha
assert fwsc_sha == '7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c', fwsc_sha
print('WP4_5_DEVICE_BASELINE')
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print(f'image_delta_vs_wp4_4={image-66960}')
print(f'normal_ram_delta_vs_wp4_4={ram-13296}')
print(f'pool_delta_vs_wp4_4={pool-113000}')
print(f'fm1tracker_bin_sha256={bin_sha}')
print(f'fm1tracker_fwsc_sha256={fwsc_sha}')
PY

grep -F 'image_delta_vs_wp4_4=0' build/wp4_5_device_metrics.txt
grep -F 'normal_ram_delta_vs_wp4_4=0' build/wp4_5_device_metrics.txt
grep -F 'pool_delta_vs_wp4_4=0' build/wp4_5_device_metrics.txt

PYTHONPATH=tools python3 - <<'PY'
from pathlib import Path
from fm1_install import product_of
product = product_of(Path('build/fm1tracker-0.1.fwsc').read_bytes())
assert product == 'FM-1_987', product
print('WP4_5_PACKAGE_IDENTITY=' + product)
PY

printf '%s\n' \
    'WP4_5_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 full_app_delta=0' \
    "WP4_5_FINAL_HOST_WAV_SHA256=$HOST_SHA" \
    'WP4_5_FINAL_BIN_SHA256=b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27' \
    'WP4_5_FINAL_FWSC_SHA256=7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c' \
    "WP4_5_FINAL_PITCH_OBJECT_SHA256=$PITCH_OBJ_SHA" \
    "WP4_5_FINAL_WAVETABLE8_OBJECT_SHA256=$WAVETABLE_OBJ_SHA" \
    'WP4_5_TABLE_BLOB_SHA256=f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee' \
    'WP4_5_D041_EXIT_OK=1 wavetable8=isolated deterministic=1 tables=8 samples=256 pi32v2=1 full_app_identical=1 physical_hardware_pending=1' \
    'WP4_5_CLOSEOUT_OK=1' \
    | tee build/wp4_5_final_summary.txt
