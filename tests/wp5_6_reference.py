#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only

import hashlib
import struct
import sys
from pathlib import Path

FS = 44100
OP_COUNT = 4
LEVEL_MAX_Q15 = 32767
LEVEL_MAX_Q15_16 = LEVEL_MAX_Q15 << 16

OFF = 0
ATTACK = 1
DECAY = 2
SUSTAIN = 3
RELEASE = 4

EXPECTED_STEP_SHA = "6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025"
EXPECTED_SUSTAIN_SHA = "a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815"
EXPECTED_EFFECTIVE_SHA = "196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b"
EXPECTED_STRESS_SHA = "8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849"
EXPECTED_MAX_RELEASE_STEPS = 2701155


def clamp_u8(value):
    value = int(value)
    if value < 0:
        return 0
    if value > 255:
        return 255
    return value


def clamp_q15(value):
    value = int(value)
    if value < 0:
        return 0
    if value > LEVEL_MAX_Q15:
        return LEVEL_MAX_Q15
    return value


def sustain_q15(control):
    v = clamp_u8(control)
    return (v << 7) | (v >> 1)


def sustain_q15_16(control):
    return sustain_q15(control) << 16


def target_samples(control):
    c = clamp_u8(control)
    if c == 0:
        return 0
    x = c - 1
    octave = x >> 4
    frac = x & 15
    return ((44 * (16 + frac) * (1 << octave)) + 8) >> 4


def step_q15_16(control):
    c = clamp_u8(control)
    if c == 0:
        return 0
    samples = target_samples(c)
    return (LEVEL_MAX_Q15_16 + samples - 1) // samples


def actual_full_scale_steps(control):
    step = step_q15_16(control)
    if step == 0:
        return 0
    return (LEVEL_MAX_Q15_16 + step - 1) // step


STEP_TABLE = [step_q15_16(c) for c in range(256)]


def effective_level_q15(static_q15, env_q15):
    static_q15 = clamp_q15(static_q15)
    env_q15 = clamp_q15(env_q15)
    if static_q15 == 0 or env_q15 == 0:
        return 0
    if env_q15 == LEVEL_MAX_Q15:
        return static_q15
    return (static_q15 * env_q15 + 16384) >> 15


class State:
    def __init__(self):
        self.level = [0] * OP_COUNT
        self.stage = [OFF] * OP_COUNT


def zero_controls():
    return {
        "a": [0] * OP_COUNT,
        "d": [0] * OP_COUNT,
        "s": [0] * OP_COUNT,
        "r": [0] * OP_COUNT,
    }


def valid_controls(ctrl):
    if ctrl is None:
        return False
    for key in ("a", "d", "s", "r"):
        if key not in ctrl or len(ctrl[key]) != OP_COUNT:
            return False
    return True


def reset(state):
    if state is None:
        return
    state.level[:] = [0] * OP_COUNT
    state.stage[:] = [OFF] * OP_COUNT


def retrigger(state, ctrl):
    if state is None:
        return
    if not valid_controls(ctrl):
        reset(state)
        return
    for op in range(OP_COUNT):
        a = clamp_u8(ctrl["a"][op])
        d = clamp_u8(ctrl["d"][op])
        s = clamp_u8(ctrl["s"][op])
        if a == 0:
            if d == 0:
                state.level[op] = sustain_q15_16(s)
                state.stage[op] = SUSTAIN
            else:
                state.level[op] = LEVEL_MAX_Q15_16
                state.stage[op] = DECAY
        else:
            state.level[op] = 0
            state.stage[op] = ATTACK


def note_off(state, ctrl):
    if state is None:
        return
    if not valid_controls(ctrl):
        reset(state)
        return
    for op in range(OP_COUNT):
        r = clamp_u8(ctrl["r"][op])
        if state.stage[op] == OFF or state.level[op] == 0 or r == 0:
            state.level[op] = 0
            state.stage[op] = OFF
        else:
            state.stage[op] = RELEASE


