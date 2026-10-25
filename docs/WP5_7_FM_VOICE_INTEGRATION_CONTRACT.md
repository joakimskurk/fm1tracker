# WP5.7 FM4 Voice Allocation and D048 Integration Contract - D055

Status: **FROZEN BEFORE FM VOICE/ROUTER INTEGRATION CHANGES**
Date: 2026-10-25
Package: WP5.7 / issue #50
Branch: `phase5/fm4-voice-integration`
Parent closeout: WP5.6 / D054 `886cb496f4e73a91c4ded16bf9e722b770a040f6`
Precontract audit: `WP5_7_PRECONTRACT_AUDIT.md`

This is the normative D055 contract for the first production FM4 voice engine and D048 family-router integration. No FM voice engine source, D050 composition seam or `dinghy.c` FM dispatch change is allowed before the source-absent D055 gate passes.

## 1. Scope

WP5.7 integrates the already completed D049-D054 primitives into one bounded FM family engine:

```text
D048 family route
      |
      v
D055 FM engine - 8 fixed slots
      |
      +-- D051 ratio mapping
      +-- D052 LEVEL/INDEX mapping
      +-- D054 per-operator envelopes
      +-- D053 OP4 one-sample feedback
      +-- D050 algorithm topology
      +-- D049 operators
```

WP5.7 does not define tracker-facing FM pages, p-lock IDs or persistent project format. Those remain WP5.8 and Phase 6 work.

## 2. Stable FM engine identity

The first FM family engine ID is:

```text
FMT_FM_ENGINE_FM4 = 0
```

Only engine ID 0 is supported by WP5.7.

Malformed/unsupported FM engine IDs fail toward silence and may not commit the D048 route.

## 3. Voice count and allocator

WP5.7 freezes:

```text
FMT_FM_ENGINE_SLOTS = 8
FMT_FM_ENGINE_TRACKS = 8
```

The FM engine uses the existing `FmtVoiceAllocator` and exact D043 policy.

Required acquisition order:

1. reuse the requesting track's current FM primary;
2. otherwise use the lowest-index IDLE slot;
3. otherwise steal the oldest detached RELEASE tail;
4. ties between tails use the lower slot index;
5. never steal an active primary owned by another track;
6. if no legal slot exists, NOTE fails and the common D048 route remains/returns NONE.

There is no independent FM allocator algorithm.

## 4. FM patch input shape

WP5.7 freezes an engine-facing patch input of exactly 28 B:

```c
typedef struct {
    uint8_t ratio_id[4];
    uint8_t level_control[4];
    FmtFmEnvelopeControls envelope;
    uint8_t algorithm;
    uint8_t index_control;
    uint8_t feedback_control;
    uint8_t reserved;
} FmtFmPatch;
```

Exact size:

```text
FmtFmPatch = 28 B
```

The `reserved` byte must be zero in the WP5.7 bridge patch and ignored by DSP.

This patch type is an engine input shape, not a persistence ABI.

## 5. Immutable WP5.7 bridge patch

Because WP5.8 owns FM parameter pages and mutable instrument storage, WP5.7 uses one immutable ROM bridge patch for production router dispatch.

Exact serialized bytes:

```text
ratio_id          1 5 1 5
level_control   255 96 255 96
attack            0 0 0 0
decay             0 0 0 0
sustain         255 255 255 255
release           96 96 96 96
algorithm          2
index_control     64
feedback_control   0
reserved           0
```

Algorithm 2 is DUAL2.

Serialized SHA-256:

```text
3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

No 16-instrument mutable FM bank may be added in WP5.7.

## 6. Exact FM voice layout

D055 freezes the following persistent runtime shape:

```text
FmtFmAlgorithmState       16 B
FmtFmEnvelopeState        20 B
FmtFmFeedbackState         2 B
alignment padding           2 B
phase_increment[4]         16 B
static_level_q15[4]         8 B
FmtFmEnvelopeControls      16 B
mod_depth_q14               2 B
gain_q15                    2 B
ratio_id[4]                 4 B
feedback_control            1 B
algorithm                   1 B
instrument                  1 B
reserved                    1 B
--------------------------------
FmtFmVoice                 92 B
```

Required static assertion:

```text
sizeof(FmtFmVoice) == 92
```

The four ratio IDs are retained so pitch-only row/tick updates can recompute phase increments without reading mutable instrument state.

## 7. Exact FM engine layout

The engine is:

```c
typedef struct {
    FmtVoiceAllocator allocator;
    FmtFmVoice voice[8];
} FmtFmEngine;
```

Required sizes:

```text
FmtVoiceAllocator = 52 B
FmtFmEngine        = 788 B
```

Required static assertions must fail the build if either size drifts.

## 8. Four-envelope allocator stage projection

For allocator validation/acquisition, each FM voice projects D054's four stages to one D043 stage:

```text
all OFF
    -> FMT_ENVELOPE_IDLE

