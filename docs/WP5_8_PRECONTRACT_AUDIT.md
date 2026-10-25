# WP5.8 pre-contract audit

Date: 2026-10-25  
Issue: #51  
Branch: `phase5/fm4-parameters`  
Parent: WP5.7/D055 closeout `80587f12786122dcec819260ddad5dd5db4b1005`

## Purpose

Freeze the smallest tracker-facing FM parameter architecture that can reuse the existing D027-D031 sparse p-lock model without changing D055 DSP/voice semantics or the exact all-8BIT regression baseline.

No production source may change during this audit/freeze checkpoint.

## Existing semantic-ID space

`FmtParamId` is one byte. Numeric ownership is already frozen:

```text
0x01..0x1F  COMMON
0x20..0x5F  8BIT
0x60..0x9F  16BIT
0xA0..0xBF  TRACK
0xC0..0xDF  CHANNEL FX
0xE0..0xEF  GLOBAL
```

Only ten IDs are currently assigned. 8BIT owns `0x20..0x25`; `0x26..0x5F` remains unassigned. The complete `0x60..0x9F` 16BIT namespace is reserved and currently unassigned. D048 maps `FMT_MODE_16BIT` to the FM engine family, so the 16BIT namespace is the correct existing semantic home for FM4 controls. No new namespace is required.

## Existing sparse p-lock contract

The D028/D031 storage model is already sufficient:

- 2,048 global sparse lock entries;
- maximum four locks per tracker step;
- one-byte semantic parameter ID + stored value;
- lock mutation/browse/delete happens in main/editor context;
- audio ISR never traverses `fmt_lock_store` and never scans row locks;
- row-local publication is prepared in main context;
- row-boundary audio consumes only bounded fixed-size snapshots.

The four-lock limit is intentionally retained. FM having more than four available controls does **not** imply more than four simultaneous locks on a step.

Stored lock values are wider than the normalized engine controls. D056 must reject values above 255 during engine-row resolution; it must never silently truncate malformed/corrupt values to `uint8_t`.

## Existing 8BIT linked-page contract

D045/WP4.9 owns:

```text
16 x Fmt8BitParamSet       96 B
8 x 64 x uint32 row cache 2048 B
```

The 2,048-byte cache and public source-level view `fmt_8bit_row_param_cache.word[track][row]` are asserted by historical focused tests. Current production reads the naturally aligned 32-bit word directly. That behavior and all existing 8BIT parameter IDs/pages must remain exact.

D048 additionally froze that future families must reuse/generalize the same physical engine-row publication storage instead of allocating another parallel 2,048-byte family cache.

### Consequence for WP5.8

Do not replace the historical 8BIT layout with an incompatible stride. Generalize the physical allocation around it:

```text
legacy / bank0-low  8 x 64 x 4 B = 2048 B
bank0-high          8 x 64 x 4 B = 2048 B
bank1-low           8 x 64 x 4 B = 2048 B
bank1-high          8 x 64 x 4 B = 2048 B
active bank         8 x 64 x 1 B =  512 B
------------------------------------------
single allocation                         8704 B
```

The first 2,048 bytes remain the historical 8BIT row-word view. 8BIT audio continues to consume that exact word and does not need a bank lookup. FM rows use two 32-bit words from one selected bank to publish four compact `{param_id,value}` overrides.

Publication rule:

1. main reads the current one-byte bank selector;
2. main writes both 32-bit words of the inactive FM bank;
3. main flips the one-byte selector only after the inactive payload is complete;
4. audio reads the selector once at row boundary and then reads only that immutable bank;
5. if audio interrupts main during publication, the old bank remains selected and complete.

This uses the existing main/ISR single-core ownership model. There is no spin, retry, IRQ masking or 64-bit atomic requirement.

Net row-cache growth relative to D055 is 6,656 B, not another per-family cache.

## D055 FM bridge state

`FmtFmPatch` is exactly 28 B. Twenty-seven bytes carry meaningful controls; the final byte is reserved and remains zero.

D055's immutable bridge patch is:

```text
ratio      1,5,1,5
level      255,96,255,96
attack     0,0,0,0
decay      0,0,0,0
sustain    255,255,255,255
release    96,96,96,96
algorithm  2
index      64
feedback   0
reserved   0
```

Exact patch-byte SHA-256:

```text
3d2ef849d345d12f79312af92950062db0dbfd5b56966c6f7a2d557251edac52
```

D055 snapshots this patch at tracker row-event boundaries. Per-sample render does not read editor/store state. WP5.8 must preserve that ownership boundary and feed a resolved local `FmtFmPatch` into the existing D055 interface.

## FM control domains already frozen by D049-D055

```text
algorithm    4 values, IDs 0..3
ratio       16 values, IDs 0..15
level       uint8 0..255
index       uint8 0..255
feedback    uint8 0..255
A/D/S/R     uint8 0..255 per operator
```

No new DSP range is needed.

## Normalized base-bank conclusion

The clean tracker-facing representation is one 27-byte normalized `FmtFmParamSet` per instrument:

```text
algorithm             1 B
index                 1 B
feedback              1 B
ratio[4]              4 B
level[4]              4 B
attack[4]             4 B
decay[4]              4 B
sustain[4]            4 B
release[4]            4 B
--------------------------------
                       27 B
```

Sixteen instruments require 432 B ordinary RAM.

Mappings into D055 are bounded and exact:

```text
algorithm = control >> 6
ratio_id  = control >> 4
all other controls pass through unchanged
reserved  = 0
```

D055-default normalized controls are therefore:

```text
algorithm 128
index      64
feedback    0
ratio      16,80,16,80
level      255,96,255,96
attack     0,0,0,0
decay      0,0,0,0
sustain    255,255,255,255
release    96,96,96,96
```

Composing those bytes must reproduce the exact D055 patch SHA above.

## Page conclusion

FM needs 27 semantic controls but only four physical knobs. Seven logical four-slot pages are sufficient. One CORE slot remains intentionally unassigned. The exact IDs/page map is frozen by D056.

The page selector is main/UI state only and is never read per sample. Final physical-control UX remains a Phase 7 hardware concern; WP5.8 may use a provisional bounded page-change gesture without changing semantic IDs or page composition.

## Editor conclusion

Reuse `fmt_lock_editor_apply_param_delta()`. Do not create another p-lock mutation algorithm.

For a held FM step:

- resolve the same note-only instrument inheritance semantics used by the linked engine path;
- map knob -> selected FM page -> semantic `FmtParamId`;
- use the resolved instrument base byte as the reference when no lock exists;
- store only 0..255 normalized values;
- refresh the affected common row publication plus the single generalized engine-row publication after mutation/delete.

CORE slot 4 is `FMT_PARAM_NONE` and must be a no-op.

## Source boundary conclusion

D049-D055 DSP/voice modules and generated tables remain frozen. WP5.8 production work is limited to parameter registry, linked page/cache/editor wiring, new FM parameter/link modules and `dinghy.c`. Exact files are frozen in the normative contract.

## Resource conclusion

D055 baseline:

```text
image       77048 B
normal RAM  20464 B
pool       113000 B
```

Known WP5.8 ordinary-RAM additions before code:

```text
generalized row cache net growth  6656 B
16 x 27-byte FM base bank           432 B
----------------------------------------
known net growth                   7088 B
```

D056 therefore reserves at most +8,192 B normal RAM and +16,384 B image over the D055 baseline. Pool RAM must remain exactly 113,000 B.

No physical FM-1 is available. `hardware_margin_claim=none`.
