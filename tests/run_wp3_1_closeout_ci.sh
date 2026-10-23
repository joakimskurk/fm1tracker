#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

BASE=c3693d8c7bb3d5306794aa232d6d25bef50c6c99

# WP3.1 adds only a registry contract/module and tests. It does not link the
# registry into dinghy.c or alter existing Phase 2 production behavior.
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_model.h \
    firmware/src/fmt_store.h firmware/src/fmt_store.c \
    firmware/src/fmt_transport.h firmware/src/fmt_transport.c \
    firmware/src/fmt_playback.h firmware/src/fmt_playback.c \
    firmware/src/fmt_pattern_input.h firmware/src/fmt_pattern_input.c \
    firmware/src/fmt_step_record.h firmware/src/fmt_step_record.c \
    firmware/src/fmt_tracker_screen.h firmware/src/fmt_tracker_screen.c \
    firmware/app.ld firmware/hal firmware/loader tools build.sh
echo "WP3_1_PHASE2_PRODUCTION_PATH_UNCHANGED=1"

sh tests/run_param_registry_tests.sh | tee build/wp3_1_param_registry_tests.log
grep -F "FMT_PARAM_REGISTRY_TESTS_OK=1" build/wp3_1_param_registry_tests.log
grep -F "id_bytes=1 assigned=4 exhaustive_values=256 none=0x00 invalid=0xFF global_reserved=0xE0_0xEF" build/wp3_1_param_registry_tests.log

# Complete inherited Phase 2 model/editor/display regressions.
sh tests/run_model_tests.sh | tee build/wp3_1_model_tests.log
grep -F "FMT_MODEL_TESTS_OK=1" build/wp3_1_model_tests.log
sh tests/run_store_tests.sh | tee build/wp3_1_store_tests.log
grep -F "FMT_STORE_TESTS_OK=1" build/wp3_1_store_tests.log
sh tests/run_transport_tests.sh | tee build/wp3_1_transport_tests.log
grep -F "FMT_TRANSPORT_TESTS_OK=1" build/wp3_1_transport_tests.log
sh tests/run_playback_tests.sh | tee build/wp3_1_playback_tests.log
grep -F "FMT_PLAYBACK_TESTS_OK=1" build/wp3_1_playback_tests.log
sh tests/run_pattern_input_tests.sh | tee build/wp3_1_pattern_input_tests.log
grep -F "FMT_PATTERN_INPUT_TESTS_OK=1" build/wp3_1_pattern_input_tests.log
sh tests/run_step_record_tests.sh | tee build/wp3_1_step_record_tests.log
grep -F "FMT_STEP_RECORD_TESTS_OK=1" build/wp3_1_step_record_tests.log
sh tests/run_tracker_screen_tests.sh | tee build/wp3_1_tracker_screen_tests.log
grep -F "FMT_TRACKER_SCREEN_TESTS_OK=1" build/wp3_1_tracker_screen_tests.log

# Preserve real full-app input/display behavior from Phase 2.
sh tests/run_pattern_context_host.sh | tee build/wp3_1_wp2_5_context.log
grep -F "WP2_5_PATTERN_CONTEXT_HOST_OK=1" build/wp3_1_wp2_5_context.log
sh tests/run_track_selection_context_host.sh | tee build/wp3_1_wp2_6_context.log
grep -F "WP2_6_TRACK_CONTEXT_HOST_OK=1" build/wp3_1_wp2_6_context.log
sh tests/run_step_rec_context_host.sh | tee build/wp3_1_wp2_7_context.log
grep -F "WP2_7_STEP_REC_CONTEXT_HOST_OK=1" build/wp3_1_wp2_7_context.log
sh tests/run_special_events_context_host.sh | tee build/wp3_1_wp2_8_context.log
grep -F "WP2_8_OFF_AUDIO_BEHAVIOR_OK=1" build/wp3_1_wp2_8_context.log
grep -F "WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1" build/wp3_1_wp2_8_context.log
sh tests/run_tracker_screen_host.sh | tee build/wp3_1_wp2_9_screen.log
grep -F "WP2_9_REVIEWED_SCREEN_HASHES_OK=1" build/wp3_1_wp2_9_screen.log
grep -F "WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1" build/wp3_1_wp2_9_screen.log
sh tests/run_phase2_integration_tests.sh | tee build/wp3_1_phase2_integration.log
grep -F "PHASE2_INTEGRATION_TESTS_OK=1" build/wp3_1_phase2_integration.log
sh tests/run_phase2_integration_host.sh | tee build/wp3_1_phase2_full_app.log
grep -F "WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1" build/wp3_1_phase2_full_app.log
grep -F "WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8" build/wp3_1_phase2_full_app.log

