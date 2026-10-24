# WP3.10 Verification — D036 Phase 3 Worst-Case Tests and Exit Gate

Date: 2026-10-24
Status: **COMPLETE — Phase 3 host/device-build exit satisfied; physical FM-1 verification remains pending**

Normative test/exit plan: `docs/WP3_10_TEST_PLAN.md`

## Final closeout checkpoint

Final verified WP3.10 / Phase 3 workflow checkpoint:

`e90d0232c179ea92e2295171403697ceaf5e3ecf`

Final GitHub Actions:

- run `40084706845`
- job `120845144453` (`phase3-closeout`)
- result **SUCCESS**

Evidence artifact:

- ID `12556590742`
- size `3,706,469` B
- ZIP SHA-256 `c4f494716ec98507ca9af6fe59313c01116136deececa7ba84e24598e78b82ee`

Final closeout markers:

- `WP3_10_PRODUCTION_IDENTICAL_OK=1 baseline=354bf7d356774ad828f1f660bd456dbab7f0490d`
- `WP3_10_PHASE3_EXIT_OK=1 wp3_1_through_wp3_10=complete host_device_build=1 physical_hardware_pending=1`
- `WP3_10_CLOSEOUT_OK=1`

## Production result

D036 found **no production defect**.

The final firmware/source tree is byte-identical to the final WP3.9 verified production checkpoint:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

WP3.10 therefore adds **zero production-source delta**. It consists only of stress/verification tooling, evidence and closeout documentation.

This is an important Phase 3 result: the bounded assumptions made separately in D027–D035 held when exercised together under the declared worst cases.

## D028 full lock-pool stress

The focused D036 harness filled the complete D028 global sparse pool:

- 8 tracks × 64 steps × 4 locks = **2,048 records**;
- all 512 pattern-0 step owners exercised;
- next valid allocation at full pool rejected deterministically;
- replacement of every existing lock remained capacity-neutral;
- first/middle/last removals restored exactly one free record each;
- released records were reallocated and owners restored exactly;
- final deterministic lock-store state hash: `4f426945`.

Marker:

`WP3_10_LOCK_POOL_STRESS_OK=1 records=2048 owners=512 hash=4f426945`

## D031/D032 cache worst case

With four locks on every proof-lane row:

- all 64 D031 playback-parameter snapshots were rebuilt and checked;
- all 64 D032 effect-cache rows were populated and checked;
- audio-facing cache sizes remain exactly 256 B each;
- no mutable D028 traversal entered audio.

Marker:

`WP3_10_CACHE_STRESS_OK=1 rows=64 locks_per_row=4 playback_cache=256 effect_cache=256`

## D034 queue saturation, wrap and recovery stress

The queue stress repeatedly exercised full-capacity and wrap behavior:

- 2,048 full 32-event queue cycles;
- 6,144 reject-newest overflow attempts;
- alternating FIFO drain and O(1) consumer discard;
- sequence counters crossed unsigned wrap safely;
- post-storm normal NOTE ON/OFF recovery remained functional;
- final queue was empty and internally consistent.

Marker:

`WP3_10_QUEUE_STRESS_OK=1 cycles=2048 rejected=6144 final_write=65519 final_read=65519 overflow=5143`

Static audit independently confirmed:

- callback queue consumption remains capped at 32 events;
- overflow discard remains O(1);
- no spin/retry or IRQ-masking primitive was introduced.

## D035 maximum presentation/backlog stress

The presentation stress used all eight visible rows with four locks per row:

- maximum bounded inspection: 8 rows × 4 records = **32 lock-record inspections per sync**;
- initial ten dirty regions converged in exactly five main-loop polls at the two-region limit;
- eight simultaneously changed row regions converged in exactly four polls;
- held-target begin/end dirtied the target row and converged without stale state;
- final dirty mask reached zero deterministically.

Marker:

`WP3_10_SCREEN_STRESS_OK=1 visible_rows=8 locks_inspected_max=32 full_polls=5 row_backlog_polls=4 converged=1`

## Combined full-app stress

The combined host fixture overlapped the independent Phase 3 subsystems in one deterministic run:

- running tracker transport;
- D032/D033 effects across visible rows;
- D031 row locks;
- K1–K4 held-step p-lock editing;
- browse/delete state;
- page changes and HOME↔SEQ transitions;
- D035 dirty backlog;
- hardware/USB-MIDI audition through D034;
- two separate queue-overflow epochs plus normal recovery notes.

Results:

- repeated stress runs were byte-deterministic;
- queue/audition activity changed audio as expected but did not alter tracker framebuffers;
- p-lock/editor activity emitted no outbound audition MIDI;
- both overflow epochs recovered completely;
- after recovery, the stress WAV suffix returned byte-identically to control;
- no stale held target or stuck audition note remained.

Frozen combined evidence:

- control WAV SHA-256 `7bdc491d929013bbf0e95005c95d4a21ac500960d418ba4ca0812a59991624ee`
- stress WAV SHA-256 `d471a5fa3a9fcf4f7a826b8afb8c13ba5c2b15a8d8df7a28ec96889b4f711c83`
- 1000 ms framebuffer SHA-256 `09e2041ae6f9133a0e1401d2b9da08797515c70dac1eb8ea46503cb1f9ea8cd5`
- 3000 ms framebuffer SHA-256 `c0362e8ca91d36240682c8c0845d4ac739fb14015601def788a6f4d6825ee916`
- frames `141120`.

Marker:

`WP3_10_COMBINED_STRESS_OK=1 deterministic=1 two_overflow_epochs=1 recovery=1 midi_neutral=1 screen_queue_neutral=1`

## D032/D033 timing matrix and long-run clock proof

The D036 timing matrix covered:

- all 256 byte values for each D032 command ARPEGGIO, SLIDE_UP, SLIDE_DOWN, PORTAMENTO and VIBRATO;
- all 256 byte values for D033 RETRIG, NOTE DELAY and NOTE CUT;
- values above 5 for D033 remain non-wrapping no-ops;
- no tick 6 action exists;
- 4,096 row transitions / 64 complete pattern wraps;
- 20,480 effect ticks;
- final transport step 0 / phase 0;
- exact row-boundary precedence.

Markers:

- `WP3_10_LONG_TICK_MATRIX_OK=1 rows=4096 wraps=64 frames=22579200 effect_ticks=20480 actions=512 final_step=0 final_phase=0`
- `WP3_10_TIMING_MATRIX_OK=1 d032_commands=5 values_each=256 d033_commands=3 values_each=256 no_tick6=1 hash=364d0c2c`
- `WP3_10_TIMING_EXIT_OK=1`

The independent D020 six-hour transport test remains green. The exact 120 BPM effect-tick frame anchors remain:

`919, 1838, 2757, 3675, 4594`, with the next row at frame `5513`.

## Static real-time boundedness audit

The corrected fail-fast realtime audit proves:

- production source unchanged from WP3.9;
- no allocator use in the relevant Phase 3 runtime modules;
- audio callback contains no lock-store traversal or screen/display calls;
- D034 queue callback consumption remains capped at 32;
- overflow discard remains O(1);
- SPSC code contains no spin/retry or IRQ masking;
- D035 screen synchronization is presentation/main-only and uses bounded lock reads;
- relevant real-time units contain no `uint64_t`/`int64_t`, `float` or `double` runtime implementation;
- D032 retains a single clock domain derived exactly from D020: row phase `661500`, effect tick phase `110250`, six ticks per row.

Final markers:

- `WP3_10_NO_ALLOCATOR_OK=1`
- `WP3_10_AUDIO_OWNERSHIP_OK=1 no_lock_traversal=1 no_screen_calls=1`
- `WP3_10_QUEUE_BOUND_OK=1 callback_cap=32 overflow_discard_o1=1`
- `WP3_10_SPSC_STATIC_OK=1 spin_retry=0 irq_masking=0`
- `WP3_10_SCREEN_OWNERSHIP_OK=1 main_presentation_only=1 lock_reads_bounded=1`
- `WP3_10_INTEGER_REALTIME_OK=1 runtime_64bit_types=0 float_double=0`
- `WP3_10_SINGLE_CLOCK_OK=1 row_phase=661500 tick_phase=110250 ticks_per_row=6`
- `WP3_10_REALTIME_AUDIT_OK=1`

A prior audit-runner bug where `tee` could mask Python's nonzero exit was discovered before final closeout. The runner was changed to preserve the Python exit status, the brittle clock text-match was replaced by numeric macro parsing, and the corrected audit passed independently and again inside the final closeout.

## Inherited deterministic regressions

The final closeout reran the complete WP3.9 closeout before D036 stress. All authoritative Phase 2 and WP3.1–WP3.9 regressions remain green.

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

D032 frozen fixtures remain unchanged, including control `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`.

D033 timing/audio behavior remains unchanged.

D034 queue fixtures remain unchanged:

- control/FIFO `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- hardware `1b13d55424dd16e36c1820501dcf6b3232a3e1d3bfb357259f74e85a45858b18`
- MIDI `f095db1bee4fa26c6ca56d298b30402b0a43a4ae700b31bf52854943151cfc71`
- overflow `31c55a429e53edf84fe5b8bc634b8fab42b45e5ad744e0b6a4505655e3b92a2f`
- recovery `42207d5ee151619f525d4416f3e77b79fe2d9f0746bb5f111c88858a2ca32b93`

D035 reviewed framebuffers remain unchanged:

- active target `d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3`
- released rows `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`.

## Final resources and firmware artifacts

Because WP3.10 has zero production-source delta, the final Phase 3 device build is exactly the WP3.9 firmware build:

| Resource | Used | Limit | Free | Delta vs WP3.9 |
|---|---:|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B | +0 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B | +0 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`.

## Phase 3 exit conclusion

All WP3.1–WP3.10 host/device-build exit criteria are satisfied.

Phase 3 therefore closes with:

- stable parameter IDs;
- bounded four-lock-per-step sparse storage;
- hardware-native held-step editing and deliberate single-lock deletion;
- audible row p-lock playback;
- deterministic six-tick tracker pitch effects;
- deterministic retrig/note-delay/note-cut events;
- bounded SPSC main→audio audition queue with reject-newest and fail-to-silence overflow recovery;
- bounded lock/effect visual feedback;
- worst-case combined stress proving the declared boundaries interact safely.

The next package is **Phase 4 / WP4.1 — 8-BIT engine interface contract**. It must freeze the engine/voice/realtime interface and resource budget before any PULSE/NOISE/TRIANGLE/WAVETABLE-8 production source is written.

## Physical hardware limitation

No physical FM-1 is currently available. Phase 3 is complete at the host/device-build level, but this does **not** claim:

- physical installation/boot/audio behavior;
- key/encoder ergonomics;
- real LCD readability/refresh perception;
- physical sound quality;
- measured AC791N execution-time margin under simultaneous worst-case audio/UI/USB activity.

Those remain explicit later hardware gates.
