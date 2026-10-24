from pathlib import Path

# Append D038 without rewriting prior decision history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D038 —' not in s
entry = r'''

## D038 — WP4.2 PULSE is a 4-byte hard-edge oscillator using the existing Q7 pitch math

Date: 2026-10-24
Status: accepted — frozen before PULSE production source

**Decision:** WP4.2 implements one isolated hard-edge PULSE primitive plus a canonical shared Q7-to-phase-increment helper, exactly as frozen in `docs/WP4_2_PULSE_CONTRACT.md`. It is not connected to the full-app proof lane in WP4.2.

`FmtPulseOsc` contains exactly one `uint32_t phase` and is therefore exactly 4 B. Reset and retrigger set phase to zero. Rendering samples the current phase before advancing it, returns exactly `+32767` or `-32767`, then advances phase by unsigned 32-bit addition/wrap. A zero increment holds phase and output. Null render fails to silence with return value zero.

Pulse width is one unsigned byte with a legal oscillator-domain range `32..224`, default `128`, interpreted as `width / 256` of one cycle high. The render threshold is `width << 24`; values below/above the legal range clamp to 32/224. This yields a continuous 12.5–87.5% range containing the classic 12.5/25/50/75% chip widths without permitting a deliberate all-high/all-low DC mode. WP4.8, not WP4.2, owns the eventual stable parameter ID/page mapping.

D038 explicitly keeps PULSE band-unlimited/hard-edge. WP4.2 adds no BLEP/polyBLEP/minBLEP, oversampling or hidden smoothing/filter. Upper-register aliasing is accepted as part of the Phase 4 chip-style identity. Any later anti-aliasing strategy must supersede D038 because it changes state, CPU cost and deterministic sample vectors.

The common pitch helper reuses the exact existing Phase 3 proof-lane behavior: unsigned Q7 semitone pitch `0..16383`, the existing 12-entry C9 phase-increment table, octave shifts and `fmt_playback_interp_inc()` rounding. Frozen compatibility vectors include C4 `25480118`, A4 `42852281`, A4+0.5 semitone `44126346`, MIDI127 `1221665363` and maximum Q7 `1293741834`. The helper runs only when pitch changes, not per rendered sample. No float or runtime 64-bit division is permitted.

WP4.2 remains isolated: `fmt_engine_pitch.*` and `fmt_osc_pulse.*` must compile independently with the pinned JieLi compiler but are not included by `dinghy.c`. Therefore the required full-app image/RAM/pool delta, default WAV delta and BIN/FWSC delta for WP4.2 are all exactly zero. The isolated state cost is 4 B per PULSE oscillator (32 B for eight future instances), the immutable pitch table is 48 B, and there is no static mutable or pool storage.

**Reason:** D037 requires one common pitch domain, fixed state and bounded per-sample work before engine integration. Reusing the already-proven pitch integers avoids a second tuning model; phase-only state makes PULSE trivial to bound; a 12.5–87.5% continuous duty range preserves classic chip voices without pathological DC endpoints; and deliberately postponing full-app integration keeps the Phase 3/WP4.1 audio/device baseline intact while the first oscillator primitive is tested in isolation.

**Consequence:** Focused tests must freeze exact pitch vectors, 25/50/75% sample vectors, clamp behavior, phase wrap, zero increment, deterministic replay and a long-render hash before WP4.2 closes. Both new translation units require real PI32v2 target compilation and static realtime audit. No NOISE/TRIANGLE/WAVETABLE-8, envelope, allocator, engine parameter page/cache or proof-sine migration may be pulled into WP4.2. Any future anti-aliasing or change to pitch integers/pulse edge ordering requires a superseding decision and new deterministic evidence.
'''
p.write_text(s.rstrip() + entry.rstrip() + '\n')

