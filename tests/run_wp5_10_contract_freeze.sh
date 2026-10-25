#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

BASE=81d3464059270130747e26fcc950b7a2e1bb03e1
OUT=build/wp5_10_contract
mkdir -p "$OUT"

# WP5.10 begins production-frozen. Benchmark and contract work may not mutate
# target production surfaces before a separately frozen optimization contract.
git diff --exit-code "$BASE" -- \
    firmware/src firmware/gen firmware/hal firmware/loader \
    firmware/app.ld build.sh tools
printf '%s\n' 'WP5_10_SOURCE_FREEZE_OK=1 base=81d3464 production_delta=0'

python3 - <<'PY' > build/wp5_10_contract/reference.log
from pathlib import Path
import re

root = Path('.')
dinghy = (root / 'firmware/src/dinghy.c').read_text()
platform = (root / 'firmware/src/platform.c').read_text()
allocator_h = (root / 'firmware/src/fmt_voice_allocator.h').read_text()
fm_h = (root / 'firmware/src/fmt_fm_engine.h').read_text()
queue_h = (root / 'firmware/src/fmt_rt_queue.h').read_text()
time_h = (root / 'firmware/hal/fm1_time.h').read_text()
fm_env = (root / 'firmware/src/fmt_fm_envelope.c').read_text()
fm_steps = (root / 'firmware/gen/fmt_fm_envelope_step_q15_16.inc').read_text()
fm_engine = (root / 'firmware/src/fmt_fm_engine.c').read_text()
fm_algo = (root / 'firmware/src/fmt_fm_algorithm.c').read_text()
bit_engine = (root / 'firmware/src/fmt_8bit_engine.c').read_text()


def define_u(text, name):
    m = re.search(r'^#define\s+' + re.escape(name) + r'\s+(\d+)u\b', text, re.M)
    assert m, name
    return int(m.group(1))

assert define_u(allocator_h, 'FMT_VOICE_ALLOCATOR_SLOTS') == 8
assert define_u(allocator_h, 'FMT_VOICE_ALLOCATOR_TRACKS') == 8
assert define_u(fm_h, 'FMT_FM_ENGINE_SLOTS') == 8
assert define_u(fm_h, 'FMT_FM_ENGINE_TRACKS') == 8
assert define_u(queue_h, 'FMT_RT_QUEUE_CAPACITY') == 32
assert define_u(platform, 'FS') == 44100
assert define_u(platform, 'HALF_FRAMES') == 256
assert define_u(time_h, 'FM1_TICKS_PER_US') == 24
assert '#define NV 4u' in dinghy

# The linked tracker callback must render both final family engines exactly once
# per sample frame and retain exactly three fixed eight-track dispatch loops:
# playback-start event dispatch, row-boundary event dispatch and sub-row tick.
a0 = dinghy.index('static void app_audio(')
a1 = dinghy.index('/* ---- notes (main loop) */', a0)
audio = dinghy[a0:a1]
assert audio.count('fmt_8bit_engine_render(&fmt_8bit_engine_runtime)') == 1
assert audio.count('fmt_fm_engine_render(&fmt_fm_engine_runtime)') == 1
assert audio.count('for (track = 0u; track < FMT_TRACKS; track++)') == 3
assert 'audition_consume_queue();' in audio

# Both production family renderers remain fixed eight-slot scans.
b0 = bit_engine.index('int16_t fmt_8bit_engine_render(')
b1 = bit_engine.index('uint8_t fmt_8bit_engine_quiet(', b0)
bit_render = bit_engine[b0:b1]
assert bit_render.count('for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)') == 1
assert 'while (' not in bit_render
f0 = fm_engine.index('int16_t fmt_fm_engine_render(')
f1 = fm_engine.index('uint8_t fmt_fm_engine_quiet(', f0)
fm_render = fm_engine[f0:f1]
assert fm_render.count('for (i = 0u; i < FMT_FM_ENGINE_SLOTS; i++)') == 1
assert 'while (' not in fm_render
assert 'fmt_fm_feedback_render_op4' in fm_render
assert 'fmt_fm_algorithm_render_from_op4' in fm_render

# FAN3 retains all four operators through OP4 + OP3/OP2/OP1 and the explicit
# modulation accumulation path used by the frozen S1 FM patch.
fan0 = fm_algo.index('if (selected == FMT_FM_ALGO_FAN3)')
fan1 = fm_algo.index('if (selected == FMT_FM_ALGO_DUAL2)', fan0)
fan = fm_algo[fan0:fan1]
assert fan.count('fmt_fm_operator_render(') == 3
assert fan.count('fmt_fm_algorithm_phase_add(') == 2
assert fan.count('fmt_fm_algorithm_edge_mod(') == 3

# Valid NOTE OFF detaches the voice and clears route ownership. A later family
# NOTE therefore does not hard-silence that detached tail merely because the
# new family differs. This is the production mechanism that makes S1 8+8
# occupancy reachable.
r0 = dinghy.index('static void tracker_release_route(')
r1 = dinghy.index('static void tracker_apply_synthetic_off(', r0)
release = dinghy[r0:r1]
assert 'fmt_fm_engine_release(&fmt_fm_engine_runtime' in release
assert 'fmt_engine_router_clear(&fmt_engine_router_runtime' in release
n0 = dinghy.index('static uint8_t tracker_note_route(')
n1 = dinghy.index('static void tracker_release_route(', n0)
note_route = dinghy[n0:n1]
assert 'current_family != FMT_ENGINE_FAMILY_NONE' in note_route
assert 'current_family != requested_family' in note_route

