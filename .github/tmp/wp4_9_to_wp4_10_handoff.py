from pathlib import Path


def once(path, old, new):
    p = Path(path)
    s = p.read_text()
    n = s.count(old)
    assert n == 1, (path, old[:120], n)
    p.write_text(s.replace(old, new, 1))

# PROJECT_STATE — authoritative current state and next action.
ps = 'docs/PROJECT_STATE.md'
once(ps,
     'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 dinghy/audio migration next - 2026-10-24**',
     'State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 representative benchmark + Phase 4 closeout active - 2026-10-24**')
once(ps, 'Current work package: WP4.9 / #40', 'Current work package: WP4.10 / #41')
once(ps,
     '## Next milestone\n\n**WP4.9 - linked 8-BIT engine integration**',
     '## Next milestone\n\n**WP4.10 - representative eight-track benchmark and Phase 4 closeout**')
once(ps,
     'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 linked dinghy/audio migration next**',
     'State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 active; final representative benchmark/exit contract must be frozen before new production changes**')
once(ps, 'WP4.9 issue: #40 — IN PROGRESS', 'WP4.9 issue: #40 — COMPLETE\nWP4.10 issue: #41 — IN PROGRESS')
once(ps,
     'WP4.8 closed on run `40104723586`, job `120903925381`. D044 passed six-ID/four-page deterministic verification, all 256 WIDTH/TABLE mappings, 4,096 exhaustive engine×slot×value resolver cases, pinned PI32v2 compilation and full-app closeout with zero linked delta. Evidence: `docs/WP4_8_VERIFICATION.md`.\n\nD045 is frozen in `docs/WP4_9_LINKED_ENGINE_CONTRACT.md`. WP4.9 deliberately crosses the isolation boundary in staged gates: track-aware playback/D031/D032 publication first, pure linked-engine state/mix second, D044 registry/cache/editor activation third, and only then `dinghy.c` proof-sine migration.\n\nD045 must define exact eight-track/eight-voice ownership, engine selection/fallback, D044 registry/cache activation, D031/D032 pitch/level precedence, D033 NOTE DELAY/RETRIG/CUT ordering, D042 articulation, D043 allocation/release-tail semantics, oscillator reset rules, mix/clamp ownership, proof-sine migration, deterministic per-engine/mixed fixtures, linked resource ceilings and the boundary to WP4.10 benchmarking.\n\nDo not begin the final eight-track benchmark/Phase 4 closeout until WP4.9 integrated host and target evidence is green.',
     'WP4.9 closed on run `40116897104`, job `120939922897`. D045 now links all eight tracker tracks through the real bounded 8-BIT engine and passed canonical per-engine/mixed/eight-track/D033/precedence fixtures, inherited non-obsolete regressions, realtime audit and pinned target build. Final resources: 72,504 B image / 19,604 B normal RAM / 113,000 B pool. Evidence: `docs/WP4_9_VERIFICATION.md`.\n\nWP4.10 / #41 is the only remaining Phase 4 package. It must freeze a representative benchmark/exit contract before any new production change, then prove the linked D037-D045 path under a deterministic eight-track representative workload and finish the Phase 4 exit gate.\n\nDo not begin Phase 5/FM4 until WP4.10 is green and Phase 4 is formally closed.')
once(ps,
     '- Preserve the historical default WAV unless a later deliberate decision supersedes it with explicit evidence.',
     '- D045 deliberately supersedes the historical proof-sine default WAV for the tracker lane. The linked 8-BIT full-app reference is `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` / 286,464 frames; preserve it unless a later explicit decision supersedes it with evidence.')
once(ps,
     'Begin D045 Gate 4 only: migrate `dinghy.c` from the Phase 3 proof-sine tracker lane to the bounded 8-BIT engine, seed deterministic 8-BIT instruments/base sets, link all eight tracker tracks through the Gate 1-3 publication/engine layers, preserve the separate D034 audition-sine lane, and add deterministic per-engine/mixed/eight-track/D033/precedence fixtures plus pinned target resource proof. Do not begin WP4.10 timing/Phase 4 closeout until Gate 4 is green.',
     'Freeze the WP4.10 / #41 representative eight-track benchmark and Phase 4 exit contract before any new production change. The benchmark must exercise the linked D037-D045 path, retain bounded realtime ownership, re-run applicable inherited stress/regression gates, freeze pinned target resources/artifact hashes and make no physical FM-1 execution-margin claim without hardware. Do not begin Phase 5/FM4 until WP4.10 is green.')
# Insert WP4.9 closeout after Gate 3 checkpoint.
once(ps,
     '- WP4.9 D045 Gate 3 verification: `9263b754c877dc0e6076c34538a496820e2ededb` — run `40111447360`, host `120923852664`, PI32v2 `120923852448`; evidence `WP4_9_GATE3_VERIFICATION.md`',
     '- WP4.9 D045 Gate 3 verification: `9263b754c877dc0e6076c34538a496820e2ededb` — run `40111447360`, host `120923852664`, PI32v2 `120923852448`; evidence `WP4_9_GATE3_VERIFICATION.md`\n- WP4.9 D045 final closeout: `89fabc51348078101c8e376bba7814af9d42ae80` — run `40116897104`, job `120939922897`; artifact `12570465704`; evidence `WP4_9_VERIFICATION.md`')
