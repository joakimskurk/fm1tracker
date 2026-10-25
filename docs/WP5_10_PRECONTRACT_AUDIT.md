# WP5.10 Precontract Audit - Worst-case mixed-engine benchmark + Phase 5 exit

Status: **COMPLETE - audit only, benchmark contract not yet frozen**  
Date: 2026-10-25  
Issue: `#53`  
Branch: `phase5/worst-case-benchmark`  
Parent documentation handoff: `c06550f72918964bd49467228688a7232d2c4cf9`  
Last target-validated production checkpoint: `81d3464059270130747e26fcc950b7a2e1bb03e1`

## Audit purpose

WP5.10 is the Phase 5 exit package. This audit determines what a realistic mixed-engine worst-case benchmark must measure before a benchmark contract is frozen or any production optimization is permitted.

No production change is authorized by this document.

## Frozen parent evidence

WP5.9 closed with:

```text
run                      40210173573 SUCCESS
target                   121219510315 SUCCESS
host                     121219510342 SUCCESS
image                    79836 B
normal RAM               27792 B
pool                    113000 B
D056 -> WP5.9 image      +32 B
D056 -> WP5.9 RAM        +0 B
D056 -> WP5.9 pool       +0 B
hardware_margin_claim    none
```

D056 remains the parent resource reference at 79,804 B image / 27,792 B normal RAM / 113,000 B pool.

## A01 - Real audio callback shape

`firmware/src/dinghy.c::app_audio()` is the production tracker renderer.

While tracker playback is started, every rendered frame:

1. renders `fmt_8bit_engine_render()`;
2. renders `fmt_fm_engine_render()`;
3. saturating-adds both family outputs;
4. advances the shared tracker transport;
5. on a row boundary, applies events for all eight logical tracks;
6. otherwise, on an effect tick, checks all eight logical tracks.

The callback also:

- consumes the bounded D034 audition queue once at callback entry;
- renders up to four manual/audition sine voices after tracker rendering.

On target, `platform.c` calls `app_audio()` for `HALF_FRAMES=256` at `FS=44100`, inside the ALNK0 audio ISR.

Audit conclusion: WP5.10 must benchmark the real linked callback path, not isolated FM operator microbenchmarks alone.

## A02 - Voice bounds are family-local, not eight total DSP voices

The logical tracker remains exactly eight tracks.

The 8-BIT family owns an eight-slot `FmtVoiceAllocator`. FM4 independently owns eight `FMT_FM_ENGINE_SLOTS` and eight track addresses. Both renderers scan their complete fixed eight-slot banks on every tracker audio frame and skip only idle voices.

Family switches and NOTE OFF can leave detached release tails inside the outgoing family while a later note uses the other family. Therefore a realistic high-load mixed window must not assume that eight logical tracks imply at most eight non-idle DSP voices across the two engines.

Contract requirement:

- the benchmark must prove observed occupancy at the measurement window;
- it must include a reachable mixed saturation case with non-idle slots in both family banks;
- it must attempt the architecture maximum of eight non-idle FM4 slots plus eight non-idle 8-BIT slots using ordinary tracker semantics and detached release tails;
- if the full 8+8 state is not reachable through the production tracker adapter, the contract must record the measured reachable maximum rather than fabricate it;
- all eight logical tracks must remain available throughout.

## A03 - FM4 is the expensive family and must be represented at full slot pressure

Every non-idle FM voice performs:

- four envelope-level mappings;
- OP4 feedback render;
- the selected four-operator algorithm path;
- output gain multiply;
- four-operator envelope advancement.

Every non-idle 8-BIT voice performs one oscillator render, one envelope render and gain scaling.

Audit conclusion: a benchmark that exercises only a token FM voice alongside seven 8-BIT tracks is not a worst-case mixed-engine benchmark. The measurement window must include all reachable FM4 slot pressure while the other family is also non-idle.

## A04 - Callback pressure outside tracker engines is real but must remain separable

The production callback can additionally process up to 32 queued D034 audition events at callback entry and render four manual/audition voices.

The contract should therefore freeze two distinguishable evidence cases:

1. **mixed-engine saturation** - isolates the sustained linked tracker DSP path and proves mixed family occupancy;
2. **full callback pressure** - adds bounded queue ingress, four audition voices and a tracker control transition to the mixed-engine load.

This separation prevents queue/audition overhead from hiding the cost and occupancy of the engine path while still testing the complete production callback envelope.

## A05 - Existing host evidence provides the correct instrumentation pattern

WP4.10 already established a safe host-only observation approach:

- build an exact copy of the production entry translation unit;
- interpose generated test-only wrappers around selected production functions;
- record counters/digests without feeding instrumentation back into firmware state;
- require deterministic repeated renders;
- explicitly avoid using host wall-clock speed as a hardware acceptance gate.

WP5.9 additionally established source-injected mixed fixtures and generated host probes without changing historical `tests/host.c` or target production source.

