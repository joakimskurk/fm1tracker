# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md` and `docs/PROJECT_CONTEXT.md` first so the active roadmap item is not mistaken for the whole product.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.8 complete; WP5.9 activation next - 2026-10-25**

Current active package: **none during the WP5.8 -> WP5.9 handoff checkpoint**  
Next package: **WP5.9 - mixed 8-BIT + FM4 validation**  
WP5.8 issue: **#51, ready to close**  
Current branch: `phase5/fm4-parameters`

## Persistent product direction

FM1Tracker remains one coherent engine-agnostic tracker with 8-BIT, FM, future virtual-analog/subtractive synthesis and drums/percussion in the same project environment. Future VA/drum DSP and resource architecture are not frozen yet and must receive their own audits/contracts. See `PROJECT_CONTEXT.md`.

## Completed Phase 5 foundation

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - COMPLETE.
- WP5.7 / D055 FM voice allocation and D048 integration - COMPLETE; closeout `80587f12786122dcec819260ddad5dd5db4b1005`, run `40177151084` SUCCESS.
- WP5.8 / D056 FM parameter pages and p-lock mapping - COMPLETE; accepted closeout run `40203483720` SUCCESS.

## WP5.8 / D056 contract

Audit: `docs/WP5_8_PRECONTRACT_AUDIT.md`.  
Normative: `docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`.  
Preimplementation evidence: `docs/WP5_8_PREIMPLEMENTATION_EVIDENCE.md`.  
Closeout evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md`.

Source-absent freeze:

```text
contract head            9436e385a96fa89b74c4c016c03f7857c2373552
run                      40178938471 SUCCESS
job                      121125607384 SUCCESS
artifact                 wp5-8-contract-evidence / 12592494488
artifact SHA-256         9c8ca18fe027b7286c3fe4e0b2086e133780b89df8721a01f4a91eafc672d51c
```

## Accepted WP5.8 production checkpoints

### Parameter model

```text
head                     aecc44ecad8cb0151a614ac397ded4a489e639a3
run                      40181092071 SUCCESS
artifact SHA-256         7338f3f8bd65161deb329bdcab83594b1338b6a43e354bacf2ab06b9af8fb04b
FM semantic IDs          27 exact
logical pages             7 exact
FmtFmParamSet             27 B exact
default D055 patch       SHA-256 3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

### Generalized row cache

```text
head                     560e1995f4fa82f9d987589bf5927c0633bf9833
run                      40181320961 SUCCESS
artifact SHA-256         43f4cc6d60c39038352bdf239e4253d4929a8f51e7a6c878707a6176e5175d1a
generalized row cache   8704 B exact
legacy 8BIT view        2048 B exact at offset 0
FM base bank             432 B exact
FM override slots          4 exact
double-buffer selector     1 byte per track/row
```

### Store-backed resolver

```text
head                     e466bcfde685a61f9c35721a2e10c1be62cad795
run                      40182000455 SUCCESS
artifact SHA-256         356a92107083fb906e7dd4e9d84144a031ea2de7b60f340477e2aa6235f3d10d
NOTE instrument inherit  exact
max locks per step         4 exact; fifth STEP_FULL
uint16 FM value filter   >255 ignored without truncation
wrong-family IDs         preserved in D028, ignored by FM payload
malformed sparse state   empty fail-safe publication
shared bank0/8BIT word   protected across family refresh
```

The earlier run `40181832759` remains deliberately rejected because a historical D028 failure was masked by a pipeline before fail-closed handling was fixed.

### Row-boundary FM patch snapshots

```text
production integration   d05e91ce505c8c014306bde9aaf31c48242f7645
verification head        72ceb68d27e176ec42137af8ab826b2d7e1a4225
run                      40182540509 SUCCESS
artifact SHA-256         024ce2308961035d3caa2ead33792a98bc12e1074552b240beb66f070f0762f2
current FM patch bank     224 B exact
validity bank               8 B exact
per-row full patch cache    0 B
store/lock traversal audio  0
```

### Page-aware held-step editor

```text
production integration   20400b498263b8d341251a6ad3521f32e0fa8171
verification head        e4e11f0319a4fd4034c7d1d07e6b6f2e22c7c841
run                      40184162192 SUCCESS
artifact SHA-256         be21e073bee5be57451e6937ca052bd4508254fc471e778d03b8e3958bbcf537
FM pages                   7 exact
FM page slots              4 exact
page state               main/UI only, zero audio reads
all-8BIT editor/audio    unchanged
```

### Mutable per-instrument FM base API

```text
head                     4fdc1e41949df6f9bdcfe08d2834d74d6cedb66d
run                      40184298151 SUCCESS
job                      121141708496 SUCCESS
artifact                 wp5-8-fm-base-api-evidence / 12595616335
artifact SHA-256         7ae4a888d8411ac04467258c37eb69495fda73fe545555513f72d7b50044bfff
```

Base mutation is bounded and instrument-local; existing p-lock overrides still win; invalid input does not mutate state; no row-cache rebuild/store traversal is performed by the setter.

## WP5.8 accepted closeout

The initial closeout run `40184529349` is not the accepted closeout because its host wrapper was red. The uploaded host evidence showed all gates had actually reached the green summary. Root cause was a wrapper race where `tee` could open the evidence path before the script created its directory. Commit `849c8b27d8c1afa09f9c093b695985c64e0c6013` pre-creates the directory. No production assertion or regression gate changed.

Accepted closeout:

```text
head                     849c8b27d8c1afa09f9c093b695985c64e0c6013
run                      40203483720 SUCCESS
host job                 121199398344 SUCCESS
target job               121199398538 SUCCESS
host artifact            12602299028
host artifact SHA-256    5669fe35f7d1f09e4515498f1e57b1de9b57e5bac850514f7df4019a53fb877c
target artifact          12603110877
target artifact SHA-256  75fa1c19938bda8bcec4b5bc68efa2f2a1c8a556da747ec9906868abda53de0d
image                    79804 B
normal RAM               27792 B
pool                     113000 B
BIN SHA-256              a336fb22050cbf6a7800c98db2c167c83ef5c4e635448c098d8e69ed42869c07
FWSC SHA-256             4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV             17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim    none
```

D056 frozen ceilings all pass:

```text
image       <= 93432 B   actual 79804 B   margin 13628 B
normal RAM  <= 28656 B   actual 27792 B   margin   864 B
pool         = 113000 B  actual 113000 B
```

The 864 B value is margin to the WP5.8 package ceiling, not a physical free-RAM claim. It is nevertheless a mandatory constraint for WP5.9/WP5.10 planning. D049-D055 synthesis/voice source and generated DSP tables remain frozen.

## Exact next action

1. close issue #51 as completed using `WP5_8_CLOSEOUT_EVIDENCE.md`;
2. activate WP5.9 as the only active package;
3. audit the existing mixed-family fixtures/router behavior and resource seams before implementation;
4. freeze a WP5.9 contract for simultaneous 8-BIT + FM4 operation, same-track family switching, deterministic behavior and exact all-8BIT regression equivalence;
5. do not pull future VA/subtractive or drum implementation into WP5.9.

No physical FM-1 is available. `hardware_margin_claim=none`.