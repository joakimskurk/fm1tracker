from pathlib import Path

# PROJECT_STATE: close WP4.5 and make WP4.6 the single active package.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5/WAVETABLE-8 active; D041 FROZEN; isolated implementation/tests next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.5 / #35'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5/D041 COMPLETE; WP4.6/envelope active; D042 contract freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.6 / #36'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)

start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n', start)
next_block = '''## Next milestone

**WP4.6 - common envelope primitive**

State: **IN PROGRESS - WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 active; D042 state/time/gate/retrigger/release contract required before envelope source**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — COMPLETE
WP4.4 issue: #34 — COMPLETE
WP4.5 issue: #35 — COMPLETE
WP4.6 issue: #36 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.5 closed on run `40095198147`, job `120875856198`. D041 WAVETABLE-8 passed all-256-sample/table-SHA/eight-hash deterministic verification, real PI32v2 compilation and a full-app closeout with zero linked delta. Evidence: `docs/WP4_5_VERIFICATION.md`.

The complete starting oscillator palette — D038 PULSE, D039 NOISE, D040 TRIANGLE and D041 WAVETABLE-8 — remains isolated from `dinghy.c`; the running full app therefore remains the exact Phase 3/WP4.1 baseline through WP4.5.

WP4.6 must freeze D042 before production envelope source. D042 must define exact common envelope topology, fixed state and Q15 level domain, parameter/time mapping, zero-time behavior, NOTE/gate-on vs D033 RETRIG semantics, OFF/release and TIE semantics, sample/update ordering, stage-transition bound, deterministic vectors/hash, realtime/resource bounds, real PI32v2 obligations and the zero-full-app-delta isolation boundary.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.6 primitive work. Do not pull final allocator, parameter pages or full engine integration forward.

'''
s = s[:start] + next_block + s[end:]

checkpoint_anchor = '- WP4.5 D041 contract: `c543c99df84ee304a1db4fb631c30d01dc8607d3`\n'
checkpoint = '- WP4.5 D041 verification/closeout: `bb56d16c31330e3515ba36241726c09a2a30729b` — run `40095198147`, job `120875856198`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE D041 checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)

old = 'Implement and verify isolated `fmt_osc_wavetable8.*` exactly under D041 with all-256-sample/table-SHA/eight-hash host tests, static realtime audit and pinned PI32v2 target compilation. Keep PULSE/NOISE/TRIANGLE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.6 or full engine integration.'
new = 'Freeze D042 and the focused WP4.6 common-envelope test contract in the repository before creating envelope production source. Keep all four oscillator primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)

last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.5 / #35 COMPLETE. D041 WAVETABLE-8 passed all-table deterministic verification, independent PI32v2 compilation and full-app closeout with zero linked delta; run `40095198147`, job `120875856198`. WP4.6 / #36 is now active. Exact next action: freeze D042 common-envelope state/time/gate/retrigger/release/test contract before production source.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 FROZEN; isolated implementation/tests next**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — COMPLETE — issue #33 — D039 + `WP4_3_VERIFICATION.md`; maximal-period deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.4 TRIANGLE** — COMPLETE — issue #34 — D040 + `WP4_4_VERIFICATION.md`; exhaustive quantized-surface deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.5 WAVETABLE-8** — IN PROGRESS — issue #35 — D041 FROZEN; isolated immutable-table implementation + all-table/hash/PI32v2 verification next
- WP4.6 envelope primitive'''
new = '''Status: **IN PROGRESS — WP4.1 / #31 through WP4.5 / #35 COMPLETE; WP4.6 / #36 common envelope active; D042 before source**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — COMPLETE — issue #33 — D039 + `WP4_3_VERIFICATION.md`; maximal-period deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.4 TRIANGLE** — COMPLETE — issue #34 — D040 + `WP4_4_VERIFICATION.md`; exhaustive quantized-surface deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.5 WAVETABLE-8** — COMPLETE — issue #35 — D041 + `WP4_5_VERIFICATION.md`; all-256-sample/table-SHA/eight-hash host/PI32v2 proof and zero linked full-app delta
- **WP4.6 common envelope primitive** — IN PROGRESS — issue #36 — freeze D042 state/time/gate/retrigger/release/test contract before source'''
assert s.count(old) == 1, 'WORK_PACKAGES Phase4 block mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.4 / #34 are COMPLETE**; **WP4.5 / #35 WAVETABLE-8 is active and D041 is FROZEN**, with isolated deterministic implementation/testing next.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.5 / #35 are COMPLETE**; **WP4.6 / #36 common envelope is active**, with D042 required before source.'
assert s.count(old) == 1, 'ROADMAP Phase4 lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5 / #35 WAVETABLE-8 active; D041 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 common envelope active; D042 before source**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.5 / #35 — D041 is frozen in `WP4_5_WAVETABLE8_CONTRACT.md`; implement/test isolated eight-table zero-order-hold WAVETABLE-8 without full-app integration. WP4.1 / #31 through WP4.4 / #34 are complete; PULSE, NOISE and TRIANGLE remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.6 / #36 — freeze D042 common-envelope state/time/gate/retrigger/release/test contract before production source. WP4.1 / #31 through WP4.5 / #35 are complete; all four oscillator primitives remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: close WAVETABLE-8 and expose D042.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| WAVETABLE-8 deterministic tables/vectors + JieLi target compile | required | required | optional later | ⬜ WP4.5 next |\n| Voice stealing deterministic | required | required | required later | ⬜ WP4.7 |'
new = '| WAVETABLE-8 deterministic tables/vectors + JieLi target compile | required | required | optional later | ✅ run `40095129004` / closeout `40095198147` |\n| WP4.5 full app remains exact WP4.4 baseline while WAVETABLE-8 is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |\n| D042 common envelope primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.6 active |\n| Envelope stage/time/gate/retrigger/release vectors + JieLi target compile | required | required | optional later | ⬜ WP4.6 next |\n| Voice stealing deterministic | required | required | required later | ⬜ WP4.7 |'
assert s.count(old) == 1, 'TEST_MATRIX envelope insertion mismatch'
s = s.replace(old, new, 1)
old = 'WP4.4 detailed evidence: `WP4_4_TRIANGLE_CONTRACT.md`, `WP4_4_VERIFICATION.md`.'
new = 'WP4.4 detailed evidence: `WP4_4_TRIANGLE_CONTRACT.md`, `WP4_4_VERIFICATION.md`.\nWP4.5 detailed evidence: `WP4_5_WAVETABLE8_CONTRACT.md`, `WP4_5_VERIFICATION.md`.'
assert s.count(old) == 1, 'TEST_MATRIX evidence anchor mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_5_TO_WP4_6_HANDOFF_OK=1 wp4_5=complete wp4_6=active next=D042')
