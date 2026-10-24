# WP3.9 Verification — D035 Lock/Effect Visual Feedback

Date: 2026-10-24
Status: **COMPLETE — host/device-build verified; physical FM-1 readability remains later hardware work**

Normative contract: `docs/WP3_9_VISUAL_CONTRACT.md`

## Verified checkpoint

Final verified source/workflow checkpoint:

`354bf7d356774ad828f1f660bd456dbab7f0490d`

Final GitHub Actions:

- run `40083510351`
- job `120841687772` (`visual-feedback-closeout`)
- result **SUCCESS**

Evidence artifact:

- ID `12555799262`
- size `3,693,890` B
- ZIP SHA-256 `62bdffec5faa027484ccc8bf99b2babbb978874054182de97970b8d52f333bca`

Closeout markers:

- `WP3_9_PRODUCTION_SCOPE_OK=1 files=4 low_level_unchanged=1`
- `WP3_9_CLOSEOUT_OK=1`

## Production scope

D035 is presentation-only. Relative to the final WP3.8 checkpoint, the only production-source differences are:

1. `firmware/src/dinghy.c`
2. `firmware/src/fmt_lock_editor.c`
3. `firmware/src/fmt_tracker_screen.c`
4. `firmware/src/fmt_tracker_screen.h`

HAL, loader, platform, linker, build tooling, queue, effect scheduler, p-lock storage and audio DSP code remain unchanged.

The two tracker-screen synchronization call sites in `dinghy.c` now pass the existing `FmtLockEditor` as read-only presentation input. No audio/ISR path draws, mutates display state or waits for UI work.

## D035 visual contract proved

The existing D026 240×240 tracker surface remains unchanged structurally:

- exactly 10 redraw regions: header, footer and eight row strips;
- at most 2 tracker regions rendered/blitted per main-loop poll;
- exactly 8 cursor-owned visible rows;
- D026 `>` cursor and `*` playhead markers retain their independent meaning.

Row presentation is now:

`ST NOTE IN VOL LK FX`

P-lock summary field:

- `--` zero locks
- `L1..L4` stored lock count
- `L!` presentation/read error
- `P0..P4` active held-step p-lock target
- `P!` target with presentation/read error

Effect field displays the raw stored command/value without changing D032/D033 semantics:

- `----` NONE
- `ARhh` ARPEGGIO
- `SUhh` SLIDE UP
- `SDhh` SLIDE DOWN
- `POhh` PORTAMENTO
- `VIhh` VIBRATO
- `RThh` RETRIG
- `DLhh` NOTE DELAY
- `CThh` NOTE CUT
- `????` unsupported/reserved command

`hh` is the raw stored effect value in uppercase hexadecimal. Valid commands with semantically inactive values are still shown as stored truth rather than silently normalized.

TRACK PAN remains storage-only under D031. Detailed lock-editor feedback now appends `STORE` when TRACK PAN is the selected/edited parameter; audible VOICE LEVEL, FINE PITCH and TRACK LEVEL retain their previous detailed wording.

## Bounded presentation state

`FmtTrackerScreen` is exactly **62 B** ordinary RAM, up from the D026 44 B state while remaining under the original 64 B cap.

D035 adds:

- eight stored effect-value bytes;
- eight lock-count/error bytes;
- one held-target-active byte;
- one held-target-step byte.

Per synchronization, presentation inspects the same eight visible rows and performs at most one `fmt_lock_count()` per row. D028 caps each list at four records, so presentation performs at most 32 lock-record inspections per sync. It allocates no pool memory and retains no mutable lock-record pointers.

Focused proof:

- `FMT_TRACKER_SCREEN_TESTS_OK=1`
- `state_bytes=62 rows=8 regions=10 max_regions_per_poll=2 pattern=0`
- `WP3_9_VISUAL_TESTS_OK=1`
- `screen_bytes=62 rows=8 regions=10 max_regions_per_poll=2`
- `FMT_LOCK_EDITOR_TESTS_OK=1`

## Reviewed framebuffer evidence

A full-app host fixture populated the eight visible rows with representative D032/D033 commands and p-lock state.

Active held-step reference at 1000 ms:

- row 01 shows active p-lock target `P2`;
- detailed footer shows `K4 PAN 129 LOCK STORE`;
- all eight stored effect mnemonics are readable;
- no clipping or field overlap was observed.

Frozen active framebuffer PPM SHA-256:

`d290415387ac8e55006863819c2188d64f0d0453e204bb194cb8d004121484f3`

Released reference at 3000 ms:

- row 01 returns to normal stored summary `L2`;
- row 02 shows `L1`;
- stale held-step/footer feedback is absent;
- all eight effect fields remain readable.

Frozen released framebuffer PPM SHA-256:

`ec15c6db429fe26d453ada14cc28a1eab8c2aa93e83c6f0bbfff67112f1a86c6`

HOME cleanup remains pixel-identical to the equivalent control:

`WP3_9_HOME_RETURN_PIXEL_IDENTICAL=1`

