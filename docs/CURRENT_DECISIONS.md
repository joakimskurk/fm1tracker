# FM1Tracker Current Decision Index

Date: 2026-10-24

`DECISIONS.md` remains the append-only historical decision log through D046. Historical entries are not rewritten when a later decision supersedes one of their constraints.

D047 and D048 are the current Phase 5 architecture decisions and live in their dedicated normative documents. They may be consolidated into the append-only historical log at a later documentation checkpoint without changing their effective dates or semantics.

This file is a compact current-supersession index for continuation/handoff.

## Current supersession

### D047 - mixed-engine instrument architecture

Normative document: `D047_MIXED_ENGINE_ARCHITECTURE.md`

Status: **ACCEPTED - frozen before Phase 5 production work**

D047 supersedes:

- D004's restriction that V0.1 projects use one project-wide 8-BIT or 16-BIT sound mode;
- older roadmap/work-package statements that mixed engine families are optional Phase 9 work;
- older architecture/data-model wording that treats `FmtSongMeta.mode` as the future project-wide playback eligibility gate.

D047 does **not** supersede or invalidate:

- D005 tracker dimensions;
- D018/D019 current song/step layout;
- D020-D036 tracker/p-lock/effect/queue semantics;
- D037-D046 Phase 4 8-BIT engine implementation and verification contracts;
- D043 no-owned-cross-track-steal safety rule;
- D044 assigned 8-BIT parameter IDs;
- D046 production-freeze and Phase 4 exit requirements.

### D048 - WP5.0 mixed-engine routing contract

Normative document: `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`

Status: **ACCEPTED - frozen before WP5.0 production migration**

D048 freezes the smallest bounded implementation of D047:

- `FmtStep` remains exactly 8 B and continues to reference instruments, not engine families directly;
- `FmtInstrumentHeader` remains exactly 4 B and its existing `mode` byte is interpreted as engine-family identity;
- compatibility family values remain `0 = 8-BIT/chip` and `1 = FM/digital`;
- `FmtSongMeta.mode` remains physically present but becomes legacy/default-family metadata and initialization/import hint only, never a playback/publication eligibility gate;
- audio consumes a fixed 64 B aligned publication bank mirroring the 16 instrument headers rather than consulting mutable song metadata to choose a family;
- global primary routing owns exactly one family byte per logical track, 8 B total, while family-specific engines keep their own local voice handles/allocators;
- a cross-family NOTE hard-silences/clears the old primary before attempting the new family and commits the new route only on success;
- OFF/CUT release through the active family and clear global primary ownership in the same event, allowing only family-local bounded detached release tails;
- TIE/EMPTY/RETRIG never allocate or change family;
- family dispatch is explicit, static and bounded, with no runtime plugin registry or heap;
- the existing 2,048 B D044 row-page cache is generalized/reused as one shared engine-page publication cache rather than duplicated per family;
- non-applicable family-specific p-lock IDs remain stored but are ignored and never reinterpreted;
- WP5.0 adds no FM4 production DSP and proves mixed-family router semantics with a bounded test-only stub family;
- WP5.0 hard incremental ceilings are +4,096 B image, +256 B normal RAM and +0 B pool versus D046;
- complete D046 all-8-BIT behavior remains the regression baseline.

### Current interpretation

- FM1Tracker is one engine-agnostic tracker/sequencer.
- Each instrument owns engine family + engine identity.
- Tracks/rows reference instruments and may mix families once those families are implemented and accepted.
- Song-level `mode` is no longer an audio gate.
- The common router owns only family selection and cross-family primary ownership, not family-local voice allocation.
- Phase 5 begins with WP5.0 migration before FM4 is linked.
- Existing 8-BIT behavior remains the regression baseline and must not be weakened by the generalization.

## Precedence rule

For historical implementation/checkpoint questions, use the decision/contract that governed that checkpoint.

For future sound-engine architecture after Phase 4, D047 takes precedence over D004 and older project-wide-mode wording. D048 is the normative implementation contract for WP5.0 and takes precedence over any less-specific D047 implementation suggestion where both apply.
