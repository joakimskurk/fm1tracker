from pathlib import Path

# Append D042 without rewriting prior decision history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D042 —' not in s, 'D042 already present'
entry = r'''

## D042 — WP4.6 common envelope is a 20-byte linear ADSR with hard NOTE/RETRIG articulation and division-free render rates

Date: 2026-10-24
Status: accepted — frozen before envelope production source

**Decision:** WP4.6 implements one isolated common linear ADSR amplitude envelope exactly as frozen in `docs/WP4_6_ENVELOPE_CONTRACT.md`. It remains disconnected from `dinghy.c` and all four Phase 4 oscillator primitives during WP4.6.

`FmtEnvelopeParams` is exactly 4 B (attack/decay/sustain/release bytes). `FmtEnvelope` is exactly 20 B: Q15.16 level, current rate, pre-resolved decay rate, sustain target and one explicit stage byte plus reserved alignment. Stage IDs are IDLE=0, ATTACK=1, DECAY=2, SUSTAIN=3 and RELEASE=4. Eight future envelope states cost exactly 160 B mutable RAM.

Level is `0..0x7FFF0000`; render returns unsigned Q15 `0..32767`. Sustain maps exactly as `(control * 257) >> 1`. A/D/R control 0 is instantaneous. Nonzero A/D/R use a frozen 16-entry `uint32_t` base-rate table indexed by the low nibble and shifted by the high nibble, so time resolution requires no runtime division and the render loop contains only fixed add/subtract/compare/clamp work. The 64 B rate table spans roughly 1.07 ms to 63.24 s full-scale slopes at 44.1 kHz.

D042 resolves event articulation explicitly. Ordinary NOTE hard-restarts the envelope from level zero but does not restart oscillator phase; TIE and EMPTY make no envelope call; D033 RETRIG performs the same hard envelope restart while D037 separately restarts oscillator phase/state; OFF and NOTE CUT start RELEASE from the exact current level. Zero-time stages collapse immediately without dummy rendered frames. `fmt_envelope_render()` advances the current linear stage before returning its Q15 level, matching the existing proof-lane ramp direction.

Frozen neutral/default params are A24/D0/S255/R72, approximately 3.02 ms attack and 23.97 ms full-scale release. The frozen long sequence A64/D96/S160, 4096 renders, R112 gate-off, 4096 renders transitions ATTACK->DECAY on render 705, DECAY->SUSTAIN on total render 1755, RELEASE->IDLE on release render 3534 and yields FNV-1a32 `0x40A44A2E` with final level zero/IDLE.

The primitive is intentionally isolated in WP4.6. Full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.5. The only isolated immutable data is the 64 B rate table; there is no pool storage or allocation.

**Reason:** Phase 4 now has four deterministic oscillator primitives but still needs one common articulation boundary before allocator and engine integration. Linear ADSR gives familiar tracker articulation and explicit TIE legato while keeping state small. A compact rate table plus nibble shifting removes division from the audio render path, makes worst-case work trivial to bound and spans useful millisecond-to-minute slopes without floating point.

**Consequence:** Focused tests must prove exact state/layout, all 256 rate controls, sustain mapping, NOTE/TIE/RETRIG/OFF semantics, zero-time edges, monotonic ramps, exact terminal levels, default timing and the frozen long-run hash. `fmt_envelope.c` must compile with the real pinned PI32v2 compiler and contain no render loop/division/float/int64/allocation/store traversal. Final allocator, parameter mapping and full engine integration remain WP4.7-WP4.9. Any change to hard articulation, stage topology, rate table, update ordering or live retiming requires a superseding decision and new deterministic evidence.
'''
p.write_text(s.rstrip() + entry.rstrip() + '\n')

