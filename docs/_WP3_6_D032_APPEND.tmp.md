## D032 — WP3.6 tracker-effect command domain, six-tick D020 timing and persistent-pitch semantics

Date: 2026-10-24  
Status: accepted for the WP3.6 one-lane proof

**Decision:** the detailed immutable WP3.6 contract is `docs/WP3_6_EFFECT_CONTRACT.md`. Tracker effects remain a separate domain from D027 p-lock parameter IDs and use the existing `FmtStep.effect/effect_value` bytes. The first assigned commands are `0x00` NONE, `0x01` ARPEGGIO, `0x02` SLIDE_UP, `0x03` SLIDE_DOWN, `0x04` PORTAMENTO and `0x05` VIBRATO; all other command bytes resolve safely to NONE in WP3.6.

Exactly six effect ticks exist per D020 row. Tick 0 is D021 row dispatch and the five internal phase thresholds are `110250`, `220500`, `330750`, `441000` and `551250`; `661500` is the next row, never a synthetic tick 6. The existing D020 phase/residual remains authoritative, stopped transport freezes effect progress and maximum BPM can cross at most one effect-tick boundary per rendered frame.

SLIDE_UP/DOWN modify persistent Q7 pitch on ticks 1..5. PORTAMENTO moves persistent Q7 pitch toward a remembered coarse-note target on explicit PORTAMENTO rows. ARPEGGIO and VIBRATO are transient only. Final active pitch is `clamp(persistent_pitch_q7 + (D031_FINE - 128) + transient_effect_mod_q7, 0, 16383)`. NOTE/TIE/EMPTY/OFF, target clearing/resume, zero values, arpeggio encoding and the 64-phase vibrato law are exactly as specified in `docs/WP3_6_EFFECT_CONTRACT.md`; no hidden zero-value effect memory exists.

`FmtPlaybackEvent` remains exactly 4 B. Main/startup publishes one separate aligned 32-bit effect snapshot for each of the 64 proof rows, exactly 256 B ordinary RAM. Audio reads one word only at row dispatch and never rereads song effect bytes in the per-sample path. WP3.6 effect runtime state is fixed at no more than 16 B. No allocator, D028 traversal, variable event loop, screen/flash work, float or runtime 64-bit division enters the audio path.

With effect NONE, both the historical default WAV and the frozen D031 locked-audio WAV must remain byte-identical. New deterministic evidence must cover ARPEGGIO, both slide directions, PORTAMENTO target/resume/stop/reached behavior, VIBRATO, D031 FINE precedence, NOTE/TIE/EMPTY/OFF interaction, invalid-command fallback and stored-data non-mutation.

**Reason:** `FmtStep` already reserves exactly two tracker-effect bytes and D027 explicitly separates command identity from semantic p-lock parameters. Six exact D020-derived ticks provide tracker-style sub-row modulation without a second free-running clock or drift, while a second 64-word cache preserves the proven D031 main-to-audio ownership model.

**Consequence:** WP3.6 may add only the first effect-command set, one pure bounded effect cache/state module, one 256 B ordinary-RAM effect cache and minimal proof-voice integration. WP3.7 retrig/note-delay/note-cut and WP3.8 general queue work remain separate.
