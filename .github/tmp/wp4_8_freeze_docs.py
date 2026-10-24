from pathlib import Path


def replace_once(text, old, new, label):
    if new in text:
        return text
    assert text.count(old) == 1, f'{label} mismatch'
    return text.replace(old, new, 1)

# DECISIONS
p = Path('docs/DECISIONS.md')
s = p.read_text()
if '## D044 —' not in s:
    s += '''

## D044 — WP4.8 freezes six 8-BIT semantic IDs and one four-control page per engine

Date: 2026-10-24
Status: accepted — frozen before WP4.8 production parameter-page source

**Decision:** WP4.8 implements the isolated 8-BIT parameter/page resolver exactly as frozen in `docs/WP4_8_PARAMETER_PAGE_CONTRACT.md`. D044 assigns six stable semantic IDs in D027's reserved `0x20..0x5F` namespace: ATTACK `0x20`, DECAY `0x21`, SUSTAIN `0x22`, RELEASE `0x23`, PULSE WIDTH `0x24`, WAVETABLE `0x25`. IDs `0x26..0x5F` remain unassigned. D031 VOICE LEVEL/FINE/TRACK LEVEL and TRACK PAN keep their existing IDs and separate ownership.

The normalized semantic base set is exactly 6 B with defaults A24/D0/S255/R72/W128/T0. Each current engine exposes exactly four ordered page controls: PULSE = WIDTH/DECAY/SUSTAIN/RELEASE; NOISE = ATTACK/DECAY/SUSTAIN/RELEASE; TRIANGLE = ATTACK/DECAY/SUSTAIN/RELEASE; WAVETABLE-8 = TABLE/DECAY/SUSTAIN/RELEASE. PULSE and WAVETABLE-8 retain fixed D042 ATTACK 24 while using slot 0 for their defining oscillator control; NOISE/TRIANGLE expose full ADSR because D039/D040 freeze no extra mutable oscillator parameter.

D044 page values are normalized bytes. Envelope bytes map directly to D042. PULSE WIDTH maps by `32 + ((v * 193) >> 8)`, giving exact `0->32`, `128->128`, `255->224`. WAVETABLE maps by `v >> 5`, dividing all 256 values into eight exact 32-value D041 table bands. Default packed page words are PULSE `0x48FF0080`, NOISE `0x48FF0018`, TRIANGLE `0x48FF0018`, WAVETABLE-8 `0x48FF0000`.

Row precedence is base semantic value overridden by an applicable D028 p-lock. The pure resolver sees at most four `(FmtParamId,uint16_t)` pairs, performs at most four inspections and ignores common/non-page/other-engine IDs. Applicable values `>255` retain the base slot and report domain fallback; duplicate applicable IDs or count `>4` structurally fall back to the untouched base page; unsupported engines fail to no-page rather than aliasing another engine.

D037's publication shape is frozen as one naturally aligned 32-bit four-byte page word per `(track,row)`, maximum `8*64*4 = 2048 B` ordinary RAM when integrated. A complete 16-instrument semantic base bank is exactly `16*6 = 96 B`. WP4.8 defines these types/shapes but does not allocate them globally.

WP4.8 does **not** activate the six D044 IDs in the already-linked global `FMT_PARAM_REGISTRY`, does not change the current D029/D031 common four-knob step-hold UI and does not connect the engine to `dinghy.c`. Registry/UI/cache activation is deferred to WP4.9's explicit integration/migration checkpoint, where the numeric IDs/page meanings may not change. This preserves the required WP4.8 zero-linked-delta full-app identity.

**Reason:** the frozen oscillators expose only two oscillator-specific editable controls (PULSE width and WAVETABLE table), while D042 supplies common articulation. A single four-control page per engine matches the hardware/page budget without inventing fake NOISE/TRIANGLE DSP or a second page. Keeping D044 IDs normative but unlinked lets the complete page/p-lock translation be proven before deliberate production migration.

**Consequence:** focused tests must prove exact IDs/layouts/default words, all-256 WIDTH/TABLE mappings, engine-page applicability, p-lock precedence/fallback, 6 B base set, 2048 B cache type, bounded <=4-lock work, replay/guards/hash and real PI32v2 compile. Production delta from the D044 pre-source checkpoint must be only the isolated WP4.8 module, and the complete full app must remain byte-identical to WP4.7. Any ID renumbering, page-order change, WIDTH/TABLE mapping change or second-page addition requires a superseding decision.
'''
p.write_text(s)

# PROJECT_STATE
p = Path('docs/PROJECT_STATE.md')
s = p.read_text()
s = replace_once(
    s,
    'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8/parameter pages active; D044 contract freeze next - 2026-10-24**',
    'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8/parameter pages active; D044 FROZEN; isolated implementation/tests next - 2026-10-24**',
    'PROJECT_STATE top state')
s = replace_once(
    s,
    'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 active; D044 stable-ID/page/default/domain/publication contract required before parameter source**',
    'State: **IN PROGRESS - WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 active; D044 FROZEN; isolated parameter-page implementation/tests next**',
    'PROJECT_STATE milestone state')
s = replace_once(
    s,
    'WP4.8 must freeze D044 before production parameter-page source. D044 must define stable semantic IDs from the reserved Phase 4 range `0x20..0x5F`, exactly four exposed controls per 8-BIT engine, parameter order/names/defaults/normalized domains, exact normalized-byte mappings into D038-D042 controls, lockability and D027/D031 publication behavior, malformed/unsupported behavior, base-value versus row-local override ownership, bounded snapshot representation, deterministic vectors and zero-full-app-delta isolation boundary.',
    'D044 is frozen in `docs/WP4_8_PARAMETER_PAGE_CONTRACT.md`: six stable IDs `0x20..0x25`, a 6 B semantic base set, one exact four-control page per engine, direct D042 envelope mapping, bounded WIDTH/TABLE transforms, <=4-lock row resolution and a future 2048 B aligned row cache. Global registry/UI/cache activation remains deferred to WP4.9 so WP4.8 stays isolated and byte-identical at full-app level.',
    'PROJECT_STATE D044 paragraph')
