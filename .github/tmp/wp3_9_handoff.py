from pathlib import Path


def read(path):
    return Path(path).read_text()


def write(path, text):
    Path(path).write_text(text)


def once(text, old, new, label):
    n = text.count(old)
    assert n == 1, f'{label}: expected 1 match, got {n}'
    return text.replace(old, new, 1)

# DECISIONS: append D035/D036 only to the complete existing file.
p = 'docs/DECISIONS.md'
s = read(p)
assert '## D035 —' not in s
assert '## D036 —' not in s
s = s.rstrip() + '''


## D035 — WP3.9 extends the existing bounded tracker surface with lock/effect feedback

Date: 2026-10-24
Status: accepted and verified

**Decision:** WP3.9 keeps the D026 240×240 tracker screen, ten redraw regions and maximum two tracker-region renders per main-loop poll. It adds no screen/page/menu architecture and no audio/ISR display coupling. The normative detailed contract is `docs/WP3_9_VISUAL_CONTRACT.md`.

Rows are `ST NOTE IN VOL LK FX`. `LK` summarizes D028 storage as `--`, `L1..L4`, or `L!`; while the D029/D030 held-step editor owns that visible row it becomes `P0..P4` or `P!`. The existing D026 `>` cursor and `*` playhead remain independent and unchanged.

`FX` is a four-character view of stored `FmtStep.effect/effect_value`: `----`, `ARhh`, `SUhh`, `SDhh`, `POhh`, `VIhh`, `RThh`, `DLhh`, `CThh`, or `????` for reserved commands. The raw stored value is shown in uppercase hexadecimal; presentation never clamps or reinterprets D032/D033 semantics.

TRACK PAN remains storage-only under D031. The row summary intentionally reports only lock storage/count. Detailed D029/D030 footer feedback appends `STORE` when the selected parameter is TRACK PAN so the storage-only status is visible without inventing a second row-level classification.

`FmtTrackerScreen` is frozen at exactly 62 bytes ordinary RAM, still below the original D026 64-byte cap. Each sync inspects only the same eight visible rows and performs at most one bounded `fmt_lock_count()` per row, therefore at most 32 lock-record inspections. No pool allocation or retained mutable lock pointer is introduced.

**Reason:** the existing D026 row already reserved an FX column but intentionally deferred stable effect names, while D028-D030 p-lock state had detailed footer feedback but no normal-row presence indication. D035 makes the now-stable Phase 3 semantics legible without increasing redraw topology or crossing the main/audio ownership boundary.

**Consequence:** WP3.9 changes presentation only. The reviewed active/released framebuffer PPM hashes are `d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3` and `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`. All frozen D031-D034 audio/runtime behavior remains byte-identical. Final resources are 66,960 B image / 13,296 B normal RAM / 113,000 B pool. Evidence is `docs/WP3_9_VERIFICATION.md`.

## D036 — WP3.10 is a worst-case verification and Phase 3 exit package, not a feature package

Date: 2026-10-24
Status: accepted before WP3.10 stress implementation

**Decision:** WP3.10 starts from the final WP3.9 production checkpoint `354bf7d356774ad828f1f660bd456dbab7f0490d` and freezes its complete stress/exit obligations in `docs/WP3_10_TEST_PLAN.md`. Production source should remain byte-identical unless a stress test exposes a real defect; any such fix must be minimal, receive a focused regression and trigger the entire exit gate again.

D036 requires worst-case D028 pool saturation/recovery, D031/D032 cache boundaries, a full D032/D033 timing matrix, D034 saturation/wrap/overflow storms, combined playback/audition/editor/UI stress, D035 maximum visible-row lock inspection and dirty-backlog convergence, repeated editor/context transitions, long deterministic clock checks, static real-time boundedness audit, all inherited regressions and the pinned JieLi/AC79 target-build/resource gate.

**Reason:** Phase 3 has accumulated several individually bounded systems whose final risk is interaction under simultaneous worst-case load rather than another missing feature. Closing the phase requires proving those declared bounds together before engine work begins.

**Consequence:** Phase 4 production work is blocked until WP3.10 passes the D036 exit criteria and Phase 3 is formally closed. WP3.10 may add tests/tooling/docs; it may change production only to fix a defect demonstrated by the frozen stress plan.
'''
write(p, s)

