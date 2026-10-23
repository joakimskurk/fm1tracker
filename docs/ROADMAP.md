# FM1Tracker Roadmap

This roadmap is ordered to reduce risk on the FM-1 hardware. A phase is complete only when its exit criteria pass; later features should not bypass a failing earlier layer.

## Phase 0 — foundation

Goal: freeze the product boundary before firmware code grows around assumptions.

Status: **COMPLETE**

Deliverables:

- architecture contract
- provisional control map
- V0.1 data model
- V0.1 8-BIT / 16-BIT engine scope
- licensing/provenance policy
- implementation roadmap

Exit criteria:

- song model fits stated pool budget on paper;
- no V0.1 feature requires floating point or unbounded real-time work;
- STEP REC, REST and p-lock gestures have explicit behavior;
- full Furnace port is explicitly out of scope.

## Phase 1 — clean Dinghy baseline

Goal: make `fm1tracker` build and run as its own FM-1 firmware before tracker features are added.

Status: **COMPLETE — 2026-10-23**

Verified:

- Dinghy provenance is pinned and licensed material is documented;
- the minimal platform import matches pinned upstream blobs and Git modes;
- pristine and renamed host builds produce verified audio/screen artifacts;
- JieLi toolchain acquisition/execution works;
- FM1Tracker identity is fixed as `FM-1_987` after a current Firmware Hub collision check;
- renamed host artifacts and visible screen identity are verified;
- renamed host audio is bit-identical to the pristine baseline;
- protected Dinghy app/platform/HAL/loader/build-driver implementation remained unchanged through Phase 1;
- official JieLi AC79 SDK tag `AC79NN_SDK_V1.2.1_2023-12-13` is externally pinned to commit `d179b4484759423312073f5fbb232501aa491047` and its consumed package inputs are hash-verified;
- complete renamed device firmware/package build passes with inherited safety checks;
- clean reference usage is 55,088 B image, 12,368 B normal RAM and 30,720 B pool RAM;
- nominal callback period is 5.804988662 ms at 256 frames / 44.1 kHz;
- actual physical-hardware render time and install behavior remain untested because no FM-1 is currently available.

See `WP1_3_VERIFICATION.md`, `WP1_4_VERIFICATION.md`, `WP1_5_VERIFICATION.md` and `TEST_MATRIX.md` for exact verification levels.

Completed tasks:

1. ✅ import a pinned Dinghy baseline with history/provenance recorded;
2. ✅ preserve upstream GPL/credits/headers;
3. ✅ add full GPL-3.0-only license text;
4. ✅ rename application identity/name/build artifacts to FM1Tracker;
5. ✅ choose a currently unused FM-1 package identity (`FM-1_987`);
6. ✅ verify unchanged pristine/renamed host behavior;
7. ✅ verify renamed firmware build safety checks;
8. ✅ create/maintain `THIRD_PARTY.md` with exact upstream and SDK provenance;
9. ✅ add automated pristine verification without redistributing SDK material;
10. ✅ add automated identity-only host regression verification;
11. ✅ complete renamed device-build/resource baseline and reproducible setup closeout.

Phase 1 exit criteria are satisfied at the host/firmware-build level. Physical FM-1 testing remains an explicit later hardware verification layer rather than a claimed Phase 1 result.

## Phase 2 — tracker core without synth ambition

Goal: prove editing and transport with the simplest possible tone source.

Status: **IN PROGRESS — WP2.1 and WP2.2 complete; WP2.3 is next**

Tasks:

1. ✅ WP2.1 — compile-time V0.1 data model, constants, bounds-safe accessors, static size assertions and host/device compile checks. No UI/audio/transport changes.
2. ✅ WP2.2 — one actual 65,896 B `FmtSong` in `.pool`, explicit initialization of all 8,192 steps, runtime bounds/reset tests and measured resource delta. Evidence: `WP2_2_VERIFICATION.md`.
3. ⬜ WP2.3 — integer/fixed-point transport clock with deterministic 0..63 wrap and long-run drift/error tests. GitHub issue #11 is READY TO START; no implementation yet.
4. ⬜ WP2.4 — one-track audible playback using the simplest baseline tone.
5. ⬜ WP2.5 — white-key 16-step page input across four pages.
6. ⬜ WP2.6 — provisional track-selection prototype.
7. ⬜ WP2.7 — STEP REC note entry.
8. ⬜ WP2.8 — REST / TIE / NOTE-OFF semantics.
9. ⬜ WP2.9 — compact tracker screen with bounded/dirty rendering.
10. ⬜ WP2.10 — Phase 2 integration test: enter and play a complete 64-step melody without a computer.

Current verified Phase 2 runtime baseline after WP2.2:

