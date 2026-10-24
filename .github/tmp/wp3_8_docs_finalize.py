from pathlib import Path


def read(path):
    return Path(path).read_text()


def write(path, text):
    Path(path).write_text(text)


def once(text, old, new, label):
    count = text.count(old)
    assert count == 1, (label, count, old[:120])
    return text.replace(old, new, 1)

# START_HERE: remove the obsolete Phase-1 special-case and make current
# repository-driven handoff explicit without duplicating PROJECT_STATE.
p = 'START_HERE.md'
s = read(p)
s = once(
    s,
    'For the current Phase 1 platform import, also read:\n\n- [`docs/UPSTREAM_DINGHY.md`](docs/UPSTREAM_DINGHY.md)\n- [`docs/DINGHY_IMPORT_MANIFEST.md`](docs/DINGHY_IMPORT_MANIFEST.md)\n\n',
    'For the current active package, also read the most recently completed verification and the active package contract named by `docs/PROJECT_STATE.md`. As of the 2026-10-24 handoff, WP3.8 is verified and WP3.9 / #28 is next with D035 required before production UI source.\n\nPlatform provenance remains available in:\n\n- [`docs/UPSTREAM_DINGHY.md`](docs/UPSTREAM_DINGHY.md)\n- [`docs/DINGHY_IMPORT_MANIFEST.md`](docs/DINGHY_IMPORT_MANIFEST.md)\n\n',
    'START_HERE phase note')
write(p, s)

# PROJECT_STATE: close WP3.8 and hand off exactly one active package.
p = 'docs/PROJECT_STATE.md'
s = read(p)
s = once(
    s,
    'State: **IN PROGRESS - WP3.1-WP3.7 complete; WP3.8 active, D034 frozen before source - 2026-10-24**',
    'State: **IN PROGRESS - WP3.1-WP3.8 complete; WP3.9 ready for D035 visual/update freeze - 2026-10-24**',
    'PROJECT_STATE state')

start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n')
replacement = '''### WP3.8 - bounded runtime event queue and overflow policy - COMPLETE - #27

Production audition-queue integration:

`223fec91746a95cc003eabe7cd1540af75e5ed26`

Final clean-tree verified source/workflow checkpoint:

`5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`

Final GitHub Actions:

- run `40079824265`
- job `120830854974` (`runtime-queue-closeout`)
- result **SUCCESS**

Evidence: `docs/WP3_8_VERIFICATION.md`  
Normative contract: `docs/WP3_8_QUEUE_CONTRACT.md`

Evidence artifact:

- ID `12554234479`
- ZIP SHA-256 `2bd3b62cbfe5eec8867066acbdcecc5221dc515afa525f3914dab04b3f8b219f`

D034 freezes an exact 8-byte `FmtRtEvent` and a 32-entry / 268-byte SPSC main-loop -> audio-ISR queue. Physical-key and USB-MIDI audition NOTE ON/OFF now enter audio through the queue; main no longer selects or mutates manual audio voices directly.

Overflow is reject-newest. Audio detects `overflow_seq`, releases all manual gates click-safely and consumer-discards the compromised batch in O(1), so overflow fails toward silence rather than a stuck note. Sequence wrap is verified across `UINT32_MAX`.

The compiler publication barrier is isolated behind `firmware/hal/fm1_barrier.h`, which is the only HAL-file difference from the WP3.7 verified checkpoint. D024 transport state and D033 sub-row scheduling remain outside the queue.

Focused proof:

`FMT_RT_QUEUE_TESTS_OK=1`

`WP3_8_QUEUE_AUDIO_BEHAVIOR_OK=1 fifo=1 hardware=1 midi=1 overflow_silence=1 recovery=1 queue_midi_neutral=1`

Final WP3.8 resources:

| Resource | Used | Limit | Free | Delta vs WP3.7 |
|---|---:|---:|---:|---:|
| app image | 66,308 B | 581,564 B | 515,256 B | +368 B |
| normal RAM | 13,280 B | 98,304 B | 85,024 B | +272 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `a1d422b0c533af7f713deca37191a45e438640e2fc1be7c2734c1e4539822251`
- `fm1tracker-0.1.fwsc` SHA-256 `b93d5dd57d4e23214ac4ef500e47d91b3e4528b620b646f8804312654df98b8a`
- package identity `FM-1_987`.

Historical default WAV, D031 locked WAV and all frozen WP3.6/WP3.7 audio fixtures remain byte-identical through WP3.8.

## Next milestone

**WP3.9 - lock/effect visual feedback**

State: **READY TO START - D035 visual/update contract required before production UI source**

GitHub issue: #28

D035 must freeze exactly what p-lock/effect state is shown, compact notation, cursor/playhead/edit-state priority, screen/update ownership, dirty-region bounds, stale-feedback cancellation and deterministic framebuffer evidence. No audio ISR may draw or mutate display state.

Do not write WP3.9 production visual-feedback source before D035 is frozen. WP3.10 remains the Phase 3 final worst-case closeout and must not be pulled forward.

'''
s = s[:start] + replacement + s[end:]
s = once(
    s,
    '- D033: retrig/delay/cut remain fixed-state, exact-six-tick and queue-free; event runtime exactly 4 B at the verified checkpoint.\n',
    '- D033: retrig/delay/cut remain fixed-state, exact-six-tick and queue-free; event runtime exactly 4 B at the verified checkpoint.\n- D034: runtime ingress is an exact 32-entry / 268-byte SPSC main->audio queue with 8-byte events, reject-newest overflow and fail-to-silence audition recovery; D024/D033 remain outside it.\n',
    'PROJECT_STATE D034 bullet')
