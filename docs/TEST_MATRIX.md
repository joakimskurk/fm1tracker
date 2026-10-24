# FM1Tracker Test Matrix

This file is the current regression index. Detailed package evidence and exact historical vectors/hashes live in the corresponding `WP*_VERIFICATION.md` and contract documents.

Legend:

- ✅ verified
- 🟨 verified only to the stated host/device-build boundary
- ⬜ not yet verified
- ⛔ blocked

## Platform / baseline

| Check | Host | Firmware/device build | Physical FM-1 | Status |
|---|---:|---:|---:|---|
| Pinned Dinghy/Felucca/SLOOP provenance | n/a | n/a | n/a | ✅ Phase 1 |
| Package identity fixed to `FM-1_987` | n/a | required | n/a | ✅ |
| Pinned JieLi toolchain + AC79 SDK identity | n/a | required | n/a | ✅ SDK `d179b4484759423312073f5fbb232501aa491047` |
| HAL-only register access / build safety | inspect | required | n/a | ✅ |
| No float in firmware DSP | inspect/test | required | n/a | ✅ through D046 |
| No runtime 64-bit division in firmware DSP | inspect/test | required | n/a | ✅ through D046 |
| USB rescue/update source path preserved | inspect | inspect/build | required later | 🟨 physical path pending |
| Actual AC791N callback execution margin | n/a | n/a | required | ⬜ no physical FM-1 |
| Install/boot/audio/panel behavior | n/a | n/a | required | ⬜ no physical FM-1 |

Phase 1 evidence: `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md`.

## Phase 2 - tracker core - COMPLETE

| Package / invariant | Host | Device build | Hardware | Status |
|---|---:|---:|---:|---|
| WP2.1 data model: 8 tracks x 64 steps x 16 patterns | required | required | n/a | ✅ |
| `FmtStep` exactly 8 B and deterministic sentinels/bounds | required | required | n/a | ✅ |
| WP2.2 one 65,896 B pool-backed `FmtSong` | required | required | n/a | ✅ |
| WP2.3 exact integer transport / no cumulative drift | required | required | later | ✅ |
| 120 BPM row timing / row 63 -> 0 wrap exact | required | required | later | ✅ |
| WP2.4 audible playback / next-frame row-boundary ordering | required | required | later | ✅ |
| WP2.5 16 white keys cover one 16-step page exactly | required | compile | required later | ✅ provisional hardware mapping |
| Four pages cover all 64 rows exactly | required | compile | required later | ✅ |
| WP2.6 eight black-key track selectors | required | compile | required later | ✅ provisional hardware mapping |
| Track/page selection cannot mutate song | required | compile | n/a | ✅ |
| WP2.7 stopped STEP REC note entry | required | required | required later | ✅ |
| Real transport-stop acknowledgement before editor writes | required | compile | required later | ✅ |
| One note gesture advances exactly once, including 63 -> 0 | required | required | required later | ✅ |
| WP2.8 REST/TIE/OFF semantics | required | required | required later | ✅ |
| Explicit NONE/NOTE/TIE/OFF playback events | required | compile | required later | ✅ |
| Sentinels never become raw pitch | required | compile | n/a | ✅ |
| WP2.9 bounded tracker screen / main-loop-only drawing | required | required | required later | ✅ |
| Ten dirty regions / max two redraws per main-loop poll | required | compile | required later | ✅ |
| HOME cleans tracker pixels | required | compile | required later | ✅ |
| WP2.10 complete 64-row/eight-track integration | required | required | required later | ✅ run `40015242947` |
| Phase 2 production/resource closeout | required | required | n/a | ✅ |

Detailed evidence: `WP2_1_VERIFICATION.md` through `WP2_9_VERIFICATION.md`, `WP2_10_TEST_PLAN.md`, `PHASE2_VERIFICATION.md`.

## Phase 3 - p-locks / effects / realtime ingress - COMPLETE

