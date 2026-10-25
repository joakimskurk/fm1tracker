# WP5.5 Verification - D053 Bounded OP4 Feedback

Status: **COMPLETE / PASS**  
Date: 2026-10-25  
Package: WP5.5 / issue #48  
Branch: `phase5/fm4-feedback`  
Normative contract: `WP5_5_FM_FEEDBACK_CONTRACT.md`  
Precontract audit: `WP5_5_PRECONTRACT_AUDIT.md`  
Preimplementation evidence: `WP5_5_PREIMPLEMENTATION_EVIDENCE.md`

## Canonical checkpoints

- WP5.4 closeout parent: `d3501eca952f3e903d4dcc4faab2851d439f7ad0`
- D053 contract freeze: `d44e1ee0de2256178f92724656db96ac46bf4530`
- accepted source-absent reference head: `10bf0e3bf8ef49253666bee999b2de95de10046d`
- accepted source-absent evidence docs: `7f4f1415f5cc79f4a806f298c1621da1d8ab0d74`
- D053 production primitive: `5c49620044b93aff5c2253d177cb05cc7bf1b3a9`
- canonical implementation verification head: `d1ddb4baab7fcb4934cbf98e46c968fcd5e1f45e`

The only production firmware files added by WP5.5 are:

```text
firmware/src/fmt_fm_feedback.h
firmware/src/fmt_fm_feedback.c
```

D049 operator, D050 algorithm, D051 ratio, D052 controls, generated sine data and `dinghy.c` remain unchanged from the WP5.4 closeout. D053 is not linked into the running application.

## Contract-first evidence

D053 was frozen before production feedback source existed.

Accepted source-absent gate:

- run `40164610821`
- job `121082987705`
- head `10bf0e3bf8ef49253666bee999b2de95de10046d`
- artifact `wp5-5-d053-contract-freeze`
- artifact ID `12587966076`
- artifact SHA-256 `0eb34eb28a92f69eeeb653ddae2661ca62d72e8eaf887a881e227fbe26c7e4b9`

The gate proved the complete 16,777,216 signed-int16-history x FEEDBACK surface, canonical OP4 behavior, all four test-only D050 compositions, algorithm-switch continuity, D051/D052 extreme stress and inherited WP5.4 regressions before implementation.

## Canonical implementation verification

Final implementation run:

- run `40164953532`
- head `d1ddb4baab7fcb4934cbf98e46c968fcd5e1f45e`
- host job `121083999893` - SUCCESS
- target job `121083999790` - SUCCESS

## Host evidence

Canonical host artifact:

- name `wp5-5-fm-feedback-host`
- artifact ID `12587379892`
- artifact SHA-256 `d57ad000b83e5aa7c155ff817a9feb74727998f7baa417ca73b11da735850408`

Observed focused markers:

```text
WP5_5_FM_FEEDBACK_C_STATE_OK=1 state_bytes=2 reset=zero retrigger=zero nulls=safe feedback_zero_tracks_history=1
WP5_5_FM_FEEDBACK_C_MAP_OK=1 controls=256 malformed=saturate q14=0..16383
WP5_5_FM_FEEDBACK_C_HISTORY_SURFACE_OK=1 history_values=65536 controls=256 pairs=16777216 min=-2147352576 max=2147287044
WP5_5_FM_FEEDBACK_C_OP4_OK=1 renders=4096 delay_samples=1 final_history=13151
WP5_5_FM_FEEDBACK_C_ALGORITHMS_OK=1 algorithms=4 renders_each=4096 op4_first=1 history=continuous
WP5_5_FM_FEEDBACK_C_SWITCH_OK=1 renders=4096 switches=3 reset_on_switch=0 final_history=13151
WP5_5_FM_FEEDBACK_C_EXTREME_OK=1 algorithms=4 renders_each=65536 output=q15_bounded final_history=2517
WP5_5_FM_FEEDBACK_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 tables=0 mutable_globals=0 deps=d049_only
WP5_5_FM_FEEDBACK_HOST_GATE_OK=1 d053=exact history_surface=exhaustive algorithms=4 switch_history=continuous extreme_stress=pass static_bounds=pass d052=exact d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical
```

## Frozen D053 payloads

Production C output matched the preimplementation reference hashes exactly:

```text
FEEDBACK Q14 map              cf791163cad612063084d0479cb00b8b86e4e2d19102c461bf548bcca4b69bc7
OP4 4096                      6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49
STACK4 4096                   25f86f13f10413f5be3ff4b50160cafeac97d758933fb5d660225525a4eb9077
FAN3 4096                     02e8b4556fb26a4134a6c257f1030b03c6808b8da2e597351fd19ef6ba9920fa
DUAL2 4096                    f23d8e23e09ee1f4c4fd5d7247c125627b0624ac98fceb9bf98c8b1a6ff55a93
PARALLEL4 4096                60ff90a19ccfc517213aa4fcce1a209240d546bbd3db5eecde9c695612501050
algorithm switch 4096         5ba3042bc21eacccc103de9da579aa1b5a4db2d78aa5541130b66b2fce75ac67
extreme STACK4 65536          d91af85e62f5791881a7b2b3bc560228ab820268c3fe7ca0762f9ae6d6922c0d
extreme FAN3 65536            1037755115ed94cc460da484851a8f86fc49f8d19b6b9e76f55fa00c9bd65406
extreme DUAL2 65536           9da3125c952f75280296266236fd7a61afe93f6816354ce145d4d433fc0c6e46
extreme PARALLEL4 65536       ed1c7ae3b47c83773e455757267da3692454d988e6c9e01dcba09acd02ea8d7a
```

The accepted source-absent exhaustive arithmetic payload remains:

```text
full signed-int16 history x 256 controls
6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af
```

## Frozen D053 behavior

Stable V1 feedback semantics:

```text
previous OP4 level-scaled sample
    -> FEEDBACK byte 0..255
    -> Q14 depth = (v << 6) | (v >> 2)
    -> signed phase offset = previous * depth * 4
    -> next OP4 D049 render
    -> current OP4 output becomes next history
```

Additional rules:

- topology is OP4 self-feedback only;
- delay is exactly one sample;
- state is exactly one signed 16-bit prior OP4 sample;
- malformed FEEDBACK values above 255 saturate;
- FEEDBACK=0 produces zero phase offset while history still tracks OP4;
- reset and retrigger clear history;
- D050 algorithm switches retain history;
- null feedback state produces an ordinary D049 render with zero phase modulation;
- null OP4 returns zero without mutating history;
- no feedback graph traversal, averaging or dynamic operator selection exists.

## Arithmetic bounds

Complete signed-16 history x all 256 FEEDBACK values is signed-32 safe:

```text
positive maximum  2147287044
positive margin       196603
negative minimum -2147352576
negative margin       131072
```

D051 ratio changes phase increment only and cannot increase the D049 Q15 sample bound used by D053.

## Pinned PI32v2 object evidence

Toolchain: `jieli-linux-toolchains-20260730.1`

`fmt_fm_feedback.o`:

- text + rodata: **130 B**
- D053 ceiling: 1,024 B
- remaining to ceiling: 894 B
- persistent state per future FM voice: **2 B**
- object SHA-256: `147c623c49fa8756f36937589d96f2da5139b6e3d1a09790e95d91c5f770256d`

Measured sections:

```text
fmt_fm_feedback_reset             8 B
fmt_fm_feedback_retrigger         8 B
fmt_fm_feedback_sanitize         10 B
fmt_fm_feedback_depth_q14        16 B
fmt_fm_feedback_phase_modulation 28 B
fmt_fm_feedback_render_op4       60 B
TOTAL                            130 B
```

Future D050 + D053 synthesis state before envelopes is therefore 18 B: 16 B D050 operator phases + 2 B feedback history.

## Full target identity

Pinned AC79 SDK commit:

`d179b4484759423312073f5fbb232501aa491047`

D053 remains isolated from `dinghy.c`, so the full target remains exactly WP5.0:

```text
image              72792 B
normal RAM         19676 B
pool RAM          113000 B
identity           FM-1_987
BIN SHA-256        a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA-256       cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

Observed target marker:

```text
WP5_5_TARGET_GATE_OK=1 feedback=pi32v2 full_app=wp5_0_identical hardware_margin_claim=none
```

Canonical target artifact:

- name `wp5-5-fm-feedback-target`
- artifact ID `12587649156`
- artifact SHA-256 `96183e229059cbc6ceb09b2057de5abf914d4d62ca0db89a8d1dea576ecb8645`

## Exit result

WP5.5 satisfies its technical exit criteria. A closeout-head rerun of the same verification workflow is required after this documentation checkpoint before issue #48 is closed and WP5.6 becomes active.

WP5.6 / issue #49 must begin as a new contract-first package. D054 does not exist yet. D053 is a frozen compatibility input and may not be silently reinterpreted by the envelope or later integration work.

No physical FM-1 was available. `hardware_margin_claim=none`.
