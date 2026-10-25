# FM1Tracker Execution Protocol

This is the procedure for doing project work. It exists to prevent scope drift and lost context.

## Read order at the start of any session

1. `docs/PROJECT_CONTEXT.md` - what FM1Tracker is, its product direction and context that must survive work-package changes.
2. `docs/PROJECT_STATE.md` - what is active right now.
3. `docs/WORK_PACKAGES.md` - exact boundaries and exit criteria.
4. `docs/DECISIONS.md` - constraints that must not be silently overturned.
5. Active phase/architecture docs referenced by the work package.
6. `THIRD_PARTY.md` before importing any external code or assets.
7. Current GitHub issue/PR for the active work package.

Do not begin by re-planning the whole project unless the active package reveals a real architectural conflict.

## Conversation-to-repository context sync

Git is the durable project memory. The roadmap controls execution order, but it is not sufficient context by itself.

Substantive user decisions or clarifications from the current conversation must not remain chat-only. When a message changes or reaffirms product scope, architecture, UX intent, engine-family direction, constraints, priorities, testing expectations, resource assumptions or the meaning of future work, capture that context in the appropriate repository documentation at the same checkpoint or the next real repository checkpoint.

Do not create noisy commits for every casual sentence. Capture decisions when they materially affect how a future session should understand or continue the project.

At each real checkpoint, ask whether the repository now answers all of these without chat history:
- what are we building and why;
- what product directions remain intended even if they are not the active work package;
- what is implemented and verified;
- what decisions and constraints are locked;
- what is uncertain, deferred or hardware-blocked;
- what is the exact next action.

If not, update the relevant context/state/decision/test documents before moving on.

## Work-package lifecycle

### 1. Start

Before code changes:
- set the package to `IN PROGRESS` in `PROJECT_STATE.md`;
- state the exact objective and non-goals;
- identify files expected to change;
- identify required verification;
- record any new architecture decision before it becomes implicit code.

### 2. Implement

Prefer small commits that each leave the tree understandable.

Rules:
- platform changes and feature changes should not be mixed without necessity;
- generated files must have a reproducible source/process;
- no hidden dependency on local files that are not documented;
- no third-party source copied first and licensed later;
- no unbounded work in the real-time path;
- no "temporary" format that silently becomes persisted project data.

### 3. Verify

Run the checks relevant to the package and record results.

Verification categories:
- host functional;
- host automated/unit;
- build/safety checks;
- memory/code budget;
- audio timing;
- physical FM-1 hardware;
- license/provenance.

A package may be marked blocked when a physical device or external toolchain is unavailable, but the exact unverified criterion must remain visible.

### 4. Close

Before closing a package:
- all exit criteria pass or an explicit waiver/deferral is recorded in `DECISIONS.md`;
- `PROJECT_STATE.md` is updated;
- `TEST_MATRIX.md` is updated if coverage changed;
- product/context documents still describe the intended instrument, including future engine families and other non-active directions that must survive the handoff;
- docs match actual behavior;
- issue is closed with verification evidence/notes;
- next package becomes the only active package.

## Branch/PR policy

Recommended structure:

```text
main
  phase1/dinghy-baseline
  phase2/tracker-core
  ...
```

Within a phase branch, use bounded commits aligned to work packages. If a package becomes risky or large, use a dedicated sub-branch/PR rather than letting the phase branch become opaque.

Never merge a phase only because it "mostly works". The phase exit criteria are the merge gate.

## Commit philosophy

Good commits answer one question:
- provenance frozen;
- baseline files imported;
- identity renamed;
- transport clock added;
- REST semantics tested;
- etc.

Avoid commits that combine unrelated architecture, UI, DSP and documentation changes.

## Definition of evidence

Claims should be backed by one or more of:
- passing host test;
- captured build output;
- deterministic test vector;
- measured memory/timing number;
- physical hardware observation;
- exact upstream commit/license reference.

"Seems fine" is not an exit criterion.

## When a design changes

Do not simply edit old text to make history look consistent.

1. Add a new decision in `DECISIONS.md` that supersedes the old one.
2. Update affected architecture/control/data docs.
3. Update `PROJECT_CONTEXT.md` when the change affects durable product direction.
4. Update `PROJECT_STATE.md`.
5. Add migration implications if persisted data or user workflow changes.

## New chat / handoff rule

A new session should be able to continue using repository state alone.

Read `PROJECT_CONTEXT.md` before reducing the project to the currently active roadmap item. An inactive future direction is not abandoned merely because it is not the current work package.

If chat history and repository state disagree, stop and resolve the discrepancy against the user's latest explicit decision. Write the resolved state back into the repository before further implementation so that Git becomes authoritative again.
