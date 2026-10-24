# WP4.10 Representative 8-Track Benchmark / Phase 4 Exit Contract

Date: 2026-10-24  
Package: WP4.10 / issue #41  
Decision: D046  
Branch: `phase4/8bit-engine`

This document is normative for the final Phase 4 package.

WP4.10 is a **verification / benchmark / closeout package**, not a feature package. No new production source may be written before this contract is recorded in `DECISIONS.md` and the project state is updated to D046 FROZEN.

## 1. Baseline and production-freeze rule

The linked D045 production baseline is the final Gate-4 production/hygiene checkpoint:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

WP4.9 Gate 5/6 and closeout work after that checkpoint is verification-only. Its final canonical closeout is:

- closeout head `89fabc51348078101c8e376bba7814af9d42ae80`
- run `40116897104`
- job `120939922897`
- evidence `docs/WP4_9_VERIFICATION.md`

WP4.10 may add tests, host fixtures, benchmark instrumentation, CI, documentation and evidence.

If no real defect is found, the final production tree under:

```text
firmware/src
firmware/hal
firmware/loader
firmware/app.ld
build.sh
tools
```

must remain byte-identical to `c93c39ad344760b4da0dfbc76829ca5fef886174`.

If a real defect is exposed:

1. stop the closeout;
2. make the smallest bounded production fix;
3. add a focused regression that fails on the frozen D045 baseline and passes after the fix;
4. record the exact source/resource/artifact delta;
5. rerun the complete WP4.10 exit gate.

WP4.10 must not add synth features, new parameters, new tracker commands, new allocation policy, persistence, live capture, FM4, UI pages or physical-hardware assumptions.

## 2. What “benchmark” means without physical FM-1 hardware

No physical FM-1 is currently available. Therefore WP4.10 separates three kinds of evidence:

### A. Deterministic functional workload

The actual linked D037-D045 path is exercised under a frozen representative eight-track workload and produces deterministic state/audio hashes.

### B. Structural real-time bound proof

Tests/static audit prove exact or bounded work counts that are independent of host CPU speed: fixed voice scans, fixed track scans, queue caps, cache/list ownership, row/tick ordering and absence of unbounded runtime work.

### C. Pinned target build/resource proof

The real JieLi/PI32v2 + pinned AC79 SDK build must succeed with exact image/RAM/pool and firmware hashes frozen.

Host wall-clock runtime may be recorded as **diagnostic information only**. It is never a pass/fail proxy for AC791N execution margin and must never be described as hardware timing proof.

Physical install/boot/audio quality and measured on-device callback margin remain an explicit later hardware gate.

## 3. Frozen representative song/workload

The representative benchmark uses pattern 0, all eight logical tracks and all 64 rows.

### Engine assignment

At every row block there must be two simultaneous instances of each D044 engine family.

Initial/default mapping is:

```text
track 0 -> instrument 0 -> PULSE
track 1 -> instrument 1 -> NOISE
track 2 -> instrument 2 -> TRIANGLE
track 3 -> instrument 3 -> WAVETABLE-8
track 4 -> instrument 0 -> PULSE
track 5 -> instrument 1 -> NOISE
track 6 -> instrument 2 -> TRIANGLE
track 7 -> instrument 3 -> WAVETABLE-8
```

At rows `16`, `32` and `48`, explicit NOTE instruments rotate by one engine:

```text
instrument = (track + row/16) & 3
```

so engine/instrument identity-change reset behavior is exercised while the global two-of-each-engine distribution remains balanced.

All intervening NOTE rows use `FMT_INSTRUMENT_INHERIT` unless a fixture specifically requires an explicit malformed/unsupported instrument case.

### Note / ownership cycle

The 64-row pattern repeats a deterministic eight-row ownership cycle:

```text
row % 8 == 0 : NOTE on all 8 tracks
row % 8 == 1 : TIE on all 8 tracks
row % 8 == 2 : ordinary NOTE on all 8 tracks
row % 8 == 3 : OFF on all 8 tracks with long RELEASE row values
row % 8 == 4 : NOTE on all 8 tracks while prior detached release tails remain candidates
row % 8 == 5 : TIE on all 8 tracks
row % 8 == 6 : ordinary NOTE / D033 event-pressure row
row % 8 == 7 : ordinary NOTE / transition row
```

