#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

BASE=47e9093ae300043b804f85ce62fee1fe758fc943

# WP2.10 is test/evidence only. Production source must remain exactly the
# verified WP2.9 closeout source tree.
git diff --exit-code "$BASE" -- firmware tools build.sh
echo "WP2_10_PRODUCTION_SOURCE_UNCHANGED=1"

sh tests/run_phase2_integration_tests.sh | tee build/wp2_10_phase2_integration_tests.log
grep -F "PHASE2_INTEGRATION_TESTS_OK=1" build/wp2_10_phase2_integration_tests.log
grep -F "visited_steps=64 tracks=8 edits_track=7 wrap=63_to_0 page_cross=15_to_16 boundaries_120=64 loop_frames=352800 final_cursor=20 final_page=1 final_track=7" build/wp2_10_phase2_integration_tests.log
grep -F "store_bytes=65896 final_events=note,empty,tie,off proof_lane=track0_unchanged screen_window=16_23" build/wp2_10_phase2_integration_tests.log

sh tests/run_model_tests.sh | tee build/wp2_10_model_tests.log
grep -F "FMT_MODEL_TESTS_OK=1" build/wp2_10_model_tests.log

sh tests/run_store_tests.sh | tee build/wp2_10_store_tests.log
grep -F "FMT_STORE_TESTS_OK=1" build/wp2_10_store_tests.log

sh tests/run_transport_tests.sh | tee build/wp2_10_transport_tests.log
grep -F "FMT_TRANSPORT_TESTS_OK=1" build/wp2_10_transport_tests.log
grep -F "phase_per_step=661500" build/wp2_10_transport_tests.log

sh tests/run_playback_tests.sh | tee build/wp2_10_playback_tests.log
grep -F "FMT_PLAYBACK_TESTS_OK=1" build/wp2_10_playback_tests.log
grep -F "loop_frames_120=352800" build/wp2_10_playback_tests.log
grep -F "event_bytes=4 kinds=none,note,tie,off" build/wp2_10_playback_tests.log

sh tests/run_pattern_input_tests.sh | tee build/wp2_10_pattern_input_tests.log
grep -F "FMT_PATTERN_INPUT_TESTS_OK=1" build/wp2_10_pattern_input_tests.log

sh tests/run_step_record_tests.sh | tee build/wp2_10_step_record_tests.log
grep -F "FMT_STEP_RECORD_TESTS_OK=1" build/wp2_10_step_record_tests.log
grep -F "wrap=63_to_0" build/wp2_10_step_record_tests.log
grep -F "specials=empty,tie,off" build/wp2_10_step_record_tests.log

sh tests/run_tracker_screen_tests.sh | tee build/wp2_10_tracker_screen_tests.log
grep -F "FMT_TRACKER_SCREEN_TESTS_OK=1" build/wp2_10_tracker_screen_tests.log
grep -F "state_bytes=44 rows=8 regions=10 max_regions_per_poll=2 pattern=0" build/wp2_10_tracker_screen_tests.log

sh tests/run_pattern_context_host.sh | tee build/wp2_10_wp2_5_context_gate.log
grep -F "WP2_5_PATTERN_CONTEXT_HOST_OK=1" build/wp2_10_wp2_5_context_gate.log

sh tests/run_track_selection_context_host.sh | tee build/wp2_10_wp2_6_context_gate.log
grep -F "WP2_6_TRACK_CONTEXT_HOST_OK=1" build/wp2_10_wp2_6_context_gate.log

sh tests/run_step_rec_context_host.sh | tee build/wp2_10_wp2_7_context_gate.log
grep -F "WP2_7_STEP_REC_CONTEXT_HOST_OK=1" build/wp2_10_wp2_7_context_gate.log

sh tests/run_special_events_context_host.sh | tee build/wp2_10_wp2_8_context_gate.log
grep -F "WP2_8_OFF_AUDIO_BEHAVIOR_OK=1" build/wp2_10_wp2_8_context_gate.log
grep -F "WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1" build/wp2_10_wp2_8_context_gate.log

sh tests/run_tracker_screen_host.sh | tee build/wp2_10_wp2_9_screen_gate.log
grep -F "WP2_9_HOME_RETURN_PIXEL_IDENTICAL=1" build/wp2_10_wp2_9_screen_gate.log
grep -F "WP2_9_REVIEWED_SCREEN_HASHES_OK=1" build/wp2_10_wp2_9_screen_gate.log
grep -F "WP2_9_TRACKER_SCREEN_HOST_OK=1" build/wp2_10_wp2_9_screen_gate.log

sh tests/run_phase2_integration_host.sh | tee build/wp2_10_full_app_gate.log
grep -F "WP2_10_HOME_RETURN_PIXEL_IDENTICAL=1" build/wp2_10_full_app_gate.log
grep -F "WP2_10_REVIEWED_INTEGRATION_SCREEN_OK=1" build/wp2_10_full_app_gate.log
grep -F "WP2_10_FULL_APP_HOST_OK=1 midi_note_pairs=4 traversed_steps=64 selected_track=8" build/wp2_10_full_app_gate.log

