# WP5.0 Mixed-Engine Routing Contract

Date: 2026-10-24  
Decision: **D048**  
Parent architecture: D047  
Package: WP5.0 / issue #43  
Parent phase: #42  
Branch: `phase5/mixed-engine-routing`

Status: **FROZEN BEFORE PRODUCTION MIGRATION**

## Purpose

WP5.0 migrates FM1Tracker from D045's historical project-wide 8-BIT playback gate to D047's per-instrument engine-family routing model.

This package is architecture/routing migration only. It must not implement FM4 operators, FM algorithms, feedback, ratios, FM envelopes or FM DSP.

The complete D046 all-8-BIT configuration remains the regression baseline.

## Frozen starting point

Production source starts from the final Phase 4 production checkpoint:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

The Phase 5 branch starts from documentation/closeout head:

`d56505ac15c233eb806c567b6a5944bccf02ec1e`

Canonical D046 verification:

- head `2379d3e7e52d1cefe9f845f8287ad0ccda8c019a`
- run `40124536375`
- job `120963022289`
- evidence `docs/WP4_10_VERIFICATION.md`
- representative WAV SHA-256 `33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`
- normal linked WAV SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

Final D046 target footprint:

- image 72,504 B
- normal RAM 19,604 B
- pool RAM 113,000 B
- remaining D037 margin: image 59,992 B / normal RAM 1,884 B / pool 16,384 B

No physical FM-1 is available. WP5.0 makes no physical execution-margin or physical-audio claim.

## Existing restriction that WP5.0 removes

D045 currently contains two separate eligibility checks for 8-BIT playback/publication:

1. `song->meta.mode == FMT_MODE_8BIT`
2. `song->instruments[instrument].mode == FMT_MODE_8BIT`

The second check is correct family routing. The first is the obsolete project-wide gate superseded by D047.

WP5.0 removes the first check from playback/editor/publication eligibility. It does not remove or weaken per-instrument family validation.

## Data model freeze

### Tracker rows do not change

`FmtStep` remains exactly 8 B.

`FmtStep.instrument` remains the row-level instrument indirection. `FMT_INSTRUMENT_INHERIT` semantics remain unchanged.

`FmtTrackConfig.instrument` remains the track default instrument.

No family or engine byte is added to a tracker row.

### Instrument header does not grow

`FmtInstrumentHeader` remains exactly 4 B:

```text
mode | engine | version | flags
```

For WP5.0, the physical/source field name `mode` is retained to minimize migration churn. Its current semantic meaning is **engine family**.

Frozen compatibility values:

- `0` = 8-BIT/chip family
- `1` = FM/digital family

WP5.0 may introduce family-named constants/accessors, but it must not renumber these values or change the 4-byte layout.

### Song meta mode becomes non-gating legacy metadata

`FmtSongMeta` remains exactly 8 B and its existing `mode` byte remains physically present.

D048 freezes its current meaning as:

**legacy/default-family metadata and initialization/import hint only.**

It must not determine whether a valid instrument may play, update, release, retrigger, publish parameters or be edited.

Runtime audio routing must produce identical results for the same instrument/row data regardless of `FmtSongMeta.mode`.

Phase 6 persistence may later rename, reserve or serialize this field explicitly, but that requires its own versioned persistence decision. WP5.0 does not create a persistence ABI.

## Instrument identity publication

Audio must not need the mutable song store to decide an instrument family/engine.

WP5.0 therefore freezes one aligned 32-bit publication word per instrument slot:

```text
16 instruments x 4 bytes = 64 bytes normal RAM
```

The word mirrors the 4-byte `FmtInstrumentHeader` identity atomically enough for the existing aligned 32-bit publication model.

Main/startup/editor context owns publication updates. Audio reads only the aligned published word.

Required properties:

- exactly 16 fixed words;
- naturally aligned 32-bit storage;
- no pointer publication;
- no heap/pool allocation;
- unsupported/malformed family or engine fails toward silence;
- `FmtSongMeta.mode` is not part of the published eligibility decision.

WP5.0 may retain version/flags in the word even if routing currently consumes only family+engine, because the storage cost is identical and it avoids inventing a second header representation.

## Global primary-route ownership

### Chosen representation

The global router owns exactly one family byte per logical tracker track:

```text
uint8_t family[FMT_TRACKS]
```

Frozen size target:

```text
8 tracks x 1 byte = 8 bytes normal RAM
```

`0xFF` means no owned primary route.

The router deliberately does **not** duplicate family-local voice handles.

### Alternatives rejected

| Representation | State cost | Decision |
|---|---:|---|
| family only, one byte/track | 8 B | **chosen** |
| family + duplicated local voice handle | 16 B | rejected: duplicates family-local ownership and can become stale |
| four-byte route identity per track | 32 B | rejected: instrument/engine already exist in row/header/engine state |
| reuse one D043 allocator globally for all families | at least existing 52 B plus semantic coupling | rejected: local voice handles and future family pool sizes are family-specific |

The global route table answers only one question: **which family currently owns this track's primary sequenced route?**

The selected family remains responsible for its own bounded local voice handle/allocation state.

## Primary ownership invariant

Across all compiled engine families:

- each logical track may have at most one owned primary sequenced route;
- a still-owned active primary belonging to another track may never be stolen;
- family-local detached release tails may continue only after primary ownership has been cleared;
- detached tails are bounded by that family's fixed pool and reclaim policy;
- a family change on a NOTE must never leave both old and new families owning the same track.

The global route byte is updated only after a family dispatch succeeds.

## Exact cross-family NOTE semantics

A NOTE resolves its requested instrument identity before family dispatch.

### Same family as current primary

Dispatch directly to that family. Family-local semantics decide whether the existing local voice is reused/reset.

For 8-BIT this preserves D045 exactly, including ordinary-NOTE phase continuity only when engine/instrument identity permits it.

### Different family from current primary

The old family's current primary is **hard-silenced/reset first**, then the route is cleared, then the new family's NOTE is attempted.

The old primary does not become a release tail during a cross-family NOTE switch.

Reason: D045 already treats engine/instrument identity change on an ordinary NOTE as replacement/reset rather than an OFF-style release-tail event. Cross-family switching preserves that replacement grammar and prevents two-family primary overlap.

If new-family dispatch fails or the family/engine is unsupported, the track remains safely silent with global route `NONE`.

### No current primary

Dispatch the NOTE to the requested family. Commit the route only if the family reports a valid owned primary.

## Exact non-NOTE routing semantics

### TIE / EMPTY / row updates

Route only to the currently owned family. They never allocate and never create a route.

### RETRIG

Route only to the currently owned family. RETRIG never changes family and never allocates.

### NOTE DELAY

When the delayed NOTE fires, use the exact normal NOTE routing procedure above, including cross-family replacement if required.

### OFF / CUT

Route release to the currently owned family, then clear the global primary route in the same event.

The family may retain its own bounded detached release tail under its existing policy.

### Malformed or unsupported NOTE

Fail toward silence for that track:

1. hard-silence any current primary family;
2. clear the global route;
3. do not alias to another family/engine.

This preserves D045's safe-silence principle.

## Static family dispatch

Engine-family dispatch is fixed and explicit at compile time.

Allowed shape:

- bounded `switch` on family ID for event/update operations;
- one explicit render contribution per compiled production family;
- one explicit quiet check per compiled production family.

Forbidden:

- runtime registration;
- dynamic plugin tables;
- heap allocation;
- unbounded family search;
- function-pointer mutation from main to audio;
- recursion;
- blocking/retry loops.

WP5.0 production has only the already-complete 8-BIT audio family. Family ID 1 is a valid identity namespace reserved for Phase 5 FM4, but production dispatch remains unsupported/silent until the FM family is explicitly integrated by later WP5 packages.

Mixed-family router semantics in WP5.0 are proven with a test-only bounded stub adapter, not by pulling FM DSP into this package.

## Engine-page publication

### One shared row cache, not one cache per family

D044 already consumes one aligned 32-bit engine page word per `(track,row)`:

```text
8 x 64 x 4 = 2,048 bytes normal RAM
```

D048 freezes this as the **single shared engine-page publication shape** for the currently resolved instrument on each row.

The word's internal meaning remains family-specific. The router only transports it to the selected family adapter.

A second 2,048-byte row cache per family is forbidden by default.