| Package / invariant | Host | Device build | Hardware | Status |
|---|---:|---:|---:|---|
| WP3.1 / D027 one-byte semantic parameter registry | required | required | n/a | ✅ |
| Parameter IDs and tracker effect command IDs remain separate | inspect/test | compile | n/a | ✅ |
| WP3.2 / D028 global 2,048-record sparse lock pool | required | required | n/a | ✅ |
| Exactly max four locks per step / 512 saturated owners | required | compile | n/a | ✅ |
| Corruption-safe owner/list validation | required | compile | n/a | ✅ |
| WP3.3 / D029 held-step p-lock editing | required | required | required later | ✅ |
| Held target cannot silently retarget via track/page input | required | required | required later | ✅ |
| WP3.4 / D030 browse + deliberate single-lock delete | required | required | required later | ✅ |
| Delete arm/cancel behavior bounded and non-destructive | required | required | required later | ✅ |
| WP3.5 / D031 VOICE LEVEL/FINE/TRACK LEVEL snapshots | required | required | later | ✅ |
| TRACK PAN remains explicit storage-only parameter | required | compile | later | ✅ |
| WP3.6 / D032 exact six-tick tracker effect clock | required | required | later | ✅ |
| ARP/SLIDE_UP/SLIDE_DOWN/PORTA/VIBRATO deterministic | required | required | later | ✅ |
| D031 FINE precedence after D032 pitch | required | required | later | ✅ |
| WP3.7 / D033 RETRIG/NOTE DELAY/NOTE CUT | required | required | later | ✅ |
| Tick values 0..5 only; values >5 schedule no action | required | compile | n/a | ✅ |
| WP3.8 / D034 exact 32-entry SPSC main->audio queue | required | required | later | ✅ |
| Reject-newest overflow + audio fail-to-silence/O(1) discard | required | required | later | ✅ |
| WP3.9 / D035 bounded p-lock/effect visual feedback | required | required | required later | ✅ |
| `FmtTrackerScreen` 62 B / ten regions / max two redraws | required | compile | required later | ✅ |
| WP3.10 / D036 full 2,048-lock saturation stress | required | required | n/a | ✅ |
| D036 2,048 queue cycles / 6,144 rejects + recovery | required | required | later | ✅ |
| D036 4,096-row / 64-wrap D032/D033 timing stress | required | required | later | ✅ |
| D036 six-hour inherited transport proof | required | required | n/a | ✅ |
| D036 realtime boundedness audit | inspect/test | required | n/a | ✅ |
| Phase 3 aggregate exit | required | required | physical caveat | ✅ run `40084706845` |

Final Phase 3 target footprint: 66,960 B image / 13,296 B normal RAM / 113,000 B pool.

Detailed evidence: `WP3_1_VERIFICATION.md` through `WP3_10_VERIFICATION.md` plus the D032-D036 contract/test-plan documents.

## Phase 4 - first engine family: 8-BIT/chip - COMPLETE