# Freeze the long FM release prerequisite. Control 255 must remain the final
# generated step 795 Q15.16 units/sample, which leaves a full-level release tail
# alive far beyond the next tracker row at 44.1 kHz.
m = re.search(r'fmt_fm_envelope_step_q15_16\[256\]\s*=\s*\{(.*?)\};', fm_steps, re.S)
assert m
steps = [int(x) for x in re.findall(r'(\d+)u', m.group(1))]
assert len(steps) == 256
assert steps[255] == 795
max_level = 32767 << 16
release_seconds = (max_level / steps[255]) / 44100.0
assert release_seconds > 60.0
assert 'state->stage[operator_index] = FMT_FM_ENVELOPE_STAGE_RELEASE;' in fm_env

nominal_ticks = 24000000 * 256 / 44100
assert 139319.0 < nominal_ticks < 139321.0

print('WP5_10_REFERENCE_OK=1 tracks=8 bit_slots=8 fm_slots=8 audition=4 queue=32 fs=44100 half_frames=256 timer_mhz=24 dispatch_loops=3')
print('WP5_10_MIXED_REACHABILITY_OK=1 fm_release_control=255 release_step=795 release_seconds_gt=60 route_clear_after_off=1')
print('WP5_10_DUAL_RENDER_OK=1 bit_render_calls=1 fm_render_calls=1 fixed_slot_scans=8+8 fan3_ops=4')
print(f'WP5_10_TIMER_REFERENCE_OK=1 nominal_half_buffer_ticks={nominal_ticks:.3f} target_runtime_timing_gate=deferred_without_execution_path')
PY
cat "$OUT/reference.log"
grep -F 'WP5_10_REFERENCE_OK=1 tracks=8 bit_slots=8 fm_slots=8 audition=4 queue=32 fs=44100 half_frames=256 timer_mhz=24 dispatch_loops=3' "$OUT/reference.log"
grep -F 'WP5_10_MIXED_REACHABILITY_OK=1 fm_release_control=255 release_step=795 release_seconds_gt=60 route_clear_after_off=1' "$OUT/reference.log"
grep -F 'WP5_10_DUAL_RENDER_OK=1 bit_render_calls=1 fm_render_calls=1 fixed_slot_scans=8+8 fan3_ops=4' "$OUT/reference.log"
grep -F 'target_runtime_timing_gate=deferred_without_execution_path' "$OUT/reference.log"

# Re-prove the accepted mixed-family parent behavior before benchmark code exists.
sh tests/run_wp5_9_mixed_validation_tests.sh > "$OUT/wp5_9_parent.log" 2>&1
cat "$OUT/wp5_9_parent.log"
grep -F 'WP5_9_HOST_GATE_OK=1 production_delta=dinghy_fix simultaneous=real saturated_sum=exact switch=real events=covered delayed_bidirectional=1 delayed_unsupported=1 plock_family_safe=exact d056=exact all8bit=wp5_0_identical hardware_margin_claim=none' "$OUT/wp5_9_parent.log"

CONTRACT=docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md
grep -F 'S1 - MIXED_SATURATION_8FM_8BIT' "$CONTRACT" >/dev/null
grep -F 'fm_non_idle_slots         = 8' "$CONTRACT" >/dev/null
grep -F 'bit8_non_idle_slots       = 8' "$CONTRACT" >/dev/null
grep -F 'warmup: 256 frames' "$CONTRACT" >/dev/null
grep -F 'measured frames: 4096 exactly' "$CONTRACT" >/dev/null
grep -F 'Run the complete S1 fixture three times from reset.' "$CONTRACT" >/dev/null
grep -F 'S2 - FULL_CALLBACK_PRESSURE' "$CONTRACT" >/dev/null
grep -F 'queue_depth_at_callback_entry = 32' "$CONTRACT" >/dev/null
grep -F '4 * 256 = 1024' "$CONTRACT" >/dev/null
grep -F 'Run S2 three times from reset' "$CONTRACT" >/dev/null
grep -F 'target_runtime_timing_gate=deferred_without_execution_path' "$CONTRACT" >/dev/null
grep -F 'image       = 79836 B' "$CONTRACT" >/dev/null
grep -F 'normal RAM  = 27792 B' "$CONTRACT" >/dev/null
grep -F 'pool        = 113000 B' "$CONTRACT" >/dev/null
grep -F 'hardware_margin_claim=none' "$CONTRACT" >/dev/null
grep -F 'separate optimization sub-contract' "$CONTRACT" >/dev/null
printf '%s\n' 'WP5_10_CONTRACT_DOCS_OK=1 s1=8+8x4096 warmup=256 repeats=3 s2=queue32+audition4x256 repeats=3 production=frozen'

printf '%s\n' 'WP5_10_CONTRACT_FREEZE_OK=1 production_delta=0 mixed=8+8 measured_frames=4096 warmup=256 s1_repeats=3 callback_queue=32 audition_sines=1024 s2_repeats=3 target_runtime_timing_gate=deferred_without_execution_path hardware_margin_claim=none' > "$OUT/summary.txt"
cat "$OUT/summary.txt"
