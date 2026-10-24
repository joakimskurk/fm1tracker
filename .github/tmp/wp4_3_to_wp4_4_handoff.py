from pathlib import Path

# PROJECT_STATE: close WP4.3 and make WP4.4 the single active package.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/NOISE active; D039 FROZEN; isolated implementation/tests next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.3 / #33'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/TRIANGLE active; D040 contract freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.4 / #34'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)

start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n', start)
next_block = '''## Next milestone

**WP4.4 - TRIANGLE oscillator**

State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 active; D040 contract/test plan required before TRIANGLE source**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — COMPLETE
WP4.4 issue: #34 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.3 closed on run `40093740514`, job `120871569195`. D039 NOISE passed exact 32,767-step LFSR-period proof, deterministic vector/hash verification, real PI32v2 compilation and a full-app closeout with zero linked delta. Evidence: `docs/WP4_3_VERIFICATION.md`.

D038 PULSE and D039 NOISE remain isolated from `dinghy.c`; the running full app therefore remains the exact Phase 3/WP4.1 baseline through WP4.3.

WP4.4 must freeze D040 before production TRIANGLE source. D040 must define exact 32-bit phase-to-Q15 triangle mapping, endpoint/midpoint/symmetry semantics, reset/retrigger/first-sample behavior, sample/advance ordering, D038 pitch reuse, deterministic vectors/hash, realtime bounds, target-compile obligations and the zero-full-app-delta isolation boundary.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.4 primitive work. Do not pull WAVETABLE-8, envelope, final allocator, parameter pages or full engine integration forward.

'''
s = s[:start] + next_block + s[end:]

checkpoint_anchor = '- WP4.3 D039 contract: `9cf3fd7d0a227c9555c34c98e37154ea038ff4cb`\n'
checkpoint = '- WP4.3 D039 verification/closeout: `42cc5aa9cb8d48f76d66817bb4977315aa60f211` — run `40093740514`, job `120871569195`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE D039 checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)

old = 'Implement and verify isolated `fmt_osc_noise.*` exactly under D039 with focused deterministic host tests, full 32,767-step LFSR-period proof, static realtime audit and pinned PI32v2 target compilation. Keep D038 PULSE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.4 or full engine integration.'
new = 'Freeze D040 and the focused WP4.4 TRIANGLE test contract in the repository before creating TRIANGLE production source. Keep D038 PULSE and D039 NOISE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)

last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.3 / #33 COMPLETE. D039 NOISE passed deterministic period/vector/hash verification, independent PI32v2 compilation and full-app closeout with zero linked delta; run `40093740514`, job `120871569195`. WP4.4 / #34 is now active. Exact next action: freeze D040 TRIANGLE contract/test plan before TRIANGLE production source.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 NOISE active; D039 FROZEN; isolated implementation/tests next**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — IN PROGRESS — issue #33 — D039 FROZEN; isolated NOISE implementation + deterministic period/vector/PI32v2 verification next
- WP4.4 TRIANGLE'''
new = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 before source**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — COMPLETE — issue #33 — D039 + `WP4_3_VERIFICATION.md`; maximal-period deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.4 TRIANGLE** — IN PROGRESS — issue #34 — freeze D040 phase/sample/test contract before source'''
assert s.count(old) == 1, 'WORK_PACKAGES Phase4 block mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 and WP4.2 / #32 are COMPLETE**; **WP4.3 / #33 NOISE is active and D039 is FROZEN** before source, with isolated deterministic implementation/testing next.'
new = 'Phase 4 is active. **WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are COMPLETE**; **WP4.4 / #34 TRIANGLE is active**, with D040 required before source.'
assert s.count(old) == 1, 'ROADMAP Phase4 lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 NOISE active; D039 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4 / #34 TRIANGLE active; D040 before source**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.3 / #33 — D039 is frozen in `WP4_3_NOISE_CONTRACT.md`; implement/test isolated clocked-LFSR NOISE without full-app integration. WP4.1 / #31 and WP4.2 / #32 are complete; PULSE remains isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.4 / #34 — freeze D040 TRIANGLE phase/sample/output/test contract before production source. WP4.1 / #31, WP4.2 / #32 and WP4.3 / #33 are complete; PULSE and NOISE remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: close NOISE and expose D040 as the next explicit gate.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| NOISE deterministic vectors + JieLi target compile | required | required | optional later | ⬜ WP4.3 next |\n| TRIANGLE/WAVETABLE bounds deterministic | required | required | optional later | ⬜ WP4.4-WP4.5 |'
new = '| NOISE deterministic vectors + JieLi target compile | required | required | optional later | ✅ run `40093607376` / closeout `40093740514` |\n| WP4.3 full app remains exact WP4.2 baseline while NOISE is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |\n| D040 TRIANGLE primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.4 active |\n| TRIANGLE deterministic vectors + JieLi target compile | required | required | optional later | ⬜ WP4.4 next |\n| WAVETABLE-8 bounds deterministic | required | required | optional later | ⬜ WP4.5 |'
assert s.count(old) == 1, 'TEST_MATRIX oscillator rows mismatch'
s = s.replace(old, new, 1)
old = 'WP4.2 detailed evidence: `WP4_2_PULSE_CONTRACT.md`, `WP4_2_VERIFICATION.md`.'
new = 'WP4.2 detailed evidence: `WP4_2_PULSE_CONTRACT.md`, `WP4_2_VERIFICATION.md`.\nWP4.3 detailed evidence: `WP4_3_NOISE_CONTRACT.md`, `WP4_3_VERIFICATION.md`.'
assert s.count(old) == 1, 'TEST_MATRIX evidence anchor mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_3_TO_WP4_4_HANDOFF_OK=1 wp4_3=complete wp4_4=active next=D040')