s = once(
    s,
    'Implement the frozen D034 queue module and focused unit tests first, then integrate existing manual key/USB-MIDI audition NOTE ON/OFF through the queue. Do not expand into live capture or D033 queueing.',
    'Start WP3.9 / #28 by freezing D035 visual/update ownership and notation before production UI source. Preserve D026 bounded screen work and all D031-D034 audio/runtime behavior.',
    'PROJECT_STATE next action')
s = once(
    s,
    '- WP3.7 verified: `d2a97798149816957745de818e4568c518a9a7e7`\n',
    '- WP3.7 verified: `d2a97798149816957745de818e4568c518a9a7e7`\n- WP3.8 verified: `5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`\n',
    'PROJECT_STATE checkpoint')
s = once(
    s,
    '2026-10-24 - WP3.7 complete and #26 closed. WP3.8 / #27 active; D034 frozen in `WP3_8_QUEUE_CONTRACT.md` before source. No WP3.8 production source has begun yet.',
    '2026-10-24 - WP3.8 complete and #27 closed at clean checkpoint `5c282892...`. WP3.9 / #28 is next; freeze D035 before production visual-feedback source.',
    'PROJECT_STATE last updated')
write(p, s)

# WORK_PACKAGES: promote WP3.8 to completed and make WP3.9 the sole next package.
p = 'docs/WORK_PACKAGES.md'
s = read(p)
s = once(
    s,
    'Status: **IN PROGRESS — WP3.1–WP3.7 complete; WP3.8 next**',
    'Status: **IN PROGRESS — WP3.1–WP3.8 complete; WP3.9 next**',
    'WORK_PACKAGES status')
s = once(
    s,
    '- **WP3.7 — retrig / note delay / note cut** — COMPLETE — issue #26\n  D033 extends the same command domain with RETRIG `0x06`, NOTE DELAY `0x07`, NOTE CUT `0x08`, exact D032 six-tick scheduling and a 4 B fixed row-event state. No general queue was pulled forward. Final clean-tree run `40077809743`, job `120824870575`. Final resources: 65,940 B image / 13,008 B normal RAM / 113,000 B pool. Evidence: `WP3_7_EVENT_CONTRACT.md`, `WP3_7_VERIFICATION.md`.\n',
    '- **WP3.7 — retrig / note delay / note cut** — COMPLETE — issue #26\n  D033 extends the same command domain with RETRIG `0x06`, NOTE DELAY `0x07`, NOTE CUT `0x08`, exact D032 six-tick scheduling and a 4 B fixed row-event state. No general queue was pulled forward. Final clean-tree run `40077809743`, job `120824870575`. Final resources: 65,940 B image / 13,008 B normal RAM / 113,000 B pool. Evidence: `WP3_7_EVENT_CONTRACT.md`, `WP3_7_VERIFICATION.md`.\n\n- **WP3.8 — bounded event queue and overflow policy** — COMPLETE — issue #27\n  D034 freezes an 8-byte event envelope and exact 32-entry / 268-byte SPSC main->audio queue, reject-newest overflow, wrap-safe sequence counters and click-safe fail-to-silence audition recovery. Physical-key and USB-MIDI audition no longer mutate manual audio voices from main. Final clean-tree run `40079824265`, job `120830854974`. Final resources: 66,308 B image / 13,280 B normal RAM / 113,000 B pool. Evidence: `WP3_8_QUEUE_CONTRACT.md`, `WP3_8_VERIFICATION.md`.\n',
    'WORK_PACKAGES completed WP3.8')
