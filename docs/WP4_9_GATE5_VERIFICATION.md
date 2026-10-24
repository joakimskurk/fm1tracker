# WP4.9 Gate 5 verification — linked 8-BIT host matrix

Status: **GREEN / CANONICAL**

Canonical workflow run: `40115886380`

Canonical artifact: `12568880243`

Artifact ZIP SHA-256: `bfef89568632fb41f2ad1032f4c25fce9eaedd2637191d50800fe64c53e3f727`

## Scope

This gate verifies D045 sections A–E through the real linked `dinghy.c` tracker lane after the proof-sine migration. The host harness mutates only public song/lock/base inputs and reads runtime state; no device-only debug hook was added.

## Frozen engine / mix / ownership WAV hashes

- PULSE: `d2661eccaf3f55dd31730339b107aa56c136d3dcc43bca2520de160fc99b60a4`
- NOISE: `ae55ce5607e8a0e4a0c0c04b60d2927d4fdc052ef43b186e7b7725a03548e1e0`
- TRIANGLE: `a338149a5a98bd108b8c72a80eb149029e604a6067cda592e1139a13114f7748`
- WAVETABLE-8: `4adf4c4fd64ca9dab4f2ae0562f8b70830924b0d7dabad8f14215de4f69897dd`
- four-engine mixed fixture: `124ccc7e1ad0126cab8b4b51105596c88315b581339add1cdcf8fa978d53713b`
- eight-track ownership fixture: `f87d6928e1648e5dfbbd157e79dd3c23ca422fb83829cf8ba9adb56348609bf4`

All four engine fixtures are non-silent and mutually distinct. The ownership fixture proves initial track `0..7` -> voice `0..7` allocation and preserves active cross-track ownership through release-tail/re-note activity.

## Frozen D033 timing through the linked engine

The linked 8-BIT path preserves the Phase 3 six-tick grid sample-for-sample:

- NOTE DELAY 3 first audible frame: `2757`
- RETRIG 2 first PCM difference from control: `1838`
- NOTE CUT 3 first PCM difference from control: `2757`

Corresponding canonical WAV hashes are frozen in `tests/run_wp4_9_linked_audio_host.sh`.

## D031 / D032 / D044 precedence evidence

Canonical state assertions include:

- FINE + SLIDE UP: tick 0 pitch `7712`, tick 1 pitch `7720`
- VOICE LEVEL 128 * TRACK LEVEL 64 -> gain `4128`
- PULSE WIDTH lock page: `48ff00c8`
- PORTAMENTO: row tick 0 pitch `7712`, tick 1 pitch `7728`
- VIBRATO: tick 0 pitch `7712`, tick 1 pitch `7727`
- NOTE envelope page `48782060`, later TIE page `4832c860`, while the envelope retains the NOTE-snapshotted decay/sustain values
- OFF-row RELEASE 220 leaves a detached RELEASE tail; RELEASE 0 is immediately IDLE
- WAVETABLE TABLE lock page `48ff00e0`
- unrelated FINE lock leaves the PULSE engine page at `48ff0060`

The permanent canonical gate is `.github/workflows/wp4_9_gate5_linked_host_verify.yml`; the fixture runner is `tests/run_wp4_9_linked_audio_host.sh`.

## Gate result

D045 verification sequence Gate 5 is complete. Gate 6 is next: inherited Phase 2/3 regressions that remain semantically valid after the intentional tracker-audio migration. Historical proof-sine hashes are not valid Gate 6 requirements; UI/editor/transport/store/queue and other non-obsolete behavior remain regression targets.
