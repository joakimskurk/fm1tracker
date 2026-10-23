# FM1Tracker Roadmap

This roadmap is ordered to reduce risk on the FM-1 hardware. A phase is complete only when its exit criteria pass; later features should not bypass a failing earlier layer.

## Phase 0 — foundation

Goal: freeze the product boundary before firmware code grows around assumptions.

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

Tasks:

1. import a pinned Dinghy baseline with history/provenance recorded;
2. preserve upstream GPL/credits/headers;
3. add full GPL-3.0-only license text;
4. rename application identity/name/build artifacts to FM1Tracker;
5. choose a currently unused FM-1 package identity after checking the current Firmware Hub/list;
6. verify unchanged host build;
7. verify firmware build safety checks;
8. create `THIRD_PARTY.md` with exact upstream commit;
9. add CI if the toolchain can be exercised reliably in GitHub Actions without redistributing restricted SDK material.

Exit criteria:

- `tests/run_host.sh` or equivalent produces FM1Tracker host audio/screen output;
- firmware build completes with no forbidden symbols/RAM overflow;
- repo contains clear provenance and reproducible setup instructions.

## Phase 2 — tracker core without synth ambition

Goal: prove editing and transport with the simplest possible tone source.

Tasks:

- implement V0.1 song structs in pool RAM;
- implement pattern/track/step bounds-safe accessors;
- integer transport clock;
- 64-step playback / 4 x 16 pages;
- single simple oscillator voice for audible validation;
- white-key step selection;
- track selection prototype;
- STEP REC note entry;
- dedicated REST action;
- NOTE OFF / TIE semantics;
- screen tracker row view;
- dirty-region rendering;
- host tests for wrap/bounds/timing.

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

Version numbers are illustrative until the baseline build/release tooling is imported.
