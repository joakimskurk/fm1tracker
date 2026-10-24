#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

START=8ffa2a3de00f89946aca37fa7015fd3940851c93

# WP3.4 may extend only the D029 editor/input UX plus tests/docs/workflow.
# Phase 2 core, D027 registry, D028 storage, platform/HAL/loader and build
# infrastructure remain protected byte-for-byte from the WP3.4 start head.
git diff --exit-code "$START" -- \
    firmware/src/fmt_model.h \
    firmware/src/fmt_store.h firmware/src/fmt_store.c \
    firmware/src/fmt_param_registry.h firmware/src/fmt_param_registry.c \
    firmware/src/fmt_lock_store.h firmware/src/fmt_lock_store.c \
    firmware/src/fmt_transport.h firmware/src/fmt_transport.c \
    firmware/src/fmt_playback.h firmware/src/fmt_playback.c \
    firmware/src/fmt_pattern_input.h firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_step_record.h firmware/src/fmt_step_record.c \
    firmware/src/fmt_tracker_screen.h firmware/src/fmt_tracker_screen.c \
    firmware/src/platform.c firmware/app.ld firmware/hal firmware/loader tools build.sh
echo "WP3_4_PROTECTED_PRODUCTION_PATHS_UNCHANGED=1"

# D030 editor contract.
sh tests/run_lock_editor_tests.sh | tee build/wp3_4_lock_editor_tests.log
grep -F "FMT_LOCK_EDITOR_TESTS_OK=1" build/wp3_4_lock_editor_tests.log
grep -F "state_bytes=10 pattern=0 knobs=4 browse=select_sorted_clamped delete=edit_arm_then_sel_confirm clear_all=0" build/wp3_4_lock_editor_tests.log
grep -F "targets=64x8 snapshot=1 replace_in_place=1 delete_first_middle_last_only=1 free_recovery=1 neighbor_isolation=1 range_delete=1" build/wp3_4_lock_editor_tests.log

# D027/D028 remain authoritative and unchanged.
sh tests/run_param_registry_tests.sh | tee build/wp3_4_param_registry_tests.log
grep -F "FMT_PARAM_REGISTRY_TESTS_OK=1" build/wp3_4_param_registry_tests.log
sh tests/run_lock_store_tests.sh | tee build/wp3_4_lock_store_tests.log
grep -F "FMT_LOCK_STORE_TESTS_OK=1" build/wp3_4_lock_store_tests.log
grep -F "record_bytes=8 capacity=2048 storage_bytes=16384 runtime_bytes=4 owners=8192 max_per_step=4" build/wp3_4_lock_store_tests.log
grep -F "FMT_LOCK_LIMIT_TESTS_OK=1" build/wp3_4_lock_store_tests.log

# Complete inherited Phase 2 model/editor/display regressions.
sh tests/run_model_tests.sh | tee build/wp3_4_model_tests.log
grep -F "FMT_MODEL_TESTS_OK=1" build/wp3_4_model_tests.log
sh tests/run_store_tests.sh | tee build/wp3_4_store_tests.log
grep -F "FMT_STORE_TESTS_OK=1" build/wp3_4_store_tests.log
sh tests/run_transport_tests.sh | tee build/wp3_4_transport_tests.log
grep -F "FMT_TRANSPORT_TESTS_OK=1" build/wp3_4_transport_tests.log
sh tests/run_playback_tests.sh | tee build/wp3_4_playback_tests.log
grep -F "FMT_PLAYBACK_TESTS_OK=1" build/wp3_4_playback_tests.log
sh tests/run_pattern_input_tests.sh | tee build/wp3_4_pattern_input_tests.log
grep -F "FMT_PATTERN_INPUT_TESTS_OK=1" build/wp3_4_pattern_input_tests.log
sh tests/run_step_record_tests.sh | tee build/wp3_4_step_record_tests.log
grep -F "FMT_STEP_RECORD_TESTS_OK=1" build/wp3_4_step_record_tests.log
sh tests/run_tracker_screen_tests.sh | tee build/wp3_4_tracker_screen_tests.log
grep -F "FMT_TRACKER_SCREEN_TESTS_OK=1" build/wp3_4_tracker_screen_tests.log

