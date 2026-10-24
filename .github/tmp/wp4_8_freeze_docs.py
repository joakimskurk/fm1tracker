from pathlib import Path

# DECISIONS: append the normative D044 record once.
p = Path('docs/DECISIONS.md')
s = p.read_text()
assert '## D044 —' not in s, 'D044 already present'
s += '''

## D044 — WP4.8 freezes six 8-BIT semantic IDs and one four-control page per engine

Date: 2026-10-24
Status: accepted — frozen before WP4.8 production parameter-page source

**Decision:** WP4.8 implements the isolated 8-BIT parameter/page resolver exactly as frozen in `docs/WP4_8_PARAMETER_PAGE_CONTRACT.md`. D044 assigns six stable semantic IDs in D027's reserved `0x20..0x5F` namespace: ATTACK `0x20`, DECAY `0x21`, SUSTAIN `0x22`, RELEASE `0x23`, PULSE WIDTH `0x24`, WAVETABLE `0x25`. IDs `0x26..0x5F` remain unassigned. D031 VOICE LEVEL/FINE/TRACK LEVEL and TRACK PAN keep their existing IDs and separate ownership.

The normalized semantic base set is exactly 6 B with defaults A24/D0/S255/R72/W128/T0. Each current engine exposes exactly four ordered page controls: PULSE = WIDTH/DECAY/SUSTAIN/RELEASE; NOISE = ATTACK/DECAY/SUSTAIN/RELEASE; TRIANGLE = ATTACK/DECAY/SUSTAIN/RELEASE; WAVETABLE-8 = TABLE/DECAY/SUSTAIN/RELEASE. PULSE and WAVETABLE-8 therefore retain fixed D042 ATTACK 24 while using slot 0 for their defining oscillator control; NOISE/TRIANGLE expose full ADSR because D039/D040 freeze no extra mutable oscillator parameter.

D044 page values are stored/published as normalized bytes. Envelope bytes map directly to D042. PULSE WIDTH maps by `32 + ((v * 193) >> 8)`, giving exact `0->32`, `128->128`, `255->224`. WAVETABLE maps by `v >> 5`, dividing all 256 values into eight exact 32-value D041 table bands. Default packed page words are PULSE `0x48FF0080`, NOISE `0x48FF0018`, TRIANGLE `0x48FF0018`, WAVETABLE-8 `0x48FF0000`.

Row precedence is base semantic value overridden by an applicable D028 p-lock. The pure resolver sees at most four `(FmtParamId,uint16_t)` pairs, performs at most four inspections and ignores common/non-page/other-engine IDs. Applicable values `>255` retain the base slot and report domain fallback; duplicate applicable IDs or count `>4` structurally fall back to the untouched base page; unsupported engines fail to no-page rather than aliasing another engine.

D037's publication shape is frozen as one naturally aligned 32-bit four-byte page word per `(track,row)`, maximum `8*64*4 = 2048 B` ordinary RAM when integrated. A complete 16-instrument semantic base bank would be exactly `16*6 = 96 B`. WP4.8 defines these types/shapes but does not allocate them globally.

Crucially, WP4.8 does **not** activate the six D044 IDs in the already-linked global `FMT_PARAM_REGISTRY`, does not change the current D029/D031 common four-knob step-hold UI and does not connect the engine to `dinghy.c`. Registry/UI/cache activation is deferred to WP4.9's explicit integration/migration checkpoint, where the numeric IDs/page meanings may not change. This preserves the required WP4.8 zero-linked-delta full-app identity.

**Reason:** the four frozen oscillator primitives expose only two oscillator-specific editable controls (PULSE width and WAVETABLE table), while D042 provides the common articulation layer. A single four-control page per engine matches the hardware/page budget without inventing fake NOISE/TRIANGLE DSP or a second page. Keeping D044 IDs normative but unlinked allows the complete page/p-lock translation to be proven deterministically before the deliberate proof-sine migration changes production artifacts.

**Consequence:** focused tests must prove exact IDs/layouts/default words, all-256 WIDTH/TABLE mappings, engine page applicability, p-lock precedence/fallback, 6 B base set, 2048 B cache type, bounded <=4-lock work, replay/guards/hash and real PI32v2 compile. Production delta from the D044 pre-source checkpoint must be only the isolated WP4.8 module, and the complete full app must remain byte-identical to WP4.7. Any ID renumbering, page-order change, WIDTH/TABLE mapping change or second-page addition requires a superseding decision.
'''
p.write_text(s)

