# WP3.1 Verification — stable parameter ID registry

Date: 2026-10-23  
Status: **COMPLETE**

GitHub issue: #19  
Phase 3 parent: #20  
Branch: `phase3/plocks-effects`

## Checkpoints

Phase 3 / WP3.1 start head:

`c3693d8c7bb3d5306794aa232d6d25bef50c6c99`

Final verified source/workflow checkpoint:

`5af8065a9c98884d22682075b600d9790cfa7940`

Full GitHub Actions verification:

- run: `40025790828`
- job: `120661526848` (`param-registry-closeout`)
- result: **SUCCESS**

Focused pre-closeout verification:

- first run `40025522363` failed only because `tee` opened `build/...log` before the pipeline-side test runner created `build/`; the exhaustive registry executable itself passed in that run;
- workflow ordering was corrected without changing registry code or expectations;
- focused rerun `40025671629` completed **SUCCESS** before the full gate was expanded.

## Frozen decision

D027 freezes parameter identity before p-lock storage exists.

`FmtParamId` is exactly one unsigned byte.

Special values:

- `0x00` = `FMT_PARAM_NONE`; never assigned/lockable;
- `0xFF` = `FMT_PARAM_INVALID`; never assignable.

Namespace ranges:

| Range | Namespace | WP3.1 state |
|---|---|---|
| `0x01..0x1F` | common voice | active namespace |
| `0x20..0x5F` | 8-BIT engine | reserved for Phase 4 |
| `0x60..0x9F` | 16-BIT engine | reserved for Phase 5 |
| `0xA0..0xBF` | track/mixer | active namespace |
| `0xC0..0xDF` | channel/send/effect | reserved for later Phase 3/engines |
| `0xE0..0xEF` | GLOBAL/master | reserved, no active WP3.1 IDs |
| `0xF0..0xFE` | future/reserved | unassigned |
| `0xFF` | INVALID | never assignable |

Range membership does not imply assignment. Only authoritative registry entries are lockable.

WP3.1 assigns exactly four semantic IDs:

| ID | Symbol | Label |
|---:|---|---|
| `0x01` | `FMT_PARAM_VOICE_LEVEL` | `VOICE LEVEL` |
| `0x02` | `FMT_PARAM_PITCH_FINE` | `FINE` |
| `0xA0` | `FMT_PARAM_TRACK_LEVEL` | `TRACK LEVEL` |
| `0xA1` | `FMT_PARAM_TRACK_PAN` | `PAN` |

Envelope, oscillator, wavetable/noise, FM-operator, send/effect and GLOBAL/master IDs remain deliberately unassigned.

`FmtStep.effect` remains a separate tracker-command identity domain.

## Implementation

Added:

- `firmware/src/fmt_param_registry.h`
- `firmware/src/fmt_param_registry.c`
- `tests/test_fmt_param_registry.c`
- `tests/run_param_registry_tests.sh`
- `tests/run_wp3_1_closeout_ci.sh`
- `.github/workflows/wp3_1_param_registry_verify.yml`

The registry is stateless. It owns no runtime object, allocator, song mutation or pool storage.

The authoritative `FMT_PARAM_REGISTRY(X)` list generates the assigned ID constants/metadata from one source list so lookup metadata cannot silently diverge from the intended numeric assignments.

Public helpers cover:

- namespace classification;
- assigned registry count/index access;
- exact ID lookup;
- assigned/lockable predicates.

Unassigned bytes inside a valid namespace return safe unassigned lookup results rather than inheriting accidental semantics.

## Exhaustive host verification

The registry test iterates all 256 possible byte values.

Frozen marker:

```text
FMT_PARAM_REGISTRY_TESTS_OK=1
id_bytes=1 assigned=4 exhaustive_values=256 none=0x00 invalid=0xFF global_reserved=0xE0_0xEF
```

Verified properties include:

