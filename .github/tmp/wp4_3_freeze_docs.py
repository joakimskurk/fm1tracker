from pathlib import Path

# Append D039 without rewriting prior decision history.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D039 —' not in s, 'D039 already present'
entry = r'''

## D039 — WP4.3 NOISE is an 8-byte clocked 15-bit LFSR with a saturated x16 pitch clock

Date: 2026-10-24
Status: accepted — frozen before NOISE production source

**Decision:** WP4.3 implements one isolated deterministic NOISE primitive exactly as frozen in `docs/WP4_3_NOISE_CONTRACT.md`. It remains disconnected from `dinghy.c` and the running full app.

`FmtNoiseOsc` is exactly 8 bytes: one 32-bit wrapping clock phase plus one 32-bit field whose low 15 bits hold the LFSR. Reset and D033 RETRIG both restore phase zero and seed `0x5A3D`. The LFSR recurrence is fixed to a right shift with feedback `bit0 XOR bit1` inserted at bit14: `(state >> 1) | (((state ^ (state >> 1)) & 1) << 14)`. Every valid nonzero state lies on the same maximal 32,767-step cycle; WP4.3 defines no alternate short/metallic tap mode.

NOISE samples current LFSR bit0 before any state advance and maps it exactly to `+32767` for one and `-32767` for zero. It then adds one unsigned 32-bit noise-clock increment to phase. Only an unsigned phase wrap may advance the LFSR, and one render can therefore perform zero or one LFSR step — never a catch-up loop.

D039 reuses D038 pitch rather than inventing another tracker pitch domain. The NOISE clock is `fmt_engine_pitch_inc_q7(pitch_q7) * 16`, implemented as a saturating left shift: inputs above `UINT32_MAX >> 4` return `UINT32_MAX` instead of wrapping. This yields approximately 4.186 kHz LFSR clock at C4 and 7.040 kHz at A4, while low notes remain deliberately coarse and high notes saturate toward one state update per audio frame.

The primitive is intentionally clocked/periodic chip noise, not mathematically white full-band noise. There is no interpolation, whitening, BLEP, oversampling, filter or hidden random source. The 32,767-step period, sample-and-hold behavior and clock/sample-rate aliasing are accepted parts of the Phase 4 8-BIT identity.

Frozen deterministic evidence includes the reset seed, first LFSR states, the `0x80000000` sixteen-sample sign vector `++--++++++++----`, exact x16/saturation clock vectors, full-cycle period proof and a 65,536-sample A4 signature: final phase `0xFB900000`, final LFSR `0x5DC1`, FNV-1a32 `0xF50385B6`.

WP4.3 remains isolated, so full-app image/RAM/pool/WAV/BIN/FWSC delta must remain exactly zero versus WP4.2. The isolated mutable state cost is 8 B per NOISE oscillator, 64 B for eight future instances, with no static mutable or pool storage.

**Reason:** D037 requires fixed-size deterministic engine state and bounded per-sample work, while NOISE must still share the tracker Q7 pitch domain. A 15-bit maximal LFSR is recognizably chip-like and cheap; the saturated x16 clock makes ordinary musical pitches useful as noise rates without overflow wrap; and the one-wrap/one-step rule makes worst-case audio work independent of pitch.

**Consequence:** Focused tests must prove the exact recurrence, 32,767-step period, reset/retrigger, clock vectors/saturation, zero/one-step render bound, short vectors, repeatability and long-run signature. `fmt_osc_noise.c` must compile with the real pinned PI32v2 compiler and pass the same no-float/no-int64/no-allocation/no-unbounded-loop audit. TRIANGLE, WAVETABLE-8, envelope, allocator, parameter pages and proof-sine migration remain later packages. Any alternate tap mode, whitening, seed, clock multiplier or multi-step catch-up rule requires a superseding decision and new deterministic evidence.
'''
p.write_text(s.rstrip() + entry.rstrip() + '\n')

# PROJECT_STATE: D039 is now frozen; implementation/testing is next.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/NOISE active; D039 contract freeze next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3/NOISE active; D039 FROZEN; isolated implementation/tests next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 active; D039 contract/test plan required before NOISE source**'
new = 'State: **IN PROGRESS - WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 active; D039 FROZEN; isolated NOISE implementation/tests next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.3 must freeze D039 before production NOISE source. D039 must define exact deterministic state/seed, PRNG/LFSR algorithm and tap direction, output mapping, Q7 pitch-to-noise clock relationship, maximum state advances per rendered frame, reset/retrigger/wrap semantics, spectral/aliasing intent, deterministic vectors/hash, target-compile obligations and the zero-full-app-delta isolation boundary.'
new = 'D039 is frozen in `docs/WP4_3_NOISE_CONTRACT.md`: 8-byte phase+15-bit-LFSR state, seed `0x5A3D`, bit0-XOR-bit1 maximal recurrence, sample-before-advance output, saturated D038 pitch-clock x16, at most one LFSR step per audio frame, fixed deterministic vectors/hash and zero full-app delta while isolated.'
assert s.count(old) == 1, 'PROJECT_STATE D039 paragraph mismatch'
s = s.replace(old, new, 1)
anchor = '- D038: WP4.2 PULSE is a 4-byte phase-only hard-edge oscillator with exact existing Q7 pitch integers, pulse width 32..224, sample-before-advance ordering and zero full-app delta while isolated.\n'
d039_bullet = '- D039: WP4.3 NOISE is an 8-byte deterministic clocked 15-bit LFSR seeded `0x5A3D`, with saturated D038 pitch clock x16 and a hard maximum of one LFSR step per audio frame while isolated.\n'
assert anchor in s, 'PROJECT_STATE D038 decision anchor missing'
if d039_bullet not in s:
    s = s.replace(anchor, anchor + d039_bullet, 1)
