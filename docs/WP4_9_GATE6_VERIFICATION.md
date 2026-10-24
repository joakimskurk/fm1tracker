# WP4.9 Gate 6 verification — inherited non-obsolete regressions

Status: **GREEN / CANONICAL**

Canonical workflow run: `40116581485`

Canonical artifact: `12569760220`

Artifact ZIP SHA-256: `51f8c05f6de640f35df89a02dc68a732a5591100756bf935f4d37bbf678272c8`

## Scope

Gate 6 re-runs inherited Phase 2/3 behavior that remains semantically valid after the intentional D045 proof-sine -> linked 8-BIT tracker-audio migration. Historical proof-sine full-app WAV hashes are intentionally excluded because Gate 5 is now the canonical linked-audio contract.

The permanent gate is `.github/workflows/wp4_9_gate6_inherited_verify.yml`; its runner is `tests/run_wp4_9_inherited_regressions.sh`.

## Component regressions

All of the following remain green:

- model/store bounds and 8 tracks × 64 steps × 16 patterns;
- transport, including the six-hour integer long-run proof;
- playback dispatch and event ABI;
- pattern input and stopped STEP REC;
- 62-byte bounded tracker screen state;
- D027 registry with 10 assigned IDs, including D044 `0x20..0x25`;
- D028 lock-store capacity / per-step limit;
- D029/D030 lock editor storage semantics;
- D031 playback-parameter snapshots;
- D032 effect snapshots/tick grid;
- D033 delayed/retrig/cut state machine;
- D034 32-entry bounded SPSC queue.

## Context and UI regressions

The original Phase 2/3 user-action scripts remain in use for performance/SEQ context, track selection and STEP REC. D044 intentionally changed the semantic 8-BIT footer/page presentation, so pre-D044 screenshot hashes are not reused as false requirements.

Instead, Gate 6 freezes the current D044/D045 UI output while retaining the old interaction scripts:

- lock editor 1000 ms: `c9972b1edac7a0c96203ce6a7b4b668c5a8aafdc21c0c45e9f6cec61d1a4feda`
- lock editor 3000 ms: `16641a96750ecdfa40aa897467643ccdea5cfc666a6a01b4e176663e953765ff`
- lock delete 1000 ms: `7daeebe59622467fc49bbecf6607db24649996e90c2e531bab06d2ea0644c150`
- lock delete 3000 ms: `d66a634d1e8a185c22ddb1605b05d1a162e89f71b85b1e0813bffb2cc17d14cf`
- D035/D044 visual feedback 1000 ms: `dab7d3f60b23289dd0fc39a8071d13c301b2108164b96fc54681a8b1558b8441`
- released visual feedback 3000 ms: `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`

The lock-editor and destructive-delete fixtures still emit zero audition MIDI, and HOME return remains pixel-identical to the clean HOME control.

## Worst-case / timing regressions

The inherited D036 component stress and timing matrix remain exact:

- lock pool: 2048 records / 512 owners;
- row caches: 64 rows × max 4 locks;
- queue stress: 2048 cycles with explicit reject-newest overflow;
- bounded screen convergence remains green;
- long timing matrix: 4096 rows / 64 wraps / 22,579,200 frames;
- D032/D033 exhaustive value matrices remain green;
- 120-BPM tick anchors remain `919,1838,2757,3675,4594`, next row `5513`.

## Linked realtime audit

The D045-aware static audit proves:

- no runtime heap allocation in the audited path;
- `app_audio()` contains no editor/store traversal, screen drawing or settings work;
- D033 action logic is contained in bounded per-track event/tick helpers called from fixed `FMT_TRACKS` loops;
- D034 queue consumption remains capped at 32 events with O(1) overflow discard;
- the linked engine render is exactly the fixed 8-slot loop and performs no mutable song/lock/cache traversal;
- no spin/mutex/IRQ-masking retry path is introduced;
- tracker presentation remains main-loop only;
- Phase 4 runtime primitives remain integer-only with no 64-bit runtime types or float/double.

## Gate result

D045 verification sequence Gate 6 is complete. Gate 7 is next: the final pinned JieLi/PI32v2 + AC79 full-build closeout, re-running canonical Gate 5 and Gate 6 and freezing the final image/RAM/pool plus BIN/FWSC/default linked-audio hashes.