NOTE pitch is deterministic and bounded to legal MIDI/Q7 range, derived from track and row so every track differs while repeated pattern wraps reproduce exactly.

Rows 3 -> 4 are the canonical D043 contention boundary: all eight primaries detach into RELEASE tails, then eight new primary NOTEs arrive in track order. The test must prove only unowned RELEASE tails are stolen and no still-owned cross-track voice is stolen.

## 4. Exact full D028 pool pressure

The benchmark must fill the complete 2,048-record D028 pool using exactly the 512 valid `(track,row)` owners in pattern 0:

```text
8 tracks * 64 rows * 4 locks = 2048 records
```

No owner exceeds the existing four-lock limit.

Every even row receives four locks:

1. D031 VOICE LEVEL
2. D031 FINE PITCH
3. D031 TRACK LEVEL
4. that engine page's slot-0 semantic parameter:
   - PULSE -> WIDTH
   - NOISE -> ATTACK
   - TRIANGLE -> ATTACK
   - WAVETABLE-8 -> TABLE

Every odd row receives four D044 locks:

1. engine page slot 0
2. DECAY
3. SUSTAIN
4. RELEASE

Values are deterministic functions of `(track,row,param)` and must include 0, midpoint and 255 boundary values across the matrix.

Release rows in the ownership cycle must use a sufficiently slow RELEASE value to guarantee a detached tail is still present when the following NOTE row is dispatched in the deterministic host fixture.

Required invariants:

- `used_count == 2048` before playback begins;
- the next valid allocation fails pool-full without partial mutation;
- all 512 D031/D044 row snapshots rebuild deterministically;
- audio never traverses D028;
- the full pool remains structurally valid after the complete benchmark;
- final lock/cache state receives a frozen deterministic digest.

## 5. D032 / D033 effect-event pressure

Every `(track,row)` stores one deterministic effect command/value.

Commands cycle through the complete assigned domain `0x00..0x08` as a deterministic function of track + row.

The value schedule must include, across each relevant command family:

```text
0, 1, 2, 3, 4, 5, 6, 255
```

and representative ordinary non-boundary byte values for D032 pitch effects.

The benchmark must prove through the real linked path:

- ARPEGGIO / SLIDE_UP / SLIDE_DOWN / PORTAMENTO / VIBRATO remain bounded and deterministic;
- NOTE DELAY / RETRIG / NOTE CUT still use the exact six-tick D020-derived grid;
- D033 values above 5 remain scheduling no-ops;
- no tick 6 exists;
- row boundary wins over sub-row work;
- row 63 -> row 0 cannot leak pending action/state;
- D031 FINE remains additive after D032 persistent/transient pitch;
- D044 A/D/S articulation snapshot semantics and row-time RELEASE semantics remain intact.

Existing exact 120 BPM anchors remain authoritative:

```text
tick 1  919
tick 2  1838
tick 3  2757
tick 4  3675
tick 5  4594
next row 5513
```

## 6. Long linked-engine run

A long deterministic linked-engine run must cross at least:

```text
4096 rows = 64 complete 64-row pattern wraps
```

The long-run test may use a compact no-WAV harness, but it must execute the real D038-D045 state transitions/render ownership rather than a separate mathematical model.

Required final evidence:

- exact final transport step/phase;
- exact number of row dispatches;
- exact number of effect ticks observed;
- exact D033 action counts;
- exact voice-allocation outcome counts (`REUSE/FRESH_IDLE/STEAL_TAIL/NO_VOICE` where observable);
- zero active cross-track steals;
- deterministic final engine/allocator/effect/event/cache digest;
- repeat run byte-identical/digest-identical.

The existing six-hour D020 transport proof remains inherited and must still pass separately; WP4.10 does not need to render six hours of audio.

## 7. Exact structural work bounds

