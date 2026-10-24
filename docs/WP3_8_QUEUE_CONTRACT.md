# WP3.8 / D034 — Bounded Runtime Event Queue Contract

Status: **FROZEN BEFORE PRODUCTION SOURCE**  
Date: 2026-10-24  
Package: WP3.8 / issue #27  
Branch: `phase3/plocks-effects`

## 1. Purpose

WP3.8 establishes the bounded runtime-event handoff that the V0.1 architecture and data model have reserved since Phase 0.

The queue is **not** a replacement for D033 retrig / note-delay / note-cut scheduling. D033 already proved those row-local actions can be represented more cheaply and more deterministically as four bytes of fixed state on D032's exact six-tick clock.

The first real cross-context problem WP3.8 solves is the existing manual/audition path:

- hardware key and USB-MIDI note intent is produced in the main loop;
- today `play()` / `stop()` write manual voice internals directly from main context;
- the long-term architecture requires main/editor state to hand compact bounded events to audio rather than mutating audio-owned voice state directly.

WP3.8 therefore freezes a **single-producer / single-consumer (SPSC) main-loop -> audio-ISR ingress queue** and uses the already-existing manual/audition NOTE ON/OFF path as its first production integration.

This is an event-foundation package only. It does **not** implement live recording, overdub, automation capture, new engines or multi-track sequencer dispatch.

## 2. What remains outside this queue

The following verified paths remain unchanged in WP3.8:

- D020 transport phase/timing;
- D021 proof-lane row dispatch;
- D031 row-local p-lock 32-bit snapshots;
- D032 six-tick tracker-effect timing/cache/state;
- D033 fixed-state RETRIG / NOTE DELAY / NOTE CUT scheduling;
- the `tracker_play_requested` desired-state mailbox and `tracker_transport_stopped` acknowledgement.

A desired run state is idempotent state, not an ordered discrete event. The one-writer D024 transport mailbox is therefore a better primitive than a queue for PLAY/STOP and is not replaced merely to exercise WP3.8.

Pattern storage also does not become the runtime queue format in WP3.8. The existing proof lane remains deliberately narrow until the multi-track/engine packages own that migration.

## 3. Runtime event record — exactly 8 bytes

D034 freezes the provisional Phase-0 `FmtRtEvent` shape as the V0.1 runtime event envelope:

```c
typedef struct {
    uint8_t  type;
    uint8_t  track;
    uint16_t value_a;
    uint32_t value_b;
} FmtRtEvent;
```

Required layout:

```text
offset 0  type     1 B
offset 1  track    1 B
offset 2  value_a  2 B
offset 4  value_b  4 B
sizeof             8 B
alignment          4 B or less
```

The record contains no pointer and no reference to mutable editor-owned storage.

### Initially assigned event types

```text
0x00  NONE        invalid / never enqueued
0x01  NOTE_ON
0x02  NOTE_OFF
0x03..0xFF reserved for later explicit decisions
```

WP3.8 assigns semantics only to NOTE_ON and NOTE_OFF.

For NOTE events:

- `track`: logical track `0..7` captured by the producer at enqueue time;
- `value_a` low byte: MIDI-style note `0..127`;
- `value_a` high byte: velocity `1..127` for NOTE_ON, ignored for NOTE_OFF;
- `value_b`: `0` in WP3.8 and reserved for future event-specific payload/timestamp/offset semantics.

The current Dinghy sine audition adapter may ignore `track` when choosing one of its four temporary manual voices, but the queue record still carries the producer's exact logical target so later engines do not need a record-layout change.

`value_b` is **not** silently defined as a timestamp in WP3.8. A later package that needs sample-offset or transport-position semantics must freeze that interpretation explicitly.

## 4. Capacity and RAM budget

The queue capacity is exactly:

```text
32 events
```

Rationale:

- FM-1 exposes 27 physical note keys;
- one main-loop poll can therefore produce at most 27 physical note-edge events;
- 32 entries can hold one complete physical keyboard edge set with five entries of headroom;
- USB-MIDI can still produce a larger burst, so deterministic overflow behavior remains required rather than pretending overflow is impossible.

The event payload storage costs:

```text
32 * 8 B = 256 B
```

Queue metadata is exactly three aligned 32-bit sequence counters:

```text
write_seq     4 B   producer-owned
read_seq      4 B   consumer-owned
overflow_seq  4 B   producer-owned
```