s = replace_once(
    s,
    'Freeze D044 and the focused WP4.8 parameter-page/p-lock publication test contract in the repository before creating parameter-page production source. Keep D038-D043 primitives isolated and keep the Phase 3 proof sine/full-app artifacts byte-identical.',
    'Implement the isolated D044 parameter-page resolver and focused deterministic tests from the frozen contract. Do not activate the six IDs in the linked global registry, allocate the final cache, alter the current knob UI or connect D038-D043 to `dinghy.c`; keep the Phase 3 proof sine/full-app artifacts byte-identical.',
    'PROJECT_STATE next action')
if '- D044: WP4.8' not in s:
    anchor = '- D043: WP4.7 allocator is a 52-byte fixed eight-slot owner/recency map that reuses same-track voices, prefers IDLE, detaches OFF/CUT release tails and may steal only the oldest unowned RELEASE tail; active cross-track stealing is forbidden.\n'
    assert anchor in s, 'PROJECT_STATE D043 anchor missing'
    s = s.replace(anchor, anchor + '- D044: WP4.8 freezes six 8-BIT semantic IDs (`0x20..0x25`), one exact four-control page per engine, a 6 B base set, bounded normalized WIDTH/TABLE mappings and a future aligned 2048 B row cache; global registry/UI/cache activation is deferred to WP4.9.\n', 1)
p.write_text(s)

# WORK_PACKAGES
p = Path('docs/WORK_PACKAGES.md')
s = p.read_text()
s = replace_once(
    s,
    'Status: **IN PROGRESS — WP4.1 / #31 through WP4.7 / #37 COMPLETE; WP4.8 / #39 four-parameter pages + p-lock mapping active; D044 before source**',
    'Status: **IN PROGRESS — WP4.1 / #31 through WP4.7 / #37 COMPLETE; WP4.8 / #39 four-parameter pages + p-lock mapping active; D044 FROZEN; isolated implementation/tests next**',
    'WORK_PACKAGES status')
s = replace_once(
    s,
    '- **WP4.8 four-parameter pages + p-lock mapping** — IN PROGRESS — issue #39 — freeze D044 stable IDs/page/default/domain/publication contract before source',
    '- **WP4.8 four-parameter pages + p-lock mapping** — IN PROGRESS — issue #39 — D044 FROZEN; isolated six-ID/page/resolver implementation + deterministic/PI32v2 verification next',
    'WORK_PACKAGES WP4.8 row')
p.write_text(s)

# ROADMAP
p = Path('docs/ROADMAP.md')
s = p.read_text()
s = replace_once(s, 'with D044 required before source.', 'with D044 now frozen and isolated implementation/testing next.', 'ROADMAP Phase4 lead')
s = replace_once(
    s,
    'Status: **IN PROGRESS — WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 parameter pages + p-lock mapping active; D044 before source**',
    'Status: **IN PROGRESS — WP4.1/D037 through WP4.7/D043 COMPLETE; WP4.8 / #39 parameter pages + p-lock mapping active; D044 FROZEN; isolated implementation/tests next**',
    'ROADMAP status')
s = replace_once(
    s,
    'Active package: WP4.8 / #39 — freeze D044 stable Phase 4 engine parameter IDs, exactly four controls per 8-BIT engine, defaults/domains/value mappings and bounded D027/D031 p-lock/publication semantics before production source. WP4.1 / #31 through WP4.7 / #37 are complete; D038-D043 primitives remain isolated. Branch: `phase4/8bit-engine`.',
    'Active package: WP4.8 / #39 — D044 is frozen in `WP4_8_PARAMETER_PAGE_CONTRACT.md`; implement/test the isolated six-ID, four-control-page and bounded p-lock resolver primitive while keeping D038-D043 and the running full app unchanged. WP4.1 / #31 through WP4.7 / #37 are complete. Branch: `phase4/8bit-engine`.',
    'ROADMAP active package')
p.write_text(s)

# TEST_MATRIX
p = Path('docs/TEST_MATRIX.md')
s = p.read_text()
s = replace_once(
    s,
    '| D044 engine parameter-page/p-lock publication contract frozen before source | required | compile | n/a | ⬜ WP4.8 active |',
    '| D044 engine parameter-page/p-lock publication contract frozen before source | required | compile | n/a | ✅ `WP4_8_PARAMETER_PAGE_CONTRACT.md` / D044 |\n| D044 six IDs + four-engine pages + all-256 WIDTH/TABLE mappings + bounded p-lock fallback | required | required | optional later | ⬜ WP4.8 next |\n| WP4.8 full app remains exact WP4.7 baseline while parameter layer is isolated | required | required | n/a | ⬜ WP4.8 next |',
    'TEST_MATRIX D044 row')
s = replace_once(
    s,
    'WP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.',
    'WP4.7 detailed evidence: `WP4_7_ALLOCATOR_CONTRACT.md`, `WP4_7_VERIFICATION.md`.\nWP4.8 normative contract: `WP4_8_PARAMETER_PAGE_CONTRACT.md`.',
    'TEST_MATRIX evidence')
p.write_text(s)

print('WP4_8_D044_DOC_FREEZE_OK=1 ids=6 base_bytes=6 page_slots=4 cache_bytes=2048 registry_activation=wp4_9')