# Preserve every historical Phase 2 full-app fixture.
sh tests/run_pattern_context_host.sh | tee build/wp3_4_wp2_5_context.log
grep -F "WP2_5_PATTERN_CONTEXT_HOST_OK=1" build/wp3_4_wp2_5_context.log
sh tests/run_track_selection_context_host.sh | tee build/wp3_4_wp2_6_context.log
grep -F "WP2_6_TRACK_CONTEXT_HOST_OK=1" build/wp3_4_wp2_6_context.log
sh tests/run_step_rec_context_host.sh | tee build/wp3_4_wp2_7_context.log
grep -F "WP2_7_STEP_REC_CONTEXT_HOST_OK=1" build/wp3_4_wp2_7_context.log
sh tests/run_special_events_context_host.sh | tee build/wp3_4_wp2_8_context.log
grep -F "WP2_8_OFF_AUDIO_BEHAVIOR_OK=1" build/wp3_4_wp2_8_context.log
grep -F "WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1" build/wp3_4_wp2_8_context.log
sh tests/run_tracker_screen_host.sh | tee build/wp3_4_wp2_9_screen.log
grep -F "WP2_9_REVIEWED_SCREEN_HASHES_OK=1" build/wp3_4_wp2_9_screen.log
grep -F "WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1" build/wp3_4_wp2_9_screen.log
sh tests/run_phase2_integration_tests.sh | tee build/wp3_4_phase2_integration.log
grep -F "PHASE2_INTEGRATION_TESTS_OK=1" build/wp3_4_phase2_integration.log
sh tests/run_phase2_integration_host.sh | tee build/wp3_4_phase2_full_app.log
grep -F "WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1" build/wp3_4_phase2_full_app.log
grep -F "WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8" build/wp3_4_phase2_full_app.log

# D029 must remain byte-for-byte visually stable for its historical fixture.
sh tests/run_lock_editor_context_host.sh | tee build/wp3_4_wp3_3_lock_editor_context.log
grep -F "WP3_3_REVIEWED_LOCK_EDITOR_SCREENS_OK=1" build/wp3_4_wp3_3_lock_editor_context.log
grep -F "WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1 midi_events=0" build/wp3_4_wp3_3_lock_editor_context.log

# New real-app D030 browse/delete fixture and reviewed framebuffers.
sh tests/run_lock_delete_context_host.sh | tee build/wp3_4_lock_delete_context_gate.log
grep -F "WP3_4_REVIEWED_LOCK_DELETE_SCREENS_OK=1" build/wp3_4_lock_delete_context_gate.log
grep -F "WP3_4_LOCK_DELETE_CONTEXT_HOST_OK=1 midi_events=0" build/wp3_4_lock_delete_context_gate.log

# WP3.4 remains storage-only, so the default proof WAV must stay frozen.
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
echo "HOST_WAV_SHA256=$HOST_SHA" | tee build/wp3_4_host_audio.txt
echo "HOST_WAV_FRAMES=$HOST_FRAMES" | tee -a build/wp3_4_host_audio.txt
echo "WP3_4_FROZEN_AUDIO_OK=1" | tee -a build/wp3_4_host_audio.txt

: "${RUNNER_TEMP:?RUNNER_TEMP is required for the pinned device build}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
test -x "$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" --version | head -n 2

# Compile the extended editor translation unit with the real target compiler in
# addition to its normal inclusion in dinghy.c.
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall \
    -Ifirmware/src -c firmware/src/fmt_lock_editor.c -o build/wp3_4_fmt_lock_editor.o
test -s build/wp3_4_fmt_lock_editor.o
echo "WP3_4_JIELI_LOCK_EDITOR_COMPILE_OK=1"

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
./build.sh --release 0.1 | tee build/wp3_4_device_build.log

test -s build/fm1tracker.elf
"$RUNNER_TEMP/jieli/toolchain/common/bin/objdump" -t build/fm1tracker.elf > build/wp3_4_symbols.txt