# PROJECT_STATE: promote WP3.9 and make WP3.10 sole active package.
p = 'docs/PROJECT_STATE.md'
s = read(p)
s = once(s,
    'State: **IN PROGRESS - WP3.1-WP3.8 complete; WP3.9 ready for D035 visual/update freeze - 2026-10-24**',
    'State: **IN PROGRESS - WP3.1-WP3.9 complete; WP3.10 active, D036 frozen before stress implementation - 2026-10-24**',
    'PROJECT_STATE phase status')
marker = 'Historical default WAV, D031 locked WAV and all frozen WP3.6/WP3.7 audio fixtures remain byte-identical through WP3.8.\n\n## Next milestone'
replacement = '''Historical default WAV, D031 locked WAV and all frozen WP3.6/WP3.7 audio fixtures remain byte-identical through WP3.8.

### WP3.9 - lock/effect visual feedback - COMPLETE - #28

Final verified production checkpoint:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

Final GitHub Actions:

- run `40083510351`
- job `120841687772` (`visual-feedback-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_9_VERIFICATION.md`
Normative contract: `docs/WP3_9_VISUAL_CONTRACT.md`

Evidence artifact:

- ID `12555799262`
- ZIP SHA-256 `62bdffec5faa027484ccc8bf99b2babbb978874054182de97970b8d52f333bca`

D035 keeps the D026 ten-region / max-two-redraw tracker surface and adds `LK` p-lock summaries plus four-character stored effect mnemonics. `FmtTrackerScreen` is exactly 62 B. TRACK PAN detailed feedback is marked `STORE`; D031-D034 runtime/audio semantics are unchanged.

Reviewed framebuffers:

- active target `d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3`
- released rows `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`

Final WP3.9 resources:

| Resource | Used | Limit | Free | Delta vs WP3.8 |
|---|---:|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B | +652 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B | +16 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`.

Historical default WAV, D031 locked WAV, all frozen D032/D033 fixtures and all D034 queue fixtures remain byte-identical through WP3.9.

## Next milestone'''
s = once(s, marker, replacement, 'PROJECT_STATE WP3.9 insertion')
start = '''**WP3.9 - lock/effect visual feedback**

State: **READY TO START - D035 visual/update contract required before production UI source**

GitHub issue: #28

D035 must freeze exactly what p-lock/effect state is shown, compact notation, cursor/playhead/edit-state priority, screen/update ownership, dirty-region bounds, stale-feedback cancellation and deterministic framebuffer evidence. No audio ISR may draw or mutate display state.

Do not write WP3.9 production visual-feedback source before D035 is frozen. WP3.10 remains the Phase 3 final worst-case closeout and must not be pulled forward.'''
next_block = '''**WP3.10 - Phase 3 worst-case tests and exit gate**

State: **IN PROGRESS - D036 frozen before stress implementation**

GitHub issue: #29

Normative test/exit plan: `docs/WP3_10_TEST_PLAN.md`.

WP3.10 must prove the complete D027-D035 stack under its declared worst cases, including full lock-pool pressure, exact effect/event timing, queue overflow storms/recovery, combined runtime/editor/UI stress, D035 redraw convergence, inherited deterministic audio and the pinned target-build/resource gate. Production source remains unchanged unless a frozen stress test exposes a real defect.

Phase 4 production work is blocked until WP3.10 passes and Phase 3 is formally closed.'''
s = once(s, start, next_block, 'PROJECT_STATE next milestone')
s = once(s,
    '- D034: runtime ingress is an exact 32-entry / 268-byte SPSC main->audio queue with 8-byte events, reject-newest overflow and fail-to-silence audition recovery; D024/D033 remain outside it.\n',
    '- D034: runtime ingress is an exact 32-entry / 268-byte SPSC main->audio queue with 8-byte events, reject-newest overflow and fail-to-silence audition recovery; D024/D033 remain outside it.\n- D035: tracker presentation remains ten regions / max two redraws, `FmtTrackerScreen` 62 B, with bounded lock-count and stored-effect feedback only.\n- D036: WP3.10 is a stress/exit package; Phase 4 is blocked until the complete D027-D035 stack passes worst-case combined verification.\n',
    'PROJECT_STATE decisions')