s = once(
    s,
    'Current verified baseline after WP3.7:\n\n- app image: 65,940 B / 581,564 B\n- normal RAM: 13,008 B / 98,304 B\n- pool RAM: 113,000 B / 344,064 B\n- D031 playback row cache: 256 B ordinary RAM\n- D032 effect row cache: 256 B ordinary RAM\n- D032 effect runtime: 12 B\n- D033 row-event runtime: 4 B\n',
    'Current verified baseline after WP3.8:\n\n- app image: 66,308 B / 581,564 B\n- normal RAM: 13,280 B / 98,304 B\n- pool RAM: 113,000 B / 344,064 B\n- D031 playback row cache: 256 B ordinary RAM\n- D032 effect row cache: 256 B ordinary RAM\n- D032 effect runtime: 12 B\n- D033 row-event runtime: 4 B\n- D034 runtime queue: 268 B ordinary RAM + 4 B overflow acknowledgement\n',
    'WORK_PACKAGES baseline')
old_next = '''## Next package

- **WP3.8 — bounded event queue and overflow policy** — IN PROGRESS — issue #27.
  D034 is frozen before source in `WP3_8_QUEUE_CONTRACT.md`: 8-byte runtime events, exact 32-entry / 268-byte SPSC main->audio queue, aligned two-word publication, reject-newest overflow and click-safe fail-to-silence audition recovery. Implement only this contract; D033 remains fixed-state and live capture remains later work.

Remaining Phase 3 order:

- WP3.8 bounded event queue and overflow policy
- WP3.9 lock/effect visual feedback
- WP3.10 Phase 3 worst-case tests
'''
new_next = '''## Next package

- **WP3.9 — lock/effect visual feedback** — READY TO START — issue #28.
  Freeze D035 before production UI source. D035 must define compact p-lock/effect notation, normal-view versus edit-only feedback, cursor/playhead/edit priority, screen-state ownership, bounded dirty regions/redraw work, stale-state cancellation and deterministic framebuffer evidence. Audio/ISR behavior remains untouched.

Remaining Phase 3 order:

- WP3.9 lock/effect visual feedback
- WP3.10 Phase 3 worst-case tests
'''
s = once(s, old_next, new_next, 'WORK_PACKAGES next block')
write(p, s)

# ROADMAP status/footprint.
p = 'docs/ROADMAP.md'
s = read(p)
s = once(s,
         'Status: **IN PROGRESS — WP3.1–WP3.7 complete; WP3.8 next**',
         'Status: **IN PROGRESS — WP3.1–WP3.8 complete; WP3.9 next**',
         'ROADMAP status')
s = once(s,
         '8. 🟨 **WP3.8 — bounded event queue and overflow policy**, issue #27, IN PROGRESS. D034 frozen in `WP3_8_QUEUE_CONTRACT.md` before source.\n9. ⬜ WP3.9 — lock/effect visual feedback.',
         '8. ✅ **WP3.8 — bounded event queue and overflow policy**. D034 freezes the exact 32-entry SPSC main->audio queue, reject-newest overflow and fail-to-silence audition recovery. Final run `40079824265`; evidence `WP3_8_QUEUE_CONTRACT.md`, `WP3_8_VERIFICATION.md`.\n9. ⬜ **WP3.9 — lock/effect visual feedback**, issue #28, READY FOR D035 VISUAL/UPDATE FREEZE.',
         'ROADMAP progress')
s = once(s,
         'Current verified Phase 3 footprint after WP3.7: 65,940 B image, 13,008 B normal RAM and 113,000 B pool. The historical default WAV, D031 locked WAV and frozen WP3.6 D032 audio fixtures remain byte-identical.\n\nImmediate next package: **WP3.8 / #27**. D034 is frozen: implement the 32-entry SPSC main->audio queue and existing audition NOTE ON/OFF adapter, then prove forced overflow and all inherited regressions before closeout.',
         'Current verified Phase 3 footprint after WP3.8: 66,308 B image, 13,280 B normal RAM and 113,000 B pool. The historical default WAV, D031 locked WAV and every frozen WP3.6/WP3.7 fixture remain byte-identical.\n\nImmediate next package: **WP3.9 / #28**. Freeze D035 visual/update ownership and compact notation before production UI source; preserve the bounded D026 redraw model and all D031-D034 runtime/audio semantics.',
         'ROADMAP footprint/next')
write(p, s)

