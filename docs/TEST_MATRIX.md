# FM1Tracker Test Matrix

This file tracks what must be verified at each layer. It is not a list of tests that already pass.

Legend:
- ✅ verified
- 🟨 partial / host-only / environment-limited
- ⬜ not yet verified
- ⛔ blocked with reason recorded in `PROJECT_STATE.md`

## Platform / baseline

| Check | Host | Firmware build | Physical FM-1 | Status |
|---|---:|---:|---:|---|
| Pinned Dinghy provenance complete | n/a | n/a | n/a | ✅ |
| Pristine Dinghy host build | required | n/a | n/a | ✅ 2026-10-23 - Actions run `39958478536` |
| Pristine Dinghy firmware build | n/a | required | optional | 🟨 pristine device package was not separately built in WP1.3; renamed equivalent baseline is fully built in WP1.5 |
| FM1Tracker package identity selected | n/a | inspect | n/a | ✅ `FM-1_987`, checked against live Hub 2026-10-23 |
| Protected baseline unchanged through Phase 1 | inspect | inspect | n/a | ✅ WP1.4/WP1.5 CI diff gates |
| FM1Tracker renamed host build | required | n/a | n/a | ✅ Actions runs `39969032366`, `39970562995` |
| FM1Tracker visible host screen identity | required | n/a | n/a | ✅ `FM1TRACKER` verified |
| FM1Tracker host audio matches pristine baseline | required | n/a | n/a | ✅ exact WP1.3 WAV SHA-256 match |
| FM1Tracker renamed firmware/package build | n/a | required | optional | ✅ Actions run `39970562995` |
| Package decodes as fixed identity `FM-1_987` | n/a | required | n/a | ✅ |
| JieLi AC79 SDK exact tag/commit + package-input hashes | n/a | required | n/a | ✅ official tag -> `d179b4484759423312073f5fbb232501aa491047` |
| Build safety checks | n/a | required | n/a | ✅ `.ram_text`, RAM/image limits and HAL-only register access pass |
| USB rescue/update path unchanged | n/a | inspect/build | required later | 🟨 source unchanged and package builds; physical update/rescue not tested |
| Baseline RAM usage captured | n/a | required | n/a | ✅ 12,368 B / 98,304 B |
| Baseline pool usage captured | n/a | required | n/a | ✅ 30,720 B / 344,064 B |
| Baseline code/image size captured | n/a | required | n/a | ✅ 55,088 B / 581,564 B |
| Nominal audio callback period documented | useful | useful | n/a | ✅ 256 / 44.1 kHz = 5.804988662 ms |
| Actual baseline audio callback execution time | optional | useful | preferred | 🟨 not measured on physical FM-1; nominal period is not render-time evidence |
| Install/boot/audio/panel behavior on physical FM-1 | n/a | n/a | required before hardware signoff | ⬜ hardware not currently available |

WP1.3 pristine evidence is recorded in `WP1_3_VERIFICATION.md`.

WP1.4 identity-only evidence is recorded in `WP1_4_VERIFICATION.md`.

WP1.5 final renamed device-build/resource evidence is recorded in `WP1_5_VERIFICATION.md`. Phase 1 build verification is complete at the host/firmware-build level. Physical FM-1 verification remains explicitly separate and has not been claimed.

## Phase 2 tracker core

