# WP5.6 Verification - D054 FM4 envelopes

Date: 2026-10-25  
Package: WP5.6 / issue #49  
Branch: `phase5/fm4-envelope`  
Status: **TECHNICALLY COMPLETE - final closeout-head rerun pending**

Normative contract: `WP5_6_FM_ENVELOPE_CONTRACT.md`.  
Precontract audit: `WP5_6_PRECONTRACT_AUDIT.md`.  
Preimplementation evidence: `WP5_6_PREIMPLEMENTATION_EVIDENCE.md`.

## Source-absent acceptance

D054 was frozen before production source existed.

```text
head      151401f164a17c3459f965971f2855bb05831c4b
run       40167537148
job       121091550742 SUCCESS
artifact  12588534519
SHA-256   eaea6ae76ee55779f45b4e2692c226e0790e0ebeb22326a9e722359fe76eec36
```

The accepted gate proved the D054 reference model and recursively re-proved D053 through D049 plus the exact WP5.0 host WAV.

## Production source boundary

The only WP5.6 production/tool additions relative to WP5.5 closeout `216c7e34b6d761cadb683bd1fc356c216b98cc27` are:

```text
firmware/src/fmt_fm_envelope.h
firmware/src/fmt_fm_envelope.c
firmware/gen/fmt_fm_envelope_step_q15_16.inc
tools/gen_fmt_fm_envelope.py
```

Unchanged:

- `dinghy.c`;
- D049 operator;
- D050 algorithm router;
- D051 ratio mapper;
- D052 level/index controls;
- D053 feedback;
- D049 generated sine table.

The envelope primitive remains isolated from the running application.

## Frozen D054 result

Architecture:

- four independent per-operator ADSR envelopes;
- envelope Q15 level scales D052 static Q15 LEVEL before D049 render;
- fixed operator identity across every D050 algorithm;
- D053 OP4 feedback naturally consumes the level-scaled OP4 sample;
- D050 algorithm changes do not reset envelope state.

State/control model:

```text
FmtFmEnvelopeState     20 B
FmtFmEnvelopeControls  16 B
OFF/ATTACK/DECAY/SUSTAIN/RELEASE = 0/1/2/3/4
internal max Q15.16    0x7fff0000
```

D050 + D053 + D054 synthesis state before voice allocator metadata is 38 B per future FM voice.

## Deterministic mappings

A/D/R time table:

```text
entries    256 x uint32
bytes      1024
SHA-256    6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025
control 0  instant
control 1  approximately 1 ms
control255 approximately 61.3 s target domain
```

All 255 finite controls are unique and monotonic.

SUSTAIN uses the exact D052 LEVEL mapping:

```text
SHA-256 a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815
```

Exhaustive D052 static LEVEL x D054 envelope LEVEL composition:

```text
pairs      65536
payload    131072 B
SHA-256    196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b
full env   exact static-level identity
```

## Event and stage semantics

Verified:

- reset -> all zero/OFF;
- hard note retrigger from zero;
- ATTACK=0 true instant attack;
- DECAY=0 true instant decay;
- RELEASE=0 true instant off;
- finite release begins at current level;
- SUSTAIN holds exact target;
- maximum RELEASE control 255 from full scale reaches OFF after exactly `2701155` advances;
- corrupt stage fails to zero/OFF;
- null controls on mutating operations fail to reset/off silence;
- null reads/maps fail safely;
- malformed public scalar controls saturate.

Future WP5.7 sample ordering is frozen as event -> read envelope -> map D052 levels -> render D049/D050/D053 -> one post-render envelope advance.

## Canonical stress

Four independent ADSR envelopes, 65,536 samples, note-off before sample 32,768:

```text
ATTACK       [1, 32, 64, 0]
DECAY        [64, 96, 128, 0]
SUSTAIN      [255, 192, 128, 64]
RELEASE      [96, 128, 160, 0]
static LEVEL [32767, 24672, 16448, 8224]
payload      786432 B
SHA-256      8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849
final        all zero/OFF
```

The production C implementation generated the same frozen payloads as the independent preimplementation model.

## Realtime/static audit

Production envelope source passed:

```text
loops=0
runtime_div=0
runtime_mod=0
int64=0
float=0
heap=0
mutable_globals=0
external_dsp_deps=0
immutable table=1024 B
```

Top-level work is exactly four envelope advances plus four effective-level mappings per future active FM voice/sample.

## Canonical implementation verification

Implementation head:

```text
401e666d21fa8290c7c622603e06e97b166c6b3e
```

GitHub Actions:

```text
run     40167933064 SUCCESS
host    121092688126 SUCCESS
target  121092688295 SUCCESS
```

Artifacts:

```text
host ID      12588901443
host SHA     4452eec3d50127a392dad79246b54cf93883837f9a02b2af64848d129c76a202
target ID    12588804647
target SHA   c8eed7e762c3d58da18f0a3b5fbad28878b6da2e70181d22f2376ea7c3686f20
```

## PI32v2 measurement

Pinned target object:

```text
fmt_fm_envelope.o text+rodata  1680 B
frozen ceiling                 2048 B
remaining margin                368 B
rodata                          1024 B
object SHA-256                  22fa7937d7776fea37c8e758ca83ee0a3f5d6c506ebd4661a8290a4516e048d1
```

Measured sections:

```text
control_sanitize       10 B
sustain_q15            16 B
time_step              18 B
reset                   16 B
retrigger_one           52 B
retrigger               54 B
note_off_one            56 B
note_off                54 B
level_q15               28 B
effective_level         48 B
map_levels              96 B
advance_one            154 B
advance                 54 B
step table            1024 B
```

## Full target regression

Because WP5.6 remains isolated, the full target must stay exact WP5.0. It does:

```text
image       72792 B
normal RAM  19676 B
pool RAM   113000 B
identity   FM-1_987
BIN SHA    a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA   cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

Inherited D053/D052/D051/D050/D049 host regressions also remain exact.

## Closeout rule

WP5.6 is technically complete. Before #49 closes, the documentation closeout head must pass `.github/workflows/wp5_6_fm_envelope_verify.yml` again with both host and target green.

WP5.7 / issue #50 remains PLANNED until that closeout completes.

No physical FM-1 is available. `hardware_margin_claim=none`.
