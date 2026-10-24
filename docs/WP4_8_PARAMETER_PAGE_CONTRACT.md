# WP4.8 8-BIT Parameter Page + P-Lock Contract

Date: 2026-10-24  
Package: WP4.8 / issue #39  
Status: **D044 contract candidate — must be recorded in DECISIONS before production parameter-page source**

Parent architecture: D027, D031, D037-D043  
Branch: `phase4/8bit-engine`

## 1. Purpose

WP4.8 freezes the bounded semantic parameter/page layer for the Phase 4 `8-BIT` family without integrating the engine into `dinghy.c` or replacing the Phase 3 proof sine.

D044 defines:

- stable Phase 4 semantic parameter IDs;
- exactly four exposed page controls for each current 8-BIT engine;
- normalized `0..255` base/default values;
- exact conversion into the already-frozen D038-D042 controls;
- row-local p-lock precedence;
- one aligned 32-bit four-byte publication word per `(track,row)`;
- future editor/base-value storage shape and cost;
- deterministic malformed/unsupported behavior.

WP4.8 does **not** activate these IDs in the already-linked global D027 registry or current step-hold knob UI. Doing that would intentionally change the running full app and therefore belongs to WP4.9's explicit integration/migration checkpoint. D044 freezes the numeric identities now so WP4.9 cannot invent or renumber them later.

## 2. Stable D044 semantic IDs

D027 reserves `0x20..0x5F` for the Phase 4 8-BIT namespace. D044 assigns exactly these first six IDs:

```text
0x20  FMT_PARAM_8BIT_ATTACK       ATTACK
0x21  FMT_PARAM_8BIT_DECAY        DECAY
0x22  FMT_PARAM_8BIT_SUSTAIN      SUSTAIN
0x23  FMT_PARAM_8BIT_RELEASE      RELEASE
0x24  FMT_PARAM_8BIT_PULSE_WIDTH  WIDTH
0x25  FMT_PARAM_8BIT_WAVETABLE    TABLE
```

IDs `0x26..0x5F` remain unassigned.

All six D044 IDs are semantically lockable one-byte normalized parameters. Their numeric identities are permanent unless a later explicit migration decision supersedes D044.

The current D027 common/track IDs remain unchanged and are not duplicated:

```text
0x01 VOICE LEVEL
0x02 FINE PITCH
0xA0 TRACK LEVEL
0xA1 TRACK PAN
```

D031 continues to own VOICE LEVEL / FINE PITCH / TRACK LEVEL row publication. TRACK PAN remains storage-only at this checkpoint.

## 3. Why registry activation is deferred to WP4.9

`FMT_PARAM_REGISTRY` is already linked into the production Phase 3 application through the lock editor/store. Expanding that linked registry in WP4.8 would change full-app image/hash even though the engine remains isolated.

D044 therefore separates two events deliberately:

1. **WP4.8:** freeze IDs and implement/test the isolated page/resolution primitive;
2. **WP4.9:** add those already-frozen IDs to the linked D027 registry and connect the page cache/UI/audio adapter as part of the deliberate proof-sine migration.

WP4.9 may not renumber, rename or reinterpret the six D044 IDs while activating them.

## 4. Semantic normalized base set

The complete 8-BIT semantic base set is exactly six bytes:

```c
typedef struct {
    uint8_t attack;
    uint8_t decay;
    uint8_t sustain;
    uint8_t release;
    uint8_t pulse_width;
    uint8_t wavetable;
} Fmt8BitParamSet;
```

Required size:

```text
sizeof(Fmt8BitParamSet) == 6 B
```

All fields use stored normalized `0..255` bytes.

Frozen defaults:

```text
ATTACK      24
DECAY        0
SUSTAIN    255
RELEASE     72
WIDTH      128
TABLE        0
```

The envelope defaults are exactly D042's neutral A24/D0/S255/R72. WIDTH 128 maps exactly to D038 pulse width 128. TABLE 0 maps to D041 SINE.

A future main/editor-owned bank for all 16 instruments is therefore exactly:

```text
16 * 6 B = 96 B ordinary RAM
```