| Check | Automated host test | Firmware build | Hardware | Status |
|---|---:|---:|---:|---|
| V0.1 dimensions/constants are compile-time enforced | required | required | n/a | ✅ WP2.1 run `39985124933` |
| `FmtStep` is exactly 8 B and `FmtPattern` exactly 4,096 B | required | required | n/a | ✅ |
| Empty-step defaults/sentinels deterministic | required | compile | n/a | ✅ |
| Step accessor rejects out-of-range track/pattern/step | required | compile | n/a | ✅ WP2.1 |
| Compile-time song core + conservative lock budget fits 96 KiB target | required | compile | n/a | ✅ 82,280 B / 98,304 B budgeted; allocator not frozen |
| WP2.1 changes no runtime image/audio behavior | required | required | n/a | ✅ host WAV/bin/fwsc bit-identical to Phase 1 |
| WP2.1 resource delta recorded | n/a | required | n/a | ✅ image 0 B, normal RAM 0 B, pool 0 B |
| Pattern store allocation/placement is bounds-safe | required | required | optional | ⬜ WP2.2 |
| Pattern wrap is exactly 64 steps | required | required later | later | ⬜ WP2.3/WP2.4 |
| Four 16-step pages map to 1–64 without overlap/gaps | required | compile | required | ⬜ WP2.5 |
| Integer clock has no cumulative drift over long run | required | required | preferred | ⬜ WP2.3 |
| STEP REC inserts note at cursor | required | required | required | ⬜ WP2.7 |
| Note entry advances exactly one logical step | required | required | required | ⬜ WP2.7 |
| REST creates empty step and advances once | required | required | required | ⬜ WP2.8 |
| TIE semantics deterministic | required | required | required | ⬜ WP2.8 |
| NOTE OFF semantics deterministic | required | required | required | ⬜ WP2.8 |
| Track switch never writes previous track accidentally | required | required | required | ⬜ WP2.6/integration |
| Edit while stopped is deterministic | required | required | required | ⬜ later Phase 2 |
| Tracker display uses bounded/dirty redraw | inspect/test | required | required | ⬜ WP2.9 |

WP2.1 compile-time-model evidence is recorded in `WP2_1_VERIFICATION.md`.

## Phase 3 p-locks/effects

| Check | Automated host test | Hardware |
|---|---:|---:|
| Maximum four locks per step enforced | required | optional |
| Replace existing parameter lock is deterministic | required | required |
| Delete lock cannot corrupt neighboring data | required | required |
| Stable parameter IDs are versioned/documented | required | n/a |
| Lock application does not mutate stored base instrument | required | observable |
| Queue overflow policy deterministic | required | stress test |
| Note delay/cut boundary behavior | required | required |
| Retrig count/timing bounded | required | required |

## Phase 4 8-BIT engine

| Check | Host | Firmware/hardware |
|---|---:|---:|
| Pitch table accuracy across intended range | required | required |
| No float/double symbols | inspect/build | required |
| No runtime 64-bit divide | inspect/build | required |
| PULSE stable at duty extremes | required | required |
| NOISE deterministic where intended | required | required |
| TRIANGLE no overflow/wrap artifact | required | required |
| WAVETABLE index bounds | required | required |
| Voice stealing deterministic | required | required |
| Representative 8-track pattern timing | benchmark | required |
| P-lock modulation remains stable | required | required |

## Phase 5 FM4

| Check | Host | Firmware/hardware |
|---|---:|---:|
| Operator pitch/ratio correctness | required | required |
| Algorithm routing matches specification | required | required |
| Feedback bounded under all values | stress test | required |
| Modulation accumulation cannot overflow unsafely | stress test | required |
| Voice allocation deterministic | required | required |
| Worst-case render timing within accepted budget | benchmark | required |

## Phase 6 persistence

| Check | Automated | Hardware |
|---|---:|---:|
| Serializer round trip byte/semantic equivalence | required | optional |
| Magic/version/length/CRC validation | required | optional |
| Truncated project fails safely | required | optional |
| Corrupt project fails safely | required | required |
| Unknown future version fails safely | required | optional |
| Save/load preserves reference project | required | required |
| Save path does not unexpectedly block audio | host inspect | required |
| Migration fixture retained for every persisted version | required | n/a |

## Phase 7 UX freeze

Physical FM-1 required for final signoff:

- STEP REC is understandable without repeated manual lookup.
- REST gesture is fast and comfortable.
- 16-step key page is visually/physically unambiguous.
- track selection does not cause frequent accidental notes/edits.
- hold-step + knob p-locking is comfortable.
- chord advance behavior matches user expectation.
- tracker rows are readable on 240 × 240 in realistic use.

## Regression rule

Once a check becomes ✅, later phases must keep it passing. If a deliberate design change invalidates a previous check, record a superseding decision and replace the test rather than silently dropping coverage.
