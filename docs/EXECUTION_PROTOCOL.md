# FM1Tracker Execution Protocol

This is the procedure for doing project work. It exists to prevent scope drift and lost context.

## Read order at the start of any session

1. `docs/PROJECT_STATE.md` — what is active right now.
2. `docs/WORK_PACKAGES.md` — exact boundaries and exit criteria.
3. `docs/DECISIONS.md` — constraints that must not be silently overturned.
4. Active phase/architecture docs referenced by the work package.
5. `THIRD_PARTY.md` before importing any external code or assets.
6. Current GitHub issue/PR for the active work package.

Do not begin by re-planning the whole project unless the active package reveals a real architectural conflict.

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
3. Update `PROJECT_STATE.md`.
4. Add migration implications if persisted data or user workflow changes.

## New chat / handoff rule

A new session should be able to continue using repository state alone.

If chat history and repository state disagree, repository state wins unless the user explicitly chooses to change it. Any chosen change is then written back into the repository before further implementation.
