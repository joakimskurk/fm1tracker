# FM1Tracker Work Packages

`PROJECT_CONTEXT.md` preserves product intent. `PROJECT_STATE.md` is the single source of truth for current execution. This file defines bounded package scope and exit criteria.

## Rules

1. One active package at a time.
2. A package closes only with durable verification or an explicit recorded waiver.
3. Do not pull later-package features forward.
4. Architectural decisions are recorded before implementation.
5. Realtime work remains bounded and deterministic.
6. Eight logical tracks may not be silently reduced.
7. Roadmap focus must not erase persistent product directions such as future virtual-analog/subtractive synthesis or drums/percussion.
8. Substantive user decisions are synchronized back into Git according to `EXECUTION_PROTOCOL.md`.

## Phase 1-4 - COMPLETE

Canonical D046 exit: run `40124536375`, job `120963022289`.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Parent issue: #42.

### WP5.0 - mixed-engine routing - COMPLETE - issue #43 - D048
### WP5.1 - fixed-point FM operator - COMPLETE - issue #44 - D049
### WP5.2 - curated FM4 algorithm routing - COMPLETE - issue #45 - D050
### WP5.3 - integer ratio table + pitch mapping - COMPLETE - issue #46 - D051
### WP5.4 - levels / modulation index - COMPLETE - issue #47 - D052
### WP5.5 - bounded feedback - COMPLETE - issue #48 - D053
### WP5.6 - FM envelope strategy - COMPLETE - issue #49 - D054
### WP5.7 - FM voice allocation/integration - COMPLETE - issue #50 - D055

D055 integrates D049-D054 as eight bounded family-local FM voices through D048 while preserving exact D043 ownership semantics. Final closeout `80587f12786122dcec819260ddad5dd5db4b1005`, run `40177151084` SUCCESS.

### WP5.8 - FM parameter pages + p-lock mapping - COMPLETE - issue #51 - D056

D056 integrates FM4 with the shared semantic parameter, sparse p-lock, row-cache and held-step editor architecture while preserving exact all-8BIT behavior.

Accepted closeout:

```text
head          849c8b27d8c1afa09f9c093b695985c64e0c6013
run           40203483720 SUCCESS
host          121199398344 SUCCESS
target        121199398538 SUCCESS
image         79804 B
normal RAM    27792 B
pool         113000 B
FWSC SHA      4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
```

Frozen D056 facts:

- 27 stable FM semantic IDs and seven four-control pages;
- 27 B normalized base per instrument, 432 B total for 16 instruments;
- max four sparse p-locks per step unchanged;
- 8,704 B generalized row cache preserving the first 2,048 B historical 8BIT view;
- double-buffered bounded FM row overrides;
- 232 B current row-boundary FM patch snapshot state, no per-row full-patch cache;
- page-aware held-step FM editor with zero page-state audio reads;
- no sparse store/editor traversal in the sample renderer;
- D049-D055 DSP/voice source frozen;
- exact WP5.0 all-8BIT WAV equivalence.

Evidence: `WP5_8_CLOSEOUT_EVIDENCE.md`.

The accepted target is 864 B below the frozen D056 normal-RAM package ceiling. This does not claim only 864 B remains physically available, but later Phase 5 packages must measure resource growth rather than assume it.

### WP5.9 - mixed 8-BIT + FM4 validation - NEXT

Purpose: validate that the already-implemented engine families and D056 parameter path behave coherently under genuinely mixed tracker use before worst-case benchmarking.

Contract-first scope:

1. audit existing WP5.0/WP5.7 mixed fixtures, D048 routing, D043 ownership and D056 row/p-lock integration;
2. define simultaneous 8-BIT + FM4 workloads that exercise multiple tracks at the same time, not only separated time windows;
3. define same-track family-switch sequences and exact ownership/state cleanup expectations;
4. include NOTE, TIE/EMPTY, RETRIG, OFF/CUT and relevant p-lock transitions where they cross the family-routing seam;
5. preserve exact WP5.0 all-8BIT bitstream equivalence as a non-negotiable regression;
6. preserve D055 default/no-lock FM behavior and D056 locked-FM semantics;
7. require deterministic repeated renders for every mixed fixture;
8. audit realtime/sample-path boundedness and forbid new sparse-store/editor traversal in audio;
9. record target image/RAM/pool deltas from the accepted D056 closeout, with the 27,792 B normal-RAM result visible in the contract;
10. use fail-closed host and pinned-target gates before closeout.

Non-goals:

- no new synthesis engine;
- no virtual-analog/subtractive implementation;
- no drum-engine implementation;
- no song/order/persistence work;
- no physical hardware-margin claim;
- no broad performance benchmark beyond what is required to validate correctness. Worst-case benchmarking belongs to WP5.10.

Exit criteria:

- precontract audit and frozen validation contract committed before production behavior changes;
- simultaneous-family fixtures deterministic and green;
- same-track family-switch fixtures deterministic and green;
- no ownership leak/active cross-track primary stealing;
- all-8BIT exact regression green;
- inherited D055/D056 semantics green;
- host + pinned-target closeout green on one accepted checkpoint;
- exact resource deltas recorded;
- durable verification document committed.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - PLANNED

Representative worst-case mixed workload, realtime audit, inherited regressions and exact target closeout. WP5.10 may measure and, if necessary, force an explicit architecture/resource decision; it may not silently reduce eight logical tracks.

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. Reference `LIVE_PERFORMANCE_CAPTURE_VISION.md`. Drums remain a required playable/capturable direction even though their engine implementation is not part of WP5.9.

## Future engine-family expansion

Virtual-analog/subtractive synthesis and drums/percussion are explicit FM1Tracker product directions in the same tracker environment. They require dedicated CPU/RAM/parameter/license audits and frozen work packages before production implementation.

## Phase 6 - song/order/persistence - PLANNED
## Phase 7 - physical hardware UX freeze - BLOCKED
## Phase 8 - interoperability - PLANNED
## Phase 9 - optional extensions - DEFERRED

`hardware_margin_claim=none` until physical FM-1 evidence exists.