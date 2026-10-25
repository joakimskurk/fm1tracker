# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.5 complete; WP5.6 technical verification COMPLETE; closeout-head verification pending - 2026-10-25**

Current active package: **WP5.6 - bounded FM4 envelope strategy closeout**  
Active issue: **#49**  
Active branch: `phase5/fm4-envelope`  
Next package after closeout: **WP5.7 - FM4 voice allocation and D048 router integration**  
Next issue: **#50**  
Expected next branch: `phase5/fm4-voice-integration`

Do not activate WP5.7 until the WP5.6 documentation closeout head has passed the same host/target verification and #49 is closed.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - COMPLETE.
- WP5.6 / D054 four independent FM4 ADSR envelopes - TECHNICALLY COMPLETE, closeout-head rerun pending.

## D054 canonical evidence

Normative contract: `docs/WP5_6_FM_ENVELOPE_CONTRACT.md`.  
Preimplementation evidence: `docs/WP5_6_PREIMPLEMENTATION_EVIDENCE.md`.  
Technical verification: `docs/WP5_6_VERIFICATION.md`.

Source-absent gate:

- head `151401f164a17c3459f965971f2855bb05831c4b`;
- run `40167537148`;
- job `121091550742` SUCCESS;
- artifact ID `12588534519`, SHA `eaea6ae76ee55779f45b4e2692c226e0790e0ebeb22326a9e722359fe76eec36`.

Implementation verification:

- head `401e666d21fa8290c7c622603e06e97b166c6b3e`;
- run `40167933064`;
- host `121092688126` SUCCESS;
- target `121092688295` SUCCESS;
- host artifact ID `12588901443`, SHA `4452eec3d50127a392dad79246b54cf93883837f9a02b2af64848d129c76a202`;
- target artifact ID `12588804647`, SHA `c8eed7e762c3d58da18f0a3b5fbad28878b6da2e70181d22f2376ea7c3686f20`.

Measured D054 cost:

```text
FmtFmEnvelopeState                 20 B per future FM voice
FmtFmEnvelopeControls              16 B per FM instrument
immutable time-step table        1024 B
fmt_fm_envelope.o text+rodata    1680 B
object ceiling                   2048 B
object margin                     368 B
object SHA 22fa7937d7776fea37c8e758ca83ee0a3f5d6c506ebd4661a8290a4516e048d1
```

D050 + D053 + D054 synthesis state before allocator metadata is 38 B per future FM voice.

## Frozen D054 compatibility

- four independent per-operator ADSR envelopes;
- 16 stable A/D/S/R control bytes total;
- Q15.16 envelope state, full scale `0x7fff0000`;
- envelope scales D052 Q15 LEVEL before D049 render;
- per-sample bounded update;
- hard retrigger from zero;
- release from current level;
- control 0 gives true instant A/D/R transitions;
- A/D/R controls 1..255 use the exact immutable 1,024 B time-step table;
- SUSTAIN uses the exact D052 LEVEL mapping;
- D050 algorithm changes do not reset envelope state;
- D049-D053 and `dinghy.c` remain unchanged;
- production source is isolated from the running application.

## Full target identity after D054

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

## Current exact action

1. commit the WP5.6 closeout documentation;
2. require `.github/workflows/wp5_6_fm_envelope_verify.yml` to pass again on that exact closeout head;
3. close #49 only after both host and target jobs are green;
4. create `phase5/fm4-voice-integration` from the accepted closeout head;
5. activate #50 and begin D055 voice/allocator/router audit contract-first.

No WP5.7 integration change is allowed before D055 is frozen.

No physical FM-1 is available. `hardware_margin_claim=none`.
