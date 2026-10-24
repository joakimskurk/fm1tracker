# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, then `WORK_PACKAGES.md`, `DECISIONS.md`, `EXECUTION_PROTOCOL.md`, `TEST_MATRIX.md`, `ROADMAP.md`, and the verification document for the most recently completed package before changing code.

## Current phase

**Phase 3 — p-locks and tracker effects**

State: **IN PROGRESS — WP3.5 active — 2026-10-24**

Parent issue: #20

Current branch: `phase3/plocks-effects`

Phase 3 branch start / completed Phase 2 handoff:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

## Completed WP3.1

**WP3.1 — stable parameter ID registry** — COMPLETE — issue #19  
Verified checkpoint: `5af8065a9c98884d22682075b600d9790cfa7940`  
Evidence: `docs/WP3_1_VERIFICATION.md`

D027 freezes one-byte semantic parameter identity and exactly four current production-lockable IDs: VOICE LEVEL, FINE PITCH, TRACK LEVEL and TRACK PAN.

## Completed WP3.2

**WP3.2 — sparse four-lock-per-step storage** — COMPLETE — issue #21  
Verified checkpoint: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`  
Evidence: `docs/WP3_2_VERIFICATION.md`

D028 freezes one owner-checked global sparse pool of 2,048 eight-byte lock records, max four active locks per individual step, deterministic sorted lists and explicit STEP_FULL / POOL_FULL / CORRUPT behavior.

## Completed WP3.3

**WP3.3 — hold-step + encoder p-lock editing** — COMPLETE — issue #22  
Verified checkpoint: `df5d7e07fc763909a141c99283cfd2be19cff847`  
Final run `40054322180`, job `120753812201` — **SUCCESS**.  
Evidence: `docs/WP3_3_VERIFICATION.md`

D029 freezes ordinary-SEQ held-white-step target ownership, stale/begin/release KNOB1..4 delta discard, the four current D027 mappings and normalized `0..255` storage-only lock editing.

## Completed WP3.4

**WP3.4 — p-lock browse / single-lock delete UX** — COMPLETE — issue #23

Verified source/workflow checkpoint:

`178ba57a19adc8e25106f1c63e0df2f16478fb33`

Final run `40064122343`, job `120783658593` — **SUCCESS**.  
Evidence: `docs/WP3_4_VERIFICATION.md`

D030 freezes SELECT browse in sorted D028 order plus deliberate EDIT arm -> later SEL one-lock deletion, with same-poll rejection, cancellation safety, exactly-one-record recovery and no clear-all panel gesture.

Final WP3.4 baseline:

| Resource | Used | Limit | Free |
|---|---:|---:|---:|
| app image | 64,228 B | 581,564 B | 517,336 B |
| normal RAM | 12,480 B | 98,304 B | 85,824 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B |

`FmtLockEditor` is 10 B. Package identity remains `FM-1_987`.

Frozen pre-p-lock-playback WAV:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

## Current milestone

**WP3.5 — basic pitch/volume p-lock playback**

State: **IN PROGRESS — 2026-10-24**

GitHub issue: #24

WP3.5 start head:

`01b35603ec387e079e4af512f8a8109e413d2714`

No WP3.5 production implementation existed at this checkpoint.

### Exact objective

Apply the first deliberately small subset of D027/D028 p-locks to deterministic tracker playback/audio without mutating stored base data, without letting the audio path traverse mutable editor-owned lock structures unsafely, and without pulling WP3.6 modulation effects or WP3.8 general queue architecture forward.

D031 must be recorded in `DECISIONS.md` before any WP3.5 playback/audio source change and must freeze:

- which current D027 IDs are consumed by playback in WP3.5;
- exact normalized `0..255` interpretation for each accepted parameter;
- level composition/order if VOICE LEVEL and TRACK LEVEL are both accepted;
- FINE PITCH center/range and integer/fixed-point mapping;
- whether TRACK PAN is included now or remains storage-only;
- D020/D021 row-boundary timing for lock application;
- unlocked-row/base-value behavior: row-local restore versus persistence;
- NOTE / TIE / OFF / EMPTY interaction;
- structurally valid but playback-domain-invalid lock handling;
- bounded sequencer/audio ownership and handoff;
- maximum row-boundary work and event/payload size;
- deterministic host-audio evidence and the exact rule for preserving or superseding the frozen pre-lock WAV.

### Existing playback/audio facts verified at package start

The current D021/D025 proof path is still deliberately one lane only: pattern 0 / logical track 0.

`FmtPlayback` is 1 B and `FmtPlaybackEvent` is exactly 4 B:

```text
boundary, step, kind, note
```

`fmt_playback_start()` dispatches the current row before frame 0, then starts transport. `fmt_playback_end_frame()` is called after each rendered frame; when D020 crosses a row boundary it decodes the new row for the **next rendered frame**.

The current event decoder reads only `FmtStep.note`. It classifies ordinary NOTE / TIE / OFF / NONE and does not inspect D028 locks.

The current audio callback owns transport writes and applies each row event directly to one dedicated proof voice. There is no general sequencer event queue yet.

D028 lock storage is a mutable sparse singly-linked pool shared with the main-loop editor. Direct audio-ISR traversal of that mutable linked list is **not assumed safe** merely because the current proof lane is small. D031 must choose an explicit bounded ownership strategy before source.

### Reviewed architectural direction before D031

The preferred narrow direction to evaluate is:

- keep WP3.5 on the existing pattern-0 / track-0 proof lane;
- consume only parameters with semantics that can be defined against the current proof sine without inventing Phase 4/5 engine contracts;
- preserve TRACK PAN as storage-only unless a stereo proof adds real value without widening scope;
- interpret locks as **row-local overrides** that are recomputed at every row boundary from explicit base defaults plus that row's locks, rather than allowing a lock to leak indefinitely through later unlocked rows;
- resolve/validate the maximum four row locks outside the per-sample render loop and hand audio a fixed bounded row snapshot/event, never a D028 linked-list pointer;
- keep per-frame DSP work constant and independent of lock count;
- retain D021's exact boundary ordering: the new row snapshot becomes active on the next rendered frame;
- treat malformed/corrupt/out-of-domain lock data as a deterministic safe base/default for that parameter, never as an unbounded retry or raw DSP value.

This direction is not yet D031 and must not be coded until the exact value maps, payload and ownership are frozen.

### Required verification

1. D031 exists before source changes;
2. accepted playback-lock subset is explicit and minimal;
3. no audio render loop traverses D028 linked lists;
4. row-resolution work is hard-bounded by max four locks;
5. unlocked rows have deterministic base/default parameter values;
6. lock effect cannot leak into later unlocked rows unless D031 explicitly says so;
7. NOTE/TIE/OFF/EMPTY interaction is exact and boundary-tested;
8. out-of-domain/corrupt lock data fails safely;
9. stored row/lock data remains byte-for-byte unchanged by playback;
10. per-frame audio work remains fixed/bounded;
11. deterministic host audio proves each accepted parameter independently and in combination;
12. historical no-lock/default pattern behavior remains byte-identical if D031 says the no-lock path is unchanged;
13. all Phase 2 + WP3.1–WP3.4 regressions remain green;
14. full JieLi device build/resource delta is recorded;
15. no WP3.6+ effect semantics are pulled forward.

### Strict non-goals

- no arpeggio/slide/porta/vibrato — WP3.6;
- no retrig/note-delay/note-cut — WP3.7;
- no broad/general event queue — WP3.8;
- no full row lock/effect visualization — WP3.9;
- no new synth-engine implementation;
- no new parameter IDs unless D031 proves an unavoidable missing semantic identity;
- no live-performance capture/automation;
- no persistence/file-format work;
- no final hardware control-map freeze.

## Important decisions / do not regress

- D005: 8 logical tracks × 64 steps × 16 patterns.
- D010: p-locks are first-class bounded sequencer data.
- D018: `FmtStep` remains exactly 8 B.
- D019: one pool-backed runtime `FmtSong`.
- D020: exact integer frame-domain transport.
- D021: step 0 before frame 0; crossed row applies to next frame.
- D025: explicit NONE/NOTE/TIE/OFF semantics.
- D027: stable one-byte semantic parameter identity.
- D028: 2,048-record owner-checked global sparse lock pool, max four per step.
- D029/D030: deterministic storage editor/browse/delete behavior.
- Preserve the frozen pre-lock WAV unless D031 deliberately supersedes the affected audio regression with explicit replacement evidence.
- No float in firmware DSP and no runtime 64-bit division.

## Product direction that must not be lost

`docs/LIVE_PERFORMANCE_CAPTURE_VISION.md` remains accepted future core direction. WP3.5 is only the first deterministic playback application of ordinary track/step p-lock data. It must not define future GLOBAL/master automation ownership.

## Hardware limitation still open

No physical FM-1 is currently available. Host/device-build verification cannot claim physical audio quality, AC791N execution margin, install/boot behavior or final p-lock musical ergonomics.

## Exact next action

Inspect the proof voice/audio and D028 read APIs deeply enough to freeze D031. Record D031 before changing `fmt_playback.*`, `dinghy.c` audio behavior or any row-event representation. Do not begin WP3.6.

## Last relevant checkpoints

- Phase 2 verified implementation/workflow: `7063be51e53c2657178084b3bef5ace813dafcd7`
- Phase 3 branch start: `c3693d8c7bb3d5306794aa232d6d25bef50c6c99`
- WP3.1 verified: `5af8065a9c98884d22682075b600d9790cfa7940`
- WP3.2 verified: `da2f5afe9b1da6defc3857b91f52b40d7525f33b`
- WP3.3 verified: `df5d7e07fc763909a141c99283cfd2be19cff847`
- WP3.4 verified: `178ba57a19adc8e25106f1c63e0df2f16478fb33`
- WP3.5 start head: `01b35603ec387e079e4af512f8a8109e413d2714`

## Last updated

2026-10-24 — WP3.5 / issue #24 marked IN PROGRESS at `01b35603...`. D031 must be frozen before any playback/audio source change.
