# WP4.6 Common Envelope Verification

Date: 2026-10-24
Package: WP4.6 / issue #36
Branch: `phase4/8bit-engine`
Decision: D042
Contract: `docs/WP4_6_ENVELOPE_CONTRACT.md`

## Result

**WP4.6 is verified COMPLETE at host and pinned target-build level.**

The common Phase 4 envelope exists as an isolated deterministic linear ADSR primitive. It is not linked into `dinghy.c`, the four D038-D041 oscillator primitives remain isolated, and the running full app remains byte-identical to the WP4.5/Phase-3 proof-audio baseline.

Physical FM-1 install/audio/execution-margin verification remains a later integrated hardware gate because no physical FM-1 is currently available.

## Frozen checkpoints

- D042 contract: `983757fc3fc48d884ade9b9fdb59df0a1d070f9c`
- D042 decision/status freeze: `ab657264c7e00a6e4b460c2d5bbb52948c18ff15`
- clean pre-source checkpoint: `9ba6709d43803a0c8faa8c86d2d1f90b2c80f1f8`
- envelope implementation checkpoint: `fb4e5511b95812555302c6f7a00256ec842e7c38`
- permanent focused-verification workflow: `2508319722eb35a77551d71a52640a46eccbd8ec`
- focused-test correction only: `b4f4a36232f2d7f6019501be59fb20afcdf848c1`
- permanent closeout workflow: `422227784c2caf04af751f5e5cea6161c675d304`

## Focused deterministic verification

Final focused run:

- run `40102268841`
- host job `120896678830` — **SUCCESS**
- PI32v2 job `120896678618` — **SUCCESS**

The host gate proves:

- `sizeof(FmtEnvelopeParams) == 4 B`;
- `sizeof(FmtEnvelope) == 20 B`;
- exact stage IDs IDLE/ATTACK/DECAY/SUSTAIN/RELEASE;
- all 256 D042 rate-control values against the frozen 16-entry base-rate table;
- all 256 sustain-control values against exact `(control * 257) >> 1` mapping;
- zero-time A/D/R edge behavior;
- NOTE hard envelope restart from zero;
- RETRIG envelope behavior exactly matching NOTE restart;
- TIE/EMPTY represented as no envelope call under D037;
- OFF/CUT release from the exact current level;
- monotonic ATTACK, DECAY and RELEASE;
- exact ATTACK -> DECAY transition on render 705;
- exact DECAY -> SUSTAIN transition on total render 1755;
- exact RELEASE -> IDLE transition on release render 3534;
- default A24/D0/S255 attack duration 133 rendered frames;
- default R72 full-scale release duration 1057 rendered frames;
- guard words around the exact 20-byte state;
- identical-state/input replay determinism;
- frozen long-run FNV-1a32 `40a44a2e`, final level zero / IDLE.

Static audit proves `fmt_envelope.*` has:

- zero render loops;
- zero runtime division/modulo in production source;
- zero float/double;
- zero runtime int64;
- zero allocation;
- zero tracker/store traversal;
- one 64-byte immutable rate table.

## Real PI32v2 compile evidence

The pinned JieLi compiler independently compiled `fmt_envelope.c`.

Target object:

- `fmt_envelope.o` SHA-256 `6c9a80691dfb805c656a6d9f74b3eb78fe2777895335f28f9717db819ac02ed8`

Focused target artifact:

- artifact ID `12562987277`
- ZIP SHA-256 `74fdc2b8c2020da8ff6981f457ba31d2d8e1215803ee9251e8e2e48eef11e59b`

## Final closeout

Final closeout run:

- run `40102339789`
- job `120896886645` — **SUCCESS**
- closeout artifact ID `12562569055`
- artifact SHA-256 `e901b335d2be4dd5f618e14199c3c43cbb83c319d7daef15c10c03e5e076b8c8`

The closeout reran focused D042 tests/static audit, recompiled the exact envelope object with the pinned PI32v2 compiler, rebuilt the complete host app, fetched and verified the pinned AC79 SDK inputs, and built/packaged the real FM-1 target firmware.

## Full-app identity remains unchanged

Because WP4.6 remains isolated, the complete running app is still exactly the WP4.5 baseline:

- app image: **66,960 B**
- normal RAM: **13,296 B**
- pool RAM: **113,000 B**
- image delta vs WP4.5: **0 B**
- normal-RAM delta vs WP4.5: **0 B**
- pool delta vs WP4.5: **0 B**
- default host WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`
- `fm1tracker.bin` SHA-256: `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256: `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity: `FM-1_987`

## Production source added

Exactly:

- `firmware/src/fmt_envelope.h`
- `firmware/src/fmt_envelope.c`

No allocator, steal policy, parameter page/cache, oscillator dispatch, tracker adapter migration or proof-sine replacement was pulled into WP4.6.

## Exit

D042 exit marker:

`WP4_6_D042_EXIT_OK=1 envelope=isolated deterministic=1 adsr=1 pi32v2=1 full_app_identical=1 physical_hardware_pending=1`

WP4.6 may therefore close. The only next Phase 4 package is **WP4.7 — deterministic voice allocation/stealing**, which must freeze its allocation/steal ownership and tie-break contract before production allocator source.
