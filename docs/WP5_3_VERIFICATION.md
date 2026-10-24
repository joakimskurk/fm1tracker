# WP5.3 Verification - D051 FM4 Ratio/Pitch Mapping

Status: **COMPLETE / PASS**  
Date: 2026-10-25  
Package: WP5.3 / issue #46  
Branch: `phase5/fm4-ratios`  
Normative contract: `WP5_3_FM_RATIO_CONTRACT.md`  
Pre-implementation evidence: `WP5_3_PREIMPLEMENTATION_EVIDENCE.md`

## Canonical checkpoints

- WP5.2 closeout / WP5.3 parent: `826a7dca7394d6e2c13f79fafa97bd05fb1986c8`
- D051 contract freeze: `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- green pre-implementation evidence: `35f583b8717e167376e1c6418f426d1e1f95ff5c`
- isolated ratio implementation: `7e975eee37ba166deb4d180645675ec9b30e5aa7`
- canonical clean verification head: `9c2b0a611b4e1d0c15c42aa8eb9259d368987340`

The only production firmware files added by WP5.3 are:

```text
firmware/src/fmt_fm_ratio.h
firmware/src/fmt_fm_ratio.c
```

`dinghy.c`, D038 pitch code, D049 operator code and D050 algorithm-routing code remain unchanged.

## Contract-first evidence

D051 was frozen before production ratio source existed.

Canonical source-absent gate:

- run `40138575267`
- job `121005262360`
- head `e3b1272a442091dd9cc140f3d6d357f6215438b5`
- artifact `wp5-3-d051-contract-freeze`
- artifact ID `12577928512`
- artifact SHA-256 `f6ff75f9dd2128f3d79cffd467ee1b88b1422a5daa788516387b7e9c50e19014`

This gate proved the 16-ratio table, exact Q8 meanings, clamp boundaries, malformed-input policy, all anchor vectors and the exhaustive 262,144-output reference before implementation.

## Canonical implementation verification

Final clean verification run:

- run `40139154497`
- head `9c2b0a611b4e1d0c15c42aa8eb9259d368987340`
- host job `121006986068` - SUCCESS
- target job `121006985928` - SUCCESS

The historical contract workflow was also made post-implementation-safe and passed on the same head:

- run `40139154713`
- job `121006986775` - SUCCESS

It preserves the canonical source-absent run rather than incorrectly attempting to re-prove source absence after implementation exists.

## Host evidence

Canonical host artifact:

- name `wp5-3-fm-ratio-host`
- artifact ID `12577038930`
- artifact SHA-256 `2b632171295a72854b08b6451b122b670dac81304323e4c86af0ed460d3cccb1`

Verified:

- production source boundary exactly two new ratio files;
- D038, D049, D050 and `dinghy.c` deltas zero;
- all 16 stable ratio IDs exact;
- invalid ratio ID resolves to UNITY;
- malformed base increment clamps to the canonical base maximum first;
- `map4` behavior exact;
- no loop, runtime division/modulo, int64, float/double, heap or external DSP dependency in production ratio source;
- exhaustive legal pitch/ratio mapping exact;
- inherited D050 algorithm hashes exact;
- inherited D049 operator/table hashes exact;
- inherited WP5.0 host application WAV exact.

Canonical D051 exhaustive result:

```text
pitches=16384
ratios=16
outputs=262144
bytes=1048576
intentional_ceiling_outputs=27615
sha256=5b7b862bde26a13d6e18f1133509d57a36e6eb1e998ff287f59916d4c179881c
```

Key marker:

```text
WP5_3_FM_RATIO_HOST_GATE_OK=1 d051=exact exhaustive=exact d050=exact d049=exact full_app_wav=wp5_0_identical
```

## Pinned PI32v2 object evidence

Toolchain: `jieli-linux-toolchains-20260730.1`

`fmt_fm_ratio.o`:

- text + rodata: **264 B**
- D051 ceiling: 1,024 B
- remaining to ceiling: 760 B
- object SHA-256: `aa5bd8b9e1afa72054af2effeefbf468cfbb8b2b8d978e028540a30dc162956d`

Measured sections:

```text
.text.fmt_fm_ratio_sanitize   10 B
.text.fmt_fm_ratio_q8         20 B
.text.fmt_fm_ratio_apply      68 B
.text.fmt_fm_ratio_map4       70 B
.rodata.cst32                 32 B
.rodata.fmt_fm_ratio_max_base 64 B
TOTAL                         264 B
```

## Full target identity

Pinned AC79 SDK commit:

`d179b4484759423312073f5fbb232501aa491047`

The isolated ratio primitive is not linked into the running application, so full target identity remains exactly WP5.0:

```text
image              72792 B
normal RAM         19676 B
pool RAM          113000 B
identity           FM-1_987
BIN SHA-256        a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA-256       cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

Key marker:

```text
WP5_3_TARGET_GATE_OK=1 ratio=pi32v2 full_app=wp5_0_identical hardware_margin_claim=none
```

Canonical target artifact:

- name `wp5-3-fm-ratio-target`
- artifact ID `12578054556`
- artifact SHA-256 `97a1a2f166c578a616d8f488e779b9905d630f20a2e5f1971542b8a2af0e29b3`

## Frozen D051 compatibility

Stable V1 ratios by ID:

```text
0  0.5
1  1.0          UNITY / invalid-ID fallback
2  1.25
3  1.4140625    exact 362/256
4  1.5
5  2.0
6  2.5
7  3.0
8  3.5
9  4.0
10 5.0
11 6.0
12 7.0
13 8.0
14 12.0
15 16.0
```

Additional frozen behavior:

- canonical base increment maximum `1293741834`;
- output increment ceiling `0x7fffffff`;
- precomputed per-ratio clamp boundaries;
- unsigned Q8 split multiply with round-half-up;
- no runtime division/modulo or 64-bit arithmetic;
- stateless four-output mapping;
- D051 ceiling prevents accidental oscillator-increment wrap/fold but is not an anti-aliasing claim.

## Exit result

WP5.3 satisfies its exit criteria.

WP5.4 may begin only as a new contract-first package. D051 ratio identities and arithmetic are compatibility inputs to later FM work and may not be silently reinterpreted.

No physical FM-1 was available. `hardware_margin_claim=none`.