# PROJECT_STATE.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/D039 COMPLETE; WP4.4/D040 COMPLETE; WP4.5/D041 COMPLETE; WP4.6/envelope active; D042 contract freeze next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6/envelope active; D042 FROZEN; isolated implementation/tests next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 active; D042 state/time/gate/retrigger/release contract required before envelope source**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 active; D042 FROZEN; isolated common-envelope implementation/tests next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.6 must freeze D042 before production envelope source. D042 must define exact common envelope topology, fixed state and Q15 level domain, parameter/time mapping, zero-time behavior, NOTE/gate-on vs D033 RETRIG semantics, OFF/release and TIE semantics, sample/update ordering, stage-transition bound, deterministic vectors/hash, realtime/resource bounds, real PI32v2 obligations and the zero-full-app-delta isolation boundary.'
new = 'D042 is frozen in `docs/WP4_6_ENVELOPE_CONTRACT.md`: 20-byte linear ADSR state, 4-byte parameter snapshot, division-free 16-entry rate mapping, hard NOTE/RETRIG envelope articulation, TIE/EMPTY no-op, OFF/CUT release from current level, advance-before-return ordering, bounded single-transition render work and zero full-app delta while isolated.'
assert s.count(old) == 1, 'PROJECT_STATE D042 paragraph mismatch'
s = s.replace(old, new, 1)
anchor = '- D041: WP4.5 WAVETABLE-8 is a 4-byte phase oscillator over eight immutable 32-sample signed-8-bit tables (256 B total), top-five-bit zero-order-hold lookup, `raw*258` Q15 expansion and direct D038 pitch reuse while isolated.\n'
d042 = '- D042: WP4.6 common envelope is a 20-byte linear ADSR with Q15.16 level, division-free 16-entry rate mapping, hard NOTE/RETRIG articulation, TIE/EMPTY no-op and OFF/CUT release from current level while isolated.\n'
assert anchor in s, 'PROJECT_STATE decision anchor missing'
if d042 not in s:
    s = s.replace(anchor, anchor + d042, 1)
old = 'Freeze D042 and the focused WP4.6 common-envelope test contract in the repository before creating envelope production source. Keep all four oscillator primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
new = 'Implement and verify isolated `fmt_envelope.*` exactly under D042 with full rate/sustain/event/stage/zero-time/default-timing/hash host tests, static realtime audit and pinned PI32v2 target compilation. Keep all four oscillator primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.7 or full engine integration.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)
checkpoint_anchor = '- WP4.5 D041 verification/closeout: `bb56d16c31330e3515ba36241726c09a2a30729b` — run `40095198147`, job `120875856198`\n'
checkpoint = '- WP4.6 D042 contract: `983757fc3fc48d884ade9b9fdb59df0a1d070f9c`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)
last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.6 / #36 active. D042 is frozen in `WP4_6_ENVELOPE_CONTRACT.md` before envelope source. Exact next action: implement/test isolated `fmt_envelope.*` with all-rate/sustain/event/stage/zero-time/default-timing/hash proof and real PI32v2 compile while the full app remains byte-identical.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.5 / #35 COMPLETE; WP4.6 / #36 common envelope active; D042 before source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.5 / #35 COMPLETE; WP4.6 / #36 common envelope active; D042 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.6 common envelope primitive** — IN PROGRESS — issue #36 — freeze D042 state/time/gate/retrigger/release/test contract before source'
new = '- **WP4.6 common envelope primitive** — IN PROGRESS — issue #36 — D042 FROZEN; isolated ADSR implementation + rate/event/stage/PI32v2 verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.5 / #35 are COMPLETE**; **WP4.6 / #36 common envelope is active**, with D042 required before source.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.5 / #35 are COMPLETE**; **WP4.6 / #36 common envelope is active and D042 is FROZEN**, with isolated deterministic implementation/testing next.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 common envelope active; D042 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 common envelope active; D042 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.6 / #36 — freeze D042 common-envelope state/time/gate/retrigger/release/test contract before production source. WP4.1 / #31 through WP4.5 / #35 are complete; all four oscillator primitives remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.6 / #36 — D042 is frozen in `WP4_6_ENVELOPE_CONTRACT.md`; implement/test isolated common linear ADSR without full-app integration. WP4.1 / #31 through WP4.5 / #35 are complete; all four oscillator primitives remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: mark only D042 contract freeze complete.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D042 common envelope primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.6 active |'
new = '| D042 common envelope primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_6_ENVELOPE_CONTRACT.md` / `983757fc...` |'
assert s.count(old) == 1, 'TEST_MATRIX D042 row mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_6_D042_DOC_FREEZE_OK=1 params_bytes=4 state_bytes=20 topology=ADSR rate_table_bytes=64 render_division=0')