all stages are OFF or RELEASE,
with at least one RELEASE
    -> FMT_ENVELOPE_RELEASE

otherwise
    -> FMT_ENVELOPE_SUSTAIN
```

The active value is only an ownership-valid marker. It does not replace or mutate D054 per-operator stages.

A detached non-idle FM slot must therefore always project to RELEASE.

## 9. NOTE semantics

A valid FM NOTE:

1. validates track, FM engine identity and patch;
2. validates/reaps allocator state;
3. acquires using D043;
4. snapshots instrument ID, ratio IDs, LEVEL/INDEX/ADSR/FEEDBACK/algorithm and row gain;
5. maps current tracker pitch through D051 for four phase increments;
6. hard-retriggers all four D049 phases;
7. clears D053 feedback history;
8. hard-retriggers D054 from zero using the snapped ADSR controls;
9. returns the acquired slot.

A NOTE always hard-retriggers FM phase/feedback/envelopes, including same-track primary reuse. This is the frozen FM V1 note-on behavior.

Malformed NOTE input fails toward safe silence for the affected FM track.

## 10. TIE/EMPTY and pitch update semantics

TIE/EMPTY never allocates and never changes the common route family.

A live FM primary may update:

- pitch;
- four D051 phase increments;
- row gain;
- patch-derived ratio/LEVEL/INDEX/ADSR/FEEDBACK/algorithm snapshots.

It must preserve:

- D049 operator phase;
- D053 feedback history;
- D054 envelope level/stage.

Thus D050 algorithm changes during a live voice retain feedback/envelope continuity exactly as D053/D054 already require.

## 11. RETRIG semantics

FM RETRIG:

- current-family only;
- never allocates;
- updates current row/pitch/patch snapshots;
- hard-retriggers D049 phases;
- clears D053 history;
- hard-retriggers D054 from zero.

No D048 route change occurs.

## 12. OFF/CUT semantics

FM OFF/CUT:

1. applies current row gain/patch release controls as already snapped for the voice;
2. calls D054 note-off on all four operators;
3. projects the resulting FM voice to RELEASE or IDLE;
4. detaches D043 ownership in the same event;
5. clears the common D048 route in the same event.

If all four operators become OFF immediately, the slot is immediately IDLE.

Otherwise it remains a detached release tail eligible for later D043 tail stealing.

## 13. Hard-silence semantics

Cross-family switch, malformed FM dispatch or explicit safe-silence:

- resets all D049 phases;
- clears D053 history;
- resets D054 envelopes to OFF/zero;
- detaches the track's FM primary;
- clears the FM voice snapshot;
- leaves unrelated FM tracks and release tails untouched.

## 14. D050/D053 composition seam

WP5.7 may extend D050 with exactly one backward-compatible API seam that accepts an OP4 sample already rendered/advanced.

Required semantics:

- the seam never advances OP4;
- it renders OP3, OP2 and OP1 exactly once according to the selected D050 algorithm;
- it performs exactly the same D050 carrier normalization;
- existing `fmt_fm_algorithm_render()` remains bit-exact and may internally call the new seam after rendering OP4 itself;
- the FM engine renders OP4 exactly once through `fmt_fm_feedback_render_op4()` and passes that sample to the seam.

At FEEDBACK=0, with equivalent initial state and controls, the new integrated path must match the accepted D050 path exactly.

No other D049-D054 semantic change is authorized.

## 15. Per-sample FM render order

For each of exactly eight fixed slots:

1. skip if all four D054 stages are OFF;
2. read current D054 envelope levels;
3. map snapped static D052 levels through D054;
4. render OP4 once through D053;
5. render OP3/OP2/OP1 through the D050 composition seam;
6. apply snapped row gain to the final voice sample;
7. advance D054 exactly once.

Then sum all eight FM slots in ascending slot order and clamp mono Q15.

There is no heap, linked list, runtime registry or variable-length traversal.

## 16. D048 integration

The existing common router remains authoritative.

WP5.7 changes only the FM cases:

### Cross-family NOTE

If current family differs from requested FM:

```text
hard-silence old family
clear common route
attempt FM NOTE
commit FM only on success
```

Failure leaves route NONE.

### Same-family FM NOTE

Attempt FM NOTE and keep/commit FM only on success. Failure hard-silences the affected FM primary and clears route.

### TIE/EMPTY

Update only the current FM primary. No allocation or route change.

### RETRIG

Retrigger current FM primary only. No allocation or route change.

### OFF/CUT

Release current FM primary, then clear common route in the same event.

### Effect-pitch updates

Pitch updates dispatch to the currently routed family. FM uses D051 with the snapped ratio IDs.

## 17. Production startup compatibility

WP5.7 must not change the accepted default all-8BIT song/instrument initialization.

Thus the normal host proof project remains all-8BIT and its audio sample stream must remain exact.

FM dispatch is exercised by focused host integration fixtures that publish a valid FM family/engine identity.

WP5.8 later provides tracker-facing FM instrument/page selection.

## 18. No FM page cache in WP5.7

WP5.7 adds:

- no second 2,048 B page cache;
- no mutable 16-instrument FM parameter bank;
- no FM p-lock IDs/pages.

The existing 8-BIT cache remains unchanged. FM ignores its page word in WP5.7.

WP5.8 owns shared-cache generalization and parameter mapping.

## 19. Resource ceilings

Parent running application:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
```

