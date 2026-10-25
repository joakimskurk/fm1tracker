# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.7 complete; WP5.8/D056 production implementation active - 2026-10-25**

Current active package: **WP5.8 - FM4 parameter pages and p-lock mapping**  
Active issue: **#51**  
Active branch: `phase5/fm4-parameters`

## Completed Phase 5 foundation

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - COMPLETE.
- WP5.7 / D055 FM voice allocation and D048 integration - COMPLETE; closeout `80587f12786122dcec819260ddad5dd5db4b1005`, run `40177151084` SUCCESS.

## WP5.8 / D056 frozen contract and authorization

Audit: `docs/WP5_8_PRECONTRACT_AUDIT.md`.  
Normative: `docs/WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`.  
Accepted preimplementation evidence: `docs/WP5_8_PREIMPLEMENTATION_EVIDENCE.md`.

Source-absent freeze:

```text
contract head            9436e385a96fa89b74c4c016c03f7857c2373552
run                      40178938471 SUCCESS
job                      121125607384 SUCCESS
artifact                 wp5-8-contract-evidence / 12592494488
artifact SHA-256         9c8ca18fe027b7286c3fe4e0b2086e133780b89df8721a01f4a91eafc672d51c
production authorization yes - bounded by D056
```

## Accepted WP5.8 production checkpoints

### Parameter model slice

```text
head                     aecc44ecad8cb0151a614ac397ded4a489e639a3
run                      40181092071 SUCCESS
job                      121132123278 SUCCESS
artifact                 wp5-8-param-model-evidence / 12594302196
artifact SHA-256         7338f3f8bd65161deb329bdcab83594b1338b6a43e354bacf2ab06b9af8fb04b
registry assigned IDs    37
FM semantic IDs          27 exact
logical pages             7 exact
FmtFmParamSet             27 B exact
default D055 patch       SHA-256 3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
all-8BIT WAV             WP5.0-identical
D055 engine behavior     exact focused regression
```

### Generalized row-cache slice

```text
head                     560e1995f4fa82f9d987589bf5927c0633bf9833
run                      40181320961 SUCCESS
job                      121132814963 SUCCESS
artifact                 wp5-8-row-cache-evidence / 12593344896
artifact SHA-256         43f4cc6d60c39038352bdf239e4253d4929a8f51e7a6c878707a6176e5175d1a
generalized row cache   8704 B exact
legacy 8BIT view        2048 B exact at offset 0
FM base bank             432 B exact
FM override slots          4 exact
double-buffer selector     1 byte per track/row
preemption model         deterministic inactive-write then selector flip
all-8BIT WAV             WP5.0-identical
```

### Store-backed FM resolver slice

```text
head                     e466bcfde685a61f9c35721a2e10c1be62cad795
run                      40182000455 SUCCESS
job                      121134811500 SUCCESS
artifact                 wp5-8-store-resolver-evidence / 12594586077
artifact SHA-256         356a92107083fb906e7dd4e9d84144a031ea2de7b60f340477e2aa6235f3d10d
NOTE instrument inherit  exact
max locks per step         4 exact; fifth STEP_FULL
uint16 FM value filter   >255 ignored without truncation
wrong-family IDs         preserved in D028, ignored by FM payload
malformed sparse state   empty fail-safe publication
shared bank0/8BIT word   protected across family-specific refresh
D028 regression          exact and fail-closed
all-8BIT WAV             WP5.0-identical
```

The earlier run `40181832759` is deliberately not accepted as evidence because a historical D028 assertion failed inside a `| tee` pipeline and its exit status was masked. The strict gate was corrected before accepting the resolver slice.

### Row-boundary FM patch snapshot slice

```text
head                     72ceb68d27e176ec42137af8ab826b2d7e1a4225
production integration   d05e91ce505c8c014306bde9aaf31c48242f7645
run                      40182540509 SUCCESS
job                      121136449218 SUCCESS
artifact                 wp5-8-row-snapshot-evidence / 12594144191
artifact SHA-256         024ce2308961035d3caa2ead33792a98bc12e1074552b240beb66f070f0762f2
current FM patch bank     224 B exact
validity bank               8 B exact
per-row patch cache         0 B
store/lock traversal audio  0
NOTE snapshot             bounded row-boundary compose
TIE/EMPTY                 live patch update without reset
RETRIG                    current row patch
OFF/CUT                   row-resolved release patch
FM p-lock mixed audio     deterministic; changes output
all-8BIT WAV             WP5.0-identical
```

### Page-aware held-step FM editor slice

```text
production integration   20400b498263b8d341251a6ad3521f32e0fa8171
verification head        e4e11f0319a4fd4034c7d1d07e6b6f2e22c7c841
run                      40184162192 SUCCESS
job                      121141304254 SUCCESS
artifact                 wp5-8-fm-editor-evidence / 12595551255
artifact SHA-256         be21e073bee5be57451e6937ca052bd4508254fc471e778d03b8e3958bbcf537
FM pages                   7 exact
FM page slots              4 exact; CORE K4 NONE
reference value          resolved instrument base, existing lock wins
mutation/delete refresh  common + generalized engine row, one row only
provisional page gesture OCT-/OCT+ only during valid FM held-step edit
page state               main/UI only, zero audio reads
legacy generic editor    exact regression
all-8BIT editor/audio    unchanged; WP5.0-identical WAV
one-shot apply tooling   retired after verified production commit
```

Frozen absolute package ceilings remain:

```text
image       <= 93432 B
normal RAM  <= 28656 B
pool         = 113000 B
```

D049-D055 synthesis/voice behavior remains frozen except for the bounded D056 adapter/integration seam.

## Current exact action

Verify the bounded mutable per-instrument FM base API. A base mutation must affect later row-boundary composition for that instrument, existing p-lock overrides must still win, unrelated instruments must remain unchanged, and invalid instrument/input must fail without mutation. Because the generalized FM row publication contains overrides only rather than full base patches, changing a base must not require a row-cache rebuild.

Then run the final WP5.8 target footprint and inherited D049-D055/8BIT regressions against the frozen absolute ceilings. If green, close WP5.8 and prepare WP5.9 according to `docs/WORK_PACKAGES.md`. Do not start WP5.9 before WP5.8 closeout evidence is recorded.

No physical FM-1 is available. `hardware_margin_claim=none`.