s = once(s,
    'Start WP3.9 / #28 by freezing D035 visual/update ownership and notation before production UI source. Preserve D026 bounded screen work and all D031-D034 audio/runtime behavior.',
    'Implement WP3.10 / #29 exactly from `docs/WP3_10_TEST_PLAN.md`: build the focused worst-case harnesses first, then combined full-app stress and final Phase 3 closeout. Do not begin Phase 4 production source.',
    'PROJECT_STATE next action')
s = once(s,
    '- WP3.8 verified: `5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`\n',
    '- WP3.8 verified: `5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`\n- WP3.9 verified production: `354bf7d356774ad828f1f660bd456dbab7f0490d`\n',
    'PROJECT_STATE checkpoint')
s = once(s,
    '2026-10-24 - WP3.8 complete and #27 closed at clean checkpoint `5c282892...`. WP3.9 / #28 is next; freeze D035 before production visual-feedback source.',
    '2026-10-24 - WP3.9 complete and #28 closed after successful run `40083510351`. WP3.10 / #29 is active; D036 is frozen in `WP3_10_TEST_PLAN.md` before stress implementation.',
    'PROJECT_STATE updated')
write(p, s)

# WORK_PACKAGES.
p = 'docs/WORK_PACKAGES.md'
s = read(p)
s = once(s,
    'Status: **IN PROGRESS — WP3.1–WP3.8 complete; WP3.9 next**',
    'Status: **IN PROGRESS — WP3.1–WP3.9 complete; WP3.10 active**',
    'WORK_PACKAGES status')
wp38 = '''- **WP3.8 — bounded event queue and overflow policy** — COMPLETE — issue #27
  D034 freezes an 8-byte event envelope and exact 32-entry / 268-byte SPSC main->audio queue, reject-newest overflow, wrap-safe sequence counters and click-safe fail-to-silence audition recovery. Physical-key and USB-MIDI audition no longer mutate manual audio voices from main. Final clean-tree run `40079824265`, job `120830854974`. Final resources: 66,308 B image / 13,280 B normal RAM / 113,000 B pool. Evidence: `WP3_8_QUEUE_CONTRACT.md`, `WP3_8_VERIFICATION.md`.
'''
wp39 = wp38 + '''
- **WP3.9 — lock/effect visual feedback** — COMPLETE — issue #28
  D035 extends the existing ten-region / max-two-redraw tracker surface with `LK` p-lock summaries, stored D032/D033 effect mnemonics/values and `STORE` detail for TRACK PAN while preserving main-only bounded drawing. `FmtTrackerScreen` is exactly 62 B. Final run `40083510351`, job `120841687772`. Final resources: 66,960 B image / 13,296 B normal RAM / 113,000 B pool. Evidence: `WP3_9_VISUAL_CONTRACT.md`, `WP3_9_VERIFICATION.md`.
'''
s = once(s, wp38, wp39, 'WORK_PACKAGES WP3.9 completed')
old = '''Current verified baseline after WP3.8:

- app image: 66,308 B / 581,564 B
- normal RAM: 13,280 B / 98,304 B
- pool RAM: 113,000 B / 344,064 B
- D031 playback row cache: 256 B ordinary RAM
- D032 effect row cache: 256 B ordinary RAM
- D032 effect runtime: 12 B
- D033 row-event runtime: 4 B
- D034 runtime queue: 268 B ordinary RAM + 4 B overflow acknowledgement
- default WAV: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- D031 locked WAV: `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`
- package identity: `FM-1_987`

## Next package

- **WP3.9 — lock/effect visual feedback** — READY TO START — issue #28.
  Freeze D035 before production UI source. D035 must define compact p-lock/effect notation, normal-view versus edit-only feedback, cursor/playhead/edit priority, screen-state ownership, bounded dirty regions/redraw work, stale-state cancellation and deterministic framebuffer evidence. Audio/ISR behavior remains untouched.

Remaining Phase 3 order:

- WP3.9 lock/effect visual feedback
- WP3.10 Phase 3 worst-case tests'''
new = '''Current verified baseline after WP3.9:

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

- WP3.10 Phase 3 worst-case tests and exit gate'''
s = once(s, old, new, 'WORK_PACKAGES baseline/next')
write(p, s)

