# WP4.1 8-BIT Engine Interface Contract

Status: **FROZEN BY D037 BEFORE PRODUCTION ENGINE SOURCE**  
Date: 2026-10-24  
Package: WP4.1 / issue #31  
Phase 4 branch: `phase4/8bit-engine`  
Phase 4 branch start / final Phase 3 handoff: `d14955f23a11044e8a348884feca24f8978fd700`

This document is the normative Phase 4 engine/voice/realtime contract. It freezes the interface and resource boundaries that WP4.2-WP4.10 must obey before PULSE, NOISE, TRIANGLE or WAVETABLE-8 production DSP is written.

It does **not** implement an oscillator, envelope, final allocator, parameter page, multi-track engine integration or a new persisted project format. The current Phase 3 proof sine remains the production firmware audio path until a later Phase 4 package deliberately replaces it.

## 1. Fixed scope and engine identity

Phase 4 provides one `8-BIT` engine family with these required oscillator engines:

```text
engine 0  SAFE / NONE
engine 1  PULSE
engine 2  NOISE
engine 3  TRIANGLE
engine 4  WAVETABLE-8
engine 5..255 unsupported in Phase 4
```

`FmtInstrumentHeader.mode == 0` remains the current legacy/unassigned baseline used by the existing proof firmware. D037 reserves `mode == 1` for the Phase 4 `8-BIT` family. D037 does not assign a numeric Phase 5 mode.

An unsupported mode or engine never silently aliases another synth. Resolution fails safely to `SAFE / NONE`, which produces no oscillator sample and cannot create a stuck gate or stale voice.

The existing `FmtInstrumentHeader` remains exactly 4 bytes in WP4.1. D037 creates no serialized ABI and no Phase 6 persistence rule.

## 2. Ownership boundary

Audio owns all mutable engine runtime state.

Audio-owned state includes:

- the fixed voice pool;
- per-voice oscillator phase/state;
- per-voice gate/envelope state once WP4.6 exists;
- per-track active-voice handles;
- final voice allocation/stealing state once WP4.7 exists;
- current resolved pitch/engine-parameter state used by rendering.

Main/editor owns:

- `FmtSong` and the D028 lock store;
- instrument/editor UI state;
- lock/effect mutation;
- rebuilding/publishing bounded row/instrument snapshots.

Main must never mutate an active engine voice, oscillator phase, envelope state or allocator state directly.

The only main-to-audio runtime ingress paths allowed by D037 are:

1. naturally aligned fixed-size publication snapshots prepared by main and sampled by audio at deterministic boundaries; and
2. the already-frozen D034 32-entry / 268-byte SPSC runtime queue for audition NOTE ON/OFF intent.

D034 remains an audition/runtime-ingress queue. D021/D032/D033 tracker timing does not move into it.

## 3. Eight tracks and eight voices

The tracker remains exactly eight logical tracks under D005.

Phase 4 freezes a fixed maximum of **eight engine voice slots**. This is an initial Phase 4 realtime/resource target, not a promise about later polyphonic-track capacity.

For sequenced Phase 4 playback:

- each logical track has one primary active-voice handle;
- a track can therefore own at most one sequenced Phase 4 voice at a time;
- a voice slot has either one owning logical track or no owner;
- the allocator may choose any physical slot for a track, but the mapping is audio-owned;
- final stealing priority is deferred to WP4.7.

The interface must support an allocation result of `NO_VOICE` without blocking, allocating or corrupting another track. WP4.7 will define the deterministic steal policy.

Future polyphonic live-performance work may supersede the one-primary-voice-per-track Phase 4 rule only through a later explicit decision and new resource evidence.

## 4. Common pitch domain

D037 reuses the D031/D032 pitch domain exactly.

Pitch presented to an engine voice is unsigned Q7 semitone pitch:

```text
0..16383
1 semitone = 128 units
integer MIDI note n = n << 7
```

The final tracker pitch remains:

