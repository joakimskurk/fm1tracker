from pathlib import Path

# DECISIONS: append D045 once.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D045 —' not in s, 'D045 already present'
s = s.rstrip() + r'''

## D045 — WP4.9 deliberately migrates the tracker proof lane to one bounded eight-track/eight-voice 8-BIT engine

Date: 2026-10-24
Status: accepted — frozen before linked engine source

**Decision:** `docs/WP4_9_LINKED_ENGINE_CONTRACT.md` is normative. WP4.9 is the first Phase 4 package allowed to change linked tracker audio. It makes pattern 0 tracks `0..7` audible through D038-D044 while preserving D020-D035 timing/editor/UI ownership and leaving the separate D034 four-voice sine audition lane unchanged.

D045 freezes project modes 8-BIT=0 and 16-BIT=1, engine identities SAFE=0/PULSE=1/NOISE=2/TRIANGLE=3/WAVETABLE-8=4, a deterministic runtime demo seed (instrument 0..3 = the four engines; track default instrument = track & 3), NOTE-only step-instrument inheritance, and safe silence for malformed/unsupported mode/engine data.

The existing four-byte `FmtPlaybackEvent` remains unchanged. A separate four-byte multi-track row event captures step/kind/note/instrument for each fixed track index. Row and D032/D033 action dispatch order is always track 0 -> 7. D031 and D032 retain their verified track-0 cache symbols and add exactly seven extra 64x32-bit track rows each. D044 activates IDs 0x20..0x25, links a 96 B sixteen-instrument base bank and the frozen 2048 B aligned row cache. Audio consumes aligned publication words only and never traverses D028.

One `Fmt8BitOscState` is 8 B, one `Fmt8BitVoice` is exactly 44 B, and one `Fmt8BitEngine` is exactly 404 B including the 52 B D043 allocator plus eight voices. Ordinary NOTE reuses oscillator phase only when the same track retains the same engine/instrument; fresh/tail-steal or engine/instrument change resets oscillator state. NOTE/RETRIG consume D042 A/D/S snapshots, OFF/CUT consume current-row RELEASE, PULSE WIDTH and WAVETABLE TABLE are live row timbre, and TIE/EMPTY never live-retime D042 A/D/S.

D031 VOICE LEVEL x TRACK LEVEL becomes one Q15 gain via `fmt_playback_level_target(32767, ...)`. D032 remains sole owner of persistent/transient pitch and D033 remains sole owner of NOTE DELAY/RETRIG/CUT timing. Per-frame render scans exactly eight slots, applies oscillator * envelope * D031 gain with bounded signed 32-bit arithmetic, sums to a 32-bit accumulator, clamps to +/-32767 and feeds mono-centre to both channels. TRACK PAN remains storage-only.

OFF/CUT starts D042 release, then D043 detaches ownership in the same event; unowned RELEASE tails continue rendering and may later be reclaimed only under D043. Allocator scans remain event-boundary O(8), never per-sample. Planned major normal-RAM delta is <=6368 B and the linked result must remain inside D037's 21,488 B normal-RAM, 132,496 B image and 129,384 B pool ceilings.

WP4.9 uses staged gates: (1) track-aware playback/D031/D032 publication with track-0 regressions green, (2) pure linked-engine state/mix tests, (3) D044 registry/cache/page-aware p-lock editing, then (4) `dinghy.c` migration and deterministic per-engine/mixed/eight-track/D033/precedence fixtures, followed by inherited regressions and pinned full firmware measurement. The historical proof-sine WAV hash is intentionally superseded only at this explicit migration checkpoint.

**Reason:** Phase 4 primitives are individually verified but not yet a product engine. D045 crosses that boundary without mixing in live-performance polyphony or the final timing benchmark, while keeping event/cache ownership and RAM cost statically bounded.

**Consequence:** No linked engine source may precede D045. WP4.9 closes only with all four engines audible through the real tracker adapter, deterministic eight-track ownership/timing/p-lock evidence, new intentional full-app hashes and resource measurements inside D037. WP4.10 remains the representative timing benchmark/Phase 4 exit gate.
''' + '\n'
p.write_text(s)

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 contract freeze next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 FROZEN; staged integration gates next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 linked integration/ownership/resource/test contract required before modifying linked production audio**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gate 1 track-aware playback/D031/D032 publication next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.9 is the first package that deliberately crosses the Phase 4 isolation boundary. D045 must be frozen before `dinghy.c`, the linked D027 registry, the 2,048 B D044 row cache, engine voice state or proof-sine audio are changed.'
new = 'D045 is frozen in `docs/WP4_9_LINKED_ENGINE_CONTRACT.md`. WP4.9 deliberately crosses the isolation boundary in staged gates: track-aware playback/D031/D032 publication first, pure linked-engine state/mix second, D044 registry/cache/editor activation third, and only then `dinghy.c` proof-sine migration.'
assert s.count(old) == 1, 'PROJECT_STATE D045 paragraph mismatch'
s = s.replace(old, new, 1)
old = 'Freeze D045 and the deterministic WP4.9 linked-integration test/resource contract in the repository before modifying `dinghy.c`, activating the six D044 IDs in the linked registry, allocating the D044 row cache or replacing the Phase 3 proof sine. Do not begin WP4.10 benchmarking yet.'
new = 'Begin D045 Gate 1 only: add bounded eight-track playback row events plus track-aware D031/D032 publication while preserving the existing track-0 APIs/symbols and inherited regressions. Do not modify `dinghy.c`, activate D044 registry IDs, allocate the D044 cache or link the engine until Gate 1 is green.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)
anchor = '- D044: WP4.8 freezes six 8-BIT semantic IDs (`0x20..0x25`), one exact four-control page per engine, a 6 B base set, bounded normalized WIDTH/TABLE mappings and a future aligned 2048 B row cache; global registry/UI/cache activation is deferred to WP4.9.\n'
d045 = '- D045: WP4.9 deliberately migrates all eight tracker tracks to one fixed eight-slot 8-BIT engine through staged gates, preserving the four-byte legacy event, D034 audition lane and D020-D035 ownership while activating track-aware caches, D044 publication and bounded D038-D043 render/mix only after their prerequisite gates are green.\n'
assert anchor in s, 'PROJECT_STATE D044 anchor missing'
if d045 not in s:
    s = s.replace(anchor, anchor + d045, 1)
