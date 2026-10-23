# WP1.5 Renamed Baseline Verification and Budget Capture

Date: 2026-10-23

## Scope

WP1.5 establishes the clean renamed FM1Tracker build as the reference baseline that all later tracker/synth work must consciously spend from.

No tracker, STEP REC, p-lock, song or new synth-engine behavior is part of this package.

WP1.5 start commit:

`57617e288972a476fd221ae379e7bb75853b7451`

Final verified implementation/workflow commit:

`14e57e071daa40ca09576c54720b384b6ada609b`

GitHub Actions run:

`39970562995`

Job:

`120471807329` (`renamed-baseline`)

Result: **SUCCESS**

A prior full-checkout run (`39970119880`, commit `6c55e03fef3c31b79ad204181609805b01dee93d`) also completed successfully with identical firmware/package hashes and resource numbers. The run above is the final-current-workflow reproduction on the branch head before documentation closeout.

## External build inputs

The pinned Dinghy upstream documentation identifies the official JieLi SDK source as:

`https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git`

Required release/tag:

`AC79NN_SDK_V1.2.1_2023-12-13`

The annotated tag resolves to SDK commit:

`d179b4484759423312073f5fbb232501aa491047`

CI shallow-cloned this exact official tag outside the FM1Tracker repository and verified the exact commit before building.

The three SDK package inputs already frozen by FM1Tracker provenance also matched their expected SHA-256 values:

| SDK input | SHA-256 |
|---|---|
| `cpu/wl82/tools/uboot.boot` | `4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4` |
| `cpu/wl82/tools/cfg_tool.bin` | `276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b` |
| `cpu/wl82/tools/cfg/eq_cfg_hw.bin` | `41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d` |

No SDK material is vendored into FM1Tracker.

The JieLi compiler/toolchain used by the verified run was installed by `tools/get_toolchain.sh`:

- package: `jieli-linux-toolchains-20260730.1`
- clang: 4.0.1

## Protected baseline check

CI compared the protected upstream-derived implementation against the WP1.4 start commit and reported:

`PROTECTED_BASELINE_UNCHANGED=1`

Protected paths remained unchanged:

- `firmware/src/dinghy.c`
- `firmware/src/platform.c`
- `firmware/hal/`
- `firmware/loader/`
- `tools/build.py`
- `tools/fm1pkg_make.py`
- `tools/fm1_install.py`

The FM1Tracker identity remains supplied by the thin wrapper/build layer introduced in WP1.4.

## Host baseline

`./tests/run_host.sh` completed successfully.

Verified:

- visible app identity contains `FM1TRACKER`;
- host WAV produced;
- two screen PNGs produced;
- host WAV remains byte-for-byte identical to the pristine WP1.3 audio baseline.

Host WAV:

- frames: 286,464
- SHA-256: `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

## Device firmware build

`./build.sh --release 0.1` completed successfully with the exact external SDK/toolchain inputs above.

Inherited Dinghy safety checks passed, including:

- `.ram_text`: 901 instructions, no calls;
- register access: HAL only (`src/`, `loader/` clean);
- image size within app-slot limit;
- normal RAM within limit;
- pool RAM within limit;
- no build-time prohibited-symbol/safety failure occurred.

Loader baseline:

- loader image: 8,232 B
- `ota.bin`: 6,954 B
- loader BSS end: `0x1c0f400`

## Reference resource budget

These are measured build outputs, not estimates.

| Resource | Used | Limit | Free | Used % |
|---|---:|---:|---:|---:|
| app image / code | 55,088 B | 581,564 B | 526,476 B | 9.47% |
| normal RAM (`.data + .bss`) | 12,368 B | 98,304 B | 85,936 B | 12.58% |
| pool RAM | 30,720 B | 344,064 B | 313,344 B | 8.93% |

The free/percentage columns are derived directly from the measured used/limit values above.

These values are now the Phase 1 baseline. Later work packages should record meaningful deltas when they consume code, normal RAM or pool RAM.

## Final firmware/package artifacts

| Artifact | Size | SHA-256 |
|---|---:|---|
| `build/fm1tracker.bin` | 55,088 B | `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf` |
| `build/fm1tracker-0.1.fwsc` | 610,110 B | `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2` |

The package was decoded after generation and verified to report:

`FM-1_987`

Temporary Dinghy-named intermediate artifacts produced by the inherited builder are renamed away by the FM1Tracker wrapper; final public build artifacts use `fm1tracker*` names.

## Audio callback timing status

The platform audio block is 256 frames. At 44.1 kHz the mathematically available callback period is:

`256 / 44100 = 5.804988662 ms`

This is **not a measured firmware render time** and must not be treated as CPU-headroom evidence.

No physical FM-1 is currently available, so actual hardware callback execution time/cycle usage remains **NOT TESTED**. Later DSP work should measure hardware execution time before relying on the nominal 5.805 ms period; the project target remains to stay comfortably below the callback deadline and preferably below half-budget for normal worst-case rendering where practical.

## Physical hardware status

Physical FM-1 install/boot/audio/panel/update verification: **NOT TESTED**.

This does not invalidate the Phase 1 build baseline, but hardware-only claims remain explicitly outside the verified level.

## Evidence artifact

GitHub Actions uploaded:

`wp1-5-renamed-baseline-evidence`

Artifact ID:

`12508847182`

Artifact ZIP SHA-256 reported by Actions:

`33bde0f2d4b50f39f54b77b17bf9de251189c730acc4c02daefa8fa75c4831aa`

The Actions artifact is temporary; this document is the durable baseline record.

## WP1.5 exit decision

All WP1.5 build/resource requirements that can be verified without physical hardware are complete:

- renamed host baseline reproducible: **PASS**;
- official external SDK provenance pinned and verified: **PASS**;
- renamed device firmware/package build: **PASS**;
- inherited safety checks: **PASS**;
- package identity: **PASS**;
- code/RAM/pool baseline captured: **PASS**;
- nominal callback period documented without misrepresenting it as measured execution time: **PASS**;
- physical hardware timing/install validation: **NOT TESTED**, explicitly deferred until hardware exists.

**WP1.5 is COMPLETE. Phase 1 is COMPLETE.**

The next work package is **WP2.1 — compile-time tracker data model**. No Phase 2 implementation was started as part of this verification package.
