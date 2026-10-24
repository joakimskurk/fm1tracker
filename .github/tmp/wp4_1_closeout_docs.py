from pathlib import Path

# PROJECT_STATE: move active package from WP4.1 to WP4.2.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = '''## Next milestone

**WP4.1 - 8-BIT engine interface contract**

State: **IN PROGRESS - D037 FROZEN; WP4.1 verification/closeout next; no production engine DSP begun**

Phase 4 parent issue: #30
WP4.1 issue: #31
Branch: `phase4/8bit-engine`

D037 must freeze engine/voice ownership, fixed-point pitch, note/tie/off/retrigger adapter semantics, per-track/per-voice state, sample/mix format, p-lock/effect integration boundary, realtime work bounds and the complete Phase 4 resource budget before PULSE/NOISE/TRIANGLE/WAVETABLE-8 production DSP is written.

The Phase 3 proof sine remains the current firmware audio implementation until a later Phase 4 package deliberately replaces/integrates it. Do not pull WP4.2+ implementation into WP4.1.
'''
new = '''## Next milestone

**WP4.2 - PULSE oscillator**

State: **IN PROGRESS - WP4.1/D037 COMPLETE; D038 PULSE contract/test plan next; no PULSE production source begun**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.1 closed on run `40087703654`, job `120853898352`, with zero production delta from the Phase 4 branch start and the full inherited Phase 3 target gate green. Evidence: `docs/WP4_1_VERIFICATION.md`.

D038 must freeze the isolated PULSE primitive before source: exact state/reset, shared Q7 pitch-to-phase-increment boundary, phase/sample ordering, pulse-width representation/clamp, nominal-Q15 output, wrap/extreme behavior, deterministic vectors, target-compile/resource obligations and the explicit proof-sine migration boundary.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.2 primitive work. Do not pull NOISE/TRIANGLE/WAVETABLE-8, envelope, final allocator, parameter pages or full engine integration forward.
'''
assert s.count(old) == 1, 'PROJECT_STATE next milestone mismatch'
s = s.replace(old, new)
s = s.replace('- D036: the complete D027-D035 stack passed worst-case combined verification with zero production delta; Phase 3 is complete at host/device-build level.\n',
              '- D036: the complete D027-D035 stack passed worst-case combined verification with zero production delta; Phase 3 is complete at host/device-build level.\n- D037: Phase 4 engine ownership, eight-voice cap, Q7 pitch domain, event/effect adapter boundary, Q15/32-bit mix rules, realtime bounds and complete Phase 4 resource ceilings are frozen before oscillator source.\n')
old = '''## Exact next action

Verify and close WP4.1 / #31 with D037 frozen in `docs/WP4_1_ENGINE_CONTRACT.md`: prove zero production firmware/DSP delta from the Phase 4 branch start, preserve the Phase 2/3 migration baseline, record verification evidence, then make WP4.2 / PULSE the only active package.
'''
new = '''## Exact next action

Freeze D038 and the focused WP4.2 PULSE test contract in the repository before creating PULSE production source. Keep the Phase 3 proof sine as full-app production audio while the isolated primitive is implemented and verified.
'''
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new)
old = '- WP3.10 / Phase 3 closeout workflow: `e90d0232c179ea92e2295171403697ceaf5e3ecf`\n'
new = old + '- WP4.1 D037 contract: `a740da66a0587b4e257629d7aba12781fad73f73`\n- WP4.1 closeout workflow: `01138c5f8a420efcc8e438ff3c2eb4090c086586`\n'
assert s.count(old) == 1, 'PROJECT_STATE checkpoint anchor mismatch'
s = s.replace(old, new)
old = '2026-10-24 - Phase 4 branch `phase4/8bit-engine` created from clean Phase 3 handoff `d14955f2...`. #30 and WP4.1 / #31 are IN PROGRESS. D037 is frozen; exact next action is WP4.1 verification/closeout with zero production DSP delta. No Phase 4 production engine DSP has begun.'
new = '2026-10-24 - WP4.1 / #31 COMPLETE. D037 frozen before source; closeout run `40087703654` SUCCESS with zero production delta and inherited Phase 3 device-build gate green. WP4.2 / #32 is now IN PROGRESS; exact next action is D038 PULSE contract/test-plan freeze before PULSE source.'
assert s.count(old) == 1, 'PROJECT_STATE last-updated mismatch'
p.write_text(s.replace(old, new))

# WORK_PACKAGES: move Phase 4 status and exact active package to WP4.2.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '''Status: **IN PROGRESS — WP4.1 / #31 active; D037 required before production engine source**

