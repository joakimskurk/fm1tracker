# WP5.9 Precontract Audit - mixed 8-BIT + FM4 validation

Date: 2026-10-25  
Branch: `phase5/mixed-validation`  
Issue: #52  
Parent closeout: WP5.8 / D056 `ce13b7a4f603e32233b1de4619ccfaf539e1e930`

## Purpose

WP5.9 exists to validate the already-implemented 8-BIT + FM4 architecture under genuinely mixed tracker use before WP5.10 performs worst-case mixed-engine benchmarking.

This audit is deliberately performed before production behavior changes. Its job is to separate:

1. invariants already proved by D048-D056;
2. integration behavior that exists in production but has not been proved with the real two-family engines at the same time;
3. host-test limitations that make the missing behavior hard to observe;
4. any actual production defect that would justify a later code change.

## Accepted parent state

WP5.8 / D056 accepted closeout:

```text
run                      40203483720 SUCCESS
host                     121199398344 SUCCESS
target                   121199398538 SUCCESS
image                    79804 B
normal RAM               27792 B
pool                    113000 B
FWSC SHA-256             4d2805f428f5114531b4c3d379576d2b207cbc9a6c04fcbdd5263cfaa0cc84a0
all-8BIT WAV SHA-256     17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
hardware_margin_claim    none
```

The 27,792 B normal-RAM result is 864 B below the deliberately frozen D056 package ceiling of 28,656 B. This is not a physical free-RAM claim. It is mandatory planning context for WP5.9 and WP5.10.

## What D048 already proves

`WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md` and `tests/wp5_0_router_contract.c` freeze the engine-agnostic routing rules:

- different families may coexist on different tracks;
- a track owns at most one routed primary family at a time;
- an active primary still owned by another track may not be stolen;
- cross-family NOTE hard-silences the old primary, clears the route, then attempts the new family;
- failed replacement leaves route NONE/silent;
- TIE/EMPTY and RETRIG do not allocate or change family;
- OFF/CUT releases the current family and clears only that track's global route;
- malformed identities fail toward silence;
- the common routing work is static and bounded.

However, the coexistence proof in `wp5_0_router_contract.c` uses a **stub second family**. It proves the D048 control model, not simultaneous real 8-BIT + FM4 DSP.

`run_wp5_0_family_switch_host.sh` also predates FM DSP integration. Its 16BIT fixture is intentionally unsupported and therefore proves fail-to-silence/re-entry behavior, not a successful real 8-BIT <-> FM4 transition.

## What D055/WP5.7 already proves

`tests/wp5_7_engine_host.c` proves the real FM4 family locally:

- eight fixed FM slots;
- exact D043 ownership semantics;
- no active cross-track primary stealing;
- detached release-tail reuse;
- NOTE/retrigger/release behavior;
- malformed FM engine identity fails safely.

`tests/run_wp5_7_integration_tests.sh` then links the real FM engine into the actual `dinghy.c` route and verifies:

- exact all-8BIT WP5.0 regression equivalence;
- deterministic real-engine FM -> 8-BIT -> FM switching on track 0;
- audible activity in each family era;
- one FM renderer call at the top-level sample seam;
- inherited D054 -> D049 behavior.

### Critical limitation

The production proof pattern is seeded only on `FMT_PLAYBACK_TRACK` by `fmt_playback_seed_proof_pattern()`. The WP5.7 mixed fixture changes instrument identity and rows only for track 0.

Therefore WP5.7 does **not** prove real simultaneous-family operation on separate tracks. Its `fm_a`, `bit`, `fm_b` windows are sequential eras of one track, and the assertion observes aggregate WAV energy only.

This means the statement `mixed=deterministic` in the WP5.7 summary is correct for deterministic family switching, but it must not be interpreted as proof that real 8-BIT and FM4 voices were active concurrently.

## What D056/WP5.8 already proves

D056 adds the real FM tracker-facing parameter path:

- 27 semantic FM parameter IDs / seven pages;
- bounded mutable instrument bases;
- four sparse locks per step unchanged;
- generalized row cache and double-buffered FM override publication;
- current row-boundary FM patch snapshots;
- no sparse store/editor traversal in the sample renderer;
- deterministic FM p-lock audio;
- exact all-8BIT regression equivalence.

`tests/run_wp5_8_row_snapshot_tests.sh` uses the same basic one-track FM -> 8-BIT -> FM source-injected fixture as WP5.7, with and without a real FM p-lock on row 0.

Therefore D056 proves that a real FM p-lock changes real FM audio deterministically across the row-boundary snapshot seam, but again does **not** prove simultaneous real 8-BIT + FM4 activity on distinct tracks.

## Production code audit

The current `dinghy.c` architecture already contains the necessary mixed-family seams:

1. all eight track events are dispatched at each row boundary;
2. `tracker_note_route()` reads per-instrument family/engine identity;
3. cross-family replacement hard-silences the old family before committing the new route;
4. successful 8-BIT and FM NOTE paths commit the corresponding family only after family-local allocation succeeds;
5. TIE/EMPTY updates stay on the currently routed family;
6. RETRIG dispatches only through the currently routed family;
7. OFF/CUT releases the currently routed family and clears that route;
8. FM row patches are snapshotted once per track/row from already-published D056 data;
9. the audio frame renders `fmt_8bit_engine_render()` once and `fmt_fm_engine_render()` once, adds them, then saturates to the final sample.

