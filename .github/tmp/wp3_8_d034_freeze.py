from pathlib import Path


def replace_once(path, old, new):
    p = Path(path)
    s = p.read_text()
    count = s.count(old)
    assert count == 1, (path, old[:100], count)
    p.write_text(s.replace(old, new, 1))

# Record D034 in the architectural decision log before production source.
p = Path('docs/DECISIONS.md')
s = p.read_text()
marker = '## D034 — WP3.8 uses a 32-entry SPSC main-to-audio runtime event queue with reject-newest overflow'
assert marker not in s
s += r'''

## D034 — WP3.8 uses a 32-entry SPSC main-to-audio runtime event queue with reject-newest overflow

Date: 2026-10-24
Status: accepted before WP3.8 production source

**Decision:** WP3.8 freezes the general runtime-event ingress anticipated by the Phase-0 architecture/data model as a single-producer/single-consumer main-loop -> audio-ISR queue. The normative detailed contract is `docs/WP3_8_QUEUE_CONTRACT.md`.

D034 does **not** move D033 RETRIG / NOTE DELAY / NOTE CUT into a queue. Those remain the verified four-byte fixed-state scheduler on D032's exact six-tick clock. D024's PLAY desired-state mailbox likewise remains a mailbox because desired run state is idempotent state rather than an ordered discrete event.

`FmtRtEvent` is frozen at exactly 8 bytes: one-byte `type`, one-byte logical `track`, 16-bit `value_a`, 32-bit `value_b`. WP3.8 assigns only NOTE_ON (`0x01`) and NOTE_OFF (`0x02`); `value_b` remains zero/reserved and is not silently defined as a timestamp. Queue slots publish the event as two aligned 32-bit words.

Capacity is exactly 32 events. The 256-byte slot array plus producer-owned `write_seq`, consumer-owned `read_seq` and producer-owned `overflow_seq` gives a target `FmtRtQueue` size of exactly 268 bytes ordinary RAM. Capacity 32 covers all 27 physical FM-1 note-key edges from one main-loop poll while retaining deterministic behavior for larger USB-MIDI bursts.

Publication is SPSC: main writes both slot words, executes a compiler memory barrier, then advances `write_seq`; audio observes `write_seq`, executes the matching barrier, reads both words, then advances only `read_seq`. No interrupt masking, lock, allocator, spin or retry is allowed. Unsigned monotonic sequence counters and a power-of-two mask define wrap-safe bounded occupancy.

Overflow is **reject-newest**. When the queue already contains 32 unread events, the producer leaves every unread slot and `write_seq` untouched, increments `overflow_seq` once and returns FULL. Drop-oldest is explicitly rejected because it would require producer mutation of consumer ownership or overwrite unread state.

The first production integration is the existing manual/audition NOTE ON/OFF path. Main no longer chooses/mutates Dinghy manual voice internals directly; it enqueues note intent, and audio consumes up to 32 events once at callback start before rendering. This is not live recording. Current hardware/USB-MIDI audition simply proves the real main->audio boundary already required by the architecture.

Because reject-newest could otherwise lose NOTE_OFF, the audition adapter observes `overflow_seq`. On any change it releases all four manual voice gates click-safely and consumer-discards the pending batch in O(1) by advancing `read_seq` to the published `write_seq`. Overflow therefore fails toward silence, never toward an indefinitely stuck note.

WP3.8 queue timing is callback-boundary immediate ingress, not a new sample-accurate scheduler. Successfully queued events are FIFO-consumed at the beginning of the next audio callback. D033 remains responsible for its already verified sample-exact sub-row timing. Future live capture/sample-offset semantics require a later explicit decision and may assign event-specific meaning to the reserved `value_b` without changing the queue envelope.

**Reason:** The current proof firmware still has one architectural shortcut where main-context `play()` / `stop()` directly mutate manual audio voice state. A small SPSC queue removes that cross-context mutation, matches the original architecture, gives future live-performance work a measured ingress primitive, and provides a real overflow problem to solve without dragging live recording or multi-track engine work into Phase 3. A 32-entry/8-byte design is large enough for one full 27-key hardware edge set but cheap enough to measure now.

**Consequence:** WP3.8 may add only the 8-byte runtime event envelope, exact 268-byte queue, NOTE ON/OFF audition adapter and the tests/diagnostics required by `WP3_8_QUEUE_CONTRACT.md`. Tests must prove exact layout, FIFO, full/reject-newest semantics, sequence wrap, O(1) discard, forced-overflow no-stuck-note recovery, real key/MIDI audition, queue-only MIDI neutrality, target alignment/resource cost and all inherited Phase 2 + WP3.1-WP3.7 deterministic audio regressions. Live REC/capture, timestamped automation, D033-through-queue, multi-track runtime dispatch, new engines and visual feedback remain out of scope.
'''
p.write_text(s)

