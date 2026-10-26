# FM1Tracker Product Refinement Direction

Status: **accepted product direction before Phase 6 format freeze**  
Date: 2026-10-26

This document records the product-sharpening direction that must survive future chats and work-package transitions.

FM1Tracker now has a functioning tracker, bounded realtime architecture, mixed 8-BIT/FM routing and FM4 foundation. The next product goal is not feature count for its own sake. The existing skeleton must become a stronger, more immediate musical instrument.

## 1. Product refinement principle

The refinement target is:

> **Playable first, characterful immediately, deep when opened.**

The tracker remains the source of truth, but the first interaction with FM1Tracker should demonstrate the actual instrument architecture rather than a legacy test voice.

New work must therefore prioritize:

- immediate musical usefulness;
- coherent instrument identity;
- strong default sounds;
- fast visual understanding of what is sounding;
- focused editors rather than exposing implementation detail;
- bounded realtime behavior and deterministic tracker semantics.

This direction does not authorize unbounded UI/DSP work and does not bypass the active work-package sequence.

## 2. HOME / performance surface

The current inherited Dinghy HOME screen is a temporary scaffold, not the intended product surface.

The future HOME/performance surface must expose at minimum:

- selected logical track;
- selected/current instrument identity;
- engine family + engine identity in a compact form;
- the actual active played note/voice state;
- simple activity feedback for all eight logical tracker tracks;
- a lightweight spectrum/audio visualization;
- immediate access to playing the selected instrument.

### 2.1 Real instrument audition

Manual keyboard audition must eventually use the selected track's actual selected instrument and common engine routing path.

The current four-voice sine audition path is a development scaffold only. It must not define the final HOME sound or instrument experience.

Audition replacement must preserve:

- bounded main-to-audio ingress;
- deterministic voice ownership;
- safe engine-family routing;
- no screen work in the audio callback;
- explicit behavior when an instrument/engine cannot allocate the requested audition voice.

### 2.2 Spectrum visualization

HOME should include a Furnace-inspired spectrum/audio visualization because it gives useful movement and immediate feedback without turning the screen into a meter dashboard.

Hard architectural requirements:

- no display calls from the audio ISR;
- no unbounded FFT/DSP work in the audio ISR;
- audio may publish a small bounded/decimated analysis buffer or bounded band-energy snapshot;
- visualization processing and drawing belong to the main/UI side;
- update rate must be capped and independently benchmarked;
- integer/fixed-point implementation is preferred under the existing FM-1 realtime rules;
- the feature must be degradable or disabled if measured hardware headroom cannot support it safely.

The visualizer is presentation feedback, never a source of musical state.

### 2.3 Eight-track activity feedback

HOME should make it obvious which logical tracks are currently active without reproducing the full tracker.

The target is a compact eight-track strip showing at least:

- selected-track emphasis;
- active/inactive state;
- current active note where meaningful, otherwise a simple activity marker;
- deterministic mapping to tracker tracks 1..8.

This display must read published/bounded runtime state and must not mutate engine or sequencer state.

## 3. Instrument editor as a first-class product surface

FM1Tracker needs a coherent instrument editor rather than only engine parameter pages and tracker p-lock gestures.

The common instrument-editor shell should eventually provide:

- instrument number/name;
- engine family + engine selection;
- primary performance parameters;
- focused engine-specific pages where needed;
- clear distinction between persistent base instrument parameters and row-local p-lock overrides;
- future access to drum note maps, sample assignment and other engine-specific structures without creating a parallel application.

The editor should expose musical concepts first. Internal storage layout must not leak into the UX unless it is itself musically meaningful.

## 4. Drum instrument / NOTE MAP direction

A Furnace-style drum keyboard map is now an explicit product requirement.

A drum-oriented instrument must be able to assign different sounds to different keyboard notes. The architectural model should be more general than `note -> PCM file`:

```text
KEY / NOTE -> SOUND SLOT -> SOURCE
```

`SOURCE` may later resolve to:

- a PCM/sample asset;
- a dedicated synthesized drum voice;
- another explicitly supported bounded drum source.