D044 defines the type/size only. WP4.8 does not allocate/link a global 96 B bank into the running full app.

Raw `Fmt8BitParamSet` memory is runtime/editor state only and is not a Phase 6 persistence format.

## 5. Exactly four exposed controls per engine

D037 engine IDs remain authoritative:

```text
1 PULSE
2 NOISE
3 TRIANGLE
4 WAVETABLE-8
```

Each current engine exposes exactly four ordered page slots.

### PULSE

```text
slot 0  WIDTH    0x24
slot 1  DECAY    0x21
slot 2  SUSTAIN  0x22
slot 3  RELEASE  0x23
```

PULSE uses fixed D042 ATTACK = 24 in Phase 4. The first hardware/page slot is reserved for D038 pulse width because it is the oscillator's defining editable control.

### NOISE

```text
slot 0  ATTACK   0x20
slot 1  DECAY    0x21
slot 2  SUSTAIN  0x22
slot 3  RELEASE  0x23
```

D039 has no additional mutable oscillator parameter under its frozen contract, so NOISE exposes the full D042 ADSR.

### TRIANGLE

```text
slot 0  ATTACK   0x20
slot 1  DECAY    0x21
slot 2  SUSTAIN  0x22
slot 3  RELEASE  0x23
```

D040 has no additional mutable oscillator parameter under its frozen contract, so TRIANGLE exposes the full D042 ADSR.

### WAVETABLE-8

```text
slot 0  TABLE    0x25
slot 1  DECAY    0x21
slot 2  SUSTAIN  0x22
slot 3  RELEASE  0x23
```

WAVETABLE-8 uses fixed D042 ATTACK = 24 in Phase 4. The first hardware/page slot is reserved for D041 table selection.

This is intentionally asymmetric. D044 does not invent a fake oscillator parameter for NOISE/TRIANGLE, and it does not hide WIDTH/TABLE behind a second page. The two engines with an oscillator-defining control trade editable ATTACK for that control while retaining DECAY/SUSTAIN/RELEASE.

## 6. Frozen default page words

A published page word stores four normalized bytes in little-endian slot order:

```text
bits  7..0   slot 0
bits 15..8   slot 1
bits 23..16  slot 2
bits 31..24  slot 3
```

Frozen default words:

```text
PULSE       [128, 0, 255, 72] -> 0x48FF0080
NOISE       [ 24, 0, 255, 72] -> 0x48FF0018
TRIANGLE    [ 24, 0, 255, 72] -> 0x48FF0018
WAVETABLE-8 [  0, 0, 255, 72] -> 0x48FF0000
```

Unsupported/SAFE engine IDs do not have a parameter page and resolve to no-page/failure rather than aliasing another engine.

## 7. Exact normalized-to-engine mappings

### 7.1 Envelope A/D/S/R

D044 ATTACK/DECAY/SUSTAIN/RELEASE normalized bytes map **directly** to the corresponding D042 `FmtEnvelopeParams` byte.

No curve, remap or second time domain is introduced.

For PULSE and WAVETABLE-8, ATTACK is fixed to D042 default 24; ATTACK p-locks are not applicable to those engine pages and are ignored by the D044 row resolver.

### 7.2 PULSE WIDTH

Stored normalized byte `v` maps to D038's exact legal width `32..224` by:

```text
pulse_width = 32 + ((v * 193) >> 8)
```

This mapping is integer-only, monotonic and has exact canonical points:

```text
v=0   -> 32
v=128 -> 128
v=255 -> 224
```

The result is already inside D038's legal range and requires no runtime division.

### 7.3 WAVETABLE TABLE

Stored normalized byte `v` maps to the D041 table selector by:

```text
table = v >> 5
```

Therefore:

```text
0..31    -> SINE / 0
32..63   -> SAW8 / 1
64..95   -> SQUARE8 / 2
96..127  -> ORGAN / 3
128..159 -> HOLLOW / 4
160..191 -> NASAL / 5
192..223 -> METAL / 6
224..255 -> DIGI / 7
```

No modulo or malformed selector path is introduced.

