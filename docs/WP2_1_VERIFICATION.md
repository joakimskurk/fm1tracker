# WP2.1 Compile-Time Tracker Data Model Verification

Date: 2026-10-23

## Scope

WP2.1 introduces only the approved V0.1 compile-time tracker data model: constants, core record types, bounds-safe accessors, defaults and static size/budget assertions.

It deliberately does **not** allocate the pattern/song store and does not add transport, playback, STEP REC/REST behavior, tracker UI, p-lock allocation/editing, synth-engine behavior or persistence.

Phase 2 branch start / Phase 1 closeout commit:

`3ed4b65d7cda15c5831533f153f91e9014a9fde7`

Final verified implementation/workflow commit:

`3e922da4a33894bc938e3252e575a2908355f3a3`

GitHub Actions run:

`39985124933`

Job:

`120522185302` (`compile-time-model`)

Result: **SUCCESS**

## Implemented model

`firmware/src/fmt_model.h` defines the frozen V0.1 dimensions:

- 8 logical tracks;
- 64 steps per pattern;
- 16 pattern slots;
- 128 order entries;
- 16 instrument slots;
- maximum four parameter locks associated with one step;
- semantic lock-pool capacity 2,048 entries.

The model defines:

- note/sentinel values (`TIE`, `OFF`, `EMPTY`);
- instrument/volume inheritance sentinel `0xFF`;
- no-effect value `0`;
- 16-bit no-lock sentinel `0xFFFF`;
- `FmtStep`;
- `FmtPattern`;
- `FmtTrackConfig`;
- `FmtInstrumentHeader`;
- `FmtOrderEntry`;
- `FmtSongMeta`;
- non-instantiated `FmtSong` aggregate for compile-time sizing/access;
- null-safe and bounds-safe pattern/step accessors;
- deterministic empty-step initialization.

Raw compiler structs remain an in-memory representation only. They are not a persistent project/file format.

## Compile-time sizes

Host tests and compile-time assertions verified:

| Type / budget | Size |
|---|---:|
| `FmtStep` | 8 B |
| `FmtPattern` | 4,096 B |
| `FmtTrackConfig` | 4 B |
| `FmtInstrumentHeader` | 4 B |
| `FmtOrderEntry` | 2 B |
| `FmtSongMeta` | 8 B |
| `FmtSong` core type | 65,896 B |
| conservative future lock-storage budget | 16,384 B |
| core + future lock budget | 82,280 B |
| editable-song pool target | 98,304 B |

The 16,384-byte lock number is a **budget reservation only** (`2,048 x 8 B`). It does not freeze a lock-record ABI, linked-list representation or allocator. Those decisions remain deferred as recorded in D018.

The budgeted model leaves 16,024 B inside the 96 KiB editable-song target for future instrument payload/other song-model data before that target would need to be revisited.

## Host model tests

`tests/run_model_tests.sh` compiled `tests/test_fmt_model.c` with:

`-std=c11 -O2 -Wall -Wextra -Werror`

Verified:

- exact dimensions/constants;
- exact structure sizes and `lock_head` layout;
- special note/inheritance/lock sentinels;
- deterministic empty-step defaults;
- null-safe accessors;
- first and last valid pattern/track/step access;
- rejection of pattern index 16+;
- rejection of track index 8+;
- rejection of step index 64+;
- conservative core + lock budget remains within 96 KiB.

Observed test marker:

`FMT_MODEL_TESTS_OK=1`

Observed size line:

`FmtStep=8 FmtPattern=4096 FmtSong=65896 lock_budget=16384 total_budgeted=82280`

## Platform boundary

CI compared the Phase 2 branch against the completed Phase 1 head.

Unchanged:

- `firmware/src/platform.c`;
- `firmware/hal/`;
- `firmware/loader/`;
- `tools/build.py`;
- `tools/fm1pkg_make.py`;
- `tools/fm1_install.py`;
- `tools/build_fm1tracker.py`.

The only modification to the existing baseline application file `firmware/src/dinghy.c` is one include of `fmt_model.h`. No model storage is instantiated and no runtime path uses the model yet.

CI marker:

`WP2_1_PLATFORM_BOUNDARY_OK=1`

## Host runtime regression

The existing host harness still runs unchanged.

Host WAV remains byte-for-byte identical to the Phase 1 reference:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

Visible identity remains `FM1TRACKER`.

CI marker:

`HOST_RUNTIME_UNCHANGED=1`

## Device firmware build and safety checks

The exact Phase 1 external build inputs were reused:

- JieLi toolchain package `jieli-linux-toolchains-20260730.1`;
- clang 4.0.1;
- official AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13`;
- resolved SDK commit `d179b4484759423312073f5fbb232501aa491047`;
- the three consumed SDK package inputs matched their frozen SHA-256 hashes.

`./build.sh --release 0.1` completed successfully.

Inherited safety checks remained green:

- `.ram_text`: 901 instructions, no calls;
- image within app-slot limit;
- normal RAM within limit;
- pool RAM within limit;
- direct register access remains HAL-only.

Package identity decoded as:

`FM-1_987`

## Runtime/resource delta versus Phase 1

WP2.1 is compile-time-only, so CI intentionally required the generated firmware and package to remain byte-identical to Phase 1.

| Resource | Phase 1 | WP2.1 | Delta |
|---|---:|---:|---:|
| app image | 55,088 B | 55,088 B | **0 B** |
| normal RAM (`.data + .bss`) | 12,368 B | 12,368 B | **0 B** |
| pool RAM | 30,720 B | 30,720 B | **0 B** |

Firmware hashes remain exactly the Phase 1 hashes:

- `fm1tracker.bin`: `7b313502f2ab222e6ecf19f970772a36f314bce7c880b505157d4d5a2e4dfbaf`;
- `fm1tracker-0.1.fwsc`: `a3afcc069e4a9fe161f8470f477a76720b411fc6f302ebcf862c272a9a683bc2`.

CI marker:

`WP2_1_RUNTIME_AND_BUDGET_BASELINE_UNCHANGED=1`

## Physical hardware status

No new runtime behavior was introduced by WP2.1, so physical FM-1 testing is not an exit requirement for this compile-time-only package.

Physical install/boot/audio/panel/update behavior remains at the existing project verification level: **NOT TESTED** because hardware is not currently available.

## Evidence artifact

GitHub Actions uploaded:

`wp2-1-compile-time-model-evidence`

Artifact ID:

`12514279312`

Artifact ZIP SHA-256 reported by Actions:

`55790b25b0891f874ea3ea736a65680c21780804ce82245e0b4e2c3138a3cc48`

The Actions artifact is temporary; this document is the durable verification record.

## WP2.1 exit decision

All WP2.1 exit criteria pass:

- V0.1 data model compiles on host: **PASS**;
- model compiles in the device firmware build: **PASS**;
- dimensions/structure sizes compile-time enforced: **PASS**;
- defaults and access bounds host-tested: **PASS**;
- model remains inside the documented editable-song budget: **PASS**;
- no UI/audio/transport/p-lock allocator/synth behavior introduced: **PASS**;
- Phase 1 host/runtime/package regression remains bit-identical: **PASS**;
- inherited device safety checks remain green: **PASS**;
- image/RAM/pool deltas captured: **PASS, all zero**.

**WP2.1 is COMPLETE.**

The next work package is **WP2.2 — bounds-safe pattern store**. It is not started by this closeout.
