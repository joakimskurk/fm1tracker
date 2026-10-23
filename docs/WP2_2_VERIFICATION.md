# WP2.2 Bounds-Safe Pattern Store Verification

Date: 2026-10-23

## Scope

WP2.2 turns the compile-time model from WP2.1 into one real editable V0.1 song backing store. The package is intentionally limited to runtime storage placement, deterministic reset/default initialization and bounds-safe song/pattern/step access.

It does **not** implement transport timing, playback/event dispatch, STEP REC, REST/TIE/NOTE-OFF editing, tracker UI, p-lock allocation/editing, synth engines or persistence.

WP2.2 start / completed WP2.1 branch head:

`11b8b9bd48e9b4b53ed8008c160b40297e1d15c4`

Final verified implementation/workflow commit:

`e180ac8cb5ba7a68fd3932883c988ad9ed67003b`

GitHub Actions run:

`39986704633`

Job:

`120527634095` (`pattern-store`)

Result: **SUCCESS**

## Implemented store

D019 freezes one runtime V0.1 `FmtSong` in the existing linker `.pool` section.

Implementation:

- `firmware/src/fmt_store.h` exposes a narrow store/reset/access API;
- `firmware/src/fmt_store.c` owns one `FmtSong fmt_store_storage` with `section(".pool")` and 4-byte alignment;
- `fmt_store_reset()` deterministically clears non-step fields, sets `default_pattern_length` to 64 and explicitly initializes all 8,192 steps with the WP2.1 empty/inherit/no-lock sentinels;
- store pattern/step access delegates to the bounds-safe WP2.1 model accessors;
- `firmware/src/dinghy.c` only includes the store module and performs one reset before entering the existing app loop.

Zero-filled memory alone is deliberately not treated as an empty tracker song because the frozen empty/inherit/no-lock representations use `0xFF` / `0xFFFF` sentinels.

## Host verification

The frozen WP2.1 model suite still passes:

`FMT_MODEL_TESTS_OK=1`

The WP2.2 store suite passes:

`FMT_STORE_TESTS_OK=1`

Verified by the store suite:

- all 16 patterns x 8 tracks x 64 steps = **8,192 steps** receive deterministic defaults;
- first and last valid pattern/track/step indexes resolve correctly;
- pattern `16`, track `8`, step `64` and `UINT32_MAX` variants are rejected safely;
- const and mutable access paths are bounded;
- step addresses map to the expected `FmtSong` members;
- editing one step does not alias neighboring steps/tracks/pattern endpoints;
- reset after mutation restores step sentinels and deterministic metadata/track/order/instrument defaults.

The existing host app/audio regression also passes. Host WAV SHA-256 remains:

`dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`

This is byte-for-byte identical to the Phase 1 and WP2.1 host baseline.

## Device store placement

The linked device ELF contains exactly one store symbol:

```text
01c20000 g       .pool        00010168 fmt_store_storage
```

Therefore:

- store address: `0x01C20000`;
- store size: `0x10168` = **65,896 B**;
- store end: `0x01C30168`;
- linker pool region: `0x01C20000` through `0x01C73FFF` (`0x01C74000` exclusive).

The store is therefore physically linked into `.pool`, beginning exactly at the pool origin, and does not consume ordinary `.data + .bss` RAM.

## Device build and resource delta

Full release build completed successfully with package identity `FM-1_987`.

Inherited Dinghy safety gates pass:

- `.ram_text`: 901 instructions, no calls;
- image/RAM/pool limits pass;
- direct register access remains HAL-only (`src/`, loader clean).

Measured resources:

| Resource | WP2.1 / Phase 1 baseline | WP2.2 | Delta |
|---|---:|---:|---:|
| app image | 55,088 B | **55,188 B** | **+100 B** |
| normal RAM (`.data + .bss`) | 12,368 B | **12,368 B** | **0 B** |
| pool RAM | 30,720 B | **96,616 B** | **+65,896 B** |

The pool delta exactly equals `sizeof(FmtSong)`, proving the large backing store did not leak into normal RAM.

Remaining physical pool after WP2.2:

`344,064 - 96,616 = 247,448 B`

D018's conservative future p-lock reservation remains **16,384 B**; WP2.2 does not allocate or freeze that lock implementation.

## Verified artifacts

- `fm1tracker.bin`: 55,188 B, SHA-256 `c952d8cfc0f8d9ae1b18750f40753663d0c14cdaa1d493e43c9502572af6d578`;
- `fm1tracker-0.1.fwsc`: SHA-256 `859892524248ee64e1ebb85c3808468f087665b39e603876b97b754e28550bd0`;
- decoded package identity: `FM-1_987`;
- host WAV SHA-256 `dac1695b8e69f26334d548005e6cae505cf39ace297dcf6a57338e70fc386b9b`.

Actions evidence artifact:

- name: `wp2-2-pattern-store-evidence`;
- artifact ID: `12516445200`;
- artifact ZIP size: 581,014 B;
- artifact ZIP SHA-256: `70b03fdf9d4548f6e3139c1ecbfb18414e0ef1c4d16cddbd9d0c4939f3e0114f`.

## Scope/regression audit

Relative to the WP2.2 start checkpoint, the package does not modify:

- `firmware/app.ld`;
- `firmware/src/platform.c`;
- `firmware/src/fmt_model.h`;
- `firmware/hal/`;
- `firmware/loader/`;
- core build/package/install tooling.

The existing `dinghy.c` change is intentionally minimal: include the FM1Tracker-owned store implementation and initialize it once before the existing app loop. Audio synthesis and existing UI interaction remain unchanged.

The old WP2.1 push workflow was converted to a manual historical smoke gate after WP2.1 closed. Its earlier failure on the first legitimate WP2.2 `dinghy.c` change was a stale package-specific scope assertion, not a WP2.2 implementation failure. WP2.2 has its own full green verification run listed above.

## Hardware status

No physical FM-1 is currently available.

Therefore physical install, boot, panel/audio behavior and actual hardware timing remain **NOT TESTED**. WP2.2 does not claim hardware signoff.

## Exit decision

All WP2.2 exit criteria that are possible without physical hardware are satisfied:

- one real `FmtSong` store exists in `.pool`;
- all 8,192 steps initialize deterministically;
- bounds and mutation/reset behavior are host-tested;
- host audio regression remains exact;
- device build and safety checks pass;
- package identity is preserved;
- normal RAM delta is zero;
- pool delta exactly matches the store size;
- actual resource/artifact evidence is recorded.

WP2.2 may therefore close. WP2.3 is the next package and must start separately.
