#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_4_contract
BASE=ed3a9765ba9e401f4d30f514a357dd0d592e6a7e
mkdir -p "$OUT"

# D052 must exist before any production control mapper source.
grep -F 'Status: **FROZEN BY D052 BEFORE FM CONTROL-MAPPING PRODUCTION SOURCE**' docs/WP5_4_FM_LEVEL_INDEX_CONTRACT.md
test ! -e firmware/src/fmt_fm_controls.h
test ! -e firmware/src/fmt_fm_controls.c
! grep -R -F 'fmt_fm_controls' firmware/src firmware/gen

# D049/D050/D051 and the running application must remain untouched at contract freeze.
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_fm_operator.h \
    firmware/src/fmt_fm_operator.c \
    firmware/src/fmt_fm_algorithm.h \
    firmware/src/fmt_fm_algorithm.c \
    firmware/src/fmt_fm_ratio.h \
    firmware/src/fmt_fm_ratio.c \
    firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_engine_pitch.c \
    firmware/gen/fmt_fm_sine_q15.inc
printf '%s\n' 'WP5_4_D052_PRODUCTION_ABSENT_OK=1 controls_source=absent d049_delta=0 d050_delta=0 d051_delta=0 dinghy_delta=0'

python3 - <<'PY' | tee "$OUT/reference.log"
import hashlib
import struct

CONTROL_MAX = 255
Q15_MAX = 32767
Q14_MAX = 16383
INT32_MAX = 2147483647

# D052 exact scalar mappings.
def sanitize(v):
    v = int(v)
    return CONTROL_MAX if v > CONTROL_MAX else (0 if v < 0 else v)

def level_q15(v):
    v = sanitize(v)
    return (v << 7) | (v >> 1)

def index_q14(v):
    v = sanitize(v)
    return (v << 6) | (v >> 2)

levels = [level_q15(v) for v in range(256)]
indices = [index_q14(v) for v in range(256)]

# LEVEL is exact nearest scaling to Q15 over all 256 inputs.
for v in range(256):
    assert levels[v] == (v * Q15_MAX + 127) // 255

