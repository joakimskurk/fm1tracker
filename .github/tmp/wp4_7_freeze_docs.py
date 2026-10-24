from pathlib import Path

# DECISIONS: append D043 without rewriting history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D043 —' not in s, 'D043 already present'
entry = r'''

## D043 — WP4.7 uses an eight-slot deterministic allocator that steals only detached RELEASE tails

Date: 2026-10-24
Status: accepted — frozen before allocator production source

**Decision:** WP4.7 implements one isolated fixed eight-slot allocator exactly as frozen in `docs/WP4_7_ALLOCATOR_CONTRACT.md`. `FmtVoiceAllocator` is exactly 52 B: eight 32-bit recency ages, one saturating 32-bit clock, eight voice->track owner bytes and eight track->voice bytes. `FmtVoiceAllocation` is exactly 4 B and reports NONE/REUSE/FRESH_IDLE/STEAL_TAIL plus whether engine state must be reset.

D043 preserves D037's one-primary-voice-per-track invariant and explicitly forbids stealing a voice still owned by another logical track. OFF/NOTE CUT first start D042 RELEASE and then detach the track from that slot; the release may continue as an unowned tail. NOTE acquisition reuses an already-owned non-IDLE slot, otherwise takes the lowest-index IDLE slot, otherwise steals the oldest unowned RELEASE tail with lower slot index as the age tie-break. If none of those candidates exists, allocation fails to `NO_VOICE` without mutation.

This release-tail rule is sufficient for the eight-track/eight-slot Phase 4 architecture: a valid requesting track with no owned slot cannot require another track's still-owned primary voice to be cut. It also allows a track to start a new primary voice while its older detached release tail continues when spare capacity exists.

The allocator receives only the caller-owned eight-byte D042 stage snapshot. IDLE synchronization clears stale owner mappings. Unowned ATTACK/DECAY/SUSTAIN is malformed and fails safely. TIE/EMPTY/RETRIG never allocate; delayed NOTE allocates only when it fires. Fresh/steal results require deterministic oscillator/engine reset by the later adapter; REUSE does not reset oscillator state, preserving D037 ordinary-NOTE phase semantics.

Recency uses only 32-bit integer state. The clock saturates at `UINT32_MAX` and never wraps; once saturated, the lower voice index remains the deterministic tie-break. Allocator work is event-boundary fixed eight-slot scanning only, with no heap, blocking, tracker-store traversal, recursion, float, runtime 64-bit arithmetic or unbounded search. Persistent isolated state cost is 52 B normal RAM and no pool RAM.

WP4.7 remains disconnected from `dinghy.c`, so full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.6.

**Reason:** D037/D042 already provide eight track identities, eight voice slots and click-safe release behavior. Detaching release tails gives the allocator a musically useful bounded steal target without ever cutting a still-owned track. Saturating recency avoids wrap arithmetic while preserving deterministic oldest-tail selection for practical lifetime and a frozen tie-break after saturation.

**Consequence:** focused tests must prove exact 52 B/4 B layouts, reset/mapping invariants, all-eight deterministic first allocation, same-track reuse, IDLE synchronization, OFF/CUT detach, free-before-tail priority, oldest-tail/tie selection, no active cross-track steal, saturation without wrap, malformed-state no-mutation, guards/replay and a frozen long sequence hash. `fmt_voice_allocator.c` must compile on pinned PI32v2 and the complete full app must remain byte-identical while isolated. Parameter mapping and full engine integration remain WP4.8/WP4.9.
'''
p.write_text(s.rstrip() + entry.rstrip() + '\n')

