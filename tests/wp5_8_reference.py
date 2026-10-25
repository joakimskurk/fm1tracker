#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
from __future__ import annotations

import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

PAGES = (
    ("CORE",    (0x60, 0x61, 0x62, 0x00)),
    ("RATIO",   (0x64, 0x65, 0x66, 0x67)),
    ("LEVEL",   (0x68, 0x69, 0x6A, 0x6B)),
    ("ATTACK",  (0x6C, 0x6D, 0x6E, 0x6F)),
    ("DECAY",   (0x70, 0x71, 0x72, 0x73)),
    ("SUSTAIN", (0x74, 0x75, 0x76, 0x77)),
    ("RELEASE", (0x78, 0x79, 0x7A, 0x7B)),
)

DEFAULT = {
    "algorithm": 128,
    "index": 64,
    "feedback": 0,
    "ratio": (16, 80, 16, 80),
    "level": (255, 96, 255, 96),
    "attack": (0, 0, 0, 0),
    "decay": (0, 0, 0, 0),
    "sustain": (255, 255, 255, 255),
    "release": (96, 96, 96, 96),
}

EXPECTED_PATCH_SHA = "3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52"


def compose_patch(p: dict[str, object], overrides: tuple[tuple[int, int], ...] = ()) -> bytes:
    # Flat normalized byte map in semantic-ID order.
    v = {
        0x60: int(p["algorithm"]),
        0x61: int(p["index"]),
        0x62: int(p["feedback"]),
    }
    for base, key in ((0x64, "ratio"), (0x68, "level"), (0x6C, "attack"),
                      (0x70, "decay"), (0x74, "sustain"), (0x78, "release")):
        values = p[key]
        assert isinstance(values, tuple) and len(values) == 4
        for i, value in enumerate(values):
            v[base + i] = int(value)

    assert len(overrides) <= 4
    for param_id, value in overrides:
        # Main-side FM resolver contract: wrong namespace, reserved IDs and
        # malformed >8-bit values do not become engine overrides.
        if param_id not in v or value < 0 or value > 255:
            continue
        v[param_id] = value

    ratio = bytes(v[0x64 + i] >> 4 for i in range(4))
    level = bytes(v[0x68 + i] for i in range(4))
    attack = bytes(v[0x6C + i] for i in range(4))
    decay = bytes(v[0x70 + i] for i in range(4))
    sustain = bytes(v[0x74 + i] for i in range(4))
    release = bytes(v[0x78 + i] for i in range(4))
    tail = bytes((v[0x60] >> 6, v[0x61], v[0x62], 0))
    return ratio + level + attack + decay + sustain + release + tail


def pack_overrides(items: tuple[tuple[int, int], ...]) -> bytes:
    valid = sorted((pid, value) for pid, value in items
                   if 0x60 <= pid <= 0x7B and pid != 0x63 and 0 <= value <= 255)
    valid = valid[:4]
    out = bytearray()
    for pid, value in valid:
        out.extend((pid, value))
    while len(out) < 8:
        out.extend((0, 0))
    return bytes(out)


def main() -> None:
    ids = [pid for _, page in PAGES for pid in page if pid]
    assert len(PAGES) == 7
    assert all(len(page) == 4 for _, page in PAGES)
    assert len(ids) == 27
    assert len(set(ids)) == 27
    assert min(ids) == 0x60 and max(ids) == 0x7B
    assert 0x63 not in ids
    assert all(0x60 <= pid <= 0x9F for pid in ids)

    # 27-byte normalized base and exact D055 bridge reproduction.
    normalized = bytes((
        DEFAULT["algorithm"], DEFAULT["index"], DEFAULT["feedback"],
        *DEFAULT["ratio"], *DEFAULT["level"], *DEFAULT["attack"],
        *DEFAULT["decay"], *DEFAULT["sustain"], *DEFAULT["release"],
    ))
    assert len(normalized) == 27
    assert 16 * len(normalized) == 432
    patch = compose_patch(DEFAULT)
    assert len(patch) == 28
    assert hashlib.sha256(patch).hexdigest() == EXPECTED_PATCH_SHA

    # Mapping boundaries are monotone/bounded and reproduce all legal IDs.
    assert [x >> 6 for x in (0, 63, 64, 127, 128, 191, 192, 255)] == [0, 0, 1, 1, 2, 2, 3, 3]
    assert [x >> 4 for x in (0, 15, 16, 31, 240, 255)] == [0, 0, 1, 1, 15, 15]

    # At most four compact overrides, deterministic semantic-ID order.
    payload = pack_overrides(((0x79, 44), (0x60, 255), (0x66, 31), (0x68, 99)))
    assert payload == bytes((0x60, 255, 0x66, 31, 0x68, 99, 0x79, 44))
    assert len(payload) == 8

    # Wrong-family/reserved/corrupt values are filtered; never uint8-truncated.
    filtered = pack_overrides(((0x20, 12), (0x63, 99), (0x60, 256), (0x61, 77)))
    assert filtered == bytes((0x61, 77, 0, 0, 0, 0, 0, 0))
    base_patch = compose_patch(DEFAULT)
    corrupt_patch = compose_patch(DEFAULT, ((0x60, 256), (0x20, 12)))
    assert corrupt_patch == base_patch

    # One physical generalized row cache with exact historical first view.
    tracks, rows = 8, 64
    legacy = tracks * rows * 4
    generalized = legacy + legacy + legacy + legacy + tracks * rows
    assert legacy == 2048
    assert generalized == 8704
    assert generalized - legacy == 6656
    assert (generalized - legacy) + 432 == 7088
    assert 7088 <= 8192

    # Contract-source audit: 16BIT namespace exists today but is not activated
    # until production implementation. This script runs before that activation.
    registry = (ROOT / "firmware/src/fmt_param_registry.h").read_text()
    assert "#define FMT_PARAM_16BIT_FIRST        0x60u" in registry
    assert "#define FMT_PARAM_16BIT_LAST         0x9Fu" in registry
    assert "FMT_PARAM_ASSIGNED_COUNT = 10u" in registry
    assert "FMT_PARAM_FM_ALGORITHM" not in registry

    print("WP5_8_REFERENCE_OK=1 ids=27 pages=7 base_bytes=27 bank_bytes=432 legacy_cache=2048 generalized_cache=8704 net_cache_growth=6656 known_ram_growth=7088 default_patch_sha256=" + EXPECTED_PATCH_SHA)


if __name__ == "__main__":
    main()