grep -E 'fmt_lock_editor_runtime$' build/wp3_4_symbols.txt | tee build/wp3_4_lock_editor_symbol.txt
grep -E 'fmt_lock_editor_runtime$' build/wp3_4_symbols.txt | grep -F '.bss'
grep -E 'fmt_lock_store_storage$' build/wp3_4_symbols.txt | tee build/wp3_4_lock_storage_symbol.txt
grep -E 'fmt_lock_store_storage$' build/wp3_4_symbols.txt | grep -F '.pool'
grep -E 'fmt_lock_store_runtime$' build/wp3_4_symbols.txt | tee build/wp3_4_lock_runtime_symbol.txt
grep -E 'fmt_store_storage$' build/wp3_4_symbols.txt | tee build/wp3_4_song_store_symbol.txt

test -s build/fm1tracker.bin
test -s build/fm1tracker-0.1.fwsc
grep -F "register access: hal/ only (src/, loader/ clean)" build/wp3_4_device_build.log

PYTHONPATH=tools python3 - <<'PY'
from pathlib import Path
from fm1_install import product_of
raw = Path('build/fm1tracker-0.1.fwsc').read_bytes()
product = product_of(raw)
assert product == 'FM-1_987', product
print('PACKAGE_IDENTITY=' + product)
PY

python3 - <<'PY' | tee build/wp3_4_metrics.txt
import hashlib, re
from pathlib import Path

log = Path('build/wp3_4_device_build.log').read_text()
m = re.search(r"image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064", log)
assert m, 'resource line not found'
image, image_limit, ram, pool = map(int, m.groups())

# Final measured WP3.4 footprint is frozen by this exact closeout run.
assert image == 64228, image
assert ram == 12480, ram
assert pool == 113000, pool

symbols = Path('build/wp3_4_symbols.txt').read_text().splitlines()
def one(name):
    lines = [line for line in symbols if line.rstrip().endswith(name)]
    assert len(lines) == 1, (name, lines)
    return lines[0].split()

editor = one('fmt_lock_editor_runtime')
lock_store = one('fmt_lock_store_storage')
lock_runtime = one('fmt_lock_store_runtime')
song_store = one('fmt_store_storage')
assert int(editor[0], 16) == 0x01C0B078, editor
assert int(editor[-2], 16) == 10 and '.bss' in editor, editor
assert int(lock_store[0], 16) == 0x01C30168, lock_store
assert int(lock_store[-2], 16) == 16384 and '.pool' in lock_store, lock_store
assert int(lock_runtime[0], 16) == 0x01C0B08C, lock_runtime
assert int(lock_runtime[-2], 16) == 4 and '.bss' in lock_runtime, lock_runtime
assert int(song_store[0], 16) == 0x01C20000, song_store
assert int(song_store[-2], 16) == 65896 and '.pool' in song_store, song_store

bin_sha = hashlib.sha256(Path('build/fm1tracker.bin').read_bytes()).hexdigest()
fwsc_sha = hashlib.sha256(Path('build/fm1tracker-0.1.fwsc').read_bytes()).hexdigest()
wav_sha = hashlib.sha256(Path('build/host/fm1tracker.wav').read_bytes()).hexdigest()
assert bin_sha == '0f828e989888158b09cdb137f7e429f97c42f4c3e30d8d814abd349d9971ed42', bin_sha
assert fwsc_sha == 'de23d8ff997d60178b3b7b9e1490c90696b5c22f3660d187edaa40cf08598a57', fwsc_sha
assert wav_sha == '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440', wav_sha

base_image, base_ram, base_pool = 61716, 12476, 113000
print('WP3_4_METRICS')
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print(f'image_delta_vs_wp3_3={image-base_image}')
print(f'normal_ram_delta_vs_wp3_3={ram-base_ram}')
print(f'pool_delta_vs_wp3_3={pool-base_pool}')
print(f'image_remaining_bytes={image_limit-image}')
print(f'normal_ram_remaining_bytes={98304-ram}')
print(f'pool_remaining_bytes={344064-pool}')
print(f'fm1tracker_bin_sha256={bin_sha}')
print(f'fm1tracker_fwsc_sha256={fwsc_sha}')
print(f'host_wav_sha256={wav_sha}')
PY

grep -F 'image_delta_vs_wp3_3=2512' build/wp3_4_metrics.txt
grep -F 'normal_ram_delta_vs_wp3_3=4' build/wp3_4_metrics.txt
grep -F 'pool_delta_vs_wp3_3=0' build/wp3_4_metrics.txt

echo "WP3_4_CLOSEOUT_GATE_OK=1"
