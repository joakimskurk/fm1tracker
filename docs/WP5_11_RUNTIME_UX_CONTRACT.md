# WP5.11 Runtime UX Integration Contract

Status: FROZEN FOR IMPLEMENTATION  
Issue: #55  
Branch: `phase5/runtime-ux-integration`  
Baseline production: `81d3464059270130747e26fcc950b7a2e1bb03e1`

## Why this package exists

Manual emulator validation after Phase 5 closeout proved that software-complete engine features were not product-accessible. The firmware exposed HOME/SEQ/STEP REC but left most implemented engine and parameter functionality unreachable or hidden behind non-discoverable gestures.

WP5.11 is a Phase 5 runtime integration correction. Phase 6 remains blocked until this package is validated.

## User-visible control contract

### Global transport/context

- HOME: performance context. Exits instrument editor.
- SEQ: tracker context. Exits instrument editor.
- PLAY: start/stop tracker playback.
- STOP semantics are fail-silent: PLAY -> STOP hard-silences all sequenced tracker routes and resets playback-start state.
- REC: existing stopped STEP REC behavior remains unchanged.

### Track and instrument editor

- SELECT encoder: select logical track 1..8 while transport is stopped and no held-step p-lock gesture owns SELECT.
- EDIT: enter/exit the explicit instrument editor when transport is stopped and no held-step editor owns EDIT.
- Entering instrument edit exits STEP REC and moves to performance context while preserving selected track.
- PRESETS encoder: select instrument 1..16 for the selected track.
- ALGORITHM encoder: select one of the already implemented engines only:
  1. PULSE
  2. NOISE
  3. TRIANGLE
  4. WAVETABLE8
  5. FM4
- Engine/instrument/base edits are stopped-transport only in WP5.11.

### Parameter editor

- K1-K4 always edit exactly the four values named on the instrument-editor screen.
- 8-BIT engines expose one VOICE page using the frozen D044 mappings:
  - PULSE: PW / DEC / SUS / REL
  - NOISE: ATT / DEC / SUS / REL
  - TRIANGLE: ATT / DEC / SUS / REL
  - WAVETABLE8: WAV / DEC / SUS / REL
- FM4 exposes the seven frozen D056 pages:
  - CORE: ALG / IDX / FB / -
  - RATIO: R1 / R2 / R3 / R4
  - LEVEL: L1 / L2 / L3 / L4
  - ATTACK: A1 / A2 / A3 / A4
  - DECAY: D1 / D2 / D3 / D4
  - SUSTAIN: S1 / S2 / S3 / S4
  - RELEASE: R1 / R2 / R3 / R4
- OCT-/OCT+ page backward/forward where multiple pages exist.
- ENV jumps into the FM envelope pages and then cycles ATTACK -> DECAY -> SUSTAIN -> RELEASE.

### Live audition

While the explicit instrument editor is active and the transport is stopped, keyboard audition must use the selected instrument's actual 8-BIT/FM4 engine and base parameters. It must not use the old generic Dinghy sine audition path. Release must use the selected engine's envelope release semantics and must not leave stuck voices.

The historical four-sine audition path remains available outside instrument edit so WP5.11 does not silently invalidate the Phase 5 worst-case benchmark contract.

### Unsupported controls

WP5.11 must not invent functionality that does not exist:

- LFO remains unsupported/unlit.
- ARP remains unsupported/unlit.
- SAVE remains unsupported until Phase 6 persistence.
- GLO remains unassigned unless a later frozen package defines it.

## Screen contract

Instrument edit owns the LCD while active and must show, without hidden state:

- selected track;
- selected instrument;
- engine name;
- parameter page;
- K1-K4 labels and current values;
- concise control hints for track, instrument, engine, page and exit.

A feature that is only reachable by knowing source-code gestures does not satisfy this package.

## Compatibility contract

WP5.11 must preserve:

- 8 logical tracks;
- existing HOME/SEQ context behavior;
- STEP REC note/special entry;
- held-step p-lock editor and delete grammar;
- existing D056 FM p-lock page behavior;
- mixed 8-BIT/FM routing semantics;
- no physical hardware margin claim.

## Required verification

1. Existing pattern-context host regression passes.
2. Existing host smoke passes.
3. STOP-silence regression passes.
4. New runtime-editor host regression proves:
   - EDIT entry;
   - SELECT track change;
   - PRESETS instrument change;
   - ALGORITHM engine change to FM4;
   - ENV page selection;
   - K1 base-parameter mutation;
   - EDIT exit.
5. Pinned AC79 target build succeeds.
6. Emulator validation confirms the controls are understandable and responsive.

`hardware_margin_claim=none` remains until physical FM-1 evidence exists.
