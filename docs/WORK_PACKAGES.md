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

### WP5.9 - mixed 8-BIT + FM4 validation - COMPLETE - issue #52

Purpose: validate that the implemented engine families and D056 parameter path behave coherently under genuinely mixed tracker use before worst-case benchmarking.

Accepted closeout:

```text
code checkpoint  81d3464059270130747e26fcc950b7a2e1bb03e1
run              40210173573 SUCCESS
target           121219510315 SUCCESS
host             121219510342 SUCCESS
image            79836 B
normal RAM       27792 B
pool            113000 B
D056 delta       +32 B image / +0 B RAM / +0 B pool
```

The validation-first package exposed delayed cross-family defect 001 before repair. A separately frozen fix contract authorized only the narrow `firmware/src/dinghy.c` correction. All eight logical tracks remain intact. Full evidence: `WP5_9_CLOSEOUT_EVIDENCE.md`.

### WP5.10 - worst-case mixed-engine benchmark + Phase 5 exit - ACTIVE - issue #53

Purpose: benchmark the bounded mixed-engine production path under realistic high-load conditions and make an evidence-based Phase 5 exit decision without silently reducing the eight logical tracker tracks.

Precontract audit: `WP5_10_PRECONTRACT_AUDIT.md` - COMPLETE.

Contract-first scope:

1. freeze benchmark/load shape before implementation;
2. benchmark the real linked `app_audio()` path, not only isolated DSP helpers;
3. preserve exactly eight logical tracker tracks;
4. prove actual family-local voice occupancy at the measurement window;
5. attempt a reachable eight non-idle FM4 + eight non-idle 8-BIT state through ordinary tracker/release-tail semantics and record the actual reachable maximum if full 8+8 cannot be reached;
6. keep mixed-engine saturation separate from a full-callback-pressure case that may add bounded D034 queue ingress and four audition voices;
7. use test-only observation wrappers/source-injected fixtures rather than permanent production debug hooks;
8. extend inherited realtime audits to both final engine renderers and preserve fixed bounds/no heap/no sparse-store traversal;
9. require deterministic repeated sample/state/occupancy evidence;
10. keep host wall-clock timing informational only;
11. use 24 MHz TIMER4 target/emulator telemetry only if a reproducible execution-return path is established, and label emulator timing as emulator timing;
12. measure the ordinary uninstrumented target image/RAM/pool against D056/WP5.9;
13. preserve exact all-8BIT, D055 default/no-lock FM, D056 locked-FM and WP5.9 mixed/defect regressions;
14. authorize production optimization only if the frozen benchmark demonstrates a real measured constraint and only after a separate bounded optimization sub-contract;
15. record an explicit Phase 5 exit decision.

Non-goals:

- no new synthesis engine;
- no virtual-analog/subtractive implementation;
- no drum-engine implementation;
- no song/order/persistence work;
- no reduction below eight logical tracks;
- no speculative production optimization;
- no physical timing, audio-stability or free-RAM claim without a physical FM-1.

Exit criteria:

- precontract audit committed before benchmark implementation;
- benchmark contract and fail-closed freeze gate committed before benchmark implementation;
- mixed saturation workload reproducible and actual occupancy recorded;
- full callback pressure workload reproducible;
- deterministic repeated host digests green;
- dual-engine realtime audit green;
- inherited correctness regressions green;
- uninstrumented pinned-target build green with exact resource result recorded;
- any optimization, if required, separately contracted and evidence-driven;
- all eight logical tracks preserved;
- `hardware_margin_claim=none` explicit unless real physical evidence exists;
- Phase 5 exit decision and durable closeout evidence committed.

## Mandatory core milestone - live performance capture + performance FX

PLANNED after engine foundations. Reference `LIVE_PERFORMANCE_CAPTURE_VISION.md`. Drums remain a required playable/capturable direction even though their engine implementation is not part of WP5.10.

## Future engine-family expansion

Virtual-analog/subtractive synthesis and drums/percussion are explicit FM1Tracker product directions in the same tracker environment. They require dedicated CPU/RAM/parameter/license audits and frozen work packages before production implementation.

## Phase 6 - song/order/persistence - PLANNED
## Phase 7 - physical hardware UX freeze - BLOCKED
## Phase 8 - interoperability - PLANNED
## Phase 9 - optional extensions - DEFERRED

`hardware_margin_claim=none` until physical FM-1 evidence exists.
