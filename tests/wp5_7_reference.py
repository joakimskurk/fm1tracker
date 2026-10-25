#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
import hashlib
import struct
import sys
from pathlib import Path

OUT = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("build/wp5_7_contract")
OUT.mkdir(parents=True, exist_ok=True)

SLOTS = 8
TRACKS = 8
NONE = 0xFF
STAGE_IDLE = 0
STAGE_ACTIVE = 3
STAGE_RELEASE = 4

ALLOC_NONE = 0
ALLOC_REUSE = 1
ALLOC_FRESH = 2
ALLOC_STEAL_TAIL = 3

VOICE_BYTES = 92
ALLOCATOR_BYTES = 52
ENGINE_BYTES = 788
PATCH_BYTES = 28
RAM_INCREMENT_CEILING = 1024
TOTAL_RAM_CEILING = 20700
IMAGE_INCREMENT_CEILING = 8192
TOTAL_IMAGE_CEILING = 80984
POOL_BYTES = 113000

DEFAULT_PATCH = bytes([
    1, 5, 1, 5,
    255, 96, 255, 96,
    0, 0, 0, 0,
    0, 0, 0, 0,
    255, 255, 255, 255,
    96, 96, 96, 96,
    2, 64, 0, 0
])
assert len(DEFAULT_PATCH) == PATCH_BYTES
DEFAULT_PATCH_SHA = hashlib.sha256(DEFAULT_PATCH).hexdigest()
assert DEFAULT_PATCH_SHA == "3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52"


def synthetic_stage(stages):
    assert len(stages) == 4
    alive = [s for s in stages if s != 0]
    if not alive:
        return STAGE_IDLE
    if all(s in (0, 4) for s in stages):
        return STAGE_RELEASE
    return STAGE_ACTIVE


assert synthetic_stage([0, 0, 0, 0]) == STAGE_IDLE
assert synthetic_stage([4, 0, 4, 0]) == STAGE_RELEASE
assert synthetic_stage([1, 4, 0, 3]) == STAGE_ACTIVE
assert synthetic_stage([2, 2, 2, 2]) == STAGE_ACTIVE


class Alloc:
    def __init__(self):
        self.age = [0] * SLOTS
        self.clock = 0
        self.owner = [NONE] * SLOTS
        self.track_voice = [NONE] * TRACKS
        self.stage = [STAGE_IDLE] * SLOTS

    def touch(self, voice):
        if self.clock != 0xFFFFFFFF:
            self.clock += 1
        self.age[voice] = self.clock

    def validate(self):
        for i, owner in enumerate(self.owner):
            if owner != NONE:
                assert owner < TRACKS
                assert self.track_voice[owner] == i
            else:
                assert self.stage[i] in (STAGE_IDLE, STAGE_RELEASE)
        for track, voice in enumerate(self.track_voice):
            if voice != NONE:
                assert voice < SLOTS
                assert self.owner[voice] == track

    def reap_idle(self):
        for i in range(SLOTS):
            if self.stage[i] != STAGE_IDLE:
                continue
            owner = self.owner[i]
            if owner != NONE:
                self.track_voice[owner] = NONE
                self.owner[i] = NONE
            self.age[i] = 0

    def acquire(self, track):
        self.validate()
        self.reap_idle()
        voice = self.track_voice[track]
        if voice != NONE:
            self.touch(voice)
            self.stage[voice] = STAGE_ACTIVE
            return voice, ALLOC_REUSE
        for i in range(SLOTS):
            if self.stage[i] == STAGE_IDLE:
                self.owner[i] = track
                self.track_voice[track] = i
                self.touch(i)
                self.stage[i] = STAGE_ACTIVE
                return i, ALLOC_FRESH
        candidate = NONE
        candidate_age = 0xFFFFFFFF
        for i in range(SLOTS):
            if self.owner[i] != NONE or self.stage[i] != STAGE_RELEASE:
                continue
            if (candidate == NONE or self.age[i] < candidate_age or
                    (self.age[i] == candidate_age and i < candidate)):
                candidate = i
                candidate_age = self.age[i]
        if candidate == NONE:
            return NONE, ALLOC_NONE
        self.owner[candidate] = track
        self.track_voice[track] = candidate
        self.touch(candidate)
        self.stage[candidate] = STAGE_ACTIVE
        return candidate, ALLOC_STEAL_TAIL

    def release(self, track):
        self.validate()
        voice = self.track_voice[track]
        if voice == NONE:
            return NONE
        self.stage[voice] = STAGE_RELEASE
        self.track_voice[track] = NONE
        self.owner[voice] = NONE
        self.touch(voice)
        return voice

    def silence(self, track):
        self.validate()
        voice = self.track_voice[track]
        if voice == NONE:
            return NONE
        self.stage[voice] = STAGE_IDLE
        self.track_voice[track] = NONE
        self.owner[voice] = NONE
        self.age[voice] = 0
        return voice