WP4.10 must verify the final `app_audio()` ownership/work shape rather than infer it from successful audio output.

### Callback-start queue work

D034 remains the only general main -> audio queue:

- capacity exactly 32 events;
- callback consumes at most 32 events;
- overflow recovery discards compromised unread state in O(1);
- no spin/retry or producer mutation of consumer ownership.

### Tracker work per audio frame

During linked tracker playback:

- exactly one `fmt_8bit_engine_render()` call per rendered tracker frame;
- engine render scans exactly 8 fixed voice slots;
- no heap allocation, store traversal, lock-list traversal or screen call from render;
- on a non-row-boundary running frame, at most 8 D032 tick-due checks occur;
- on a row-boundary frame, exactly 8 fixed track-row events are considered in ascending track order and the D032 tick path is skipped for that frame;
- each due track tick performs at most one D033 scheduled action;
- row/event helpers contain no variable-length queue/list traversal.

### Manual/audition work

The separate D034 audition lane remains bounded to four fixed sine voices. It is not folded into the eight tracker voices in WP4.10.

The closeout must fail if the realtime path gains an allocator, an unbounded loop, D028 traversal, screen work, floating-point DSP, runtime 64-bit division, spin/retry publication or a second tracker/effect clock.

## 8. Concurrent D034 queue pressure

The linked tracker benchmark must include a deterministic concurrent audition-pressure fixture while all eight tracker tracks are active.

Required epochs:

1. exactly 32 accepted NOTE ON/OFF queue events in FIFO order;
2. one or more rejected pushes while full;
3. audio overflow detection -> manual/audition fail-to-silence + O(1) discard;
4. recovery NOTE ON/OFF after the overflow epoch;
5. no stuck manual gate;
6. linked eight-track tracker transport/effect timing remains deterministic and is not reset by audition overflow.

The benchmark must distinguish tracker-engine state from audition-lane state so an audition overflow cannot be hidden by the tracker mix.

## 9. Main/editor/UI pressure remains bounded

WP4.10 reuses the authoritative Phase 3/4 UI and editor stress rather than inventing a new screen architecture.

At minimum rerun:

- full D028 pool stress;
- held-step edit/browse/delete context transitions;
- maximum visible-row lock inspection;
- D035 ten-region / max-two-redraw backlog convergence;
- HOME return pixel cleanup;
- track/page selection boundaries;
- D044 page-aware lock writes/deletes and row-cache refresh.

Required ownership remains:

- screen/presentation is main-loop only;
- main/editor may bounded-scan inheritance/locks;
- audio consumes aligned publication snapshots only.

## 10. Representative host audio evidence

A finite canonical full-app host fixture must render all eight tracks simultaneously with all four engines and the representative D031/D032/D033/D044 workload.

It must freeze:

- WAV SHA-256;
- frame count;
- deterministic final tracker/engine/queue digest;
- no unexpected MIDI output from tracker/p-lock operations;
- no sample outside signed 16-bit final clamp;
- repeat-run identity.

The normal WP4.9 linked default WAV remains separately frozen:

`17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

WP4.10 may add a new representative benchmark WAV, but must not silently replace the normal default-app reference.

## 11. Pinned target/resource exit gate

Use the same pinned target inputs as WP4.9:

- JieLi toolchain `jieli-linux-toolchains-20260730.1`
- AC79 SDK commit `d179b4484759423312073f5fbb232501aa491047`
- identity `FM-1_987`

If production is unchanged, the target build must remain exactly:

```text
image       72,504 B
normal RAM  19,604 B
pool       113,000 B
```

and:

- `fm1tracker.bin` SHA-256 `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`
- `fm1tracker-0.1.fwsc` SHA-256 `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`

D037 Phase 4 ceilings remain hard limits:

```text
image      <= 132,496 B
normal RAM <= 21,488 B
pool       <= 129,384 B
```

Current WP4.9 margin is therefore:

```text
image       59,992 B
normal RAM   1,884 B
pool        16,384 B
```

The final target audit must also retain register-access hygiene (`hal/ only`) and inspect the important linked state/cache symbols/sections.

## 12. Inherited regression set

WP4.10 must rerun every authoritative regression still semantically applicable after D045.

This includes at least:

- Phase 2 model/store/transport/input/STEP REC behavior;
- D027-D030 registry/lock/editor behavior;
- track-aware D031/D032 cache tests;
- D033 event/tick-grid tests;
- D034 queue/wrap/overflow/recovery tests;
- D035 UI/framebuffer convergence;
- D036 worst-case lock/queue/timing tests;
- D038-D044 oscillator/envelope/allocator/page focused tests;
- WP4.9 Gate 5 canonical linked fixture matrix;
- WP4.9 Gate 6 realtime/static ownership audit;
- normal WP4.9 linked default host WAV;
- pinned WP4.9 target artifact/resource identity.

Historical proof-sine tracker WAV assertions superseded by D045 must remain explicitly excluded rather than silently rebased.

## 13. Host timing policy

A benchmark implementation may record host elapsed time, per-frame throughput or aggregate host render rate for diagnostics/repeatability.

Such numbers:

- are not normative acceptance thresholds;
- must be labelled host-only;
- must not be converted into claimed AC791N CPU percentage or execution margin;
- must not replace the structural boundedness + real target build gates above.

A future physical-FM-1 benchmark may supersede this limitation with measured on-device callback/cycle evidence.

## 14. Required evidence and exit markers

The final WP4.10 verification must produce durable evidence in:

`docs/WP4_10_VERIFICATION.md`

At minimum freeze:

- clean production baseline / source-delta result;
- representative benchmark workload description/version;
- full-pool lock/cache digest;
- long-run linked state digest and row/tick/action/allocation counts;
- representative benchmark WAV hash/frame count;
- D034 concurrent-pressure result;
- realtime structural audit result;
- inherited regression result;
- pinned toolchain/SDK identity;
- image/RAM/pool and remaining D037 margin;
- BIN/FWSC hashes;
- explicit physical-hardware limitation.

Recommended machine-readable markers are:

```text
WP4_10_PRODUCTION_FREEZE_OK=1
WP4_10_FULL_POOL_LINKED_OK=1
WP4_10_LONG_RUN_OK=1
WP4_10_QUEUE_PRESSURE_OK=1
WP4_10_REALTIME_AUDIT_OK=1
WP4_10_INHERITED_REGRESSIONS_OK=1
WP4_10_TARGET_IDENTITY_OK=1
WP4_10_PHASE4_EXIT_OK=1 hardware_margin_claim=none
```

## 15. Phase 4 exit rule

Phase 4 may be marked **COMPLETE at host/device-build level** only when:

1. D046 is recorded in `DECISIONS.md` and project state before benchmark implementation;
2. the 2,048-record representative linked workload is deterministic and bounded;
3. all eight logical tracks and all four engines survive the representative/long-run matrices;
4. D031/D032/D033/D044 precedence/timing remains exact through the linked path;
5. D043 release-tail contention never steals another track's still-owned active primary voice;
6. D034 queue pressure/overflow/recovery coexists safely with linked tracker playback;
7. no unbounded realtime work or ownership regression is found;
8. inherited non-obsolete regressions remain green;
9. the pinned target build is inside D037 ceilings and exact resources/artifacts are frozen;
10. any real defect fix is documented and the entire exit gate reruns green;
11. physical FM-1 limitations are stated rather than inferred away.

Only after these gates pass may Phase 4 / issue #30 and WP4.10 / issue #41 close and the repository hand off to Phase 5/FM4 planning.

## 16. Non-goals

WP4.10 does not add:

- FM4 / Phase 5 DSP;
- more than eight sequenced Phase 4 voice slots;
- new oscillator types or wavetable data;
- filters, sends or new FX;
- new p-lock IDs or tracker effect commands;
- live REC/capture, overdub or metronome semantics;
- persistence/project save-load;
- a new instrument/base editor;
- mute/solo or audible TRACK PAN;
- physical hardware timing claims without a physical FM-1.
