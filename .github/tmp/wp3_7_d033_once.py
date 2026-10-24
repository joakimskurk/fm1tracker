from pathlib import Path

D033 = r'''## D033 — WP3.7 uses fixed-state six-tick retrig / note-delay / note-cut scheduling without the WP3.8 queue

Date: 2026-10-24
Status: accepted for the WP3.7 one-lane proof

**Decision:** WP3.7 extends the existing `FmtStep.effect/effect_value` command domain with `0x06 RETRIG`, `0x07 NOTE_DELAY` and `0x08 NOTE_CUT`. The detailed normative semantics are frozen in `docs/WP3_7_EVENT_CONTRACT.md`.

The three commands reuse D032's exact six positions per row: tick 0 at row dispatch and ticks 1..5 at transport phases `110250`, `220500`, `330750`, `441000`, `551250`. The next row at `661500` wins over any sub-row action; no tick 6 exists.

`RETRIG` values `1..5` are tick intervals and fire at internal ticks divisible by the interval; value `0` performs no repeats and values above `5` are safe no-ops. The proof adapter resets oscillator phase only, preserving persistent pitch, row level target and amplitude state; future engines may map the same semantic action to their envelope retrigger primitive.

`NOTE_DELAY` values `1..5` withhold only an ordinary NOTE until that tick; value `0` is immediate and values above `5` do not suppress the NOTE. TIE, EMPTY/NONE and OFF are never delayed. While a NOTE waits, a previous active note may continue and receives the current row's D031 row-local parameter snapshot. The delayed NOTE then uses that same row snapshot and follows ordinary non-portamento persistent-pitch semantics.

`NOTE_CUT` values `0..5` synthesize full D025 OFF semantics at that tick without rewriting the stored row; values above `5` are safe no-ops. CUT can stop a new same-row note or an earlier sustained note and cannot be resurrected by later TIE/EMPTY/retrig ticks.

Because one row still owns only one `effect/effect_value` command, D032 pitch effects and D033 event commands cannot be simultaneous on the same row. D031 p-locks remain orthogonal and may coexist. RETRIG does not mutate persistent pitch/porta state; delayed NOTE sets persistent pitch when it lands and clears a pending porta target; CUT clears active pitch/porta/transient state through OFF semantics.

WP3.7 explicitly does **not** introduce the general WP3.8 queue. One command/value, one pending note byte and one pending-valid byte are sufficient fixed state, with a target of at most 4 bytes. RETRIG repetitions are derived mathematically from the current D032 tick and never enqueued. The existing D032 64-row effect cache is reused; no third row cache is required.

Reserved command bytes `0x09..0xFF` continue to resolve to NONE. Valid D033 commands with value above `5` never wrap modulo six and never schedule work into another row. Playback never mutates `FmtStep`, p-lock records or stored effect bytes.

`FmtStep` remains exactly 8 B, `FmtPlaybackEvent` remains exactly 4 B, D020/D021 ordering is unchanged, and all D031/D032 audio regressions remain mandatory.

**Reason:** the existing one-command-per-row model plus D032's exact six-tick clock gives a complete bounded implementation for retrig, delay and cut. A queue would add overflow and ordering machinery without solving a WP3.7 requirement and is therefore correctly deferred to WP3.8.

**Consequence:** WP3.7 source may extend D032 command validation/cache and add one tiny fixed event scheduler plus the minimal proof-voice adapter hooks. Tests must cover values `0..5` and `>5`, exact frame/tick timing, NOTE/TIE/EMPTY/OFF interaction, delayed-note D031 row values, CUT non-resurrection, RETRIG after OFF no-op, D032 persistent pitch/porta continuation, invalid-command NONE fallback, stored-data immutability and all inherited regressions. No WP3.8 queue/overflow implementation may be added under WP3.7.
'''

p = Path('docs/DECISIONS.md')
s = p.read_text()
if '\n## D033 — ' not in s:
    if not s.endswith('\n'):
        s += '\n'
    s += '\n' + D033
    p.write_text(s)

p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
repls = [
    ('State: **IN PROGRESS - WP3.6 complete; WP3.7 ready to start - 2026-10-24**',
     'State: **IN PROGRESS - WP3.7 active; D033 frozen before source - 2026-10-24**'),
    ('State: **READY TO START - no WP3.7 implementation has begun**',
     'State: **IN PROGRESS - D033 frozen; production source is the next step**'),
    ('GitHub issue: #26\n\nBefore source, freeze D033 covering:',
     'GitHub issue: #26\n\nWP3.7 start head: `e2f354018b4cbad636b51ac8ae98c9b226e7ef18`\n\nD033 is frozen in `docs/DECISIONS.md` and `docs/WP3_7_EVENT_CONTRACT.md`. It defines:'),
    ('Start WP3.7 / issue #26 as its own package. Verify actual branch HEAD, mark WP3.7 IN PROGRESS, freeze D033 before source, and do not begin WP3.8.',
     'Implement WP3.7 / #26 from the frozen D033 contract. Keep the fixed six-tick scheduler and do not begin WP3.8 queue work.'),
    ('2026-10-24 - WP3.6 verified complete. WP3.7 / #26 is READY TO START; no WP3.7 source exists.',
     '2026-10-24 - WP3.7 active. D033 frozen before source; no WP3.7 production source has been committed yet.')
]
for old, new in repls:
    assert s.count(old) == 1, (old, s.count(old))
    s = s.replace(old, new, 1)
p.write_text(s)

p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '- **WP3.7 — retrig / note delay / note cut** — READY TO START — issue #26.\n  Freeze D033 before source: exact command encoding, sub-row timing, NOTE/TIE/OFF/EMPTY interaction, retrigger/delay/cut semantics, precedence with D031/D032 and fixed real-time bounds. Do not pull the general WP3.8 queue forward unless D033 explicitly proves a minimal prerequisite is unavoidable.'
new = '- **WP3.7 — retrig / note delay / note cut** — IN PROGRESS — issue #26.\n  D033 is frozen before source in `WP3_7_EVENT_CONTRACT.md`: commands `0x06..0x08`, exact D032 six-tick scheduling, fixed pending-note state, no general queue, and explicit D031/D032 interaction. Implement only this contract; WP3.8 remains separate.'
assert s.count(old) == 1, s.count(old)
p.write_text(s.replace(old, new, 1))

print('WP3_7_D033_DOCS_PATCHED=1')
