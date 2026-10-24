#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build build/host build/tests

WP3_8=5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018

# D035 is deliberately presentation-only. Freeze the production-source scope:
# the tracker presentation cache/formatting, detailed storage-only footer marker,
# and the two app sync call sites are the only production changes vs WP3.8.
PROD_DIFF="$(git diff --name-only "$WP3_8" -- firmware/src firmware/hal firmware/loader tools build.sh)"
EXPECTED_DIFF="firmware/src/dinghy.c
firmware/src/fmt_lock_editor.c
firmware/src/fmt_tracker_screen.c
firmware/src/fmt_tracker_screen.h"
test "$PROD_DIFF" = "$EXPECTED_DIFF"
git diff --exit-code "$WP3_8" -- firmware/hal firmware/loader tools build.sh firmware/app.ld firmware/src/platform.c
echo 'WP3_9_PRODUCTION_SCOPE_OK=1 files=4 low_level_unchanged=1'

# D035 presentation contract, inherited D026 screen semantics and D029/D030
# detailed lock-editor semantics.
sh tests/run_tracker_screen_tests.sh | tee build/wp3_9_screen_unit.log
grep -F 'FMT_TRACKER_SCREEN_TESTS_OK=1' build/wp3_9_screen_unit.log
grep -F 'state_bytes=62 rows=8 regions=10 max_regions_per_poll=2 pattern=0' build/wp3_9_screen_unit.log
grep -F 'WP3_9_VISUAL_TESTS_OK=1' build/wp3_9_screen_unit.log
grep -F 'screen_bytes=62 rows=8 regions=10 max_regions_per_poll=2' build/wp3_9_screen_unit.log

sh tests/run_lock_editor_tests.sh | tee build/wp3_9_lock_editor_unit.log
grep -F 'FMT_LOCK_EDITOR_TESTS_OK=1' build/wp3_9_lock_editor_unit.log
grep -F 'state_bytes=10 pattern=0 knobs=4 browse=select_sorted_clamped delete=edit_arm_then_sel_confirm clear_all=0' build/wp3_9_lock_editor_unit.log

# Visually reviewed D035 reference surface and stale-HOME cleanup.
sh tests/run_wp3_9_visual_host.sh | tee build/wp3_9_visual_gate.log
grep -F 'WP3_9_ACTIVE_SCREEN_SHA256=d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3' build/wp3_9_visual_gate.log
grep -F 'WP3_9_RELEASED_SCREEN_SHA256=ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6' build/wp3_9_visual_gate.log
grep -F 'WP3_9_REVIEWED_SCREEN_HASHES_OK=1' build/wp3_9_visual_gate.log
grep -F 'WP3_9_HOME_RETURN_PIXEL_IDENTICAL=1' build/wp3_9_visual_gate.log
grep -F 'WP3_9_VISUAL_HOST_OK=1' build/wp3_9_visual_gate.log

# D034 queue behavior must remain exact.
sh tests/run_rt_queue_tests.sh | tee build/wp3_9_d034_unit.log
grep -F 'FMT_RT_QUEUE_TESTS_OK=1' build/wp3_9_d034_unit.log
sh tests/run_wp3_8_queue_audio_host.sh | tee build/wp3_9_d034_audio.log
grep -F 'WP3_8_CONTROL_WAV_SHA256=8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6' build/wp3_9_d034_audio.log
grep -F 'WP3_8_FIFO_WAV_SHA256=8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6' build/wp3_9_d034_audio.log
grep -F 'WP3_8_HARDWARE_WAV_SHA256=1b13d55424dd16e36c1820501dcf6b3232a3e1d3bfb357259f74e85a45858b18' build/wp3_9_d034_audio.log
grep -F 'WP3_8_MIDI_WAV_SHA256=f095db1bee4fa26c6ca56d298b30402b0a43a4ae700b31bf52854943151cfc71' build/wp3_9_d034_audio.log
grep -F 'WP3_8_OVERFLOW_WAV_SHA256=31c55a429e53edf84fe5b8bc634b8fab42b45e5ad744e0b6a4505655e3b92a2f' build/wp3_9_d034_audio.log
grep -F 'WP3_8_RECOVERY_WAV_SHA256=42207d5ee151619f525d4416f3e77b79fe2d9f0746bb5f111c88858a2ca32b93' build/wp3_9_d034_audio.log
grep -F 'WP3_8_QUEUE_AUDIO_BEHAVIOR_OK=1 fifo=1 hardware=1 midi=1 overflow_silence=1 recovery=1 queue_midi_neutral=1' build/wp3_9_d034_audio.log

