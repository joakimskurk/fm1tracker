#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_5_contract
BASE=d3501eca952f3e903d4dcc4faab2851d439f7ad0
mkdir -p "$OUT"

# D053 must be frozen before any feedback production source exists.
grep -F 'Status: **FROZEN BY D053 BEFORE FM FEEDBACK PRODUCTION SOURCE**' docs/WP5_5_FM_FEEDBACK_CONTRACT.md
test ! -e firmware/src/fmt_fm_feedback.h
test ! -e firmware/src/fmt_fm_feedback.c
! grep -R -F 'fmt_fm_feedback' firmware/src firmware/gen

# Frozen DSP layers and running application remain untouched at contract freeze.
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_fm_operator.h \
    firmware/src/fmt_fm_operator.c \
    firmware/src/fmt_fm_algorithm.h \
    firmware/src/fmt_fm_algorithm.c \
    firmware/src/fmt_fm_ratio.h \
    firmware/src/fmt_fm_ratio.c \
    firmware/src/fmt_fm_controls.h \
    firmware/src/fmt_fm_controls.c \
    firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_engine_pitch.c \
    firmware/gen/fmt_fm_sine_q15.inc
printf '%s\n' 'WP5_5_D053_PRODUCTION_ABSENT_OK=1 feedback_source=absent d049_delta=0 d050_delta=0 d051_delta=0 d052_delta=0 dinghy_delta=0'

python3 - <<'PY' | tee "$OUT/reference.log"
import hashlib
import math
import struct

MASK32 = 0xffffffff
INT32_MAX = 2147483647
INT32_MIN = -2147483648
Q15_MAX = 32767
Q14_MAX = 16383


def to_s32(v):
    v &= MASK32
    return v if v <= INT32_MAX else v - (1 << 32)


def pack_s32(v):
    return struct.pack('<i', v)


# Rebuild the accepted D049 sine source mathematically and prove model identity.
sine = [int(math.floor(math.sin((math.pi / 2.0) * i / 256.0) * 32767.0 + 0.5)) for i in range(257)]
table_payload = b''.join(struct.pack('<h', x) for x in sine)
assert hashlib.sha256(table_payload).hexdigest() == '6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68'


def sine_q15(phase):
    phase &= MASK32
    quadrant = phase >> 30
    quarter_offset = phase & 0x3fffffff
    coord = quarter_offset if (quadrant & 1) == 0 else 0x40000000 - quarter_offset
    index = coord >> 22
    if index == 256:
        magnitude = sine[256]
    else:
        frac = coord & 0x003fffff
        lo = sine[index]
        hi = sine[index + 1]
        magnitude = lo + (((hi - lo) * frac + 0x00200000) >> 22)
    return -magnitude if (quadrant & 2) else magnitude


def apply_level(sample, level):
    level = min(max(int(level), 0), Q15_MAX)
    if sample == 0 or level == 0:
        return 0
    if level == Q15_MAX:
        return sample
    magnitude = -sample if sample < 0 else sample
    scaled = (magnitude * level + 16384) >> 15
    return -scaled if sample < 0 else scaled


def op_render(phase, increment, phase_modulation, level):
    sample_phase = (phase + (phase_modulation & MASK32)) & MASK32
    sample = apply_level(sine_q15(sample_phase), level)
    return sample, (phase + increment) & MASK32


def edge_mod(sample, depth):
    depth = min(max(int(depth), 0), Q14_MAX)
    return to_s32(sample * depth * 4)


def phase_add(a, b):
    return to_s32((a & MASK32) + (b & MASK32))


def normalize(value, shift):
    if value < 0:
        return -((-value) >> shift)
    return value >> shift


