# D047 - Mixed-engine instrument architecture

Date: 2026-10-24  
Status: **ACCEPTED - product/architecture direction frozen before Phase 5 production work**  
Supersedes: D004 project-wide sound-mode restriction and the old Phase 9 deferral of mixed engines  
Does not supersede: D037-D046 Phase 4 implementation/verification contracts

## Decision

FM1Tracker is one engine-agnostic tracker/sequencer. A project is **not** globally an 8-BIT project or a 16-BIT project.

Each instrument slot owns its own sound-engine family and engine identity. Logical tracks and tracker rows reference instruments, so one pattern/project may freely combine instruments from different engine families.

The intended product model is:

```text
TRACKER / SONG
    |
    +-- track/row -> instrument slot
                       |
                       +-- engine family
                       +-- engine
                       +-- engine-specific parameters/state
```

Examples that the architecture must support without another sequencer redesign:

```text
Track 1 -> PULSE
Track 2 -> NOISE
Track 3 -> FM4
Track 4 -> modern VA / supersaw engine
Track 5 -> WAVETABLE-8
Track 6 -> drum engine
Track 7 -> FM4
Track 8 -> TRIANGLE
```

The exact future engine list is still resource- and work-package-gated. D047 enables mixed families architecturally; it does not promote every possible synth engine into the current release scope.

## Why this changes now

The original D004 restriction was a scope simplification while the tracker, p-lock system and first engine family were unproven. Those foundations now exist.

Keeping a global 8-BIT/16-BIT project mode would create an artificial musical limitation and would force another architecture migration later when modern synth, drum or additional digital engines are introduced.

The current data model already separates tracker lanes from instruments:

- `FmtStep.instrument` selects or inherits an instrument;
- `FmtTrackConfig.instrument` supplies a track default;
- `FmtInstrumentHeader` is already exactly 4 B with separate `mode` and `engine` bytes;
- tracker timing, p-lock storage and effect commands are already engine-independent;
- D045 already defines deterministic instrument/engine identity-change reset behavior.

Therefore this is primarily a routing/ownership generalization, not a tracker rewrite.

## Non-destructive Phase 4 rule

WP4.10 / D046 remains the only active work package until Phase 4 closes.

D047 causes **no Phase 4 production-source change**. In particular, it does not change:

- the D045/D046 production baseline;
- the linked 8-BIT engine or its exact resources/hashes;
- D043 allocator semantics;
- D044 parameter IDs/page behavior;
- D045 host fixtures;
- D046 representative benchmark obligations;
- any completed Phase 1-4 verification evidence.

The current Phase 4 implementation is intentionally an all-8-BIT first engine-family implementation. It remains a valid, frozen subset of the new architecture.

Historical contracts and verification documents must continue to describe what was actually implemented and proven at their checkpoints. They are not rewritten to pretend mixed-engine routing existed earlier.

## Existing layout compatibility

### FmtStep

`FmtStep` remains exactly 8 B. Its one-byte `instrument` field already provides the required sequencer-to-instrument indirection. No step-layout change is required by D047.

### FmtTrackConfig

The existing track default-instrument byte remains valid. A logical track has no permanent sound family of its own; it normally plays its selected/default instrument, and a row may explicitly select another instrument according to the existing NOTE-only inheritance rules unless a later tracker-semantics decision supersedes them.

### FmtInstrumentHeader

The existing 4-byte layout remains unchanged through Phase 4:

```c
typedef struct {
    uint8_t mode;
    uint8_t engine;
    uint8_t version;
    uint8_t flags;
} FmtInstrumentHeader;
```

For future mixed-engine work, the current `mode` byte is conceptually the **engine-family discriminator**. The field is not renamed in production during WP4.10 because that would destroy the D046 production freeze for no functional benefit.

Current numeric family values remain valid compatibility identities:

```text
0 = current 8-BIT/chip family
1 = current 16-BIT/FM-digital family
```

Additional family identities, if required for modern synth/drum/other engines, require an explicit later decision. They are not guessed or silently assigned here.

WP5.0 must decide whether source code keeps the historical `mode` name, adds a semantic alias, or performs a measured rename. The four-byte header size and existing values must remain stable unless a separate migration decision proves a reason to change them.

### FmtSongMeta.mode

`FmtSongMeta.mode` remains physically present and unchanged while Phase 4 is frozen, but D047 supersedes its role as a project-wide playback eligibility gate.

In the target mixed-engine architecture, the song-level mode byte must **not** prevent an instrument from another valid family from playing. WP5.0 must explicitly redefine the field as one of:

- legacy/default-family metadata;
- a compatibility/import hint;
- reserved metadata for a later serialized format;

or another documented non-gating meaning.

No raw runtime struct has yet been frozen as the Phase 6 persistent file format, so this semantic cleanup can occur before serialization without a released-project migration burden.

## Engine routing contract for Phase 5

Phase 5 must begin with a new bounded package:

**WP5.0 - mixed-engine routing contract and migration**

WP5.0 is a prerequisite for linking FM4 into the tracker. It must not be skipped by directly adding FM4 to the D045 8-BIT path.

WP5.0 must freeze and prove at least the following.

### 1. Instrument is authoritative

At NOTE dispatch, the selected/resolved instrument header determines engine family + engine. The sequencer itself remains ignorant of oscillator/FM implementation details.

### 2. Mixed families may coexist

Different logical tracks may own voices from different engine families at the same time. A single track may also change to an instrument from another family on a later NOTE if the normal instrument-selection semantics request it.

### 3. Single primary ownership per logical track

Across all engine families there must still be at most one currently owned primary sequenced voice for one logical track.