# Replace final Last updated paragraph wholesale.
p = Path(ps); s = p.read_text(); marker = '## Last updated\n\n'; i = s.index(marker) + len(marker)
s = s[:i] + '2026-10-24 - WP4.9 / #40 COMPLETE. D045 linked the real eight-track tracker lane to PULSE/NOISE/TRIANGLE/WAVETABLE-8, preserved bounded D031-D034 ownership, passed canonical Gate 5 fixtures + Gate 6 inherited/realtime closeout, and built at 72,504 B image / 19,604 B normal RAM / 113,000 B pool. Final run `40116897104`, job `120939922897`; evidence `WP4_9_VERIFICATION.md`. WP4.10 / #41 is now the only active Phase 4 package; freeze its representative benchmark/exit contract before any production change.\n'
p.write_text(s)

# WORK_PACKAGES — execution order only.
wp = 'docs/WORK_PACKAGES.md'
once(wp,
     'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 next**',
     'Status: **IN PROGRESS — WP4.1 / #31 through WP4.9 / #40 COMPLETE; WP4.10 / #41 representative benchmark + Phase 4 closeout active**')
once(wp,
     '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gates 1-3 GREEN (`WP4_9_GATE1_VERIFICATION.md`, `WP4_9_GATE2_VERIFICATION.md`, `WP4_9_GATE3_VERIFICATION.md`); Gate 4 `dinghy.c` tracker-lane migration + integrated host/target proof next\n- WP4.10 eight-track hardware benchmark',
     '- **WP4.9 linked 8-BIT engine integration** — COMPLETE — issue #40 — D045 + `WP4_9_VERIFICATION.md`; final run `40116897104`, job `120939922897`; linked target 72,504 B image / 19,604 B normal RAM / 113,000 B pool\n- **WP4.10 representative eight-track benchmark + Phase 4 closeout** — IN PROGRESS — issue #41 — freeze benchmark/exit contract before any production change')

# ROADMAP — current phase pointer.
rm = 'docs/ROADMAP.md'
once(rm,
     'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**. Gates 1-3 are GREEN; Gate 4 `dinghy.c` tracker-lane migration + integrated host/target proof is next.',
     'Phase 4 is active. **WP4.1 / #31 through WP4.9 / #40 are COMPLETE**. The linked D037-D045 8-BIT tracker lane is production audio and passed final WP4.9 host/target closeout. **WP4.10 / #41** is now the only active Phase 4 package: representative eight-track benchmark + final Phase 4 exit gate.')
once(rm,
     'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 next**',
     'Status: **IN PROGRESS — WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 representative benchmark + Phase 4 closeout active**')
once(rm,
     'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; Gates 1-3 are GREEN (`40108097887`, `40109064516`, `40111447360`). Execute Gate 4 `dinghy.c` tracker-lane migration + integrated 8-BIT host/target proof next. Preserve the separate D034 audition-sine lane and keep WP4.10 timing/closeout blocked until Gate 4 is green. Branch: `phase4/8bit-engine`.',
     'Active package: WP4.10 / #41 — WP4.9 closed on run `40116897104`, job `120939922897`; evidence `WP4_9_VERIFICATION.md`. Freeze the representative eight-track benchmark/Phase 4 exit contract before any new production change, then close Phase 4 without claiming physical execution margin until hardware exists. Branch: `phase4/8bit-engine`.')

# TEST_MATRIX — turn now-proven WP4.9 obligations green and point to final package.
tm = 'docs/TEST_MATRIX.md'
once(tm,
     '| Integrated PULSE/NOISE/TRIANGLE/WAVETABLE host fixtures through real adapter | required | required | optional later | ⬜ WP4.9 |',
     '| Integrated PULSE/NOISE/TRIANGLE/WAVETABLE host fixtures through real adapter | required | required | optional later | ✅ WP4.9 run `40116897104` |')
once(tm,
     '| Integrated NOTE/TIE/OFF/DELAY/RETRIG/CUT + D031/D032/D044 precedence | required | required | optional later | ⬜ WP4.9 |',
     '| Integrated NOTE/TIE/OFF/DELAY/RETRIG/CUT + D031/D032/D044 precedence | required | required | optional later | ✅ WP4.9 canonical Gate 5 |')
once(tm,
     '| Linked engine resources remain inside D037 ceilings | inspect/build | required | n/a | ⬜ WP4.9 |',
     '| Linked engine resources remain inside D037 ceilings | inspect/build | required | n/a | ✅ 72,504 B image / 19,604 B RAM / 113,000 B pool |')
once(tm,
     '| P-lock modulation stable | required | required | required later | ⬜ WP4.9 |',
     '| P-lock modulation stable | required | required | required later | ✅ D031/D032/D044 precedence + page fixtures green |')
once(tm,
     'WP4.9 Gate 3 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE3_VERIFICATION.md`.\nWP4.8 normative contract: `WP4_8_PARAMETER_PAGE_CONTRACT.md`.',
     'WP4.9 Gate 3 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE3_VERIFICATION.md`.\nWP4.9 final linked-engine evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_VERIFICATION.md`.\nWP4.8 normative contract: `WP4_8_PARAMETER_PAGE_CONTRACT.md`.')

# START_HERE — stale handoff note.
sh = 'START_HERE.md'
once(sh,
     'For the current active package, also read the most recently completed verification and the active package contract named by `docs/PROJECT_STATE.md`. As of the 2026-10-24 handoff, WP3.8 is verified and WP3.9 / #28 is next with D035 required before production UI source.',
     'For the current active package, also read the most recently completed verification and the active package contract named by `docs/PROJECT_STATE.md`. As of the current 2026-10-24 handoff, WP4.9 / D045 is verified in `docs/WP4_9_VERIFICATION.md`, and WP4.10 / #41 is the only active Phase 4 package; freeze its representative benchmark/exit contract before any new production change.')

print('WP4_9_TO_WP4_10_HANDOFF_OK=1')