def advance_one(level, stage, a, d, s, r):
    if level < 0:
        level = 0
    if level > LEVEL_MAX_Q15_16:
        level = LEVEL_MAX_Q15_16

    a = clamp_u8(a)
    d = clamp_u8(d)
    s = clamp_u8(s)
    r = clamp_u8(r)
    target = sustain_q15_16(s)

    if stage == OFF:
        return 0, OFF

    if stage == ATTACK:
        if a == 0:
            return LEVEL_MAX_Q15_16, DECAY
        step = STEP_TABLE[a]
        if level >= LEVEL_MAX_Q15_16 - step:
            return LEVEL_MAX_Q15_16, DECAY
        return level + step, ATTACK

    if stage == DECAY:
        if level <= target or d == 0:
            return target, SUSTAIN
        step = STEP_TABLE[d]
        if level - target <= step:
            return target, SUSTAIN
        return level - step, DECAY

    if stage == SUSTAIN:
        return target, SUSTAIN

    if stage == RELEASE:
        if r == 0 or level == 0:
            return 0, OFF
        step = STEP_TABLE[r]
        if level <= step:
            return 0, OFF
        return level - step, RELEASE

    return 0, OFF


def advance(state, ctrl):
    if state is None:
        return
    if not valid_controls(ctrl):
        reset(state)
        return
    for op in range(OP_COUNT):
        state.level[op], state.stage[op] = advance_one(
            state.level[op],
            state.stage[op],
            ctrl["a"][op],
            ctrl["d"][op],
            ctrl["s"][op],
            ctrl["r"][op],
        )


def levels_q15(state):
    if state is None:
        return [0] * OP_COUNT
    out = []
    for level in state.level:
        if level < 0:
            level = 0
        if level > LEVEL_MAX_Q15_16:
            level = LEVEL_MAX_Q15_16
        out.append(level >> 16)
    return out


def map_effective_levels(state, static_q15):
    if state is None or static_q15 is None or len(static_q15) != OP_COUNT:
        return [0] * OP_COUNT
    env = levels_q15(state)
    return [effective_level_q15(static_q15[i], env[i]) for i in range(OP_COUNT)]


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def verify_step_table(out_dir):
    payload = b"".join(struct.pack("<I", value) for value in STEP_TABLE)
    assert len(payload) == 1024
    assert sha256(payload) == EXPECTED_STEP_SHA
    assert STEP_TABLE[0] == 0
    assert len(set(STEP_TABLE[1:])) == 255
    assert all(STEP_TABLE[i] > STEP_TABLE[i + 1] for i in range(1, 255))

    anchors = {
        1: (44, 48804958, 44),
        16: (85, 25263743, 85),
        32: (171, 12558001, 171),
        64: (682, 3148707, 682),
        96: (2728, 787177, 2728),
        127: (10560, 203354, 10560),
        128: (10912, 196795, 10912),
        160: (43648, 49199, 43648),
        192: (174592, 12300, 174587),
        224: (698368, 3075, 698348),
        254: (2613248, 822, 2612431),
        255: (2703360, 795, EXPECTED_MAX_RELEASE_STEPS),
    }
    for control, expected in anchors.items():
        actual = (target_samples(control), STEP_TABLE[control], actual_full_scale_steps(control))
        assert actual == expected, (control, actual, expected)

    (out_dir / "time_steps.bin").write_bytes(payload)
    print(f"WP5_6_D054_TIME_MAP_OK=1 controls=256 bytes=1024 sha256={EXPECTED_STEP_SHA} max_release_steps={EXPECTED_MAX_RELEASE_STEPS}")


def verify_sustain_map(out_dir):
    values = [sustain_q15(c) for c in range(256)]
    payload = b"".join(struct.pack("<H", value) for value in values)
    assert sha256(payload) == EXPECTED_SUSTAIN_SHA
    assert (values[0], values[1], values[127], values[128], values[255]) == (0, 128, 16319, 16448, 32767)
    assert sustain_q15(256) == 32767
    assert sustain_q15(0xFFFFFFFF) == 32767
    (out_dir / "sustain_map.bin").write_bytes(payload)
    print(f"WP5_6_D054_SUSTAIN_MAP_OK=1 controls=256 sha256={EXPECTED_SUSTAIN_SHA} malformed=saturate")


def verify_effective_map(out_dir):
    payload = bytearray()
    for static_control in range(256):
        static = sustain_q15(static_control)
        for env_control in range(256):
            env = sustain_q15(env_control)
            value = effective_level_q15(static, env)
            assert 0 <= value <= LEVEL_MAX_Q15
            payload.extend(struct.pack("<H", value))
    assert len(payload) == 131072
    assert sha256(payload) == EXPECTED_EFFECTIVE_SHA
    assert effective_level_q15(32767, 32767) == 32767
    assert effective_level_q15(16448, 32767) == 16448
    assert effective_level_q15(0xFFFFFFFF, 0xFFFFFFFF) == 32767
    (out_dir / "effective_levels.bin").write_bytes(payload)
    print(f"WP5_6_D054_EFFECTIVE_LEVEL_OK=1 pairs=65536 bytes=131072 sha256={EXPECTED_EFFECTIVE_SHA} full_env=identity")