# D033/D032/D031 audio and timing stay byte-identical.
sh tests/run_tracker_event_tests.sh | tee build/wp3_9_d033_unit.log
grep -F 'FMT_TRACKER_EVENT_TESTS_OK=1' build/wp3_9_d033_unit.log
sh tests/run_wp3_7_event_audio_host.sh | tee build/wp3_9_d033_audio.log
grep -F 'WP3_7_EVENT_AUDIO_BEHAVIOR_OK=1 delay=1 cut=1 cut0=1 retrig=1 invalid_noop=1 midi_events=0' build/wp3_9_d033_audio.log

sh tests/run_tracker_effect_tests.sh | tee build/wp3_9_d032_unit.log
grep -F 'FMT_TRACKER_EFFECT_TESTS_OK=1' build/wp3_9_d032_unit.log
sh tests/run_wp3_6_tick_grid_tests.sh | tee build/wp3_9_tick_grid.log
grep -F 'WP3_6_TICK_GRID_TESTS_OK=1' build/wp3_9_tick_grid.log
sh tests/run_wp3_6_effect_audio_host.sh | tee build/wp3_9_d032_audio.log
grep -F 'WP3_6_CONTROL_WAV_SHA256=8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6' build/wp3_9_d032_audio.log
grep -F 'WP3_6_ARP_WAV_SHA256=5e95abbdc6a0a9a65af402b7393b9b4088e968297d239679cd78b3872595a743' build/wp3_9_d032_audio.log
grep -F 'WP3_6_SLIDE_WAV_SHA256=2e205d5d46a6c604fbe6c5976dbf57f252dc4288de70ee69a66b7a6769d4e50e' build/wp3_9_d032_audio.log
grep -F 'WP3_6_SLIDE_DOWN_WAV_SHA256=9bc32956995fe8ada92c3bf2fde80d6b9f6e3a1cd32e06343a170ed70e1e3edc' build/wp3_9_d032_audio.log
grep -F 'WP3_6_PORTA_WAV_SHA256=2d84235e03ca33d055baf2b2d0d3a430a1b28d3ff4f0faa913d5556f52f1f7e7' build/wp3_9_d032_audio.log
grep -F 'WP3_6_VIBRATO_WAV_SHA256=e14673fca74449f31783a44168edbf7146b0a1aa73f492dd9460febb7542c4a4' build/wp3_9_d032_audio.log
grep -F 'WP3_6_ARP_FINE_WAV_SHA256=5ab516bc70ca571e4dfa43330075e08c389d1fb5ae1a668bc14231f1a50fd950' build/wp3_9_d032_audio.log
grep -F 'WP3_6_INVALID_WAV_SHA256=8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6' build/wp3_9_d032_audio.log

sh tests/run_playback_params_tests.sh | tee build/wp3_9_d031_unit.log
grep -F 'FMT_PLAYBACK_PARAMS_TESTS_OK=1' build/wp3_9_d031_unit.log
sh tests/run_wp3_5_locked_audio_host.sh | tee build/wp3_9_d031_audio.log
grep -F 'WP3_5_LOCKED_AUDIO_SHA256=8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8' build/wp3_9_d031_audio.log
grep -F 'WP3_5_LOCKED_AUDIO_BEHAVIOR_OK=1 pre_equal=1 combined_level=1 fine_pitch=1 empty_mute=1 unlocked_restore=1 midi_events=0' build/wp3_9_d031_audio.log