def d050_render(phases, algorithm, increments, levels, depth):
    algorithm = algorithm if 0 <= algorithm < 4 else 0
    depth = min(depth, Q14_MAX)

    def render(index, modulation):
        sample, phase = op_render(phases[index], increments[index], modulation, levels[index])
        phases[index] = phase
        return sample

    if algorithm == 0:
        s4 = render(3, 0)
        s3 = render(2, edge_mod(s4, depth))
        s2 = render(1, edge_mod(s3, depth))
        return render(0, edge_mod(s2, depth))
    if algorithm == 1:
        s4 = render(3, 0)
        s3 = render(2, 0)
        s2 = render(1, 0)
        modulation = phase_add(edge_mod(s4, depth), edge_mod(s3, depth))
        modulation = phase_add(modulation, edge_mod(s2, depth))
        return render(0, modulation)
    if algorithm == 2:
        s4 = render(3, 0)
        s3 = render(2, edge_mod(s4, depth))
        s2 = render(1, 0)
        s1 = render(0, edge_mod(s2, depth))
        return normalize(s1 + s3, 1)
    s4 = render(3, 0)
    s3 = render(2, 0)
    s2 = render(1, 0)
    s1 = render(0, 0)
    return normalize(s1 + s2 + s3 + s4, 2)


# Re-prove the D050 model before adding test-only feedback composition.
initial = [0x10203040, 0x50607080, 0x90A0B0C0, 0xD0E0F000]
increments = [0x01234567, 0x02345678, 0x03456789, 0x0456789A]
levels = [32767, 28000, 22000, 18000]
depth = 7000
expected_d050 = [
    '25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd',
    '8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653',
    'c4c31cabf4b8e121591c4e0923aee67523eaf7d5b60b60ca1a545c13fc2269',
    'd316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882',
]
for algorithm in range(4):
    phases = initial[:]
    payload = bytearray()
    for _ in range(4096):
        payload += pack_s32(d050_render(phases, algorithm, increments, levels, depth))
    assert hashlib.sha256(payload).hexdigest() == expected_d050[algorithm]


def feedback_sanitize(control):
    control = int(control)
    if control > 255:
        return 255
    if control < 0:
        return 0
    return control


def feedback_depth_q14(control):
    value = feedback_sanitize(control)
    return (value << 6) | (value >> 2)


def feedback_phase(history, control):
    return history * feedback_depth_q14(control) * 4


# Exact control mapping and D052 numeric-equivalence proof.
feedback_depths = [feedback_depth_q14(v) for v in range(256)]
assert feedback_depths[0] == 0
assert feedback_depths[1] == 64
assert feedback_depths[64] == 4112
assert feedback_depths[128] == 8224
assert feedback_depths[192] == 12336
assert feedback_depths[254] == 16319
assert feedback_depths[255] == 16383
for malformed in (256, 257, 65535, 0xffffffff):
    assert feedback_depth_q14(malformed) == 16383
control_payload = b''.join(struct.pack('<H', x) for x in feedback_depths)
assert hashlib.sha256(control_payload).hexdigest() == 'cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7'

# Exhaust the complete signed-16 history domain x all 256 controls.
surface_hash = hashlib.sha256()
minimum = 0
maximum = 0
for history in range(-32768, 32768):
    for mapped_depth in feedback_depths:
        value = history * mapped_depth * 4
        assert INT32_MIN <= value <= INT32_MAX
        minimum = min(minimum, value)
        maximum = max(maximum, value)
        surface_hash.update(pack_s32(value))
assert minimum == -2147352576
assert maximum == 2147287044
assert minimum - INT32_MIN == 131072
assert INT32_MAX - maximum == 196603
assert surface_hash.hexdigest() == '6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af'


def feedback_op4_render(phase, history, increment, level, control):
    modulation = feedback_phase(history, control)
    sample, phase = op_render(phase, increment, modulation, level)
    assert -32767 <= sample <= 32767
    return sample, phase, sample


# Canonical isolated OP4 one-sample fixture.
phase = 0xD0E0F000
history = 0
payload = bytearray()
first = []
causal = []
for i in range(4096):
    modulation = feedback_phase(history, 192)
    before = history
    sample, phase, history = feedback_op4_render(phase, history, 0x0456789A, 18000, 192)
    if i < 6:
        causal.append((before, modulation, sample))
    if i < 12:
        first.append(sample)
    payload += pack_s32(sample)
