from pathlib import Path

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gate 1 track-aware playback/D031/D032 publication next**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gate 1 GREEN; Gate 2 pure linked-engine state/voice/render/mix next**'
assert old in s
s = s.replace(old, new, 1)
old = '## Exact next action\n\nBegin D045 Gate 1 only: add bounded eight-track playback row events plus track-aware D031/D032 publication while preserving the existing track-0 APIs/symbols and inherited regressions. Do not modify `dinghy.c`, activate D044 registry IDs, allocate the D044 cache or link the engine until Gate 1 is green.\n'
new = '## Exact next action\n\nBegin D045 Gate 2 only: implement and verify the pure linked-engine state/voice/render/mix layer from `WP4_9_LINKED_ENGINE_CONTRACT.md` — exact `Fmt8BitOscState` 8 B, `Fmt8BitVoice` 44 B and `Fmt8BitEngine` 404 B layouts, deterministic oscillator/envelope lifecycle, release tails, engine dispatch and bounded eight-slot Q15/32-bit mix. Do not modify `dinghy.c` or activate the D044 linked registry/cache/editor path until Gate 2 is green.\n'
assert old in s
s = s.replace(old, new, 1)
anchor = '- WP4.9 D045 contract: `fd31801b8a73e27345d4575026b593fb45e10c20`\n'
assert anchor in s
if '- WP4.9 D045 Gate 1 verification:' not in s:
    s = s.replace(anchor, anchor + '- WP4.9 D045 Gate 1 verification: `362c3f825791e5def2c9367fc27e6032018e8539` — run `40108097887`, host `120913890803`, PI32v2 `120913891037`\n', 1)
start = s.index('## Last updated\n')
s = s[:start] + '''## Last updated

2026-10-24 - WP4.9 / #40 active. D045 is frozen and Gate 1 is GREEN: eight-track row dispatch plus track-aware D031/D032 publication passed source-boundary, inherited host regressions, exact proof-audio identity and pinned PI32v2 compilation on run `40108097887`. Exact next action: Gate 2 only — pure 404 B linked-engine state/voice/render/mix; keep `dinghy.c` and Gate 3 registry/cache/editor activation untouched.
'''
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 next**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 GREEN; Gate 2 next**'
assert old in s
s = s.replace(old, new, 1)
old = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gate 1 track-aware playback + D031/D032 publication before engine linking'
new = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gate 1 GREEN (`WP4_9_GATE1_VERIFICATION.md`); Gate 2 pure 404 B engine state/voice/render/mix next'
assert old in s
s = s.replace(old, new, 1)
p.write_text(s)

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
repls = {
'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**, with staged integration Gate 1 next.':
'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**. Gate 1 is GREEN; Gate 2 pure linked-engine state/voice/render/mix is next.',
'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 next**':
'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 GREEN; Gate 2 next**',
'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; execute Gate 1 track-aware playback + D031/D032 publication first, with legacy track-0 behavior retained. Do not link the engine or modify `dinghy.c` before Gate 1 is green. Branch: `phase4/8bit-engine`.':
'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; Gate 1 track-aware playback + D031/D032 publication is GREEN on run `40108097887`. Execute Gate 2 pure linked-engine state/voice/render/mix next. Do not activate Gate 3 registry/cache/editor work or modify `dinghy.c` before Gate 2 is green. Branch: `phase4/8bit-engine`.'
}
for old, new in repls.items():
    assert old in s, old
    s = s.replace(old, new, 1)
p.write_text(s)

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D045 Gate 1 eight-track row events + track-aware D031/D032 publication | required | required | n/a | ⬜ WP4.9 Gate 1 |'
new = '| D045 Gate 1 eight-track row events + track-aware D031/D032 publication | required | required | n/a | ✅ run `40108097887`; host `120913890803`; PI32v2 `120913891037` |'
assert old in s
s = s.replace(old, new, 1)
anchor = 'WP4.8 detailed evidence: `WP4_8_PARAMETER_PAGE_CONTRACT.md`, `WP4_8_VERIFICATION.md`.\n'
assert anchor in s
if 'WP4.9 Gate 1 detailed evidence:' not in s:
    s = s.replace(anchor, anchor + 'WP4.9 Gate 1 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE1_VERIFICATION.md`.\n', 1)
p.write_text(s)

print('WP4_9_GATE1_TO_GATE2_STATE_OK=1 gate1=green gate2=next dinghy=blocked gate3=blocked')
