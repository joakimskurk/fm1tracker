#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_3_contract
BASE=826a7dca7394d6e2c13f79fafa97bd05fb1986c8
mkdir -p "$OUT"

# D051 must exist before any production ratio source.
grep -F 'Status: **FROZEN BY D051 BEFORE FM RATIO PRODUCTION SOURCE**' docs/WP5_3_FM_RATIO_CONTRACT.md
test ! -e firmware/src/fmt_fm_ratio.h
test ! -e firmware/src/fmt_fm_ratio.c
! grep -R -F 'fmt_fm_ratio' firmware/src firmware/gen

# D049/D050 and the running application must remain untouched at contract freeze.
git diff --exit-code "$BASE" -- \
    firmware/src/dinghy.c \
    firmware/src/fmt_engine_pitch.h \
    firmware/src/fmt_engine_pitch.c \
    firmware/src/fmt_fm_operator.h \
    firmware/src/fmt_fm_operator.c \
    firmware/src/fmt_fm_algorithm.h \
    firmware/src/fmt_fm_algorithm.c \
    firmware/gen/fmt_fm_sine_q15.inc
printf '%s\n' 'WP5_3_D051_PRODUCTION_ABSENT_OK=1 ratio_source=absent d038_delta=0 d049_delta=0 d050_delta=0 dinghy_delta=0'

python3 - <<'PY' | tee "$OUT/reference.log"
import hashlib
import struct

PITCH_MAX_Q7 = 16383
BASE_MAX = 1293741834
CEILING = 0x7FFFFFFF
UNITY_ID = 1
RATIO_Q8 = [
    128, 256, 320, 362,
    384, 512, 640, 768,
    896, 1024, 1280, 1536,
    1792, 2048, 3072, 4096,
]
MAX_BASE = [
    1293741834, 1293741834, 1293741834, 1293741834,
    1293741834, 1073741823, 858993458, 715827882,
    613566756, 536870911, 429496729, 357913941,
    306783378, 268435455, 178956970, 134217727,
]
C9 = [
    815363807, 863847862, 915214929, 969636441,
    1027294024, 1088380105, 1153098554, 1221665363,
    1294309365, 1371273005, 1452813141, 1539201906,
]

assert len(RATIO_Q8) == 16
assert len(MAX_BASE) == 16
assert hashlib.sha256(b''.join(struct.pack('<H', x) for x in RATIO_Q8)).hexdigest() == \
    '5d98aebf644051ac555efcf1ceabf4015f26aaa2884722b515c73b5518659a60'
assert hashlib.sha256(b''.join(struct.pack('<I', x) for x in MAX_BASE)).hexdigest() == \
    'c39e8210aadfc1bd347fbb86a7c2058ae8681c123892044c316cf48a55a15255'
combined = b''.join(struct.pack('<H', x) for x in RATIO_Q8) + \
           b''.join(struct.pack('<I', x) for x in MAX_BASE)
assert hashlib.sha256(combined).hexdigest() == \
    '17d978a26a632bf5f28dc7efcc519f84dd270a2a1c19baac9739dd8e8605f28c'