Parent issue: #30

- **WP4.1 engine interface contract** — IN PROGRESS — issue #31 — D037 FROZEN; verification/closeout next; no production engine DSP
- WP4.2 PULSE
'''
new = '''Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 active; D038 required before PULSE source**

Parent issue: #30

- **WP4.1 engine interface contract** — COMPLETE — issue #31 — D037 + `WP4_1_VERIFICATION.md`; zero production delta
- **WP4.2 PULSE** — IN PROGRESS — issue #32 — freeze D038/test contract before source
'''
assert s.count(old) == 1, 'WORK_PACKAGES Phase 4 block mismatch'
p.write_text(s.replace(old, new))

# ROADMAP current Phase 4 status/action and stale Phase 3 handoff line.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Immediate next package: **Phase 4 / WP4.1 / #31**. Freeze D037 engine/voice/realtime architecture before any 8-BIT engine production source.'
new = 'Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source and zero production delta; **WP4.2 / #32 PULSE is now active**, with D038 required before oscillator source.'
assert s.count(old) == 1, 'ROADMAP Phase 3 handoff mismatch'
s = s.replace(old, new)
old = 'Status: **IN PROGRESS — WP4.1 D037 FROZEN; verification/closeout next; no production engine DSP**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2 / #32 PULSE active; D038 before source**'
assert s.count(old) == 1, 'ROADMAP Phase 4 status mismatch'
s = s.replace(old, new)
old = 'Active package: WP4.1 / #31 — D037 is frozen in `WP4_1_ENGINE_CONTRACT.md`; verify zero production delta and close WP4.1 before WP4.2. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.2 / #32 — freeze D038 PULSE primitive/test contract before production oscillator source. WP4.1 / #31 is complete with evidence in `WP4_1_VERIFICATION.md`. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new))

# TEST_MATRIX: add Phase 4 section before regression rule.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
anchor = '\n## Regression rule\n'
assert s.count(anchor) == 1, 'TEST_MATRIX regression anchor mismatch'
assert '## Phase 4 8-BIT engine' not in s, 'TEST_MATRIX Phase 4 section already exists'
section = r'''

## Phase 4 8-BIT engine

| Check | Automated host | Firmware/device build | Hardware | Status |
|---|---:|---:|---:|---|
| D037 engine/voice/realtime contract frozen before oscillator source | inspect/gate | n/a | n/a | ✅ WP4.1 / #31 |
| WP4.1 production tree byte-identical to clean Phase 4 branch start | git compare | required | n/a | ✅ `d14955f2...`, delta 0 |
| No PULSE/NOISE/TRIANGLE/WAVETABLE-8 production source in WP4.1 | inspect/gate | required | n/a | ✅ WP4.1 |
| Complete inherited Phase 3 exit/stress/realtime gate reruns green | required | required | later | ✅ run `40087703654` |
| Pinned JieLi/AC79 build remains exact Phase 3 artifact identity | n/a | required | n/a | ✅ 66,960 B image / 13,296 B RAM / 113,000 B pool |
| WP4.1 firmware BIN/FWSC hashes unchanged | n/a | required | n/a | ✅ `b2fc3af1...` / `7870d11b...` |
| D037 Phase 4 resource ceilings frozen | inspect/gate | n/a | n/a | ✅ image 132,496 / RAM 21,488 / pool 129,384 maximum |
| Physical FM-1 install/audio/execution margin | n/a | n/a | required later | ⬜ no physical FM-1 available |
| D038 PULSE primitive/test contract frozen | required | compile | n/a | ⬜ WP4.2 active |
| PULSE deterministic vectors + JieLi target compile | required | required | optional later | ⬜ WP4.2 next |

WP4.1 detailed evidence: `WP4_1_ENGINE_CONTRACT.md`, `WP4_1_VERIFICATION.md`.
'''
s = s.replace(anchor, section + anchor)
p.write_text(s)

print('WP4_1_CLOSEOUT_DOCS_OK=1 next=WP4.2 issue=32')
