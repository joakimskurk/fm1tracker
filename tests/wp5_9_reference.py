#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

dinghy = (ROOT / "firmware/src/dinghy.c").read_text()
playback = (ROOT / "firmware/src/fmt_playback.c").read_text()
wp57 = (ROOT / "tests/run_wp5_7_integration_tests.sh").read_text()
host = (ROOT / "tests/host.c").read_text()
contract = (ROOT / "docs/WP5_9_MIXED_VALIDATION_CONTRACT.md").read_text()

# Production already contains the two real family renderers in one bounded mix.
assert dinghy.count("fmt_8bit_engine_render(&fmt_8bit_engine_runtime)") == 1
assert dinghy.count("fmt_fm_engine_render(&fmt_fm_engine_runtime)") == 1
assert "if (sample > 32767)" in dinghy
assert "else if (sample < -32767)" in dinghy

# Freeze the exact reason WP5.9 exists: the proof pattern is track-0 only and
# the accepted WP5.7 real-engine fixture switches that one track in time.
seed_lo = playback.index("void fmt_playback_seed_proof_pattern(void)")
seed_hi = playback.index("int fmt_playback_start(", seed_lo)
seed = playback[seed_lo:seed_hi]
assert "FMT_PLAYBACK_TRACK" in seed
assert "for (track" not in seed
assert "fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 16u)" in wp57
assert "fmt_store_step(FMT_PLAYBACK_PATTERN, 0u, 32u)" in wp57
assert "switches=fm_to_8bit_to_fm" in wp57

# The generic host harness is intentionally still pre-WP5.9 here. The freeze
# checkpoint must exist before mixed observability is added.
assert "extern Fmt8BitEngine fmt_8bit_engine_runtime;" in host
assert "extern FmtFmEngine fmt_fm_engine_runtime;" not in host
assert "extern FmtEngineRouter fmt_engine_router_runtime;" not in host

# Contract invariants that implementation must not silently weaken.
for token in (
    "FM_ONLY",
    "BIT8_ONLY",
    "COMBINED",
    "COMBINED == expected",
    "FM4 NOTE -> 8-BIT NOTE -> FM4 NOTE",
    "target-firmware runtime object",
    "image                    = 79804 B",
    "normal RAM               = 27792 B",
    "pool                     = 113000 B",
    "hardware_margin_claim=none",
):
    assert token in contract, token

# Exact production clamp oracle used later by the mixed fixture.
def clamp(v: int) -> int:
    if v > 32767:
        return 32767
    if v < -32767:
        return -32767
    return v

vectors = [
    (1000, 2000, 3000),
    (30000, 10000, 32767),
    (-30000, -10000, -32767),
    (32767, -32767, 0),
    (-32767, 32767, 0),
]
for a, b, expected in vectors:
    assert clamp(a + b) == expected

print(
    "WP5_9_REFERENCE_OK=1 "
    "production_mix=two_real_families "
    "existing_real_fixture=single_track_sequential "
    "host_observability=pre_fm "
    "oracle=saturated_sum "
    "target_identity=d056_exact"
)
