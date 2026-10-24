# WP5.0 Verification - D048 Mixed-Engine Routing

Date: 2026-10-24  
Status: **COMPLETE at host/device-build level**  
Branch: `phase5/mixed-engine-routing`

## Canonical closeout

- production verification head: `813792fb25632b4d7acda787422e51f713fad21e`
- aggregate run: `40129822006`
- aggregate job: `120978955451`
- aggregate artifact: `12574707919`
- aggregate artifact SHA-256: `7a43d638fbf8e9ad8be9124b88645237459577c6cbf5fab8ea2661f100b898fa`
- pinned AC79 SDK: `d179b4484759423312073f5fbb232501aa491047`
- package identity: `FM-1_987`
- physical execution-margin claim: `hardware_margin_claim=none`

Canonical marker:

```text
WP5_0_EXIT_OK=1 representative_wav_sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9 default_wav_sha256=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076 image=72792 ram=19676 pool=113000 bin_sha256=a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125 fwsc_sha256=cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073 inherited=pass realtime=pass family_switch=pass song_meta=nongating fm_dsp=absent hardware_margin_claim=none
```

## What D048 changed

D047 superseded the historical project-wide 8-BIT/16-BIT architecture. WP5.0 implements the smallest bounded production migration required by that decision.

The verified production architecture is now:

```text
tracker row/track
    -> instrument reference
    -> published instrument family + engine identity
    -> fixed per-track family router
    -> family-local engine/allocator
```

The tracker itself is engine-agnostic. The instrument owns family and engine identity.

`FmtSongMeta.mode` remains physically present for compatibility/default-family metadata, but it is no longer allowed to gate valid playback or publication.

## Frozen representation

- `FmtStep` remains exactly 8 B.
- `FmtInstrumentHeader` remains exactly 4 B.
- compatibility family values remain `0 = 8-BIT/chip`, `1 = FM/digital`.
- published instrument identity bank is 16 x 4 B = 64 B, aligned and fixed-size.
- global primary family route is exactly 1 B per logical track = 8 B total.
- family dispatch is static and bounded. No runtime plugin registry or heap was introduced.
- the existing single 2,048 B row-page publication cache remains shared. No second family cache was added.

## Routing semantics verified

Focused production tests prove:

- different family identities can coexist across logical tracks in the router model;
- a same-track cross-family NOTE hard-silences and clears the old family before the new family is attempted;
- the new family route commits only after successful dispatch;
- unsupported FM-family dispatch in WP5.0 fails toward silence because FM DSP is deliberately not implemented yet;
- delayed NOTE uses the same family router as immediate NOTE;
- malformed/unknown family identity fails safely to silence;
- invalid 8-BIT engine identity fails safely to silence;
- OFF/CUT route through active ownership and clear primary ownership;
- TIE/EMPTY/RETRIG do not allocate a new family route;
- non-applicable 8-BIT p-lock data on a non-8-BIT row remains stored but is ignored, never reinterpreted;
- active cross-track primary steals remain exactly zero.

Canonical focused marker:

```text
WP5_0_FAMILY_SWITCH_OK=1 cross_family_order=hard_silence_clear_attempt_commit_on_success delayed_note=same_router malformed=safe nonapplicable_plock=ignored fm_dsp=absent active_cross_track_steals=0
```

## Song metadata non-gating proof

The actual integrated application was built twice with identical row/instrument data. The only change in the second build was startup assignment of `FmtSongMeta.mode` from 8-BIT to 16-BIT.

Both complete host renders were byte-identical:

- frames: `286464`
- SHA-256: `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

Canonical marker:

```text
WP5_0_POSTMIGRATION_SONG_META_NONGATING_OK=1 mode8_mode16_wav=byte_identical sha256=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076 frames=286464
```

## D046 regression equivalence

The complete first 8-BIT engine family remains the regression baseline.

WP5.0 re-ran the frozen D046 representative workload through the migrated production source:

- 8 logical tracks
- 64 representative rows
- 512 saturated lock owners
- 2,048 p-lock records
- all four 8-BIT engines
- D031/D032/D033/D044 pressure
- 4,096-row / 64-wrap long run
- concurrent D034 queue pressure/recovery
- repeat-identical output/state/action/allocation evidence
- zero active cross-track primary steals

Representative WAV stayed byte-identical to D046:

`33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9`

Normal full-app WAV stayed byte-identical:

`17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`

Canonical representative marker:

```text
WP5_0_REPRESENTATIVE_OK=1 tracks=8 rows=64 locks=2048 long_rows=4096 repeats=identical active_cross_track_steals=0 d046_wav=byte_identical
```

## Realtime audit

The D048 structural realtime audit proves:

- route access is O(1), one byte for the addressed track;
- published family/engine identity read is O(1), four bytes for the addressed instrument;
- audio does not traverse mutable song metadata to choose family;
- publication writes do not occur in the audio callback;
- no dynamic family registry exists;
- no per-track variable family loop exists;
- the shared 2,048 B page cache remains singular;
- inherited queue capacity remains 32;
- inherited 8-BIT render remains fixed at eight slots;
- row-boundary ordering remains authoritative over tick work;
- one transport-derived six-tick tracker clock remains authoritative;
- no heap entered the audited path;
- no float/double entered the audited path;
- no audited callback/realtime leaf gained 64-bit runtime types.

Canonical marker:

```text
WP5_0_REALTIME_AUDIT_OK=1 hardware_margin_claim=none
```

## Inherited regressions

All applicable Phase 1-4 component, context, UI, stress and timing regressions pass after migration.

```text
WP5_0_INHERITED_REGRESSIONS_OK=1 phase1_4=applicable contracts=pass obsolete_proof_sine=excluded
```

Historical proof-sine hashes that were already superseded by the linked engine are intentionally excluded. Historical D037-D046 evidence remains intact and authoritative for the checkpoints it verified.

## Target resource closeout

D046 baseline:

- image: 72,504 B
- normal RAM: 19,604 B
- pool: 113,000 B

D048 final:

- image: **72,792 B**, delta **+288 B**
- normal RAM: **19,676 B**, delta **+72 B**
- pool: **113,000 B**, delta **+0 B**

D048 hard incremental ceilings were +4,096 B image, +256 B normal RAM and +0 B pool. All are satisfied with substantial margin.

Final target artifacts:

- BIN SHA-256: `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125`
- FWSC SHA-256: `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073`

## Scope discipline

WP5.0 deliberately contains **no FM4 production DSP**.

The FM/digital family identity exists in the routing contract, but unsupported production FM dispatch fails safely toward silence. This keeps the router migration independently verifiable before synthesis complexity is added.

## Harness correction during closeout

Two aggregate attempts exposed a test-harness ordering issue, not a production regression. A generated song-mode probe copied `dinghy.c` into the shared `build/tests` directory. Because `dinghy.c` quote-includes production `.c` files, files left by earlier test suites could shadow production include resolution.

The final harness generates that translation unit in a dedicated empty directory. The standalone post-migration gate and the full aggregate gate are both green after this correction. No production firmware source was changed by the harness fix.

## Physical hardware boundary

No physical FM-1 is available.

This closeout proves host determinism, static/realtime structure, pinned target compilation, resources and firmware artifact identity. It does **not** prove physical install/boot behavior, audio quality, panel feel, screen feel or measured AC791N callback execution margin.

`hardware_margin_claim=none`

## Exit decision

**WP5.0 / D048 is COMPLETE.**

The common mixed-engine routing foundation is accepted. WP5.1 may now begin the FM operator primitive as a separate bounded package.
