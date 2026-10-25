# WP5.4 Verification - D052 FM4 Level / Modulation-Index Mapping

Status: **COMPLETE / PASS**  
Date: 2026-10-25  
Package: WP5.4 / issue #47  
Branch: `phase5/fm4-levels`  
Normative contract: `WP5_4_FM_LEVEL_INDEX_CONTRACT.md`  
Pre-implementation evidence: `WP5_4_PREIMPLEMENTATION_EVIDENCE.md`

## Canonical checkpoints

- WP5.3 closeout / WP5.4 parent: `ed3a9765ba9e401f4d30f514a357dd0d592e6a7e`
- D052 contract freeze: `18ea7f9af97e32e3903c7bbf1b2fbeef6dcc762e`
- accepted source-absent evidence: `0511028c492211f4f466913816ec52809b2c9adf`
- D052 public API: `313b97e04a2128f61299d99efe77c5f5dbc3142b`
- D052 mapper implementation: `3100a23f899424c74f9d5b8cdeaa921065a24202`
- exhaustive focused test: `1db0c406b3262011fbe1e931489d481306370a75`
- host gate runner: `1e33c5385f5035d3191e5d822a1d89d4e2999abe`
- canonical implementation verification head: `8a283c9216e7f72bab0770f5b1a8a0255f9bd54d`

The only production firmware files added by WP5.4 are:

```text
firmware/src/fmt_fm_controls.h
firmware/src/fmt_fm_controls.c
```

`dinghy.c`, D049 operator source, D050 algorithm source and D051 ratio source remain unchanged. The D052 source is not linked into the running application.

## Contract-first evidence

D052 was frozen before production control-mapping source existed.

Canonical source-absent gate:

- run `40140119411`
- job `121009903030`
- head `18ea7f9af97e32e3903c7bbf1b2fbeef6dcc762e`
- artifact `wp5-4-d052-contract-freeze`
- artifact ID `12577269868`
- artifact SHA-256 `d147dfd682c10e9400a9f65533ab6cdb2dc9db85b872035fe75495ac799cef03`

This gate proved all 256 LEVEL mappings, all 256 INDEX mappings, the full 65,536-pair overflow surface, topology role separation and the D051 composition vectors before implementation.

## Canonical implementation verification

Final implementation run:

- run `40143482844`
- head `8a283c9216e7f72bab0770f5b1a8a0255f9bd54d`
- host job `121019875458` - SUCCESS
- target job `121019875309` - SUCCESS

## Host evidence

Canonical host artifact:

- name `wp5-4-fm-controls-host`
- artifact ID `12579074081`
- artifact SHA-256 `8f98edfc38e09a1972f5ca168ad3b8690508782ca14b31179496ec4edb27de2c`

Verified:

- production source boundary is exactly the two D052 control files;
- D049, D050, D051 and `dinghy.c` deltas are zero;
- `FmtFmMappedControls` is exactly 10 B;
- all 256 LEVEL mappings are exact;
- all 256 INDEX mappings are exact;
- malformed scalar input saturates to 255;
- null level input maps all four levels to silence;
- null output is a no-op;
- all 65,536 LEVEL x INDEX edge cases are signed-32 safe under the frozen D049/D050 edge arithmetic;
- three D051 composition vectors are exact;
- no loops, runtime division/modulo, int64, float, heap, tables or external DSP dependencies exist in production D052 source;
- inherited D051, D050, D049 and WP5.0 host regressions remain exact.

Canonical payloads:

```text
LEVEL + INDEX mappings SHA-256  26e55d6a5fcacb5f432ecc3620b41a1c18e23f277aea6a13cb72d988a1929b38
edge-safety SHA-256             cf642aee2276abf355a62b7fb0ff1d8ff496e49b9d0a3397a0acc88fff421de0
D051 composition SHA-256        e1349b6d834de9655c2417e4a7a5aabdf64bed6138d8026bec10e23a4465486c
```

Observed focused markers:

```text
WP5_4_FM_CONTROLS_C_MAP_OK=1 controls=256 struct_bytes=10 malformed=saturate null_levels=silence
WP5_4_FM_CONTROLS_C_EDGE_OK=1 pairs=65536 max=2147287044 int32_margin=196603
WP5_4_FM_CONTROLS_C_COMPOSITION_OK=1 vectors=3 bytes=78
WP5_4_FM_CONTROLS_STATIC_AUDIT_OK=1 loops=0 runtime_div=0 runtime_mod=0 int64=0 float=0 heap=0 tables=0 external_dsp_deps=0
WP5_4_FM_CONTROLS_HOST_GATE_OK=1 d052=exact exhaustive=exact edge_safety=pass d051=exact d050=exact d049=exact full_app_wav=wp5_0_identical
```

## Pinned PI32v2 object evidence

Toolchain: `jieli-linux-toolchains-20260730.1`

`fmt_fm_controls.o`:

- text + rodata: **108 B**
- D052 ceiling: 1,024 B
- remaining to ceiling: 916 B
- object SHA-256: `05867ebe120829d1e3b8bf6f0504f29d63e9643e4fa5eede4c6d858d31d25059`

Measured sections:

```text
.text.fmt_fm_control_sanitize  10 B
.text.fmt_fm_level_q15         16 B
.text.fmt_fm_index_q14         16 B
.text.fmt_fm_controls_map      66 B
TOTAL                         108 B
```

The implementation uses no persistent DSP state. The mapped transient struct remains exactly 10 B.

## Full target identity

Pinned AC79 SDK commit:

`d179b4484759423312073f5fbb232501aa491047`

The D052 mapper remains isolated from the running application, so full target identity is exactly WP5.0:

```text
image              72792 B
normal RAM         19676 B
pool RAM          113000 B
identity           FM-1_987
BIN SHA-256        a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125
FWSC SHA-256       cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073
```

Observed marker:

```text
WP5_4_TARGET_GATE_OK=1 controls=pi32v2 full_app=wp5_0_identical hardware_margin_claim=none
```

Canonical target artifact:

- name `wp5-4-fm-controls-target`
- artifact ID `12579672157`
- artifact SHA-256 `7b4d5e9d76555998ba3d099090f1e5ef592f775e25338f19b4c09de30b6bc01f`

## Frozen D052 compatibility

Stable V1 controls:

```text
OP1_LEVEL 0..255
OP2_LEVEL 0..255
OP3_LEVEL 0..255
OP4_LEVEL 0..255
INDEX     0..255
```

Frozen transforms:

```text
sanitize(x)   = min(x, 255)
level_q15(v)  = (v << 7) | (v >> 1)
index_q14(v)  = (v << 6) | (v >> 2)
```

Additional compatibility rules:

- four independent LEVEL values remain separate from global INDEX;
- INDEX is one global modulation-strength scale for D050 V1;
- carrier LEVEL is never fused with INDEX;
- PARALLEL4 ignores INDEX audibly because it has no modulation edge;
- null LEVEL input fails toward silence;
- no persistent state is owned by D052;
- later UI/p-lock presentation may differ but may not silently reinterpret these DSP bytes.

## Exit result

WP5.4 satisfies its exit criteria.

WP5.5 / issue #48 may begin only as a new contract-first package. D053 does not exist yet. D052 mappings are compatibility inputs to feedback and later FM work and may not be silently reinterpreted.

No physical FM-1 was available. `hardware_margin_claim=none`.