Reason: D046 leaves only 1,884 B inside the D037 normal-RAM ceiling. Duplicating the existing cache for one additional family would cost 2,048 B before any FM voice state exists and would already exceed that remaining margin.

WP5.0 must therefore generalize/reuse the existing physical row-cache storage rather than allocate a parallel FM cache.

### Family-specific resolvers

At main-context rebuild/edit time:

- common D031 parameters remain in the existing common publication path;
- D032/D033 timing/effect state remains unchanged;
- the row's resolved instrument family selects the family-specific page resolver;
- only semantic parameter IDs applicable to that family/page are consumed;
- non-applicable IDs remain stored but are ignored;
- an ID must never acquire a different meaning merely because another family is active.

Until FM parameter IDs/resolver exist, FM-family rows publish a safe zero/no-page word.

## Editor boundary

The held-step editor remains main-context only.

WP5.0 must make page/edit mapping family-aware without inventing FM controls:

- 8-BIT row context continues to use D044 mapping exactly;
- unsupported/not-yet-implemented family context reports no engine-page mapping;
- common D031 parameters retain their existing semantics;
- existing stored 8-BIT-specific p-locks are not deleted when a row later resolves to another family;
- they are simply non-applicable and ignored until an 8-BIT family context makes them applicable again.

## Timing invariants

WP5.0 does not change:

- D020/D021 transport timing;
- row-boundary next-frame ordering;
- D032's exact six-tick clock;
- D033 NOTE DELAY / RETRIG / CUT timing;
- track dispatch order 0 through 7;
- D034 audition queue ownership/overflow policy;
- main-only UI/screen ownership.

Family routing occurs inside the existing bounded row/tick event points.

No second engine clock or tracker scheduler is allowed.

## Render and quiet boundary

Per rendered frame, each compiled production family may contribute at most one bounded render call.

The top-level mixer sums those fixed family contributions using bounded signed arithmetic and explicit clamp behavior already required by D037.

WP5.0 itself must preserve the all-8-BIT sample stream exactly for the canonical D046 configuration. Because no second production family renders yet, the routing migration must not intentionally change the D046 WAV.

`quiet()` must require every compiled family and the D034 audition lane to be quiet before flash/settings work proceeds.

## Realtime ownership

Audio owns:

- global 8-byte primary-family route state;
- all family-specific engine/voice/allocator runtime state;
- D032/D033 runtime state;
- transport/playback runtime state.

Main owns:

- song/instrument edits;
- p-lock store/editor;
- family-specific base parameter edits;
- aligned instrument-identity publication writes;
- shared row-page publication writes;
- UI/screen state.

Audio must not traverse D028, scan song rows, call editor code, draw, allocate or perform unbounded work.

## WP5.0 resource budget

WP5.0 is routing infrastructure, not a synth engine. It must remain cheap.

Hard incremental ceilings versus the D046 final target:

- app image: **+4,096 B maximum**
- normal RAM: **+256 B maximum**
- pool RAM: **+0 B**

Therefore WP5.0 target totals must remain at or below:

- image 76,600 B
- normal RAM 19,860 B
- pool RAM 113,000 B

The intended new persistent normal-RAM state is only:

- 64 B instrument identity publication;
- 8 B global route-family state;
- zero additional row-cache bytes because the existing 2,048 B page cache is reused/generalized.

Any need to exceed these WP5.0 ceilings requires a new recorded decision before acceptance.

## Required pre-migration evidence

Before production source changes, tests/harness must demonstrate the current limitation and freeze the target semantics.

At minimum:

1. **Global-gate reproduction**  
   An 8-BIT instrument that is otherwise valid becomes ineligible today when only `FmtSongMeta.mode` is changed to 16-BIT. This proves the obsolete gate exists.

2. **Instrument-header authority target**  
   The post-migration expected behavior is frozen: the same valid 8-BIT instrument remains eligible regardless of song meta mode.

3. **Router-state pure tests**  
   Test-only family stubs prove tracks can hold different family IDs simultaneously, same-track family switch clears old ownership before committing new ownership, failed dispatch leaves route NONE, OFF/CUT clears route, TIE/RETRIG do not allocate, and no operation mutates another track's route.

4. **Size tests**  
   `FmtEngineRouter` target size 8 B and instrument identity publication exactly 64 B.

5. **Shared-cache contract**  
   Exactly one 2,048 B engine-page row cache exists after migration.

