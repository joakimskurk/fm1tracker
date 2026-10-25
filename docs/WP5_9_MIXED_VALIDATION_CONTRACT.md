# WP5.9 Mixed 8-BIT + FM4 Validation Contract

Date frozen: 2026-10-25  
Issue: #52  
Branch: `phase5/mixed-validation`  
Parent: WP5.8 / D056  
Audit: `WP5_9_PRECONTRACT_AUDIT.md`

## Status

**PREIMPLEMENTATION CONTRACT.**

WP5.9 is a validation package for the architecture already defined by D048-D056. This contract does **not** introduce D057 and does not authorize a new production architecture.

D048-D056 remain normative.

## Objective

Directly prove that the real 8-BIT and FM4 engines can:

1. own different logical tracks at the same time;
2. render nonzero audio during the same sample window;
3. combine through the production top-level mixer exactly and deterministically;
4. switch family on one track without leaking or stealing ownership;
5. preserve NOTE/TIE/EMPTY/RETRIG/OFF/CUT and delayed-NOTE semantics across the real family seam;
6. preserve D056 p-lock family applicability;
7. preserve exact all-8BIT, default/no-lock FM and locked-FM regressions.

## Parent production checkpoint

Accepted D056 code/workflow checkpoint:

```text
849c8b27d8c1afa09f9c093b695985c64e0c6013
```

Accepted D056 target:

```text
image                    79804 B
normal RAM               27792 B
pool                    113000 B
BIN SHA-256              a336fb22050cbf6a7800c98db2c167c83ef5c4e635448c098d8e69ed42869c07
FWSC SHA-256             4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV SHA-256     17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
```

The D056 package ceiling leaves 864 B normal-RAM margin. This is not a physical free-RAM claim.

## C01 - production source freeze

Before any WP5.9 fixture/harness implementation, these trees must be byte-for-byte unchanged from `849c8b27d8c1afa09f9c093b695985c64e0c6013`:

```text
firmware/src/**
firmware/gen/**
tools/**
```

After the contract freeze, the normal WP5.9 validation path continues to keep those trees unchanged.

Allowed validation-only changes:

```text
tests/host.c
tests/wp5_9_*
tests/run_wp5_9_*
.github/workflows/wp5_9_*
docs/**
START_HERE.md
```

A newly failing mixed fixture is **not** authorization to patch firmware. If a real production defect is exposed:

1. preserve the failing evidence;
2. document the exact defect and violated invariant;
3. amend/freeze this contract;
4. only then authorize the narrow production fix.

## C02 - real simultaneous-family fixture

The fixture must use the real production 8-BIT and FM4 engines, not a stub family.

At least two distinct logical tracks must simultaneously own active primaries:

```text
track A -> FMT_ENGINE_FAMILY_FM / FM4
track B -> FMT_ENGINE_FAMILY_8BIT / supported 8-BIT engine
track A != track B
```

During an explicit overlap window:

- the isolated FM render must be nonzero;
- the isolated 8-BIT render must be nonzero;
- both ownership routes must be observable and valid;
- neither family may own the other track's primary;
- active cross-track primary steals must remain zero.

The fixture must render three otherwise-identical variants:

```text
FM_ONLY
BIT8_ONLY
COMBINED
```

For each stereo sample in the validated overlap window:

```text
expected = clamp16(FM_ONLY + BIT8_ONLY)
COMBINED == expected
```

where `clamp16` matches production exactly:

```text
> 32767  ->  32767
< -32767 -> -32767
otherwise unchanged
```

This is the audio oracle for real simultaneous-family coexistence.

`COMBINED` must be rendered at least twice and be byte-identical across repeats.

## C03 - real same-track family switch

A real-engine integration fixture must perform:

```text
FM4 NOTE -> 8-BIT NOTE -> FM4 NOTE
```

on one logical track while at least one unrelated track remains owned/active.

At each switch the test must prove:

1. previous family primary is hard-silenced before replacement commit;
2. global route changes only after new family allocation succeeds;
3. old and new families never simultaneously own the switched track;
4. unrelated route/owner state is unchanged;
5. no active cross-track primary is stolen;
6. failure/malformed replacement leaves the switched route NONE/silent.

The existing WP5.7 audio-window proof is inherited but is not sufficient by itself because it does not inspect real mixed ownership state.

## C04 - real event semantics

Using real engines, cover the current D048/D055/D056 semantics:

### TIE / EMPTY

