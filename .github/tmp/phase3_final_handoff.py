from pathlib import Path
import re


def read(path):
    return Path(path).read_text()


def write(path, text):
    Path(path).write_text(text.rstrip() + '\n')


def once(text, old, new, label):
    n = text.count(old)
    assert n == 1, f'{label}: expected 1 match, got {n}'
    return text.replace(old, new, 1)


def section(text, start_heading, next_heading, replacement, label):
    start = text.find(start_heading)
    assert start >= 0, f'{label}: start not found'
    end = text.find(next_heading, start + len(start_heading))
    assert end >= 0, f'{label}: end not found'
    return text[:start] + replacement.rstrip() + '\n\n' + text[end:]

# ---------------------------------------------------------------------------
# DECISIONS — promote D036 from frozen plan to verified Phase 3 exit decision.
p = 'docs/DECISIONS.md'
s = read(p)
s = once(s,
    '## D036 — WP3.10 is a worst-case verification and Phase 3 exit package, not a feature package\n\nDate: 2026-10-24\nStatus: accepted before WP3.10 stress implementation',
    '## D036 — WP3.10 is a worst-case verification and Phase 3 exit package, not a feature package\n\nDate: 2026-10-24\nStatus: accepted and verified — Phase 3 exit complete',
    'DECISIONS D036 status')
old = '**Consequence:** Phase 4 production work is blocked until WP3.10 passes the D036 exit criteria and Phase 3 is formally closed. WP3.10 may add tests/tooling/docs; it may change production only to fix a defect demonstrated by the frozen stress plan.'
new = '''**Consequence:** D036 passed without finding a production defect. WP3.10 therefore leaves the final WP3.9 production checkpoint `354bf7d356774ad828f1f660bd456dbab7f0490d` byte-identical and adds zero production resource cost. Final run `40084706845`, job `120845144453`, artifact `12556590742` (`c4f494716ec98507ca9af6fe59313c01116136deececa7ba84e24598e78b82ee`) satisfies the Phase 3 host/device-build exit gate. Final resources remain 66,960 B image / 13,296 B normal RAM / 113,000 B pool and firmware hashes remain `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27` / `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`. Evidence is `docs/WP3_10_VERIFICATION.md`.

Phase 3 is now formally complete at host/device-build level. Phase 4 may begin only with WP4.1 / D037 engine-interface architecture; physical FM-1 install/audio/ergonomics/LCD/execution-margin gates remain explicitly open.'''
s = once(s, old, new, 'DECISIONS D036 consequence')
write(p, s)

# ---------------------------------------------------------------------------
# PROJECT_STATE — Phase 3 final handoff, with Phase 4 next but not yet active
# on this phase branch.
p = 'docs/PROJECT_STATE.md'
s = read(p)
current = '''## Current phase

**Phase 3 - p-locks and tracker effects — COMPLETE**

State: **COMPLETE - WP3.1-WP3.10 host/device-build exit satisfied - 2026-10-24**

Parent issue: #20 — CLOSED / completed  
Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

Final Phase 3 closeout workflow checkpoint:

`e90d0232c179ea92e2295171403697ceaf5e3ecf`

Final Phase 3 production firmware checkpoint remains the WP3.9 source tree:

`354bf7d356774ad828f1f660bd456dbab7f0490d`
'''
s = section(s, '## Current phase\n', '## Completed Phase 3 packages\n', current, 'PROJECT_STATE current phase')

