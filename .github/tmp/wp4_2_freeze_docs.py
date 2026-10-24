from pathlib import Path

# D038 was already committed by the first successful one-time run. This final
# pass is deliberately idempotent and only closes the remaining PROJECT_STATE
# handoff gap discovered during post-commit review.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D038 — WP4.2 PULSE is a 4-byte hard-edge oscillator using the existing Q7 pitch math' in s
assert 'pulse width is one unsigned byte' in s.lower()

p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
assert 'D038 FROZEN; focused PULSE implementation/tests next' in s

old = 'Freeze D038 and the focused WP4.2 PULSE test contract in the repository before creating PULSE production source. Keep the Phase 3 proof sine as full-app production audio while the isolated primitive is implemented and verified.'
new = 'Implement and verify the isolated D038 primitives `fmt_engine_pitch.*` and `fmt_osc_pulse.*` with focused deterministic host tests, static realtime audit and pinned PI32v2 target compilation. Keep the Phase 3 proof sine and full-app artifacts byte-identical; do not begin WP4.3 or full engine integration.'
if old in s:
    s = s.replace(old, new, 1)
else:
    assert new in s, 'PROJECT_STATE exact action is neither old nor final'

anchor = '- D037: Phase 4 engine ownership, eight-voice cap, Q7 pitch domain, event/effect adapter boundary, Q15/32-bit mix rules, realtime bounds and complete Phase 4 resource ceilings are frozen before oscillator source.\n'
d038 = '- D038: WP4.2 PULSE is a 4-byte phase-only hard-edge oscillator with exact existing Q7 pitch integers, pulse width 32..224, sample-before-advance ordering and zero full-app delta while isolated.\n'
assert anchor in s
if d038 not in s:
    s = s.replace(anchor, anchor + d038, 1)

checkpoint_anchor = '- WP4.1 closeout workflow: `01138c5f8a420efcc8e438ff3c2eb4090c086586`\n'
checkpoint = '- WP4.2 D038 contract: `247ff3c03cdf3cd8e0878fe37d79693b26afc398`\n- WP4.2 D038 decision/status freeze: `6bf5324ec09266064fbb75828bffb5e773d7ad14`\n'
assert checkpoint_anchor in s
if '- WP4.2 D038 contract:' not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)
p.write_text(s)

# The other durable status files were already updated by 6bf5324e; verify them
# here so this one-time pass cannot silently regress their handoff state.
assert 'D038 FROZEN; isolated PULSE implementation/tests next' in Path('docs/WORK_PACKAGES.md').read_text()
assert 'D038 FROZEN; isolated implementation/tests next' in Path('docs/ROADMAP.md').read_text()
assert '✅ `WP4_2_PULSE_CONTRACT.md` / `247ff3c0...`' in Path('docs/TEST_MATRIX.md').read_text()

print('WP4_2_D038_HANDOFF_FINAL_OK=1 next=isolated_pulse_implementation')
