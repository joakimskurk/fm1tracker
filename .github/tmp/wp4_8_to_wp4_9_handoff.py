from pathlib import Path


def replace_between(text, start_marker, end_marker, replacement):
    start = text.index(start_marker)
    end = text.index(end_marker, start)
    return text[:start] + replacement + text[end:]

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
assert 'Current work package: WP4.8 / #39' in s, 'PROJECT_STATE not on WP4.8/#39'
assert 'D044 FROZEN' in s, 'PROJECT_STATE missing D044 freeze'
s = s.replace('Current work package: WP4.8 / #39', 'Current work package: WP4.9 / #40', 1)
old_state = 'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8/parameter pages active; D044 FROZEN; isolated implementation/tests next - 2026-10-24**'
new_state = 'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 contract freeze next - 2026-10-24**'
assert old_state in s, 'PROJECT_STATE top state mismatch'
s = s.replace(old_state, new_state, 1)
next_block = '''## Next milestone

**WP4.9 - linked 8-BIT engine integration**

State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 linked integration/ownership/resource/test contract required before modifying linked production audio**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — COMPLETE
WP4.4 issue: #34 — COMPLETE
WP4.5 issue: #35 — COMPLETE
WP4.6 issue: #36 — COMPLETE
WP4.7 issue: #37 — COMPLETE
WP4.8 issue: #39 — COMPLETE
WP4.9 issue: #40 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.8 closed on run `40104723586`, job `120903925381`. D044 parameter pages passed six-ID/four-page deterministic verification, all 256 WIDTH/TABLE mappings, 4,096 exhaustive engine×slot×value resolver cases, pinned PI32v2 compilation and full-app closeout with zero linked delta. Evidence: `docs/WP4_8_VERIFICATION.md`.

WP4.9 is the first package that deliberately crosses the Phase 4 isolation boundary. D045 must be frozen before `dinghy.c`, the linked D027 registry, the 2,048 B D044 row cache, engine voice state or proof-sine audio are changed.

D045 must define exact eight-track/eight-voice ownership, engine selection/fallback, D044 registry/cache activation, D031/D032 pitch/level precedence, D033 NOTE DELAY/RETRIG/CUT ordering, D042 articulation, D043 allocation/release-tail semantics, oscillator reset rules, mix/clamp ownership, proof-sine migration, deterministic per-engine/mixed fixtures, linked resource ceilings and the boundary to WP4.10 benchmarking.

Do not begin the final eight-track benchmark/Phase 4 closeout until WP4.9 integrated host and target evidence is green.

'''
s = replace_between(s, '## Next milestone\n', '## Important decisions / do not regress\n', next_block)
exact_block = '''## Exact next action

Freeze D045 and the deterministic WP4.9 linked-integration test/resource contract in the repository before modifying `dinghy.c`, activating the six D044 IDs in the linked registry, allocating the D044 row cache or replacing the Phase 3 proof sine. Do not begin WP4.10 benchmarking yet.

'''
s = replace_between(s, '## Exact next action\n', '## Last relevant checkpoints\n', exact_block)
if '- WP4.8 D044 verification/closeout:' not in s:
    anchor = '- WP4.8 D044 contract:'
    idx = s.index(anchor)
    line_end = s.index('\n', idx)
    s = s[:line_end+1] + '- WP4.8 D044 verification/closeout: `9e6c5268259d50eb74a2326259a5cbf3c05d189e` — run `40104723586`, job `120903925381`\n' + s[line_end+1:]
last_block = '''## Last updated

2026-10-24 - WP4.8 / #39 COMPLETE. D044 six-ID/four-page parameter resolver passed exhaustive deterministic host verification, pinned PI32v2 compilation and full-app closeout with zero linked delta; run `40104723586`, job `120903925381`. WP4.9 / #40 is now active. Exact next action: freeze D045 linked engine integration/ownership/resource/test contract before changing the linked audio path.
'''
s = s[:s.index('## Last updated\n')] + last_block
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
start = s.index('# Phase 4 — 8-BIT engine')
end = s.index('---\n\n# Phase 5', start)
block = s[start:end]
lines = block.splitlines()
for i, line in enumerate(lines):
    if line.startswith('Status: **'):
        lines[i] = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 before linked source**'
    elif line.startswith('- **WP4.8 four-parameter pages'):
        lines[i] = '- **WP4.8 four-parameter pages + p-lock mapping** — COMPLETE — issue #39 — D044 + `WP4_8_VERIFICATION.md`; six stable IDs, bounded resolver and zero linked full-app delta'
    elif line.startswith('- WP4.9 ') or line.startswith('- **WP4.9 '):
        lines[i] = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — freeze D045 integration/ownership/resource/test contract before linked source'