No production defect requiring a code change has been identified by this audit.

The missing evidence is therefore a **validation gap**, not currently an implementation gap.

## Host harness audit

The generic `tests/host.c` fixture interface was created before real FM integration and remains 8-BIT-centric:

- it declares/probes `Fmt8BitEngine`, but not `FmtFmEngine` or `FmtEngineRouter`;
- `probe` reports 8-BIT track/voice ownership only;
- fixture `clear`, `step` and `lock` refresh 8-BIT linked parameters, but do not refresh the D056 FM linked cache;
- fixture `base` edits only `Fmt8BitParamSet`;
- there is no generic fixture command for instrument family/engine identity.

This is why WP5.7 and WP5.8 created source-injected `dinghy.c` variants instead of expressing their fixtures entirely through the generic host script.

These are test-harness limitations. Extending the host harness does not require changing target firmware behavior or adding target RAM.

## WP5.9 recommended implementation boundary

Unless a new mixed fixture exposes a real firmware defect, WP5.9 should make **zero production-source behavior changes**.

Allowed WP5.9 changes should be limited to:

- `tests/host.c` mixed-family observability/fixture support, if needed;
- new WP5.9 host test sources/scripts/reference analysis;
- new WP5.9 CI workflow(s);
- documentation/status/evidence.

Any required change under `firmware/src/**` or `firmware/gen/**` must stop the validation-only path, be documented as a newly discovered production defect, and receive an explicit contract amendment before implementation.

## Required fixture A - real simultaneous-family overlap

The contract should require a fixture with at least:

- one track owning a real FM4 primary;
- a different track owning a real 8-BIT primary during the same sample window;
- both isolated family renders nonzero in that overlap window;
- repeated combined renders byte-identical;
- observable global route and family-local ownership agree for both tracks;
- no active cross-track primary stealing.

A strong audio oracle is to render three otherwise-identical variants:

1. FM-only;
2. 8-BIT-only;
3. both simultaneously.

For every frame in the overlap region, the combined PCM should equal the firmware's exact saturated sum of the two isolated PCM streams. This directly proves that both real family renderers coexist and are combined through the production top-level mixer rather than relying only on aggregate nonzero energy.

## Required fixture B - real same-track family switch

Strengthen the existing WP5.7 FM -> 8-BIT -> FM test with state/ownership assertions, not just WAV windows:

- FM NOTE commits FM route and FM owner;
- switch to 8-BIT hard-silences/clears old FM primary before 8-BIT ownership is committed;
- switch back to FM performs the inverse;
- unrelated tracks/routes remain unchanged;
- no frame leaves both families owning the same logical track;
- failed/malformed replacement remains fail-to-silence as inherited from D048.

## Required fixture C - event semantics across the real family seam

At minimum verify with actual engines:

- TIE/EMPTY do not allocate or switch family;
- RETRIG uses the currently routed family and current row patch/base state;
- OFF/CUT clears the global primary route while family-local release tails remain bounded;
- delayed NOTE uses the normal cross-family NOTE route;
- D056 FM p-locks remain applicable only to FM and do not alias into 8-BIT controls;
- 8-BIT p-locks remain non-applicable to FM.

## Regression and resource gates

WP5.9 must retain:

- exact all-8BIT SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`;
- D055 default/no-lock FM equivalence;
- D056 locked-FM semantics;
- D049-D056 inherited regression gates;
- no heap/float/int64/runtime-unbounded work in sample path;
- no sparse lock-store/editor reads in sample rendering;
- pinned JieLi target build;
- exact image/RAM/pool measurement and deltas from the D056 closeout.

Because the recommended path is test-only, target firmware resource values should remain exactly the D056 values. If they do not, the source boundary or build changed and closeout must fail until explained.

## Non-goals

WP5.9 does not implement:

- virtual-analog/subtractive synthesis;
- drums/percussion;
- a new engine family;
- song/order/persistence;
- worst-case performance benchmarking beyond correctness evidence;
- physical hardware margin claims.

The future VA/subtractive and drum directions remain explicit product context in `PROJECT_CONTEXT.md`; they are not abandoned because WP5.9 validates only the currently implemented families.

## Audit conclusion

**No production change is justified before the WP5.9 validation contract is frozen.**

The existing architecture appears structurally capable of real simultaneous-family operation, but the repository currently lacks direct evidence for it. The narrowest robust next step is therefore:

1. freeze a WP5.9 validation contract based on this audit;
2. add mixed-family observability to the host harness only as required;
3. implement simultaneous real-engine, same-track switch and event-semantics fixtures;
4. keep `firmware/src/**` and `firmware/gen/**` frozen unless those fixtures expose a real defect;
5. close with one fail-closed host + pinned-target checkpoint.

`hardware_margin_claim=none`.
