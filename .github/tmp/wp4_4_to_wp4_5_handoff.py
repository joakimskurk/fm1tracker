from pathlib import Path

# PROJECT_STATE: close WP4.4 and make WP4.5 the single active package.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/TRIANGLE active; D040 FROZEN; isolated implementation/tests next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.4 / #34'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5/WAVETABLE-8 active; D041 contract freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.5 / #35'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)

start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n', start)
next_block = '''## Next milestone

**WP4.5 - WAVETABLE-8 oscillator**

State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 active; D041 contract/test plan required before WAVETABLE-8 source**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — COMPLETE
WP4.4 issue: #34 — COMPLETE
WP4.5 issue: #35 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.4 closed on run `40094496353`, job `120873805827`. D040 TRIANGLE passed exhaustive 131,072-bin bounds/antisymmetry verification, deterministic vector/hash proof, real PI32v2 compilation and a full-app closeout with zero linked delta. Evidence: `docs/WP4_4_VERIFICATION.md`.

D038 PULSE, D039 NOISE and D040 TRIANGLE remain isolated from `dinghy.c`; the running full app therefore remains the exact Phase 3/WP4.1 baseline through WP4.4.

WP4.5 must freeze D041 before production WAVETABLE-8 source. D041 must define exact table count/length/sample representation, curated immutable contents, phase-to-index mapping, interpolation policy, table-selection/fallback semantics, reset/retrigger/first-sample behavior, direct D038 pitch reuse, deterministic vectors/hashes, realtime and image-cost bounds, real PI32v2 obligations and the zero-full-app-delta isolation boundary.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.5 primitive work. Do not pull envelope, final allocator, parameter pages or full engine integration forward.

'''
s = s[:start] + next_block + s[end:]

checkpoint_anchor = '- WP4.4 D040 contract: `79167dd9e76fdac09801fe8fc59bb7c9bb94a72e`\n'
checkpoint = '- WP4.4 D040 verification/closeout: `55e40a9a9cc94fa3c329ff2a3f12603006b8158f` — run `40094496353`, job `120873805827`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE D040 checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)

old = 'Implement and verify isolated `fmt_osc_triangle.*` exactly under D040 with focused endpoint/symmetry/vector/hash host tests, static realtime audit and pinned PI32v2 target compilation. Keep PULSE/NOISE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.5 or full engine integration.'
new = 'Freeze D041 and the focused WP4.5 WAVETABLE-8 test contract in the repository before creating WAVETABLE-8 production source. Keep PULSE/NOISE/TRIANGLE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)

last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.4 / #34 COMPLETE. D040 TRIANGLE passed exhaustive deterministic bounds/symmetry/vector/hash verification, independent PI32v2 compilation and full-app closeout with zero linked delta; run `40094496353`, job `120873805827`. WP4.5 / #35 is now active. Exact next action: freeze D041 WAVETABLE-8 contract/test plan before production source.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 FROZEN; isolated implementation/tests next**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — COMPLETE — issue #33 — D039 + `WP4_3_VERIFICATION.md`; maximal-period deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.4 TRIANGLE** — IN PROGRESS — issue #34 — D040 FROZEN; isolated TRIANGLE implementation + deterministic bounds/symmetry/PI32v2 verification next
- WP4.5 WAVETABLE-8'''
new = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 before source**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — COMPLETE — issue #33 — D039 + `WP4_3_VERIFICATION.md`; maximal-period deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.4 TRIANGLE** — COMPLETE — issue #34 — D040 + `WP4_4_VERIFICATION.md`; exhaustive quantized-surface deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.5 WAVETABLE-8** — IN PROGRESS — issue #35 — freeze D041 table/index/interpolation/test contract before source'''
assert s.count(old) == 1, 'WORK_PACKAGES Phase4 block mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are COMPLETE**; **WP4.4 / #34 TRIANGLE is active and D040 is FROZEN**, with isolated deterministic implementation/testing next.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.4 / #34 are COMPLETE**; **WP4.5 / #35 WAVETABLE-8 is active**, with D041 required before source.'
assert s.count(old) == 1, 'ROADMAP Phase4 lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 before source**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.4 / #34 — D040 is frozen in `WP4_4_TRIANGLE_CONTRACT.md`; implement/test isolated phase-only TRIANGLE without full-app integration. WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are complete; PULSE and NOISE remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.5 / #35 — freeze D041 WAVETABLE-8 table/count/sample/index/interpolation/test contract before production source. WP4.1 / #31 through WP4.4 / #34 are complete; PULSE, NOISE and TRIANGLE remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: close TRIANGLE and expose D041.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| TRIANGLE deterministic vectors + JieLi target compile | required | required | optional later | ⬜ WP4.4 next |\n| WAVETABLE-8 bounds deterministic | required | required | optional later | ⬜ WP4.5 |'
new = '| TRIANGLE deterministic vectors + JieLi target compile | required | required | optional later | ✅ run `40094383287` / closeout `40094496353` |\n| WP4.4 full app remains exact WP4.3 baseline while TRIANGLE is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |\n| D041 WAVETABLE-8 primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.5 active |\n| WAVETABLE-8 deterministic tables/vectors + JieLi target compile | required | required | optional later | ⬜ WP4.5 next |'
assert s.count(old) == 1, 'TEST_MATRIX oscillator rows mismatch'
s = s.replace(old, new, 1)
old = 'WP4.3 detailed evidence: `WP4_3_NOISE_CONTRACT.md`, `WP4_3_VERIFICATION.md`.'
new = 'WP4.3 detailed evidence: `WP4_3_NOISE_CONTRACT.md`, `WP4_3_VERIFICATION.md`.\nWP4.4 detailed evidence: `WP4_4_TRIANGLE_CONTRACT.md`, `WP4_4_VERIFICATION.md`.'
assert s.count(old) == 1, 'TEST_MATRIX evidence anchor mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_4_TO_WP4_5_HANDOFF_OK=1 wp4_4=complete wp4_5=active next=D041')