replace_once(
    'docs/PROJECT_STATE.md',
    'State: **IN PROGRESS - WP3.1-WP3.7 complete; WP3.8 ready for D034 architecture freeze - 2026-10-24**',
    'State: **IN PROGRESS - WP3.1-WP3.7 complete; WP3.8 active, D034 frozen before source - 2026-10-24**')
replace_once(
    'docs/PROJECT_STATE.md',
    'State: **READY TO START - D034 architecture freeze required before production source**',
    'State: **IN PROGRESS - D034 frozen; production queue implementation next**')
replace_once(
    'docs/PROJECT_STATE.md',
    'WP3.8 must first prove the actual producer/consumer need for a queue beyond D033\'s fixed row-local state, then freeze:\n\n- event record and exact byte size;\n- queue capacity and RAM budget;\n- producer/consumer ownership and head/tail rules;\n- deterministic overflow behavior;\n- event timestamp domain and ordering against D020/D021/D031/D032/D033;\n- reset/STOP/seek semantics;\n- bounded worst-case audio work and malformed-event fallback.\n\nDo not begin WP3.8 production source before D034 is frozen. Do not retrofit queue assumptions into the verified D033 path unless D034 explicitly supersedes a D033 rule.',
    'D034 is frozen in `docs/WP3_8_QUEUE_CONTRACT.md`: exactly 8-byte `FmtRtEvent`, 32-entry / 268-byte SPSC main->audio queue, producer-owned `write_seq`/`overflow_seq`, consumer-owned `read_seq`, aligned two-word publication, reject-newest overflow, callback-boundary immediate timing, and fail-to-silence audition recovery. D033 and the D024 transport mailbox remain outside the queue.\n\nProduction source may now implement **only** this contract, beginning with the queue module/unit tests and then routing existing manual key/USB-MIDI NOTE ON/OFF through it. Live capture/timestamp semantics and multi-track sequencer migration remain later work.')
replace_once(
    'docs/PROJECT_STATE.md',
    'Start WP3.8 / #27 with D034 architecture analysis and freeze. Do **not** write queue production source until D034 is accepted.',
    'Implement the frozen D034 queue module and focused unit tests first, then integrate existing manual key/USB-MIDI audition NOTE ON/OFF through the queue. Do not expand into live capture or D033 queueing.')
replace_once(
    'docs/PROJECT_STATE.md',
    '2026-10-24 - WP3.7 complete and issue #26 closed. WP3.8 / #27 is ready for D034 architecture freeze; no WP3.8 production source has begun.',
    '2026-10-24 - WP3.7 complete and #26 closed. WP3.8 / #27 active; D034 frozen in `WP3_8_QUEUE_CONTRACT.md` before source. No WP3.8 production source has begun yet.')

replace_once(
    'docs/WORK_PACKAGES.md',
    '- **WP3.8 — bounded event queue and overflow policy** — READY TO START — issue #27.\n  Freeze D034 before production source. D034 must establish the real producer/consumer need for a queue, exact event record/capacity/RAM budget, ownership and memory visibility, timestamp domain, deterministic overflow policy, reset/STOP behavior, ordering against D020/D021/D031/D032/D033 and bounded worst-case audio work. The verified D033 path remains fixed-state unless D034 explicitly supersedes it.',
    '- **WP3.8 — bounded event queue and overflow policy** — IN PROGRESS — issue #27.\n  D034 is frozen before source in `WP3_8_QUEUE_CONTRACT.md`: 8-byte runtime events, exact 32-entry / 268-byte SPSC main->audio queue, aligned two-word publication, reject-newest overflow and click-safe fail-to-silence audition recovery. Implement only this contract; D033 remains fixed-state and live capture remains later work.')

replace_once(
    'docs/ROADMAP.md',
    '8. ⬜ **WP3.8 — bounded event queue and overflow policy**, issue #27, READY FOR D034 ARCHITECTURE FREEZE.',
    '8. 🟨 **WP3.8 — bounded event queue and overflow policy**, issue #27, IN PROGRESS. D034 frozen in `WP3_8_QUEUE_CONTRACT.md` before source.')
replace_once(
    'docs/ROADMAP.md',
    'Immediate next package: **WP3.8 / #27**. Freeze D034 before source. D034 must prove the producer/consumer need, exact event record/capacity/RAM budget, ownership, timestamp domain, overflow policy and bounded audio work before any queue implementation begins.',
    'Immediate next package: **WP3.8 / #27**. D034 is frozen: implement the 32-entry SPSC main->audio queue and existing audition NOTE ON/OFF adapter, then prove forced overflow and all inherited regressions before closeout.')

replace_once(
    'docs/TEST_MATRIX.md',
    '| Queue overflow policy deterministic | required | required | stress test | ⬜ WP3.8 / #27 |',
    '| Queue overflow policy deterministic | required | required | stress test | 🟨 D034 frozen: reject-newest; implementation pending |')

print('D034_FREEZE_PROPAGATED=1')
