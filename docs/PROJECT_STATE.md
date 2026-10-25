# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.4 complete; WP5.5 technical verification COMPLETE; closeout-head verification pending - 2026-10-25**

Current active package: **WP5.5 - bounded deterministic FM4 feedback closeout**  
Active issue: **#48**  
Active branch: `phase5/fm4-feedback`  
Next package after closeout: **WP5.6 - bounded FM4 envelope strategy**  
Next issue: **#49**  
Expected next branch: `phase5/fm4-envelope`

Do not activate WP5.6 until the WP5.5 documentation closeout head has passed the same host/target verification and #48 is closed.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 feedback - TECHNICALLY COMPLETE, closeout-head rerun pending.

## D053 canonical evidence

Normative contract: `docs/WP5_5_FM_FEEDBACK_CONTRACT.md`.  
Preimplementation evidence: `docs/WP5_5_PREIMPLEMENTATION_EVIDENCE.md`.  
Final technical evidence: `docs/WP5_5_VERIFICATION.md`.

Source-absent gate:

- run `40164610821`, job `121082987705`;
- artifact ID `12587966076`, SHA `0eb34eb28a92f69eeeb653ddae2661ca62d72e8eaf887a881e227fbe26c7e4b9`.

Implementation verification:

- head `d1ddb4baab7fcb4934cbf98e46c968fcd5e1f45e`;
- run `40164953532`;
- host job `121083999893`;
- target job `121083999790`;
- host artifact ID `12587379892`, SHA `d57ad000b83e5aa7c155ff817a9feb74727998f7baa417ca73b11da735850408`;
- target artifact ID `12587649156`, SHA `96183e229059cbc6ceb09b2057de5abf914d4d62ca0db89a8d1dea576ecb8645`.

Measured D053 cost:

```text
FmtFmFeedbackState      2 B per future FM voice
fmt_fm_feedback.o     130 B text+rodata
object ceiling       1024 B
object SHA           147c623c49fa8756f36937589d96f2da5139b6e3d1a09790e95d91c5f770256d
```

## Frozen D053 compatibility

- OP4 self-feedback only;
- exactly one-sample delay;
- one signed-16 prior OP4 output as state;
- distinct FEEDBACK byte `0..255`;
- Q14 mapping `(v << 6) | (v >> 2)`;
- phase offset `previous_op4_q15 * depth_q14 * 4`;
- FEEDBACK=0 keeps history tracking while phase offset is zero;
- reset/retrigger clear history;
- D050 algorithm switches retain history;
- full signed-int16 history x all 256 controls is signed-32 safe;
- production source is only `firmware/src/fmt_fm_feedback.h/.c`;
- D049-D052 and `dinghy.c` remain unchanged.

## Full target identity after D053

D053 remains isolated from the running application:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

## Current exact action

1. commit the WP5.5 closeout documentation;
2. require `.github/workflows/wp5_5_feedback_verify.yml` to pass again on that exact closeout head;
3. close #48 only after both host and target jobs are green;
4. create `phase5/fm4-envelope` from the accepted closeout head;
5. activate #49 and begin D054 envelope audit contract-first.

No WP5.6 production envelope source is allowed before D054 is frozen source-absent.

No physical FM-1 is available. `hardware_margin_claim=none`.
