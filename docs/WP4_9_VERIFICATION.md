# WP4.9 Verification — linked 8-BIT engine integration

Status: **COMPLETE**

Normative contract: `docs/WP4_9_LINKED_ENGINE_CONTRACT.md` (D045)

Final verified checkpoint:

`89fabc51348078101c8e376bba7814af9d42ae80`

Final GitHub Actions:

- run `40116897104`
- job `120939922897` (`wp4-9-closeout`)
- result **SUCCESS**

Evidence artifact:

- ID `12570465704`
- size `774782` B
- ZIP SHA-256 `ed88b9533006e700bfb2f30021a26b99ba77a25b90079a6ff6ad588d0d294b0c`

## What WP4.9 changed

WP4.9 deliberately crosses the Phase 4 isolation boundary and replaces the Phase 3 proof-sine tracker lane with the linked D037-D044 8-BIT engine stack while preserving the separate D034 manual/audition sine lane.

The linked tracker path now provides:

- eight logical tracker tracks feeding one fixed eight-slot Phase 4 engine;
- PULSE, NOISE, TRIANGLE and WAVETABLE-8 through the real tracker adapter;
- deterministic D043 voice ownership / release-tail behavior;
- track-aware D031 and D032 publication while preserving the legacy track-0 APIs;
- D044 registry IDs `0x20..0x25`, 96 B instrument base bank and 2048 B aligned row cache;
- page-aware selected-track p-lock editing;
- D031 gain, D032 pitch/effects, D033 retrig/delay/cut and D044 page precedence through the linked path;
- fixed eight-slot render with no mutable song/lock traversal from the audio ISR.

The production migration checkpoint used by the final closeout is:

`c93c39ad344760b4da0dfbc76829ca5fef886174`

The final closeout additionally freezes all Gate 5/6 verification and inherited non-obsolete regressions without changing the production audio implementation.

## Gate 5 — canonical linked host matrix

Marker:

`WP4_9_GATE5_CANONICAL_OK=1`

Canonical engine / integration WAV SHA-256 values:

- PULSE: `d2661eccaf3f55dd31730339b107aa56c136d3dcc43bca2520de160fc99b60a4`
- NOISE: `ae55ce5607e8a0e4a0c0c04b60d2927d4fdc052ef43b186e7b7725a03548e1e0`
- TRIANGLE: `a338149a5a98bd108b8c72a80eb149029e604a6067cda592e1139a13114f7748`
- WAVETABLE-8: `4adf4c4fd64ca9dab4f2ae0562f8b70830924b0d7dabad8f14215de4f69897dd`
- mixed-engine fixture: `124ccc7e1ad0126cab8b4b51105596c88315b581339add1cdcf8fa978d53713b`
- eight-track ownership fixture: `f87d6928e1648e5dfbbd157e79dd3c23ca422fb83829cf8ba9adb56348609bf4`

D033 timing through the linked engine remains exact:

- NOTE DELAY 3 first nonzero frame: `2757`
- RETRIG 2 first differing frame: `1838`
- NOTE CUT 3 first differing frame: `2757`

Representative D031/D032/D044 precedence evidence:

- slide tick 0/1 pitch: `7712 / 7720`, gain `4128`, page `48ff00c8`
- portamento row/tick 1 pitch: `7712 / 7728`
- vibrato tick 0/1 pitch: `7712 / 7727`
- articulation page snapshot: NOTE `48782060`, TIE `4832c860`
- RELEASE row behavior: slow release remains stage `4`, fast release reaches stage `0`
- WAVETABLE TABLE lock page: `48ff00e0`
- unrelated-lock page: `48ff0060`

The default full-app linked 8-BIT audio reference is now intentionally different from the historical proof-sine baseline:

- WAV SHA-256 `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`
- frames `286464`

## Gate 6 — inherited regressions and realtime audit

Markers:

- `WP4_9_GATE6_INHERITED_REGRESSIONS_OK=1 obsolete_proof_sine_hashes_excluded=1`
- `WP4_9_GATE6_REALTIME_AUDIT_OK=1`

The closeout proves:

- no runtime allocator in the linked tracker audio path;
- audio ownership remains bounded and contains no editor/store traversal;
- D034 queue work remains bounded to capacity 32 with O(1) overflow discard;
- linked engine render scans exactly eight slots per frame;
- no spin/retry loop or IRQ masking was introduced for SPSC publication;
- screen/presentation work remains main-loop-owned;
- Phase 4 realtime code contains no floating point and no 64-bit runtime types;
- inherited model/store/transport/playback/editor/UI/worst-case/timing gates remain green where they are still semantically applicable.

Canonical UI hashes after the linked-audio migration:

- lock editor 1000 ms: `c9972b1edac7a0c96203ce6a7b4b668c5a8aafdc21c0c45e9f6cec61d1a4feda`
- lock editor 3000 ms: `16641a96750ecdfa40aa897467643ccdea5cfc666a6a01b4e176663e953765ff`
- lock delete 1000 ms: `7daeebe59622467fc49bbecf6607db24649996e90c2e531bab06d2ea0644c150`
- lock delete 3000 ms: `d66a634d1e8a185c22ddb1605b05d1a162e89f71b85b1e0813bffb2cc17d14cf`
- visual 1000 ms: `dab7d3f60b23289dd0fc39a8071d13c301b2108164b96fc54681a8b1558b8441`
- visual 3000 ms: `ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`
- HOME return remains pixel-identical.

## Pinned target build

Pinned inputs:

- JieLi toolchain: `jieli-linux-toolchains-20260730.1`
- AC79 SDK commit: `d179b4484759423312073f5fbb232501aa491047`
- package identity: `FM-1_987`

Final resources:

| Resource | Used | D037 ceiling | Remaining | Delta vs WP4.8 |
|---|---:|---:|---:|---:|
| app image | 72,504 B | 132,496 B | 59,992 B | +5,544 B |
| normal RAM | 19,604 B | 21,488 B | 1,884 B | +6,308 B |
| pool RAM | 113,000 B | 129,384 B | 16,384 B | +0 B |

Final firmware artifacts:

- `fm1tracker.bin` SHA-256 `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`
- `fm1tracker-0.1.fwsc` SHA-256 `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`

Closeout marker:

`WP4_9_CLOSEOUT_OK=1 gates=5,6 host=linked_8bit target=pinned d037_ceiling=pass hardware_margin_claim=none`

## Exit conclusion

WP4.9 is complete. The linked eight-track 8-BIT tracker engine is now the real production tracker audio lane and satisfies D045 on host and pinned target builds.

No physical FM-1 execution-margin claim is made because hardware is not yet available.

The only remaining Phase 4 package is WP4.10: representative worst-case timing / resource-margin benchmark and final Phase 4 exit gate.
