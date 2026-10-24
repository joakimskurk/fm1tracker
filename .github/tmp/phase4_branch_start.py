from pathlib import Path


def read(path):
    return Path(path).read_text()


def write(path, text):
    Path(path).write_text(text.rstrip() + '\n')


def once(text, old, new, label):
    n = text.count(old)
    assert n == 1, f'{label}: expected 1 match, got {n}'
    return text.replace(old, new, 1)


def section(text, start_heading, next_heading, replacement, label):
    start = text.find(start_heading)
    assert start >= 0, f'{label}: start missing'
    end = text.find(next_heading, start + len(start_heading))
    assert end >= 0, f'{label}: end missing'
    return text[:start] + replacement.rstrip() + '\n\n' + text[end:]

# PROJECT_STATE — activate Phase 4/WP4.1 on the new phase branch.
p = 'docs/PROJECT_STATE.md'
s = read(p)
current = '''## Current phase

**Phase 4 - 8-BIT engine**

State: **IN PROGRESS - WP4.1 active; D037 engine-interface architecture freeze next - 2026-10-24**

Parent issue: #30
Current work package: WP4.1 / #31
Current branch: `phase4/8bit-engine`

Phase 4 branch start / final Phase 3 handoff:

`d14955f23a11044e8a348884feca24f8978fd700`

Final Phase 3 production firmware baseline inherited by Phase 4:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

Final Phase 3 closeout run/job:

`40084706845` / `120845144453`
'''
s = section(s, '## Current phase\n', '## Completed Phase 3 packages\n', current, 'PROJECT_STATE Phase4 current')

old = '''**Phase 4 / WP4.1 - 8-BIT engine interface contract**

State: **READY TO START - create/use `phase4/8bit-engine`, freeze D037 before production engine source**

Phase 4 parent issue: #30
WP4.1 issue: #31

D037 must freeze engine/voice ownership, fixed-point pitch, note/tie/off/retrigger adapter semantics, per-track/per-voice state, sample/mix format, p-lock/effect integration boundary, realtime work bounds and the complete Phase 4 resource budget before PULSE/NOISE/TRIANGLE/WAVETABLE-8 production DSP is written.

The Phase 3 proof sine remains the current firmware audio implementation until a later Phase 4 package deliberately replaces/integrates it. Do not pull WP4.2+ implementation into WP4.1.'''
new = '''**WP4.1 - 8-BIT engine interface contract**

State: **IN PROGRESS - D037 architecture freeze next; no production engine DSP begun**

Phase 4 parent issue: #30
WP4.1 issue: #31
Branch: `phase4/8bit-engine`

D037 must freeze engine/voice ownership, fixed-point pitch, note/tie/off/retrigger adapter semantics, per-track/per-voice state, sample/mix format, p-lock/effect integration boundary, realtime work bounds and the complete Phase 4 resource budget before PULSE/NOISE/TRIANGLE/WAVETABLE-8 production DSP is written.

The Phase 3 proof sine remains the current firmware audio implementation until a later Phase 4 package deliberately replaces/integrates it. Do not pull WP4.2+ implementation into WP4.1.'''
s = once(s, old, new, 'PROJECT_STATE WP4.1 milestone')
s = once(s,
    'Create/use the Phase 4 branch `phase4/8bit-engine`, then start WP4.1 / #31 by freezing D037 in the repository **before any production oscillator/engine DSP source**. Preserve every Phase 2/3 deterministic regression as the migration baseline.',
    'Freeze D037 for WP4.1 / #31 in the repository **before any production oscillator/engine DSP source**. Preserve every Phase 2/3 deterministic regression as the migration baseline and define explicit Phase 4 image/RAM/pool headroom before WP4.2.',
    'PROJECT_STATE exact action')
lu = s.find('## Last updated\n')
assert lu >= 0
s = s[:lu] + '''## Last updated

2026-10-24 - Phase 4 branch `phase4/8bit-engine` created from clean Phase 3 handoff `d14955f2...`. #30 and WP4.1 / #31 are IN PROGRESS. Exact next action is the D037 engine-interface architecture freeze; no Phase 4 production engine DSP has begun.
'''
write(p, s)

# WORK_PACKAGES — activate WP4.1 only.
p = 'docs/WORK_PACKAGES.md'
s = read(p)
s = once(s,
    'Status: **READY TO START — WP4.1 / #31; D037 required before production engine source**',
    'Status: **IN PROGRESS — WP4.1 / #31 active; D037 required before production engine source**',
    'WORK_PACKAGES Phase4 status')
s = once(s,
    '- **WP4.1 engine interface contract** — READY TO START — issue #31',
    '- **WP4.1 engine interface contract** — IN PROGRESS — issue #31 — freeze D037 before source',
    'WORK_PACKAGES WP4.1')
write(p, s)

# ROADMAP — Phase 4 active, still architecture-only.
p = 'docs/ROADMAP.md'
s = read(p)
s = once(s,
    'Status: **READY TO START — WP4.1 architecture first**',
    'Status: **IN PROGRESS — WP4.1 architecture active; D037 before source**',
    'ROADMAP Phase4 status')
s = once(s,
    'Next package: WP4.1 / #31 — freeze D037 before engine source.',
    'Active package: WP4.1 / #31 — freeze D037 before engine source. Branch: `phase4/8bit-engine`.',
    'ROADMAP Phase4 package')
write(p, s)

print('PHASE4_BRANCH_START_OK=1')