## 8. Page metadata API boundary

The isolated WP4.8 primitive may expose fixed helpers equivalent to:

```text
page_param(engine, slot) -> D044 ID / NONE
page_default_word(engine) -> packed 32-bit word / failure
page_value_from_set(engine, slot, base_set) -> normalized byte
```

Page lookup is fixed switch/table work over four known engine IDs and four slots. There is no dynamic registration.

Malformed engine or slot returns `NONE`/failure and never aliases another engine/slot.

## 9. P-lock precedence and applicability

D037 precedence is frozen exactly:

```text
resolved row slot = semantic instrument/base value
                  overridden by an applicable D028 row p-lock
```

A D044 p-lock is applicable only when its semantic ID appears in the current engine's four-slot page.

Examples:

- `WIDTH` applies to PULSE only;
- `TABLE` applies to WAVETABLE-8 only;
- `ATTACK` applies to NOISE/TRIANGLE only in Phase 4;
- `DECAY`, `SUSTAIN`, `RELEASE` apply to all four engines.

A valid assigned/common lock that is not part of the current D044 page is ignored by the D044 resolver because it belongs to another publication layer or engine context.

D031 common locks remain handled by the existing D031 cache and are never copied into the D044 page word.

## 10. Isolated resolver input shape

WP4.8 must not traverse D028 linked storage from audio and does not need to link the current global store into the new primitive.

The isolated resolver accepts a bounded snapshot equivalent to:

```text
engine
Fmt8BitParamSet base
lock_count 0..4
up to four (FmtParamId, uint16_t value) pairs
```

This mirrors D028's already-frozen maximum four locks per step without duplicating its storage allocator.

The resolver performs at most four lock inspections.

WP4.9 main/editor integration will obtain those pairs through the existing validated `fmt_lock_count()` / `fmt_lock_at()` API and publish the resolved word. Audio will never call D028 traversal.

## 11. Malformed lock behavior

Exact D044 behavior:

- `lock_count > 4` -> structural fallback to the untouched base page word;
- null required input -> failure/no mutation;
- applicable value `0..255` -> exact row override;
- applicable value `>255` -> ignore that lock, retain its base value, report DOMAIN_FALLBACK to the caller;
- non-applicable/common/other-engine ID -> ignore for this page;
- duplicate applicable D044 ID within the bounded input -> structural fallback to the untouched base page word;
- unsupported engine -> ENGINE_FALLBACK / no valid page word.

The publication word itself remains exactly four values and contains no status flags. Status is returned separately to main/test code. Audio consumes values only.

## 12. Publication representation

D037's maximum publication shape is now frozen exactly:

```c
typedef struct {
    uint32_t word[8][64];
} Fmt8BitRowParamCache;
```

Required size:

```text
8 * 64 * 4 B = 2048 B ordinary RAM
```

Each word is naturally 4-byte aligned and contains exactly the four normalized page values for the resolved engine context of that track/row.

D044 defines the type/shape and pack/unpack behavior. WP4.8 does **not** allocate/link a global 2048 B cache into the running full app. WP4.9 owns actual cache allocation/publication and must measure the resulting RAM delta.

Main/editor publishes; audio reads one complete aligned word at deterministic row dispatch. Audio never traverses D028 records.

## 13. Instrument inheritance boundary

WP4.8 does not create a second instrument resolver.

The D044 pure resolver receives an already-selected engine ID and base `Fmt8BitParamSet`. WP4.9 integration is responsible for applying the existing tracker/instrument inheritance rules before invoking D044 resolution.

Unsupported mode/engine still fails toward D037 SAFE/NONE silence.

## 14. Current knob UI boundary

The existing D029/D031 step-hold knob mapping remains unchanged in WP4.8:

```text
KNOB1 VOICE LEVEL
KNOB2 FINE PITCH
KNOB3 TRACK LEVEL
KNOB4 TRACK PAN
```

This is deliberate: changing that linked UI before the engine migration would alter current production behavior and break the isolated-zero-delta checkpoint.

D044 freezes page slot order now so WP4.9/Phase 7 can bind a deliberate engine-page UI context without inventing new semantic IDs.