# D027-D030 and Phase 2 bounded model/editor primitives remain authoritative.
sh tests/run_param_registry_tests.sh | tee build/wp3_9_d027.log
grep -F 'FMT_PARAM_REGISTRY_TESTS_OK=1' build/wp3_9_d027.log
sh tests/run_lock_store_tests.sh | tee build/wp3_9_d028.log
grep -F 'FMT_LOCK_STORE_TESTS_OK=1' build/wp3_9_d028.log
grep -F 'FMT_LOCK_LIMIT_TESTS_OK=1' build/wp3_9_d028.log
sh tests/run_model_tests.sh | tee build/wp3_9_model.log
grep -F 'FMT_MODEL_TESTS_OK=1' build/wp3_9_model.log
sh tests/run_store_tests.sh | tee build/wp3_9_store.log
grep -F 'FMT_STORE_TESTS_OK=1' build/wp3_9_store.log
sh tests/run_transport_tests.sh | tee build/wp3_9_transport.log
grep -F 'FMT_TRANSPORT_TESTS_OK=1' build/wp3_9_transport.log
sh tests/run_playback_tests.sh | tee build/wp3_9_playback.log
grep -F 'FMT_PLAYBACK_TESTS_OK=1' build/wp3_9_playback.log
sh tests/run_pattern_input_tests.sh | tee build/wp3_9_pattern_input.log
grep -F 'FMT_PATTERN_INPUT_TESTS_OK=1' build/wp3_9_pattern_input.log
sh tests/run_step_record_tests.sh | tee build/wp3_9_step_record.log
grep -F 'FMT_STEP_RECORD_TESTS_OK=1' build/wp3_9_step_record.log
sh tests/run_phase2_integration_tests.sh | tee build/wp3_9_phase2_integration_unit.log
grep -F 'PHASE2_INTEGRATION_TESTS_OK=1' build/wp3_9_phase2_integration_unit.log

# Full-app non-visual Phase 2 context checks are unchanged and contain no
# tracker-pixel freeze.
sh tests/run_pattern_context_host.sh | tee build/wp3_9_wp2_5_context.log
grep -F 'WP2_5_PATTERN_CONTEXT_HOST_OK=1' build/wp3_9_wp2_5_context.log
sh tests/run_track_selection_context_host.sh | tee build/wp3_9_wp2_6_context.log
grep -F 'WP2_6_TRACK_CONTEXT_HOST_OK=1' build/wp3_9_wp2_6_context.log
sh tests/run_step_rec_context_host.sh | tee build/wp3_9_wp2_7_context.log
grep -F 'WP2_7_STEP_REC_CONTEXT_HOST_OK=1' build/wp3_9_wp2_7_context.log
sh tests/run_special_events_context_host.sh | tee build/wp3_9_wp2_8_context.log
grep -F 'WP2_8_OFF_AUDIO_BEHAVIOR_OK=1' build/wp3_9_wp2_8_context.log
grep -F 'WP2_8_SPECIAL_EVENTS_CONTEXT_HOST_OK=1' build/wp3_9_wp2_8_context.log

# WP2.10, D029 and D030 have historical screenshots whose exact pixels are
# intentionally superseded by D035. Re-run their gestures directly and keep
# the behavioral/MIDI/HOME proofs without asserting the obsolete pixels.
cc -O2 -Wall -Wextra -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"dev"' \
    -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp3_9_compat firmware/src/dinghy.c

