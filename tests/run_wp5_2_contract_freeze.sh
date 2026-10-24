#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_2_contract
BASE=bc71289690c8f9bd36e9cbbb378e0df3de1b9f25
mkdir -p "$OUT"

# D050 must exist before production algorithm routing source.
grep -F 'Status: **FROZEN BY D050 BEFORE FM ALGORITHM PRODUCTION SOURCE**' docs/WP5_2_FM_ALGORITHM_CONTRACT.md
test ! -e firmware/src/fmt_fm_algorithm.h
test ! -e firmware/src/fmt_fm_algorithm.c
! grep -R -F 'fmt_fm_algorithm' firmware/src firmware/gen

# The accepted D049 operator baseline must be untouched by the contract-only checkpoint.
git diff --exit-code "$BASE" -- \
    firmware/src/fmt_fm_operator.h \
    firmware/src/fmt_fm_operator.c \
    firmware/gen/fmt_fm_sine_q15.inc \
    tools/gen_wp5_1_fm_sine.py
printf '%s\n' 'WP5_2_D050_PRODUCTION_ABSENT_OK=1 algorithm_source=absent d049_delta=0 dinghy_delta=0'

python3 - <<'PY' | tee build/wp5_2_contract/reference.log
import hashlib
import math
import struct

MASK = 0xFFFFFFFF
INT32_MAX = 0x7FFFFFFF
INT32_MIN = -0x80000000

STACK4 = 0
FAN3 = 1
DUAL2 = 2
PARALLEL4 = 3

TABLE = [
    int(math.floor(math.sin((math.pi / 2.0) * i / 256.0) * 32767.0 + 0.5))
    for i in range(257)
]
TABLE_PAYLOAD = b''.join(struct.pack('<h', value) for value in TABLE)
assert hashlib.sha256(TABLE_PAYLOAD).hexdigest() == '6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68'


def u32(value):
    return value & MASK


def bits_to_i32(value):
    value &= MASK
    if value <= INT32_MAX:
        return value
    return -1 - (MASK - value)


def sine_q15(phase):
    phase &= MASK
    quadrant = phase >> 30
    quarter_offset = phase & 0x3FFFFFFF
    if (quadrant & 1) == 0:
        coord = quarter_offset
    else:
        coord = 0x40000000 - quarter_offset
    index = coord >> 22
    if index == 256:
        magnitude = TABLE[256]
    else:
        frac = coord & 0x003FFFFF
        lo = TABLE[index]
        hi = TABLE[index + 1]
        magnitude = lo + (((hi - lo) * frac + 0x00200000) >> 22)
    return -magnitude if (quadrant & 2) else magnitude


def apply_level(sample, level):
    level = min(max(level, 0), 32767)
    if sample == 0 or level == 0:
        return 0
    if level == 32767:
        return sample
    scaled = (abs(sample) * level + 16384) >> 15
    return -scaled if sample < 0 else scaled


def op_render(phase, phase_increment, phase_modulation, level):
    sample_phase = u32(phase + (phase_modulation & MASK))
    sample = apply_level(sine_q15(sample_phase), level)
    return sample, u32(phase + phase_increment)


def clamp_depth(depth):
    return min(max(depth, 0), 16383)


def edge_mod(sample, depth):
    result = sample * clamp_depth(depth) * 4
    assert INT32_MIN <= result <= INT32_MAX
    return result


def mod_add(a, b):
    return bits_to_i32((a & MASK) + (b & MASK))


def normalize(sum_value, carriers):
    if carriers == 1:
        return sum_value
    shift = 1 if carriers == 2 else 2
    if sum_value < 0:
        return -((-sum_value) >> shift)
    return sum_value >> shift


def sanitize_algorithm(algorithm):
    return algorithm if 0 <= algorithm < 4 else STACK4


