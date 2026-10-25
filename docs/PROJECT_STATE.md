# FM1Tracker Project State

This is the **single source of truth for current execution state**. Read `START_HERE.md` and `docs/PROJECT_CONTEXT.md` first so the active roadmap item is not mistaken for the whole product.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.8 complete; WP5.9 active - 2026-10-25**

Current active package: **WP5.9 - mixed 8-BIT + FM4 validation**  
Active issue: **#52**  
Current branch: `phase5/mixed-validation`  
Precontract audit: `docs/WP5_9_PRECONTRACT_AUDIT.md`

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
- WP5.7 / D055 FM voice allocation and D048 integration - COMPLETE.
- WP5.8 / D056 FM parameter pages and p-lock mapping - COMPLETE.

## Accepted WP5.8 parent checkpoint

```text
head                     849c8b27d8c1afa09f9c093b695985c64e0c6013
run                      40203483720 SUCCESS
host                     121199398344 SUCCESS
target                   121199398538 SUCCESS
image                    79804 B
normal RAM               27792 B
pool                    113000 B
FWSC SHA-256             4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV             17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim    none
```

Full evidence: `docs/WP5_8_CLOSEOUT_EVIDENCE.md`.

The accepted D056 target is 864 B below the deliberately frozen WP5.8 normal-RAM package ceiling. This is not a physical free-RAM claim, but resource growth must remain explicit through WP5.9/WP5.10.

## WP5.9 audit findings

The precontract audit found a **validation gap, not a proven production defect**.

Existing evidence is split:

- D048 proves cross-track family coexistence and route semantics with a stub second family;
- WP5.7 proves the real FM4 engine, D043 ownership semantics and deterministic real FM -> 8-BIT -> FM switching on track 0;
- D056 proves real FM p-lock/row-snapshot behavior, again using a one-track mixed fixture;
- the production proof pattern seeds notes only on `FMT_PLAYBACK_TRACK`, so existing real-engine mixed fixtures never directly prove simultaneous FM4 and 8-BIT voices on different tracks;
- production `app_audio()` already renders both family engines once per frame and saturating-sums them;
- no production code defect has yet been identified.

The generic host harness is also 8-BIT-centric: its probes and linked-cache fixture refreshes do not expose FM/router state. That is a test-harness limitation and can be corrected without changing target firmware behavior or target RAM.

## Frozen direction before implementation

WP5.9 should remain **validation-only** unless a new fixture exposes an actual firmware defect.

Expected allowed changes before such a defect exists:

- host test harness observability/fixture support;
- WP5.9 tests/reference analysis;
- WP5.9 CI;
- documentation/evidence.

Any change under `firmware/src/**` or `firmware/gen/**` must stop this path and trigger an explicit contract amendment explaining the discovered defect before implementation.

## Exact next action

1. freeze `docs/WP5_9_MIXED_VALIDATION_CONTRACT.md` from the completed audit;
2. add a fail-closed contract-freeze CI gate;
3. prove the freeze before test-harness implementation;
4. then add real simultaneous-family overlap, strengthened same-track switch and event-semantics fixtures;
5. retain exact all-8BIT, D055 default/no-lock FM and D056 locked-FM regressions;
6. close with host + pinned-target evidence on one checkpoint.

The simultaneous-family oracle should use real FM-only, 8-BIT-only and combined renders and require the combined PCM to equal the firmware's exact saturated sum of the isolated streams during a window where both isolated streams are nonzero.

No physical FM-1 is available. `hardware_margin_claim=none`.