# ROADMAP.
p = 'docs/ROADMAP.md'
s = read(p)
s = once(s,
    'Status: **IN PROGRESS — WP3.1–WP3.8 complete; WP3.9 next**',
    'Status: **IN PROGRESS — WP3.1–WP3.9 complete; WP3.10 active**',
    'ROADMAP status')
s = once(s,
    '9. ⬜ **WP3.9 — lock/effect visual feedback**, issue #28, READY FOR D035 VISUAL/UPDATE FREEZE.\n10. ⬜ WP3.10 — Phase 3 worst-case tests.',
    '9. ✅ **WP3.9 — lock/effect visual feedback**. D035 preserves the ten-region / max-two-redraw surface, adds bounded `LK` and stored `FX` feedback, and keeps every audio/runtime fixture unchanged. Final run `40083510351`; evidence `WP3_9_VISUAL_CONTRACT.md`, `WP3_9_VERIFICATION.md`.\n10. 🟨 **WP3.10 — Phase 3 worst-case tests and exit gate**, issue #29, IN PROGRESS. D036 frozen in `WP3_10_TEST_PLAN.md`.',
    'ROADMAP progress')
s = once(s,
    'Current verified Phase 3 footprint after WP3.8: 66,308 B image, 13,280 B normal RAM and 113,000 B pool. The historical default WAV, D031 locked WAV and every frozen WP3.6/WP3.7 fixture remain byte-identical.\n\nImmediate next package: **WP3.9 / #28**. Freeze D035 visual/update ownership and compact notation before production UI source; preserve the bounded D026 redraw model and all D031-D034 runtime/audio semantics.',
    'Current verified Phase 3 footprint after WP3.9: 66,960 B image, 13,296 B normal RAM and 113,000 B pool. `FmtTrackerScreen` is 62 B; the historical default WAV plus every frozen D031-D034 audio fixture remain byte-identical.\n\nImmediate package: **WP3.10 / #29**. D036 is frozen; execute worst-case/combined stress and the final Phase 3 exit gate before any Phase 4 production source.',
    'ROADMAP footprint/next')
write(p, s)