def verify_zero_time_and_retrigger():
    ctrl = {
        "a": [0, 0, 1, 32],
        "d": [0, 64, 0, 96],
        "s": [64, 128, 192, 255],
        "r": [0, 1, 64, 255],
    }
    state = State()
    retrigger(state, ctrl)
    assert state.stage == [SUSTAIN, DECAY, ATTACK, ATTACK]
    assert levels_q15(state) == [8224, 32767, 0, 0]

    for _ in range(37):
        advance(state, ctrl)
    snapshot = (list(state.level), list(state.stage))
    assert snapshot != ([0] * OP_COUNT, [OFF] * OP_COUNT)
    retrigger(state, ctrl)
    assert state.stage == [SUSTAIN, DECAY, ATTACK, ATTACK]
    assert levels_q15(state) == [8224, 32767, 0, 0]

    note_off(state, ctrl)
    assert state.stage[0] == OFF and state.level[0] == 0
    assert state.stage[1:] == [RELEASE, OFF, OFF]
    assert state.level[2] == 0 and state.level[3] == 0

    state.stage[0] = 99
    state.level[0] = LEVEL_MAX_Q15_16 + 12345
    advance(state, ctrl)
    assert state.stage[0] == OFF and state.level[0] == 0

    retrigger(state, None)
    assert state.stage == [OFF] * OP_COUNT and state.level == [0] * OP_COUNT
    print("WP5_6_D054_ZERO_RETRIGGER_OK=1 hard_retrigger=zero attack0=instant decay0=instant release0=instant corrupt_stage=off null_controls=silence")


def verify_max_release():
    ctrl = zero_controls()
    ctrl["r"] = [255] * OP_COUNT
    state = State()
    state.level[0] = LEVEL_MAX_Q15_16
    state.stage[0] = SUSTAIN
    note_off(state, ctrl)
    assert state.stage[0] == RELEASE
    count = 0
    while state.stage[0] != OFF:
        advance(state, ctrl)
        count += 1
        assert count <= EXPECTED_MAX_RELEASE_STEPS
    assert count == EXPECTED_MAX_RELEASE_STEPS
    assert state.level[0] == 0
    print(f"WP5_6_D054_MAX_RELEASE_OK=1 control=255 advances={count}")


def verify_stress(out_dir):
    ctrl = {
        "a": [1, 32, 64, 0],
        "d": [64, 96, 128, 0],
        "s": [255, 192, 128, 64],
        "r": [96, 128, 160, 0],
    }
    static = [32767, 24672, 16448, 8224]
    state = State()
    retrigger(state, ctrl)
    payload = bytearray()

    for sample in range(65536):
        if sample == 32768:
            note_off(state, ctrl)
        for value in map_effective_levels(state, static):
            payload.extend(struct.pack("<H", value))
        payload.extend(bytes(state.stage))
        advance(state, ctrl)

    digest = sha256(payload)
    assert len(payload) == 65536 * 12
    assert digest == EXPECTED_STRESS_SHA
    assert state.level == [0] * OP_COUNT
    assert state.stage == [OFF] * OP_COUNT
    (out_dir / "stress.bin").write_bytes(payload)
    print(f"WP5_6_D054_STRESS_OK=1 samples=65536 bytes={len(payload)} sha256={digest} final=off")


def verify_model_shape():
    state_bytes = 4 * 4 + 4
    controls_bytes = 4 * 4
    assert state_bytes == 20
    assert controls_bytes == 16
    assert (OFF, ATTACK, DECAY, SUSTAIN, RELEASE) == (0, 1, 2, 3, 4)
    assert LEVEL_MAX_Q15_16 == 0x7FFF0000
    print("WP5_6_D054_MODEL_SHAPE_OK=1 operators=4 state_bytes=20 controls_bytes=16 stages=5 q15_16_max=0x7fff0000")


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: wp5_6_reference.py OUT_DIR")
    out_dir = Path(sys.argv[1])
    out_dir.mkdir(parents=True, exist_ok=True)

    verify_model_shape()
    verify_step_table(out_dir)
    verify_sustain_map(out_dir)
    verify_effective_map(out_dir)
    verify_zero_time_and_retrigger()
    verify_max_release()
    verify_stress(out_dir)

    print("WP5_6_D054_REFERENCE_OK=1 architecture=4xADSR update=per_sample source=absent")


if __name__ == "__main__":
    main()
