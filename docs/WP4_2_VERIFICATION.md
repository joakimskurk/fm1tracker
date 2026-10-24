# WP4.2 Verification — D038 PULSE Oscillator

Date: 2026-10-24  
Branch: `phase4/8bit-engine`  
Package: WP4.2 / issue #32  
Result: **PASS — isolated PULSE primitive complete; full app unchanged**

## Scope

WP4.2 introduces the first Phase 4 oscillator primitive while deliberately leaving the running firmware on the frozen Phase 3/WP4.1 proof sine.

Normative contract:

- `docs/WP4_2_PULSE_CONTRACT.md`
- D038 in `docs/DECISIONS.md`
- initial contract commit `247ff3c03cdf3cd8e0878fe37d79693b26afc398`
- D038 decision/status freeze `6bf5324ec09266064fbb75828bffb5e773d7ad14`
- private MIDI-128 clarification commit `978f550c90401061555315772e0c948ad80aa00b`

Pre-source clean checkpoint:

`da2ad7b54b8f55b9219c5e359e717aa881921877`

Final closeout workflow source checkpoint:

`fe00532ee40d5d70a90344e9d9c3aac65cecdc42`

## Production source added

Exactly four isolated files were added:

- `firmware/src/fmt_engine_pitch.h`
- `firmware/src/fmt_engine_pitch.c`
- `firmware/src/fmt_osc_pulse.h`
- `firmware/src/fmt_osc_pulse.c`

The closeout gate emitted:

`WP4_2_ISOLATED_PRODUCTION_DELTA_OK=1 files=4 linked_delta=0`

`firmware/src/dinghy.c` remained byte-identical to the pre-source checkpoint. The new modules are therefore production-quality source but are not yet linked into the full app.

## Focused deterministic behavior

Authoritative focused workflow:

- workflow: `WP4.2 PULSE verification`
- run: `40088983995`
- host job: `120857585143` — SUCCESS
- PI32v2 job: `120857585035` — SUCCESS

Focused host proof:

- `FmtPulseOsc == 4 B`;
- legal pulse width `32..224`;
- exact 25/50/75% reference vectors;
- sample-before-phase-advance ordering;
- reset/retrigger phase zero;
- null safety;
- exact unsigned phase wrap;
- zero increment holds phase/output;
- guard words prove no mutation outside oscillator phase;
- exact D038 pitch vectors;
- strict monotonic phase increment over every legal Q7 input `0..16383`;
- malformed public pitch above `16383` clamps to legal maximum;
- deterministic replay from identical state/input.

Frozen long-run signature:

- samples: `65,536`
- pitch Q7: `8832` (A4)
- phase increment: `42,852,281`
- width: `96`
- final phase: `0xDFB90000`
- FNV-1a32: `0x8AB03EE1`

The runner emitted:

- `WP4_2_PULSE_TESTS_OK=1`
- `state_bytes=4 width=32..224 phase_order=sample_before_advance hard_edge=1`
- `pitch_q7=0..16383 strict_monotonic=1 max_inc=1293741834`
- `long_hash_fnv1a32=8ab03ee1 samples=65536 pitch_q7=8832 width=96 final_phase=dfb90000`
- `WP4_2_PULSE_FOCUSED_GATE_OK=1`

## Realtime/source audit

Static audit passed:

`WP4_2_STATIC_AUDIT_OK=1 pulse_loops=0 alloc=0 float=0 int64=0 store_traversal=0`

The PULSE render path contains no loop, allocation, song/lock traversal, float/double or 64-bit integer type. Width clamp, threshold compare, sample selection and phase addition are fixed bounded integer work.

WP4.2 intentionally remains hard-edge/band-unlimited under D038. No BLEP, oversampling or hidden smoothing/filter was introduced.

## PI32v2 target-compile evidence

Both isolated modules compile independently with the pinned real JieLi PI32v2 compiler.

Frozen object SHA-256:

- `fmt_engine_pitch.o`: `6203ee8b5ab68a1000066c25a52b674dbed9a61ff30eb7579bf29a07abe69f20`
- `fmt_osc_pulse.o`: `ed5ca2ea6dd5d15476fe4a3e6f42de25ab21ad3bf91f57ec654d31157b000144`

Focused target artifact from run `40088983995`:

- artifact ID `12557772955`
- artifact ZIP SHA-256 `b3c74cb236587f91b334e28da62b48c153ad0bb423f8715978f718781046f92b`

## Final full-app closeout

Authoritative closeout:

- workflow: `WP4.2 PULSE closeout verification`
- run: `40089122159`
- job: `120857992257`
- result: **SUCCESS**
- workflow head: `fe00532ee40d5d70a90344e9d9c3aac65cecdc42`
- evidence artifact: `wp4-2-pulse-closeout-evidence`
- artifact ID: `12558342138`
- artifact size: `1,120,173` B
- artifact ZIP SHA-256: `30b11776fef77fac2071019d73e0e20ff3c5ebdaf2937aab7f5c357016055b14`

The first closeout run `40089086522` is not accepted: the PULSE tests passed, but its workflow wrapper lacked the pre-created `build/` directory and Pillow required by the inherited host runner. Commit `fe00532e...` fixes only those workflow dependencies; the entire closeout was then rerun from the beginning and passed.

## Full-app identity remains frozen

The host proof remained exactly WP4.1:

- frames: `286,464`
- WAV SHA-256: `453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

The pinned JieLi/AC79 device build remained exactly WP4.1:

- image: **66,960 B / 581,564 B**
- normal RAM: **13,296 B / 98,304 B**
- pool RAM: **113,000 B / 344,064 B**
- image delta vs WP4.1: **0 B**
- normal RAM delta vs WP4.1: **0 B**
- pool RAM delta vs WP4.1: **0 B**
- package identity: `FM-1_987`

Frozen full-app artifacts remain:

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`

The closeout emitted:

- `WP4_2_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 full_app_delta=0`
- `WP4_2_D038_EXIT_OK=1 pulse=isolated deterministic=1 pi32v2=1 full_app_identical=1 physical_hardware_pending=1`
- `WP4_2_CLOSEOUT_OK=1`

## Hardware limitation

PULSE is not yet wired into the running firmware, so physical FM-1 sound-quality verification is not a WP4.2 exit requirement. Physical install/audio/execution-margin gates remain open for the later integrated engine/hardware packages.

## Exit

WP4.2 / D038 is complete at the isolated host/PI32v2/device-baseline level.

The next package is WP4.3 / NOISE. It must freeze its own deterministic noise-state, seeding, clock/pitch relationship and output contract before production NOISE source is written. The Phase 3 proof sine remains the full-app production audio path until a later explicit integration package.
