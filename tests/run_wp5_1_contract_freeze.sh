#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

CONTRACT=docs/WP5_1_FM_OPERATOR_CONTRACT.md
BASELINE=6c0add8828474e9308710e02957231e7225eb121
EXPECTED_WAV=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
EXPECTED_TABLE=6062d337f69ac6ae99b7da9cdf95f87aea1ee04a9110f26839f0a85f74efff68
EXPECTED_LONG=499f9d5bbb14c5a4016b7fbd8dbfa6784df88cf9cb673695eaaf64d4b4710e66

# D049 must be frozen before production FM operator source exists.
test -f "$CONTRACT"
grep -F 'FROZEN BY D049 BEFORE FM OPERATOR PRODUCTION SOURCE' "$CONTRACT"
test ! -e firmware/src/fmt_fm_operator.c
test ! -e firmware/src/fmt_fm_operator.h
if grep -F 'fmt_fm_operator' firmware/src/dinghy.c; then
    echo 'FM operator entered dinghy.c before the D049 implementation gate' >&2
    exit 1
fi

# The contract commit itself may not change production firmware/generated data.
git diff --exit-code "$BASELINE" -- firmware/src firmware/gen
printf '%s\n' 'WP5_1_D049_PRODUCTION_ABSENT_OK=1 firmware_delta=0 operator_source=absent dinghy_include=absent'

python3 - "$EXPECTED_TABLE" "$EXPECTED_LONG" <<'PY'
import hashlib
import math
import struct
import sys

expected_table, expected_long = sys.argv[1:]

TABLE = [
    int(math.floor(math.sin((math.pi / 2.0) * i / 256.0) * 32767.0 + 0.5))
    for i in range(257)
]
raw_table = b''.join(struct.pack('<h', x) for x in TABLE)
table_sha = hashlib.sha256(raw_table).hexdigest()
assert table_sha == expected_table, (table_sha, expected_table)
assert len(TABLE) == 257
assert TABLE[0] == 0
assert TABLE[1] == 201
assert TABLE[2] == 402
assert TABLE[128] == 23170
assert TABLE[255] == 32766
assert TABLE[256] == 32767


def sine_q15(phase):
    phase &= 0xFFFFFFFF
    quadrant = phase >> 30
    quarter_offset = phase & 0x3FFFFFFF
    coord = 0x40000000 - quarter_offset if quadrant & 1 else quarter_offset
    index = coord >> 22
    if index == 256:
        magnitude = TABLE[256]
    else:
        frac = coord & 0x003FFFFF
        lo = TABLE[index]
        hi = TABLE[index + 1]
        magnitude = lo + (((hi - lo) * frac + 0x00200000) >> 22)
    return -magnitude if quadrant >= 2 else magnitude


def scale_q15(sample, level):
    level = min(max(int(level), 0), 32767)
    if level == 0 or sample == 0:
        return 0
    if level == 32767:
        return sample
    magnitude = -sample if sample < 0 else sample
    scaled = (magnitude * level + 16384) >> 15
    return -scaled if sample < 0 else scaled


def render(phase, increment, modulation, level):
    sample_phase = (phase + (modulation & 0xFFFFFFFF)) & 0xFFFFFFFF
    sample = scale_q15(sine_q15(sample_phase), level)
    return (phase + increment) & 0xFFFFFFFF, sample

anchors = {
    0x00000000: 0,
    0x08000000: 6393,
    0x10000000: 12539,
    0x20000000: 23170,
    0x30000000: 30273,
    0x40000000: 32767,
    0x60000000: 23170,
    0x7FFFFFFF: 0,
    0x80000000: 0,
    0xA0000000: -23170,
    0xC0000000: -32767,
    0xE0000000: -23170,
    0xFFFFFFFF: 0,
}
for phase, expected in anchors.items():
    assert sine_q15(phase) == expected, (hex(phase), sine_q15(phase), expected)