build/host/fm1tracker_wp3_9_compat build/host/wp3_9_phase2_integration tests/wp2_10_phase2_integration.script > build/wp3_9_phase2_full_app.log
build/host/fm1tracker_wp3_9_compat build/host/wp3_9_phase2_home tests/wp2_10_home_equivalent_control.script > build/wp3_9_phase2_home.log
on_count="$(grep -c 'MIDI out 90' build/wp3_9_phase2_full_app.log)"
off_count="$(grep -c 'MIDI out 80' build/wp3_9_phase2_full_app.log)"
test "$on_count" -eq 4
test "$off_count" -eq 4
cmp build/host/wp3_9_phase2_integration-3000.ppm build/host/wp3_9_phase2_home-3000.ppm
echo 'WP3_9_PHASE2_FULL_APP_COMPAT_OK=1 midi_note_pairs=4 home_identical=1'

build/host/fm1tracker_wp3_9_compat build/host/wp3_9_lock_editor tests/wp3_3_lock_editor_context.script > build/wp3_9_lock_editor_context.log
test "$(grep -c 'MIDI out' build/wp3_9_lock_editor_context.log || true)" -eq 0
echo 'WP3_9_D029_FULL_APP_COMPAT_OK=1 midi_events=0'

build/host/fm1tracker_wp3_9_compat build/host/wp3_9_lock_delete tests/wp3_4_lock_delete_context.script > build/wp3_9_lock_delete_context.log
test "$(grep -c 'MIDI out' build/wp3_9_lock_delete_context.log || true)" -eq 0
echo 'WP3_9_D030_FULL_APP_COMPAT_OK=1 midi_events=0'

# Historical default proof audio remains byte-identical.
./tests/run_host.sh
HOST_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$HOST_SHA" = '453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440'
echo "WP3_9_DEFAULT_AUDIO_OK=1 sha256=$HOST_SHA"

# Pinned real target toolchain + SDK + package build.
: "${RUNNER_TEMP:?RUNNER_TEMP is required for the pinned device build}"
./tools/get_toolchain.sh "$RUNNER_TEMP/jieli"
test -x "$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang"
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" --version | head -n 2

# Independently target-compile the D035 presentation translation unit.
"$RUNNER_TEMP/jieli/toolchain/pi32v2/bin/clang" -target pi32v2 -Os -ffunction-sections -fno-builtin -Wall \
    -Ifirmware/src -c firmware/src/fmt_tracker_screen.c -o build/wp3_9_fmt_tracker_screen.o
test -s build/wp3_9_fmt_tracker_screen.o
echo 'WP3_9_JIELI_TRACKER_SCREEN_COMPILE_OK=1'

SDK="$RUNNER_TEMP/ac79-sdk"
rm -rf "$SDK"
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
    https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git "$SDK"
ACTUAL_SDK="$(git -C "$SDK" rev-parse HEAD)"
echo "AC79_SDK_COMMIT=$ACTUAL_SDK"
test "$ACTUAL_SDK" = 'd179b4484759423312073f5fbb232501aa491047'
test "$(sha256sum "$SDK/cpu/wl82/tools/uboot.boot" | awk '{print $1}')" = '4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4'
test "$(sha256sum "$SDK/cpu/wl82/tools/cfg_tool.bin" | awk '{print $1}')" = '276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b'
test "$(sha256sum "$SDK/cpu/wl82/tools/cfg/eq_cfg_hw.bin" | awk '{print $1}')" = '41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d'
echo 'AC79_SDK_INPUTS_OK=1'

JIELI_TOOLCHAIN="$RUNNER_TEMP/jieli/toolchain" AC79_SDK="$SDK" \
    ./build.sh --release 0.1 | tee build/wp3_9_device_build.log

test -s build/fm1tracker.elf
test -s build/fm1tracker.bin
test -s build/fm1tracker-0.1.fwsc
"$RUNNER_TEMP/jieli/toolchain/common/bin/objdump" -t build/fm1tracker.elf > build/wp3_9_symbols.txt
grep -F 'register access: hal/ only (src/, loader/ clean)' build/wp3_9_device_build.log