wp310 = '''### WP3.10 - Phase 3 worst-case tests and exit gate - COMPLETE - #29

Final closeout workflow checkpoint:

`e90d0232c179ea92e2295171403697ceaf5e3ecf`

Final GitHub Actions:

- run `40084706845`
- job `120845144453` (`phase3-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_10_VERIFICATION.md`  
Normative test plan: `docs/WP3_10_TEST_PLAN.md`

Evidence artifact:

- ID `12556590742`
- ZIP SHA-256 `c4f494716ec98507ca9af6fe59313c01116136deececa7ba84e24598e78b82ee`

D036 found no production defect. It proved the complete D027-D035 stack under full lock-pool pressure, all-row cache pressure, repeated queue full/wrap/overflow recovery, combined playback/audition/editor/UI stress, all D032/D033 byte values, 4,096 rows / 64 wraps, the retained six-hour transport proof and a fail-fast static realtime audit.

Key exit markers:

- `WP3_10_LOCK_POOL_STRESS_OK=1 records=2048 owners=512 hash=4f426945`
- `WP3_10_QUEUE_STRESS_OK=1 cycles=2048 rejected=6144 final_write=65519 final_read=65519 overflow=5143`
- `WP3_10_COMBINED_STRESS_OK=1 deterministic=1 two_overflow_epochs=1 recovery=1 midi_neutral=1 screen_queue_neutral=1`
- `WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1 hash=364d0c2c`
- `WP3_10_REALTIME_AUDIT_OK=1`
- `WP3_10_PHASE3_EXIT_OK=1 wp3_1_through_wp3_10=complete host_device_build=1 physical_hardware_pending=1`

WP3.10 changes no production firmware source. Final Phase 3 resources therefore remain:

| Resource | Used | Limit | Free | Delta vs WP3.9 |
|---|---:|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B | +0 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B | +0 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`.

'''
marker = '## Next milestone\n'
idx = s.find(marker)
assert idx >= 0, 'PROJECT_STATE next milestone marker missing'
assert '### WP3.10 - Phase 3 worst-case tests and exit gate - COMPLETE - #29' not in s
s = s[:idx] + wp310 + s[idx:]

next_milestone = '''## Next milestone

**Phase 4 / WP4.1 - 8-BIT engine interface contract**

State: **READY TO START - create/use `phase4/8bit-engine`, freeze D037 before production engine source**

Phase 4 parent issue: #30  
WP4.1 issue: #31

D037 must freeze engine/voice ownership, fixed-point pitch, note/tie/off/retrigger adapter semantics, per-track/per-voice state, sample/mix format, p-lock/effect integration boundary, realtime work bounds and the complete Phase 4 resource budget before PULSE/NOISE/TRIANGLE/WAVETABLE-8 production DSP is written.

The Phase 3 proof sine remains the current firmware audio implementation until a later Phase 4 package deliberately replaces/integrates it. Do not pull WP4.2+ implementation into WP4.1.
'''
s = section(s, '## Next milestone\n', '## Important decisions / do not regress\n', next_milestone, 'PROJECT_STATE next milestone')

s = once(s,
    '- D036: WP3.10 is a stress/exit package; Phase 4 is blocked until the complete D027-D035 stack passes worst-case combined verification.\n',
    '- D036: the complete D027-D035 stack passed worst-case combined verification with zero production delta; Phase 3 is complete at host/device-build level.\n',
    'PROJECT_STATE D036 bullet')

s = section(s, '## Exact next action\n', '## Last relevant checkpoints\n', '''## Exact next action

Create/use the Phase 4 branch `phase4/8bit-engine`, then start WP4.1 / #31 by freezing D037 in the repository **before any production oscillator/engine DSP source**. Preserve every Phase 2/3 deterministic regression as the migration baseline.
''', 'PROJECT_STATE exact next')

s = once(s,
    '- WP3.9 verified production: `354bf7d356774ad828f1f660bd456dbab7f0490d`\n',
    '- WP3.9 verified production: `354bf7d356774ad828f1f660bd456dbab7f0490d`\n- WP3.10 / Phase 3 closeout workflow: `e90d0232c179ea92e2295171403697ceaf5e3ecf`\n',
    'PROJECT_STATE checkpoints')

# Last-updated block may be last section; replace from heading to EOF.
lu = s.find('## Last updated\n')
assert lu >= 0, 'PROJECT_STATE Last updated missing'
s = s[:lu] + '''## Last updated

2026-10-24 - WP3.10 / #29 and Phase 3 / #20 closed after final run `40084706845`. Phase 3 is COMPLETE at host/device-build level with zero WP3.10 production delta. Phase 4 / #30 and WP4.1 / #31 are created; next step is the D037 engine-interface freeze on `phase4/8bit-engine`.
'''
write(p, s)

