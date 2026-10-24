# WP3.3 Verification — hold-step + encoder p-lock editing

Date: 2026-10-24  
Status: **COMPLETE**

GitHub issue: #22  
Phase 3 parent: #20  
Branch: `phase3/plocks-effects`

## Checkpoints

WP3.3 start head:

`361c5efa308ade56a35fd3b50e6a77c9e3ee41bb`

Final verified source/workflow checkpoint:

`df5d7e07fc763909a141c99283cfd2be19cff847`

Final GitHub Actions verification:

- run: `40054322180`
- job: `120753812201` (`lock-editor-closeout`)
- result: **SUCCESS**

Focused editor checkpoint before full integration:

- run `40053621519`
- job `120751574105`
- result **SUCCESS**

Reviewed full-app screen checkpoint before closeout:

- run `40054002297`
- job `120752790488`
- result **SUCCESS**

## Frozen decision

D029 was recorded before WP3.3 editor source existed.

The prototype p-lock gesture is:

1. enter ordinary SEQ/pattern context;
2. press and hold one D022 white step key;
3. that key is selected and pattern 0 + selected track + resulting global step are snapshotted;
4. while that exact white key remains held, KNOB1..4 write/replace locks on the snapshot target;
5. releasing the owner key ends the hold immediately.

P-lock hold is unavailable in PERFORMANCE and STEP REC. Transport running/stopped state does not gate it because WP3.3 writes storage only and playback still does not consume locks.

While a hold is active:

- other white step keys cannot retarget the editor;
- black track selectors are suppressed;
- OCT page navigation is suppressed;
- PLAY may start/stop without cancelling the hold;
- HOME or successful STEP REC entry cancels the hold without a write.

If multiple white-key downs arrive in one poll, the lowest physical white key owns the gesture. A simultaneous black+white poll uses the track selected at the beginning of that poll and suppresses the black-key change.

## Relative encoder contract

The existing FM-1 HAL already reports signed accumulated detent steps through `fm1_enc_take()`.

WP3.3 therefore does not invent an analog pickup threshold. One physical detent is one relative editor step.

KNOB1..4 are drained on **every** main-loop poll:

- inactive deltas are discarded;
- deltas from the poll that begins a hold are discarded;
- deltas from a release/cancel poll are discarded;
- only a hold that was already stable before the poll may consume that poll's deltas.

This prevents old encoder movement from becoming a lock when a step is subsequently held and gives release/context cancellation precedence over same-poll encoder motion.

## Parameter mapping and editor values

D029 maps the four current D027 production-lockable IDs directly to KNOB1..4:

| Knob | Parameter | ID | First-lock reference |
|---|---|---:|---:|
| KNOB1 | VOICE LEVEL | `0x01` | 255 |
| KNOB2 | FINE PITCH | `0x02` | 128 |
| KNOB3 | TRACK LEVEL | `0xA0` | 255 |
| KNOB4 | TRACK PAN | `0xA1` | 128 |

This is UI mapping only. D027 parameter identity remains semantic and independent of encoder position.

WP3.3 freezes an **editor-normalized** range `0..255` for these four IDs:

- one detent = ±1;
- signed multi-detent movement is applied in one bounded operation;
- values saturate at 0/255;
- a missing lock starts from the parameter reference above, then applies the first non-zero delta;
- non-zero intent still creates an explicit lock when saturation leaves the value at the reference boundary;
- an existing current-ID lock above 255 is rejected as an editor-domain RANGE error rather than silently reinterpreted.

These values do not yet define dB, cents, pan law, engine coefficients or audio behavior. Those semantics remain WP3.5+/engine work.

## Implementation

Added:

- `firmware/src/fmt_lock_editor.h`
- `firmware/src/fmt_lock_editor.c`
- `tests/test_fmt_lock_editor.c`
- `tests/run_lock_editor_tests.sh`
- `tests/wp3_3_lock_editor_context.script`
- `tests/run_lock_editor_context_host.sh`
- `tests/run_wp3_3_closeout_ci.sh`
- `.github/workflows/wp3_3_lock_editor_verify.yml`

Updated:

- `firmware/src/dinghy.c`
- `tests/host.c`
- `docs/DECISIONS.md`
- `docs/CONTROL_MAP_V0.1.md`
- `docs/PROJECT_STATE.md`

`FmtLockEditor` is exactly 8 bytes of ordinary RAM:

- last value;
- active flag;
- owning physical white key;
- snapshotted track;
- snapshotted global step;
- last attempted parameter;
- last status.

The editor owns no pool allocation. It addresses D028 through bounded pattern/track/step coordinates and calls `fmt_lock_get()` / `fmt_lock_set()` only after D029 input intent has been resolved.

No WP3.3 API deletes or clears a lock.