# INDEX is monotonic, endpoint exact and <=1 Q14 from nearest ideal linear scaling.
assert indices[0] == 0 and indices[255] == Q14_MAX
assert all(indices[i] <= indices[i + 1] for i in range(255))
assert max(abs(indices[v] - ((v * Q14_MAX + 127) // 255)) for v in range(256)) <= 1

level_payload = b''.join(struct.pack('<H', x) for x in levels)
index_payload = b''.join(struct.pack('<H', x) for x in indices)
assert hashlib.sha256(level_payload).hexdigest() == 'a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815'
assert hashlib.sha256(index_payload).hexdigest() == 'cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7'
assert hashlib.sha256(level_payload + index_payload).hexdigest() == '26e55d6a5fcacb5f432ecc3620b41a1c18e23f277aea6a13cb72d988a1929b38'

assert [levels[v] for v in (0, 1, 32, 64, 96, 127, 128, 160, 192, 224, 254, 255)] == [
    0, 128, 4112, 8224, 12336, 16319, 16448, 20560, 24672, 28784, 32639, 32767,
]
assert [indices[v] for v in (0, 1, 3, 4, 32, 64, 96, 127, 128, 160, 192, 224, 254, 255)] == [
    0, 64, 192, 257, 2056, 4112, 6168, 8159, 8224, 10280, 12336, 14392, 16319, 16383,
]

# Malformed scalar values saturate, never wrap.
for malformed in (256, 257, 65535, 0xFFFFFFFF):
    assert level_q15(malformed) == Q15_MAX
    assert index_q14(malformed) == Q14_MAX

# D049 exact positive sine-peak level arithmetic.
def d049_peak(level):
    if level == 0:
        return 0
    if level >= Q15_MAX:
        return Q15_MAX
    return (Q15_MAX * level + 16384) >> 15

# Exhaust every legal LEVEL x INDEX pair through D049 peak -> D050 edge arithmetic.
edge_payload = bytearray()
max_edge = 0
for level_control in range(256):
    sample = d049_peak(levels[level_control])
    for index_control in range(256):
        edge = sample * indices[index_control] * 4
        assert 0 <= edge <= INT32_MAX
        max_edge = max(max_edge, edge)
        edge_payload += struct.pack('<I', edge)

assert len(edge_payload) == 262144
assert max_edge == 2147287044
assert INT32_MAX - max_edge == 196603
assert hashlib.sha256(edge_payload).hexdigest() == 'cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0'

# Frozen D050 role sets. Bits 0..3 are OP1..OP4.
roles = {
    'STACK4':    (0b0001, 0b1110),
    'FAN3':      (0b0001, 0b1110),
    'DUAL2':     (0b0101, 0b1010),
    'PARALLEL4': (0b1111, 0b0000),
}
for name, (carriers, modulators) in roles.items():
    assert carriers & modulators == 0, name
    assert carriers | modulators == 0b1111, name
assert roles['PARALLEL4'][1] == 0

# Representative D051 -> D052 prepared D050 input vectors.
vectors = [
    ([25480118, 50960236, 36030479, 203840944], [255, 192, 128, 64], 128),
    ([407681904, 1223045711, 2147483647, 2147483647], [64, 128, 192, 255], 255),
    ([796253, 1592506, 1125952, 6370024], [0, 64, 128, 255], 64),
]
prepared = bytearray()
expected = [
    ([32767, 24672, 16448, 8224], 8224),
    ([8224, 16448, 24672, 32767], 16383),
    ([0, 8224, 16448, 32767], 4112),
]
for (increments, control_levels, control_index), (expected_levels, expected_index) in zip(vectors, expected):
    mapped_levels = [level_q15(x) for x in control_levels]
    mapped_index = index_q14(control_index)
    assert mapped_levels == expected_levels
    assert mapped_index == expected_index
    for x in increments:
        prepared += struct.pack('<I', x)
    for x in mapped_levels:
        prepared += struct.pack('<H', x)
    prepared += struct.pack('<H', mapped_index)

assert len(prepared) == 78
assert hashlib.sha256(prepared).hexdigest() == 'e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c'

print('WP5_4_D052_LEVEL_MAP_OK=1 controls=256 q15=0..32767 exact_nearest=1 sha256=a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815')
print('WP5_4_D052_INDEX_MAP_OK=1 controls=256 q14=0..16383 bit_expand=8_to_14 max_linear_error=1 sha256=cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7')
print('WP5_4_D052_EDGE_SAFETY_OK=1 pairs=65536 max=2147287044 int32_margin=196603 sha256=cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0')
print('WP5_4_D052_TOPOLOGY_ROLES_OK=1 algorithms=4 carrier_modulator_overlap=0 global_index=1 per_edge_index=0')
print('WP5_4_D052_D051_COMPOSITION_OK=1 vectors=3 bytes=78 sha256=e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c')
PY

grep -F 'WP5_4_D052_LEVEL_MAP_OK=1 controls=256 q15=0..32767 exact_nearest=1' "$OUT/reference.log"
grep -F 'WP5_4_D052_INDEX_MAP_OK=1 controls=256 q14=0..16383 bit_expand=8_to_14 max_linear_error=1' "$OUT/reference.log"
grep -F 'WP5_4_D052_EDGE_SAFETY_OK=1 pairs=65536 max=2147287044 int32_margin=196603' "$OUT/reference.log"
grep -F 'WP5_4_D052_TOPOLOGY_ROLES_OK=1 algorithms=4 carrier_modulator_overlap=0 global_index=1 per_edge_index=0' "$OUT/reference.log"
grep -F 'WP5_4_D052_D051_COMPOSITION_OK=1 vectors=3 bytes=78 sha256=e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c' "$OUT/reference.log"

# Re-prove D051, which recursively re-proves D050/D049/WP5.0 host behavior.
sh tests/run_wp5_3_fm_ratio_tests.sh | tee "$OUT/d051_inherited.log"
grep -F 'WP5_3_FM_RATIO_HOST_GATE_OK=1 d051=exact exhaustive=exact d050=exact d049=exact full_app_wav=wp5_0_identical' "$OUT/d051_inherited.log"

printf '%s\n' 'WP5_4_D052_CONTRACT_FREEZE_OK=1 production=absent controls=5 level_map=exact index_map=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