- app image: 55,188 B / 581,564 B;
- normal RAM: 12,368 B / 98,304 B;
- pool RAM: 96,616 B / 344,064 B;
- `FmtSong` store: 65,896 B at `.pool` address `0x01C20000`;
- host audio remains byte-identical to the Phase 1 baseline.

Exit criteria:

- a 64-step melody can be entered from the FM-1 keyboard without a computer;
- REST advances exactly one step;
- playback loops without cumulative timing drift;
- edits during stop mode are deterministic and bounds-safe.

## Phase 3 — parameter locks and tracker effects

Goal: make the sequencer genuinely tracker-like and hardware-native.

Tasks:

- choose/profile the final sparse lock representation;
- max four p-locks/step;
- hold/select step + encoder lock gesture;
- lock replace/delete UI;
- stable parameter ID table;
- first tracker effects: arpeggio, slide, portamento, vibrato, retrig, note delay, note cut, volume slide;
- timestamped/bounded event handoff if NOTE DELAY requires sub-step offsets;
- visible lock/effect markers on screen/LEDs.

Exit criteria:

- worst-case lock data remains inside budget;
- no lock edit can corrupt current audio state;
- effect behavior is documented and host-tested;
- queue overflow behavior is deterministic.

## Phase 4 — 8-BIT engine

Goal: make 8-BIT mode musically complete enough for real patterns.

Order:

1. PULSE
2. NOISE
3. TRIANGLE
4. WAVETABLE-8

Tasks:

- fixed-point oscillators;
- envelope primitive;
- 8-voice target with measured stealing policy;
- primary four-parameter pages;
- p-lock parameter mappings;
- pitch and retrigger integration;
- hardware timing benchmarks.

Exit criteria:

- representative eight-track pattern remains below audio timing budget;
- no forbidden float/runtime 64-bit divide paths;
- p-locks can alter engine parameters without instability;
- pitch is correct across intended keyboard range.

## Phase 5 — 16-BIT FM4 engine

Goal: establish a distinct, deeper second sound mode without destabilizing the tracker core.

Tasks:

- four fixed-point operators;
- curated algorithm table;
- ratio table;
- modulation index/levels;
- bounded feedback;
- shared/common envelope first;
- deterministic voice allocation;
- p-lock mappings;
- hardware worst-case benchmark.

Exit criteria:

- clearly distinct 16-BIT sonic palette;
- stable pitch/modulation behavior;
- normal worst-case render target remains below half audio callback budget or voice/algorithm limits are explicitly reduced.

## Phase 6 — song/order mode and persistence

Goal: move from one pattern to complete songs.

Tasks:

- 128-entry order list;
- repeats/loop range;
- pattern switching at safe boundaries;
- project serializer with magic/version/length/CRC;
- audit and allocate FM1Tracker flash region;
- explicit save/load UI;
- corrupted-project recovery;
- migration tests.

Exit criteria:

- power-cycle/save/load round trip preserves a test song;
- malformed project data fails safely;
- save path never blocks the audio ISR unexpectedly.

## Phase 7 — hardware UX freeze

Goal: validate the provisional mappings on a physical FM-1 and remove awkward gestures.

Test specifically:

- FX as REST;
- white keys as 16 steps;
- black keys as track selectors;
- hold-step + knob p-lock ergonomics;
- chord entry advance behavior;
- OCT buttons as page navigation;
- readability of the tracker view on 240 x 240.

Exit criteria:

- final V1 control map documented;
- no frequently used action needs an uncomfortable multi-button gesture;
- STEP REC can be used without consulting documentation after initial learning.

## Phase 8 — interoperability

Goal: connect FM1Tracker to desktop workflows without turning the firmware into Furnace.

Candidates:

- MIDI dump/import of patterns/projects;
- host-side project converter;
- Furnace documented export/subset investigation;
- optional one-way Furnace -> FM1Tracker conversion for supported notes/instruments/effects;
- later round-trip only if semantics can be preserved honestly.

Every imported format is a compatibility layer outside the real-time synth core.

## Phase 9 — optional expansion

Only after measurement:

- WAVETABLE-16
- PCM/sample playback
- swing
- microtiming
- per-track lengths/polymeter
- live recording
- mixed 8-BIT/16-BIT tracks
- delay/chorus/reverb
- specifically named chip emulation cores that pass license/resource audits

## Release philosophy

Prefer small playable releases:

```text
0.1 host tracker skeleton
0.2 FM-1 step recorder
0.3 p-lock tracker
0.4 8-BIT playable
0.5 16-BIT FM4
0.6 song/save
1.0 hardware UX freeze
```

Version numbers are illustrative until the release strategy is explicitly frozen.
