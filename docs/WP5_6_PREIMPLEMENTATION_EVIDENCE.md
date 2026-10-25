# WP5.6 D054 Preimplementation Evidence

Date: 2026-10-25  
Package: WP5.6 / issue #49  
Branch: `phase5/fm4-envelope`  
Status: **ACCEPTED - D054 SOURCE-ABSENT GATE GREEN**

Normative contract: `docs/WP5_6_FM_ENVELOPE_CONTRACT.md`.  
Precontract audit: `docs/WP5_6_PRECONTRACT_AUDIT.md`.

## Accepted checkpoint

Contract/gate head:

```text
151401f164a17c3459f965971f2855bb05831c4b
```

GitHub Actions:

```text
run      40167537148
job      121091550742
status   SUCCESS
```

Artifact:

```text
name     wp5-6-d054-contract-freeze
ID       12588534519
SHA-256  eaea6ae76ee55779f45b4e2692c226e0790e0ebeb22326a9e722359fe76eec36
```

## Source boundary proven

At the accepted checkpoint there was no:

```text
firmware/src/fmt_fm_envelope.h
firmware/src/fmt_fm_envelope.c
firmware/gen/fmt_fm_envelope_step_q15_16.inc
tools/gen_fmt_fm_envelope.py
```

The accepted WP5.5 closeout `216c7e34b6d761cadb683bd1fc356c216b98cc27` remained unchanged for:

- `dinghy.c`;
- D049 operator;
- D050 algorithm router;
- D051 ratio mapper;
- D052 level/index controls;
- D053 feedback;
- generated D049 sine table.

## Frozen D054 model proven

The source-absent reference proved:

- four independent per-operator ADSR envelopes;
- exactly 20 B persistent envelope state;
- exactly 16 B envelope control model;
- stage IDs OFF/ATTACK/DECAY/SUSTAIN/RELEASE = 0/1/2/3/4;
- unsigned Q15.16 envelope state with full scale `0x7fff0000`;
- per-sample bounded update cadence;
- hard retrigger from zero;
- true instant zero-time ATTACK/DECAY/RELEASE behavior;
- release from current level;
- corrupt-stage and null-control fail-to-silence behavior.

## Frozen control maps

Time table:

```text
controls    256
payload     1024 B, little-endian uint32
SHA-256     6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025
```

SUSTAIN map:

```text
controls    256
SHA-256     a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815
```

Exhaustive D052 static LEVEL x envelope LEVEL map:

```text
pairs       65536
payload     131072 B
SHA-256     196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b
```

Maximum RELEASE control 255 from full scale reaches OFF after exactly:

```text
2701155 advances
```

## Frozen stress

Canonical four-operator 65,536-sample stress:

```text
ATTACK       [1, 32, 64, 0]
DECAY        [64, 96, 128, 0]
SUSTAIN      [255, 192, 128, 64]
RELEASE      [96, 128, 160, 0]
static LEVEL [32767, 24672, 16448, 8224]
note-off     before sample 32768
```

Serialized payload:

```text
786432 B
SHA-256 8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849
```

All four envelopes finish zero/OFF.

## Inherited regressions

The D054 gate reran `tests/run_wp5_5_fm_feedback_tests.sh`, which recursively re-proved:

- D053 feedback exact;
- D052 controls exact;
- D051 ratios exact;
- D050 algorithms exact;
- D049 operator exact;
- full host application WAV exact WP5.0.

## Implementation authorization

D054 production implementation is now authorized, but only within the frozen WP5.6 primitive boundary:

```text
firmware/src/fmt_fm_envelope.h
firmware/src/fmt_fm_envelope.c
firmware/gen/fmt_fm_envelope_step_q15_16.inc
tools/gen_fmt_fm_envelope.py
```

No D049-D053 or `dinghy.c` modification is authorized in WP5.6 primitive implementation.

Implementation must preserve:

- 20 B state;
- 16 B controls or less;
- exactly 1,024 B immutable step table;
- PI32v2 envelope object text+rodata <= 2,048 B;
- zero new mutable global RAM;
- full application exact WP5.0 while isolated.

No physical FM-1 is available. `hardware_margin_claim=none`.
