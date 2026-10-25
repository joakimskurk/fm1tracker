# WP5.8 / D056 - FM4 parameter pages and p-lock mapping contract

Date frozen: 2026-10-25  
Issue: #51  
Branch: `phase5/fm4-parameters`  
Parent closeout: `80587f12786122dcec819260ddad5dd5db4b1005`

Status: **FROZEN BEFORE PRODUCTION IMPLEMENTATION**

## 1. Decision

D056 exposes the D049-D055 FM4 engine to the existing semantic p-lock/editor architecture using:

- the already-reserved 16BIT semantic-ID namespace;
- one normalized 27-byte FM parameter set per instrument;
- seven stable four-knob logical FM pages;
- the existing maximum four sparse locks per step;
- one generalized physical engine-row publication allocation that preserves the historical 8BIT 2,048-byte view;
- row-boundary patch composition only, never mutable store/editor reads per sample.

D056 does not change FM synthesis math, envelope semantics, allocator ownership or D048 family-switch ordering.

## 2. Stable FM semantic IDs

The following IDs are permanently assigned in the existing `0x60..0x9F` 16BIT namespace:

| Page | Knob | ID | Symbol | Normalized domain |
|---|---:|---:|---|---:|
| CORE | 1 | `0x60` | `FM_ALGORITHM` | 0..255 |
| CORE | 2 | `0x61` | `FM_INDEX` | 0..255 |
| CORE | 3 | `0x62` | `FM_FEEDBACK` | 0..255 |
| CORE | 4 | - | `NONE` | - |
| RATIO | 1 | `0x64` | `FM_OP1_RATIO` | 0..255 |
| RATIO | 2 | `0x65` | `FM_OP2_RATIO` | 0..255 |
| RATIO | 3 | `0x66` | `FM_OP3_RATIO` | 0..255 |
| RATIO | 4 | `0x67` | `FM_OP4_RATIO` | 0..255 |
| LEVEL | 1 | `0x68` | `FM_OP1_LEVEL` | 0..255 |
| LEVEL | 2 | `0x69` | `FM_OP2_LEVEL` | 0..255 |
| LEVEL | 3 | `0x6A` | `FM_OP3_LEVEL` | 0..255 |
| LEVEL | 4 | `0x6B` | `FM_OP4_LEVEL` | 0..255 |
| ATTACK | 1 | `0x6C` | `FM_OP1_ATTACK` | 0..255 |
| ATTACK | 2 | `0x6D` | `FM_OP2_ATTACK` | 0..255 |
| ATTACK | 3 | `0x6E` | `FM_OP3_ATTACK` | 0..255 |
| ATTACK | 4 | `0x6F` | `FM_OP4_ATTACK` | 0..255 |
| DECAY | 1 | `0x70` | `FM_OP1_DECAY` | 0..255 |
| DECAY | 2 | `0x71` | `FM_OP2_DECAY` | 0..255 |
| DECAY | 3 | `0x72` | `FM_OP3_DECAY` | 0..255 |
| DECAY | 4 | `0x73` | `FM_OP4_DECAY` | 0..255 |
| SUSTAIN | 1 | `0x74` | `FM_OP1_SUSTAIN` | 0..255 |
| SUSTAIN | 2 | `0x75` | `FM_OP2_SUSTAIN` | 0..255 |
| SUSTAIN | 3 | `0x76` | `FM_OP3_SUSTAIN` | 0..255 |
| SUSTAIN | 4 | `0x77` | `FM_OP4_SUSTAIN` | 0..255 |
| RELEASE | 1 | `0x78` | `FM_OP1_RELEASE` | 0..255 |
| RELEASE | 2 | `0x79` | `FM_OP2_RELEASE` | 0..255 |
| RELEASE | 3 | `0x7A` | `FM_OP3_RELEASE` | 0..255 |
| RELEASE | 4 | `0x7B` | `FM_OP4_RELEASE` | 0..255 |

`0x63` is deliberately unassigned. `0x7C..0x9F` remains available for later 16BIT-family controls. IDs must never be renumbered to make a future UI prettier.

The assigned registry count becomes 37 when D056 production activation occurs: 10 inherited + 27 FM.

## 3. Logical pages

Stable page enum/order:

```text
0 CORE
1 RATIO
2 LEVEL
3 ATTACK
4 DECAY
5 SUSTAIN
6 RELEASE
```