# ---------------------------------------------------------------------------
# WORK_PACKAGES — Phase 3 complete and Phase 4 WP4.1 ready.
p = 'docs/WORK_PACKAGES.md'
s = read(p)
s = once(s,
    'Status: **IN PROGRESS — WP3.1–WP3.9 complete; WP3.10 active**',
    'Status: **COMPLETE — WP3.1–WP3.10 host/device-build exit satisfied**',
    'WORK_PACKAGES phase3 status')

anchor = '''- **WP3.9 — lock/effect visual feedback** — COMPLETE — issue #28
  D035 extends the existing ten-region / max-two-redraw tracker surface with `LK` p-lock summaries, stored D032/D033 effect mnemonics/values and `STORE` detail for TRACK PAN while preserving main-only bounded drawing. `FmtTrackerScreen` is exactly 62 B. Final run `40083510351`, job `120841687772`. Final resources: 66,960 B image / 13,296 B normal RAM / 113,000 B pool. Evidence: `WP3_9_VISUAL_CONTRACT.md`, `WP3_9_VERIFICATION.md`.
'''
addition = anchor + '''
- **WP3.10 — Phase 3 worst-case tests and exit gate** — COMPLETE — issue #29
  D036 saturated all 2,048 lock records, stressed all 64 D031/D032 rows, ran 2,048 full queue cycles / 6,144 rejects, combined tracker+queue+editor+UI stress, all D032/D033 byte values, 4,096 rows / 64 wraps, retained the six-hour transport proof and completed a fail-fast realtime audit. No production defect was found; production source/resources/hashes remain exactly WP3.9. Final run `40084706845`, job `120845144453`. Evidence: `WP3_10_TEST_PLAN.md`, `WP3_10_VERIFICATION.md`.
'''
s = once(s, anchor, addition, 'WORK_PACKAGES WP3.10 insertion')

old = '''Current verified baseline after WP3.9:

- app image: 66,960 B / 581,564 B
- normal RAM: 13,296 B / 98,304 B
- pool RAM: 113,000 B / 344,064 B
- D031 playback row cache: 256 B ordinary RAM
- D032 effect row cache: 256 B ordinary RAM
- D032 effect runtime: 12 B
- D033 row-event runtime: 4 B
- D034 runtime queue: 268 B ordinary RAM + 4 B overflow acknowledgement
- D035 `FmtTrackerScreen`: 62 B ordinary RAM, ten regions / max two redraws
- default WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- D031 locked WAV: `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`
- package identity: `FM-1_987`

## Next package

- **WP3.10 — Phase 3 worst-case tests and exit gate** — IN PROGRESS — issue #29.
  D036 is frozen in `WP3_10_TEST_PLAN.md`. Stress the complete D027-D035 stack and close Phase 3; production source remains unchanged unless a frozen stress test exposes a real defect.

Remaining Phase 3 order:

- WP3.10 Phase 3 worst-case tests and exit gate
'''
new = '''Final verified Phase 3 baseline after WP3.10:

- app image: 66,960 B / 581,564 B
- normal RAM: 13,296 B / 98,304 B
- pool RAM: 113,000 B / 344,064 B
- D031 playback row cache: 256 B ordinary RAM
- D032 effect row cache: 256 B ordinary RAM
- D032 effect runtime: 12 B
- D033 row-event runtime: 4 B
- D034 runtime queue: 268 B ordinary RAM + 4 B overflow acknowledgement
- D035 `FmtTrackerScreen`: 62 B ordinary RAM, ten regions / max two redraws
- default WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- D031 locked WAV: `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`
- final `fm1tracker.bin`: `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- final `fm1tracker-0.1.fwsc`: `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity: `FM-1_987`
- Phase 3 exit run: `40084706845`, job `120845144453`

Phase 3 is complete at host/device-build level. Physical FM-1 verification remains a later explicit gate.
'''
s = once(s, old, new, 'WORK_PACKAGES final baseline')

s = once(s,
    '# Phase 4 — 8-BIT engine\n\n- WP4.1 engine interface contract',
    '# Phase 4 — 8-BIT engine\n\nStatus: **READY TO START — WP4.1 / #31; D037 required before production engine source**\n\nParent issue: #30\n\n- **WP4.1 engine interface contract** — READY TO START — issue #31',
    'WORK_PACKAGES Phase4 start')