def render(phases, algorithm, increments, levels, depth):
    p = list(phases)
    algorithm = sanitize_algorithm(algorithm)
    depth = clamp_depth(depth)

    if algorithm == STACK4:
        s4, p[3] = op_render(p[3], increments[3], 0, levels[3])
        s3, p[2] = op_render(p[2], increments[2], edge_mod(s4, depth), levels[2])
        s2, p[1] = op_render(p[1], increments[1], edge_mod(s3, depth), levels[1])
        s1, p[0] = op_render(p[0], increments[0], edge_mod(s2, depth), levels[0])
        output = s1
    elif algorithm == FAN3:
        s4, p[3] = op_render(p[3], increments[3], 0, levels[3])
        s3, p[2] = op_render(p[2], increments[2], 0, levels[2])
        s2, p[1] = op_render(p[1], increments[1], 0, levels[1])
        modulation = mod_add(edge_mod(s4, depth), edge_mod(s3, depth))
        modulation = mod_add(modulation, edge_mod(s2, depth))
        s1, p[0] = op_render(p[0], increments[0], modulation, levels[0])
        output = s1
    elif algorithm == DUAL2:
        s4, p[3] = op_render(p[3], increments[3], 0, levels[3])
        s3, p[2] = op_render(p[2], increments[2], edge_mod(s4, depth), levels[2])
        s2, p[1] = op_render(p[1], increments[1], 0, levels[1])
        s1, p[0] = op_render(p[0], increments[0], edge_mod(s2, depth), levels[0])
        output = normalize(s1 + s3, 2)
    else:
        s4, p[3] = op_render(p[3], increments[3], 0, levels[3])
        s3, p[2] = op_render(p[2], increments[2], 0, levels[2])
        s2, p[1] = op_render(p[1], increments[1], 0, levels[1])
        s1, p[0] = op_render(p[0], increments[0], 0, levels[0])
        output = normalize(s1 + s2 + s3 + s4, 4)

    assert -32767 <= output <= 32767
    return output, p

# Frozen edge-depth anchors and exact sign mirrors.
anchors = {
    0: 0,
    1: 131068,
    4096: 536854528,
    8192: 1073709056,
    16383: 2147287044,
    65535: 2147287044,
}
for depth, expected in anchors.items():
    assert edge_mod(32767, depth) == expected
    assert edge_mod(-32767, depth) == -expected
assert clamp_depth(16384) == 16383

# Frozen modulo-phase accumulation anchors.
assert mod_add(INT32_MAX, 1) == INT32_MIN
assert mod_add(INT32_MIN, -1) == INT32_MAX
assert mod_add(0x70000000, 0x30000000) == -1610612736
assert mod_add(-0x70000000, -0x30000000) == 1610612736

# Frozen carrier normalization anchors.
for value, expected in [(-65534, -32767), (-3, -1), (-1, 0), (1, 0), (3, 1), (65534, 32767)]:
    assert normalize(value, 2) == expected
for value, expected in [(-131068, -32767), (-3, 0), (-1, 0), (1, 0), (3, 0), (131068, 32767)]:
    assert normalize(value, 4) == expected

INITIAL = [0x10203040, 0x50607080, 0x90A0B0C0, 0xD0E0F000]
INCREMENTS = [0x01234567, 0x02345678, 0x03456789, 0x0456789A]
LEVELS = [32767, 28000, 22000, 18000]
DEPTH = 7000

EXPECTED_FIRST12 = {
    STACK4: [31709, 32144, 32478, 32686, 32765, 32732, 32615, 32445, 32245, 32031, 31808, 31583],
    FAN3: [13322, 12487, 11858, 11434, 11223, 11214, 11408, 11786, 12337, 13043, 13887, 14841],
    DUAL2: [19166, 17927, 16580, 15148, 13662, 12161, 10686, 9284, 8003, 6886, 5971, 5288],
    PARALLEL4: [3298, 3168, 3078, 3028, 3017, 3043, 3106, 3203, 3331, 3488, 3670, 3874],
}

EXPECTED_HASH = {
    STACK4: '25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd',
    FAN3: '8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653',
    DUAL2: 'c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269',
    PARALLEL4: 'd316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882',
}