PYTHONPATH=tools python3 - <<'PY'
from pathlib import Path
from fm1_install import product_of
product = product_of(Path('build/fm1tracker-0.1.fwsc').read_bytes())
assert product == 'FM-1_987', product
print('PACKAGE_IDENTITY=' + product)
PY

# Final resource placement and deterministic artifacts. Do not guess the image
# or ordinary-RAM delta: measure it here against the verified WP3.8 baseline.
python3 - <<'PY' | tee build/wp3_9_measured_metrics.txt
import hashlib, re
from pathlib import Path

log = Path('build/wp3_9_device_build.log').read_text()
m = re.search(r"image (\d+) B of (\d+); RAM \.data\+\.bss (\d+) B of 98304; pool (\d+) B of 344064", log)
assert m, 'resource line not found'
image, image_limit, ram, pool = map(int, m.groups())
assert image < image_limit
assert ram < 98304
assert pool == 113000, pool

symbols = Path('build/wp3_9_symbols.txt').read_text().splitlines()
def one(name):
    hits = [x.split() for x in symbols if x.rstrip().endswith(name)]
    assert len(hits) == 1, (name, hits)
    return hits[0]

screen = one('fmt_tracker_screen_runtime')
editor = one('fmt_lock_editor_runtime')
queue = one('fmt_rt_queue_runtime')
overflow = one('fmt_rt_queue_overflow_seen')
effect = one('fmt_tracker_effect_runtime')
event = one('fmt_tracker_row_event_runtime')
lock_store = one('fmt_lock_store_storage')
song_store = one('fmt_store_storage')
assert int(screen[-2], 16) == 62 and '.bss' in screen, screen
assert int(editor[-2], 16) == 10 and '.bss' in editor, editor
assert int(queue[-2], 16) == 268 and '.bss' in queue, queue
assert int(overflow[-2], 16) == 4 and '.bss' in overflow, overflow
assert int(effect[-2], 16) == 12 and '.bss' in effect, effect
assert int(event[-2], 16) == 4 and '.bss' in event, event
assert int(lock_store[-2], 16) == 16384 and '.pool' in lock_store, lock_store
assert int(song_store[-2], 16) == 65896 and '.pool' in song_store, song_store

def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()

print('WP3_9_MEASURED_METRICS')
print(f'image_bytes={image}')
print(f'normal_ram_bytes={ram}')
print(f'pool_ram_bytes={pool}')
print(f'image_delta_vs_wp3_8={image-66308}')
print(f'normal_ram_delta_vs_wp3_8={ram-13280}')
print(f'pool_delta_vs_wp3_8={pool-113000}')
print(f'image_remaining_bytes={image_limit-image}')
print(f'normal_ram_remaining_bytes={98304-ram}')
print(f'pool_remaining_bytes={344064-pool}')
print('tracker_screen_bytes=62')
print('lock_editor_bytes=10')
print('queue_bytes=268')
print(f'fm1tracker_bin_sha256={sha("build/fm1tracker.bin")}')
print(f'fm1tracker_fwsc_sha256={sha("build/fm1tracker-0.1.fwsc")}')
print(f'default_wav_sha256={sha("build/host/fm1tracker.wav")}')
print(f'd031_locked_wav_sha256={sha("build/host/fm1tracker_wp3_5_locked.wav")}')
for name in ('control','fifo','hardware','midi','overflow','recovery'):
    print(f'wp3_8_{name}_wav_sha256={sha(f"build/host/fm1tracker_wp3_8_{name}.wav")}')
PY

grep -F 'pool_delta_vs_wp3_8=0' build/wp3_9_measured_metrics.txt
grep -F 'tracker_screen_bytes=62' build/wp3_9_measured_metrics.txt
grep -F 'default_wav_sha256=453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440' build/wp3_9_measured_metrics.txt
grep -F 'd031_locked_wav_sha256=8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8' build/wp3_9_measured_metrics.txt

echo 'WP3_9_CLOSEOUT_OK=1'
