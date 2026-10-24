from pathlib import Path

# Append D041 without rewriting prior decision history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D041 —' not in s, 'D041 already present'
entry = r'''

## D041 — WP4.5 WAVETABLE-8 is eight immutable 32-sample signed-8-bit tables with zero-order-hold lookup

Date: 2026-10-24
Status: accepted — frozen before WAVETABLE-8 production source

**Decision:** WP4.5 implements one isolated deterministic WAVETABLE-8 primitive exactly as frozen in `docs/WP4_5_WAVETABLE8_CONTRACT.md`. It remains disconnected from `dinghy.c` and the running full app.

WAVETABLE-8 means exactly eight curated immutable tables of 32 signed 8-bit samples each: SINE, SAW8, SQUARE8, ORGAN, HOLLOW, NASAL, METAL and DIGI. The 256 table bytes are frozen in the contract with row-major SHA-256 `f95b092a5106a8b05acb57d2960b0162b48ea42e2f8217b8923b0b7346046aee`. No runtime table generation or editable table memory belongs to WP4.5.

`FmtWavetable8Osc` contains exactly one `uint32_t phase` and is therefore exactly 4 B. Reset and D033 RETRIG set phase to zero. The top five phase bits select table index `phase >> 27`; there is no interpolation. Current phase/sample is read before the common D038 phase increment advances the oscillator.

Table selectors `0..7` select the eight exact identities. Every malformed selector `>=8` falls back to table 0 / SINE rather than wrapping modulo 8. The table bytes are restricted to `-127..127`; `-128` is deliberately unused. Each raw byte expands to nominal Q15 by exact integer multiplication `raw * 258`, yielding full table range `-32766..32766` without a clamp/divide/asymmetric endpoint case.

The primitive has fixed O(1) render work: one selector comparison, one phase shift, one immutable lookup, one constant signed multiply and one phase add/store. It has no loop, allocation, float, 64-bit arithmetic, tracker-store traversal or hidden smoothing. The 32-sample zero-order-hold steps, aliasing and imaging are intentional Phase 4 8-BIT character; later interpolation/anti-aliasing requires an explicit superseding decision.

Frozen 65,536-sample A4 FNV-1a32 signatures are: SINE `40a7ddab`, SAW8 `7c509a67`, SQUARE8 `a573d449`, ORGAN `1d6f9fcb`, HOLLOW `b8048a69`, NASAL `a3661307`, METAL `27204689`, DIGI `48f2989f`, each ending at phase `0xDFB90000`.

The isolated immutable image-data cost is exactly 256 B and mutable oscillator-state cost is 4 B per instance / 32 B for eight future instances. Because WAVETABLE-8 remains unlinked in WP4.5, full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.4.

**Reason:** the fourth 8-BIT engine needs a broader tonal palette without introducing unbounded DSP, extra tuning math or large RAM. Eight small 32-step signed-byte waves give a deliberately lo-fi tracker palette for only 256 immutable bytes, while direct D038 phase ownership and no interpolation keep render cost trivial to bound.

**Consequence:** focused tests must prove all 256 frozen samples, selector fallback, `raw*258` expansion, zero/wrap/replay/guards, unchanged D038 pitch vectors, table-data SHA and all eight long-run signatures. `fmt_osc_wavetable8.c` must compile with the real pinned PI32v2 compiler while the D038 pitch object stays byte-identical. Envelope, allocator, parameter pages and proof-sine migration remain later packages. Any table-byte, interpolation, selector, expansion or table-count change requires a superseding decision and new deterministic evidence.
'''
p.write_text(s.rstrip() + entry.rstrip() + '\n')

