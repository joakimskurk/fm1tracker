# WP5.8 / D056 Closeout Evidence

Date: 2026-10-25

Package: **WP5.8 - FM parameter pages + p-lock mapping**  
Decision: **D056**  
Branch: `phase5/fm4-parameters`

## Result

**COMPLETE.** D056 connects FM4 to the shared tracker parameter and sparse p-lock architecture without creating a parallel tracker model and without changing the accepted all-8BIT output.

Accepted closeout code/workflow checkpoint:

```text
head     849c8b27d8c1afa09f9c093b695985c64e0c6013
run      40203483720 SUCCESS
host     121199398344 SUCCESS
target   121199398538 SUCCESS
```

Artifacts:

```text
host     wp5-8-closeout-host / 12602299028
         sha256:5669fe35f7d1f09e4515498f1e57b1de9b57e5bac850514f7df4019a53fb877c
target   wp5-8-closeout-target / 12603110877
         sha256:75fa1c19938bda8bcec4b5bc68efa2f2a1c8a556da747ec9906868abda53de0d
```

Pinned AC79 SDK commit:

```text
d179b4484759423312073f5fbb232501aa491047
```

## D056 tracker-facing FM model

Verified contract:

```text
FM semantic IDs             27
ID block                    0x60..0x7B, 0x63 intentionally unassigned
logical pages                7
FmtFmParamSet               27 B
instrument bases            16
FM base bank               432 B
locks per step               4 maximum
lock storage value          uint16_t, >255 FM values rejected not truncated
```

The seven four-control pages are CORE, RATIO, LEVEL, ATTACK, DECAY, SUSTAIN and RELEASE. The shared semantic parameter registry remains authoritative; no FM-only parallel registry was introduced.

## Generalized row cache and publication

Verified physical cache:

```text
total engine-row cache     8704 B
legacy 8BIT view           2048 B at offset 0
FM base bank                432 B
FM override slots             4
FM override banks             2
active selector               1 byte per track/row
```

The first 2048 B remains byte/layout/API compatible with the historical 8BIT cache. FM publication writes the inactive override bank and flips the selector only after the row payload is complete.

NOTE instrument inheritance is exact. A fifth lock returns STEP_FULL. Wrong-family IDs remain valid D028 data but are ignored by FM composition. Malformed sparse state fails safe to an empty FM publication. Shared 8BIT data remains protected during family-specific refresh.

## Row-boundary FM patch snapshots

Current active FM patch state is bounded and not replicated per row:

```text
active patch bank           224 B
validity bank                 8 B
snapshot total              232 B
per-row full patch cache      0 B
```

NOTE composes at the row boundary. TIE/EMPTY can update the live patch without resetting the voice. RETRIG uses the current row patch. OFF/CUT use the row-resolved release patch. Sparse lock/store traversal does not occur in the per-sample renderer.

Focused locked-FM audio is deterministic and the lock changes output. The all-8BIT production WAV remains the accepted WP5.0 stream:

```text
sha256 17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
```

## Page-aware editor and mutable instrument bases

Held-step FM editing uses the D056 semantic page model. The provisional OCT-/OCT+ page gesture is active only for a valid held FM step. The legacy generic editor and 8BIT behavior remain unchanged.

The mutable instrument base API is bounded:

```text
head     4fdc1e41949df6f9bdcfe08d2834d74d6cedb66d
run      40184298151 SUCCESS
job      121141708496 SUCCESS
artifact wp5-8-fm-base-api-evidence / 12595616335
sha256   7ae4a888d8411ac04467258c37eb69495fda73fe545555513f72d7b50044bfff
```

A base mutation affects later composition for that instrument, unrelated instruments remain unchanged, an existing p-lock wins over the base, invalid input does not mutate state, and the setter does not rebuild row caches or traverse sparse lock storage.

## Source and realtime boundary

Exactly nine production paths differ from the WP5.7 closeout base `80587f12786122dcec819260ddad5dd5db4b1005`:

```text
firmware/src/dinghy.c
firmware/src/fmt_8bit_linked_params.c
firmware/src/fmt_8bit_linked_params.h
firmware/src/fmt_fm_linked_params.c
firmware/src/fmt_fm_linked_params.h
firmware/src/fmt_fm_params.c
firmware/src/fmt_fm_params.h
firmware/src/fmt_lock_editor.c
firmware/src/fmt_param_registry.h
```

D049-D055 synthesis/voice/router source and generated FM DSP tables remain frozen.

Accepted host closeout markers:

```text
WP5_8_CLOSEOUT_SOURCE_OK=1 decision=D056 production_delta=9 d049_d055=frozen generated=frozen
WP5_8_CLOSEOUT_REALTIME_OK=1 row_compose=bounded per_sample_store=0 per_sample_editor=0 page_state_audio=0 heap=0 float=0 int64=0 hardware_margin_claim=none
WP5_8_CLOSEOUT_HOST_OK=1 decision=D056 ids=27 pages=7 locks=4 cache=8704 base_bank=432 snapshot=232 editor=page_aware base_api=bounded d055=exact d054_d049=exact all8bit=wp5_0_identical realtime=pass hardware_margin_claim=none
```

The host closeout independently re-runs the D054 gate, which recursively re-proves D053 through D049 and the accepted full-app all-8BIT stream.

## Pinned target footprint

```text
image                       79804 B
normal RAM                  27792 B
pool RAM                   113000 B
image delta vs D055          2756 B
normal RAM delta vs D055     7328 B
pool delta vs D055              0 B
D056 image ceiling          93432 B
D056 normal-RAM ceiling     28656 B
image margin to ceiling     13628 B
normal-RAM margin              864 B
BIN sha256  a336fb22050cbf6a7800c98db2c167c83ef5c4e635448c098d8e69ed42869c07
FWSC sha256 4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
```

Target symbols confirm the expected bounded allocations:

```text
fmt_engine_row_param_cache  0x2200 = 8704 B
fmt_fm_instrument_base      0x01b0 = 432 B
fmt_fm_engine_runtime       0x0314 = 788 B
```

The 864 B margin is margin to the deliberately frozen **WP5.8 package ceiling**, not a claim that the AC791N has only 864 B remaining overall. It is nevertheless important planning context: WP5.9 and WP5.10 must measure actual mixed-engine behavior and may not assume additional normal-RAM growth is harmless.

## Closeout-wrapper correction

The first closeout run `40184529349` had a green target and a red host job even though the uploaded host evidence reached `WP5_8_CLOSEOUT_HOST_OK=1`. The cause was CI wrapper ordering: `tee build/wp5_8_closeout_host/ci.log` could start before the closeout script created that directory. With `set -o pipefail`, the failed `tee` correctly made the job red after the actual tests had completed successfully.

Commit `849c8b27d8c1afa09f9c093b695985c64e0c6013` pre-creates the evidence directory before starting the pipeline. No production source, assertion or regression gate was weakened. Run `40203483720` then passed both host and target, and the final host artifact includes `ci.log`.

## Exit state

All WP5.8 implementation and closeout criteria are satisfied. D056 is implemented and verified.

WP5.9 owns mixed 8BIT + FM4 validation. It must start contract/audit first and may not silently broaden into future virtual-analog/subtractive or drum-engine implementation. Those remain explicit FM1Tracker product directions in `PROJECT_CONTEXT.md` and require their own later work-package contracts.

No physical FM-1 is available. `hardware_margin_claim=none`.