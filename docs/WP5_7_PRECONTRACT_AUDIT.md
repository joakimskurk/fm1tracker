# WP5.7 FM4 Voice/Allocator/Router Precontract Audit

Status: **AUDIT COMPLETE - D055 contract candidate ready**
Date: 2026-10-25
Package: WP5.7 / issue #50
Branch: `phase5/fm4-voice-integration`
Parent closeout: WP5.6 / D054 `886cb496f4e73a91c4ded16bf9e722b770a040f6`

This audit measures the existing D043/D045/D048 runtime architecture against the completed D049-D054 FM4 primitives before any FM voice or router integration source is changed.

## 1. Existing 8-BIT allocator baseline

D043 already provides the ownership policy required by D048:

- 8 logical tracks;
- 8 allocator slots;
- one primary slot at most per track;
- a track reuses its own primary;
- otherwise the lowest idle slot is used;
- otherwise the oldest detached RELEASE tail is stolen, with lowest slot index as tie-break;
- an active primary owned by another track is never stolen;
- OFF/CUT detaches ownership while release continues;
- hard silence clears the primary immediately.

Exact existing sizes:

```text
FmtVoiceAllocator = 52 B
Fmt8BitVoice      = 44 B
Fmt8BitEngine     = 404 B
```

No D043 semantic change is required for FM4.

## 2. Existing D048 common router baseline

The common router owns exactly one family byte per logical track:

```text
FmtEngineRouter = 8 B
```

D048 cross-family NOTE ordering is already implemented in `dinghy.c`:

```text
hard-silence old family
clear route
attempt NOTE in requested family
commit new family only on success
```

TIE/EMPTY/RETRIG remain current-family-only. OFF/CUT releases the current family and clears the global route in the same event.

The FM family case is currently an intentional safe-silence stub. WP5.7 replaces only that stub with a bounded family-local engine adapter.

## 3. Completed FM4 primitive state

Persistent per-voice synthesis state already frozen before WP5.7:

```text
D050 four operator phases      16 B
D053 OP4 feedback history       2 B
D054 four ADSR envelopes       20 B
------------------------------------
FM synthesis state subtotal    38 B
```

D051 ratio mapping and D052 control mapping are stateless.

D054 instrument envelope controls are exactly 16 B.

## 4. Candidate FM voice layout

WP5.7 needs row-local snapshots so audio never traverses mutable song/editor state.

Candidate exact voice layout:

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

Eight slots plus the existing allocator shape:

```text
52 + 8 * 92 = 788 B
```

This keeps all eight logical tracks without active cross-track stealing.

## 5. Voice count alternatives

### Four FM slots

Rejected.

Four slots would force either active cross-track stealing or silent inability to give all eight tracker tracks an FM primary. That contradicts the project rule that eight logical tracks may not be silently reduced.

### Shared 8-BIT/FM slot pool

Rejected for WP5.7.

D048 explicitly keeps family-local allocators/voice handles. A shared pool would couple unrelated engine state, complicate release-tail ownership and make cross-family NOTE switching less local.

### Eight family-local FM slots

Accepted candidate.

This mirrors D043 exactly and still leaves normal-RAM headroom.

## 6. FM parameter storage before WP5.8

WP5.8 owns tracker-facing FM pages, p-lock IDs and the eventual per-instrument FM base bank.

WP5.7 therefore must not pre-freeze a 16-instrument mutable FM parameter store.

Instead WP5.7 uses one immutable 28 B default patch in ROM as a bridge:

```text
ratio_id[4]             4 B
level_control[4]        4 B
ADSR controls          16 B
algorithm               1 B
index_control           1 B
feedback_control        1 B
reserved                1 B
---------------------------
total                   28 B
```

Frozen bridge patch bytes:

```text
ratios   [1, 5, 1, 5]   -> 1x, 2x, 1x, 2x
levels   [255, 96, 255, 96]
attack   [0, 0, 0, 0]
decay    [0, 0, 0, 0]
sustain  [255, 255, 255, 255]
release  [96, 96, 96, 96]
algorithm DUAL2
INDEX     64
FEEDBACK   0
reserved   0
```