# PROJECT_STATE: fix the stale top line and move exact next action past the freeze.
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.6/D042 COMPLETE; WP4.7/allocator active; D043 FROZEN; isolated implementation/tests next - 2026-10-24**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8/parameter pages active; D044 FROZEN; isolated implementation/tests next - 2026-10-24**'
assert s.count(old) == 1, 'PROJECT_STATE top state mismatch'
s = s.replace(old, new, 1)
old = 'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 active; D044 stable-ID/page/default/domain/publication contract required before parameter source**'
new = 'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 active; D044 FROZEN; isolated parameter-page implementation/tests next**'
assert s.count(old) == 1, 'PROJECT_STATE milestone state mismatch'
s = s.replace(old, new, 1)
old = 'WP4.8 must freeze D044 before production parameter-page source. D044 must define stable semantic IDs from the reserved Phase 4 range `0x20..0x5F`, exactly four exposed controls per 8-BIT engine, parameter order/names/defaults/normalized domains, exact normalized-byte mappings into D038-D042 controls, lockability and D027/D031 publication behavior, malformed/unsupported behavior, base-value versus row-local override ownership, bounded snapshot representation, deterministic vectors and zero-full-app-delta isolation boundary.'
new = 'D044 is frozen in `docs/WP4_8_PARAMETER_PAGE_CONTRACT.md`: six stable IDs `0x20..0x25`, 6 B semantic base set, one exact four-control page per engine, direct D042 envelope mapping, bounded WIDTH/TABLE transforms, <=4-lock row resolution and a future 2048 B aligned row cache. Global registry/UI/cache activation remains deferred to WP4.9 so WP4.8 stays isolated and byte-identical at full-app level.'
assert s.count(old) == 1, 'PROJECT_STATE D044 paragraph mismatch'
s = s.replace(old, new, 1)
old = 'Freeze D044 and the focused WP4.8 parameter-page/p-lock publication test contract in the repository before creating parameter-page production source. Keep D038-D043 primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.'
new = 'Implement the isolated D044 parameter-page resolver and focused deterministic tests from the frozen contract. Do not activate the six IDs in the linked global registry, allocate the final cache, alter the current knob UI or connect D038-D043 to `dinghy.c`; keep the Phase 3 proof sine/full-app artifacts byte-identical.'
assert s.count(old) == 1, 'PROJECT_STATE exact next action mismatch'
s = s.replace(old, new, 1)
if '- D044: WP4.8' not in s:
    anchor = '- D043: WP4.7 allocator is a 52-byte fixed eight-slot owner/recency map that reuses same-track voices, prefers IDLE, detaches OFF/CUT release tails and may steal only the oldest unowned RELEASE tail; active cross-track stealing is forbidden.\n'
    insert = anchor + '- D044: WP4.8 freezes six 8-BIT semantic IDs (`0x20..0x25`), one exact four-control page per engine, a 6 B base set, bounded normalized WIDTH/TABLE mappings and a future aligned 2048 B row cache; global registry/UI/cache activation is deferred to WP4.9.\n'
    assert anchor in s, 'PROJECT_STATE D043 anchor missing'
    s = s.replace(anchor, insert, 1)
p.write_text(s)

