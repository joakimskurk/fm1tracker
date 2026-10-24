# WP3.4 Verification — p-lock browse / single-lock delete UX

Date: 2026-10-24  
Branch: `phase3/plocks-effects`  
Issue: #23

## Result

**WP3.4 is verified complete at the host/device-build level.**

Verified source/workflow checkpoint:

`178ba57a19adc8e25106f1c63e0df2f16478fb33`

Final GitHub Actions:

- run `40064122343`
- job `120783658593` (`lock-delete-closeout`)
- result **SUCCESS**

Final evidence artifact:

- ID `12548328977`
- name `wp3-4-lock-delete-evidence`
- ZIP SHA-256 `e2da7f2e514cd107765cf8835f883e46bc32eef6e2eb9bc3cf8de721aac104d2`

WP3.4 start head was:

`8ffa2a3de00f89946aca37fa7015fd3940851c93`

The start-to-verified compare is 19 commits ahead and changes only the WP3.4 editor/input path, package docs, dedicated tests and workflow. D027 registry, D028 storage, the Phase 2 model/transport/playback/editor/display modules, platform/HAL/loader and build infrastructure remain protected from the package start.

## Frozen decision

D030 was recorded in `docs/DECISIONS.md` and aligned with `docs/CONTROL_MAP_V0.1.md` **before production source implementation**.

D030 keeps D029's ordinary-SEQ held-white-step target and adds bounded selection and one-lock deletion:

- target remains pattern 0 + snapshotted selected track + global step for the hold;
- SELECT encoder browses only existing locks in D028's strict ascending semantic parameter-ID order;
- browse clamps at first/last, never wraps;
- successful KNOB1..4 write selects that semantic parameter;
- EDIT only arms deletion of the current selected lock;
- only a later main-loop poll containing SEL can confirm;
- same-poll EDIT+SEL cannot delete;
- EDIT release, target/context exit, PLAY or unrelated focus/navigation intent cancels a pending arm;
- while armed, SELECT and KNOB1..4 deltas are drained but discarded;
- confirm revalidates the semantic parameter before one `fmt_lock_remove()` call;
- successful delete returns exactly one record to the D028 free list;
- selection then moves to the item at the deleted ordinal, otherwise the new last item, otherwise NONE;
- zero-lock, NOT_FOUND, BAD_ADDRESS and CORRUPT cases are bounded and non-mutating;
- no user-facing clear-all gesture exists in WP3.4;
- full per-row lock visualization remains WP3.9;
- locks remain storage-only; audio/playback still does not consume them.

D028 `fmt_lock_clear()` remains an internal verified storage primitive. It is deliberately not exposed as panel UX because at most four locks exist on one step and one-by-one deletion is the safer prototype before physical ergonomics are known.

## Editor state

`FmtLockEditor` grew from 8 B to a compile-time asserted **10 B** by adding only:

- selected semantic `FmtParamId`;
- delete-armed flag.

No D028 record, pool capacity, owner mapping, `FmtStep` layout or D027 parameter identity changed.

Final linked runtime symbol:

```text
01c0b078 .bss 0000000a fmt_lock_editor_runtime
```

The D028 allocator runtime remains 4 B but linker alignment moves it to:

```text
01c0b08c .bss 00000004 fmt_lock_store_runtime
```

## Unit/storage verification

The frozen WP3.4 unit marker is:

```text
FMT_LOCK_EDITOR_TESTS_OK=1
state_bytes=10 pattern=0 knobs=4 browse=select_sorted_clamped delete=edit_arm_then_sel_confirm clear_all=0
targets=64x8 snapshot=1 replace_in_place=1 delete_first_middle_last_only=1 free_recovery=1 neighbor_isolation=1 range_delete=1
```

Tests cover:

- all inherited D029 target/mapping/reference/replacement/range behavior;
- sorted browse independent of insertion order;
- clamped browse at first and last lock, including extreme signed deltas;
- zero-lock browse as explicit safe no-op;
- successful KNOB write selecting its semantic parameter;
- EDIT arm and cancellation without storage mutation;
- first, middle, last and only-lock deletion;
- exactly one recovered free record for every successful deletion;
- unchanged neighboring step/list state;
- current-ID lock values above the D029 `0..255` editing domain remaining identifiable as RANGE and still safely removable;
- delete race to NOT_FOUND without fallback mutation;
- BAD_ADDRESS and CORRUPT rejection without partial mutation.

D027 and D028 tests remain green, including exact 8-byte lock records, 2,048-record global capacity, owner checking, strict sorted lists, max-four-per-step enforcement, pool exhaustion/recovery and corruption isolation.

## Real full-app input verification

The new D030 full-app fixture goes through the real `dinghy.c` panel path and proves:

- SELECT motion before or during hold-start cannot leak into the new target;
- two locks can be created and browsed on a held target without MIDI audition;
- same-poll EDIT+SEL produces an armed state rather than deletion;
- encoder movement while armed is drained/discarded;
- EDIT release cancels without deletion;
- PLAY cancels an arm before transport handling and then performs its normal transport action;
- an unrelated physical note-key down cancels an arm without redirecting D029's held target;
- only a later valid EDIT -> SEL sequence removes the selected lock;
- the fixture emits **zero unintended MIDI events**.

Frozen marker:

```text
WP3_4_LOCK_DELETE_CONTEXT_HOST_OK=1 midi_events=0 same_poll_confirm=fixture play_cancel=fixture unrelated_key_cancel=fixture valid_delete=fixture
```