Reviewed framebuffer gate:

`WP3_9_REVIEWED_SCREEN_HASHES_OK=1`

## Inherited behavior and audio regressions

All inherited runtime/audio behavior remained green.

Phase 2 full-app compatibility:

- `WP3_9_PHASE2_FULL_APP_COMPAT_OK=1 midi_note_pairs=4 home_identical=1`
- `WP3_9_D029_FULL_APP_COMPAT_OK=1 midi_events=0`
- `WP3_9_D030_FULL_APP_COMPAT_OK=1 midi_events=0`
- WP2.5–WP2.8 context tests remain green.

Historical default WAV remains byte-identical:

`453dc6b7a2f06a24b1a236ecd692b9beca59f8a070fdc954c4ea1c79c0e13440`

D031 locked WAV remains byte-identical:

`8d2e478e366c0af18b0631da268ba75cc358157d043c567bee2e734852d0e9a8`

D032 frozen audio fixtures remain byte-identical, including:

- control `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- ARP `5e95abbdc6a0a9a65af402b7393b9b4088e968297d239679cd78b3872595a743`
- SLIDE UP `2e205d5d46a6c604fbe6c5976dbf57f252dc4288de70ee69a66b7a6769d4e50e`
- SLIDE DOWN `9bc32956995fe8ada92c3bf2fde80d6b9f6e3a1cd32e06343a170ed70e1e3edc`
- PORTAMENTO `2d84235e03ca33d055baf2b2d0d3a430a1b28d3ff4f0faa913d5556f52f1f7e7`
- VIBRATO `e14673fca74449f31783a44168edbf7146b0a1aa73f492dd9460febb7542c4a4`
- ARP + FINE `5ab516bc70ca571e4dfa43330075e08c389d1fb5ae1a668bc14231f1a50fd950`

D033 delay/cut/retrig behavior and timing remain unchanged, including delay 3 and cut 3 at frame 2757 and retrig 2 first divergence at frame 1838.

D034 queue fixtures remain byte-identical:

- control / FIFO `8933b4ae192e314dbaa729b929ebfc0195178842ccd9e89306deffd71a9db1b6`
- hardware `1b13d55424dd16e36c1820501dcf6b3232a3e1d3bfb357259f74e85a45858b18`
- MIDI `f095db1bee4fa26c6ca56d298b30402b0a43a4ae700b31bf52854943151cfc71`
- overflow `31c55a429e53edf84fe5b8bc634b8fab42b45e5ad744e0b6a4505655e3b92a2f`
- recovery `42207d5ee151619f525d4416f3e77b79fe2d9f0746bb5f111c88858a2ca32b93`

## Target build and resource accounting

The D035 tracker-screen translation unit compiles independently with the pinned JieLi PI32v2 toolchain:

`WP3_9_JIELI_TRACKER_SCREEN_COMPILE_OK=1`

Pinned AC79 SDK commit:

`d179b4484759423312073f5fbb232501aa491047`

Full target build passed register-access hygiene:

`register access: hal/ only (src/, loader/ clean)`

Final WP3.9 resources:

| Resource | Used | Limit | Free | Delta vs WP3.8 |
|---|---:|---:|---:|---:|
| app image | 66,960 B | 581,564 B | 514,604 B | +652 B |
| normal RAM | 13,296 B | 98,304 B | 85,008 B | +16 B |
| pool RAM | 113,000 B | 344,064 B | 231,064 B | +0 B |

The presentation struct itself grows by 18 B from 44 B to 62 B; the linked ordinary-RAM delta is +16 B because the final link layout/alignment changed. The measured linked result, not the arithmetic struct delta, is authoritative for firmware budgeting.

## Final firmware artifacts

- `fm1tracker.bin` SHA-256 `b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27`
- `fm1tracker-0.1.fwsc` SHA-256 `7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c`
- package identity `FM-1_987`

## Scope preserved

WP3.9 added no:

- p-lock parameter or storage semantics;
- effect command or effect editing control;
- D034 queue behavior or overflow UI;
- audio/DSP/effect timing behavior;
- live REC/capture semantics;
- multi-track playback migration;
- synth engine;
- persistence;
- new screen/page/menu architecture;
- extra redraw regions or increased redraw-per-poll budget.

## Hardware limitation

No physical FM-1 is currently available. The screenshots establish deterministic 240×240 host geometry and visual intent, but do not claim final physical LCD readability, viewing angle, refresh perception, encoder/key ergonomics or measured AC791N UI/audio execution margin. Those remain later hardware gates.

## Conclusion

D035 satisfies WP3.9 at the host/device-build level. Lock/effect state is now legible on the existing bounded tracker surface, the presentation path remains main-loop-only and bounded, and every frozen audio/runtime behavior through WP3.8 remains unchanged.

The next and final Phase 3 package is **WP3.10 — Phase 3 worst-case tests**. It should begin with a frozen stress/exit test plan rather than new feature production code.