Target `FmtRtQueue` size:

```text
268 B
```

No pool RAM is used. No allocator exists.

## 5. Storage publication format

Although `FmtRtEvent` is an 8-byte logical object, queue slots are published as **two aligned 32-bit words**, not as a concurrently visible compiler struct assignment.

Word 0:

```text
bits  0..7   type
bits  8..15  track
bits 16..31  value_a
```

Word 1:

```text
bits 0..31   value_b
```

This keeps each individual shared slot access at the platform's preferred aligned 32-bit width.

## 6. SPSC ownership

There is exactly one producer and one consumer:

### Producer — main loop

Main is the only writer of:

- slot payload before publication;
- `write_seq`;
- `overflow_seq`.

Main may read `read_seq` to test capacity.

### Consumer — audio ISR

Audio is the only writer of:

- `read_seq`.

Audio may read `write_seq`, `overflow_seq` and published slot words.

No context writes the other context's owned sequence counter.

The queue must not disable interrupts, spin, retry until success or use a lock/mutex.

## 7. Publication ordering and compiler visibility

Producer enqueue order is fixed:

1. read `write_seq` and `read_seq`;
2. reject if full;
3. write slot word 0;
4. write slot word 1;
5. compiler memory barrier;
6. publish `write_seq + 1`.

Consumer dequeue order is fixed:

1. read `read_seq` and `write_seq`;
2. return empty if equal;
3. compiler memory barrier;
4. read slot word 0;
5. read slot word 1;
6. unpack the complete event;
7. publish `read_seq + 1`.

The implementation uses aligned volatile 32-bit shared words plus a compiler memory barrier compatible with the host compiler and JieLi clang. It does not require C11 atomics or a runtime library helper.

The target build must verify that `FmtRtQueue` is naturally aligned in ordinary RAM.

## 8. Sequence-counter and wrap rules

`write_seq`, `read_seq` and `overflow_seq` are unsigned 32-bit monotonic modulo counters.

Queue depth is computed as unsigned:

```text
write_seq - read_seq
```

The producer never advances more than `FMT_RT_QUEUE_CAPACITY` ahead of the consumer, so ordinary unsigned wrap arithmetic remains unambiguous for the queue's bounded distance.

Slot index is:

```text
sequence & (capacity - 1)
```

Capacity is therefore fixed as a power of two.

Host tests must exercise sequence values around `UINT32_MAX`; no test may assume the counters never wrap.

## 9. Overflow policy — reject newest

When depth is already 32:

- the new event is **not written**;
- existing unread events are untouched;
- `write_seq` does not advance;
- producer increments `overflow_seq` exactly once;
- enqueue returns an explicit FULL result;
- there is no retry loop.

This is **reject-newest** semantics.

WP3.8 explicitly rejects drop-oldest because that would require the producer to modify the consumer-owned `read_seq` or overwrite an unread slot, weakening the SPSC ownership contract.

## 10. Safe audition recovery after overflow

Reject-newest alone could drop a NOTE_OFF and leave an audition voice held. Therefore the first production consumer adds one deterministic safety rule:

- audio remembers the last observed `overflow_seq`;
- if `overflow_seq` changed, audio releases all four temporary Dinghy manual/audition voice gates;
- audio then consumer-discards all currently queued audition events in **O(1)** by publishing `read_seq = write_seq`;
- normal queue consumption resumes on the following/newly enqueued events.

Release remains click-safe through the existing amplitude glide; voice amplitudes are not hard-zeroed.

Overflow recovery deliberately favors silence over a potentially stuck note or replaying a partially lost gesture.

The queue module itself does not know about voices; it only exposes the overflow sequence and an O(1) consumer discard operation. Future consumers must freeze their own safe overflow recovery if NOTE semantics differ.

## 11. Consumer work bound

Audio checks the queue **once per audio callback, before rendering the callback's frames**.

It may consume at most:

```text
32 events per callback
```

A loop over queued events is therefore variable in occupancy but hard-bounded by the compile-time capacity. There is no event-queue loop inside the per-sample tracker render path.

For current NOTE_ON/OFF audition events, each consumed event performs only fixed validation plus the existing fixed four-voice selection/release operation.

No allocator, D028 list traversal, screen work, flash access, float or runtime 64-bit division is permitted.

## 12. WP3.8 timing semantics

The queue is an **immediate callback-boundary ingress**, not a sample-accurate scheduler.