- route family does not change;
- no new primary is allocated;
- current family receives the new row parameter snapshot/update where applicable;
- FM uses the newly snapshotted D056 patch without phase/envelope reset.

### RETRIG

- dispatches only to the currently routed family;
- does not change family;
- FM uses the current D056 row patch;
- 8-BIT uses its current linked base/page state.

### OFF / CUT

- release/cut applies to the currently routed family only;
- global primary route clears in the same event;
- any detached family-local release tail remains bounded;
- unrelated tracks remain unchanged.

### delayed NOTE

- old family remains active until the delayed tick;
- delayed NOTE goes through the normal `tracker_note_route()` replacement path;
- successful cross-family replacement follows C03 ordering;
- failure remains fail-to-silence.

## C05 - p-lock family safety in mixed operation

The integrated mixed fixtures must preserve semantic-family applicability:

- FM parameter IDs alter FM rows only;
- FM IDs do not alias into 8-BIT controls;
- 8-BIT IDs do not alias into FM controls;
- maximum four locks per step remains unchanged;
- malformed >255 FM values are not silently truncated;
- sparse lock storage remains tracker data even when a lock is non-applicable to the current family.

At least one simultaneous/mixed fixture must contain a real D056 FM p-lock and demonstrate deterministic changed FM audio without altering the unrelated 8-BIT route/owner state.

## C06 - host observability

WP5.9 may extend `tests/host.c` only for host-side mixed validation.

Permitted additions include:

- public extern observation of `FmtFmEngine`;
- public extern observation of `FmtEngineRouter`;
- FM linked-cache refresh alongside existing 8-BIT fixture refresh;
- mixed route/voice information in host `probe` output;
- fixture-only identity/base helpers if required.

The host harness must not add a target-firmware runtime object, target code path or persisted data field.

Existing D046/D048/D055/D056 host tests must remain green after any harness extension.

## C07 - deterministic regressions

WP5.9 host closeout must fail unless all of the following remain exact:

- WP5.0 all-8BIT production WAV SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`;
- inherited D049-D055 engine behavior;
- D056 default/no-lock FM composition;
- D056 locked-FM row-snapshot behavior;
- D056 max-four-lock and family-filter semantics;
- repeated WP5.9 mixed renders byte-identical.

No `| tee` pipeline may be used as a fail-closed test boundary unless pipeline failure propagation is explicitly enabled. Prefer command -> file -> `cat`/`grep` for critical gates.

## C08 - realtime boundary

No validation change may introduce target sample-path work.

Inherited target rules remain:

- no heap;
- no float;
- no runtime int64/division dependency in DSP;
- no sparse lock-store traversal per sample;
- no editor/UI traversal per sample;
- both family renderers called once per frame;
- bounded static family-local state.

## C09 - target identity gate

If WP5.9 closes without a production fix, the pinned target output must remain **exactly D056**, not merely remain under a ceiling:

```text
image                    = 79804 B
normal RAM               = 27792 B
pool                     = 113000 B
BIN SHA-256              = a336fb22050cbf6a7800c98db2c167c83ef5c4e635448c098d8e69ed42869c07
FWSC SHA-256             = 4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
```

A difference means either the frozen production boundary moved or the build is no longer identical. It must be explained before closeout.

If an amended contract later authorizes a production bug fix, C09 must be explicitly replaced with new measured ceilings/deltas before that fix is merged.

## C10 - physical boundary

No physical FM-1 is available.

Host/emulator and pinned-toolchain evidence may establish deterministic behavior, source/resource bounds and package identity. It may not establish real-device audio quality, install behavior, thermal behavior or measured callback headroom.

```text
hardware_margin_claim=none
```

## Non-goals

WP5.9 does not implement:

- virtual-analog/subtractive synthesis;
- drums/percussion;
- any new sound engine;
- song/order/persistence;
- final worst-case mixed-engine benchmarking;
- physical hardware UX or timing claims.

Future VA/subtractive and drum engines remain explicit FM1Tracker product directions in `PROJECT_CONTEXT.md`.

## Preimplementation exit gate

Fixture/harness implementation is authorized only after CI proves:

```text
WP5_9_CONTRACT_FREEZE_OK=1 production_delta=0 real_simultaneous=required same_track_switch=required event_semantics=required plock_family_safe=required target_identity=d056_exact hardware_margin_claim=none
```

The freeze checkpoint is evidence that the validation target was defined before the validation harness was changed.