## Storage-only behavior

WP3.3 intentionally does **not** apply p-locks to audio.

A successful lock edit may change only:

- the target step's `lock_head` when a first record is allocated; and
- the associated D028 lock record value/list state.

It does not change:

- note;
- instrument;
- row volume;
- tracker effect/effect value/flags;
- proof playback voice;
- manual audition voice;
- base instrument/track parameter state.

Existing-lock edits use D028 replacement in place and consume no additional pool record.

## Unit verification

Frozen unit marker:

```text
FMT_LOCK_EDITOR_TESTS_OK=1
state_bytes=8 pattern=0 knobs=4 value_domain=0_255 references=255,128,255,128
targets=64x8 snapshot=1 replace_in_place=1 pool_full=1 corrupt=1 range_reject=1
```

The unit suite verifies:

- exact 8-byte editor state;
- exact KNOB1..4 mapping and first-lock references;
- all 64 global step targets across the four D022 pages;
- all eight D023 selected tracks;
- black/invalid/PERFORMANCE begin rejection;
- zero delta creates no lock;
- first-lock initialization and clamping;
- explicit lock creation at a saturated reference boundary;
- replacement in place with unchanged used-count/head;
- non-lock row fields and neighboring rows remain unchanged;
- snapshotted target survives later mutation of cursor/page/track state;
- existing value >255 returns editor RANGE without mutation;
- D028 POOL_FULL propagates without mutation;
- D028 cross-owner corruption propagates as CORRUPT without mutation;
- minimal hold/success/error footer formatting.

D028's independent fifth-prevalidated-ID harness remains the proof of the storage STEP_FULL branch. With exactly four production D027 IDs and four KNOB lanes, a fifth unique lock is deliberately unreachable through the normal WP3.3 mapping.

## Full-app input verification

`tests/host.c` now supports deterministic relative encoder events:

```text
T enc ID DELTA
```

The host fixture drives the real unchanged app loop semantics rather than calling the editor module directly.

It verifies the following gesture sequence:

- stale KNOB1 movement before hold;
- SEQ entry;
- direct track selection;
- page navigation;
- white-step hold;
- same-poll encoder movement at hold start;
- later eligible KNOB1 edit;
- black track-key attempt during hold;
- OCT page-change attempt during hold;
- PLAY toggle during hold;
- release with same-poll encoder movement;
- normal track/page navigation after release;
- second hold of the same target;
- same-poll encoder movement at second hold start;
- in-place KNOB1 replacement;
- repeated focus-suppression attempts;
- PLAY resume while hold remains active.

The entire ordinary-SEQ fixture emits **zero MIDI audition events**.

Frozen marker:

```text
WP3_3_LOCK_EDITOR_CONTEXT_HOST_OK=1 midi_events=0 stale_begin_release_deltas=fixture focus_suppression=fixture
```

## Reviewed tracker framebuffer evidence

The two WP3.3 tracker screenshots were visually reviewed before their raw framebuffer hashes were frozen.

At 1000 ms the screen shows:

- `SEQ CUR19 STOP 120`;
- `P01 T2 PG2/4 17-24`;
- cursor still on row 19 despite black-key/OCT attempts;
- footer `K1 VOICE 254 LOCK`.

This proves that stale pre-hold movement and same-poll hold-start movement did not leak into the write: the first eligible `-1` movement from reference 255 produced 254.

At 3000 ms the screen shows:

- `SEQ CUR19 PLY15 120`;
- `P01 T2 PG2/4 17-24`;
- the same snapshotted target after PLAY stop/resume and focus-stealing attempts;
- footer `K1 VOICE 255 LOCK`.

This proves the release-poll `-10` movement was discarded and the subsequent stable-hold `+1` replaced 254 with 255 in place.

Frozen raw PPM SHA-256:

- 1000 ms: `224c0c05bdc29360e8829fd6f69ea21b68d38dc3a369e33d1373e461ea678fd0`
- 3000 ms: `95bbee218132222d18d4593d9e5427cc25c0efcd340ec768a3c9df2ec9751212`

Reviewed PNG SHA-256:

- 1000 ms: `60a246db09b29ec25fe384eac41805f7fc6529b30e1709ef756434594b05fee5`
- 3000 ms: `8116347e0330ca16d0e1d160f4490e6a0a14f82524dcd40192e6b385e18e508e`

Frozen marker:

```text
WP3_3_REVIEWED_LOCK_EDITOR_SCREENS_OK=1
```

## Inherited regression evidence

The final run keeps all inherited checks green:

- D027 parameter registry;
- D028 lock storage and fifth-lock storage harness;
- Phase 2 data model/store;
- exact D020 transport;
- D021/D025 playback;
- D022/D023 pattern/track input;
- D024/D025 STEP REC/special events;
- D026 tracker-screen model;
- WP2.5 pattern-context full app;
- WP2.6 track-selection full app;
- WP2.7 STEP REC full app;
- WP2.8 special-event/OFF audio fixture;
- WP2.9 reviewed tracker-screen hashes/HOME return;
- WP2.10 direct integration;
- WP2.10 reviewed full-app integration.

The existing Phase 2 reviewed framebuffers remain byte-identical, including:

`a993551ce922d90186080bf554b82477b4336e160a6b301c446f0d275569b6d7`

The default host WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

Frames remain exactly `286464`.

## JieLi/device verification

Pinned external inputs remain:

- JieLi toolchain package `jieli-linux-toolchains-20260730.1`;
- compiler clang 4.0.1 / PI32v2;
- official AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13`;
- SDK commit `d179b4484759423312073f5fbb232501aa491047`;
- previously frozen SDK package-input hashes;
- package identity `FM-1_987`.

`fmt_lock_editor.c` also compiles independently with the real pinned PI32v2 target compiler.

Device safety gates remain green:

- loader image 8,232 B at `0x01C0A800`;
- loader BSS end `0x01C0F400`;
- ota.bin 6,954 B;
- `.ram_text`: 901 instructions, no calls;
- direct register access remains HAL-only.

## Runtime symbols

Final linked symbols include:

```text
01c0b078 .bss  00000008 fmt_lock_editor_runtime
01c0b088 .bss  00000004 fmt_lock_store_runtime
01c20000 .pool 00010168 fmt_store_storage
01c30168 .pool 00004000 fmt_lock_store_storage
```

The D028 lock-record pool remains physically unchanged at 16,384 B. The D028 runtime control object moves only because the new 8-byte editor object occupies ordinary BSS before it; this is not a storage ABI or persistence address contract.

## Resource cost

WP3.2 baseline:

- image 59,388 B;
- normal RAM 12,468 B;
- pool 113,000 B.

Final WP3.3:

| Resource | WP3.2 | WP3.3 | Delta | Free |
|---|---:|---:|---:|---:|
| app image | 59,388 B | 61,716 B | **+2,328 B** | 519,848 B |
| normal RAM | 12,468 B | 12,476 B | **+8 B** | 85,828 B |
| pool RAM | 113,000 B | 113,000 B | **+0 B** | 231,064 B |

The normal-RAM delta is exactly the 8-byte `FmtLockEditor` runtime state. No new song/p-lock pool storage is allocated in WP3.3.

## Final artifacts

`fm1tracker.bin`:

`ee3daf66216e1f8ba4a04183bc36aca13236b693acd548a1dd8de90f48800d40`

`fm1tracker-0.1.fwsc`:

`422e8283025419ec1991f0e13d52321e7dc837fab8218c80e969c69101e97922`

Package identity remains `FM-1_987`.

## Evidence artifact

Final GitHub Actions artifact:

- name: `wp3-3-lock-editor-evidence`
- artifact ID: `12544667629`
- size: `1,127,569 B`
- ZIP SHA-256: `06e00463f14d4d09cea2573cea55b37ed7ba6eee74eb64209f4c4b1efeb20516`

## Scope proof

The final source/workflow compare from the WP3.3 start head changes only:

- D029/control/state documentation;
- new `fmt_lock_editor` module;
- minimal `dinghy.c` editor/input/footer integration;
- host relative-encoder support;
- focused/full-app tests and workflow.

The final gate explicitly proves the following protected production paths are unchanged from the WP3.3 start head:

- core model/store;
- D027 registry;
- D028 lock store;
- transport/playback;
- pattern/track cursor module;
- STEP REC module;
- tracker-screen model;
- platform implementation;
- linker script;
- HAL/loader;
- build tools/scripts.

No lock delete/clear UX, playback lock application, tracker-effect implementation, engine work, persistence or live-performance capture entered WP3.3.

## Hardware limitation

No physical FM-1 is currently available.

Therefore WP3.3 verifies software semantics, host panel behavior, reviewed framebuffers, target compilation and device resource placement, but does **not** claim:

- physical comfort of holding a white key while turning a knob;
- actual encoder/key scan feel;
- display update feel on the 240×240 panel;
- physical install/boot/audio behavior;
- measured AC791N main-loop/audio execution timing.

Those remain later hardware gates, especially Phase 7 p-lock ergonomics.

## Exit result

WP3.3 exit criteria are satisfied at the host/device-build level.

The next package is WP3.4: lock replace/delete UX. WP3.4 must preserve D027/D028/D029 guarantees and must freeze its delete/clear/browse gesture contract before source. Applying locks to playback/audio remains WP3.5+ and must not be pulled into WP3.4.