assert causal == [
    (0, 0, -16477),
    (-16477, -813041088, -14123),
    (-14123, -696885312, -16632),
    (-16632, -820689408, -16072),
    (-16072, -793056768, -17085),
    (-17085, -843042240, -17264),
]
assert first == [-16477, -14123, -16632, -16072, -17085, -17264, -17664, -17880, -17994, -17951, -17698, -17154]
assert phase == 0x386A9000
assert history == 13151
assert hashlib.sha256(payload).hexdigest() == '6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49'


def d053_test_composition(phases, history_box, algorithm, increments, levels, depth, feedback_control):
    algorithm = algorithm if 0 <= algorithm < 4 else 0
    depth = min(depth, Q14_MAX)

    # D053 changes only the first OP4 render in the test-only composition model.
    s4, phase4, history = feedback_op4_render(
        phases[3], history_box[0], increments[3], levels[3], feedback_control)
    phases[3] = phase4
    history_box[0] = history

    def render(index, modulation):
        sample, phase = op_render(phases[index], increments[index], modulation, levels[index])
        phases[index] = phase
        return sample

    if algorithm == 0:
        s3 = render(2, edge_mod(s4, depth))
        s2 = render(1, edge_mod(s3, depth))
        return render(0, edge_mod(s2, depth))
    if algorithm == 1:
        s3 = render(2, 0)
        s2 = render(1, 0)
        modulation = phase_add(edge_mod(s4, depth), edge_mod(s3, depth))
        modulation = phase_add(modulation, edge_mod(s2, depth))
        return render(0, modulation)
    if algorithm == 2:
        s3 = render(2, edge_mod(s4, depth))
        s2 = render(1, 0)
        s1 = render(0, edge_mod(s2, depth))
        return normalize(s1 + s3, 1)
    s3 = render(2, 0)
    s2 = render(1, 0)
    s1 = render(0, 0)
    return normalize(s1 + s2 + s3 + s4, 2)


expected_feedback_hashes = [
    '25f86f13f10413f5be3ff4b50160cafeac97d758933fb5d660225525a4eb9077',
    '02e8b4556fb26a4134a6c257f1030b03c6808b8da2e597351fd19ef6ba9920fa',
    'f23d8e23e09ee1f4c4fd5d7247c125627b0624ac98fceb9bf98c8b1a6ff55a93',
    '60ff90a19ccfc517213aa4fcce1a209240d546bbd3db5eecde9c695612501050',
]
expected_first = [
    [31709, 32339, 32237, 32501, 32563, 32671, 32732, 32764, 32759, 32710, 32608, 32444],
    [13322, 14313, 9242, 7971, 4630, 2391, -101, -2283, -4239, -5886, -7139, -7896],
    [19166, 17261, 17505, 16364, 15924, 15103, 14378, 13571, 12724, 11818, 10843, 9786],
    [3298, 3541, 2564, 2352, 1749, 1357, 917, 531, 183, -113, -341, -480],
]
for algorithm in range(4):
    phases = initial[:]
    history_box = [0]
    payload = bytearray()
    first = []
    for i in range(4096):
        sample = d053_test_composition(phases, history_box, algorithm, increments, levels, depth, 192)
        if i < 12:
            first.append(sample)
        payload += pack_s32(sample)
    assert first == expected_first[algorithm]
    assert hashlib.sha256(payload).hexdigest() == expected_feedback_hashes[algorithm]
    assert phases == [0x4476A040, 0x95C7F080, 0xE71940C0, 0x386A9000]
    assert history_box[0] == 13151

# Algorithm-switch continuity: do not clear feedback history at topology changes.
phases = initial[:]
history_box = [0]
payload = bytearray()
for i in range(4096):
    algorithm = i // 1024
    payload += pack_s32(d053_test_composition(phases, history_box, algorithm, increments, levels, depth, 192))
assert hashlib.sha256(payload).hexdigest() == '5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67'
assert history_box[0] == 13151

# Prove that reset-on-algorithm-switch is observably different and therefore forbidden.
phases = initial[:]
history_box = [0]
payload = bytearray()
previous = None
for i in range(4096):
    algorithm = i // 1024
    if previous is not None and algorithm != previous:
        history_box[0] = 0
    payload += pack_s32(d053_test_composition(phases, history_box, algorithm, increments, levels, depth, 192))
    previous = algorithm
