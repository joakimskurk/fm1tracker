# FM1Tracker Project State

This is the **single source of truth for current execution state**.

Read `START_HERE.md` first.

## Current phase

**Phase 5 - mixed-engine routing + FM4**

State: **IN PROGRESS - WP5.0 through WP5.5 complete; WP5.6 ACTIVE; D054 source-absent gate ACCEPTED - 2026-10-25**

Current active package: **WP5.6 - bounded FM4 envelope strategy**  
Active issue: **#49**  
Active branch: `phase5/fm4-envelope`  
Parent closeout: **WP5.5 / D053** at `216c7e34b6d761cadb683bd1fc356c216b98cc27`

Do not begin WP5.7 voice/router integration, WP5.8 tracker parameter pages, WP5.9 audible mixed integration or WP5.10 Phase 5 exit while WP5.6 is active.

## Completed Phase 5 baseline

- WP5.0 / D048 mixed-engine routing - COMPLETE.
- WP5.1 / D049 fixed-point FM operator - COMPLETE.
- WP5.2 / D050 curated four-operator routing - COMPLETE.
- WP5.3 / D051 integer ratio/pitch mapping - COMPLETE.
- WP5.4 / D052 operator level/modulation-index mapping - COMPLETE.
- WP5.5 / D053 bounded OP4 one-sample feedback - COMPLETE.

WP5.5 final closeout:

- head `216c7e34b6d761cadb683bd1fc356c216b98cc27`;
- run `40165352623`;
- host `121085156557` SUCCESS;
- target `121085156767` SUCCESS.

## D054 accepted preimplementation state

Audit: `docs/WP5_6_PRECONTRACT_AUDIT.md`.  
Normative contract: `docs/WP5_6_FM_ENVELOPE_CONTRACT.md`.  
Preimplementation evidence: `docs/WP5_6_PREIMPLEMENTATION_EVIDENCE.md`.

Accepted source-absent gate:

- head `151401f164a17c3459f965971f2855bb05831c4b`;
- run `40167537148`;
- job `121091550742` SUCCESS;
- artifact ID `12588534519`;
- artifact SHA `eaea6ae76ee55779f45b4e2692c226e0790e0ebeb22326a9e722359fe76eec36`.

Frozen D054:

- four independent per-operator ADSR envelopes;
- 16 stable control bytes total, A/D/S/R x 4;
- exactly 20 B persistent envelope state per future FM voice;
- Q15.16 level accumulator, max `0x7fff0000`;
- stable stage IDs OFF/ATTACK/DECAY/SUSTAIN/RELEASE = 0..4;
- per-sample update;
- hard retrigger from zero;
- release from current level;
- zero time = true instant transition;
- 256-entry immutable 32-bit A/D/R step table, exactly 1024 B;
- time-table SHA `6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025`;
- SUSTAIN mapping SHA `a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815`;
- exhaustive LEVEL composition SHA `196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b`;
- canonical 65,536-sample stress SHA `8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849`;
- maximum release reaches OFF after exactly `2701155` advances;
- PI32v2 object ceiling <= 2048 B text+rodata;
- zero new mutable global RAM;
- D049-D053 and `dinghy.c` remain unchanged.

## Full target identity at WP5.6 entry

WP5.6 remains isolated from the running application:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

No physical FM-1 is available. `hardware_margin_claim=none`.

## Current exact action

D054 implementation is now authorized only within:

```text
firmware/src/fmt_fm_envelope.h
firmware/src/fmt_fm_envelope.c
firmware/gen/fmt_fm_envelope_step_q15_16.inc
tools/gen_fmt_fm_envelope.py
```

Next:

1. generate the frozen 1,024 B step table from the exact integer D054 law;
2. implement the 20 B/16 B envelope primitive without changing D049-D053 or `dinghy.c`;
3. add production C tests matching all frozen D054 hashes/vectors;
4. compile isolated envelope object for PI32v2 and enforce <= 2,048 B text+rodata;
5. rebuild full target and require exact WP5.0 identity;
6. write `WP5_6_VERIFICATION.md` before WP5.7 begins.
