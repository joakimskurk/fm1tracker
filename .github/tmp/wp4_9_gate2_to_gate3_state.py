from pathlib import Path

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
s = s.replace(
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 FROZEN; staged integration gates next - 2026-10-24**',
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 D044 registry/cache/editor activation next - 2026-10-24**',
    1)
s = s.replace(
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gate 1 GREEN; Gate 2 pure linked-engine state/voice/render/mix next**',
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 D044 linked registry/cache/base-bank + page-aware p-lock editor next**',
    1)
old = '## Exact next action\n\nBegin D045 Gate 2 only: implement and verify the pure linked-engine state/voice/render/mix layer from `WP4_9_LINKED_ENGINE_CONTRACT.md` — exact `Fmt8BitOscState` 8 B, `Fmt8BitVoice` 44 B and `Fmt8BitEngine` 404 B layouts, deterministic oscillator/envelope lifecycle, release tails, engine dispatch and bounded eight-slot Q15/32-bit mix. Do not modify `dinghy.c` or activate the D044 linked registry/cache/editor path until Gate 2 is green.\n'
new = '## Exact next action\n\nBegin D045 Gate 3 only: activate D044 IDs `0x20..0x25` in the linked D027 registry, add the fixed 16 × 6 B instrument base bank, publish the aligned 8 × 64 × 4 B D044 row cache from main context, and make held-step KNOB1..4 p-lock editing use the selected track\'s exact four engine-page IDs while preserving D028 max-four sparse locks. Do not modify `dinghy.c` or migrate the proof-sine tracker lane until Gate 3 is green.\n'
assert old in s
s = s.replace(old, new, 1)
anchor = '- WP4.9 D045 Gate 1 verification: `362c3f825791e5def2c9367fc27e6032018e8539` — run `40108097887`, host `120913890803`, PI32v2 `120913891037`\n'
assert anchor in s
if '- WP4.9 D045 Gate 2 verification:' not in s:
    s = s.replace(anchor, anchor + '- WP4.9 D045 Gate 2 verification: `ceee70e94e3aaa6fd266e18490f4b3ce28ee51e5` — run `40109064516`, host `120916760172`, PI32v2 `120916760440`; replay `c2a8d4df`\n', 1)
start = s.index('## Last updated\n')
s = s[:start] + '''## Last updated

2026-10-24 - WP4.9 / #40 active. D045 Gates 1-2 are GREEN. Gate 2 added only the isolated 404 B `Fmt8BitEngine` pair, passed lifecycle/release-tail/malformed-note/eight-slot mix tests, all D038-D044 regressions and pinned PI32v2 compilation on run `40109064516`; replay `c2a8d4df`, object `967fba0d...`. Exact next action: Gate 3 only — D044 linked registry/cache/base bank + page-aware selected-track p-lock editor. Keep `dinghy.c` untouched.
'''
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
s = s.replace(
    'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 GREEN; Gate 2 next**',
    'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 next**',
    1)
s = s.replace(
    '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gate 1 GREEN (`WP4_9_GATE1_VERIFICATION.md`); Gate 2 pure 404 B engine state/voice/render/mix next',
    '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gates 1-2 GREEN (`WP4_9_GATE1_VERIFICATION.md`, `WP4_9_GATE2_VERIFICATION.md`); Gate 3 D044 registry/cache/base-bank + page-aware p-lock editor next',
    1)
p.write_text(s)

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
repls = {
    'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**. Gate 1 is GREEN; Gate 2 pure linked-engine state/voice/render/mix is next.':
    'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**. Gates 1-2 are GREEN; Gate 3 D044 linked registry/cache/base-bank + page-aware p-lock editor is next.',
    'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gate 1 GREEN; Gate 2 next**':
    'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 next**',
    'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; Gate 1 track-aware playback + D031/D032 publication is GREEN on run `40108097887`. Execute Gate 2 pure linked-engine state/voice/render/mix next. Do not activate Gate 3 registry/cache/editor work or modify `dinghy.c` before Gate 2 is green. Branch: `phase4/8bit-engine`.':
    'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; Gates 1-2 are GREEN (`40108097887`, `40109064516`). Execute Gate 3 D044 linked registry/cache/base-bank + page-aware selected-track p-lock editor next. Do not modify `dinghy.c` before Gate 3 is green. Branch: `phase4/8bit-engine`.'
}
for old, new in repls.items():
    assert old in s, old
    s = s.replace(old, new, 1)
p.write_text(s)

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D045 Gate 2 fixed 404 B engine state + deterministic oscillator/envelope/mix | required | required | optional later | ⬜ WP4.9 Gate 2 |'
new = '| D045 Gate 2 fixed 404 B engine state + deterministic oscillator/envelope/mix | required | required | optional later | ✅ run `40109064516`; replay `c2a8d4df`; PI32v2 object `967fba0d...` |'
assert old in s
s = s.replace(old, new, 1)
anchor = 'WP4.9 Gate 1 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE1_VERIFICATION.md`.\n'
assert anchor in s
if 'WP4.9 Gate 2 detailed evidence:' not in s:
    s = s.replace(anchor, anchor + 'WP4.9 Gate 2 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE2_VERIFICATION.md`.\n', 1)
p.write_text(s)

print('WP4_9_GATE2_TO_GATE3_STATE_OK=1 gate1=green gate2=green gate3=next dinghy=blocked')
