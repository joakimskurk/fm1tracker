from pathlib import Path

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
repls = {
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 D044 registry/cache/editor activation next - 2026-10-24**':
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9/linked 8-BIT engine integration active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 dinghy/audio migration next - 2026-10-24**',
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 D044 linked registry/cache/base-bank + page-aware p-lock editor next**':
    'State: **IN PROGRESS - WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 linked dinghy/audio migration next**',
    'Begin D045 Gate 3 only: activate D044 IDs `0x20..0x25` in the linked D027 registry, add the fixed 16 × 6 B instrument base bank, publish the aligned 8 × 64 × 4 B D044 row cache from main context, and make held-step KNOB1..4 p-lock editing use the selected track\'s exact four engine-page IDs while preserving D028 max-four sparse locks. Do not modify `dinghy.c` or migrate the proof-sine tracker lane until Gate 3 is green.':
    'Begin D045 Gate 4 only: migrate `dinghy.c` from the Phase 3 proof-sine tracker lane to the bounded 8-BIT engine, seed deterministic 8-BIT instruments/base sets, link all eight tracker tracks through the Gate 1-3 publication/engine layers, preserve the separate D034 audition-sine lane, and add deterministic per-engine/mixed/eight-track/D033/precedence fixtures plus pinned target resource proof. Do not begin WP4.10 timing/Phase 4 closeout until Gate 4 is green.',
    '2026-10-24 - WP4.9 / #40 active. D045 Gates 1-2 are GREEN. Gate 2 added only the isolated 404 B `Fmt8BitEngine` pair, passed lifecycle/release-tail/malformed-note/eight-slot mix tests, all D038-D044 regressions and pinned PI32v2 compilation on run `40109064516`; replay `c2a8d4df`, object `967fba0d...`. Exact next action: Gate 3 only — D044 linked registry/cache/base bank + page-aware selected-track p-lock editor. Keep `dinghy.c` untouched.':
    '2026-10-24 - WP4.9 / #40 active. D045 Gates 1-3 are GREEN. Gate 3 activated exactly six D044 IDs, the 96 B base-bank/2048 B aligned row-cache publication layer and page-aware selected-track p-lock mutation, while `dinghy.c` and proof audio remained unchanged. Canonical run `40111447360`; evidence `WP4_9_GATE3_VERIFICATION.md`. Exact next action: Gate 4 only — migrate the tracker lane to the linked 8-BIT engine and prove integrated host/target behavior/resources.'
}
for old, new in repls.items():
    assert old in s, old
    s = s.replace(old, new, 1)
anchor = '- WP4.9 D045 Gate 2 verification: `ceee70e94e3aaa6fd266e18490f4b3ce28ee51e5` — run `40109064516`, host `120916760172`, PI32v2 `120916760440`; replay `c2a8d4df`\n'
assert anchor in s
if 'WP4.9 D045 Gate 3 verification:' not in s:
    s = s.replace(anchor, anchor + '- WP4.9 D045 Gate 3 verification: `9263b754c877dc0e6076c34538a496820e2ededb` — run `40111447360`, host `120923852664`, PI32v2 `120923852448`; evidence `WP4_9_GATE3_VERIFICATION.md`\n', 1)
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 next**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.8 / #39 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 next**'
assert old in s
s = s.replace(old, new, 1)
old = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gates 1-2 GREEN (`WP4_9_GATE1_VERIFICATION.md`, `WP4_9_GATE2_VERIFICATION.md`); Gate 3 D044 registry/cache/base-bank + page-aware p-lock editor next'
new = '- **WP4.9 linked 8-BIT engine integration** — IN PROGRESS — issue #40 — D045 FROZEN; Gates 1-3 GREEN (`WP4_9_GATE1_VERIFICATION.md`, `WP4_9_GATE2_VERIFICATION.md`, `WP4_9_GATE3_VERIFICATION.md`); Gate 4 `dinghy.c` tracker-lane migration + integrated host/target proof next'
assert old in s
s = s.replace(old, new, 1)
p.write_text(s)

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**. Gates 1-2 are GREEN; Gate 3 D044 linked registry/cache/base-bank + page-aware p-lock editor is next.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.8 / #39 are COMPLETE**; **WP4.9 / #40 linked 8-BIT engine integration is active and D045 is FROZEN**. Gates 1-3 are GREEN; Gate 4 `dinghy.c` tracker-lane migration + integrated host/target proof is next.'
assert old in s
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-2 GREEN; Gate 3 next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.8/D044 COMPLETE; WP4.9 / #40 linked 8-BIT integration active; D045 FROZEN; Gates 1-3 GREEN; Gate 4 next**'
assert old in s
s = s.replace(old, new, 1)
old = 'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; Gates 1-2 are GREEN (`40108097887`, `40109064516`). Execute Gate 3 D044 linked registry/cache/base-bank + page-aware selected-track p-lock editor next. Do not modify `dinghy.c` before Gate 3 is green. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.9 / #40 — D045 is frozen in `WP4_9_LINKED_ENGINE_CONTRACT.md`; Gates 1-3 are GREEN (`40108097887`, `40109064516`, `40111447360`). Execute Gate 4 `dinghy.c` tracker-lane migration + integrated 8-BIT host/target proof next. Preserve the separate D034 audition-sine lane and keep WP4.10 timing/closeout blocked until Gate 4 is green. Branch: `phase4/8bit-engine`.'
assert old in s
s = s.replace(old, new, 1)
p.write_text(s)

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D045 Gate 3 D044 linked registry/cache/page-aware p-lock editor | required | required | optional later | ⬜ WP4.9 Gate 3 |'
new = '| D045 Gate 3 D044 linked registry/cache/page-aware p-lock editor | required | required | optional later | ✅ run `40111447360`; 96 B base bank / 2048 B cache; frozen PI32v2 objects |'
assert old in s
s = s.replace(old, new, 1)
anchor = 'WP4.9 Gate 2 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE2_VERIFICATION.md`.\n'
assert anchor in s
if 'WP4.9 Gate 3 detailed evidence:' not in s:
    s = s.replace(anchor, anchor + 'WP4.9 Gate 3 detailed evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_GATE3_VERIFICATION.md`.\n', 1)
p.write_text(s)