interpolation = {
    0x01234567: 915,
    0x05555555: 4277,
    0x15555555: 16383,
    0x23456789: 24953,
    0x55555555: 28377,
    0x76543210: 7705,
    0x87654321: -5915,
    0xABCDEF01: -28824,
    0xFEDCBA98: -915,
}
for phase, expected in interpolation.items():
    assert sine_q15(phase) == expected, (hex(phase), sine_q15(phase), expected)

# Exact half-cycle sign symmetry over a broad deterministic sweep.
for i in range(65536):
    phase = (i * 0x9E3779B1) & 0xFFFFFFFF
    assert sine_q15((phase + 0x80000000) & 0xFFFFFFFF) == -sine_q15(phase)

level_vectors = {
    0: 0,
    1: 1,
    8192: 8192,
    16384: 16384,
    24576: 24575,
    32766: 32765,
    32767: 32767,
    65535: 32767,
}
for level, expected in level_vectors.items():
    assert scale_q15(32767, level) == expected, (level, scale_q15(32767, level), expected)
    assert scale_q15(-32767, level) == -expected

mod_vectors = [
    (0x10000000, +0x10000000, 23170),
    (0x10000000, -0x10000000, 0),
    (0x70000000, +0x20000000, -12539),
    (0x00000000, +0x40000000, 32767),
    (0x00000000, -0x40000000, -32767),
    (0x00000000, -0x80000000, 0),
    (0x00000000, +0x7FFFFFFF, 0),
]
for phase, modulation, expected in mod_vectors:
    new_phase, sample = render(phase, 0, modulation, 32767)
    assert new_phase == phase
    assert sample == expected, (hex(phase), modulation, sample, expected)

# Sample current phase before phase advance.
phase = 0
quarter = []
post = []
for _ in range(8):
    phase, sample = render(phase, 0x40000000, 0, 32767)
    quarter.append(sample)
    post.append(phase)
assert quarter == [0, 32767, 0, -32767, 0, 32767, 0, -32767]
assert post[:4] == [0x40000000, 0x80000000, 0xC0000000, 0]

# Explicit unsigned wrap and zero-increment modulation independence.
phase, sample = render(0xFFFFFFF0, 0x30, 0, 32767)
assert phase == 0x20
held_phase, held_a = render(0x12345678, 0, 0, 32767)
held_phase_2, held_b = render(0x12345678, 0, 0x10000000, 32767)
assert held_phase == held_phase_2 == 0x12345678
assert held_a != held_b

# Frozen 4096-render stress vector.
phase = 0x13579BDF
samples = []
for i in range(4096):
    mod_u32 = (i * 0x1F123BB5) & 0xFFFFFFFF
    modulation = mod_u32 if mod_u32 < 0x80000000 else mod_u32 - 0x100000000
    level = (i * 257) & 0x7FFF
    phase, sample = render(phase, 42852281, modulation, level)
    samples.append(sample)
raw = b''.join(struct.pack('<i', x) for x in samples)
long_sha = hashlib.sha256(raw).hexdigest()
assert long_sha == expected_long, (long_sha, expected_long)
assert phase == 0xF1532BDF, hex(phase)
assert samples[:12] == [0, 248, 437, 146, -609, -1277, -1165, -56, 1466, 2310, 1650, -361]

print(f'WP5_1_D049_REFERENCE_OK=1 table_entries=257 table_sha256={table_sha} long_sha256={long_sha} final_phase=0x{phase:08x} symmetry_probes=65536')
PY

# Contract freeze must not alter the running app audio.
sh tests/run_host.sh >/dev/null
WAV_SHA="$(sha256sum build/host/fm1tracker.wav | awk '{print $1}')"
test "$WAV_SHA" = "$EXPECTED_WAV"
printf 'WP5_1_D049_BASELINE_WAV_OK=1 sha256=%s\n' "$WAV_SHA"
printf '%s\n' 'WP5_1_D049_CONTRACT_FREEZE_OK=1 production=absent source_delta=0 reference=pass full_app_wav=wp5_0_identical hardware_margin_claim=none'