An event successfully enqueued between audio callbacks becomes eligible at the beginning of the next callback, before any frame in that callback is rendered. FIFO order is preserved for all events consumed in that callback.

At the normal 256-frame / 44.1 kHz platform block size, this preserves the same practical context boundary as the existing main-written manual voice controls while removing direct main-context voice mutation.

WP3.8 does not promise an intra-block timestamp. D033 remains responsible for its already verified sample-exact six-tick NOTE DELAY/CUT/RETRIG actions.

Future live-performance capture may use a separately frozen transport-position snapshot and/or assign `value_b` event-specific timing semantics, but that work is explicitly not part of WP3.8.

## 13. Manual/audition integration

WP3.8 moves existing manual sine-voice mutation behind the queue:

- hardware note-down -> enqueue NOTE_ON;
- hardware note-up -> enqueue NOTE_OFF;
- USB-MIDI NOTE_ON -> enqueue NOTE_ON;
- USB-MIDI NOTE_OFF / NOTE_ON velocity 0 -> enqueue NOTE_OFF.

The main loop no longer chooses a manual voice or writes its oscillator increment/gate/age.

Audio owns manual voice allocation and note-on/off mutation. Existing deterministic four-voice selection policy is preserved and executed when the event is consumed.

`last_note` remains UI intent and is updated only after a valid NOTE_ON enqueue succeeds.

The current USB-MIDI output behavior for physical keys is unchanged; the runtime queue is an internal audio handoff, not a MIDI queue.

`quiet()` must treat a non-empty runtime queue as not quiet, preventing a pending NOTE_ON from being hidden behind a flash/settings operation before audio consumes it.

## 14. Malformed input

The producer helper rejects before enqueue:

- event type NONE;
- NOTE_ON/OFF with track outside `0..7`;
- NOTE_ON/OFF with note above 127;
- NOTE_ON with velocity 0 or above 127.

Rejected malformed input does not consume capacity and does not increment `overflow_seq`.

The audio adapter also validates known event types defensively. Unknown/reserved event types are ignored safely and cannot alter voice state.

## 15. Tests required before closeout

### Queue unit tests

Must prove:

- `sizeof(FmtRtEvent) == 8` and exact offsets;
- `sizeof(FmtRtQueue) == 268`;
- reset -> empty, depth 0, overflow 0;
- exact FIFO payload round-trip;
- 32 successful enqueues and the 33rd rejected;
- full rejection leaves all 32 unread events unchanged;
- each failed full enqueue increments `overflow_seq` once;
- dequeue advances only consumer-owned `read_seq`;
- consumer discard is O(1) state change and empties the queue;
- wrap behavior around `UINT32_MAX` preserves FIFO/depth;
- malformed NOTE helpers do not enter the queue;
- no pointer appears in the event record.

### Full-app / audio tests

Must prove:

- ordinary hardware audition NOTE_ON/OFF still sounds/releases deterministically;
- USB-MIDI audition still sounds/releases deterministically;
- ordered same-callback NOTE transitions are consumed FIFO;
- forced queue overflow cannot leave a manual voice gate stuck;
- overflow recovery discards the compromised pending batch and returns to usable operation;
- no additional MIDI-out event is emitted by queue consumption itself;
- default tracker WAV remains byte-identical;
- D031 locked WAV remains byte-identical;
- every frozen WP3.6 and WP3.7 audio fixture remains byte-identical.

### Device-build evidence

Must prove:

- real JieLi/PI32v2 compilation;
- queue symbol in ordinary RAM with 4-byte alignment;
- exact event/queue sizes;
- measured image/RAM/pool deltas;
- package identity remains `FM-1_987`;
- all inherited Phase 2 + WP3.1-WP3.7 regressions remain green.

## 16. Explicit non-goals

WP3.8 does not implement:

- live REC/capture;
- quantization, overdub or merge rules;
- sample-accurate timestamped main-loop events;
- D033 event scheduling through this queue;
- multi-track pattern decoding through this queue;
- engine voice architecture;
- effect/lock visual feedback;
- persistence;
- final hardware control-map decisions.

## 17. Exit condition

WP3.8 may close only when the queue and first real main->audio audition integration are verified under forced overflow, all inherited deterministic tracker/audio evidence remains intact, and target resource cost is measured.

If the queue causes a frozen tracker/audio regression, the package is not complete; do not weaken D020-D033 timing/evidence to accommodate it.