# WORK_PACKAGES.
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
old = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.6 / #36 COMPLETE; WP4.7 / #37 deterministic allocator active; D043 FROZEN; isolated implementation/tests next**'
new = 'Status: **IN PROGRESS — WP4.1 / #31 through WP4.7 / #37 COMPLETE; WP4.8 / #39 parameter pages + p-lock mapping active; D044 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'WORK_PACKAGES status mismatch'
s = s.replace(old, new, 1)
old = '- **WP4.7 deterministic voice allocation/stealing** — IN PROGRESS — issue #37 — D043 FROZEN; isolated allocator implementation + ownership/release-tail/saturation/PI32v2 verification next\n- WP4.8 four-parameter pages + p-lock mapping'
new = '- **WP4.7 deterministic voice allocation/stealing** — COMPLETE — issue #37 — D043 + `WP4_7_VERIFICATION.md`; release-tail-only deterministic allocation and zero linked full-app delta\n- **WP4.8 four-parameter pages + p-lock mapping** — IN PROGRESS — issue #39 — D044 FROZEN; isolated six-ID/page/resolver implementation + deterministic/PI32v2 verification next'
assert s.count(old) == 1, 'WORK_PACKAGES package rows mismatch'
s = s.replace(old, new, 1)
p.write_text(s)

# ROADMAP.
p = Path('docs/ROADMAP.md')
s = p.read_text()
s = s.replace('with D044 required before source.', 'with D044 now frozen and isolated implementation/testing next.', 1)
old = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 parameter pages + p-lock mapping active; D044 before source**'
new = 'Status: **IN PROGRESS — WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 parameter pages + p-lock mapping active; D044 FROZEN; isolated implementation/tests next**'
assert s.count(old) == 1, 'ROADMAP status mismatch'
s = s.replace(old, new, 1)
old = 'Active package: WP4.8 / #39 — freeze D044 stable Phase 4 engine parameter IDs, exactly four controls per 8-BIT engine, defaults/domains/value mappings and bounded D027/D031 p-lock/publication semantics before production source. WP4.1 / #31 through WP4.7 / #37 are complete; D038-D043 primitives remain isolated. Branch: `phase4/8bit-engine`.'
new = 'Active package: WP4.8 / #39 — D044 is frozen in `WP4_8_PARAMETER_PAGE_CONTRACT.md`; implement/test the isolated six-ID, four-control-page and bounded p-lock resolver primitive while keeping D038-D043 and the running full app unchanged. WP4.1 / #31 through WP4.7 / #37 are complete. Branch: `phase4/8bit-engine`.'
assert s.count(old) == 1, 'ROADMAP active package mismatch'
s = s.replace(old, new, 1)
p.write_text(s)

# TEST_MATRIX.
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
old = '| D044 engine parameter-page/p-lock publication contract frozen before source | required | compile | n/a | ⬜ WP4.8 active |'
new = '| D044 engine parameter-page/p-lock publication contract frozen before source | required | compile | n/a | ✅ `WP4_8_PARAMETER_PAGE_CONTRACT.md` / D044 |\n| D044 six IDs + four-engine pages + all-256 WIDTH/TABLE mappings + bounded p-lock fallback | required | required | optional later | ⬜ WP4.8 next |\n| WP4.8 full app remains exact WP4.7 baseline while parameter layer is isolated | required | required | n/a | ⬜ WP4.8 next |'
assert s.count(old) == 1, 'TEST_MATRIX D044 row mismatch'
s = s.replace(old, new, 1)
old = 'WP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.'
new = 'WP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.\nWP4.8 normative contract: `WP4_8_PARAMETER_PAGE_CONTRACT.md`.'
assert s.count(old) == 1, 'TEST_MATRIX evidence anchor mismatch'
s = s.replace(old, new, 1)
p.write_text(s)

print('WP4_8_D044_DOC_FREEZE_OK=1 ids=6 base_bytes=6 page_slots=4 cache_bytes=2048 registry_activation=wp4_9')