```text
final_q7 = clamp(
    persistent_D032_pitch_q7
    + (D031_FINE - 128)
    + D032_transient_q7,
    0,
    16383)
```

The engine layer receives this already-resolved `final_q7`. It must not reinterpret D031 lock storage or D032 effect command bytes.

All pitched Phase 4 oscillators use a 32-bit wrapping phase accumulator. Pitch-to-phase-increment conversion is shared/common code, deterministic at 44.1 kHz, and runs only when pitch changes, never as an expensive conversion inside every per-sample oscillator render.

Runtime pitch conversion may use bounded integer lookup/interpolation. It may not use floating point or runtime 64-bit division.

NOISE may use engine-specific clock/state rules in WP4.3, but it still receives the same resolved Q7 pitch and cannot invent a parallel tracker-pitch representation.

## 5. Tracker note/event adapter semantics

The engine adapter consumes the already-frozen D025/D033 semantics rather than redefining stored tracker events.

### NOTE

A NOTE requests/retains the track's primary voice, applies the resolved pitch and row parameters, and gates the voice on.

Ordinary NOTE does **not** imply an oscillator-phase restart. This preserves the distinction between a normal note event and D033 RETRIG. A newly reset/inactive voice begins from deterministic phase/state zero when first activated.

Envelope note-on/retrigger behavior is supplied through the common envelope boundary after WP4.6.

### TIE

TIE never allocates a voice, never restarts oscillator phase and never creates sound when no voice is active. It continues an already active track voice while allowing D031/D032 row-local level/pitch updates to take effect.

### EMPTY / REST

EMPTY sends no note/gate command. An already active voice may continue naturally, and D031/D032 row-local parameter/effect updates may still alter the active voice. EMPTY cannot resurrect an inactive voice.

### OFF

OFF releases the track's active voice through the common gate/envelope boundary. It does not free/reassign a slot synchronously in a way that can click; final release-to-free policy belongs to WP4.6/WP4.7.

### RETRIG

D033 RETRIG acts on the already active voice at the exact frozen D032 tick. It performs no allocation and does not change pitch or resolved row parameters.

RETRIG explicitly restarts oscillator phase/state required for a deterministic retrigger and calls the envelope retrigger hook once WP4.6 exists. RETRIG with no active voice is a no-op.

### NOTE DELAY

D033 NOTE DELAY withholds the ordinary NOTE until the exact frozen tick. When fired, it uses the exact D031/D032 row snapshot captured for that row and then performs the same NOTE adapter operation described above.

### NOTE CUT

D033 NOTE CUT performs the same release operation as OFF at the exact frozen tick. It does not allocate or inspect editor storage.

## 6. Per-track versus per-voice state

Per-track runtime state contains only common adapter ownership/state such as:

- primary voice handle / `NO_VOICE`;
- currently resolved instrument/mode/engine identity;
- logical active/gate state required to interpret TIE/EMPTY/OFF safely;
- current fixed row snapshot references/values needed by the engine adapter.

Per-voice state contains render-time state such as:

- owner track or unowned marker;
- engine ID;
- Q7 pitch and derived phase increment;
- 32-bit oscillator phase;
- engine-specific fixed state;
- common gate/envelope state;
- deterministic allocation age/priority fields once WP4.7 defines them.

No voice stores a pointer into `FmtSong`, D028 lock records, screen state or editor state.

## 7. Engine dispatch and oscillator render interface

Phase 4 uses integer engine IDs and explicit bounded `switch` dispatch. There is no virtual dispatch, heap object, function-pointer vtable or plugin-style runtime registration in the audio path.

Each active voice render performs exactly one selected oscillator render for that frame.

The oscillator primitive produces one signed mono nominal-Q15 sample in the inclusive range:

```text
-32767..32767
```

Engine-specific state is fixed-size. PULSE, TRIANGLE and WAVETABLE-8 use the common 32-bit phase domain. NOISE uses a deterministic fixed-size integer state defined by WP4.3.