# PROJECT_STATE.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5/WAVETABLE-8 active; D041 contract freeze next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5/WAVETABLE-8 active; D041 FROZEN; isolated implementation/tests next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 active; D041 contract/test plan required before WAVETABLE-8 source**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 active; D041 FROZEN; isolated WAVETABLE-8 implementation/tests next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.5 must freeze D041 before production WAVETABLE-8 source. D041 must define exact table count/length/sample representation, curated immutable contents, phase-to-index mapping, interpolation policy, table-selection/fallback semantics, reset/retrigger/first-sample behavior, direct D038 pitch reuse, deterministic vectors/hashes, realtime and image-cost bounds, real PI32v2 obligations and the zero-full-app-delta isolation boundary.'
new = 'D041 is frozen in `docs/WP4_5_WAVETABLE8_CONTRACT.md`: eight immutable 32-sample signed-8-bit tables, 256 B exact row-major data, top-five-bit phase index, no interpolation, malformed selector fallback to SINE, exact `raw*258` Q15 expansion, direct D038 pitch reuse, eight deterministic hashes and zero full-app delta while isolated.'
assert s.count(old) == 1, 'PROJECT_STATE D041 paragraph mismatch'
s = s.replace(old, new, 1)
anchor = '- D040: WP4.4 TRIANGLE is a 4-byte phase-only integer oscillator with exact zero/peak quarter-cycle points, 15-bit quarter-slope quantization, half-cycle antisymmetry and direct D038 pitch reuse while isolated.\n'
d041 = '- D041: WP4.5 WAVETABLE-8 is a 4-byte phase oscillator over eight immutable 32-sample signed-8-bit tables (256 B total), top-five-bit zero-order-hold lookup, `raw*258` Q15 expansion and direct D038 pitch reuse while isolated.\n'
assert anchor in s, 'PROJECT_STATE decision anchor missing'
if d041 not in s:
    s = s.replace(anchor, anchor + d041, 1)
old = 'Freeze D041 and the focused WP4.5 WAVETABLE-8 test contract in the repository before creating WAVETABLE-8 production source. Keep PULSE/NOISE/TRIANGLE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
new = 'Implement and verify isolated `fmt_osc_wavetable8.*` exactly under D041 with all-256-sample/table-SHA/eight-hash host tests, static realtime audit and pinned PI32v2 target compilation. Keep PULSE/NOISE/TRIANGLE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.6 or full engine integration.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)
checkpoint_anchor = '- WP4.4 D040 verification/closeout: `55e40a9a9cc94fa3c329ff2a3f12603006b8158f` — run `40094496353`, job `120873805827`\n'
checkpoint = '- WP4.5 D041 contract: `c543c99df84ee304a1db4fb631c30d01dc8607d3`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)
last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.5 / #35 active. D041 is frozen in `WP4_5_WAVETABLE8_CONTRACT.md` before production source. Exact next action: implement/test isolated `fmt_osc_wavetable8.*` with all-256-sample, selector-fallback, table-SHA and eight long-hash proof plus real PI32v2 compile while the full app remains byte-identical.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 before source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.5 WAVETABLE-8** — IN PROGRESS — issue #35 — freeze D041 table/index/interpolation/test contract before source'
new = '- **WP4.5 WAVETABLE-8** — IN PROGRESS — issue #35 — D041 FROZEN; isolated immutable-table implementation + all-table/hash/PI32v2 verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.4 / #34 are COMPLETE**; **WP4.5 / #35 WAVETABLE-8 is active**, with D041 required before source.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.4 / #34 are COMPLETE**; **WP4.5 / #35 WAVETABLE-8 is active and D041 is FROZEN**, with isolated deterministic implementation/testing next.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.5 / #35 — freeze D041 WAVETABLE-8 table/count/sample/index/interpolation/test contract before production source. WP4.1 / #31 through WP4.4 / #34 are complete; PULSE, NOISE and TRIANGLE remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.5 / #35 — D041 is frozen in `WP4_5_WAVETABLE8_CONTRACT.md`; implement/test isolated eight-table zero-order-hold WAVETABLE-8 without full-app integration. WP4.1 / #31 through WP4.4 / #34 are complete; PULSE, NOISE and TRIANGLE remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: mark only D041 contract freeze complete.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D041 WAVETABLE-8 primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.5 active |'
new = '| D041 WAVETABLE-8 primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_5_WAVETABLE8_CONTRACT.md` / `c543c99d...` |'
assert s.count(old) == 1, 'TEST_MATRIX D041 row mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_5_D041_DOC_FREEZE_OK=1 tables=8 length=32 sample=int8 data_bytes=256 interpolation=none state_bytes=4')