This allows arbitrary kit layouts without making the tracker track itself a special drum track.

### 4.1 Required musical behavior

The future drum-note-map surface should support:

- arbitrary note-to-sound assignment within the supported keyboard/note range;
- live audition of the mapped sound;
- use of the same mapped instrument during tracker playback and future live capture;
- deterministic replay from normal tracker note events;
- a clear `unassigned` state;
- no hidden remapping based on track number.

Likely per-slot controls may include pitch, level, decay and choke/exclusive grouping, but exact fields require a dedicated drum-engine contract and resource audit before implementation.

### 4.2 Tracker compatibility

The tracker should continue to store an ordinary note event plus instrument identity. The drum instrument resolves that note through its NOTE MAP.

Do not redesign the eight logical tracker lanes into fixed drum/melodic channel types.

## 5. Sample/asset architecture direction

PCM/sample playback is still conditional on storage, transfer, image, RAM and realtime budgets. It is not silently promoted to an implemented V1 feature by this document.

However, Phase 6 persistence must not make future sample-backed instruments unnecessarily difficult.

The project format must be capable of representing, through versioned/extensible structures:

- stable instrument identity and human-readable instrument name;
- engine family + engine identity;
- engine-specific instrument payloads;
- optional NOTE MAP data;
- stable asset/sample references that are not raw transient runtime pointers;
- future user-wavetable or sample metadata if those features are later accepted;
- unknown/newer instrument blocks in a way that supports explicit validation/migration/failure behavior.

Raw C structure dumps and hard-coded host file paths are not acceptable persistence contracts.

## 6. Stronger sonic starting point

Engine primitives alone are not the product's sonic identity.

FM1Tracker should ship with a deliberately curated factory sound bank using the engines that actually exist.

Before adding additional synthesis families merely to increase feature count, the existing 8-BIT and FM4 families should demonstrate a musically convincing range of starting sounds.

Target categories include, where the current engines support them well:

- chip basses;
- chip leads/plucks;
- digital basses;
- FM bells/keys;
- metallic/percussive FM sounds;
- FM plucks/stabs;
- useful noise/percussion patches;
- intentionally strange/digital character sounds.

Factory sounds must be treated as testable product content:

- deterministic parameter data;
- valid under the same persistence/versioning rules as user instruments;
- no hidden alternate DSP path;
- representative enough to expose weak engine defaults or broken parameter ranges.

## 7. Relationship to Phase 6

Phase 6 remains the next implementation phase.

The refinement direction changes the Phase 6 **precontract requirements**, not its fundamental purpose.

Before the first persistence implementation is frozen, the Phase 6 audit/contract must explicitly account for:

1. human-readable instrument identity/name;
2. extensible per-engine instrument payloads;
3. future drum NOTE MAP representation;
4. future sample/asset identifiers without raw path coupling;
5. migration/unknown-block/corruption behavior;
6. preservation of the existing per-instrument mixed-engine identity;
7. factory instruments using the same serialized model as normal instruments where practical.

Phase 6 does **not** need to implement the drum engine, PCM playback, spectrum UI or final instrument editor in order to satisfy this requirement.

## 8. Required future refinement work packages

After their dependencies are ready, the roadmap must include bounded packages for:

- HOME/performance replacement of legacy sine-scaffold audition;
- compact eight-track activity publication/display;
- bounded HOME spectrum analysis/display;
- common instrument-editor shell;
- curated factory instrument bank and musical-default review;
- drum-engine preimplementation audit;
- drum NOTE MAP data/editor contract;
- sample/asset audit before any PCM implementation.

Exact package numbering/order must be chosen from the then-current repository state rather than invented here.

## 9. Non-goals of this decision

This document does not:

- implement PCM/sample playback;
- guarantee user sample import;
- guarantee user wavetables;
- freeze drum DSP architecture;
- freeze final HOME pixel layout;
- freeze the final instrument-editor control map;
- require a desktop-Furnace clone UI;
- weaken realtime/resource gates;
- change the active Phase 6 prerequisite sequence.

Furnace is a workflow and interaction reference where useful. FM1Tracker remains a hardware-first instrument designed around the FM-1.