The historical WP3.3 hold/encoder full-app fixture also remains byte-for-byte visually stable:

```text
WP3_3_REVIEWED_LOCK_EDITOR_SCREENS_OK=1
WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1 midi_events=0 stale_begin_release_deltas=fixture focus_suppression=fixture
```

## Reviewed tracker feedback

The new D030 framebuffers were visually inspected before their hashes were frozen.

### 1000 ms — armed delete, not deleted

The tracker remains on `CUR19 / T2 / PG2`, while the footer reads:

```text
DEL K1 VOICE? SEL YES
```

This frame occurs after EDIT and SEL arrive in the same input poll and therefore proves the same-poll chord did **not** delete.

Raw PPM SHA-256:

`6ce56a09073b4d70ec5ff7001ae787966f17d6bf610f1db2e239df152e14205d`

PNG SHA-256:

`9ef8b51b7666e362f32f6d9f5abd57392c778198f09bc06e99ba8ca9067d091e`

### 3000 ms — one deliberate deletion

The target is still the same cursor/track/page and the footer reads:

```text
DELETED K1 VOICE
```

Earlier PLAY-cancel and unrelated-key-cancel attempts did not cause deletion; only the final later-poll confirmation did.

Raw PPM SHA-256:

`f0185637136d5b609ebae1adbfbba7a62989a9246cb63093e6263242040e6cc6`

PNG SHA-256:

`85de320da9fdbd2069c6b59d18a0173da9674877f7c00f3e90d2daf3388b6032`

## Inherited regressions

The final closeout gate runs and passes:

- Phase 2 model/store/transport/playback/pattern-input/STEP REC/tracker-screen unit tests;
- WP2.5 pattern-context host fixture;
- WP2.6 track-selection fixture;
- WP2.7 STEP REC fixture;
- WP2.8 special-event/OFF audio fixture;
- WP2.9 reviewed tracker-screen and HOME-return fixtures;
- WP2.10 direct integration and real full-app integration;
- WP3.1 parameter registry;
- WP3.2 lock storage;
- WP3.3 unit/full-app p-lock edit behavior and exact reviewed screen hashes;
- WP3.4 unit/full-app browse/delete behavior and reviewed screen hashes.

The protected production-path diff gate also confirms that WP3.4 did not alter D027/D028 storage semantics, Phase 2 core modules, HAL/platform, loader or build infrastructure.

## Audio regression

WP3.4 does not apply locks to playback/audio. The default proof render remains exactly:

- frames: `286464`
- SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

This is byte-identical to the frozen D021/WP2.4 baseline and the WP3.3 closeout.

The frozen WAV becomes the pre-p-lock-playback reference for WP3.5. If WP3.5 intentionally changes default audio, that requires an explicit D031 decision and replacement evidence rather than silently weakening this regression.

## Toolchain / device build

The final gate uses:

- JieLi package `jieli-linux-toolchains-20260730.1`;
- `clang 4.0.1` PI32v2 target compiler;
- official AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13`;
- exact SDK commit `d179b4484759423312073f5fbb232501aa491047`;
- the previously frozen SDK package-input hashes;
- existing `.ram_text` no-calls check;
- HAL-only register-access check;
- package identity `FM-1_987`.

The standalone `fmt_lock_editor.c` translation unit also compiles successfully with the real PI32v2 compiler before the complete link.

## Final resources

| Resource | WP3.4 used | Limit | Free | Delta vs WP3.3 |
|---|---:|---:|---:|---:|
| app image | 64,228 B | 581,564 B | 517,336 B | +2,512 B |
| normal RAM | 12,480 B | 98,304 B | 85,824 B | +4 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

The normal-RAM delta is +4 B even though the editor state itself grows by 2 B because the next 4-byte allocator runtime symbol moves to a later alignment boundary.

Final linked storage/runtime symbols:

```text
01c0b078 .bss  0000000a fmt_lock_editor_runtime
01c0b08c .bss  00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

The D028 lock pool remains exactly 16,384 B and the complete pool usage remains unchanged from WP3.3.

## Final device artifacts

`fm1tracker.bin` SHA-256:

`0f828e989888158b09cdb137f7e429f97c42f4c3e30d8d814abd349d9971ed42`

`fm1tracker-0.1.fwsc` SHA-256:

`de23d8ff997d60178b3b7b9e1490c90696b5c22f3660d187edaa40cf08598a57`

Package identity remains:

`FM-1_987`

The final run reproduces these exact hashes and exact resource/linker assertions rather than merely reporting them.

## Physical limitations

No physical FM-1 was available for WP3.4. This verification therefore does **not** claim:

- that holding a white key while browsing/deleting is physically comfortable;
- that EDIT -> SEL is the final discoverable deletion gesture;
- that button/encoder scan feel matches the host timing model ergonomically;
- that the tracker footer is equally readable on the actual display;
- physical install/boot/audio behavior;
- measured AC791N main-loop/audio execution timing.

Those remain Phase 7 and hardware gates. D030 is an accepted deterministic prototype, not the final physical UX freeze.

## Scope deliberately not entered

WP3.4 contains no:

- p-lock playback/audio application;
- user-facing clear-all action;
- arpeggio/slide/porta/vibrato;
- retrig/note-delay/note-cut;
- general event queue;
- full row lock visualization;
- synth engine;
- live-performance capture;
- persistence/file format.

WP3.5 is therefore the next package and must freeze D031 before any playback/audio source changes.
