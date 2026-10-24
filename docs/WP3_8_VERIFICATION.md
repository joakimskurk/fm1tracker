# WP3.8 Verification

Status: **COMPLETE**  
Date: 2026-10-24  
Package: WP3.8 / issue #27  
Branch: `phase3/plocks-effects`

## Final checkpoint

Production audition-queue integration:

`223fec91746a95cc003eabe7cd1540af75e5ed26`

Final clean-tree verified source/workflow checkpoint:

`5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`

Final GitHub Actions:

- run `40079824265`
- job `120830854974` (`runtime-queue-closeout`)
- result **SUCCESS**

Evidence artifact:

- name `wp3-8-runtime-queue-evidence`
- ID `12554234479`
- size `3,680,087` B
- ZIP SHA-256 `2bd3b62cbfe5eec8867066acbdcecc5221dc515afa525f3914dab04b3f8b219f`

## Architecture freeze

D034 was accepted before production source. The normative contract is `docs/WP3_8_QUEUE_CONTRACT.md`.

WP3.8 introduces a single-producer / single-consumer main-loop -> audio-ISR runtime ingress queue. It deliberately does **not** replace:

- D020/D021 sequencer timing;
- D031 row-local p-lock snapshots;
- D032 six-tick effect scheduling;
- D033 fixed-state RETRIG / NOTE DELAY / NOTE CUT scheduling;
- the D024 PLAY desired-state mailbox.

The first production consumer is the already-existing manual/audition NOTE ON/OFF path from physical keys and USB-MIDI.

## Runtime event and queue layout

`FmtRtEvent` is exactly 8 B:

```text
offset 0  type     1 B
offset 1  track    1 B
offset 2  value_a  2 B
offset 4  value_b  4 B
sizeof             8 B
```

WP3.8 assigns:

- `0x01` NOTE_ON;
- `0x02` NOTE_OFF;
- all other values remain unassigned/reserved for later explicit decisions.

`value_b` remains zero/reserved in WP3.8; this package does not silently define a timestamp format.

`FmtRtQueue` is exactly 268 B ordinary RAM:

- 32 events x 8 B = 256 B slot payload;
- producer-owned `write_seq` = 4 B;
- consumer-owned `read_seq` = 4 B;
- producer-owned `overflow_seq` = 4 B.

Each logical 8-byte event is published as two aligned 32-bit words.

## Ownership / publication

Main is the only producer. Audio is the only consumer.

Producer publication order:

1. inspect `write_seq` / `read_seq`;
2. reject if full;
3. write slot word 0;
4. write slot word 1;
5. compiler publication barrier;
6. publish `write_seq + 1`.

Consumer order:

1. inspect `read_seq` / `write_seq`;
2. return empty if equal;
3. compiler publication barrier;
4. read both slot words;
5. unpack event;
6. publish `read_seq + 1`.

The compiler-only barrier is implemented behind the hardware/compiler boundary in `firmware/hal/fm1_barrier.h`; no inline assembly was added under `firmware/src` or `firmware/loader`.

The final closeout proves that this is the **only** HAL-file difference from the verified WP3.7 checkpoint. All pre-existing HAL files remain unchanged.

## Capacity / sequence wrap

Queue capacity is exactly 32 events, intentionally enough for all 27 physical FM-1 note-key edges from one main-loop poll plus five entries of headroom.

Sequence counters are unsigned 32-bit monotonic modulo counters. Occupancy uses unsigned `write_seq - read_seq`, and slot selection uses `sequence & 31`.

Focused host tests explicitly cross `UINT32_MAX` and verify FIFO/depth semantics through wrap.

## Overflow semantics

Overflow policy is **reject newest**:

- the 33rd event is not written;
- all 32 unread events remain intact;
- `write_seq` is unchanged;
- `overflow_seq` increments once per rejected enqueue;
- there is no spin/retry loop.

Drop-oldest was rejected because it would require producer mutation of consumer ownership or overwrite unread data.

### Audition fail-to-silence recovery

Because reject-newest can otherwise reject a NOTE_OFF, audio tracks `overflow_seq`.

When a new overflow is observed:

- all four manual/audition voice gates are released through the existing click-safe amplitude glide;
- the compromised pending batch is consumer-discarded in O(1) by advancing `read_seq` to the published `write_seq`;
- normal future queue consumption resumes.

The verified failure mode is therefore silence, never a permanently stuck note.

## Audio integration

Before WP3.8, main-context `play()` / `stop()` selected and mutated manual audio voices directly.

After WP3.8:

- physical key NOTE ON/OFF enqueues runtime note intent;
- USB-MIDI NOTE ON/OFF enqueues the same event format;
- main no longer chooses a manual voice or writes its oscillator increment/gate/age;
- audio owns manual voice allocation and NOTE ON/OFF mutation;
- at most 32 queue events are consumed once per audio callback before frames are rendered;
- no queue loop exists inside the per-sample tracker render path;
- queue consumption itself emits no MIDI output.

`quiet()` now treats pending runtime events or unresolved overflow recovery as not quiet, so flash/settings work cannot overtake queued note intent.

WP3.8 timing is callback-boundary immediate FIFO ingress, not an intra-block/sample-accurate event scheduler. D033 remains responsible for its verified exact sub-row timing.

## Focused queue tests

Final host result:

`FMT_RT_QUEUE_TESTS_OK=1`

with:

`event_bytes=8 queue_bytes=268 capacity=32 usable=32 overflow=reject_newest wrap=1 discard_o1=1 malformed_reject=1`

The focused tests prove:

- exact event offsets/size;
- exact queue size/capacity;
- reset/empty semantics;
- FIFO payload round-trip;
- 32 successful enqueues and deterministic 33rd rejection;
- rejection does not mutate unread events;
- overflow counter behavior;
- consumer-only dequeue ownership;
- O(1) discard;
- `UINT32_MAX` sequence wrap;
- malformed NOTE helper rejection without capacity/overflow mutation.

## Full-app audio proof

Final result:

`WP3_8_QUEUE_AUDIO_BEHAVIOR_OK=1 fifo=1 hardware=1 midi=1 overflow_silence=1 recovery=1 queue_midi_neutral=1`

Six deterministic 39,680-frame fixtures are frozen:

- control: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- same-callback FIFO NOTE_ON -> NOTE_OFF: `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- physical hardware audition: `1b13d55424dd16e36c1820501dcf6b3232a3e1d3bfb357259f74e85a45858b18`
- USB-MIDI audition: `f095db1bee4fa26c6ca56d298b30402b0a43a4ae700b31bf52854943151cfc71`
- forced overflow fail-to-silence: `31c55a429e53edf84fe5b8bc634b8fab42b45e5ad744e0b6a4505655e3b92a2f`
- post-overflow recovery: `42207d5ee151619f525d4416f3e77b79fe2d9f0746bb5f111c88858a2ca32b93`

The forced-overflow fixture queues more than 32 NOTE ON events and intentionally supplies no matching NOTE OFFs. The render still settles back to the byte-identical tracker-only control, proving the fail-to-silence rule rather than merely testing a counter.

The recovery fixture then proves a later normal NOTE ON/OFF works after forced overflow.

## Inherited deterministic regressions

The final clean-tree gate re-ran and preserved:

- Phase 2 complete integration/device-build obligations;
- WP3.1 parameter registry;
- WP3.2 sparse lock storage;
- WP3.3 p-lock editing;
- WP3.4 browse/delete UX;
- WP3.5 D031 p-lock playback;
- WP3.6 D032 effect unit/timing/audio proofs;
- WP3.7 D033 event unit/audio proofs;
- historical package identity and device safety checks.

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

Every frozen WP3.6 and WP3.7 audio fixture remains byte-identical through WP3.8.

## JieLi / device evidence

Toolchain remains:

- `jieli-linux-toolchains-20260730.1`
- JieLi clang target `pi32v2`.

AC79 SDK basis remains:

- tag `AC79NN_SDK_V1.2.1_2023-12-13`
- commit `d179b4484759423312073f5fbb232501aa491047`.

`fmt_rt_queue.c` compiles independently with the real PI32v2 compiler:

`WP3_8_JIELI_QUEUE_COMPILE_OK=1`

Final linked symbols include:

```text
01c0b174 .bss 00000004 fmt_tracker_row_event_runtime
01c0b178 .bss 00000004 fmt_rt_queue_overflow_seen
01c0b17c .bss 00000100 fmt_playback_param_cache
01c0b294 .bss 0000010c fmt_rt_queue_runtime
01c0b3a4 .bss 0000000c fmt_tracker_effect_runtime
```

`fmt_rt_queue_runtime` is 4-byte aligned and exactly `0x10c` / 268 B.

Loader / device safety remains green:

- loader image 8,232 B at `0x01c0a800`;
- loader `ota.bin` 6,954 B;
- `.ram_text`: 901 instructions, no calls;
- register access remains HAL-only;
- package identity remains `FM-1_987`.

## Final resources

| Resource | WP3.7 | WP3.8 | Delta | Limit | Free |
|---|---:|---:|---:|---:|---:|
| app image | 65,940 B | 66,308 B | +368 B | 581,564 B | 515,256 B |
| normal RAM | 13,008 B | 13,280 B | +272 B | 98,304 B | 85,024 B |
| pool RAM | 113,000 B | 113,000 B | +0 B | 344,064 B | 231,064 B |

The +272 B normal-RAM delta is exactly the 268-byte queue plus the 4-byte audio-owned overflow acknowledgement. No pool allocation was added.

## Final firmware artifacts

- `fm1tracker.bin` SHA-256 `a1d422b0c533af7f713deca37191a45e438640e2fc1be7c2734c1e4539822251`
- `fm1tracker-0.1.fwsc` SHA-256 `b93d5dd57d4e23214ac4ef500e47d91b3e4528b620b646f8804312654df98b8a`
- package identity `FM-1_987`.

## Scope preserved

WP3.8 did **not** implement:

- live REC/capture;
- quantization/overdub/merge rules;
- intra-block/sample-accurate main-loop timestamps;
- D033 scheduling through the queue;
- multi-track pattern dispatch through the queue;
- a synth engine;
- lock/effect visual feedback;
- persistence;
- final hardware control-map freeze.

## Hardware limitation

No physical FM-1 is currently available. Host/device-build evidence does not claim physical audition latency/feel, install/boot behavior, physical sound quality or measured AC791N execution margin.

## Handoff

WP3.9 is next: lock/effect visual feedback. Its visual ownership/update contract must be frozen before production UI source. WP3.8 leaves a measured, bounded runtime-event foundation without changing the verified D031-D033 tracker timing paths.