D055 freezes:

```text
FmtFmEngine target                    788 B
FmtFmEngine hard ceiling              800 B

WP5.7 normal-RAM increment ceiling   1024 B
WP5.7 total normal-RAM ceiling      20700 B

WP5.7 image increment ceiling        8192 B
WP5.7 total image ceiling           80984 B

WP5.7 pool increment                    0 B
WP5.7 total pool                    113000 B
```

Any breach requires a new decision before closeout.

## 20. Realtime bounds

Per sample:

- one fixed 8-slot 8-BIT render remains;
- one fixed 8-slot FM render is added;
- each active FM slot renders four operators exactly once;
- each active FM slot advances four envelopes exactly once;
- no runtime division/modulo;
- no runtime 64-bit integer arithmetic;
- no float/double;
- no heap;
- no song-store/editor/cache traversal;
- no graph traversal or dynamic dispatch registry.

Physical callback-margin claims remain blocked until hardware exists.

## 21. Source boundary

Before D055 source-absent acceptance, all production files from the WP5.6 closeout must be byte-identical.

After acceptance, WP5.7 production changes are limited to:

```text
firmware/src/fmt_fm_engine.h
firmware/src/fmt_fm_engine.c
firmware/src/fmt_fm_algorithm.h
firmware/src/fmt_fm_algorithm.c
firmware/src/dinghy.c
```

D049, D051, D052, D053, D054, D043 allocator source and generated tables must remain byte-identical.

The D050 files may change only for the composition seam described above.

## 22. Source-absent reference anchors

D055 reference model freezes:

```text
FmtFmVoice             92 B
FmtVoiceAllocator      52 B
FmtFmEngine           788 B
FmtFmPatch             28 B
```

Default patch SHA-256:

```text
3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

Deterministic 4,096-event allocator stress:

```text
trace bytes 262144
SHA-256 97b87ba4b287c4bdc6cbb41021e3f5d75d9f62897f5aa5d63d5ebdc5927280af

reuse       1896
fresh        565
steal tail   124
active cross-track steals 0
```

The route model must prove cross-family silence/clear/attempt/commit ordering and no route changes for TIE/EMPTY/RETRIG.

## 23. Exit criteria

WP5.7 closes only when all are true:

1. D055 frozen before production integration source;
2. source-absent D055 gate green and durable;
3. 8-slot FM engine implemented within 800 B engine-state target;
4. D043 allocator policy exact, active cross-track steals zero;
5. D050 composition seam backward-compatible;
6. D049-D054 integrated with exact event/reset/release semantics;
7. D048 FM dispatch implemented with exact cross-family ordering;
8. default all-8BIT host audio remains exact;
9. focused FM and mixed-route host fixtures pass;
10. target image/RAM/pool remain within D055 ceilings;
11. `WP5_7_VERIFICATION.md` records canonical evidence before WP5.8 begins.

No physical FM-1 is available. `hardware_margin_claim=none`.
