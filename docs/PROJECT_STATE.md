# FM1Tracker Project State

This is the **single source of truth for current execution state**.

A new chat/session should read `START_HERE.md`, then this file, `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`, `WP5_2_FM_ALGORITHM_CONTRACT.md`, `WP5_1_VERIFICATION.md`, `WP5_1_FM_OPERATOR_CONTRACT.md`, `WP5_0_VERIFICATION.md`, `D047_MIXED_ENGINE_ARCHITECTURE.md`, `CURRENT_DECISIONS.md`, `WORK_PACKAGES.md`, `EXECUTION_PROTOCOL.md`, `CURRENT_TEST_STATE.md`, `TEST_MATRIX.md`, and `ROADMAP.md` before changing code.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.2 D050 production routing committed, implementation verification pending - 2026-10-24**

Current work package: **WP5.2 - curated FM4 algorithm routing**  
Parent issue: **#42**  
Work-package issue: **#45**  
Current branch: `phase5/fm4-algorithms`  
WP5.1 closeout: `bc71289690c8f9bd36e9cbbb378e0df3de1b9f25`  
D050 contract: `f2d328f516bb66517d1e06d98aab1f3c09dcc68b`  
D050 green pre-implementation evidence checkpoint: `6ecdb64653dee06411eecd4e73571cf21b11c3d9`

Do not begin WP5.3+ while WP5.2 is active.

## Completed baseline

- Phase 1-3: COMPLETE.
- Phase 4 / D046 8-BIT family: COMPLETE.
- WP5.0 / D048 mixed-engine routing: COMPLETE.
- WP5.1 / D049 fixed-point FM operator: COMPLETE.

Canonical WP5.0 exit: run `40129822006`, job `120978955451`, target 72,792 B image / 19,676 B RAM / 113,000 B pool.

Canonical WP5.1 exit: run `40134823600`, host `120994003059`, target `120994002842`.

## D050 contract-first evidence

Source-absent gate is GREEN:

- run `40135979431`
- job `120997461479`
- artifact `12577609841`
- artifact SHA `d3465c39b17416d58c56d98c196a18ae1c4dbd6e07d2ab7cc4ca2b3992cab85`
- evidence `WP5_2_PREIMPLEMENTATION_EVIDENCE.md`

## Active production implementation

The isolated WP5.2 production implementation consists only of:

- `firmware/src/fmt_fm_algorithm.h`
- `firmware/src/fmt_fm_algorithm.c`
- `tests/test_wp5_2_fm_algorithm.c`
- `tests/run_wp5_2_fm_algorithm_tests.sh`
- `.github/workflows/wp5_2_fm_algorithm_verify.yml`

`dinghy.c`, D048 router and D049 operator source/data remain unchanged. The algorithm layer remains inaudible and unlinked from the running firmware.

D050 implementation must preserve:

- state exactly four D049 operators / 16 B;
- algorithms STACK4/FAN3/DUAL2/PARALLEL4 IDs 0..3;
- OP4 -> OP3 -> OP2 -> OP1 evaluation;
- every operator advances exactly once per sample;
- invalid algorithm -> STACK4;
- mod depth clamp 0..16383;
- exact edge mapping `sample * depth * 4`;
- modulo 2^32 modulation accumulation;
- sign-safe 1/2/4 carrier normalization;
- no loops/graph traversal/heap/float/int64/divide/modulo/store/router access in render;
- router object <= 2,048 B text+rodata;
- full app exact WP5.0 while isolated.

Frozen hashes:

- STACK4 `25dfde1bb29de2c70305d6abafee91fd7480e69eaa71fd1a3fbde3c1e6e855cd`
- FAN3 `8d9ae5310369e3d5a36bcf8c44d5e74049d3d6e109362a844be2796b26330653`
- DUAL2 `c4c31cabf4b8e121591c4c1e0923aee67523eaf7d5b60b60ca1a545c13fc2269`
- PARALLEL4 `d316c3e5467cbfc606c788f669a3b19dba13535969dcce3b67b2821f0977a882`

## Exact next action

Run the committed WP5.2 implementation workflow and require both host and target jobs green:

1. source boundary only adds `fmt_fm_algorithm.h/.c` under firmware/tools/gen;
2. focused vectors, long hashes, 65,536-render stress and algorithm-switch phase continuity pass;
3. static audit passes;
4. D049 and WP5.0/D046 inherited host gates pass;
5. isolated PI32v2 router object compiles and stays <= 2,048 B;
6. full target remains 72,792 / 19,676 / 113,000 B with exact WP5.0 BIN/FWSC hashes;
7. after green evidence write `WP5_2_VERIFICATION.md`, close #45 and only then activate WP5.3.

## Explicitly out of scope

Ratios/tuning, final level/mod-index mapping, feedback, envelopes, FM voice integration, FM p-lock pages and audible mixed FM are WP5.3+ work.

No physical FM-1 is available. `hardware_margin_claim=none`.