# TEST_MATRIX: turn D034 from pending to frozen evidence and add durable doc.
p = 'docs/TEST_MATRIX.md'
s = read(p)
s = once(
    s,
    '| Queue overflow policy deterministic | required | required | stress test | 🟨 D034 frozen: reject-newest; implementation pending |',
    '| `FmtRtEvent` exact 8 B layout / no pointers | required | target compile | n/a | ✅ WP3.8 / D034 |\n'
    '| `FmtRtQueue` exact 32-entry / 268 B ordinary-RAM layout | required | required | n/a | ✅ WP3.8 |\n'
    '| SPSC ownership / aligned two-word publication bounded | required | required | n/a | ✅ main producer / audio consumer |\n'
    '| Sequence counters remain FIFO/depth-correct through `UINT32_MAX` wrap | required | compile | n/a | ✅ WP3.8 wrap test |\n'
    '| Queue overflow policy deterministic | required | required | stress test | ✅ reject-newest; 33rd rejected without unread mutation |\n'
    '| Audition overflow cannot leave a stuck manual gate | required | required | later | ✅ forced >32 NOTE ON fixture fails to silence |\n'
    '| Queue recovery returns to usable NOTE ON/OFF operation | required | required | later | ✅ post-overflow recovery fixture |\n'
    '| Physical-key + USB-MIDI audition enter audio only through D034 queue | required | compile | required later | ✅ WP3.8 full-app proof |\n'
    '| Queue consumption itself emits no MIDI output | required | compile | later | ✅ queue_midi_neutral=1 |\n'
    '| Compiler publication barrier stays behind HAL boundary | inspect/test | required | n/a | ✅ only new HAL file `fm1_barrier.h` |\n'
    '| D034 preserves D024 mailbox and D033 fixed-state scheduler boundaries | inspect/test | required | n/a | ✅ WP3.8 |\n'
    '| WP3.8 final resource delta measured/frozen | n/a | required | n/a | ✅ +368 B image / +272 B RAM / +0 B pool |\n'
    '| WP3.8 preserves all Phase 2 + WP3.1-WP3.7 regressions | required | required | later | ✅ run `40079824265` |',
    'TEST_MATRIX WP3.8 rows')
s = once(s,
         '- `WP3_7_VERIFICATION.md`\n',
         '- `WP3_7_VERIFICATION.md`\n- `WP3_8_VERIFICATION.md`\n',
         'TEST_MATRIX evidence')
write(p, s)

# CHANGELOG append durable milestone once.
p = 'CHANGELOG.md'
s = read(p)
marker = '## 2026-10-24 - WP3.8 bounded runtime event queue'
assert marker not in s
s += '''

## 2026-10-24 - WP3.8 bounded runtime event queue

- Froze D034 before production source in `docs/WP3_8_QUEUE_CONTRACT.md`.
- Added exact 8-byte `FmtRtEvent` and 32-entry / 268-byte SPSC main-loop -> audio-ISR queue with wrap-safe 32-bit sequence counters.
- Chose deterministic reject-newest overflow; unread events are never overwritten and producer never mutates consumer ownership.
- Added fail-to-silence audition recovery: on overflow audio releases manual gates click-safely and consumer-discards the compromised batch in O(1), then accepts later events normally.
- Routed existing physical-key and USB-MIDI audition NOTE ON/OFF through D034; main no longer allocates or mutates manual audio voices directly.
- Kept D024 transport desired-state mailbox and D033 exact-six-tick scheduler outside the queue.
- Moved the compiler publication barrier behind `firmware/hal/fm1_barrier.h`; this is the only HAL-file difference from the verified WP3.7 checkpoint.
- Frozen full-app audio hashes prove FIFO, physical audition, MIDI audition, forced-overflow silence, post-overflow recovery and queue MIDI neutrality.
- Final resources: 66,308 B image / 13,280 B normal RAM / 113,000 B pool; delta vs WP3.7 +368 / +272 / +0 B.
- Final artifacts: `fm1tracker.bin` `a1d422b0c533af7f713deca37191a45e438640e2fc1be7c2734c1e4539822251`; `fm1tracker-0.1.fwsc` `b93d5dd57d4e23214ac4ef500e47d91b3e4528b620b646f8804312654df98b8a`.
- Final clean-tree verification: checkpoint `5c2828921f30c7d299cbc0e7aa6b7cfa9f4b3018`, run `40079824265`, job `120830854974`; evidence `docs/WP3_8_VERIFICATION.md`.
- Next: WP3.9 / #28 lock/effect visual feedback. D035 visual/update contract must be frozen before production UI source.
'''
write(p, s)

print('WP3_8_DOCS_FINALIZED=1')