There are exactly seven pages and four slots per page. `CORE[3]` returns `FMT_PARAM_NONE` and accepts no edit.

Page selection is main/UI-only state. D056 freezes semantic page composition, not the final Phase 7 physical gesture. A provisional emulator/hardware gesture is allowed if it is bounded, does not alter transport timing, and leaves all 8BIT controls unchanged.

## 4. Normalized `FmtFmParamSet`

Exact logical content is 27 bytes:

```c
uint8_t algorithm;
uint8_t index;
uint8_t feedback;
uint8_t ratio[4];
uint8_t level[4];
uint8_t attack[4];
uint8_t decay[4];
uint8_t sustain[4];
uint8_t release[4];
```

`sizeof(FmtFmParamSet)` must be exactly 27 B. Sixteen instrument bases therefore consume exactly 432 B.

Every semantic p-lock value is normalized 0..255. Raw stored values greater than 255 are invalid for FM resolution and must be ignored/fail-safe; truncation is forbidden.

## 5. Exact D055-compatible defaults

Normalized defaults:

```text
algorithm  128
index       64
feedback     0
ratio       16,80,16,80
level       255,96,255,96
attack      0,0,0,0
decay       0,0,0,0
sustain     255,255,255,255
release     96,96,96,96
```

Bounded conversion to D055:

```text
patch.algorithm        = params.algorithm >> 6
patch.index_control    = params.index
patch.feedback_control = params.feedback
patch.ratio_id[i]      = params.ratio[i] >> 4
patch.level_control[i] = params.level[i]
patch.envelope.*[i]    = corresponding normalized byte
patch.reserved         = 0
```

The resulting 28 patch bytes must hash to:

```text
3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

This is the exact WP5.7/D055 immutable bridge patch. A song with default FM bases and no FM locks must therefore enter D055 with exactly the old patch bytes.

## 6. Sparse p-lock semantics

D028 remains authoritative:

- maximum four locks per step;
- no additional FM lock pool;
- no page-specific duplicate storage;
- semantic IDs are stored, not knob/page ordinals;
- changing the selected page never changes existing lock identity;
- a step may contain locks from different FM pages, up to the same four-lock total.

For a resolved FM row, main context publishes at most four valid FM overrides. Common/track/8BIT/wrong-family IDs are not copied into the FM engine payload. Malformed store state or a value >255 publishes no unsafe FM override.

Wrong-family semantic IDs remain in the sparse store as user data but have no effect on the current engine family. They become meaningful again only if the row resolves to their owning family.

## 7. Single physical engine-row cache

D056 generalizes the D045 allocation while preserving its first 2,048 B source-level 8BIT view.

Exact target layout size:

```text
legacy/bank0-low   2048 B
bank0-high         2048 B
bank1-low          2048 B
bank1-high         2048 B
active-bank         512 B
-------------------------
total              8704 B
```

The old `Fmt8BitRowParamCache` type remains exactly 2,048 B. Existing source using `fmt_8bit_row_param_cache.word[track][row]` must continue to compile and observe the same first-word semantics. Implementation may expose that view through a source-compatible macro/member alias into the generalized allocation; it may not allocate a second legacy mirror.

8BIT rows:

- use the existing aligned 32-bit word exactly;
- ignore FM bank selector/high words;
- existing WAV hashes and page behavior must remain bit-identical.

FM rows:

- two 32-bit words encode four fixed two-byte override slots;
- each slot is `{FmtParamId id, uint8_t value}`;
- unused slots are `{FMT_PARAM_NONE,0}`;
- slot ordering is deterministic ascending semantic ID;
- payload is double buffered per track/row;
- main writes inactive low/high words before flipping the one-byte active bank;
- audio reads active bank once at row boundary;
- audio never retries/spins and never traverses the lock store.

The generalized allocation is ordinary RAM and not editable-song pool RAM.

## 8. Row/instrument semantics

FM linked parameter resolution uses the same tracker inheritance contract as D045/D048:

- track instrument is the starting instrument;
- only a normal NOTE may update inherited instrument from a step;
- TIE/EMPTY/OFF/CUT do not change inherited instrument identity;
- malformed explicit instrument on a normal NOTE makes that row fail safe;
- FM parameter publication is valid only when the resolved instrument family is `FMT_ENGINE_FAMILY_FM` and its engine identity is supported;
- other families publish no FM overrides for that row.

D055 event semantics remain authoritative after patch composition:

- NOTE snapshots the composed patch and hard-retriggers;
- TIE/EMPTY may update the current patch snapshot without resetting phase/envelope stage;
- RETRIG resets according to D055;
- OFF/CUT use the row-resolved release controls before detaching/releasing.

## 9. Editor mapping

Held-step p-lock editing reuses `fmt_lock_editor_apply_param_delta()`.

FM knob mapping is page-aware. Reference value comes from the resolved instrument's normalized base byte when that semantic ID is not already locked. Generic editor clamp remains 0..255.

After a successful FM lock mutation or delete, main refreshes:

1. the existing common playback row snapshot;
2. the generalized engine-row publication for that track/row.

No full 8x64 rebuild is required for a one-row lock mutation.

FM base setters are main-only. Changing one instrument base may rebuild bounded engine-row publications. WP5.8 does not require a final physical instrument-editor UX; it must provide and test bounded mutable per-instrument base storage/API.

## 10. Realtime boundary

Forbidden inside per-sample FM render and row-event audio helpers:

- `fmt_lock_*` traversal;
- `fmt_store_*` reads;
- mutable editor reads;
- heap allocation;
- floating point;
- 64-bit arithmetic;
- runtime division/modulo;
- unbounded loops;
- spin/retry publication.

FM patch composition from one 27-byte base + at most four already-published overrides is allowed only at the bounded row-event boundary before the existing D055 call.

D049-D055 synthesis/voice source remains frozen unless a separately documented blocker proves the contract impossible.

## 11. Production source boundary

After the source-absent gate is green, WP5.8 may change only:

```text
firmware/src/fmt_param_registry.h
firmware/src/fmt_param_registry.c
firmware/src/fmt_8bit_linked_params.h
firmware/src/fmt_8bit_linked_params.c
firmware/src/fmt_lock_editor.c
firmware/src/dinghy.c
```

and may add only:

```text
firmware/src/fmt_fm_params.h
firmware/src/fmt_fm_params.c
firmware/src/fmt_fm_linked_params.h
firmware/src/fmt_fm_linked_params.c
```

Any additional production file requires a contract amendment before the code change.

Frozen production files include:

```text
fmt_fm_operator.*
fmt_fm_algorithm.*
fmt_fm_ratio.*
fmt_fm_controls.*
fmt_fm_feedback.*
fmt_fm_envelope.*
fmt_fm_engine.*
fmt_voice_allocator.*
fmt_engine_router.*
firmware/gen/*
```

## 12. Resource ceilings

Relative to D055 closeout:

```text
D055 image       77048 B
D055 normal RAM  20464 B
D055 pool       113000 B
```

WP5.8 ceilings:

```text
image increment       <= 16384 B
normal RAM increment  <=  8192 B
pool                   = 113000 B exactly
```

Therefore absolute ceilings are:

```text
image       <= 93432 B
normal RAM  <= 28656 B
pool         = 113000 B
```

Known mandatory normal-RAM growth is 7,088 B: +6,656 B generalized-cache net +432 B FM base bank. The remaining 1,104 B is the complete WP5.8 margin for page/runtime bookkeeping.

These are package ceilings, not claims of physical CPU timing margin.

## 13. Required gates

Before production implementation:

- zero firmware/gen diff from `80587f12786122dcec819260ddad5dd5db4b1005`;
- D056 reference model green;
- 16BIT namespace still source-absent/unassigned;
- D055 focused engine/integration regressions green.

Implementation verification must then prove:

- all 27 IDs exact and assigned;
- seven pages exact;
- `FmtFmParamSet == 27 B`, bank == 432 B;
- exact default patch SHA;
- four-lock step limit unchanged;
- wrong-family/invalid/value>255 safety;
- deterministic double-buffer publication under preemption model;
- exact old 8BIT 2,048-byte view and bit-identical all-8BIT WAV;
- focused FM p-lock timing for NOTE/TIE/EMPTY/RETRIG/OFF/CUT;
- D049-D055 inherited tests;
- target footprint within frozen ceilings;
- no per-sample store/editor/cache traversal beyond fixed row snapshots.

## 14. Out of scope

- more than four p-locks per step;
- new modulation matrix/LFOs;
- final physical knob/button UX;
- broad simultaneous-family stress campaign, owned by WP5.9;
- Phase 5 worst-case benchmark/exit, owned by WP5.10;
- physical timing/audio-quality claims.

`hardware_margin_claim=none`.
