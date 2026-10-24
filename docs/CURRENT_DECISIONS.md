# FM1Tracker Current Decision Index

Date: 2026-10-24

`DECISIONS.md` remains the append-only historical decision log through D046. Historical entries are not rewritten when a later decision supersedes a constraint.

D047 and D048 are the accepted current Phase 5 architecture decisions. D048 is now implemented and verified by WP5.0. D049 has **not** been frozen yet and must be written during WP5.1 contract-first work before production FM operator code.

## D047 - mixed-engine instrument architecture

Normative document: `D047_MIXED_ENGINE_ARCHITECTURE.md`

Status: **ACCEPTED**

D047 supersedes:

- D004's future restriction that a project uses one project-wide 8-BIT or 16-BIT sound mode;
- older statements that mixed engine families are Phase 9-only architecture;
- older future wording that treated `FmtSongMeta.mode` as playback eligibility.

D047 does not invalidate D005, D018-D046 or any historical Phase 1-4 verification. The complete D046 all-8-BIT family remains a regression baseline.

## D048 - mixed-engine routing implementation contract

Normative document: `WP5_0_MIXED_ENGINE_ROUTING_CONTRACT.md`

Status: **ACCEPTED, IMPLEMENTED, VERIFIED**

Verification: `WP5_0_VERIFICATION.md`, run `40129822006`, job `120978955451`.

D048 freezes:

- `FmtStep` remains exactly 8 B and references instruments, not engine families directly;
- `FmtInstrumentHeader` remains exactly 4 B, with `mode` interpreted as family identity;
- compatibility family values remain `0 = 8-BIT/chip`, `1 = FM/digital`;
- `FmtSongMeta.mode` remains physically present but is compatibility/default-family metadata, never a playback/publication eligibility gate;
- audio receives family/engine identity through a fixed aligned 64 B publication bank;
- global primary routing stores exactly one family byte per logical track, 8 B total;
- family-local engines retain local voice handles/allocators;
- cross-family NOTE hard-silences and clears the old primary before attempting the new family, and commits only on successful dispatch;
- OFF/CUT route through the active family and clear global primary ownership;
- TIE/EMPTY/RETRIG do not allocate or change family;
- family dispatch is explicit, static and bounded, with no runtime plugin registry or heap;
- one existing 2,048 B row-page publication cache remains shared;
- non-applicable family-specific p-lock IDs remain stored but are ignored and never reinterpreted;
- WP5.0 contains no FM4 production DSP;
- complete D046 8-BIT behavior remains regression-equivalent.

Final D048 resource cost versus D046: +288 B image / +72 B normal RAM / +0 B pool.

## D049 - WP5.1 FM operator primitive

Status: **NOT YET FROZEN**

Issue: #44.

D049 must be written before production FM operator source is accepted. It must freeze at minimum:

- operator state layout and size;
- phase accumulator format and wrap behavior;
- deterministic integer carrier lookup representation;
- signed modulation input format/range;
- operator level and output format/range;
- overflow/saturation behavior;
- reset/init semantics;
- realtime arithmetic restrictions;
- deterministic host vector set;
- target compile and resource ceilings for WP5.1.

D049 must not silently decide algorithm routing, ratio UX, feedback, envelopes, voice allocation, parameter pages or mixed-engine production integration. Those remain WP5.2-WP5.9 work.

## Current interpretation

- FM1Tracker is one engine-agnostic tracker/sequencer.
- Each instrument owns engine family + engine identity.
- Tracks/rows reference instruments.
- Song-level `mode` is not an audio gate.
- The common router owns family selection and cross-family primary ownership, not family-local polyphony.
- The complete 8-BIT family remains the sound/regression baseline while FM4 is added package by package.
- No physical timing-margin claim is permitted without physical FM-1 evidence.

## Precedence rule

For historical checkpoints, use the decision/contract that governed that checkpoint.

For future sound-engine architecture after Phase 4, D047 supersedes D004 and older project-wide-mode wording. D048 is authoritative for common mixed-engine routing. Once frozen, D049 will be authoritative only for the FM operator primitive and must not override D047/D048 routing semantics.
