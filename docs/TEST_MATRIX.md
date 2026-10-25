# FM1Tracker Test Matrix

This file is the current regression index. Exact historical vectors, hashes and package evidence remain in the corresponding `WP*_VERIFICATION.md` and contract documents.

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
| exact integer transport / row-boundary ordering | ✅ |
| 2,048 sparse p-locks, max four per step | ✅ |
| D032 exact six-tick tracker clock | ✅ |
| D034 exact 32-entry SPSC queue | ✅ |
| no active still-owned cross-track primary stealing | ✅ |
| no float in firmware DSP | ✅ through WP5.5, required onward |
| no runtime 64-bit division in firmware DSP | ✅ through WP5.5, required onward |
| physical AC791N execution margin | ⛔ no physical FM-1 |

## Completed Phase 5 FM foundations

### D048 WP5.0 mixed-engine routing

- per-instrument family/engine routing ✅
- song mode non-gating ✅
- 64 B identity publication / 8 B family route ✅
- D046 audio exact ✅
- target 72,792 / 19,676 / 113,000 B ✅

### D049 WP5.1 fixed-point FM operator

- 4 B state and generated Q15 sine table ✅
- PI32v2 object 692 B ✅
- full app exact WP5.0 ✅

### D050 WP5.2 curated FM4 routing

- four static algorithms and fixed OP4 -> OP3 -> OP2 -> OP1 evaluation ✅
- 16 B routing state ✅
- fixed vector/hash/stress suite exact ✅
- PI32v2 router object 362 B ✅
- full app exact WP5.0 ✅

### D051 WP5.3 integer ratio/pitch mapping

- 16 stable Q8 ratios ✅
- exhaustive 262,144 outputs exact ✅
- PI32v2 ratio object 264 B ✅
- full app exact WP5.0 ✅

### D052 WP5.4 level/index mapping

- four LEVEL bytes + one global INDEX byte ✅
- exact Q15/Q14 mappings and full edge proof ✅
- transient mapped struct 10 B ✅
- PI32v2 controls object 108 B ✅
- full target exact WP5.0 ✅
- evidence `WP5_4_VERIFICATION.md` ✅

### D053 WP5.5 bounded OP4 feedback

- D053 frozen source-absent ✅ run `40164610821`, job `121082987705`
- topology OP4 -> OP4 only ✅
- exact one-sample delay ✅
- state 2 B signed prior OP4 output ✅
- FEEDBACK 0..255 -> Q14 exact ✅
- full signed-int16 x 256 arithmetic surface exact ✅ 16,777,216 pairs
- full surface SHA `6eff2600842fa460d7663f025d0f9d13fc1176deabfcb3aa39e22e5fbdb852af` ✅
- canonical OP4 SHA `6eee497f61dec5556882cc7e66ef0b493529dcf1ff56d8e06193886eaa792e49` ✅
- four algorithm hashes exact ✅
- algorithm switch retains history ✅
- four extreme 65,536-sample stresses exact ✅
- reset/retrigger/null semantics exact ✅
- source boundary only `fmt_fm_feedback.h/.c` ✅
- static bounded source audit ✅
- inherited D049-D052/WP5.0 regressions exact ✅
- PI32v2 feedback object 130 B / 1,024 B ceiling ✅
- object SHA `147c623c49fa8756f36937589d96f2da5139b6e3d1a09790e95d91c5f770256d` ✅
- future feedback state 2 B per FM voice ✅
- full target exact 72,792 / 19,676 / 113,000 B ✅
- BIN `a2f0c2a1ce924e77f8b6818721ca30bec7dd55344564b87eac7fd784a91db125` ✅
- FWSC `cab0d447fd6ef0c76e34e9b591ce6120517498dd0d10902d4300ee1594e8f073` ✅
- canonical run `40164953532`, host `121083999893`, target `121083999790` ✅
- evidence `WP5_5_VERIFICATION.md` ✅

## WP5.6 bounded envelope strategy - NEXT AFTER CLOSEOUT

Issue #49.

| Check | Status |
|---|---|
| common vs per-operator envelope cost/capability audit | ⬜ |
| bounded V1 stage architecture | ⬜ |
| stable time/level control domains | ⬜ |
| retrigger/reset/zero-time behavior | ⬜ |
| deterministic update cadence | ⬜ |
| deterministic vectors and long stress | ⬜ |
| state/realtime/PI32v2 ceilings | ⬜ |
| D054 frozen before production source | ⬜ |
| source-absent reference gate | ⬜ |
| inherited D049-D053 regressions | ⬜ |
| full app exact WP5.0 while isolated | ⬜ |
| `WP5_6_VERIFICATION.md` | ⬜ |

D054 does not exist yet.

## Remaining Phase 5

WP5.7 FM voice/router integration, WP5.8 FM pages/p-locks, WP5.9 mixed validation, WP5.10 worst-case Phase 5 exit.

No physical FM-1 is available. `hardware_margin_claim=none`.