Family switching must not leave two simultaneously owned primary voices for the same track. A previous engine may retain a detached release tail only under an explicitly bounded policy analogous to D043.

### 4. No active cross-track stealing

The D043 safety property survives generalization: one track's new engine request may never silently steal another track's still-owned active primary voice.

Exact cross-family allocator representation is deliberately deferred to WP5.0 measurement. A shared heterogeneous slot union, family-specific pools behind one global ownership router, or another bounded representation may be chosen only after its RAM/CPU cost is measured.

### 5. Static, bounded dispatch

Mixed-engine routing does not mean a desktop plugin system.

Firmware routing remains compile-time/static and bounded. The audio path may use explicit integer family/engine switches or equally bounded tables, but no heap allocation, dynamic loading, unbounded registration traversal, virtual plugin discovery or blocking lookup is allowed.

### 6. Engine-specific state remains engine-specific

The current `Fmt8BitEngine`, D044 base bank and D044 row cache remain the 8-BIT family's proven implementation. FM4 and future families may own different fixed runtime state and parameter payloads.

WP5.0 must define the narrow common adapter contract rather than forcing every engine into the physical `Fmt8BitVoice` layout.

### 7. Common tracker semantics stay common

D020 transport, D031 common level/fine semantics, D032/D033 tracker effects/events, D034 audition ingress and D028 lock storage remain sequencer infrastructure.

An engine family may define how a semantic action is rendered, but it may not create a parallel transport, parallel p-lock store or private tracker clock.

### 8. Parameter identity remains semantic

D027 one-byte parameter IDs remain stable. Existing assigned IDs are never renumbered because of D047.

The currently frozen ranges remain valid historical namespaces. New FM4/modern/drum parameters require explicit assigned IDs and collision tests. A future engine does not gain lockability merely by belonging to a family.

A p-lock that is not meaningful for the resolved instrument/engine must be ignored safely, never reinterpreted as an unrelated parameter.

### 9. Four-control primary-page philosophy remains useful

The hardware-facing concept of a small current-engine primary parameter page remains. Different engines may expose different semantic controls while the UI follows the selected instrument.

D047 does not require one universal physical parameter struct for all engines. Publication format and any common four-byte primary-page snapshot are frozen only after WP5.0/FM4 measurement proves the correct representation.

## Resource and polyphony policy

Architectural freedom does **not** mean every theoretical engine/voice combination is guaranteed to fit the FM-1.

The following are frozen product rules:

- eight logical tracker tracks remain the baseline;
- engine family choice must not silently reduce the logical track count;
- an engine may have a different voice cost/polyphony policy from another engine;
- unsupported resource combinations must fail or constrain themselves by an explicit deterministic policy, never by silent engine substitution or random quality reduction;
- CPU/RAM budgets remain measured gates before a new engine or larger polyphony promise is accepted;
- physical FM-1 callback margin remains a later hardware requirement.

The accepted live-performance target of 3-6 voices for at least one polyphonic instrument remains conditional on measured hardware budget.

## Product/UI direction

The product should no longer present a mandatory project-wide `8-BIT` versus `16-BIT` choice.

`8-BIT`, `FM/DIGITAL`, `MODERN`, `DRUM` and similar names may remain useful **engine categories** or browser groupings. They are not project types.

A user should be able to choose an instrument/engine per track and eventually per explicit tracker instrument event without thinking about a global sound mode.

The exact instrument browser and engine-category UI remain later UX packages. D047 freezes architecture, not a new menu implementation during WP4.10.

## Phase 5 order after D047

Phase 5 becomes:

```text
WP5.0 mixed-engine routing contract + migration
WP5.1 FM operator primitive
WP5.2 curated FM algorithm routing
WP5.3 ratio table
WP5.4 levels / modulation index
WP5.5 bounded feedback
WP5.6 FM envelope strategy
WP5.7 FM voice allocation / integration
WP5.8 FM parameter pages + p-lock mapping
WP5.9 mixed 8-BIT + FM4 pitch/modulation/instrument-switch validation
WP5.10 worst-case mixed-engine benchmark and Phase 5 exit
```

The exact issue IDs are assigned only when Phase 4 is formally closed.

## Future engines

The architecture must permit later dedicated work packages for engines such as modern virtual analog, supersaw, broader wavetable/digital synthesis, drum synthesis or other measured/licensed engines without changing the tracker/song/event architecture again.

This is an extensibility requirement, not a promise that those engines are all V1 prerequisites.

PCM/sample playback, authentic chip cores and additional expensive DSP remain separately gated by licensing, storage and profiling decisions.

## Required regression principle

When WP5.0 begins, mixed-engine work must prove that the complete frozen 8-BIT path still behaves identically when only 8-BIT instruments are used, except for any explicitly documented adapter-only artifact change that is demonstrated necessary.

At minimum the migration must retain:

- all eight logical tracks;
- D020/D032 exact timing;
- D028 lock safety;
- D031 common parameter precedence;
- D033 delay/retrig/cut behavior;
- D034 queue isolation;
- D043 no-owned-cross-track-steal property;
- D044 8-BIT parameter semantics;
- the four current 8-BIT engines;
- bounded realtime work;
- pinned target/resource measurement.

A new FM4/mixed-engine feature is not allowed to make the already-proven 8-BIT engine family less deterministic or less safe.

## Consequence

D004's project-wide mode restriction is superseded for future architecture. Mixed engine families are now core, not optional Phase 9 expansion.

Phase 4 remains unchanged and must close under D046 first. Once Phase 4 is green, Phase 5 starts with WP5.0 and only then begins FM4 production integration.
