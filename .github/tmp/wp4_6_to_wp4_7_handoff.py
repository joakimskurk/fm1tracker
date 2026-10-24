from pathlib import Path

# PROJECT_STATE: close WP4.6 and make WP4.7 the single active package.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6/envelope active; D042 FROZEN; isolated implementation/tests next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.6 / #36'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7/allocator active; D043 contract freeze next - 2026-10-24**\n\nParent issue: #30\nCurrent work package: WP4.7 / #37'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)

start = s.index('## Next milestone\n')
end = s.index('## Important decisions / do not regress\n', start)
next_block = '''## Next milestone

**WP4.7 - deterministic voice allocation and stealing**

State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 active; D043 ownership/allocation/steal/tie-break contract required before allocator source**

Phase 4 parent issue: #30
WP4.1 issue: #31 — COMPLETE
WP4.2 issue: #32 — COMPLETE
WP4.3 issue: #33 — COMPLETE
WP4.4 issue: #34 — COMPLETE
WP4.5 issue: #35 — COMPLETE
WP4.6 issue: #36 — COMPLETE
WP4.7 issue: #37 — IN PROGRESS
Branch: `phase4/8bit-engine`

WP4.6 closed on run `40102339789`, job `120896886645`. D042 common envelope passed all-rate/sustain/event/stage/zero-time/hash deterministic verification, real PI32v2 compilation and a full-app closeout with zero linked delta. Evidence: `docs/WP4_6_VERIFICATION.md`.

The complete starting oscillator palette plus D042 envelope remain isolated from `dinghy.c`; the running full app therefore remains the exact Phase 3/WP4.1 baseline through WP4.6.

WP4.7 must freeze D043 before production allocator source. D043 must define exact eight-slot ownership state, one-primary-voice-per-track reuse, free/IDLE/release selection, deterministic steal priority and tie-breaks, age/wrap semantics, reset/preserve behavior on allocation or steal, D037/D042 NOTE/TIE/OFF/RETRIG interaction, NO_VOICE behavior, bounded work and focused deterministic evidence.

The Phase 3 proof sine remains the full-app production audio implementation during isolated WP4.7 allocator work. Do not pull engine parameter pages or full engine integration forward.

'''
s = s[:start] + next_block + s[end:]

checkpoint_anchor = '- WP4.6 D042 contract: `983757fc3fc48d884ade9b9fdb59df0a1d070f9c`\n'
checkpoint = '- WP4.6 D042 verification/closeout: `14a9f06ab3d808cf9579d53e881ef75a2fa30486` — run `40102339789`, job `120896886645`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE D042 checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)

old = 'Implement and verify isolated `fmt_envelope.*` exactly under D042 with full rate/sustain/event/stage/zero-time/default-timing/hash host tests, static realtime audit and pinned PI32v2 target compilation. Keep all four oscillator primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.7 or full engine integration.'
new = 'Freeze D043 and the focused WP4.7 deterministic allocator/steal test contract in the repository before creating allocator production source. Keep all four oscillator primitives and D042 envelope isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)

last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.6 / #36 COMPLETE. D042 common envelope passed deterministic all-rate/sustain/event/stage/zero-time/hash verification, independent PI32v2 compilation and full-app closeout with zero linked delta; run `40102339789`, job `120896886645`. WP4.7 / #37 is now active. Exact next action: freeze D043 deterministic allocator ownership/free/release/steal/tie-break/age-wrap/test contract before production source.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = '''Status: **IN PROGRESS — WP4.1 / #31 through WP4.5 / #35 COMPLETE; WP4.6 / #36 common envelope active; D042 FROZEN; isolated implementation/tests next**

Parent issue: #30'''
new = '''Status: **IN PROGRESS — WP4.1 / #31 through WP4.6 / #36 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 before source**

Parent issue: #30'''
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.6 common envelope primitive** — IN PROGRESS — issue #36 — D042 FROZEN; isolated ADSR implementation + rate/event/stage/PI32v2 verification next\n- WP4.7 deterministic voice allocation/stealing'
new = '- **WP4.6 common envelope primitive** — COMPLETE — issue #36 — D042 + `WP4_6_VERIFICATION.md`; deterministic all-rate/sustain/event/stage host/PI32v2 proof and zero linked full-app delta\n- **WP4.7 deterministic voice allocation/stealing** — IN PROGRESS — issue #37 — freeze D043 ownership/free/release/steal/tie-break/age-wrap/test contract before source'
assert s.count(old) == 1, 'WORK_PACKAGES package rows mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 through WP4.5 / #35 are COMPLETE**; **WP4.6 / #36 common envelope is active and D042 is FROZEN**, with isolated deterministic implementation/testing next.'
new = 'Phase 4 is active. **WP4.1 / #31 through WP4.6 / #36 are COMPLETE**; **WP4.7 / #37 deterministic voice allocation/stealing is active**, with D043 required before source.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.5/D041 COMPLETE; WP4.6 / #36 common envelope active; D042 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 before source**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.6 / #36 — D042 is frozen in `WP4_6_ENVELOPE_CONTRACT.md`; implement/test isolated common linear ADSR without full-app integration. WP4.1 / #31 through WP4.5 / #35 are complete; all four oscillator primitives remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.7 / #37 — freeze D043 deterministic allocator ownership/free/release/steal/tie-break/age-wrap/test contract before production source. WP4.1 / #31 through WP4.6 / #36 are complete; all four oscillator primitives plus D042 envelope remain isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| Envelope stage/time/gate/retrigger/release vectors + JieLi target compile | required | required | optional later | ⬜ WP4.6 next |\n| Voice stealing deterministic | required | required | required later | ⬜ WP4.7 |'
new = '| Envelope stage/time/gate/retrigger/release vectors + JieLi target compile | required | required | optional later | ✅ run `40102268841` / closeout `40102339789` |\n| WP4.6 full app remains exact WP4.5 baseline while envelope is isolated | required | required | n/a | ✅ 0 B linked delta / WAV+BIN+FWSC identical |\n| D043 deterministic allocator/steal contract frozen before source | required | compile | n/a | ⬜ WP4.7 active |\n| Voice allocation/stealing deterministic | required | required | required later | ⬜ WP4.7 next |'
assert s.count(old) == 1, 'TEST_MATRIX allocator insertion mismatch'
s = s.replace(old, new, 1)
old = 'WP4.5 detailed evidence: `WP4_5_WAVETABLE8_CONTRACT.md`, `WP4_5_VERIFICATION.md`.'
new = 'WP4.5 detailed evidence: `WP4_5_WAVETABLE8_CONTRACT.md`, `WP4_5_VERIFICATION.md`.\nWP4.6 detailed evidence: `WP4_6_ENVELOPE_CONTRACT.md`, `WP4_6_VERIFICATION.md`.'
assert s.count(old) == 1, 'TEST_MATRIX evidence anchor mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_6_TO_WP4_7_HANDOFF_OK=1 wp4_6=complete wp4_7=active next=D043')