if '- WP4.9 D045 contract:' not in s:
    marker = '- WP4.8 D044 verification/closeout: `9e6c5268259d50eb74a2326259a5cbf3c05d189e` — run `40104723586`, job `120903925381`\n'
    assert marker in s, 'PROJECT_STATE WP4.8 checkpoint missing'
    s = s.replace(marker, marker + '- WP4.9 D045 contract: `fd31801b8a73e27345d4575026b593fb45e10c20`\n', 1)
last = '## Last updated\n\n'
idx = s.index(last)
s = s[:idx] + last + '2026-10-24 - WP4.9 / #40 active. D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md` before linked source. Exact next action: Gate 1 only — implement/test eight-track row events plus track-aware D031/D032 caches with track-0 regressions green; do not touch `dinghy.c` or activate the linked engine yet.\n'
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 before linked source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — freeze D045 integration/ownership/resource/test contract before linked source'
new = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gate 1 track-aware playback + D031/D032 publication before engine linking'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active**, with D045 required before linked source.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**, with staged integration Gate 1 next.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 before linked source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.9 / #40 — freeze D045 linked engine integration/ownership/resource/test contract before modifying `dinghy.c`, activating D044 IDs or allocating linked engine caches. WP4.1 / #31 through WP4.8 / #39 are complete. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; execute Gate 1 track-aware playback + D031/D032 publication first, with legacy track-0 behavior retained. Do not link the engine or modify `dinghy.c` before Gate 1 is green. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D045 linked 8-BIT engine integration contract frozen before source | required | required | n/a | ⬜ WP4.9 active |'
new = '| D045 linked 8-BIT engine integration contract frozen before source | required | required | n/a | ✅ `WP4_9_LINKED_ENGINE_CONTRACT.md` / `fd31801b...` |'
assert s.count(old) == 1, 'TEST_MATRIX D045 row mismatch'
s = s.replace(old, new, 1)
if '| D045 Gate 1 eight-track row events + track-aware D031/D032 publication |' not in s:
    marker = new + '\n'
    insert = ('| D045 Gate 1 eight-track row events + track-aware D031/D032 publication | required | required | n/a | ⬜ WP4.9 Gate 1 |\n'
              '| D045 Gate 2 fixed 404 B engine state + deterministic oscillator/envelope/mix | required | required | optional later | ⬜ WP4.9 Gate 2 |\n'
              '| D045 Gate 3 D044 linked registry/cache/page-aware p-lock editor | required | required | optional later | ⬜ WP4.9 Gate 3 |\n')
    s = s.replace(marker, marker + insert, 1)
p.write_text(s)

print('WP4_9_D045_DOC_FREEZE_OK=1 tracks=8 voices=8 event_bytes=4 engine_bytes=404 staged_gates=1,2,3,4')
