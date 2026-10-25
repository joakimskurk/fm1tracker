# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md`, contract and test-plan documents.

Legend:

- ✅ verified/frozen
- ⬜ active/future requirement
- ⛔ physical hardware unavailable

## Permanent invariants

| Check | Status |
|---|---|
| package identity `FM-1_987` | ✅ |
| pinned JieLi/AC79 build path | ✅ |
| 8 logical tracks x 64 steps x 16 patterns | ✅ |
| `FmtStep` exactly 8 B | ✅ |
| deterministic transport / row-boundary ordering | ✅ |
| 2,048 sparse p-locks, max four per step | ✅ |
| D032 exact six-tick tracker clock | ✅ |
| D034 exact 32-entry SPSC queue | ✅ |
| no active still-owned cross-track primary stealing | ✅ |
| no float in firmware DSP | ✅ through D054, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through D054, required onward |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Phase 5 FM foundation

### D048 WP5.0 mixed-engine routing

- per-instrument family/engine routing ✅
- song mode non-gating ✅
- 64 B identity publication / 8 B family route ✅
- all-8-BIT audio exact ✅
- target 72,792 / 19,676 / 113,000 B ✅

### D049 WP5.1 operator

- 4 B phase state ✅
- generated Q15 sine table ✅
- deterministic fixed-point render ✅
- full app exact WP5.0 ✅

### D050 WP5.2 algorithms

- four static algorithms ✅
- fixed OP4 -> OP3 -> OP2 -> OP1 evaluation ✅
- every operator exactly once/sample ✅
- 16 B routing state ✅

### D051 WP5.3 ratio/pitch

- 16 stable Q8 ratios ✅
- exhaustive mapping exact ✅
- deterministic ceiling behavior ✅

### D052 WP5.4 LEVEL/INDEX

- four LEVEL bytes + global INDEX byte ✅
- exact Q15/Q14 mappings ✅
- 65,536 edge combinations signed-32 safe ✅
- transient mapped struct 10 B ✅

### D053 WP5.5 feedback

- OP4 self-feedback only ✅
- exactly one-sample history ✅
- 2 B persistent state ✅
- 16,777,216 history/control arithmetic pairs safe ✅
- PI32v2 object 130 B ✅
- final closeout run `40165352623` ✅

### D054 WP5.6 envelopes

- four independent per-operator ADSR envelopes ✅
- D054 source-absent gate `40167537148` ✅
- exactly 20 B persistent state ✅
- exactly 16 B controls ✅
- Q15.16 state, max `0x7fff0000` ✅
- stable stage IDs 0..4 ✅
- immutable 256 x uint32 time table, 1,024 B ✅
- time-table SHA `6e94aa6553b78919913d49a24c80ca7317c913fff344294a107137f85ffbc025` ✅
- SUSTAIN map SHA `a799f70bd763053e79c3d9de6acdda97523fe54632105756c4252dc33d51c815` ✅
- LEVEL composition 65,536 pairs SHA `196659105845f6183b0779183a583226129a5b7f35e8ac8ac811479b117e606b` ✅
- hard retrigger / instant A-D-R / current-level release ✅
- max release exactly `2701155` advances ✅
- 65,536-sample stress SHA `8cdf12197813d5718b3b4c5a40375534f93ea006f61e5aa325f4b48af71e0849` ✅
- generated table byte-identical to generator ✅
- isolated source boundary exactly four files ✅
- no loops/div/mod/int64/float/heap/mutable globals ✅
- inherited D053-D049/WP5.0 host regressions exact ✅
- PI32v2 envelope object 1,680 B / 2,048 B ceiling ✅
- object SHA `22fa7937d7776fea37c8e758ca83ee0a3f5d6c506ebd4661a8290a4516e048d1` ✅
- full target exact 72,792 / 19,676 / 113,000 B ✅
- BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` ✅
- FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` ✅
- implementation run `40167933064`, host `121092688126`, target `121092688295` ✅
- evidence `WP5_6_VERIFICATION.md` ✅
- final documentation-head rerun ⬜

## WP5.7 FM voice/router integration - NEXT AFTER CLOSEOUT

Issue #50.

Required contract-first checks include:

- FM voice count/state budget ⬜
- ownership and primary/release-tail model ⬜
- deterministic allocator tie-breaks ⬜
- D048 cross-family NOTE ordering ⬜
- TIE/EMPTY/RETRIG/OFF/CUT family routing ⬜
- D049-D054 reset/retrigger/release integration ⬜
- no active cross-track primary stealing ⬜
- exact all-8-BIT regression equivalence ⬜
- target image/RAM/pool ceilings ⬜
- D055 frozen before integration changes ⬜

No physical FM-1 is available. `hardware_margin_claim=none`.
