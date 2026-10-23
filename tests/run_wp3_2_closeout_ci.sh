#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

START=dc0f4280f27e8a1a1a4608faef8d59cfceb68a1e

# WP3.2 may change only the bounded lock-storage layer, the minimal app linkage
# needed to allocate/reset it, tests/workflow, and package documentation.
git diff --exit-code "$START" -- \
    firmware/src/fmt_model.h \
    firmware/src/fmt_store.h firmware/src/fmt_store.c \
    firmware/src/fmt_transport.h firmware/src/fmt_transport.c \
    firmware/src/fmt_playback.h firmware/src/fmt_playback.c \
    firmware/src/fmt_pattern_input.h firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_step_record.h firmware/src/fmt_step_record.c \
    firmware/src/fmt_tracker_screen.h firmware/src/fmt_tracker_screen.c \
    firmware/app.ld firmware/hal firmware/loader tools build.sh
echo "WP3_2_PROTECTED_PRODUCTION_PATHS_UNCHANGED=1"

# D028 storage contract and fifth-lock branch.
sh tests/run_lock_store_tests.sh | tee build/wp3_2_lock_store_tests.log
grep -F "FMT_LOCK_STORE_TESTS_OK=1" build/wp3_2_lock_store_tests.log
grep -F "record_bytes=8 capacity=2048 storage_bytes=16384 runtime_bytes=4 owners=8192 max_per_step=4" build/wp3_2_lock_store_tests.log
grep -F "pool_exhaustion=explicit owner_check=1 sorted_by_param=1 replace_in_place=1 corruption_no_partial_mutation=1" build/wp3_2_lock_store_tests.log
grep -F "FMT_LOCK_LIMIT_TESTS_OK=1" build/wp3_2_lock_store_tests.log
grep -F "max_per_step=4 fifth_prevalidated=STEP_FULL used_unchanged=1" build/wp3_2_lock_store_tests.log

# WP3.1 registry remains authoritative.
sh tests/run_param_registry_tests.sh | tee build/wp3_2_param_registry_tests.log
grep -F "FMT_PARAM_REGISTRY_TESTS_OK=1" build/wp3_2_param_registry_tests.log

# Complete inherited Phase 2 model/editor/display regressions.
sh tests/run_model_tests.sh | tee build/wp3_2_model_tests.log
grep -F "FMT_MODEL_TESTS_OK=1" build/wp3_2_model_tests.log
sh tests/run_store_tests.sh | tee build/wp3_2_store_tests.log
grep -F "FMT_STORE_TESTS_OK=1" build/wp3_2_store_tests.log
sh tests/run_transport_tests.sh | tee build/wp3_2_transport_tests.log
grep -F "FMT_TRANSPORT_TESTS_OK=1" build/wp3_2_transport_tests.log
sh tests/run_playback_tests.sh | tee build/wp3_2_playback_tests.log
grep -F "FMT_PLAYBACK_TESTS_OK=1" build/wp3_2_playback_tests.log
sh tests/run_pattern_input_tests.sh | tee build/wp3_2_pattern_input_tests.log
grep -F "FMT_PATTERN_INPUT_TESTS_OK=1" build/wp3_2_pattern_input_tests.log
sh tests/run_step_record_tests.sh | tee build/wp3_2_step_record_tests.log
grep -F "FMT_STEP_RECORD_TESTS_OK=1" build/wp3_2_step_record_tests.log
sh tests/run_tracker_screen_tests.sh | tee build/wp3_2_tracker_screen_tests.log
grep -F "FMT_TRACKER_SCREEN_TESTS_OK=1" build/wp3_2_tracker_screen_tests.log