No FM oscillator/operator source is allowed in these tests.

## Required post-migration host evidence

WP5.0 cannot close until host tests prove:

- `FmtSongMeta.mode` has no playback/publication eligibility effect;
- valid 8-BIT instrument family/engine still resolves exactly;
- malformed family/engine fails safely;
- all 8 tracks can own family routes independently;
- test-only mixed-family coexistence is deterministic;
- same-track family switch ordering is old-silence -> clear -> new-note -> commit-on-success;
- OFF/CUT route clear and detached-tail semantics are deterministic;
- TIE/EMPTY/RETRIG never allocate/change family;
- delayed NOTE uses the same router path as immediate NOTE;
- one shared 2,048 B row cache is used;
- 8-BIT page/editor behavior remains D044-compatible;
- non-applicable engine IDs/p-lock IDs are ignored, never reinterpreted;
- complete D046 all-8-BIT representative workload remains deterministic;
- canonical D046 representative WAV stays byte-identical unless a real defect requires an explicitly recorded superseding decision.

## Required structural realtime audit

The WP5.0 closeout audit must prove:

- one global route-family byte read/write per affected track event at most;
- no family loop with runtime/unbounded length;
- fixed maximum eight track dispatches at row boundary;
- fixed maximum eight tick-due checks on a non-row-boundary running frame;
- family-local allocator scans remain their own fixed bounds;
- no song-row/lock/editor traversal in per-sample render;
- no heap/pool allocation in realtime;
- no float/double;
- no runtime 64-bit division;
- no spin/retry/blocking/IRQ masking introduced by the router.

## Required target evidence

Pinned build inputs remain:

- JieLi toolchain `jieli-linux-toolchains-20260730.1`
- AC79 SDK `d179b4484759423312073f5fbb232501aa491047`
- identity `FM-1_987`

WP5.0 closeout must record:

- image / normal RAM / pool RAM;
- delta versus D046;
- exact BIN and FWSC hashes;
- inherited regression result;
- canonical all-8-BIT WAV hash;
- router/publication structural markers;
- physical limitation statement.

No host elapsed-time result may be converted into an AC791N execution-margin claim.

## Required markers

Recommended machine-readable markers:

```text
WP5_0_PREMIGRATION_GATE_REPRODUCED=1
WP5_0_ROUTER_SIZE_OK=1 bytes=8
WP5_0_IDENTITY_PUBLICATION_OK=1 instruments=16 bytes=64
WP5_0_SHARED_PAGE_CACHE_OK=1 rows=512 bytes=2048 copies=1
WP5_0_SONG_MODE_NONGATING_OK=1
WP5_0_MIXED_ROUTE_STUB_OK=1 tracks=8 active_cross_track_steals=0
WP5_0_SWITCH_ORDER_OK=1
WP5_0_8BIT_REGRESSION_OK=1
WP5_0_REALTIME_AUDIT_OK=1
WP5_0_TARGET_OK=1 hardware_margin_claim=none
WP5_0_EXIT_OK=1 hardware_margin_claim=none
```

## Exit criteria

WP5.0 is complete only when all are true:

1. D048 is recorded before production migration.
2. Pre-migration tests reproduce the global song-mode gate.
3. `FmtSongMeta.mode` no longer gates valid per-instrument playback/publication/edit context.
4. Per-instrument family + engine is authoritative.
5. The 8-byte global router enforces at most one primary family route per logical track.
6. Cross-family NOTE replacement cannot leave two owned primaries.
7. Test-only family stubs prove deterministic mixed-family routing without importing FM DSP.
8. Exactly one shared 2,048 B engine-page row cache exists.
9. Instrument identity publication is exactly 64 B aligned normal RAM.
10. All applicable Phase 1-4 regressions pass.
11. The canonical D046 all-8-BIT workload/WAV remains regression-equivalent unless an explicitly proven defect requires a new decision.
12. Pinned target remains within the WP5.0 resource ceilings and exact artifacts are recorded.
13. `docs/WP5_0_VERIFICATION.md` records durable evidence.
14. Physical limitations are explicit: `hardware_margin_claim=none`.
15. No FM4 production DSP is included in WP5.0.

Only after these criteria pass may WP5.1 FM operator work begin.