| Package / invariant | Host | Device build | Hardware | Status |
|---|---:|---:|---:|---|
| WP4.1 / D037 engine/realtime/resource contract | inspect/gate | required | n/a | ✅ |
| D037 hard ceilings: image 132,496 / RAM 21,488 / pool 129,384 B | inspect/gate | required | n/a | ✅ |
| WP4.2 / D038 PULSE deterministic primitive | required | required | optional later | ✅ |
| WP4.3 / D039 NOISE deterministic maximal-period primitive | required | required | optional later | ✅ |
| WP4.4 / D040 TRIANGLE deterministic primitive | required | required | optional later | ✅ |
| WP4.5 / D041 WAVETABLE-8 tables/vectors | required | required | optional later | ✅ |
| WP4.6 / D042 common integer ADSR | required | required | optional later | ✅ |
| WP4.7 / D043 deterministic eight-slot allocator | required | required | required later | ✅ |
| D043 no stealing of still-owned active cross-track primary voice | required | required | required later | ✅ D046 re-proved |
| WP4.8 / D044 engine pages + six stable parameter IDs | required | required | optional later | ✅ |
| D044 IDs `0x20..0x25` stable | required | compile | n/a | ✅ |
| WP4.9 / D045 all-eight-track linked 8-BIT integration | required | required | optional later | ✅ run `40116897104` |
| Integrated PULSE/NOISE/TRIANGLE/WAVETABLE path | required | required | optional later | ✅ |
| NOTE/TIE/OFF/DELAY/RETRIG/CUT + D031/D032/D044 precedence | required | required | optional later | ✅ |
| Normal linked default WAV frozen | required | required | n/a | ✅ `17fa5ce1759c...` |
| D046 production remains byte-identical to `c93c39ad...` | git compare | required | n/a | ✅ |
| D046 exact 2,048-record workload over 8 tracks x 64 rows | required | required | n/a | ✅ 512 owners / 4 engines |
| D046 representative 64-row render deterministic | required | required | optional later | ✅ sample FNV `abbd4fcd`, state FNV `b5f80714` |
| D046 representative WAV repeat byte-identical | required | required | n/a | ✅ SHA `33e819237fb0...`, 353,024 frames |
| D046 exact representative D033 action counts | required | required | n/a | ✅ NONE 2958 / DELAY 23 / CUT 37 / RETRIG 62 |
| D046 exact representative allocator outcomes | required | required | n/a | ✅ NONE 0 / REUSE 224 / FRESH 36 / STEAL_TAIL 68 |
| D046 4,096-row / 64-wrap linked long run | required | required | optional later | ✅ `ticks=163840`, step 0 / phase 0 |
| D046 long-run sample/state digest repeat identity | required | required | n/a | ✅ `a86caee1` / `53ae5fad` |
| D046 exact long-run D033 action counts | required | required | n/a | ✅ NONE 188808 / DELAY 1472 / CUT 2368 / RETRIG 3968 |
| D046 exact long-run allocator outcomes | required | required | n/a | ✅ NONE 0 / REUSE 14336 / FRESH 1800 / STEAL_TAIL 4352 |
| D046 active cross-track steals | required | required | required later | ✅ exactly 0 |
| D046 allocator classification invariant failures | required | required | n/a | ✅ exactly 0 |
| D046 concurrent D034 pressure/recovery | required | required | optional later | ✅ overflow 16 / 40, recovery 1 |
| D046 queue callback bound | inspect/test | required | n/a | ✅ capacity/cap 32, O(1) discard |
| D046 linked render scans exactly eight fixed slots | inspect/test | required | n/a | ✅ |
| D046 row boundary wins / one six-tick clock | inspect/test | required | n/a | ✅ |
| D046 no allocator/heap, store traversal or screen work in realtime path | inspect/test | required | n/a | ✅ |
| D046 no float/double or realtime 64-bit types in audited leaves | inspect/test | required | n/a | ✅ |
| D046 inherited non-obsolete Phase 2-4 regressions | required | required | physical caveat | ✅ |
| D046 pinned target image/RAM/pool | n/a | required | n/a | ✅ 72,504 / 19,604 / 113,000 B |
| D046 target BIN identity | n/a | required | n/a | ✅ `214fba8ec0af...` |
| D046 target FWSC identity | n/a | required | n/a | ✅ `20f0bbe556ed...` |
| D046 complete Phase 4 exit | required | required | physical caveat | ✅ run `40124536375`, job `120963022289` |
| Representative eight-track physical execution margin | n/a | n/a | required later | ⬜ no physical FM-1 |
| Physical install/audio/control/screen validation | n/a | n/a | required later | ⬜ no physical FM-1 |

Final Phase 4 evidence:

- `WP4_1_VERIFICATION.md` through `WP4_9_VERIFICATION.md`
- `WP4_10_PHASE4_EXIT_CONTRACT.md`
- `WP4_10_VERIFICATION.md`

Final Phase 4 production remains the D045 baseline `c93c39ad344760b4da0dfbc76829ca5fef886174`.

## Phase 5 - mixed-engine routing + FM4 - IN PROGRESS

Normative direction: `D047_MIXED_ENGINE_ARCHITECTURE.md`.

### WP5.0 - active