# PROJECT_STATE: D038 is now frozen; focused implementation/testing is next.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; D038 PULSE contract/test plan next; no PULSE production source begun**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; D038 FROZEN; focused PULSE implementation/tests next; no full-app engine integration**'
assert s.count(old) == 1, 'PROJECT_STATE state mismatch'
s = s.replace(old, new)
old = 'D038 must freeze the isolated PULSE primitive before source: exact state/reset, shared Q7 pitch-to-phase-increment boundary, phase/sample ordering, pulse-width representation/clamp, nominal-Q15 output, wrap/extreme behavior, deterministic vectors, target-compile/resource obligations and the explicit proof-sine migration boundary.'
new = 'D038 is frozen in `docs/WP4_2_PULSE_CONTRACT.md`: 4-byte phase-only state, exact existing Q7 pitch integers, sample-before-advance ordering, pulse width 32..224, hard-edge/band-unlimited output, focused deterministic vectors and zero full-app delta while isolated.'
assert s.count(old) == 1, 'PROJECT_STATE D038 paragraph mismatch'
s = s.replace(old, new)
old = 'Freeze D038 and the focused WP4.2 PULSE test contract in the repository before creating PULSE production source. Keep the Phase 3 proof sine as full-app production audio while the isolated primitive is implemented and verified.'
new = 'Implement and verify the isolated D038 primitives `fmt_engine_pitch.*` and `fmt_osc_pulse.*` with focused deterministic host tests, static realtime audit and pinned PI32v2 target compilation. Keep the Phase 3 proof sine and full-app artifacts byte-identical; do not begin WP4.3 or full engine integration.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
old_last = '2026-10-24 - WP4.1 / #31 COMPLETE. D037 frozen before source; closeout run `40087703654` SUCCESS with zero production delta and inherited Phase 3 device-build gate green. WP4.2 / #32 is now IN PROGRESS; exact next action is D038 PULSE contract/test-plan freeze before PULSE source.'
new_last = '2026-10-24 - WP4.2 / #32 active. D038 is now frozen in `WP4_2_PULSE_CONTRACT.md` before PULSE source. Exact next action is isolated PULSE/pitch implementation plus deterministic host/PI32v2 verification with the full-app proof sine unchanged.'
assert s.count(old_last) == 1, 'PROJECT_STATE last updated mismatch'
p.write_text(s.replace(old_last, new_last))

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 active; D038 required before PULSE source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 active; D038 FROZEN; isolated PULSE implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new)
old = '- **WP4.2 PULSE** — IN PROGRESS — issue #32 — freeze D038/test contract before source'
new = '- **WP4.2 PULSE** — IN PROGRESS — issue #32 — D038 FROZEN; isolated pitch/PULSE implementation + deterministic verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source and zero production delta; **WP4.2 / #32 PULSE is now active**, with D038 required before oscillator source.'
new = 'Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source and zero production delta; **WP4.2 / #32 PULSE is active and D038 is FROZEN** before oscillator source.'
assert s.count(old) == 1, 'ROADMAP Phase3 handoff mismatch'
s = s.replace(old, new)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2 / #32 PULSE active; D038 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2 / #32 PULSE active; D038 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new)
old = 'Active package: WP4.2 / #32 — freeze D038 PULSE primitive/test contract before production oscillator source. WP4.1 / #31 is complete with evidence in `WP4_1_VERIFICATION.md`. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.2 / #32 — D038 is frozen in `WP4_2_PULSE_CONTRACT.md`; implement/test the isolated shared pitch helper and PULSE primitive without full-app integration. WP4.1 / #31 is complete. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new))

# TEST_MATRIX: mark only the contract freeze complete; implementation rows stay open.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D038 PULSE primitive/test contract frozen | required | compile | n/a | ⬜ WP4.2 active |'
new = '| D038 PULSE primitive/test contract frozen | required | compile | n/a | ✅ `WP4_2_PULSE_CONTRACT.md` / `247ff3c0...` |'
assert s.count(old) == 1, 'TEST_MATRIX D038 row mismatch'
p.write_text(s.replace(old, new))

print('WP4_2_D038_DOC_FREEZE_OK=1 state_bytes=4 width=32..224 full_app_delta=0')

# Deliberate trigger: workflow existed before this push.