# TEST_MATRIX: add D035 verified rows plus D036 pending stress obligations.
p = 'docs/TEST_MATRIX.md'
s = read(p)
anchor = '| WP3.8 preserves all Phase 2 + WP3.1-WP3.7 regressions | required | required | later | ✅ run `40079824265` |\n'
addition = anchor + '''| D035 tracker presentation state exactly 62 B and <= D026 64 B cap | required | required | n/a | ✅ WP3.9 |
| D035 redraw topology remains exactly 10 regions / max 2 per poll | required | compile | required later | ✅ WP3.9 |
| D035 lock summaries `--/L1..L4/P0..P4/L!/P!` deterministic | required | compile | required later | ✅ WP3.9 |
| D035 stored effect command/value mnemonics deterministic | required | compile | required later | ✅ `----/AR/SU/SD/PO/VI/RT/DL/CT/????` |
| TRACK PAN detailed feedback explicitly marks storage-only state | required | compile | required later | ✅ `STORE` suffix |
| D035 visible-row sync bounded to <=32 lock-record inspections | inspect/test | compile | n/a | ✅ 8 rows × max 4 locks |
| D035 held-target begin/end and row/effect changes dirty only bounded regions | required | compile | required later | ✅ focused D035 tests |
| D035 HOME return clears stale feedback | required | compile | required later | ✅ pixel-identical control |
| D035 reviewed active/released framebuffers frozen | required | compile | required later | ✅ `d2904153...` / `ec15c6db...` |
| WP3.9 preserves historical default and all D031-D034 audio fixtures | required | required | n/a | ✅ run `40083510351` |
| WP3.9 final resource delta measured/frozen | n/a | required | n/a | ✅ +652 B image / +16 B RAM / +0 B pool |
| D036 full 2,048-record lock-pool saturation/recovery stress | required | required | n/a | 🟨 WP3.10 planned |
| D036 D032/D033 complete timing/boundary stress | required | required | n/a | 🟨 WP3.10 planned |
| D036 D034 repeated full/wrap/overflow-storm/recovery stress | required | required | later | 🟨 WP3.10 planned |
| D036 combined playback + queue + editor + D035 backlog stress | required | required | later | 🟨 WP3.10 planned |
| D036 static real-time boundedness audit | inspect/test | required | n/a | 🟨 WP3.10 planned |
| D036 final Phase 3 inherited regression/device-build exit gate | required | required | hardware caveat | 🟨 WP3.10 planned |
'''
s = once(s, anchor, addition, 'TEST_MATRIX D035/D036 rows')
s = once(s,
    '- `WP3_8_VERIFICATION.md`\n',
    '- `WP3_8_VERIFICATION.md`\n- `WP3_9_VERIFICATION.md`\n- `WP3_10_TEST_PLAN.md` (D036 active exit plan)\n',
    'TEST_MATRIX evidence')
write(p, s)

# CHANGELOG: append milestone summary.
p = 'CHANGELOG.md'
s = read(p).rstrip()
assert '## 2026-10-24 - WP3.9 lock/effect visual feedback' not in s
s += '''


## 2026-10-24 - WP3.9 lock/effect visual feedback

- Froze D035 before production UI changes in `docs/WP3_9_VISUAL_CONTRACT.md`.
- Kept the D026 240×240 tracker surface at exactly ten regions and max two tracker redraws per main-loop poll.
- Added `LK` row summaries for zero/one-to-four locks, active held-step targets and presentation errors.
- Replaced the Phase-2 placeholder FX display with compact stored D032/D033 command/value mnemonics such as `AR37`, `RT02`, `DL03` and `CT00`.
- Marked detailed TRACK PAN p-lock feedback with `STORE` while preserving its D031 storage-only semantics.
- Grew `FmtTrackerScreen` from 44 B to exactly 62 B, still under the original 64 B D026 budget; linked normal-RAM delta is +16 B.
- Visually reviewed/froze the active and released D035 framebuffers: `d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3` and `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`.
- Preserved HOME cleanup, all Phase 2 behavior and every frozen D031-D034 audio fixture byte-for-byte.
- Final resources: 66,960 B image / 13,296 B normal RAM / 113,000 B pool; delta vs WP3.8 +652 / +16 / +0 B.
- Final artifacts: `fm1tracker.bin` `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`; `fm1tracker-0.1.fwsc` `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`.
- Final verification: production checkpoint `354bf7d356774ad828f1f660bd456dbab7f0490d`, run `40083510351`, job `120841687772`; evidence `docs/WP3_9_VERIFICATION.md`.

### Next

**WP3.10 - Phase 3 worst-case tests and exit gate**, issue #29, is IN PROGRESS. D036 is frozen in `docs/WP3_10_TEST_PLAN.md`; Phase 4 production work remains blocked until the complete Phase 3 stack passes the exit gate.
'''
write(p, s + '\n')

print('WP3_9_HANDOFF_SYNC_OK=1')