# Preserve complete real app behavior from Phase 2.
sh tests/run_pattern_context_host.sh | tee build/wp3_2_wp2_5_context.log
grep -F "WP2_5_PATTERN_CONTEXT_HOST_OK=1" build/wp3_2_wp2_5_context.log
sh tests/run_track_selection_context_host.sh | tee build/wp3_2_wp2_6_context.log
grep -F "WP2_6_TRACK_CONTEXT_HOST_OK=1" build/wp3_2_wp2_6_context.log
sh tests/run_step_rec_context_host.sh | tee build/wp3_2_wp2_7_context.log
grep -F "WP2_7_STEP_REC_CONTEXT_HOST_OK=1" build/wp3_2_wp2_7_context.log
sh tests/run_special_events_context_host.sh | tee build/wp3_2_wp2_8_context.log
grep -F "WP2_8_OFF_AUDIO_BEHAVIOR_OK=1" build/wp3_2_wp2_8_context.log
grep -F "WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1" build/wp3_2_wp2_8_context.log
sh tests/run_tracker_screen_host.sh | tee build/wp3_2_wp2_9_screen.log
grep -F "WP2_9_REVIEWED_SCREEN_HASHES_OK=1" build/wp3_2_wp2_9_screen.log
grep -F "WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1" build/wp3_2_wp2_9_screen.log
sh tests/run_phase2_integration_tests.sh | tee build/wp3_2_phase2_integration.log
grep -F "PHASE2_INTEGRATION_TESTS_OK=1" build/wp3_2_phase2_integration.log
sh tests/run_phase2_integration_host.sh | tee build/wp3_2_phase2_full_app.log
grep -F "WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1" build/wp3_2_phase2_full_app.log
grep -F "WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8" build/wp3_2_phase2_full_app.log

# Storage is not applied to playback in WP3.2, so default audio stays frozen.
./tests/run_host.sh
test -s build/host/fm1tracker.wav
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
HOST_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
test "$HOST_SHA" = "453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440"
test "$HOST_FRAMES" = "286464"
echo "HOST_WAV_SHA256=$HOST_SHA" | tee build/wp3_2_host_audio.txt
echo "HOST_WAV_FRAMES=$HOST_FRAMES" | tee -a build/wp3_2_host_audio.txt
echo "WP3_2_FROZEN_AUDIO_OK=1" | tee -a build/wp3_2_host_audio.txt

: "${RUNNER_TEMP:?RUNNER_TEMP is required for the pinned device build}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
test -x "$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" --version | head -n 2

# Compile the dedicated translation units with the real target compiler as an
# explicit ABI/toolchain check in addition to their inclusion in dinghy.c.
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall \
    -Ifirmware/src -c firmware/src/fmt_param_registry.c -o build/wp3_2_fmt_param_registry.o
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall \
    -Ifirmware/src -c firmware/src/fmt_lock_store.c -o build/wp3_2_fmt_lock_store.o
test -s build/wp3_2_fmt_param_registry.o
test -s build/wp3_2_fmt_lock_store.o
echo "WP3_2_JIELI_LOCK_STORE_COMPILE_OK=1"

SDK="$RUNNER_TEMP/ac79-sdk"
rm -rf "$SDK"
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
    https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git "$SDK"
ACTUAL_SDK="$(git -C "$SDK" rev-parse HEAD)"
echo "AC79_SDK_COMMIT=$ACTUAL_SDK"
test "$ACTUAL_SDK" = "d179b4484759423312073f5fbb232501aa491047"
test "$(sha256sum "$SDK/cpu/wl82/tools/uboot.boot" | awk '{print $1}')" = \
    "4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4"
test "$(sha256sum "$SDK/cpu/wl82/tools/cfg_tool.bin" | awk '{print $1}')" = \
    "276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b"
test "$(sha256sum "$SDK/cpu/wl82/tools/cfg/eq_cfg_hw.bin" | awk '{print $1}')" = \
    "41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d"
echo "AC79_SDK_INPUTS_OK=1"

JIELI_TOOLCHAIN="$RUNNER_TEMP/jieli/toolchain" \
AC79_SDK="$SDK" \
./build.sh --release 0.1 | tee build/wp3_2_device_build.log