Audit conclusion: WP5.10 should extend these existing mechanisms rather than add permanent debug hooks to production firmware.

## A06 - Existing static realtime audits are useful but stale for final mixed load

WP4.10 and WP5.0 already prove important inherited bounds:

- no heap allocation in realtime paths;
- one bounded 32-entry queue consumer;
- one shared tracker clock;
- eight-track row/tick loops;
- no song/editor/screen traversal in the sample path;
- fixed eight-slot 8-BIT rendering;
- integer-only realtime code.

However, the WP5.0 audit predates the final FM4 renderer and only asserts the 8-BIT render call. WP5.10 must extend the realtime audit to include:

- exactly one 8-BIT render and one FM4 render per tracker frame;
- fixed eight-slot scans in both engines;
- no heap, sparse-store/editor traversal or screen work introduced by FM4;
- bounded four-operator FM inner work;
- the existing row-boundary-wins ordering and eight-track logical model.

## A07 - Target timer exists, but current automated target path does not execute firmware

`firmware/hal/fm1_time.h` exposes free-running TIMER4 from the 24 MHz crystal through `fm1_ticks()`. On real target code this can measure elapsed target ticks without ROM calls or interrupts from the timer itself.

The production target audio half-buffer is 256 frames at 44.1 kHz, corresponding nominally to about 5.805 ms or about 139,320 TIMER4 ticks between half-buffer deadlines.

Current WP5.9 target closeout proves pinned toolchain build, exact image/RAM/pool and package identity, but it does not execute the firmware on physical FM-1 hardware in CI.

Audit conclusion:

- TIMER4 is a valid candidate for test-only target runtime telemetry;
- the WP5.10 contract may use it only if a reproducible target/emulator execution path can return the measurements;
- emulator timing must be labeled emulator timing, not physical-device timing;
- host wall-clock timing must remain informational only;
- no timing number from host or emulator may be represented as physical FM-1 realtime margin.

`hardware_margin_claim=none`

## A08 - Resource evidence must use an uninstrumented production build

Test-only benchmark instrumentation may change its own test image and therefore cannot redefine the production resource baseline.

The WP5.10 production closeout build must measure the ordinary uninstrumented firmware against:

```text
D056 image        79804 B
WP5.9 image       79836 B
normal RAM        27792 B
pool             113000 B
```

If no production optimization is authorized, `firmware/src`, `firmware/gen`, target HAL/loader, linker/build and production tools must remain frozen from `81d3464` and the production target footprint should remain unchanged except for reproducibility noise that must itself be explained.

If the frozen benchmark exposes a real constraint, work stops and a separate bounded optimization sub-contract must define exactly what may change and what resource/performance result is required.

## A09 - Required inherited regressions

WP5.10 must retain, at minimum:

- exact all-8BIT regression;
- D055 default/no-lock FM behavior;
- D056 locked-FM semantics;
- WP5.9 simultaneous mixed-family fixtures;
- WP5.9 same-track family switch and delayed cross-family defect regression;
- allocator ownership invariants;
- queue overflow/recovery bounds;
- deterministic repeated host renders.

A benchmark is not allowed to replace correctness verification.

## A10 - Phase 5 exit semantics without physical hardware

Phase 5 can close on software/target-build evidence if the frozen benchmark demonstrates:

- deterministic reachable worst-case workload shape;
- bounded realtime source structure;
- all eight logical tracks preserved;
- stable host state/audio digests under repeated high-load execution;
- accepted uninstrumented target build and exact resource accounting;
- no measured software/target-build constraint requiring a production architecture change.

Because no physical FM-1 is available, Phase 5 closeout cannot claim physical audio stability, real-device CPU headroom or physical free-RAM margin. Those remain explicit hardware-validation debt for the later physical-hardware phase.

## Contract decisions to freeze next

The WP5.10 benchmark contract must freeze before benchmark implementation:

1. exact mixed saturation fixture and how 8+8 or the measured reachable maximum is proven;
2. exact full-callback-pressure fixture;
3. measurement windows, warmup, row/transition placement and repetition count;
4. deterministic sample/state/occupancy digests and required repeat equality;
5. test-only observation hooks and proof that production state is not altered;
6. static realtime audit checks for both engine renderers;
7. host timing as informational-only if collected;
8. whether TIMER4 target/emulator telemetry is reproducibly retrievable; if not, it is explicitly excluded rather than guessed;
9. uninstrumented target build/resource gate against D056/WP5.9;
10. inherited regression set;
11. optimization trigger and required stop-to-contract behavior;
12. exact Phase 5 exit decision format.

## Audit outcome

**WP5.10 PRECONTRACT AUDIT: PASS**

The architecture is benchmarkable without changing production firmware. The next legal step is to freeze `docs/WP5_10_WORST_CASE_MIXED_BENCHMARK_CONTRACT.md` plus its fail-closed contract-freeze gate. No production optimization is authorized yet.