write(p, s)

# ---------------------------------------------------------------------------
# ROADMAP — mark all Phase 3 green and Phase 4 next.
p = 'docs/ROADMAP.md'
s = read(p)
s = once(s,
    'Status: **IN PROGRESS — WP3.1–WP3.9 complete; WP3.10 active**',
    'Status: **COMPLETE — WP3.1–WP3.10 host/device-build exit satisfied**',
    'ROADMAP phase3 status')
s = once(s,
    '10. 🟨 **WP3.10 — Phase 3 worst-case tests and exit gate**, issue #29, IN PROGRESS. D036 frozen in `WP3_10_TEST_PLAN.md`.',
    '10. ✅ **WP3.10 — Phase 3 worst-case tests and exit gate**. D036 found no production defect after full-pool/cache/queue/UI/timing/combined stress and fail-fast realtime audit. Final run `40084706845`; evidence `WP3_10_TEST_PLAN.md`, `WP3_10_VERIFICATION.md`.',
    'ROADMAP WP3.10')
s = once(s,
    'Current verified Phase 3 footprint after WP3.9: 66,960 B image, 13,296 B normal RAM and 113,000 B pool. `FmtTrackerScreen` is 62 B; the historical default WAV plus every frozen D031-D034 audio fixture remain byte-identical.\n\nImmediate package: **WP3.10 / #29**. D036 is frozen; execute worst-case/combined stress and the final Phase 3 exit gate before any Phase 4 production source.\n\nExit criteria remain:\n\n- worst-case lock/effect data inside budget;\n- no lock/effect edit corrupts audio/song state;\n- effect behavior host-tested and documented;\n- all event work bounded.',
    'Final verified Phase 3 footprint after WP3.10: 66,960 B image, 13,296 B normal RAM and 113,000 B pool, with zero production delta versus WP3.9. The historical default WAV plus every frozen D031-D035 regression remain deterministic. Final exit run `40084706845`, job `120845144453`.\n\nPhase 3 exit criteria are satisfied at host/device-build level: worst-case lock/effect data is inside budget, editing did not corrupt song/audio state, effect/event/queue behavior is bounded and deterministic, combined stress recovered safely, and the pinned target build remained byte-identical. Physical FM-1 verification remains later hardware work.\n\nImmediate next package: **Phase 4 / WP4.1 / #31**. Freeze D037 engine/voice/realtime architecture before any 8-BIT engine production source.',
    'ROADMAP phase3 final paragraph')

# Add status/parent to Phase 4 heading if not already present.
s = once(s,
    '## Phase 4 — 8-BIT engine\n\nGoal: a musically complete chip-style palette.',
    '## Phase 4 — 8-BIT engine\n\nGoal: a musically complete chip-style palette.\n\nStatus: **READY TO START — WP4.1 architecture first**\n\nParent issue: #30  \nNext package: WP4.1 / #31 — freeze D037 before engine source.',
    'ROADMAP Phase4 status')
write(p, s)

# ---------------------------------------------------------------------------
# TEST_MATRIX — D036 all green and final evidence durable.
p = 'docs/TEST_MATRIX.md'
s = read(p)
repls = {
'| D036 full 2,048-record lock-pool saturation/recovery stress | required | required | n/a | 🟨 WP3.10 planned |':
'| D036 full 2,048-record lock-pool saturation/recovery stress | required | required | n/a | ✅ 2,048 records / 512 owners / hash `4f426945` |',
'| D036 D032/D033 complete timing/boundary stress | required | required | n/a | 🟨 WP3.10 planned |':
'| D036 D032/D033 complete timing/boundary stress | required | required | n/a | ✅ all 256 values / command; 4,096 rows / 64 wraps |',
'| D036 D034 repeated full/wrap/overflow-storm/recovery stress | required | required | later | 🟨 WP3.10 planned |':
'| D036 D034 repeated full/wrap/overflow-storm/recovery stress | required | required | later | ✅ 2,048 full cycles / 6,144 rejects + recovery |',
'| D036 combined playback + queue + editor + D035 backlog stress | required | required | later | 🟨 WP3.10 planned |':
'| D036 combined playback + queue + editor + D035 backlog stress | required | required | later | ✅ deterministic; two overflow epochs; recovery; screen-neutral queue |',
'| D036 static real-time boundedness audit | inspect/test | required | n/a | 🟨 WP3.10 planned |':
'| D036 static real-time boundedness audit | inspect/test | required | n/a | ✅ fail-fast audit; no allocator/list/screen in audio; queue cap 32 |',
'| D036 final Phase 3 inherited regression/device-build exit gate | required | required | hardware caveat | 🟨 WP3.10 planned |':
'| D036 final Phase 3 inherited regression/device-build exit gate | required | required | hardware caveat | ✅ run `40084706845`; zero production delta; physical hardware pending |',
}
for old, new in repls.items():
    s = once(s, old, new, 'TEST_MATRIX ' + old[:24])
