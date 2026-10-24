# FM1Tracker Current Decision Index

Date: 2026-10-24

`DECISIONS.md` remains the append-only historical decision log through D046. Historical entries are not rewritten when a later decision supersedes one of their constraints.

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

### Current interpretation

- FM1Tracker is one engine-agnostic tracker/sequencer.
- Each instrument owns engine family + engine identity.
- Tracks/rows reference instruments and may eventually mix families in one project.
- Existing `FmtInstrumentHeader.mode` values remain compatibility family identities for now.
- Existing `FmtSongMeta.mode` remains physically present during frozen Phase 4 but is no longer the target future project-wide gate.
- Phase 5 begins with WP5.0 mixed-engine routing contract + migration before FM4 is linked.
- Existing 8-BIT behavior remains the regression baseline and must not be weakened by the generalization.

## Precedence rule

For historical implementation/checkpoint questions, use the decision/contract that governed that checkpoint.

For future sound-engine architecture after Phase 4, D047 takes precedence over D004 and older project-wide-mode wording.
