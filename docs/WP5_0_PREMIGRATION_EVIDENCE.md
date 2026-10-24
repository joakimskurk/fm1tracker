# WP5.0 Pre-Migration Evidence

Date: 2026-10-24  
Decision: D048  
Package: WP5.0 / issue #43  
Branch: `phase5/mixed-engine-routing`

Status: **PASS - production migration is now allowed under D048**

## Canonical run

- workflow: `WP5.0 D048 pre-migration gate`
- head: `380ee4b7943de8878b93989a652bb3e34f01b170`
- run: `40126003645`
- job: `120967423106`
- conclusion: SUCCESS
- artifact: `wp5-0-d048-premigration`
- artifact ID: `12572841977`
- artifact digest: `sha256:0b7b5925584e0fa663769eece6e36191f83dc4687ebf2299919fcb4db3c02d87`

The first run, `40125949079`, was red only because the cache-definition grep also counted the `extern` declaration in the header. The behavioral gate probe and router model had already passed. The grep was narrowed to the actual top-level definition and the complete gate was rerun green.

## Production freeze

The pre-migration run begins by proving that the production tree is still byte-identical to the final Phase 4 production baseline:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

Marker:

```text
WP5_0_PREMIGRATION_PRODUCTION_FREEZE_OK=1 baseline=c93c39ad344760b4da0dfbc76829ca5fef886174
```

No WP5.0 production migration existed at the time this evidence was frozen.

## Obsolete global gate reproduced behaviorally

The probe extracts and compiles the actual `fmt_8bit_linked_row_context()` implementation from the frozen D046 production commit. It does not hand-copy or reinterpret that function.

The test creates one valid 8-BIT/PULSE instrument and changes only `FmtSongMeta.mode` from `FMT_MODE_8BIT` to `FMT_MODE_16BIT`.

Result:

```text
WP5_0_PREMIGRATION_GATE_REPRODUCED=1 valid_8bit=1 song_mode_only_change=blocked instrument_gate=present
```

Therefore the old project-wide song-mode gate is a real behavioral restriction, not only a documentation artifact.

The test separately restores song mode to 8-BIT and changes the instrument header family, proving that the per-instrument family gate is a separate existing restriction which D048 intentionally retains/generalizes.

Static source evidence also confirms exactly one old song-level condition in the frozen linked-parameter source:

```text
WP5_0_PREMIGRATION_SOURCE_GATE_OK=1 baseline_song_mode_checks=1
```

## D048 router model frozen before implementation

A test-only model freezes the required common router semantics without importing or implementing FM DSP.

Verified target shapes:

```text
WP5_0_PREMIGRATION_ROUTER_MODEL_SIZE_OK=1 bytes=8
WP5_0_PREMIGRATION_IDENTITY_MODEL_OK=1 instruments=16 bytes=64
WP5_0_PREMIGRATION_SHARED_CACHE_MODEL_OK=1 rows=512 bytes=2048 copies=1
```

Verified routing behavior:

```text
WP5_0_PREMIGRATION_MIXED_ROUTE_MODEL_OK=1 tracks=8 active_cross_track_steals=0
WP5_0_PREMIGRATION_SWITCH_ORDER_MODEL_OK=1 old_silence=before_new_note commit_on_success=1 failure_route=none
WP5_0_PREMIGRATION_NONNOTE_MODEL_OK=1 tie_alloc=0 retrig_alloc=0 off_clears_route=1
```

The model proves before production implementation that:

- different tracks may own different family routes simultaneously;
- cross-family NOTE ordering is old-family hard silence, route clear, new NOTE attempt, commit only on success;
- failed dispatch leaves the route `NONE`;
- unsupported family fails toward silence;
- TIE and RETRIG do not allocate or change family;
- OFF/CUT clears only the selected track's global route;
- unrelated track routes remain unchanged.

## Shared D044 cache starting shape

The physical starting source contains exactly one actual `Fmt8BitRowParamCache` definition and an exact 2,048-byte assertion:

```text
WP5_0_PREMIGRATION_SHARED_CACHE_SOURCE_OK=1 definitions=1 size_asserts=1 bytes=2048
```

D048 therefore enters production migration with the single-cache invariant frozen. The migration may generalize/reuse this storage but may not introduce a parallel per-family 2,048-byte row cache without a new decision.

## Final pre-migration marker

```text
WP5_0_PREMIGRATION_OK=1 production=frozen gate=reproduced router_model=frozen shared_cache=2048 fm_dsp=absent
```

## Consequence

D048 contract-first requirements are satisfied. WP5.0 may now begin the smallest bounded production migration.

The next production step is limited to common routing infrastructure and removal of the obsolete project-wide song-mode eligibility gate. FM4 DSP remains out of scope until WP5.0 closes.
