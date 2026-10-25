#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Source-absent D053 deterministic reference model.

This host-only model re-proves D049/D050 numerical identity, then freezes the
D053 OP4 one-sample feedback behavior before production feedback source exists.
"""

import hashlib
import math
import struct

MASK32 = 0xFFFFFFFF
INT32_MAX = 2147483647
INT32_MIN = -2147483648
Q15_MAX = 32767
Q14_MAX = 16383


def to_s32(value):
    value &= MASK32
    return value if value <= INT32_MAX else value - (1 << 32)


def pack_s32(value):
    return struct.pack("<i", value)


# D049 mathematical sine source.
SINE = [
    int(math.floor(math.sin((math.pi / 2.0) * i / 256.0) * 32767.0 + 0.5))
    for i in range(257)
]
TABLE_PAYLOAD = b"".join(struct.pack("<h", value) for value in SINE)
assert hashlib.sha256(TABLE_PAYLOAD).hexdigest() == (
    "6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68"
)


def sine_q15(phase):
    phase &= MASK32
    quadrant = phase >> 30
    quarter_offset = phase & 0x3FFFFFFF
    coord = quarter_offset if (quadrant & 1) == 0 else 0x40000000 - quarter_offset
    index = coord >> 22
    if index == 256:
        magnitude = SINE[256]
    else:
        frac = coord & 0x003FFFFF
        lo = SINE[index]
        hi = SINE[index + 1]
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
    return -((-value) >> shift) if value < 0 else value >> shift


def d050_render(phases, algorithm, increments, levels, depth):
    algorithm = algorithm if 0 <= algorithm < 4 else 0
    depth = min(depth, Q14_MAX)

    def render(index, modulation):
        sample, phase = op_render(
            phases[index], increments[index], modulation, levels[index]
        )
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


INITIAL = [0x10203040, 0x50607080, 0x90A0B0C0, 0xD0E0F000]
INCREMENTS = [0x01234567, 0x02345678, 0x03456789, 0x0456789A]
LEVELS = [32767, 28000, 22000, 18000]
DEPTH = 7000
EXPECTED_D050 = [
    "25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd",
    "8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653",
    "c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269",
    "d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882",
]

# Prove that this host model is still exactly D050 before adding D053 behavior.
for algorithm in range(4):
    phases = INITIAL[:]
    digest = hashlib.sha256()
    for _ in range(4096):
        digest.update(
            pack_s32(d050_render(phases, algorithm, INCREMENTS, LEVELS, DEPTH))
        )
    assert digest.hexdigest() == EXPECTED_D050[algorithm]


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


# Exact D053 control map. It is numerically identical to D052 INDEX mapping but
# remains a separate semantic control.
FEEDBACK_DEPTHS = [feedback_depth_q14(value) for value in range(256)]
assert FEEDBACK_DEPTHS[0] == 0
assert FEEDBACK_DEPTHS[1] == 64
assert FEEDBACK_DEPTHS[64] == 4112
assert FEEDBACK_DEPTHS[128] == 8224
assert FEEDBACK_DEPTHS[192] == 12336
assert FEEDBACK_DEPTHS[254] == 16319
assert FEEDBACK_DEPTHS[255] == 16383
for malformed in (256, 257, 65535, 0xFFFFFFFF):
    assert feedback_depth_q14(malformed) == 16383
CONTROL_PAYLOAD = b"".join(struct.pack("<H", value) for value in FEEDBACK_DEPTHS)
assert hashlib.sha256(CONTROL_PAYLOAD).hexdigest() == (
    "cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7"
)

# Exhaust the entire int16 history domain against all 256 legal FEEDBACK bytes.
surface_digest = hashlib.sha256()
minimum = 0
maximum = 0
for history in range(-32768, 32768):
    for mapped_depth in FEEDBACK_DEPTHS:
        value = history * mapped_depth * 4
        assert INT32_MIN <= value <= INT32_MAX
        minimum = min(minimum, value)
        maximum = max(maximum, value)
        surface_digest.update(pack_s32(value))
assert minimum == -2147352576
assert maximum == 2147287044
assert minimum - INT32_MIN == 131072
assert INT32_MAX - maximum == 196603
assert surface_digest.hexdigest() == (
    "6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af"
)


def feedback_op4_render(phase, history, increment, level, control):
    modulation = feedback_phase(history, control)
    sample, phase = op_render(phase, increment, modulation, level)
    assert -32767 <= sample <= 32767
    return sample, phase, sample


# Canonical isolated OP4 one-sample fixture.
phase = 0xD0E0F000
history = 0
op4_digest = hashlib.sha256()
first = []
causal = []
for index in range(4096):
    modulation = feedback_phase(history, 192)
    before = history
    sample, phase, history = feedback_op4_render(
        phase, history, 0x0456789A, 18000, 192
    )
    if index < 6:
        causal.append((before, modulation, sample))
    if index < 12:
        first.append(sample)
    op4_digest.update(pack_s32(sample))
assert causal == [
    (0, 0, -16477),
    (-16477, -813041088, -14123),
    (-14123, -696885312, -16632),
    (-16632, -820689408, -16072),
    (-16072, -793056768, -17085),
    (-17085, -843042240, -17264),
]
assert first == [
    -16477, -14123, -16632, -16072, -17085, -17264,
    -17664, -17880, -17994, -17951, -17698, -17154,
]
assert phase == 0x386A9000
assert history == 13151
assert op4_digest.hexdigest() == (
    "6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49"
)


def d053_test_composition(
    phases, history_box, algorithm, increments, levels, depth, feedback_control
):
    algorithm = algorithm if 0 <= algorithm < 4 else 0
    depth = min(depth, Q14_MAX)

    # Only the first OP4 render gains D053 one-sample feedback.
    s4, phase4, new_history = feedback_op4_render(
        phases[3], history_box[0], increments[3], levels[3], feedback_control
    )
    phases[3] = phase4
    history_box[0] = new_history

    def render(index, modulation):
        sample, phase_value = op_render(
            phases[index], increments[index], modulation, levels[index]
        )
        phases[index] = phase_value
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


EXPECTED_FEEDBACK_HASHES = [
    "25f86f13f10413f5be3ff4b50160cafeac97d758933fb5d660225525a4eb9077",
    "02e8b4556fb26a4134a6c257f1030b03c6808b8da2e597351fd19ef6ba9920fa",
    "f23d8e23e09ee1f4c4fd5d7247c125627b0624ac98fceb9bf98c8b1a6ff55a93",
    "60ff90a19ccfc517213aa4fcce1a209240d546bbd3db5eecde9c695612501050",
]
EXPECTED_FIRST = [
    [31709, 32339, 32237, 32501, 32563, 32671, 32732, 32764, 32759, 32710, 32608, 32444],
    [13322, 14313, 9242, 7971, 4630, 2391, -101, -2283, -4239, -5886, -7139, -7896],
    [19166, 17261, 17505, 16364, 15924, 15103, 14378, 13571, 12724, 11818, 10843, 9786],
    [3298, 3541, 2564, 2352, 1749, 1357, 917, 531, 183, -113, -341, -480],
]
for algorithm in range(4):
    phases = INITIAL[:]
    history_box = [0]
    digest = hashlib.sha256()
    first = []
    for index in range(4096):
        sample = d053_test_composition(
            phases, history_box, algorithm, INCREMENTS, LEVELS, DEPTH, 192
        )
        if index < 12:
            first.append(sample)
        digest.update(pack_s32(sample))
    assert first == EXPECTED_FIRST[algorithm]
    assert digest.hexdigest() == EXPECTED_FEEDBACK_HASHES[algorithm]
    assert phases == [0x4476A040, 0x95C7F080, 0xE71940C0, 0x386A9000]
    assert history_box[0] == 13151

# Algorithm changes retain feedback history.
phases = INITIAL[:]
history_box = [0]
switch_digest = hashlib.sha256()
for index in range(4096):
    algorithm = index // 1024
    switch_digest.update(
        pack_s32(
            d053_test_composition(
                phases, history_box, algorithm, INCREMENTS, LEVELS, DEPTH, 192
            )
        )
    )
assert switch_digest.hexdigest() == (
    "5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67"
)
assert history_box[0] == 13151

# Clearing history on algorithm switches is deliberately a different model.
phases = INITIAL[:]
history_box = [0]
wrong_switch_digest = hashlib.sha256()
previous_algorithm = None
for index in range(4096):
    algorithm = index // 1024
    if previous_algorithm is not None and algorithm != previous_algorithm:
        history_box[0] = 0
    wrong_switch_digest.update(
        pack_s32(
            d053_test_composition(
                phases, history_box, algorithm, INCREMENTS, LEVELS, DEPTH, 192
            )
        )
    )
    previous_algorithm = algorithm
assert wrong_switch_digest.hexdigest() == (
    "51f0959b5e6840ed9dacc2f56eb9c2ea8c501ef5cb32080d0290a181e1e235aa"
)

# D051/D052 high-increment, max-index, max-feedback stress.
EXTREME_INCREMENTS = [407681904, 1223045711, 2147483647, 2147483647]
EXTREME_LEVELS = [8224, 16448, 24672, 32767]
EXTREME_HASHES = [
    "d91af85e62f5791881a7b2b3bc560228ab820268c3fe7ca0762f9ae6d6922c0d",
    "1037755115ed94cc460da484851a8f86fc49f8d19b6b9e76f55fa00c9bd65406",
    "9da3125c952f75280296266236fd7a61afe93f6816354ce145d4d433fc0c6e46",
    "ed1c7ae3b47c83773e455757267da3692454d988e6c9e01dcba09acd02ea8d7a",
]
for algorithm in range(4):
    phases = [0, 0, 0, 0]
    history_box = [0]
    digest = hashlib.sha256()
    for _ in range(65536):
        sample = d053_test_composition(
            phases,
            history_box,
            algorithm,
            EXTREME_INCREMENTS,
            EXTREME_LEVELS,
            16383,
            255,
        )
        assert -32767 <= sample <= 32767
        digest.update(pack_s32(sample))
    assert digest.hexdigest() == EXTREME_HASHES[algorithm]
    assert phases == [0xBB700000, 0x324F0000, 0xFFFF0000, 0xFFFF0000]
    assert history_box[0] == 2517

print(
    "WP5_5_D053_D050_REFERENCE_OK=1 algorithms=4 renders_each=4096 "
    "hashes=exact"
)
print(
    "WP5_5_D053_CONTROL_MAP_OK=1 controls=256 q14=0..16383 "
    "same_numeric_map_as_d052_index=1 "
    "sha256=cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7"
)
print(
    "WP5_5_D053_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 "
    "pairs=16777216 min=-2147352576 max=2147287044 "
    "int32_negative_margin=131072 int32_positive_margin=196603 "
    "sha256=6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af"
)
print(
    "WP5_5_D053_OP4_FIXTURE_OK=1 delay_samples=1 renders=4096 "
    "final_history=13151 "
    "sha256=6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49"
)
print(
    "WP5_5_D053_ALGORITHM_FIXTURES_OK=1 algorithms=4 renders_each=4096 "
    "history=continuous op4_first=1"
)
print(
    "WP5_5_D053_SWITCH_CONTINUITY_OK=1 renders=4096 switches=3 "
    "reset_on_switch=0 "
    "sha256=5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67"
)
print(
    "WP5_5_D053_EXTREME_STRESS_OK=1 algorithms=4 renders_each=65536 "
    "feedback=255 index=16383 output=q15_bounded final_history=2517"
)
