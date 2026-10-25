# WP5.9 Defect 001 Fix Contract

Date frozen: 2026-10-25  
Parent contract: `WP5_9_MIXED_VALIDATION_CONTRACT.md`  
Defect evidence: `WP5_9_DEFECT_001_DELAYED_CROSS_FAMILY.md`

## Status

**NARROW PRODUCTION-FIX AMENDMENT.**

This amendment does not introduce D057 or change D048-D056 architecture. It authorizes only the minimum correction required to restore the already-frozen D048 delayed-NOTE timing invariant at the D056 FM row-snapshot seam.

It supersedes C01 and C09 of the original WP5.9 validation contract only where explicitly stated below. Every other WP5.9 contract requirement remains normative.

## F01 - exact defect

When track 0 owns an FM primary and an incoming NOTE is delayed to a non-FM instrument, the row-boundary D056 snapshot attempt returns invalid for FM and immediately overwrites `tracker_fm_row_patch_valid[track]` with zero.

The global route intentionally remains FM until the delayed tick, so the next `tracker_apply_effect_pitch()` sees `FM route + invalid FM patch` and hard-silences the route early.

Observed first-run evidence:

```text
10 ms   FM route/owner
140 ms  NONE                 <- defect, NOTE_DELAY 3 has not fired
200 ms  8-BIT route/owner    <- delayed NOTE has fired
```

## F02 - authorized production boundary

Exactly one production source file may change relative to accepted D056 checkpoint `849c8b27d8c1afa09f9c093b695985c64e0c6013`:

```text
firmware/src/dinghy.c
```

These remain byte-for-byte frozen:

```text
firmware/src/fmt_engine_router.*
firmware/src/fmt_8bit_engine.*
firmware/src/fmt_fm_engine.*
firmware/src/fmt_fm_linked_params.*
firmware/src/fmt_fm_params.*
firmware/src/fmt_tracker_event.*
firmware/gen/**
tools/**
```

No new target source file, runtime object, persisted field or cache is authorized.

## F03 - semantic fix

The currently owned FM row patch must remain usable while a delayed NOTE is pending if:

- the current global route is FM;
- the incoming event is a NOTE suppressed by NOTE_DELAY for a later tick;
- the incoming instrument does not produce a valid FM snapshot.

The existing patch bytes are already left untouched when `tracker_fm_snapshot_row()` returns false. The fix therefore must preserve the previous **validity state** for this delayed-current-FM case instead of allocating or copying another patch.

For all other cases, existing D056 behavior remains:

- valid FM snapshot replaces current patch and sets valid;
- immediate non-FM NOTE may invalidate the FM row patch because replacement happens now;
- malformed non-delayed state fails safe;
- TIE/EMPTY/OFF/CUT retain current row-snapshot semantics;
- delayed NOTE dispatch still goes through `tracker_note_route()` at the scheduled tick.

## F04 - no additional state

The fix must add:

```text
0 B target normal RAM
0 B target pool RAM
0 full FM patch copies
0 sparse-store audio traversal
```

No second FM patch bank is permitted.

## F05 - bidirectional delayed-family verification

Post-fix validation must prove both successful directions with real engines:

```text
FM4 -> delayed 8-BIT
8-BIT -> delayed FM4
```

For each direction:

- old route/owner remains active before scheduled delay tick;
- new route/owner is committed after scheduled delay tick;
- old and new families never simultaneously own the switched track;
- unrelated active track remains unchanged.

A delayed malformed/unsupported replacement must keep the old family until the scheduled tick and then fail to route NONE/silence, matching normal D048 NOTE failure timing.

## F06 - inherited validation remains mandatory

After the fix all already-passing first-run evidence must remain green:

- exact simultaneous saturated-sum oracle;
- simultaneous real ownership;
- no dual ownership / active cross-track steal;
- real FM -> 8-BIT -> FM same-track switch;
- TIE/EMPTY route stability;
- integrated RETRIG audio change;
- OFF/CUT semantics;
- p-lock family safety;
- D056 inherited closeout;
- exact all-8BIT WAV.

## F07 - amended target gate

Because `dinghy.c` changes, exact D056 BIN/FW hashes are no longer required after the fix. They must be replaced by new exact hashes at closeout.

Frozen resource limits for this narrow fix:

```text
image       <= 80828 B   (D056 actual + 1024 B maximum)
normal RAM   = 27792 B   (no growth authorized)
pool         = 113000 B  (no growth authorized)
```

The final image delta and exact BIN/FWSC hashes must be recorded. The D056 93,432 B image package ceiling remains a looser inherited ceiling; this amendment intentionally imposes the tighter 80,828 B fix ceiling.

No physical margin claim is created by these numbers.

## F08 - fix authorization gate

`firmware/src/dinghy.c` may be edited only after CI proves the pre-fix source still contains the exact defect seam and this amendment is present:

```text
WP5_9_DEFECT_001_FREEZE_OK=1 defect=delayed_fm_to_8bit_early_silence authorized_source=dinghy.c ram_growth=0 image_fix_ceiling=80828 hardware_margin_claim=none
```

`hardware_margin_claim=none`.