assert hashlib.sha256(payload).hexdigest() == '51f0959b5e6840ed9dacc2f56eb9c2ea8c501ef5cb32080d0290a181e1e235aa'

# D051/D052 extreme 65,536-sample stress for every D050 topology.
extreme_increments = [407681904, 1223045711, 2147483647, 2147483647]
extreme_levels = [8224, 16448, 24672, 32767]
extreme_hashes = [
    'd91af85e62f5791881a7b2b3bc560228ab820268c3fe7ca0762f9ae6d6922c0d',
    '1037755115ed94cc460da484851a8f86fc49f8d19b6b9e76f55fa00c9bd65406',
    '9da3125c952f75280296266236fd7a61afe93f6816354ce145d4d433fc0c6e46',
    'ed1c7ae3b47c83773e455757267da3692454d988e6c9e01dcba09acd02ea8d7a',
]
for algorithm in range(4):
    phases = [0, 0, 0, 0]
    history_box = [0]
    digest = hashlib.sha256()
    for _ in range(65536):
        sample = d053_test_composition(
            phases, history_box, algorithm, extreme_increments, extreme_levels, 16383, 255)
        assert -32767 <= sample <= 32767
        digest.update(pack_s32(sample))
    assert digest.hexdigest() == extreme_hashes[algorithm]
    assert phases == [0xBB700000, 0x324F0000, 0xFFFF0000, 0xFFFF0000]
    assert history_box[0] == 2517

print('WP5_5_D053_CONTROL_MAP_OK=1 controls=256 q14=0..16383 same_numeric_map_as_d052_index=1 sha256=cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7')
print('WP5_5_D053_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216 min=-2147352576 max=2147287044 int32_negative_margin=131072 int32_positive_margin=196603 sha256=6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af')
print('WP5_5_D053_OP4_FIXTURE_OK=1 delay_samples=1 renders=4096 final_history=13151 sha256=6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49')
print('WP5_5_D053_ALGORITHM_FIXTURES_OK=1 algorithms=4 renders_each=4096 history=continuous op4_first=1')
print('WP5_5_D053_SWITCH_CONTINUITY_OK=1 renders=4096 switches=3 reset_on_switch=0 sha256=5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67')
print('WP5_5_D053_EXTREME_STRESS_OK=1 algorithms=4 renders_each=65536 feedback=255 index=16383 output=q15_bounded final_history=2517')
PY

grep -F 'WP5_5_D053_CONTROL_MAP_OK=1 controls=256' "$OUT/reference.log"
grep -F 'WP5_5_D053_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216' "$OUT/reference.log"
grep -F 'WP5_5_D053_OP4_FIXTURE_OK=1 delay_samples=1 renders=4096 final_history=13151' "$OUT/reference.log"
grep -F 'WP5_5_D053_ALGORITHM_FIXTURES_OK=1 algorithms=4 renders_each=4096 history=continuous op4_first=1' "$OUT/reference.log"
grep -F 'WP5_5_D053_SWITCH_CONTINUITY_OK=1 renders=4096 switches=3 reset_on_switch=0' "$OUT/reference.log"
grep -F 'WP5_5_D053_EXTREME_STRESS_OK=1 algorithms=4 renders_each=65536 feedback=255 index=16383 output=q15_bounded final_history=2517' "$OUT/reference.log"

# Re-prove D052, which recursively re-proves D051/D050/D049/WP5.0 host behavior.
sh tests/run_wp5_4_fm_controls_tests.sh | tee "$OUT/d052_inherited.log"
grep -F 'WP5_4_FM_CONTROLS_HOST_GATE_OK=1 d052=exact exhaustive=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/d052_inherited.log"

printf '%s\n' 'WP5_5_D053_CONTRACT_FREEZE_OK=1 production=absent topology=op4_self delay=one_sample state_bytes=2 control=byte_q14 history_surface=exhaustive switch_history=continuous extreme_stress=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