WAVETABLE-8 lookup tables are immutable/const firmware data unless a later package explicitly freezes editable-table storage. Const waveform tables count against app image, not runtime pool RAM.

## 8. Envelope boundary

The oscillator and envelope are separate layers.

An oscillator produces the raw nominal-Q15 source sample. The common envelope/level stage supplies a bounded unsigned Q15 gain:

```text
0..32767
```

Voice scaling is fixed integer multiplication followed by deterministic right shift. The oscillator never traverses p-lock storage or owns envelope timing.

WP4.2-WP4.5 may use a minimal deterministic gate/level shim in isolated tests. The actual integer envelope primitive and its exact state machine are WP4.6 work.

## 9. Mix format and clipping contract

The Phase 4 engine mixer uses signed 32-bit accumulators.

For each frame:

1. render at most eight active voice samples;
2. apply each voice's common Q15 gain/level stage;
3. sum into signed 32-bit mix accumulators;
4. apply track/channel routing that exists at that package checkpoint;
5. saturate explicitly before the final nominal 16-bit audio range is emitted/added to the platform output path.

For eight full-scale nominal-Q15 voices the raw sum magnitude is bounded below 262,144, far inside signed 32-bit range. Integer wraparound is forbidden; clipping is explicit saturation.

Until a later package deliberately activates pan, the 8-BIT engine is mono-centre and feeds both output channels equally. D031 TRACK PAN remains storage-only and is not silently made audible by D037.

## 10. Engine-specific parameter publication and p-lock precedence

D027 remains authoritative: Phase 4 engine parameter IDs may only be assigned from `0x20..0x5F`. Concrete IDs and four-parameter page mappings are WP4.8 work.

D037 freezes the precedence rule now:

```text
resolved row value = engine/instrument default
                   overridden by an assigned D028 row p-lock when present
```

D031 common VOICE LEVEL / FINE / TRACK LEVEL remain common parameters and are not duplicated in the 8-BIT namespace.

Audio never traverses D028 lock lists. WP4.8 may publish one naturally aligned 32-bit resolved engine-parameter word for every `(track,row)`:

```text
4 resolved one-byte engine parameter values
```

Maximum cache size:

```text
8 tracks * 64 rows * 4 bytes = 2048 bytes ordinary RAM
```

The byte-to-parameter ordering is frozen only when WP4.8 assigns the concrete page/ID schema. Until then D037 reserves the publication shape and maximum cost, not parameter meanings.

`FmtInstrumentHeader` is not enlarged by WP4.1. Editable instrument-default backing storage, if required, must be explicitly designed and budgeted by the package that introduces it; raw runtime structs must not become a Phase 6 file format by accident.

## 11. D032/D033 integration boundary

D032 remains the sole owner of tracker pitch-effect interpretation and six-tick phase semantics. The engine adapter receives resolved Q7 pitch changes at row/tick boundaries.

D033 remains the sole owner of RETRIG / NOTE DELAY / NOTE CUT scheduling. The engine adapter receives fixed actions at their exact scheduled tick.

The 8-BIT engine does not inspect `FmtStep.effect`, does not create a second effect clock and does not route D033 through D034.

## 12. Reset and malformed-state behavior

Reset is deterministic:

- all eight track handles become `NO_VOICE`;
- all eight voice slots become inactive/unowned;
- oscillator phase/state is zeroed to deterministic engine defaults;
- gate/envelope state is silent;
- allocator age/state is reset;
- unsupported mode/engine state resolves to SAFE/NONE.

Malformed or unsupported engine data must fail toward silence for the affected voice/track only. It must not read out of bounds, fall back to another oscillator, retain a stale engine state or poison another track.

## 13. Hard realtime work bounds

The complete Phase 4 audio path must remain statically bounded.

Maximum work categories are:

- callback ingress: at most the existing 32 D034 queue entries;
- row boundary: at most eight logical tracker-row adapter dispatches;
- D032/D033 effect tick: at most eight track pitch/action updates;
- each rendered frame: at most eight active voice oscillator renders plus fixed common gain/mix work;
- allocator scan: at most eight voice slots when allocation is required.