./tests/run_host.sh
test -s build/host/fm1tracker.wav
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
HOST_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/fm1tracker.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
echo "HOST_WAV_SHA256=$HOST_SHA" | tee build/wp2_10_host_audio.txt
echo "HOST_WAV_FRAMES=$HOST_FRAMES" | tee -a build/wp2_10_host_audio.txt
test "$HOST_SHA" = "453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440"
test "$HOST_FRAMES" = "286464"
echo "WP2_10_FROZEN_AUDIO_OK=1" | tee -a build/wp2_10_host_audio.txt

: "${RUNNER_TEMP:?RUNNER_TEMP is required for the pinned device build}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
test -x "$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" --version | head -n 2

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
./build.sh --release 0.1 | tee build/wp2_10_device_build.log

test -s build/fm1tracker.elf
"$RUNNER_TEMP/jieli/toolchain/common/bin/objdump" -t build/fm1tracker.elf > build/wp2_10_symbols.txt
for name in fmt_store_storage fmt_transport_runtime fmt_playback_runtime fmt_pattern_input_runtime fmt_step_record_runtime fmt_tracker_screen_runtime; do
    grep -E "${name}$" build/wp2_10_symbols.txt | tee "build/wp2_10_${name}_symbol.txt"
done
grep -E 'fmt_store_storage$' build/wp2_10_symbols.txt | grep -F '.pool'
grep -E 'fmt_transport_runtime$|fmt_playback_runtime$|fmt_pattern_input_runtime$|fmt_step_record_runtime$|fmt_tracker_screen_runtime$' build/wp2_10_symbols.txt | grep -F '.bss'

test -s build/fm1tracker.bin
test -s build/fm1tracker-0.1.fwsc
grep -F "register access: hal/ only (src/, loader/ clean)" build/wp2_10_device_build.log

PYTHONPATH=tools python3 - <<'PY'
from pathlib import Path
from fm1_install import product_of
raw = Path('build/fm1tracker-0.1.fwsc').read_bytes()
product = product_of(raw)
assert product == 'FM-1_987', product
print('PACKAGE_IDENTITY=' + product)
PY

python3 - <<'PY' | tee build/wp2_10_metrics.txt
import hashlib
import os
import re
from pathlib import Path

log = Path('build/wp2_10_device_build.log').read_text()
m = re.search(r"image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064", log)
assert m, 'resource line not found'
image, image_limit, ram, pool = map(int, m.groups())

# Test-only WP2.10: exact WP2.9 production footprint is required.
assert image == 59252, image
assert ram == 12464, ram
assert pool == 96616, pool

symbols = Path('build/wp2_10_symbols.txt').read_text().splitlines()
def one(name):
    lines = [line for line in symbols if line.rstrip().endswith(name)]
    assert len(lines) == 1, (name, lines)
    return lines[0].split()

expected = {
    'fmt_step_record_runtime': (0x01C0B070, 3, '.bss'),
    'fmt_pattern_input_runtime': (0x01C0B073, 4, '.bss'),
    'fmt_transport_runtime': (0x01C0B078, 8, '.bss'),
    'fmt_tracker_screen_runtime': (0x01C0B080, 44, '.bss'),
    'fmt_playback_runtime': (0x01C0B0AC, 1, '.bss'),
    'fmt_store_storage': (0x01C20000, 65896, '.pool'),
}
for name, (addr, size, section) in expected.items():
    sym = one(name)
    assert int(sym[0], 16) == addr, (name, sym)
    assert int(sym[-2], 16) == size, (name, sym)
    assert section in sym, (name, sym)

bin_sha = hashlib.sha256(Path('build/fm1tracker.bin').read_bytes()).hexdigest()
fwsc_sha = hashlib.sha256(Path('build/fm1tracker-0.1.fwsc').read_bytes()).hexdigest()
wav_sha = hashlib.sha256(Path('build/host/fm1tracker.wav').read_bytes()).hexdigest()
assert bin_sha == '34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c', bin_sha
assert fwsc_sha == '3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7', fwsc_sha
assert wav_sha == '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440', wav_sha

phase1_image, phase1_ram, phase1_pool = 55088, 12368, 30720
print('PHASE2_CLOSEOUT_METRICS')
print(f"commit={os.environ.get('GITHUB_SHA', 'local')}")
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print(f'phase2_image_delta_vs_phase1={image - phase1_image}')
print(f'phase2_normal_ram_delta_vs_phase1={ram - phase1_ram}')
print(f'phase2_pool_delta_vs_phase1={pool - phase1_pool}')
print(f'image_remaining_bytes={image_limit - image}')
print(f'normal_ram_remaining_bytes={98304 - ram}')
print(f'pool_remaining_bytes={344064 - pool}')
print(f'fm1tracker_bin_sha256={bin_sha}')
print(f'fm1tracker_fwsc_sha256={fwsc_sha}')
print(f'host_wav_sha256={wav_sha}')
print('production_delta_vs_wp2_9=0')
PY

grep -F "phase2_image_delta_vs_phase1=4164" build/wp2_10_metrics.txt
grep -F "phase2_normal_ram_delta_vs_phase1=96" build/wp2_10_metrics.txt
grep -F "phase2_pool_delta_vs_phase1=65896" build/wp2_10_metrics.txt
grep -F "production_delta_vs_wp2_9=0" build/wp2_10_metrics.txt

echo "WP2_10_PHASE2_CLOSEOUT_BUDGET_OK=1"
echo "WP2_10_PHASE2_CLOSEOUT_GATE_OK=1"
