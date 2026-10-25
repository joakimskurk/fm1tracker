# WP5.5 Preimplementation Evidence

Status: **ACCEPTED SOURCE-ABSENT GATE**  
Date: 2026-10-25  
Package: WP5.5 / issue #48  
Branch: `phase5/fm4-feedback`

## Frozen checkpoint

D053 was frozen before any FM feedback production source existed.

- WP5.4 parent closeout: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`
- D053 contract commit: `d44e1ee0de2256178f92724656db96ac46bf4530`
- corrected standalone reference/gate commit: `10bf0e3bf8ef49253666bee999b2de95de10046d`
- workflow: `.github/workflows/wp5_5_contract_freeze.yml`
- run: `40164610821`
- job: `121082987705`
- conclusion: `success`
- artifact id: `12587966076`
- artifact name: `wp5-5-d053-contract-freeze`
- artifact digest: `sha256:0eb34eb28a92f69eeeb653ddae2661ca62d72e8eaf887a881e227fbe26c7e4b9`

Two earlier diagnostic runs were red because the new test harness contained a transcription typo in the inherited D050 DUAL2 expected hash. Production source was still absent. The accepted gate uses the exact normative D050 hash and first re-proves all four D050 4,096-sample hashes before any D053 checks.

## Source-absent proof

At the accepted checkpoint:

- `firmware/src/fmt_fm_feedback.h` was absent;
- `firmware/src/fmt_fm_feedback.c` was absent;
- no production source referenced `fmt_fm_feedback`;
- D049 operator source was unchanged from the WP5.4 closeout;
- D050 algorithm source was unchanged from the WP5.4 closeout;
- D051 ratio source was unchanged from the WP5.4 closeout;
- D052 controls source was unchanged from the WP5.4 closeout;
- generated D049 sine data was unchanged;
- `firmware/src/dinghy.c` was unchanged from the WP5.4 closeout.

Accepted marker:

```text
WP5_5_D053_PRODUCTION_ABSENT_OK=1 feedback_source=absent d049_delta=0 d050_delta=0 d051_delta=0 d052_delta=0 dinghy_delta=0
```

## D053 control and arithmetic proof

The accepted source-absent reference model proved:

- all 256 FEEDBACK byte values map exactly to Q14 with `(v << 6) | (v >> 2)`;
- the numeric mapping is intentionally identical to D052 INDEX while FEEDBACK remains a separate semantic parameter;
- every signed-16 history value `-32768..32767` crossed with every FEEDBACK byte is signed-32 safe;
- 65,536 history values x 256 controls = 16,777,216 exact combinations;
- positive maximum is `2147287044`, with signed-32 margin `196603`;
- negative minimum is `-2147352576`, with signed-32 margin `131072`.

Frozen hashes:

```text
FEEDBACK Q14 payload       cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7
full history/control       6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af
```

## One-sample OP4 proof

Canonical 4,096-render OP4 fixture:

```text
initial phase    0xD0E0F000
phase increment  0x0456789A
level_q15        18000
FEEDBACK         192
initial history  0
```

Accepted result:

```text
final OP4 phase  0x386A9000
final history    13151
output SHA-256   6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49
```

This proves the exact causal order: previous output is scaled into current phase modulation, D049 renders exactly once, and the current level-scaled OP4 output becomes next history.

## D050 composition and switch proof

The source-absent model first re-proved the four accepted no-feedback D050 hashes exactly, then used a test-only composition that changes only the first OP4 render to the frozen D053 rule.

D053 4,096-output hashes:

```text
STACK4    25f86f13f10413f5be3ff4b50160cafeac97d758933fb5d660225525a4eb9077
FAN3      02e8b4556fb26a4134a6c257f1030b03c6808b8da2e597351fd19ef6ba9920fa
DUAL2     f23d8e23e09ee1f4c4fd5d7247c125627b0624ac98fceb9bf98c8b1a6ff55a93
PARALLEL4 60ff90a19ccfc517213aa4fcce1a209240d546bbd3db5eecde9c695612501050
```

All four retain identical D050 phase advancement and finish with feedback history `13151`.

Algorithm-switch continuity was explicitly distinguished from the forbidden reset-on-switch model:

```text
continuous history SHA  5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67
reset-on-switch SHA     51f0959b5e6840ed9dacc2f56eb9c2ea8c501ef5cb32080d0290a181e1e235aa
```

D053 therefore retains feedback history across D050 algorithm changes.

## D051/D052 extreme stress

Each D050 topology passed 65,536 renders at the frozen high-increment D051 vector, D052 INDEX maximum and D053 FEEDBACK maximum.

```text
STACK4    d91af85e62f5791881a7b2b3bc560228ab820268c3fe7ca0762f9ae6d6922c0d
FAN3      1037755115ed94cc460da484851a8f86fc49f8d19b6b9e76f55fa00c9bd65406
DUAL2     9da3125c952f75280296266236fd7a61afe93f6816354ce145d4d433fc0c6e46
PARALLEL4 ed1c7ae3b47c83773e455757267da3692454d988e6c9e01dcba09acd02ea8d7a
```

Every output remained in D049 Q15 bounds and all algorithms finished with feedback history `2517`.

## Inherited proof

The accepted D053 gate re-ran the WP5.4 host gate, which recursively re-proves D052, D051, D050, D049 and the exact WP5.0 host application behavior.

Final accepted marker:

```text
WP5_5_D053_CONTRACT_FREEZE_OK=1 production=absent topology=op4_self delay=one_sample state_bytes=2 control=byte_q14 history_surface=exhaustive switch_history=continuous extreme_stress=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical hardware_margin_claim=none
```

## Permission to implement

This evidence authorizes only the D053 production surface:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

D049, D050, D051, D052, generated sine data and `dinghy.c` must remain unchanged in WP5.5. The feedback primitive remains isolated from the running application until a later FM integration package.

WP5.5 is not complete until focused C verification, static realtime/source audit, pinned PI32v2 object measurement, inherited regressions and exact WP5.0 full-target identity are all green.

No physical FM-1 was available. `hardware_margin_claim=none`.
