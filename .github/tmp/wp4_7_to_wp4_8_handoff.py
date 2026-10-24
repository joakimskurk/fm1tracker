from pathlib import Path

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7/allocator active; D043 FROZEN; isolated implementation/tests next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.7 / #37'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8/parameter pages active; D044 contract freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.8 / #39'
assert s.count(old) == 1, 'PROJECT_STATE top mismatch'
s = s.replace(old, new, 1)

start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n', start)
next_block = '''## Next milestone

**WP4.8 - four-parameter engine pages + p-lock mapping**

State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 active; D044 stable-ID/page/default/domain/publication contract required before parameter source**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — COMPLETE
WP4.4 issue: #34 — COMPLETE
WP4.5 issue: #35 — COMPLETE
WP4.6 issue: #36 — COMPLETE
WP4.7 issue: #37 — COMPLETE
WP4.8 issue: #39 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.7 closed on run `40103427588`, job `120900076833`. D043 deterministic allocator passed all-eight allocation order, all 256 IDLE/RELEASE masks, same-track reuse, stale-IDLE cleanup, detached release-tail stealing, no active cross-track steal, saturation/corruption/replay/hash verification, real PI32v2 compilation and a full-app closeout with zero linked delta. Evidence: `docs/WP4_7_VERIFICATION.md`.

The complete D038-D043 oscillator/envelope/allocator primitive set remains isolated from `dinghy.c`; the running full app therefore remains the exact Phase 3/WP4.1 baseline through WP4.7.

WP4.8 must freeze D044 before production parameter-page source. D044 must define stable semantic IDs from the reserved Phase 4 range `0x20..0x5F`, exactly four exposed controls per 8-BIT engine, parameter order/names/defaults/normalized domains, exact normalized-byte mappings into D038-D042 controls, lockability and D027/D031 publication behavior, malformed/unsupported behavior, base-value versus row-local override ownership, bounded snapshot representation, deterministic vectors and zero-full-app-delta isolation boundary.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.8 parameter-layer work. Do not pull WP4.9 proof-sine migration or full engine integration forward.

'''
s = s[:start] + next_block + s[end:]

old = 'Implement and verify isolated `fmt_voice_allocator.*` exactly under D043 with ownership/free/release-tail/recency/saturation/corruption/replay host tests, static realtime audit and pinned PI32v2 target compilation. Keep D038-D042 primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.8 or full engine integration.'
new = 'Freeze D044 and the focused WP4.8 parameter-page/p-lock publication test contract in the repository before creating parameter-page production source. Keep D038-D043 primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)

checkpoint_anchor = '- WP4.7 D043 contract: `1215c89aa48cd2727c10c6af327fdc0052d72562`\n'
checkpoint = '- WP4.7 D043 verification/closeout: `bb25dad92ad71709679589376c039fbfe0a6d92c` — run `40103427588`, job `120900076833`\n- WP4.7 final static-audit correction: `14fc240486f2cf72bfaafcb072fec2c946cc4c50` — run `40103577801` SUCCESS\n'
assert checkpoint_anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if 'WP4.7 D043 verification/closeout:' not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)

last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.7 / #37 COMPLETE. D043 allocator passed focused host/PI32v2 verification and final zero-delta target closeout; final audit correction run `40103577801` is green. WP4.8 / #39 is now active. Exact next action: freeze D044 stable engine parameter IDs, four-control page/default/domain mappings and bounded D027/D031 p-lock/publication semantics before production source.\n'
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.6 / #36 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.7 / #37 COMPLETE; WP4.8 / #39 four-parameter pages + p-lock mapping active; D044 before source**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.7 deterministic voice allocation/stealing** — IN PROGRESS — issue #37 — D043 FROZEN; isolated allocator implementation + ownership/release-tail/saturation/PI32v2 verification next\n- WP4.8 four-parameter pages + p-lock mapping'
new = '- **WP4.7 deterministic voice allocation/stealing** — COMPLETE — issue #37 — D043 + `WP4_7_VERIFICATION.md`; deterministic release-tail allocator host/PI32v2 proof and zero linked full-app delta\n- **WP4.8 four-parameter pages + p-lock mapping** — IN PROGRESS — issue #39 — freeze D044 stable IDs/page/default/domain/publication contract before source'
assert s.count(old) == 1, 'WORK_PACKAGES packages mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.6 / #36 are COMPLETE**; **WP4.7 / #37 deterministic voice allocation/stealing is active and D043 is FROZEN**, with isolated deterministic implementation/testing next.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.7 / #37 are COMPLETE**; **WP4.8 / #39 four-parameter engine pages + p-lock mapping is active**, with D044 required before source.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 parameter pages + p-lock mapping active; D044 before source**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.7 / #37 — D043 is frozen in `WP4_7_ALLOCATOR_CONTRACT.md`; implement/test isolated fixed eight-slot allocator with release-tail stealing only and no active cross-track steal. WP4.1 / #31 through WP4.6 / #36 are complete; D038-D042 primitives remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.8 / #39 — freeze D044 stable Phase 4 engine parameter IDs, exactly four controls per 8-BIT engine, defaults/domains/value mappings and bounded D027/D031 p-lock/publication semantics before production source. WP4.1 / #31 through WP4.7 / #37 are complete; D038-D043 primitives remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| Voice allocation/stealing deterministic | required | required | required later | ⬜ WP4.7 next |\n| P-lock modulation stable | required | required | required later | ⬜ WP4.8-WP4.9 |'
new = '| Voice allocation/stealing deterministic | required | required | required later | ✅ run `40103284377` / closeout `40103427588`; audit `40103577801` |\n| WP4.7 full app remains exact WP4.6 baseline while allocator is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |\n| D044 engine parameter-page/p-lock publication contract frozen before source | required | compile | n/a | ⬜ WP4.8 active |\n| P-lock modulation stable | required | required | required later | ⬜ WP4.8-WP4.9 |'
assert s.count(old) == 1, 'TEST_MATRIX rows mismatch'
s = s.replace(old, new, 1)
old = 'WP4.6 detailed evidence: `WP4_6_ENVELOPE_CONTRACT.md`, `WP4_6_VERIFICATION.md`.\n'
new = 'WP4.6 detailed evidence: `WP4_6_ENVELOPE_CONTRACT.md`, `WP4_6_VERIFICATION.md`.\nWP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.\n'
assert s.count(old) == 1, 'TEST_MATRIX evidence mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_7_TO_WP4_8_HANDOFF_OK=1 wp4_7=complete wp4_8=active issue=39 next=D044')