test -s build/fm1tracker.elf
"$RUNNER_TEMP/jieli/toolchain/common/bin/objdump" -t build/fm1tracker.elf > build/wp3_2_symbols.txt

grep -E 'fmt_lock_store_storage$' build/wp3_2_symbols.txt | tee build/wp3_2_lock_storage_symbol.txt
grep -E 'fmt_lock_store_storage$' build/wp3_2_symbols.txt | grep -F '.pool'
grep -E 'fmt_lock_store_runtime$' build/wp3_2_symbols.txt | tee build/wp3_2_lock_runtime_symbol.txt
grep -E 'fmt_lock_store_runtime$' build/wp3_2_symbols.txt | grep -F '.bss'
grep -E 'fmt_store_storage$' build/wp3_2_symbols.txt | tee build/wp3_2_song_store_symbol.txt
grep -E 'fmt_store_storage$' build/wp3_2_symbols.txt | grep -F '.pool'

test -s build/fm1tracker.bin
test -s build/fm1tracker-0.1.fwsc
grep -F "register access: hal/ only (src/, loader/ clean)" build/wp3_2_device_build.log

PYTHONPATH=tools python3 - <<'PY'
from pathlib import Path
from fm1_install import product_of
raw = Path('build/fm1tracker-0.1.fwsc').read_bytes()
product = product_of(raw)
assert product == 'FM-1_987', product
print('PACKAGE_IDENTITY=' + product)
PY

python3 - <<'PY' | tee build/wp3_2_metrics.txt
import hashlib, re
from pathlib import Path

log = Path('build/wp3_2_device_build.log').read_text()
m = re.search(r"image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064", log)
assert m, 'resource line not found'
image, image_limit, ram, pool = map(int, m.groups())

# D028 exact storage/runtime allocation. Image cost is measured/frozen only
# after the first real target link because code GC/link layout determines it.
assert ram == 12468, ram
assert pool == 113000, pool

symbols = Path('build/wp3_2_symbols.txt').read_text().splitlines()
def one(name):
    lines = [line for line in symbols if line.rstrip().endswith(name)]
    assert len(lines) == 1, (name, lines)
    return lines[0].split()

lock_store = one('fmt_lock_store_storage')
lock_runtime = one('fmt_lock_store_runtime')
song_store = one('fmt_store_storage')
assert int(lock_store[-2], 16) == 16384, lock_store
assert '.pool' in lock_store, lock_store
assert int(lock_runtime[-2], 16) == 4, lock_runtime
assert '.bss' in lock_runtime, lock_runtime
assert int(song_store[-2], 16) == 65896, song_store
assert '.pool' in song_store, song_store

bin_sha = hashlib.sha256(Path('build/fm1tracker.bin').read_bytes()).hexdigest()
fwsc_sha = hashlib.sha256(Path('build/fm1tracker-0.1.fwsc').read_bytes()).hexdigest()
wav_sha = hashlib.sha256(Path('build/host/fm1tracker.wav').read_bytes()).hexdigest()
assert wav_sha == '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440', wav_sha

base_image, base_ram, base_pool = 59252, 12464, 96616
print('WP3_2_METRICS')
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print(f'image_delta_vs_wp3_1={image-base_image}')
print(f'normal_ram_delta_vs_wp3_1={ram-base_ram}')
print(f'pool_delta_vs_wp3_1={pool-base_pool}')
print(f'image_remaining_bytes={image_limit-image}')
print(f'normal_ram_remaining_bytes={98304-ram}')
print(f'pool_remaining_bytes={344064-pool}')
print(f'fm1tracker_bin_sha256={bin_sha}')
print(f'fm1tracker_fwsc_sha256={fwsc_sha}')
print(f'host_wav_sha256={wav_sha}')
PY

grep -F 'normal_ram_delta_vs_wp3_1=4' build/wp3_2_metrics.txt
grep -F 'pool_delta_vs_wp3_1=16384' build/wp3_2_metrics.txt

echo "WP3_2_MEASURED_CLOSEOUT_GATE_OK=1"