# Reproduce D038 exactly.
def note_inc(note):
    return C9[note % 12] >> (10 - note // 12)

def interp(lo, hi, frac_q7):
    if frac_q7 == 0 or hi <= lo:
        return lo
    diff = hi - lo
    q = diff >> 7
    r = diff & 127
    return lo + q * frac_q7 + ((r * frac_q7 + 64) >> 7)

def pitch_inc_q7(pitch_q7):
    pitch = min(max(int(pitch_q7), 0), PITCH_MAX_Q7)
    note = pitch >> 7
    frac = pitch & 127
    lo = note_inc(note)
    if frac == 0:
        return lo
    return interp(lo, note_inc(note + 1), frac)

assert pitch_inc_q7(PITCH_MAX_Q7) == BASE_MAX
assert [pitch_inc_q7(x) for x in (0, 128, 7680, 15360, 16000, 16383)] == [
    796253, 843601, 25480118, 815363807, 1088380105, 1293741834,
]

# D051 reference mapping.
def sanitize_ratio(ratio_id):
    ratio_id = int(ratio_id)
    return ratio_id if 0 <= ratio_id < 16 else UNITY_ID

def apply_ratio(base_increment, ratio_id):
    selected = sanitize_ratio(ratio_id)
    base = min(max(int(base_increment), 0), BASE_MAX)
    if base > MAX_BASE[selected]:
        return CEILING
    ratio = RATIO_Q8[selected]
    whole = base >> 8
    remainder = base & 255
    scaled = whole * ratio + ((remainder * ratio + 128) >> 8)
    assert 0 <= scaled <= CEILING
    return scaled

def map4(base_increment, ratio_ids):
    ids = [UNITY_ID] * 4 if ratio_ids is None else list(ratio_ids)
    assert len(ids) == 4
    return [apply_ratio(base_increment, ids[i]) for i in range(4)]

# Exact table-derived boundaries.
for i, ratio in enumerate(RATIO_Q8):
    offline = min(BASE_MAX, (CEILING * 256 + 127) // ratio)
    assert offline == MAX_BASE[i], (i, ratio, offline, MAX_BASE[i])

# Low/mid/high anchors from D051.
selected_ids = [0, 1, 3, 5, 9, 13, 15]
expected = {
    0:     [398127, 796253, 1125952, 1592506, 3185012, 6370024, 12740048],
    128:   [421801, 843601, 1192905, 1687202, 3374404, 6748808, 13497616],
    7680:  [12740059, 25480118, 36030479, 50960236, 101920472, 203840944, 407681888],
    15360: [407681904, 815363807, 1152975383, 1630727614, CEILING, CEILING, CEILING],
    16000: [544190053, 1088380105, 1539037492, CEILING, CEILING, CEILING, CEILING],
    16383: [646870917, 1293741834, 1829431812, CEILING, CEILING, CEILING, CEILING],
}
for pitch, row in expected.items():
    base = pitch_inc_q7(pitch)
    actual = [apply_ratio(base, ratio_id) for ratio_id in selected_ids]
    assert actual == row, (pitch, actual, row)

# Four-output anchors.
assert map4(pitch_inc_q7(7680), [1, 5, 3, 13]) == [
    25480118, 50960236, 36030479, 203840944,
]
assert map4(pitch_inc_q7(15360), [0, 4, 9, 15]) == [
    407681904, 1223045711, CEILING, CEILING,
]
assert map4(pitch_inc_q7(16383), [1, 3, 5, 15]) == [
    1293741834, 1829431812, CEILING, CEILING,
]
assert map4(pitch_inc_q7(7680), None) == [25480118] * 4

# Invalid IDs are exact UNITY aliases.
for invalid in (-1, 16, 17, 255, 65535, 0xFFFFFFFF):
    for base in (0, 796253, 25480118, BASE_MAX, 0xFFFFFFFF):
        assert apply_ratio(base, invalid) == apply_ratio(base, UNITY_ID)

# Malformed base sanitizes before ratio mapping.
assert apply_ratio(0xFFFFFFFF, UNITY_ID) == BASE_MAX
assert apply_ratio(0xFFFFFFFF, 0) == 646870917
assert apply_ratio(0xFFFFFFFF, 15) == CEILING

# First legal Q7 pitch that clamps for every high ratio.
first_clip_expected = {
    5: 15970,
    6: 15476,
    7: 15071,
    8: 14730,
    9: 14434,
    10: 13940,
    11: 13535,
    12: 13194,
    13: 12898,
    14: 11999,
    15: 11362,
}
for ratio_id in range(5):
    assert all(pitch_inc_q7(p) <= MAX_BASE[ratio_id] for p in range(PITCH_MAX_Q7 + 1))
for ratio_id, expected_pitch in first_clip_expected.items():
    first = next(p for p in range(PITCH_MAX_Q7 + 1)
                 if pitch_inc_q7(p) > MAX_BASE[ratio_id])
    assert first == expected_pitch, (ratio_id, first, expected_pitch)
    assert apply_ratio(pitch_inc_q7(first - 1), ratio_id) < CEILING
    assert apply_ratio(pitch_inc_q7(first), ratio_id) == CEILING

# Exhaustive legal pitch x ratio reference.
payload = bytearray()
clipped = 0
for pitch in range(PITCH_MAX_Q7 + 1):
    base = pitch_inc_q7(pitch)
    for ratio_id in range(16):
        out = apply_ratio(base, ratio_id)
        payload += struct.pack('<I', out)
        if out == CEILING and base > MAX_BASE[ratio_id]:
            clipped += 1

assert len(payload) == 1048576
assert clipped == 27615
exhaustive_sha = hashlib.sha256(payload).hexdigest()
assert exhaustive_sha == '5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c'

print('WP5_3_D051_RATIO_TABLE_OK=1 ids=16 unity=1 q8_sha=5d98aebf644051ac555efcf1ceabf4015f26aaa2884722b515c73b5518659a60 boundaries_sha=c39e8210aadfc1bd347fbb86a7c2058ae8681c123892044c316cf48a55a15255')
print('WP5_3_D051_ARITHMETIC_OK=1 base_max=1293741834 ceiling=2147483647 q8_round=half_up runtime_div=0 runtime_int64=0')
print('WP5_3_D051_CLAMP_OK=1 ratios_never_clip=0..4 high_ratio_first_clip=exact malformed_base=canonical_max invalid_ratio=unity')
print('WP5_3_D051_MAP4_OK=1 outputs=4 state_bytes=0 null_ratio_ids=unity')
print(f'WP5_3_D051_EXHAUSTIVE_OK=1 pitches=16384 ratios=16 outputs=262144 bytes={len(payload)} clipped={clipped} sha256={exhaustive_sha}')
PY

grep -F 'WP5_3_D051_RATIO_TABLE_OK=1 ids=16 unity=1' "$OUT/reference.log"
grep -F 'WP5_3_D051_ARITHMETIC_OK=1 base_max=1293741834 ceiling=2147483647 q8_round=half_up runtime_div=0 runtime_int64=0' "$OUT/reference.log"
grep -F 'WP5_3_D051_CLAMP_OK=1 ratios_never_clip=0..4 high_ratio_first_clip=exact malformed_base=canonical_max invalid_ratio=unity' "$OUT/reference.log"
grep -F 'WP5_3_D051_MAP4_OK=1 outputs=4 state_bytes=0 null_ratio_ids=unity' "$OUT/reference.log"
grep -F 'WP5_3_D051_EXHAUSTIVE_OK=1 pitches=16384 ratios=16 outputs=262144 bytes=1048576 clipped=27615 sha256=5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c' "$OUT/reference.log"

# Re-prove D050, D049 and the inherited WP5.0 host baseline.
sh tests/run_wp5_2_fm_algorithm_tests.sh | tee "$OUT/d050_inherited.log"
grep -F 'WP5_2_FM_ALGORITHM_HOST_GATE_OK=1 d050=exact algorithms=4 hashes=exact stress=pass d049=exact full_app_wav=wp5_0_identical' "$OUT/d050_inherited.log"

printf '%s\n' 'WP5_3_D051_CONTRACT_FREEZE_OK=1 production=absent ratios=16 exhaustive=pass d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none' | tee "$OUT/summary.txt"