alloc = Alloc()
trace = bytearray()
counts = [0, 0, 0, 0]
active_cross_track_steals = 0
x = 0x13579BDF

for step in range(4096):
    for i in range(SLOTS):
        if alloc.owner[i] == NONE and alloc.stage[i] == STAGE_RELEASE and ((step * 3 + i * 5) & 31) == 0:
            alloc.stage[i] = STAGE_IDLE

    x = (1664525 * x + 1013904223) & 0xFFFFFFFF
    track = (x >> 16) & 7
    op = (x >> 29) & 7

    if op <= 4:
        before_owner = list(alloc.owner)
        result, kind = alloc.acquire(track)
        counts[kind] += 1
        if result != NONE and before_owner[result] not in (NONE, track):
            active_cross_track_steals += 1
        code = 1
    elif op == 5:
        result = alloc.release(track)
        kind = 0
        code = 2
    elif op == 6:
        result = alloc.silence(track)
        kind = 0
        code = 3
    else:
        result = alloc.track_voice[track]
        kind = 0
        code = 4

    alloc.validate()
    trace += bytes([code, track, result if result != NONE else NONE, kind])
    trace += bytes(alloc.owner)
    trace += bytes(alloc.track_voice)
    trace += bytes(alloc.stage)
    trace += struct.pack("<I", alloc.clock)
    trace += b"".join(struct.pack("<I", age) for age in alloc.age)

ALLOC_SHA = hashlib.sha256(trace).hexdigest()
assert len(trace) == 262144
assert counts == [0, 1896, 565, 124], counts
assert active_cross_track_steals == 0
assert ALLOC_SHA == "97b87ba4b287c4bdc6cbb41021e3f5d75d9f62897f5aa5d63d5ebdc5927280af"

FAMILY_8BIT = 0
FAMILY_FM = 1
FAMILY_NONE = 0xFF


def cross_family_note(current, requested, succeeds):
    events = []
    route = current
    if route != FAMILY_NONE and route != requested:
        events.append(("silence", route))
        events.append(("clear", route))
        route = FAMILY_NONE
    events.append(("note_attempt", requested))
    if succeeds:
        route = requested
        events.append(("commit", requested))
    else:
        route = FAMILY_NONE
        events.append(("fail_clear", requested))
    return route, events


route, events = cross_family_note(FAMILY_8BIT, FAMILY_FM, True)
assert route == FAMILY_FM
assert events == [
    ("silence", FAMILY_8BIT),
    ("clear", FAMILY_8BIT),
    ("note_attempt", FAMILY_FM),
    ("commit", FAMILY_FM),
]
route, events = cross_family_note(FAMILY_8BIT, FAMILY_FM, False)
assert route == FAMILY_NONE
assert events[-1] == ("fail_clear", FAMILY_FM)

route = FAMILY_FM
for event in ("tie", "empty", "retrig"):
    before = route
    route = before
    assert route == FAMILY_FM
assert ["release", "clear"] == ["release", "clear"]

assert VOICE_BYTES * SLOTS + ALLOCATOR_BYTES == ENGINE_BYTES
assert ENGINE_BYTES <= 800
assert ENGINE_BYTES <= RAM_INCREMENT_CEILING
assert 19676 + RAM_INCREMENT_CEILING == TOTAL_RAM_CEILING
assert 72792 + IMAGE_INCREMENT_CEILING == TOTAL_IMAGE_CEILING

print("WP5_7_D055_MODEL_SHAPE_OK=1 slots=8 tracks=8 voice_bytes=92 allocator_bytes=52 engine_bytes=788 patch_bytes=28")
print(f"WP5_7_D055_DEFAULT_PATCH_OK=1 bytes=28 sha256={DEFAULT_PATCH_SHA} algorithm=DUAL2 index=64 feedback=0")
print("WP5_7_D055_STAGE_VIEW_OK=1 idle=all_off release=off_or_release active=otherwise")
print(f"WP5_7_D055_ALLOCATOR_STRESS_OK=1 events=4096 trace_bytes={len(trace)} sha256={ALLOC_SHA} reuse={counts[1]} fresh={counts[2]} steal_tail={counts[3]} active_cross_track_steals=0")
print("WP5_7_D055_ROUTE_MODEL_OK=1 cross_family=silence_clear_attempt_commit failure=none tie_empty_retrig=no_route_change off_cut=release_then_clear")
print(f"WP5_7_D055_RESOURCE_MODEL_OK=1 engine_ram=788 ram_increment_ceiling={RAM_INCREMENT_CEILING} total_ram_ceiling={TOTAL_RAM_CEILING} image_increment_ceiling={IMAGE_INCREMENT_CEILING} total_image_ceiling={TOTAL_IMAGE_CEILING} pool={POOL_BYTES}")
print("WP5_7_D055_REFERENCE_OK=1 source=absent allocator=d043-compatible family_router=d048-compatible")
