# FM1Tracker Polyphony and Poly Editor Direction

Status: **accepted product direction before Phase 6 format freeze**  
Date: 2026-10-26

This document records the intended relationship between logical tracker tracks, polyphonic note storage, voice allocation and the future editor. It is a product/architecture direction, not an implementation work package.

## 1. Current state

The current V0.1 tracker model is monophonic per logical track row:

- `FmtStep` contains one `note` field;
- each logical track currently owns at most one active primary sequenced voice;
- release tails may coexist, but they are not additional editable note lanes;
- the four-voice HOME sine audition scaffold is not tracker polyphony;
- the accepted performance target of 3-6 voices for at least one instrument/track is future work, not current behavior.

Therefore FM1Tracker does not yet have true editable chord/polyphonic tracker rows.

## 2. Core direction

Polyphony must remain a property of one logical track/instrument. It must **not** consume several of the eight logical tracker tracks merely to represent several notes of one chord.

The intended relationship is:

```text
logical track -> one instrument -> several note lanes -> several engine voices
```

The eight logical tracks remain available for musical roles and mixed engines.

## 3. NerdSEQ inspiration - what to take

The useful reference from the NerdSEQ MIDI track is its row-level polyphonic representation: one tracker row can expose several independent note columns rather than forcing one chord across several tracks.

FM1Tracker should adopt the principle, not clone the whole NerdSEQ MIDI implementation.

Useful ideas:

- several visible note lanes inside one track editor;
- one row represents one chord/voicing event;
- independent note continuation/off semantics per lane;
- optional per-note velocity/level where the resource and UI contracts justify it;
- live/step recording of several simultaneously played notes into those lanes;
- deterministic lane assignment so replay/editing remains stable.

Not adopted as a baseline:

- grouping several logical FM1Tracker tracks to obtain polyphony;
- MIDI-specific CC/NRPN/aftertouch complexity;
- large multi-page MIDI parameter surfaces unrelated to the selected internal instrument;
- functionality that exists only because NerdSEQ must drive external modular/MIDI hardware.

## 4. V1 editor target - four visible note lanes

The preferred first bounded poly editor target is **four note lanes per polyphonic logical track**.

Four is chosen because it:

- covers triads, seventh chords and most compact keyboard voicings;
- maps naturally to the proven NerdSEQ-style four-note tracker concept;
- is realistic on the FM-1 240x240 display;
- avoids prematurely promising six simultaneous editable columns before measured hardware headroom exists;
- still leaves the architecture able to extend beyond four later if justified.

The previous 3-6 voice performance target is not contradicted: four active editable note lanes are the preferred first product baseline, while a later measured implementation may raise the engine voice limit or add additional lane paging.

## 5. Poly track view

A poly-capable instrument should be allowed to use a different tracker row presentation from a monophonic instrument.

Conceptual row:

```text
ST  N1   N2   N3   N4   LK FX
01  C-4  E-4  G-4  B-4  -- ----
02  ===  ===  OFF  ===  L1 VI20
```

Exact pixel layout is not frozen here.

Requirements:

- the user can see the chord as several notes on the same row;
- cursor focus can move between note lanes without changing logical track;
- EMPTY / NOTE / TIE / OFF semantics exist independently per lane where applicable;
- common row/instrument/FX information must remain accessible without turning the 240x240 screen into a desktop tracker clone;
- detailed per-note velocity/level may use a focused step/poly editor or secondary page instead of crowding the main row.

## 6. Storage direction - do not widen every FmtStep blindly

The current core song already spends most of its bounded editable-song budget. Adding three additional note bytes to every step would add 24,576 bytes across 16 patterns x 8 tracks x 64 steps before per-note velocity or metadata, and would exceed the current 96 KiB editable-song target when combined with the existing lock budget.

Therefore Phase 6 must **not** assume that polyphony is represented by simply widening every `FmtStep` to four full note columns.

Preferred design direction:

- keep the existing primary note as the compact/common monophonic case;
- represent additional note lanes through a bounded, explicitly versioned poly/chord extension structure or sparse pool;
- allocate extension data only for rows/tracks that actually require more than one note;
- keep lookup bounded and publish audio-ready snapshots before the audio ISR;
- define hard capacity/error behavior rather than falling back silently.

The exact record size, pool size, indexing and persistence encoding require a dedicated contract and measurement before implementation.

## 7. Lane semantics

A polyphonic row needs deterministic lane identity across rows.

Preferred first semantics:

- lane 1 is the existing/base note lane;
- lanes 2-4 are explicit extension lanes;
- each lane may independently contain EMPTY, NOTE, TIE or OFF;
- note-off/release affects the owning lane/voice rather than all voices on the logical track unless an explicit all-off command is used;
- retrigger/tie behavior must be deterministic per lane;
- p-locks remain track/row semantic parameter locks by default, not four duplicated parameter-lock sets per note.

Per-note expression is a later explicit feature. Do not multiply the entire p-lock model by four without a separate need/resource contract.

## 8. Recording direction

### STEP REC

For a poly-capable instrument, a chord gesture should be recordable into one tracker step.

The existing monophonic lowest-note/full-release rule remains valid for monophonic instruments, but must not define future poly STEP REC.

A future poly STEP REC package should:

- collect a bounded chord gesture;
- sort/assign notes to lanes deterministically;
- write one row and advance once;
- define overflow behavior when more notes are played than supported lanes;
- preserve exact auditioned pitches.

### Live capture

Future live capture should record simultaneous notes into the same logical track and resolve them into bounded note lanes/events rather than spreading them into neighboring tracks.

Voice/lane assignment and note-off matching must be deterministic and editable afterward.

## 9. Voice allocation

Storage polyphony and synthesis polyphony are related but not identical.

A poly-capable engine must define:

- maximum simultaneous active voices for that instrument;
- deterministic lane-to-voice ownership;
- deterministic stealing or refusal behavior at capacity;
- coexistence with the other seven logical tracks;
- bounded release-tail behavior;
- resource failure behavior that never silently changes engine type or drops arbitrary notes.

No implementation may obtain polyphony by silently reducing the eight logical tracker tracks.

## 10. Phase 6 consequence

Before the project format freezes, Phase 6 must allow a future bounded poly/chord extension without requiring a destructive redesign of saved songs.

The format audit must explicitly account for:

- monophonic rows remaining compact;
- optional multi-note row/chord data;
- versioned lane count/capability;
- independent lane NOTE/TIE/OFF semantics;
- migration and unknown-extension behavior;
- bounded sparse allocation/capacity errors;
- compatibility with mixed engine identity and future live capture.

Phase 6 does not need to implement poly playback/editor code, but it must not make the accepted direction impossible or unnecessarily wasteful.

## 11. Product rule

A polyphonic FM1Tracker instrument should feel like **one instrument on one tracker track playing a chord**, not like four monophonic tracks disguised as polyphony.