# Default audio remains byte-identical because the registry is not in runtime.
./tests/run_host.sh
test -s build/host/fm1tracker.wav
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$HOST_SHA" = "453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440"
echo "HOST_WAV_SHA256=$HOST_SHA" | tee build/wp3_1_host_audio.txt
echo "WP3_1_PHASE2_AUDIO_UNCHANGED=1" | tee -a build/wp3_1_host_audio.txt

: "${RUNNER_TEMP:?RUNNER_TEMP is required for the pinned device build}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
test -x "$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" --version | head -n 2

# Compile the new registry translation unit with the real target compiler even
# though WP3.1 intentionally does not link it into the app runtime yet.
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall \
    -Ifirmware/src -c firmware/src/fmt_param_registry.c -o build/wp3_1_fmt_param_registry.o

test -s build/wp3_1_fmt_param_registry.o
echo "WP3_1_JIELI_REGISTRY_COMPILE_OK=1"

SDK="$RUNNER_TEMP/ac79-sdk"
rm -rf "$SDK"
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
    https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git "$SDK"
ACTUAL_SDK="$(git -C "$SDK" rev-parse HEAD)"
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
./build.sh --release 0.1 | tee build/wp3_1_device_build.log

test -s build/fm1tracker.bin
test -s build/fm1tracker-0.1.fwsc
grep -F "register access: hal/ only (src/, loader/ clean)" build/wp3_1_device_build.log

PYTHONPATH=tools python3 - <<'PY'
from pathlib import Path
from fm1_install import product_of
raw = Path('build/fm1tracker-0.1.fwsc').read_bytes()
product = product_of(raw)
assert product == 'FM-1_987', product
print('PACKAGE_IDENTITY=' + product)
PY

python3 - <<'PY' | tee build/wp3_1_metrics.txt
import hashlib, re
from pathlib import Path
log = Path('build/wp3_1_device_build.log').read_text()
m = re.search(r"image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064", log)
assert m, 'resource line not found'
image, image_limit, ram, pool = map(int, m.groups())
assert image == 59252, image
assert ram == 12464, ram
assert pool == 96616, pool
bin_sha = hashlib.sha256(Path('build/fm1tracker.bin').read_bytes()).hexdigest()
fwsc_sha = hashlib.sha256(Path('build/fm1tracker-0.1.fwsc').read_bytes()).hexdigest()
assert bin_sha == '34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c', bin_sha
assert fwsc_sha == '3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7', fwsc_sha
print('WP3_1_METRICS')
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print('image_delta_vs_phase2=0')
print('normal_ram_delta_vs_phase2=0')
print('pool_delta_vs_phase2=0')
print(f'image_remaining_bytes={image_limit-image}')
print(f'normal_ram_remaining_bytes={98304-ram}')
print(f'pool_remaining_bytes={344064-pool}')
print(f'fm1tracker_bin_sha256={bin_sha}')
print(f'fm1tracker_fwsc_sha256={fwsc_sha}')
PY

grep -F 'image_delta_vs_phase2=0' build/wp3_1_metrics.txt
grep -F 'normal_ram_delta_vs_phase2=0' build/wp3_1_metrics.txt
grep -F 'pool_delta_vs_phase2=0' build/wp3_1_metrics.txt

echo "WP3_1_CLOSEOUT_GATE_OK=1"