No Phase 4 audio operation may loop over patterns, 64 rows, 2,048 locks or a variable linked structure.

Forbidden in the audio path:

- heap/pool allocation or free;
- D028 linked-list traversal;
- screen/UI work;
- file/flash persistence;
- blocking locks, waits or retries;
- floating point;
- runtime 64-bit division;
- recursion;
- unbounded search;
- virtual/function-pointer dispatch for oscillator selection.

Pitch-to-increment recomputation occurs only when pitch changes, not every sample.

## 14. Complete Phase 4 resource budget

The frozen Phase 3 baseline is:

| Resource | Phase 3 used | Device limit | Free at Phase 4 start |
|---|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B |

D037 gives the complete Phase 4 work these incremental ceilings:

| Resource | Maximum Phase 4 delta | Phase 4 total ceiling |
|---|---:|---:|
| app image | +65,536 B | 132,496 B |
| normal RAM | +8,192 B | 21,488 B |
| pool RAM | +16,384 B | 129,384 B |

Within the +8,192 B normal-RAM ceiling:

- the optional full 8-track × 64-row × 32-bit engine-parameter publication cache may consume at most 2,048 B;
- all Phase 4 engine realtime voice/track/allocator/envelope state together should remain at or below 2,048 B;
- the remaining normal-RAM allowance is reserved for integration/control evidence and later Phase 4 needs.

Const waveform/lookup tables belong in app image. Pool growth is not a default entitlement: any Phase 4 package using the +16 KiB pool allowance must justify editable runtime storage explicitly and measure it.

Crossing any Phase 4 ceiling requires a new recorded architecture decision before the over-budget implementation is accepted.

## 15. Migration from the Phase 3 proof sine

WP4.1 changes no production DSP.

WP4.2-WP4.8 may build oscillator/envelope/allocation/parameter primitives behind isolated deterministic host tests and target-compile gates without replacing the existing full-app proof lane.

The first package that replaces or materially reroutes the Phase 3 proof sine in production must:

- state that migration explicitly before the source change;
- preserve D020/D021/D025 timing/event semantics;
- preserve D031/D032/D033 precedence/timing through the adapter;
- preserve D034 ownership/overflow behavior;
- run every relevant Phase 2/3 regression;
- record the intentional audio-hash change with new deterministic reference evidence rather than silently editing old hashes.

The historical default WAV remains a migration baseline until such a deliberate superseding checkpoint.

## 16. Deterministic verification obligations

Each WP4.2-WP4.9 engine package must provide, as applicable:

- pure/focused host unit tests;
- deterministic sample vectors or frozen WAV hashes;
- exact reset/repeatability checks;
- malformed/unsupported fallback checks;
- target compilation with the pinned JieLi compiler;
- full firmware device build and measured image/RAM/pool deltas when production integration changes;
- inherited Phase 2/3 regression gates.

NOISE tests must use a deterministic documented seed/state. WAVETABLE-8 tests must freeze table/sample interpretation. Allocation tests must prove deterministic results under identical event streams.

WP4.10 must run a representative eight-track worst-case benchmark and close Phase 4 only if the final source remains within the D037 ceilings or a later explicit decision has superseded them.

## 17. WP4.1 exit condition

WP4.1 is architecture-only.

It may close when:

- D037 is present in `DECISIONS.md` and this contract is committed;
- `PROJECT_STATE.md`, `WORK_PACKAGES.md`, `ROADMAP.md` and issue #31 agree on the frozen contract;
- a verification gate proves no production firmware/DSP source changed from the clean Phase 4 branch start;
- the Phase 3 baseline still passes the selected inherited build/safety checks;
- no PULSE/NOISE/TRIANGLE/WAVETABLE-8 production implementation exists yet.

Only after that closeout may WP4.2 production oscillator work begin.