## 15. Exact isolated resource budget

WP4.8 type-level future costs:

```text
Fmt8BitParamSet                 6 B per instrument
16-instrument base bank        96 B ordinary RAM when integrated
Fmt8BitRowParamCache         2048 B ordinary RAM when integrated
const/runtime page metadata     implementation-dependent small image data
pool RAM                         0 B
```

WP4.8 itself keeps these unlinked/unallocated from the complete app.

Therefore the complete host/device application must remain the exact WP4.7 baseline:

```text
image       66,960 B
normal RAM  13,296 B
pool RAM    113,000 B
host WAV    453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440
BIN         b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27
FWSC        7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c
identity    FM-1_987
```

The future integrated 96 B base bank + 2048 B row cache remain within D037's +8192 B Phase 4 normal-RAM ceiling. WP4.9 must measure the actual integrated delta rather than assuming it.

## 16. Realtime/source bounds

The isolated resolver must use:

- fixed switch/table lookup;
- at most four lock inspections;
- integer byte/shift/multiply operations only.

Forbidden:

- heap/pool allocation;
- float/double;
- runtime 64-bit integer arithmetic;
- blocking/locks;
- tracker-store traversal inside the resolver;
- pattern/64-row scan inside one row resolve;
- recursion;
- dynamic registration/function-pointer dispatch;
- audio-side D028 traversal.

## 17. Focused verification contract

Before WP4.8 may close, automated tests must prove at minimum:

1. six exact D044 numeric IDs and reserved-range membership;
2. `Fmt8BitParamSet == 6 B`;
3. `Fmt8BitRowParamCache == 2048 B`;
4. exact four-slot page order for PULSE/NOISE/TRIANGLE/WAVETABLE-8;
5. unsupported engine/slot -> no-page/NONE;
6. exact default set A24/D0/S255/R72/W128/T0;
7. exact default words `48ff0080`, `48ff0018`, `48ff0018`, `48ff0000`;
8. pack/unpack round trip for all byte positions;
9. all 256 WIDTH bytes map monotonically inside 32..224 with canonical 0/128/255 points;
10. all 256 TABLE bytes map exactly to `value >> 5` / selectors 0..7;
11. envelope bytes map directly to D042 controls;
12. applicable row locks override the correct slot only;
13. common/non-page/other-engine IDs do not alter the D044 word;
14. values >255 retain base slot and report DOMAIN_FALLBACK;
15. duplicate applicable ID and `count>4` cause full structural fallback to base;
16. no resolver operation mutates the input base set or lock snapshot;
17. exhaustive engine × page-slot × normalized-value vectors are deterministic;
18. replay/guard/hash sequence is byte-identical;
19. static audit proves lock loop bound <=4 and no allocation/float/int64/division/store traversal;
20. real pinned JieLi PI32v2 compilation succeeds;
21. production source delta from the clean D044 pre-source checkpoint is exactly the isolated WP4.8 parameter module;
22. full host WAV / target BIN / FWSC / resources remain byte-identical to WP4.7 while isolated.

## 18. Non-goals

WP4.8 does not:

- connect the 8-BIT engine to `dinghy.c`;
- replace the Phase 3 proof sine;
- allocate the final 2048 B row cache;
- activate D044 IDs in the linked D027 registry;
- change the current common four-knob step-hold UI;
- alter D038-D043 DSP/allocator semantics;
- add a second page per engine;
- add NOISE color/tap mode or TRIANGLE shape control;
- define persistence/file ABI;
- perform the WP4.9 pitch/retrigger/full-engine migration;
- perform the WP4.10 eight-track benchmark.

## 19. Exit rule

No WP4.8 production parameter-page source may be created until D044 is recorded in `DECISIONS.md` and the project status/test matrix identify this contract as frozen.

WP4.9 must consume these exact IDs/page mappings/defaults and may only activate them in the global registry as part of its deliberate integration checkpoint. Any change to the six IDs, four-slot ordering, WIDTH/TABLE mapping or page applicability requires a superseding recorded decision and new deterministic evidence.
