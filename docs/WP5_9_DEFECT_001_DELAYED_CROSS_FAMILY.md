# WP5.9 Defect 001 - delayed FM -> 8-BIT NOTE silences early

Date: 2026-10-25  
Issue: #52  
Detected by: first real WP5.9 mixed-engine validation run

## Evidence

Failing validation checkpoint:

```text
head                     3341ae60882a3781933c6742031933c7205a4f5a
run                      40205093456 FAILURE (expected rejection after defect discovery)
job                      121204217383 FAILURE
artifact                 wp5-9-mixed-host-evidence / 12603372062
artifact SHA-256         1cbeb9970a971586bcb1348594ea9a9e78bc6d16ea11938723b743f448903b6e
```

The same run proved these earlier assertions before reaching the defect:

```text
WP5_9_PLOCK_FAMILY_SAFE_OK=1 fm_lock=audible bit8_lock=audible wrong8_on_fm=ignored wrongfm_on_bit8=ignored
WP5_9_SIMULTANEOUS_AUDIO_OK=1 frames=52736 overlap_both_nonzero=77998 oracle=saturated_sum repeats=byte_identical sha256=f6a966b68df8a45c8e2ab142b09142d7dd6f91eb631b99fe8ad0efe2f1555317
WP5_9_SIMULTANEOUS_OWNERSHIP_OK=1 track0=fm track1=8bit dual_owner=0 active_cross_track_steals=0
WP5_9_REAL_SWITCH_OK=1 sequence=fm_to_8bit_to_fm tie=route_stable empty=route_stable off=route_clear unrelated_track=stable dual_owner=0
WP5_9_RETRIG_INTEGRATED_OK=1 family=fm route=stable audio_changed=1 deterministic=1
```

So the first real simultaneous-family audio oracle passed exactly. The defect is narrower than general mixed-family operation.

## Reproduction

Real engines, track 0:

```text
row 0: FM4 NOTE
row 1: 8-BIT NOTE + NOTE_DELAY 3
row 2: FM4 NOTE + NOTE_CUT 3
```

Track 1 holds an unrelated 8-BIT primary throughout.

Observed route/ownership:

```text
10 ms   t0 = FM4                         correct
140 ms  t0 = NONE                        WRONG
200 ms  t0 = 8-BIT                       correct after delayed tick
270 ms  t0 = FM4                         correct
340 ms  t0 = NONE after NOTE CUT         correct
```

At 140 ms row 1 has started, but NOTE_DELAY 3 has not fired. D048 requires the old family to remain active until the delayed NOTE goes through the normal NOTE router at its scheduled tick.

## Root cause

`tracker_apply_event()` correctly performs the pre-NOTE update against the previously owned FM patch. It then snapshots the incoming row:

```c
tracker_fm_row_patch_valid[track] =
    (uint8_t)tracker_fm_snapshot_row(track, event->step, instrument);
```

For the pending 8-BIT instrument, `tracker_fm_snapshot_row()` correctly returns false because the new row is not FM. That overwrites the track's existing FM snapshot validity with 0 even though the old FM primary is still intentionally routed during NOTE_DELAY.

`tracker_apply_event()` and subsequent pre-delay ticks call `tracker_apply_effect_pitch()`. Because the global route is still FM while `tracker_fm_row_patch_valid[track] == 0`, that function calls `tracker_hard_silence_route(track)`.

The old FM primary is therefore killed at row start/early tick instead of surviving until delayed NOTE dispatch.

## Violated inherited invariant

D048 requires delayed NOTE to use the same normal NOTE route when the delayed tick occurs. Before that tick, the currently owned family remains the active primary.

The WP5.0 unsupported-family delayed fixture already froze this timing principle for 8-BIT. WP5.9 is the first fixture to exercise it with a real FM old family and the D056 FM row-patch validity seam.

## Narrow fix requirement

The fix must preserve the currently owned FM patch while a delayed cross-family NOTE is pending and the incoming row does not produce a valid FM snapshot.

It must **not**:

- add a second per-track FM patch bank;
- add target RAM;
- traverse sparse lock storage in audio;
- alter immediate NOTE cross-family ordering;
- alter successful delayed 8-BIT -> FM behavior;
- alter TIE/EMPTY/RETRIG/OFF/CUT semantics;
- alter unrelated tracks;
- alter all-8BIT output.

A valid solution may keep the previous FM snapshot valid until the delayed NOTE dispatch when the current route is FM and the incoming NOTE is delayed/non-FM. The exact implementation is frozen separately before code changes.

## Required post-fix evidence

At minimum:

- FM -> delayed 8-BIT: old FM still owned before tick, 8-BIT owned after tick;
- 8-BIT -> delayed FM: old 8-BIT still owned before tick, FM owned after tick;
- unrelated track ownership unchanged;
- no dual ownership;
- delayed malformed/unsupported replacement follows D048 fail-to-silence at the scheduled tick, not earlier;
- simultaneous-family saturated-sum oracle remains exact;
- p-lock family safety remains exact;
- inherited D056 closeout remains green;
- all-8BIT WAV remains exact;
- no new target RAM object;
- target resource deltas are measured.

`hardware_margin_claim=none`.
