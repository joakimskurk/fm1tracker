from pathlib import Path

# PROJECT_STATE: repair the stale top-level execution state and move the next milestone to WP4.3.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1 active; D037 engine-interface architecture freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.1 / #31'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/NOISE active; D039 contract freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.3 / #33'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n', start)
next_block = '''## Next milestone

**WP4.3 - NOISE oscillator**

State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 active; D039 contract/test plan required before NOISE source**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.2 closed on run `40089122159`, job `120857992257`, with focused PULSE host/PI32v2 verification green and the complete full app byte-identical to the WP4.1/Phase 3 baseline. Evidence: `docs/WP4_2_VERIFICATION.md`.

D038 remains frozen: PULSE is a 4-byte phase-only hard-edge oscillator using the canonical Q7 pitch helper, pulse width 32..224 and sample-before-advance ordering. Its source remains isolated from `dinghy.c` until the later explicit engine-integration package.

WP4.3 must freeze D039 before production NOISE source. D039 must define exact deterministic state/seed, PRNG/LFSR algorithm and tap direction, output mapping, Q7 pitch-to-noise clock relationship, maximum state advances per rendered frame, reset/retrigger/wrap semantics, spectral/aliasing intent, deterministic vectors/hash, target-compile obligations and the zero-full-app-delta isolation boundary.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.3 primitive work. Do not pull TRIANGLE/WAVETABLE-8, envelope, final allocator, parameter pages or full engine integration forward.

'''
s = s[:start] + next_block + s[end:]
checkpoint = '- WP4.2 D038 verification/closeout: `776cbd47e55fc5da6f57e894325ba67d67a61f29` — run `40089122159`, job `120857992257`\n'
anchor = '- WP4.2 D038 decision/status freeze: `6bf5324ec09266064fbb75828bffb5e773d7ad14`\n'
assert anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(anchor, anchor + checkpoint, 1)
last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.2 / #32 COMPLETE. D038 PULSE passed focused deterministic host verification, independent PI32v2 compilation and full-app closeout with zero linked delta; run `40089122159`, job `120857992257`. WP4.3 / #33 is now active. Exact next action: freeze D039 NOISE contract/test plan before NOISE production source.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 active; D038 FROZEN; isolated PULSE implementation/tests next**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — IN PROGRESS — issue #32 — D038 FROZEN; isolated pitch/PULSE implementation + deterministic verification next
- WP4.3 NOISE'''
new = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 NOISE active; D039 before source**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — COMPLETE — issue #32 — D038 + `WP4_2_VERIFICATION.md`; deterministic host/PI32v2 proof and zero linked full-app delta
- **WP4.3 NOISE** — IN PROGRESS — issue #33 — freeze D039 deterministic noise/clock contract before source'''
assert s.count(old) == 1, 'WORK_PACKAGES Phase4 block mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source and zero production delta; **WP4.2 / #32 PULSE is active and D038 is FROZEN** before oscillator source.'
new = 'Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source; **WP4.2 / #32 PULSE is COMPLETE** with D038 deterministic host/PI32v2 proof and zero linked full-app delta; **WP4.3 / #33 NOISE is active**, with D039 required before source.'
assert s.count(old) == 1, 'ROADMAP Phase4 lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2 / #32 PULSE active; D038 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 NOISE active; D039 before source**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.2 / #32 — D038 is frozen in `WP4_2_PULSE_CONTRACT.md`; implement/test the isolated shared pitch helper and PULSE primitive without full-app integration. WP4.1 / #31 is complete. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.3 / #33 — freeze D039 NOISE state/seed/clock/output/test contract before production source. WP4.1 / #31 and WP4.2 / #32 are complete; PULSE remains isolated from the full app. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: close PULSE rows and split the next oscillator gates so NOISE has an explicit D039 checkpoint.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| PULSE deterministic vectors + JieLi target compile | required | required | optional later | ⬜ WP4.2 next |\n| NOISE/TRIANGLE/WAVETABLE bounds deterministic | required | required | optional later | ⬜ WP4.3-WP4.5 |'
new = '| PULSE deterministic vectors + JieLi target compile | required | required | optional later | ✅ run `40088983995` / closeout `40089122159` |\n| WP4.2 full app remains exact WP4.1 baseline while PULSE is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |\n| D039 NOISE primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.3 active |\n| NOISE deterministic vectors + JieLi target compile | required | required | optional later | ⬜ WP4.3 next |\n| TRIANGLE/WAVETABLE bounds deterministic | required | required | optional later | ⬜ WP4.4-WP4.5 |'
assert s.count(old) == 1, 'TEST_MATRIX oscillator rows mismatch'
s = s.replace(old, new, 1)
old = 'WP4.1 detailed evidence: `WP4_1_ENGINE_CONTRACT.md`, `WP4_1_VERIFICATION.md`.'
new = 'WP4.1 detailed evidence: `WP4_1_ENGINE_CONTRACT.md`, `WP4_1_VERIFICATION.md`.\nWP4.2 detailed evidence: `WP4_2_PULSE_CONTRACT.md`, `WP4_2_VERIFICATION.md`.'
assert s.count(old) == 1, 'TEST_MATRIX evidence anchor mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_2_TO_WP4_3_HANDOFF_OK=1 wp4_2=complete wp4_3=active next=D039')
