# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.5 complete; WP3.6 ready to start — 2026-10-24**

Parent issue: #20  
Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed Phase 3 packages

### WP3.1 — stable parameter ID registry — COMPLETE — #19

Verified checkpoint: `5af8065a9c98884d22682075b600d9790cfa7940`  
Evidence: `docs/WP3_1_VERIFICATION.md`

D027 freezes one-byte semantic parameter identity and the four current production-lockable IDs: VOICE LEVEL, FINE PITCH, TRACK LEVEL and TRACK PAN.

### WP3.2 — sparse four-lock-per-step storage — COMPLETE — #21

Verified checkpoint: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`  
Evidence: `docs/WP3_2_VERIFICATION.md`

D028 freezes one owner-checked global sparse pool of 2,048 eight-byte lock records, max four active locks per step, sorted semantic IDs and corruption-safe bounded operations.

### WP3.3 — hold-step + encoder p-lock editing — COMPLETE — #22

Verified checkpoint: `df5d7e07fc763909a141c99283cfd2be19cff847`  
Final run `40054322180` — SUCCESS.  
Evidence: `docs/WP3_3_VERIFICATION.md`

D029 freezes held-white-step target ownership, stale/begin/release encoder-delta discard and KNOB1..4 normalized `0..255` lock editing.

### WP3.4 — p-lock browse / single-lock delete UX — COMPLETE — #23

Verified checkpoint: `178ba57a19adc8e25106f1c63e0df2f16478fb33`  
Final run `40064122343` — SUCCESS.  
Evidence: `docs/WP3_4_VERIFICATION.md`

D030 freezes sorted SELECT browse plus deliberate EDIT arm -> later SEL one-lock deletion, with same-poll rejection, cancellation safety and no clear-all panel gesture.

### WP3.5 — basic pitch/volume p-lock playback — COMPLETE — #24

WP3.5 start head:

`01b35603ec387e079e4af512f8a8109e413d2714`

Final verified source/workflow checkpoint:

`17316f203dba7a93c9d03a52df6f45b12a7a2580`

Final GitHub Actions:

- run `40069110111`
- job `120798506694` (`playback-closeout`)
- result **SUCCESS**

Durable evidence: `docs/WP3_5_VERIFICATION.md`

Evidence artifact:

- ID `12550920281`
- ZIP SHA-256 `a6e92feef5639f69619afa458e18b626bd717e9c8d20b483fc414921a7f5cbcd`

## D031 playback contract

WP3.5 keeps the current proof lane at pattern 0 / track 0 and applies exactly three D027 locks to playback:

- VOICE LEVEL
- FINE PITCH
- TRACK LEVEL

TRACK PAN remains storage-only.

Locks are **row-local overrides**. Missing values restore explicit defaults on every row:

- VOICE 255
- FINE 128
- TRACK 255

VOICE and TRACK level compose against the proof target with bounded integer `/255` scaling. FINE uses signed Q7 semitone offset centered at 128 and deterministic integer linear interpolation between the existing semitone phase increments.

NOTE/TIE/EMPTY/OFF remain D025 events, while the independent D031 row snapshot may modulate an already-active voice on TIE/EMPTY rows. OFF cannot be resurrected. A zero level lock mutes the active row but is not NOTE OFF, so a later unlocked row may restore the still-logically-active note.

Audio never traverses the mutable D028 linked list. Main/editor code resolves at most four locks into one aligned 32-bit snapshot per proof row. The fixed cache is:

```text
volatile uint32_t fmt_playback_param_cache[64]
```

It is exactly 256 B ordinary RAM. Audio reads one complete 32-bit word only at D021 row dispatch. `FmtPlaybackEvent` remains exactly 4 B and D020/D021 frame ordering is unchanged.

Structural corruption publishes complete safe defaults. Accepted values above 255 fall back only that parameter to its default. Playback never mutates stored row/lock data.

## Frozen WP3.5 audio evidence

Historical no-lock WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

286,464 frames.

Separate D031 locked-audio fixture:

- control WAV `5545ea2322ff2f01b77bce0c94ed8ae379386eec1baea0f712b6daf000baf3fd`
- locked WAV `8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`
- 52,736 frames
- identical before first locked-row boundary frame 22,050
- combined VOICE/TRACK attenuation verified
- FINE pitch frequency increase verified
- EMPTY + level 0 reaches exact silence
- following unlocked EMPTY row restores the still-active note
- zero unintended MIDI events.

## Final WP3.5 resources

| Resource | Used | Limit | Free | Delta vs WP3.4 |
|---|---:|---:|---:|---:|
| app image | 64,724 B | 581,564 B | 516,840 B | +496 B |
| normal RAM | 12,736 B | 98,304 B | 85,568 B | +256 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

The +256 B normal-RAM delta is exactly the row snapshot cache.

Relevant final symbols:

```text
01c0b074 .bss 00000100 fmt_playback_param_cache
01c0b178 .bss 0000000a fmt_lock_editor_runtime
01c0b18c .bss 00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

Final artifacts:

- `fm1tracker.bin` SHA-256 `d0380ac6cd2ee0e5edb1eab3e4d3937bb62eb074d27219927b3e8b7e52a244ae`
- `fm1tracker-0.1.fwsc` SHA-256 `5e14f28985fcda4c3f54dbe8a45ff020aba65a565696972fde30d4fa366d3372`
- package identity `FM-1_987`

## Next milestone

**WP3.6 — arpeggio / slide / portamento / vibrato tracker effects**

State: **READY TO START — no WP3.6 implementation has begun**

GitHub issue: #25

Before source, freeze the next decision covering exact tracker effect IDs/encoding, row continuation/reset behavior, D020/D021 timing, integer pitch/modulation rules, NOTE/TIE/OFF/EMPTY interaction, effect memory if any, precedence with D031 FINE PITCH and hard real-time bounds.

WP3.6 must not pull WP3.7 retrig/note-delay/note-cut or WP3.8 general event queue forward without an explicit superseding decision.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: `FmtStep` remains exactly 8 B.
- D019: one pool-backed runtime `FmtSong`.
- D020/D021: exact integer transport and next-frame row-boundary ordering.
- D025: explicit NONE/NOTE/TIE/OFF semantics.
- D027: stable one-byte semantic parameter identity.
- D028: 2,048-record owner-checked global sparse lock pool, max four per step.
- D029/D030: deterministic create/replace/browse/delete editor behavior.
- D031: three row-local playback parameters through a 256-byte 32-bit snapshot cache; TRACK PAN storage-only.
- Preserve the historical default WAV unless a later deliberate audio decision records a superseding regression.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. Current Phase 3 work is the tracker/p-lock/event foundation for that milestone. Do not silently force future GLOBAL/master automation into normal track-owned D028 storage.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build evidence does not claim physical control comfort, install/boot/audio behavior, physical sound quality or measured AC791N execution margin.

## Exact next action

Start WP3.6 / issue #25 as its own package. Verify actual branch HEAD, mark WP3.6 IN PROGRESS, freeze its effect/timing contract before source, and do not begin WP3.7.

## Last relevant checkpoints

- Phase 2 verified: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 verified: `178ba57a19adc8e25106f1c63e0df2f16478fb33`
- WP3.5 verified: `17316f203dba7a93c9d03a52df6f45b12a7a2580`

## Last updated

2026-10-24 — WP3.5 verified complete. WP3.6 / #25 is READY TO START; no WP3.6 source exists.
