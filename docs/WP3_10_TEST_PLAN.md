# WP3.10 Phase 3 Worst-Case Test / Exit Plan — D036

Date: 2026-10-24
Status: **ACCEPTED BEFORE WP3.10 STRESS IMPLEMENTATION**

Parent issue: #29

This document freezes the final Phase 3 verification package. WP3.10 is not a feature package: it must prove that D027-D035 remain bounded, deterministic and mutually compatible at their declared worst cases. Production source should remain unchanged unless a stress test exposes a real defect.

## D036 — Phase 3 exits only after the complete p-lock/effect/event/queue/visual stack passes bounded worst-case stress

### 1. Baseline and scope freeze

The production baseline is the final WP3.9 verified checkpoint:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

WP3.10 may add tests, host fixtures, verification tooling and documentation. If no defect is found, the final production-source tree must be byte-identical to that baseline.

If a real defect is found:

- make the smallest bounded fix required;
- add a focused regression that fails on the WP3.9 baseline and passes after the fix;
- record the production delta and new resource cost explicitly;
- rerun every WP3.10 exit gate.

WP3.10 must not add new p-lock parameters, tracker commands, queue semantics, synth engines, capture semantics, persistence or UI features.

### 2. D028 worst-case lock-store stress

The D028 global lock pool is fixed at 2,048 records, maximum four locks per step.

Required stress:

1. Allocate all 2,048 records using valid unique owners/parameter IDs without exceeding four locks on any step.
2. Verify `used_count == 2048` and zero free records.
3. Verify the next valid allocation fails deterministically as pool-full without partial mutation.
4. Replace every already-present semantic lock in place and prove pool usage remains 2,048.
5. Remove records from first/middle/last positions of owner lists under full-pool conditions and verify exactly one free record per successful removal.
6. Reallocate every released record and prove exact recovery to full capacity.
7. Exercise owner indices across pattern, track and step boundaries, including first and last legal owner.
8. Inject bounded malformed/corrupt chain cases and prove no neighboring owner/list mutation.
9. Verify sorted semantic ID order and max-four enforcement throughout.

The test must hash or otherwise deterministically summarize the final complete lock-store state so repeat runs can detect hidden nondeterminism.

### 3. D031/D032 cache and audio-boundary stress

Audio must continue to consume fixed snapshots rather than mutable D028 lists.

Required stress:

- populate the proof playback lane with the maximum four locks on every one of its 64 rows;
- rebuild/refresh the D031 256 B playback-parameter cache and verify every row snapshot against the lock store;
- populate every row with a valid D032/D033 command/value combination, including boundary and no-op values;
- rebuild the D032 256 B effect cache and verify every row word;
- mutate lock/effect storage only from main/test context and prove audio-facing snapshots do not partially observe list mutation;
- prove cache rebuild/refresh never mutates song or lock storage;
- prove audio source still contains no D028 traversal/allocator path.

### 4. D032/D033 timing boundary matrix

The exact six-tick D020-derived grid remains authoritative.

Stress must cover, at minimum:

- every D032 command at minimum, representative and maximum byte values;
- RETRIG, NOTE DELAY and NOTE CUT values `0..5`, `6`, and `255`;
- row 63 -> row 0 wrap;
- command changes on consecutive rows;
- NOTE/TIE/EMPTY/OFF adjacency around delay/cut/retrig rows;
- D031 FINE plus D032 pitch effects at boundary values;
- slide/porta pitch saturation at low/high Q7 bounds;
- porta target replacement/resume behavior;
- vibrato phase continuation across rows;
- row-boundary precedence over any hypothetical tick 6;
- no event/action leakage into the next row.

At 120 BPM, the existing exact frame anchors `919/1838/2757/3675/4594` and next row `5513` must remain unchanged.

### 5. D034 queue saturation, wrap and recovery storm

The runtime queue remains exactly 32 entries / 268 B, SPSC main -> audio, reject-newest.

Required stress:

1. Repeatedly fill exactly 32 events and prove FIFO order.
2. Attempt one or many additional pushes while full and prove unread slots / `write_seq` remain unchanged while `overflow_seq` advances exactly once per rejected push.
3. Drain/reuse the queue through many ring wraps.
4. Start sequence counters around `UINT32_MAX` and cross wrap repeatedly.
5. Alternate NOTE_ON/OFF bursts for all 27 hardware keys plus MIDI events.
6. Force repeated overflow storms and verify every detected overflow causes fail-to-silence manual-gate release plus O(1) discard.
7. Verify normal audition recovers after each overflow epoch.
8. Verify no stuck manual gate after any stress sequence.
9. Verify callback consumption remains bounded to at most 32 events.
10. Verify D024 transport mailbox and D033 fixed-state scheduler remain outside the queue.

### 6. Combined runtime stress

A full-app host scenario must overlap the independent Phase 3 subsystems rather than testing each only in isolation.

The scenario must simultaneously include:

- running tracker transport on the proof lane;
- consecutive D032/D033 effects;
- D031 row locks;
- hardware/USB-MIDI audition through D034;
- p-lock hold/edit/browse/delete operations from main context;
- track/page cursor changes where allowed;
- D035 screen synchronization/redraw backlog;
- at least one forced D034 overflow and recovery.