s = once(s,
    '- `WP3_10_TEST_PLAN.md` (D036 active exit plan)\n',
    '- `WP3_10_TEST_PLAN.md`\n- `WP3_10_VERIFICATION.md` — final Phase 3 exit evidence\n',
    'TEST_MATRIX WP3.10 evidence')
write(p, s)

# ---------------------------------------------------------------------------
# CHANGELOG — durable final Phase 3 closure entry.
p = 'CHANGELOG.md'
s = read(p)
assert '## 2026-10-24 - WP3.10 / Phase 3 closeout' not in s
s += '''

## 2026-10-24 - WP3.10 / Phase 3 closeout

- Froze D036 in `docs/WP3_10_TEST_PLAN.md` as a verification/stress-only package; no Phase 4 feature work was pulled forward.
- Saturated the complete D028 pool: 2,048 records across 512 owners, deterministic exhaustion/replacement/recovery, final state hash `4f426945`.
- Stressed all 64 D031/D032 proof-lane rows at four locks per row and verified both fixed 256 B caches.
- Ran 2,048 full D034 queue cycles with 6,144 reject-newest events across wrap, alternating FIFO drain/O(1) discard and post-storm recovery.
- Proved D035 worst-case visible inspection at 32 lock records/sync and deterministic ten-region/two-redraw backlog convergence.
- Added combined full-app tracker/effect/p-lock/editor/UI/queue stress with two overflow epochs; stress was deterministic, recovered completely, emitted no p-lock MIDI and left framebuffers queue-neutral.
- Exhaustively exercised all 256 values for each D032 command and each D033 RETRIG/DELAY/CUT command, then ran 4,096 rows / 64 wraps / 20,480 effect ticks with final step/phase zero.
- Retained the independent six-hour D020 transport proof and exact 120 BPM tick anchors `919/1838/2757/3675/4594`, next row `5513`.
- Hardened the realtime audit after detecting a test-runner `tee` exit-mask hazard; the corrected fail-fast audit verifies no allocator, no audio lock traversal/screen calls, callback queue cap 32, O(1) discard, no SPSC spin/IRQ masking, integer-only realtime units and one D020-derived six-tick clock.
- D036 found no production defect. WP3.10 production delta is exactly zero versus WP3.9 checkpoint `354bf7d356774ad828f1f660bd456dbab7f0490d`.
- Final Phase 3 resources remain 66,960 B image / 13,296 B normal RAM / 113,000 B pool.
- Final firmware remains `fm1tracker.bin` `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27` and `fm1tracker-0.1.fwsc` `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`.
- Final closeout: run `40084706845`, job `120845144453`, artifact `12556590742`, artifact SHA-256 `c4f494716ec98507ca9af6fe59313c01116136deececa7ba84e24598e78b82ee`.
- Phase 3 / #20 and WP3.10 / #29 are closed complete at host/device-build level. Physical FM-1 install/audio/ergonomics/LCD/execution-margin verification remains later explicit hardware work.

### Next

Phase 4 / #30 is ready to start. WP4.1 / #31 must freeze D037 engine/voice/realtime architecture and resource budgets before any PULSE/NOISE/TRIANGLE/WAVETABLE-8 production source.
'''
write(p, s)

print('PHASE3_FINAL_HANDOFF_OK=1')