Serialized SHA-256:

```text
3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

The bridge patch is not a persistence/UI contract. WP5.8 may replace its storage source while preserving the engine patch input shape.

## 7. D050/D053 composition seam

D053 must render OP4 with one-sample feedback, but the accepted D050 renderer currently renders OP4 internally.

Duplicating D050 topology logic in the FM engine is rejected.

WP5.7 may add one narrow backward-compatible D050 composition seam:

```text
OP4 already rendered exactly once
        |
        v
D050 renders OP3/OP2/OP1 only
and performs the same carrier normalization
```

Requirements:

- the existing `fmt_fm_algorithm_render()` public behavior remains bit-exact;
- the new seam never advances OP4;
- the FM engine renders OP4 through D053, then calls the seam;
- with FEEDBACK=0, integrated output must match the old D050 path exactly for equivalent state/inputs.

No other D049-D054 semantic change is justified.

## 8. Allocator stage projection

D043 accepts one stage byte per slot, while D054 has four envelope stages.

The FM engine can project four D054 stages to one allocator stage without losing ownership semantics:

```text
all four OFF
    -> IDLE

every operator is OFF or RELEASE
and at least one is RELEASE
    -> RELEASE

otherwise
    -> active owned stage (SUSTAIN is sufficient)
```

Thus detached non-idle FM slots are always represented as RELEASE, exactly what D043 validates.

## 9. Event semantics candidate

NOTE:

- acquire/reuse through D043;
- snapshot patch, pitch, gain and controls;
- map four D051 phase increments;
- hard retrigger D050 phases;
- clear D053 history;
- hard retrigger D054 envelopes from zero;
- return slot only after state is valid.

TIE/EMPTY row update:

- no allocation;
- preserve D050 phase, D053 feedback history and D054 envelope state;
- update pitch/gain and patch-derived row snapshot.

RETRIG:

- no allocation;
- hard retrigger D050, D053 and D054 on the current FM primary.

OFF/CUT:

- call D054 note-off;
- detach D043 ownership in the same event;
- zero-release voices may become IDLE immediately;
- non-zero release continues as detached tail.

Hard silence:

- reset D050/D053/D054 state for the current primary;
- detach ownership immediately;
- clear voice snapshot.

Algorithm changes during a live voice preserve D053 history and D054 envelope state, as already frozen by D053/D054.

## 10. Render ordering candidate

Per active FM slot, one sample performs:

```text
read current D054 envelope levels
map static D052 levels through D054
D053 renders OP4 once
D050 composition seam renders OP3/OP2/OP1
apply row gain to final FM voice output
advance four D054 envelopes once
```

The engine then sums exactly eight fixed slots and clamps mono Q15.

This preserves a zero first sample for non-zero ATTACK and keeps the first RELEASE sample continuous with the pre-release level.

## 11. Resource candidate

Accepted WP5.6 running application baseline:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
```

Candidate WP5.7 ceilings:

```text
FM engine ordinary RAM      exactly 788 B target
WP5.7 RAM increment ceiling        1024 B
WP5.7 total RAM ceiling           20700 B

WP5.7 image increment ceiling      8192 B
WP5.7 total image ceiling         80984 B

pool increment                        0 B
pool total                       113000 B
```

The RAM ceiling leaves 788 B below the historical D037 normal-RAM ceiling of 21,488 B.

No mutable FM instrument bank is allowed in WP5.7.

## 12. Audit conclusion

D055 should freeze:

- eight family-local FM slots;
- direct reuse of D043 allocator policy;
- exact 92 B FM voice / 788 B FM engine targets;
- the immutable 28 B bridge patch;
- one narrow backward-compatible D050 OP4 composition seam;
- D048 event ordering unchanged;
- exact all-8BIT regression equivalence;
- no FM page cache or mutable parameter bank until WP5.8;
- the resource ceilings above.

No production integration source should change before the D055 source-absent gate passes.