| Check | Host | Device build | Status |
|---|---:|---:|---|
| WP5.0 routing/migration contract frozen before production changes | required | n/a | ⬜ active next action |
| Current project-wide family gate captured by focused pre-migration test | required | compile | ⬜ |
| Per-instrument family + engine is authoritative routing identity | required | required | ⬜ |
| `FmtSongMeta.mode` no longer gates otherwise valid instrument playback | required | required | ⬜ |
| Different engine families coexist across logical tracks | required | required | ⬜ |
| Same track can switch instrument across families deterministically | required | required | ⬜ |
| Max one owned primary sequenced voice per logical track across families | required | required | ⬜ |
| Still-owned active cross-track primary voices cannot be stolen | required | required | ⬜ |
| Cross-family allocator/ownership representation measured and frozen | required | required | ⬜ |
| Dispatch is static/bounded, no dynamic plugin architecture | inspect/test | required | ⬜ |
| Non-applicable p-lock IDs cannot be reinterpreted by another family | required | required | ⬜ |
| Common publication/parameter adapter boundary frozen | required | required | ⬜ |
| All-8-BIT D046 configuration remains regression-identical | required | required | ⬜ |
| WP5.0 resource delta captured inside explicit budget | required | required | ⬜ |

### FM4 after WP5.0

| Check | Host | Device build / hardware |
|---|---:|---:|
| WP5.1 four-operator fixed-point primitive | required | required |
| WP5.2 curated algorithm routing | required | required |
| WP5.3 ratio table / pitch correctness | required | required |
| WP5.4 operator levels / modulation index | required | required |
| WP5.5 bounded feedback | stress | required |
| WP5.6 envelope strategy | required | required |
| WP5.7 voice allocation/integration | required | required |
| WP5.8 semantic parameter pages + p-lock mapping | required | required |
| WP5.9 mixed 8-BIT + FM4 validation | required | required |
| WP5.10 worst-case mixed-engine benchmark / Phase 5 exit | benchmark | required |

Physical callback execution margin remains a later physical-FM-1 gate.

## Mandatory live performance capture milestone

This is core product work after the engine foundations, not optional Phase 9 work.

| Check | Host | Hardware |
|---|---:|---:|
| REC can arm/disarm while playback continues | required | required |
| Live notes map deterministically to tracker time | required | required |
| Track switching during capture cannot misroute writes | required | required |
| Drum capture representation/layout frozen and deterministic | required | required |
| Metronome locked to transport | required | required |
| CHANNEL/GLOBAL automation ownership explicit | required | required |
| Parameter/FX capture bounded, not an unbounded ADC stream | required | stress |
| Reference drums+bass+poly+lead configuration meets measured budget | benchmark | required |
| Poly target measured without silently reducing eight tracker tracks | benchmark | required |

## Phase 6 - song/order/persistence

| Check | Host | Device/hardware |
|---|---:|---:|
| Order-list model and safe pattern switching | required | required |
| Repeat/loop ranges deterministic | required | required |
| Binary format magic/version/length/CRC | required | required |
| Serializer round trip | required | optional |
| Corrupt/truncated/future data fails safely | required | required |
| Save/load preserves reference project | required | required |
| Save path does not unexpectedly block audio | inspect | required |
| Migration fixture retained per format version | required | n/a |
| Power-cycle round trip | n/a | required |

## Phase 7 - physical hardware UX freeze

Physical FM-1 required:

- STEP REC understandable and fast;
- REST/TIE/OFF gestures comfortable;
- white-key step pages unambiguous;
- black-key track selection avoids accidental notes/edits;
- chord behavior matches expectation;
- tracker rows readable on 240x240;
- p-lock hold/delete ergonomics comfortable and deliberate;
- live PLAY/REC, metronome and CHANNEL/GLOBAL performance FX feel immediate;
- measured execution margin is acceptable under representative worst case.

## Phase 8 - interoperability

Required before any claim of Furnace compatibility beyond inspiration/workflow reference:

- internal interchange specification;
- MIDI transfer investigation;
- Furnace export/import audit;
- converter prototype if legally/technically justified;
- explicit round-trip/non-round-trip decision.

D003 remains authoritative: Furnace is a workflow/interoperability reference, not a full engine port.

## Phase 9 - optional extensions

Only after core milestones and explicit budget/license review:

- WAVETABLE-16;
- PCM/sample engines beyond approved core drum needs;
- swing/microtiming/polymeter;
- additional modern VA/supersaw/wavetable families;
- extra DSP effects;
- licensed chip cores.

Live performance capture is not optional Phase 9 work.

## Regression rule

Once a check becomes ✅, later phases keep it passing. If a deliberate design change invalidates an old assertion, record a superseding decision and replace the assertion explicitly rather than silently dropping coverage.