EXPECTED_FINAL = [0x4476A040, 0x95C7F080, 0xE71940C0, 0x386A9000]

for algorithm in range(4):
    phases = list(INITIAL)
    first = []
    payload = bytearray()
    for index in range(4096):
        output, phases = render(phases, algorithm, INCREMENTS, LEVELS, DEPTH)
        if index < 12:
            first.append(output)
        payload += struct.pack('<i', output)
    assert first == EXPECTED_FIRST12[algorithm]
    assert hashlib.sha256(payload).hexdigest() == EXPECTED_HASH[algorithm]
    assert phases == EXPECTED_FINAL

# Invalid algorithm IDs must be exact STACK4 aliases, including phase advancement.
for invalid in (4, 7, 255, 65535):
    a_out, a_phase = render(INITIAL, invalid, INCREMENTS, LEVELS, DEPTH)
    b_out, b_phase = render(INITIAL, STACK4, INCREMENTS, LEVELS, DEPTH)
    assert a_out == b_out and a_phase == b_phase

# Broad deterministic stress: 65,536 renders per algorithm.
for algorithm in range(4):
    phases = [0x00000000, 0x40000000, 0x80000000, 0xC0000000]
    for index in range(65536):
        increments = [
            u32(0x01020304 + index * 3),
            u32(0x02030405 + index * 5),
            u32(0x03040506 + index * 7),
            u32(0x04050607 + index * 11),
        ]
        levels = [
            (index * 257) & 0xFFFF,
            (index * 911) & 0xFFFF,
            (index * 1237) & 0xFFFF,
            (index * 1999) & 0xFFFF,
        ]
        depth = (index * 37) & 0xFFFF
        old = list(phases)
        output, phases = render(phases, algorithm, increments, levels, depth)
        assert -32767 <= output <= 32767
        assert phases == [u32(old[i] + increments[i]) for i in range(4)]

print('WP5_2_D050_REFERENCE_TOPOLOGIES_OK=1 algorithms=4 order=op4_op3_op2_op1 operators_per_sample=4')
print('WP5_2_D050_MODULATION_OK=1 depth=0..16383 accumulation=mod2p32 carriers=1_2_4 normalization=pow2_toward_zero')
print('WP5_2_D050_HASHES_OK=1 stack4=25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd fan3=8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653 dual2=c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269 parallel4=d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882 final_phases=4476a040,95c7f080,e71940c0,386a9000')
print('WP5_2_D050_STRESS_OK=1 algorithms=4 renders_each=65536 phase_advance=exact output_q15=bounded invalid_fallback=stack4')
PY

grep -F 'WP5_2_D050_REFERENCE_TOPOLOGIES_OK=1 algorithms=4 order=op4_op3_op2_op1 operators_per_sample=4' "$OUT/reference.log"
grep -F 'WP5_2_D050_MODULATION_OK=1 depth=0..16383 accumulation=mod2p32 carriers=1_2_4 normalization=pow2_toward_zero' "$OUT/reference.log"
grep -F 'WP5_2_D050_HASHES_OK=1' "$OUT/reference.log"
grep -F 'WP5_2_D050_STRESS_OK=1 algorithms=4 renders_each=65536 phase_advance=exact output_q15=bounded invalid_fallback=stack4' "$OUT/reference.log"

# Re-prove the accepted D049 implementation and exact full-app host baseline.
sh tests/run_wp5_1_fm_operator_tests.sh | tee "$OUT/d049_regression.log"
grep -F 'WP5_1_FM_OPERATOR_HOST_GATE_OK=1 d049=exact generator=exact long_vector=exact full_app_wav=wp5_0_identical router=pass representative=pass' "$OUT/d049_regression.log"

printf '%s\n' 'WP5_2_D050_CONTRACT_FREEZE_OK=1 production=absent d049=unchanged algorithms=4 reference=pass full_app_wav=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