# PROJECT_STATE.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7/allocator active; D043 contract freeze next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7/allocator active; D043 FROZEN; isolated implementation/tests next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 active; D043 ownership/allocation/steal/tie-break contract required before allocator source**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 active; D043 FROZEN; isolated allocator implementation/tests next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.7 must freeze D043 before production allocator source. D043 must define exact eight-slot ownership state, one-primary-voice-per-track reuse, free/IDLE/release selection, deterministic steal priority and tie-breaks, age/wrap semantics, reset/preserve behavior on allocation or steal, D037/D042 NOTE/TIE/OFF/RETRIG interaction, NO_VOICE behavior, bounded work and focused deterministic evidence.'
new = 'D043 is frozen in `docs/WP4_7_ALLOCATOR_CONTRACT.md`: exact 52-byte eight-slot ownership/recency state, same-track reuse, lowest-IDLE fresh allocation, OFF/CUT detached release tails, oldest unowned RELEASE-tail steal only, no active cross-track steal, saturating 32-bit recency with lower-index tie-break, explicit NO_VOICE fail-safe and bounded fixed-slot work.'
assert s.count(old) == 1, 'PROJECT_STATE D043 paragraph mismatch'
s = s.replace(old, new, 1)
anchor = '- D042: WP4.6 common envelope is a 20-byte linear ADSR with Q15.16 level, division-free 16-entry rate mapping, hard NOTE/RETRIG articulation, TIE/EMPTY no-op and OFF/CUT release from current level while isolated.\n'
d043 = '- D043: WP4.7 allocator is a 52-byte fixed eight-slot owner/recency map that reuses same-track voices, prefers IDLE, detaches OFF/CUT release tails and may steal only the oldest unowned RELEASE tail; active cross-track stealing is forbidden.\n'
assert anchor in s, 'PROJECT_STATE decision anchor missing'
if d043 not in s:
    s = s.replace(anchor, anchor + d043, 1)
old = 'Freeze D043 and the focused WP4.7 deterministic allocator/steal test contract in the repository before creating allocator production source. Keep all four oscillator primitives and D042 envelope isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
new = 'Implement and verify isolated `fmt_voice_allocator.*` exactly under D043 with ownership/free/release-tail/recency/saturation/corruption/replay host tests, static realtime audit and pinned PI32v2 target compilation. Keep D038-D042 primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.8 or full engine integration.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)
checkpoint_anchor = '- WP4.6 D042 verification/closeout: `14a9f06ab3d808cf9579d53e881ef75a2fa30486` — run `40102339789`, job `120896886645`\n'
checkpoint = '- WP4.7 D043 contract: `1215c89aa48cd2727c10c6af327fdc0052d72562`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)
last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.7 / #37 active. D043 is frozen in `WP4_7_ALLOCATOR_CONTRACT.md` before allocator source. Exact next action: implement/test isolated `fmt_voice_allocator.*` with mapping/free/detach/release-tail/recency/saturation/fail-safe proof and real PI32v2 compile while the full app remains byte-identical.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.6 / #36 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 before source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.6 / #36 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.7 deterministic voice allocation/stealing** — IN PROGRESS — issue #37 — freeze D043 ownership/free/release/steal/tie-break/age-wrap/test contract before source'
new = '- **WP4.7 deterministic voice allocation/stealing** — IN PROGRESS — issue #37 — D043 FROZEN; isolated allocator implementation + ownership/release-tail/saturation/PI32v2 verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.6 / #36 are COMPLETE**; **WP4.7 / #37 deterministic voice allocation/stealing is active**, with D043 required before source.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.6 / #36 are COMPLETE**; **WP4.7 / #37 deterministic voice allocation/stealing is active and D043 is FROZEN**, with isolated deterministic implementation/testing next.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.7 / #37 — freeze D043 deterministic allocator ownership/free/release/steal/tie-break/age-wrap/test contract before production source. WP4.1 / #31 through WP4.6 / #36 are complete; all four oscillator primitives plus D042 envelope remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.7 / #37 — D043 is frozen in `WP4_7_ALLOCATOR_CONTRACT.md`; implement/test isolated fixed eight-slot allocator with release-tail stealing only and no active cross-track steal. WP4.1 / #31 through WP4.6 / #36 are complete; D038-D042 primitives remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D043 deterministic allocator/steal contract frozen before source | required | compile | n/a | ⬜ WP4.7 active |'
new = '| D043 deterministic allocator/steal contract frozen before source | required | compile | n/a | ✅ `WP4_7_ALLOCATOR_CONTRACT.md` / `1215c89a...` |'
assert s.count(old) == 1, 'TEST_MATRIX D043 row mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_7_D043_DOC_FREEZE_OK=1 allocator_bytes=52 result_bytes=4 slots=8 active_cross_track_steal=0 release_tail_steal=1')