The scenario must prove:

- no song/lock corruption;
- no stuck audition note;
- no stale p-lock held target after release/context change;
- no effect/timing corruption;
- no unexpected MIDI emission from p-lock editing;
- deterministic final store/editor/transport/queue state;
- deterministic output WAV for the fixed host fixture.

### 7. D035 worst-case presentation stress

D035 remains ten regions, max two rendered per main-loop poll, 62 B state.

Required stress:

- all eight visible rows each contain four locks, exercising the maximum 32 lock-record inspections per sync;
- all eight rows contain non-NONE effects with changing effect values;
- cursor and visible playhead move while rows change;
- held-step target begins/ends while old/new row regions are already dirty;
- repeated footer browse/value/delete updates occur faster than the two-region drain budget;
- track/page/half-page transitions mark all ten regions and drain deterministically without dropping pending bits;
- HOME exit and return clear stale tracker/editor pixels;
- no draw call from audio/ISR;
- screen synchronization never mutates song/lock storage.

The test must prove eventual convergence: after input becomes stable, the dirty mask must drain to zero under the two-regions-per-poll rule and the final framebuffer must equal a deterministic reference.

### 8. Repeated editor/context transition stress

Exercise long deterministic sequences of:

- track selection;
- page changes;
- cursor/white-step selection;
- held-step p-lock begin/release;
- K1..K4 edits at clamp boundaries;
- SELECT browse at first/last bounds;
- delete arm/cancel/confirm;
- PLAY while held/armed;
- STEP REC enter/exit;
- HOME exit/return;
- note-key conflicts that D029/D030 suppress/cancel.

Required invariants:

- held target never silently retargets;
- no same-poll destructive delete;
- no latent delete arm after cancellation/context change;
- free-list count matches successful allocations/removals exactly;
- p-lock gestures emit no audition MIDI;
- stored neighboring rows/tracks remain unchanged except intended targets.

### 9. Long deterministic clock/run test

Retain D020/D021 exact integer transport as the only tracker clock.

Required:

- at least the existing six-hour transport proof remains green;
- a combined effects/events run crosses many pattern wraps without accumulated row/tick drift;
- expected row/effect tick ordering remains exact;
- no new float/double DSP dependency;
- no runtime 64-bit division in firmware DSP/real-time path.

### 10. Static real-time boundedness audit

WP3.10 must inspect the final production sources and fail if the real-time path gains:

- malloc/free or equivalent allocator use;
- lock-store linked-list traversal from audio;
- unbounded queue drain;
- screen/display calls from audio;
- spin/retry loops around SPSC publication;
- runtime float DSP;
- runtime 64-bit divide;
- hidden second transport/effect clock.

The D034 callback queue drain cap remains 32. D032/D033 sub-row work remains fixed-state and at most one scheduled D033 action per nonzero effect tick.

### 11. Resource and target-build exit gate

Run the pinned JieLi/PI32v2 + AC79 SDK device build used by WP3.8/WP3.9.

Required:

- package identity `FM-1_987`;
- register-access hygiene remains `hal/ only`;
- all fixed-state symbols remain in expected RAM/pool sections;
- final image, normal RAM and pool usage are recorded;
- if production source is unchanged, resources and firmware hashes must be identical to WP3.9;
- if a defect fix changes production, exact delta and rationale must be documented;
- pool usage must remain within the fixed 344,064 B region and the D028 + song-store placement must remain explicit.

### 12. Inherited deterministic regressions

The final Phase 3 closeout must rerun all authoritative inherited regressions, including:

- Phase 2 model/store/transport/playback/editor/screen behavior;
- D027 registry;
- D028 lock store;
- D029/D030 lock editor;
- D031 playback params and locked WAV;
- D032 effect timing/audio fixtures;
- D033 event timing/audio fixtures;
- D034 queue/audition/overflow fixtures;
- D035 visual/framebuffer references;
- historical default WAV.

Any intentional replacement of a visual fixture must be explicitly justified; audio hashes must not be silently rebased.

### 13. Phase 3 exit criteria

Phase 3 may be marked **COMPLETE** only if:

1. all D027-D035 semantics remain deterministic under their declared worst cases;
2. no stress test finds corruption, stale destructive state, stuck note or unbounded real-time work;
3. combined full-app stress passes deterministically;
4. D035 converges under its fixed redraw budget;
5. every inherited audio/runtime regression is green;
6. pinned target build/resource audit is green;
7. any production defect fix is explicitly documented and fully reverified;
8. physical-hardware limitations are stated rather than inferred away.

Only after these gates pass may the repository hand off to Phase 4 / WP4.1 engine interface work.

### 14. Hardware limitation

No physical FM-1 is currently available. WP3.10 can close Phase 3 at host/device-build level, but cannot claim:

- physical key/encoder ergonomics;
- real LCD readability/refresh perception;
- physical install/boot/audio behavior;
- measured AC791N execution-time margin under simultaneous worst-case audio/UI/USB activity.

Those remain explicit later hardware gates.
