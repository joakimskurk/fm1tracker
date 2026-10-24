from pathlib import Path

# Append D040 without rewriting prior decision history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D040 —' not in s, 'D040 already present'
entry = r'''

## D040 — WP4.4 TRIANGLE is a 4-byte phase-only integer triangle with exact zero crossings and Q15 peaks

Date: 2026-10-24
Status: accepted — frozen before TRIANGLE production source

**Decision:** WP4.4 implements one isolated deterministic TRIANGLE primitive exactly as frozen in `docs/WP4_4_TRIANGLE_CONTRACT.md`. It remains disconnected from `dinghy.c` and the running full app.

`FmtTriangleOsc` contains exactly one `uint32_t phase` and is therefore exactly 4 B. Reset and D033 RETRIG both set phase to zero. Rendering samples current phase before advancing it by the already-frozen D038 phase increment.

The exact phase-to-sample mapping derives `quadrant = phase >> 30` and `offset = (phase >> 15) & 0x7FFF`. Quadrants map to `offset`, `32767-offset`, `-offset`, and `-32767+offset`. Canonical quarter-cycle samples are therefore exactly `0, +32767, 0, -32767`, output is always inside D037 `-32767..32767`, and half-cycle antisymmetry is exact.

The 15-bit quarter-slope quantization is intentional. It supplies every required integer Q15 amplitude without multiplication, division or `-32768`; phase cells around quadrant boundaries may hold an extremum/zero for adjacent phase values. WP4.4 adds no interpolation, smoothing, BLEP, oversampling or hidden filter.

TRIANGLE reuses `fmt_engine_pitch_inc_q7()` directly and creates no engine-specific pitch math. The primitive has no parameter beyond phase increment, no waveform table and no static mutable state. One render is fixed O(1) integer work with no loop, allocation, tracker-store traversal, float or 64-bit arithmetic.

Frozen deterministic vectors include quarter-cycle `0,+32767,0,-32767` and eighth-cycle `0,+16384,+32767,+16383,0,-16384,-32767,-16383`. The 65,536-sample A4 signature is final phase `0xDFB90000`, FNV-1a32 `0xBC0F75ED`.

WP4.4 remains isolated, so full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.3. The isolated mutable state cost is 4 B per TRIANGLE oscillator, 32 B for eight future instances.

**Reason:** D037 already freezes a common 32-bit phase/Q7 pitch domain and nominal-Q15 range. The quadrant/15-bit-offset construction gives natural zero-crossing reset semantics, exact positive/negative full-scale peaks, exact half-cycle symmetry and trivially bounded target arithmetic without introducing a table, multiply/divide path or parallel tuning model.

**Consequence:** Focused tests must prove exact canonical/boundary vectors, exhaustive high-16 range bounds and half-cycle antisymmetry, wrap/zero-increment/replay/guards, unchanged D038 pitch vectors and the frozen long-run signature. `fmt_osc_triangle.c` must compile with the real pinned PI32v2 compiler while the D038 pitch object stays byte-identical. WAVETABLE-8, envelope, allocator, parameter pages and proof-sine migration remain later packages. Any alternate phase orientation, amplitude rounding, interpolation or anti-aliasing requires a superseding decision and new deterministic evidence.
'''
p.write_text(s.rstrip() + entry.rstrip() + '\n')

# PROJECT_STATE.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/TRIANGLE active; D040 contract freeze next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/TRIANGLE active; D040 FROZEN; isolated implementation/tests next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 active; D040 contract/test plan required before TRIANGLE source**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 active; D040 FROZEN; isolated TRIANGLE implementation/tests next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.4 must freeze D040 before production TRIANGLE source. D040 must define exact 32-bit phase-to-Q15 triangle mapping, endpoint/midpoint/symmetry semantics, reset/retrigger/first-sample behavior, sample/advance ordering, D038 pitch reuse, deterministic vectors/hash, realtime bounds, target-compile obligations and the zero-full-app-delta isolation boundary.'
new = 'D040 is frozen in `docs/WP4_4_TRIANGLE_CONTRACT.md`: 4-byte phase-only state, exact quadrant/15-bit-offset Q15 mapping, canonical `0,+32767,0,-32767` quarter-cycle points, sample-before-advance ordering, direct D038 pitch reuse, exact half-cycle antisymmetry, deterministic vectors/hash and zero full-app delta while isolated.'
assert s.count(old) == 1, 'PROJECT_STATE D040 paragraph mismatch'
s = s.replace(old, new, 1)
anchor = '- D039: WP4.3 NOISE is an 8-byte deterministic clocked 15-bit LFSR seeded `0x5A3D`, with saturated D038 pitch clock x16 and a hard maximum of one LFSR step per audio frame while isolated.\n'
d040 = '- D040: WP4.4 TRIANGLE is a 4-byte phase-only integer oscillator with exact zero/peak quarter-cycle points, 15-bit quarter-slope quantization, half-cycle antisymmetry and direct D038 pitch reuse while isolated.\n'
assert anchor in s, 'PROJECT_STATE decision anchor missing'
if d040 not in s:
    s = s.replace(anchor, anchor + d040, 1)
old = 'Freeze D040 and the focused WP4.4 TRIANGLE test contract in the repository before creating TRIANGLE production source. Keep D038 PULSE and D039 NOISE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
new = 'Implement and verify isolated `fmt_osc_triangle.*` exactly under D040 with focused endpoint/symmetry/vector/hash host tests, static realtime audit and pinned PI32v2 target compilation. Keep PULSE/NOISE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.5 or full engine integration.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)
checkpoint_anchor = '- WP4.3 D039 verification/closeout: `42cc5aa9cb8d48f76d66817bb4977315aa60f211` — run `40093740514`, job `120871569195`\n'
checkpoint = '- WP4.4 D040 contract: `79167dd9e76fdac09801fe8fc59bb7c9bb94a72e`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)
last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.4 / #34 active. D040 is frozen in `WP4_4_TRIANGLE_CONTRACT.md` before TRIANGLE source. Exact next action: implement/test isolated `fmt_osc_triangle.*` with exhaustive bounds/symmetry vectors, deterministic hash and real PI32v2 compile while the full app remains byte-identical.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 before source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.4 TRIANGLE** — IN PROGRESS — issue #34 — freeze D040 phase/sample/test contract before source'
new = '- **WP4.4 TRIANGLE** — IN PROGRESS — issue #34 — D040 FROZEN; isolated TRIANGLE implementation + deterministic bounds/symmetry/PI32v2 verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are COMPLETE**; **WP4.4 / #34 TRIANGLE is active**, with D040 required before source.'
new = 'Phase 4 is active. **WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are COMPLETE**; **WP4.4 / #34 TRIANGLE is active and D040 is FROZEN**, with isolated deterministic implementation/testing next.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.4 / #34 — freeze D040 TRIANGLE phase/sample/output/test contract before production source. WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are complete; PULSE and NOISE remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.4 / #34 — D040 is frozen in `WP4_4_TRIANGLE_CONTRACT.md`; implement/test isolated phase-only TRIANGLE without full-app integration. WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are complete; PULSE and NOISE remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: mark only D040 contract freeze complete.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D040 TRIANGLE primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.4 active |'
new = '| D040 TRIANGLE primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_4_TRIANGLE_CONTRACT.md` / `79167dd9...` |'
assert s.count(old) == 1, 'TEST_MATRIX D040 row mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_4_D040_DOC_FREEZE_OK=1 state_bytes=4 q15=-32767..32767 reset_sample=0 phase_order=sample_before_advance')
