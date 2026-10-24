from pathlib import Path

# Append D037 without rewriting prior decision history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D037 —' not in s
entry = r'''

## D037 — Phase 4 uses one bounded eight-voice 8-BIT engine interface

Date: 2026-10-24
Status: accepted — architecture frozen before production engine DSP

**Decision:** WP4.1 freezes the complete Phase 4 engine/voice/realtime boundary in `docs/WP4_1_ENGINE_CONTRACT.md` before PULSE, NOISE, TRIANGLE or WAVETABLE-8 production DSP is written.

The Phase 4 `8-BIT` family is `FmtInstrumentHeader.mode == 1`. Within that family engine IDs are `0 SAFE/NONE`, `1 PULSE`, `2 NOISE`, `3 TRIANGLE`, `4 WAVETABLE-8`; unsupported values fail to silence rather than aliasing another engine. The existing mode-zero proof firmware remains untouched until a later explicit migration package.

Audio owns all mutable engine runtime state. Phase 4 has exactly eight logical track adapters and a fixed maximum of eight engine voice slots, with one primary sequenced voice handle per track. Main/editor owns song/lock/UI mutation and may reach audio only through bounded aligned publication snapshots plus the already-frozen D034 audition queue. D021/D032/D033 tracker timing does not move into that queue.

The engine consumes the existing D031/D032 unsigned Q7 semitone pitch domain `0..16383`, with one semitone equal to 128 units. D032 remains the owner of persistent/transient pitch effects; D033 remains the owner of RETRIG / NOTE DELAY / NOTE CUT scheduling. Normal NOTE does not imply oscillator-phase restart, while RETRIG explicitly restarts deterministic oscillator phase/state without allocation or pitch change. TIE/EMPTY never allocate or resurrect a voice; OFF/CUT release through the common gate/envelope boundary.

Oscillator selection uses explicit bounded integer `switch` dispatch, never a vtable or runtime registration. Pitched engines use a 32-bit wrapping phase accumulator. Each oscillator produces one signed nominal-Q15 mono sample, common envelope/level gain is unsigned Q15, and at most eight scaled voices are summed in signed 32-bit accumulators with explicit saturation and no integer wrap. TRACK PAN remains storage-only until a later explicit package makes it audible.

D027 remains authoritative for parameters. Phase 4 engine-specific IDs may only come from `0x20..0x5F`; concrete IDs/page mappings remain WP4.8. Audio never traverses D028. The reserved maximum resolved engine-parameter publication shape is one aligned 32-bit word per `(track,row)`, at most `8 * 64 * 4 = 2048` bytes ordinary RAM.

The hard realtime bound is fixed: at callback ingress at most 32 D034 events; at a row boundary at most eight track dispatches; at an effect tick at most eight track updates; per rendered frame at most eight voice renders plus fixed gain/mix work; allocation may inspect at most eight slots. Heap/pool allocation, D028 traversal, screen work, persistence, blocking waits/retries, float, runtime 64-bit division, recursion and unbounded search are forbidden in audio.

The complete Phase 4 incremental ceilings versus the frozen Phase 3 baseline are `+65,536 B` app image, `+8,192 B` normal RAM and `+16,384 B` pool RAM, yielding total ceilings of `132,496 B`, `21,488 B` and `129,384 B`. Within normal RAM, the optional full engine-row cache may consume at most 2,048 B and all engine realtime voice/track/allocator/envelope state together should remain at or below 2,048 B. Crossing a ceiling requires a new recorded decision before acceptance.

**Reason:** Phase 3 deliberately proved tracker timing, p-locks, effect scheduling, runtime ingress and UI before replacing the sine proof lane. Phase 4 now needs one measured engine contract so oscillator packages cannot invent incompatible pitch domains, ownership, queues, allocation rules, parameter paths or mix formats independently. Eight fixed voices cover the eight logical sequencer tracks while keeping worst-case audio work trivial to bound and leaving substantial device headroom for Phase 5 and later core work.

**Consequence:** WP4.1 is documentation/architecture only and must close with zero production firmware/DSP delta from the clean Phase 4 branch start. WP4.2-WP4.8 may build isolated deterministic primitives behind this interface. The first production migration away from the Phase 3 proof sine must be explicit, preserve D020-D034 semantics through the adapter, rerun inherited regressions and freeze new intentional audio evidence instead of silently rewriting old hashes. WP4.10 must prove the final eight-track Phase 4 configuration remains inside the D037 ceilings or record a superseding decision.
'''
p.write_text(s.rstrip() + entry + '\n')

# Project state: the architecture is now frozen; verification is the next action.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - D037 architecture freeze next; no production engine DSP begun**'
new = 'State: **IN PROGRESS - D037 FROZEN; WP4.1 verification/closeout next; no production engine DSP begun**'
assert s.count(old) == 1
s = s.replace(old, new)
old = 'Freeze D037 for WP4.1 / #31 in the repository **before any production oscillator/engine DSP source**. Preserve every Phase 2/3 deterministic regression as the migration baseline and define explicit Phase 4 image/RAM/pool headroom before WP4.2.'
new = 'Verify and close WP4.1 / #31 with D037 frozen in `docs/WP4_1_ENGINE_CONTRACT.md`: prove zero production firmware/DSP delta from the Phase 4 branch start, preserve the Phase 2/3 migration baseline, record verification evidence, then make WP4.2 / PULSE the only active package.'
assert s.count(old) == 1
s = s.replace(old, new)
s = s.replace('Exact next action is the D037 engine-interface architecture freeze; no Phase 4 production engine DSP has begun.',
              'D037 is frozen; exact next action is WP4.1 verification/closeout with zero production DSP delta. No Phase 4 production engine DSP has begun.')
p.write_text(s)

# Work-package execution line.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '- **WP4.1 engine interface contract** — IN PROGRESS — issue #31 — freeze D037 before source'
new = '- **WP4.1 engine interface contract** — IN PROGRESS — issue #31 — D037 FROZEN; verification/closeout next; no production engine DSP'
assert s.count(old) == 1
p.write_text(s.replace(old, new))

# Roadmap status/action.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 architecture active; D037 before source**'
new = 'Status: **IN PROGRESS — WP4.1 D037 FROZEN; verification/closeout next; no production engine DSP**'
assert s.count(old) == 1
s = s.replace(old, new)
old = 'Active package: WP4.1 / #31 — freeze D037 before engine source. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.1 / #31 — D037 is frozen in `WP4_1_ENGINE_CONTRACT.md`; verify zero production delta and close WP4.1 before WP4.2. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1
p.write_text(s.replace(old, new))

print('WP4_1_D037_DOC_FREEZE_OK=1')

# Touch marker: workflow exists before this push, so this edit is the deliberate trigger.