- `sizeof(FmtParamId) == 1`;
- exact namespace endpoints and ordering;
- exactly four assigned entries;
- strict assigned-ID uniqueness/order;
- exact labels and flags for assigned entries;
- all 256 values classify into exactly the intended namespace;
- only the four assigned IDs are reported assigned/lockable;
- `NONE`, `INVALID`, all future engine IDs, all effect IDs and all GLOBAL/master IDs remain safely unassigned;
- out-of-range registry index access returns no entry.

## JieLi target verification

The new registry translation unit is intentionally not linked into the application runtime in WP3.1. WP3.2 will be the first package to consume the semantic IDs in actual p-lock data.

The closeout gate nevertheless compiles `firmware/src/fmt_param_registry.c` directly with the pinned JieLi target compiler:

- toolchain: `jieli-linux-toolchains-20260730.1`
- clang: `4.0.1`
- target: `pi32v2`

Marker:

```text
WP3_1_JIELI_REGISTRY_COMPILE_OK=1
```

This proves target-compiler compatibility without changing Phase 2 runtime behavior just to make the registry measurable.

## Inherited regression evidence

All inherited Phase 2 unit/full-app gates remain green in the same final run:

- data model/store;
- exact D020 transport;
- D021/D025 playback semantics;
- D022/D023 step/track input;
- D024/D025 STEP REC and special-event editing;
- D026 tracker-screen model;
- WP2.5 context host fixture;
- WP2.6 track-selector host fixture;
- WP2.7 STEP REC host fixture;
- WP2.8 NOTE OFF/audio host fixture;
- WP2.9 reviewed framebuffer/HOME-return fixture;
- WP2.10 direct Phase 2 integration test;
- WP2.10 full-app Phase 2 integration fixture.

Reviewed Phase 2 framebuffer hashes remain unchanged, including integrated tracker framebuffer:

`a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`

The frozen default host WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

## Device build and runtime cost

Pinned external build inputs remain:

- official AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13`;
- SDK commit `d179b4484759423312073f5fbb232501aa491047`;
- previously frozen SDK input hashes;
- package identity `FM-1_987`;
- HAL-only direct register access.

Final device resources are exactly the Phase 2 baseline:

| Resource | Phase 2 | WP3.1 | Delta | Free |
|---|---:|---:|---:|---:|
| app image | 59,252 B | 59,252 B | **0 B** | 522,312 B |
| normal RAM | 12,464 B | 12,464 B | **0 B** | 85,840 B |
| pool RAM | 96,616 B | 96,616 B | **0 B** | 247,448 B |

Markers:

```text
image_delta_vs_phase2=0
normal_ram_delta_vs_phase2=0
pool_delta_vs_phase2=0
WP3_1_CLOSEOUT_GATE_OK=1
```

Because the registry is not yet linked into runtime, the complete FM1Tracker artifacts are also byte-identical to the Phase 2 closeout:

- `fm1tracker.bin`: `34d0e8c4344e091704608e08357964e7ae0b18673dd0b93f36c4e5bcc2cb4c7c`
- `fm1tracker-0.1.fwsc`: `3cfd0a2da3836720d2f921b04f29e93c16ca83ab18ab7242c99f2c333d890ca7`

## Evidence artifact

GitHub Actions artifact:

- name: `wp3-1-param-registry-evidence`
- artifact ID: `12533180473`
- size: `1,109,349 B`
- ZIP SHA-256: `fa28342bf1fcdf06fb168c8c19b5459afdc6e515bdced044c1f9761efecaee4b`

## Scope proof

The final gate explicitly verifies the existing Phase 2 production paths remain unchanged from the WP3.1 start head, including:

- `dinghy.c`;
- core model/store/transport/playback/editor/display modules;
- linker script;
- HAL/loader;
- build tools/scripts.

WP3.1 changes only the new registry contract/module/tests and associated documentation/workflow.

No p-lock allocator/storage, lock editor, tracker-effect semantics, engine implementation, live-performance capture, persistence or hardware control-map change exists in WP3.1.

## Exit result

WP3.1 exit criteria are satisfied.

The stable ID layer is now ready for WP3.2 to define the actual bounded sparse p-lock record/storage model. WP3.2 must treat D027 IDs as semantic identity and must not renumber them or silently turn namespace membership into assignment.