old = 'Implement and verify the isolated D038 primitives `fmt_engine_pitch.*` and `fmt_osc_pulse.*` with focused deterministic host tests, static realtime audit and pinned PI32v2 target compilation. Keep the Phase 3 proof sine and full-app artifacts byte-identical; do not begin WP4.3 or full engine integration.'
new = 'Implement and verify isolated `fmt_osc_noise.*` exactly under D039 with focused deterministic host tests, full 32,767-step LFSR-period proof, static realtime audit and pinned PI32v2 target compilation. Keep D038 PULSE isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical; do not begin WP4.4 or full engine integration.'
assert s.count(old) == 1, 'PROJECT_STATE exact action mismatch'
s = s.replace(old, new, 1)
checkpoint_anchor = '- WP4.2 D038 verification/closeout: `776cbd47e55fc5da6f57e894325ba67d67a61f29` — run `40089122159`, job `120857992257`\n'
checkpoint = '- WP4.3 D039 contract: `9cf3fd7d0a227c9555c34c98e37154ea038ff4cb`\n'
assert checkpoint_anchor in s, 'PROJECT_STATE checkpoint anchor missing'
if checkpoint not in s:
    s = s.replace(checkpoint_anchor, checkpoint_anchor + checkpoint, 1)
last_marker = '## Last updated\n\n'
idx = s.index(last_marker)
s = s[:idx] + last_marker + '2026-10-24 - WP4.3 / #33 active. D039 is frozen in `WP4_3_NOISE_CONTRACT.md` before NOISE source. Exact next action: implement/test isolated `fmt_osc_noise.*` with deterministic period/vector/hash proof and real PI32v2 compile while the full app remains byte-identical.\n'
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 NOISE active; D039 before source**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 COMPLETE; WP4.2 / #32 COMPLETE; WP4.3 / #33 NOISE active; D039 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.3 NOISE** — IN PROGRESS — issue #33 — freeze D039 deterministic noise/clock contract before source'
new = '- **WP4.3 NOISE** — IN PROGRESS — issue #33 — D039 FROZEN; isolated NOISE implementation + deterministic period/vector/PI32v2 verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package mismatch'
p.write_text(s.replace(old, new, 1))

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
old = 'Phase 4 is active. **WP4.1 / #31 is COMPLETE** with D037 frozen before source; **WP4.2 / #32 PULSE is COMPLETE** with D038 deterministic host/PI32v2 proof and zero linked full-app delta; **WP4.3 / #33 NOISE is active**, with D039 required before source.'
new = 'Phase 4 is active. **WP4.1 / #31 and WP4.2 / #32 are COMPLETE**; **WP4.3 / #33 NOISE is active and D039 is FROZEN** before source, with isolated deterministic implementation/testing next.'
assert s.count(old) == 1, 'ROADMAP lead mismatch'
s = s.replace(old, new, 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 NOISE active; D039 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 COMPLETE; WP4.2/D038 COMPLETE; WP4.3 / #33 NOISE active; D039 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.3 / #33 — freeze D039 NOISE state/seed/clock/output/test contract before production source. WP4.1 / #31 and WP4.2 / #32 are complete; PULSE remains isolated from the full app. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.3 / #33 — D039 is frozen in `WP4_3_NOISE_CONTRACT.md`; implement/test isolated clocked-LFSR NOISE without full-app integration. WP4.1 / #31 and WP4.2 / #32 are complete; PULSE remains isolated. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: mark only the D039 contract freeze complete.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D039 NOISE primitive/test contract frozen before source | required | compile | n/a | ⬜ WP4.3 active |'
new = '| D039 NOISE primitive/test contract frozen before source | required | compile | n/a | ✅ `WP4_3_NOISE_CONTRACT.md` / `9cf3fd7d...` |'
assert s.count(old) == 1, 'TEST_MATRIX D039 row mismatch'
p.write_text(s.replace(old, new, 1))

print('WP4_3_D039_DOC_FREEZE_OK=1 state_bytes=8 seed=0x5A3D period=32767 clock=x16_saturating max_steps_per_frame=1')