new_block = '\n'.join(lines) + '\n'
s = s[:start] + new_block + s[end:]
p.write_text(s)

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
assert 'WP4.8' in s and '#39' in s, 'ROADMAP missing WP4.8 handoff state'
phase4_start = s.index('## Phase 4 — 8-BIT engine')
phase5_start = s.index('## Phase 5 — 16-BIT FM4 engine', phase4_start)
prefix = s[:phase4_start]
phase4 = s[phase4_start:phase5_start]
# Replace the lead paragraph immediately before Phase 4 when present.
prefix_lines = prefix.splitlines()
for i, line in enumerate(prefix_lines):
    if line.startswith('Phase 4 is active.'):
        prefix_lines[i] = 'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active**, with D045 required before linked source.'
prefix = '\n'.join(prefix_lines) + ('\n' if prefix.endswith('\n') else '')
phase_lines = phase4.splitlines()
for i, line in enumerate(phase_lines):
    if line.startswith('Status: **'):
        phase_lines[i] = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 before linked source**'
    elif line.startswith('Active package:'):
        phase_lines[i] = 'Active package: WP4.9 / #40 — freeze D045 linked engine integration/ownership/resource/test contract before modifying `dinghy.c`, activating D044 IDs or allocating linked engine caches. WP4.1 / #31 through WP4.8 / #39 are complete. Branch: `phase4/8bit-engine`.'
phase4 = '\n'.join(phase_lines) + '\n'
p.write_text(prefix + phase4 + s[phase5_start:])

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
repls = {
    '| D044 six IDs + four-engine pages + all-256 WIDTH/TABLE mappings + bounded p-lock fallback | required | required | optional later | ⬜ WP4.8 next |':
    '| D044 six IDs + four-engine pages + all-256 WIDTH/TABLE mappings + bounded p-lock fallback | required | required | optional later | ✅ run `40104638904` / closeout `40104723586` |',
    '| WP4.8 full app remains exact WP4.7 baseline while parameter layer is isolated | required | required | n/a | ⬜ WP4.8 next |':
    '| WP4.8 full app remains exact WP4.7 baseline while parameter layer is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |',
}
for old, new in repls.items():
    assert old in s, f'TEST_MATRIX missing: {old}'
    s = s.replace(old, new, 1)
if '| D045 linked 8-BIT engine integration contract frozen before source |' not in s:
    marker = '| P-lock modulation stable | required | required | required later | ⬜ WP4.8-WP4.9 |'
    assert marker in s, 'TEST_MATRIX p-lock marker missing'
    insert = ('| D045 linked 8-BIT engine integration contract frozen before source | required | required | n/a | ⬜ WP4.9 active |\n'
              '| Integrated PULSE/NOISE/TRIANGLE/WAVETABLE host fixtures through real adapter | required | required | optional later | ⬜ WP4.9 |\n'
              '| Integrated NOTE/TIE/OFF/DELAY/RETRIG/CUT + D031/D032/D044 precedence | required | required | optional later | ⬜ WP4.9 |\n'
              '| Linked engine resources remain inside D037 ceilings | inspect/build | required | n/a | ⬜ WP4.9 |\n')
    s = s.replace(marker, insert + '| P-lock modulation stable | required | required | required later | ⬜ WP4.9 |', 1)
if 'WP4.8 detailed evidence:' not in s:
    marker = 'WP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.\n'
    assert marker in s, 'TEST_MATRIX evidence marker missing'
    s = s.replace(marker, marker + 'WP4.8 detailed evidence: `WP4_8_PARAMETER_PAGE_CONTRACT.md`, `WP4_8_VERIFICATION.md`.\n', 1)
p.write_text(s)

print('WP4_8_TO_WP4_9_HANDOFF_OK=1 wp4_8=complete wp4_9=active next=D045')
