from pathlib import Path


def once(path, old, new):
    p = Path(path)
    s = p.read_text()
    n = s.count(old)
    assert n == 1, (path, old[:140], n)
    p.write_text(s.replace(old, new, 1))

# Append the accepted decision exactly once.
p = Path('docs/DECISIONS.md')
s = p.read_text().rstrip() + '\n'
assert '## D046 —' not in s
s += '''\n## D046 — WP4.10 is a production-frozen representative linked-engine benchmark and Phase 4 exit gate\n\nDate: 2026-10-24\nStatus: accepted — frozen before WP4.10 benchmark/test implementation\n\n**Decision:** `docs/WP4_10_PHASE4_EXIT_CONTRACT.md` is normative. WP4.10 is verification/benchmark/closeout work, not a feature package. The linked D045 production baseline is `c93c39ad344760b4da0dfbc76829ca5fef886174`; if no real defect is found, production source/HAL/loader/link/build/tooling must remain byte-identical to that checkpoint. Any defect requires the smallest bounded fix, a focused regression, explicit resource/artifact delta and a complete exit-gate rerun.\n\nD046 freezes a representative 8-track / 64-row workload using all four engines simultaneously, deterministic engine rotation at rows 16/32/48, and an eight-row NOTE/TIE/NOTE/OFF/NOTE/TIE/event/NOTE ownership cycle. The full D028 pool is intentionally saturated as exactly `8 * 64 * 4 = 2048` records: even rows combine D031 VOICE LEVEL/FINE/TRACK LEVEL with the engine-defining D044 slot-0 parameter; odd rows fill the complete D044 four-slot page. Values span boundary and representative bytes, with slow RELEASE rows forcing D043 detached-tail contention on the following NOTE row.\n\nEvery track/row also receives deterministic D032/D033 command/value pressure across the complete assigned command domain and the boundary values `0..6,255`. Exact D020/D032 timing remains authoritative; the long linked run crosses at least 4096 rows / 64 pattern wraps while retaining deterministic row/tick/action/allocation counts and state digests. D043 may reclaim only unowned RELEASE tails and must never steal a still-owned active cross-track primary voice.\n\nD034 remains a separate 32-entry audition queue and four-sine manual lane. The representative linked workload includes concurrent full-queue/overflow/recovery epochs while all eight tracker tracks run, proving bounded 32-event consumption, O(1) overflow discard, fail-to-silence manual recovery and tracker timing/state independence.\n\n“Benchmark” without a physical FM-1 means deterministic functional workload + structural realtime-bound proof + pinned target build/resource evidence. Host elapsed time may be recorded for diagnostics only and is never a pass/fail hardware-timing proxy. The structural audit freezes one eight-slot engine scan per tracker sample, at most eight tick-due checks on a non-row-boundary running frame, exactly eight fixed row events on a row-boundary frame, at most one D033 scheduled action per due track tick, and no allocator/list/store/screen/float/runtime-64-bit-divide/spin/retry work in the audio path.\n\nPinned target inputs remain `jieli-linux-toolchains-20260730.1`, AC79 SDK `d179b4484759423312073f5fbb232501aa491047`, identity `FM-1_987`. With no production fix, exact WP4.9 target identity must remain 72,504 B image / 19,604 B normal RAM / 113,000 B pool, BIN `214fba8ec0af4be3120bc91dafbaa24e7d4d8a5a6de7395de2473f12374c4215`, FWSC `20f0bbe556edcf6a00118f9983335913dda2f29fc64d3c87997bfd6756bcf119`, and the normal linked host WAV remains `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076`.\n\n**Reason:** WP4.9 proved correctness of the linked engine but the phase still needs one declared representative whole-system workload that combines maximum p-lock storage pressure, all engines/tracks, effect/event timing, release-tail contention and queue pressure before Phase 5 begins. Structural bounds and a real pinned target build are meaningful without hardware; host wall-clock timing is not a substitute for measured AC791N callback margin.\n\n**Consequence:** WP4.10 may add only tests, fixtures, benchmark instrumentation, CI, documentation and evidence unless those tests expose a real bounded defect. The final evidence is `docs/WP4_10_VERIFICATION.md`. Phase 4 / #30 and WP4.10 / #41 may close at host/device-build level only after D046's representative/full-pool/long-run/queue/realtime/inherited/target gates are green. Physical install/audio/quality/execution-margin verification remains explicitly pending until a physical FM-1 exists. Phase 5/FM4 must not start earlier.\n'''
p.write_text(s)

# PROJECT_STATE.
ps = 'docs/PROJECT_STATE.md'
once(ps,
     'State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 representative benchmark + Phase 4 closeout active - 2026-10-24**',
     'State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 active; D046 FROZEN; benchmark/exit implementation next - 2026-10-24**')
once(ps,
     'State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 active; final representative benchmark/exit contract must be frozen before new production changes**',
     'State: **IN PROGRESS - WP4.1/D037 through WP4.9/D045 COMPLETE; WP4.10 / #41 active; D046 FROZEN; verification-only benchmark/exit implementation next**')
once(ps,
     'WP4.10 / #41 is the only remaining Phase 4 package. It must freeze a representative benchmark/exit contract before any new production change, then prove the linked D037-D045 path under a deterministic eight-track representative workload and finish the Phase 4 exit gate.',
     'D046 is frozen in `docs/WP4_10_PHASE4_EXIT_CONTRACT.md`. WP4.10 / #41 is verification/benchmark/closeout only unless a real defect is found. The next implementation must build the frozen full-pool representative workload, long linked run, concurrent D034 pressure, structural realtime audit, inherited regression gate and pinned target identity proof without changing production source.')
once(ps,
     '- D045 deliberately supersedes the historical proof-sine default WAV for the tracker lane. The linked 8-BIT full-app reference is `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` / 286,464 frames; preserve it unless a later explicit decision supersedes it with evidence.',
     '- D045 deliberately supersedes the historical proof-sine default WAV for the tracker lane. The linked 8-BIT full-app reference is `17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076` / 286,464 frames; preserve it unless a later explicit decision supersedes it with evidence.\n- D046: WP4.10 is production-frozen verification/benchmark work. It saturates all 2,048 D028 records across all 512 pattern-0 track/rows, exercises all four engines + D031/D032/D033/D044 + D043 tail contention + D034 queue pressure, proves structural realtime bounds and pinned target identity, and treats host elapsed time as diagnostic only.')
once(ps,
     'Freeze the WP4.10 / #41 representative eight-track benchmark and Phase 4 exit contract before any new production change. The benchmark must exercise the linked D037-D045 path, retain bounded realtime ownership, re-run applicable inherited stress/regression gates, freeze pinned target resources/artifact hashes and make no physical FM-1 execution-margin claim without hardware. Do not begin Phase 5/FM4 until WP4.10 is green.',
     'Implement D046 verification only: first the deterministic 2,048-lock / 512-row linked workload and long-run state/count digest, then concurrent D034 pressure + structural realtime audit, inherited regressions and exact pinned target identity. Keep production byte-identical to `c93c39ad344760b4da0dfbc76829ca5fef886174` unless a real defect is proven. Do not begin Phase 5/FM4 until the complete WP4.10 exit gate is green.')
once(ps,
     '- WP4.9 D045 final closeout: `89fabc51348078101c8e376bba7814af9d42ae80` — run `40116897104`, job `120939922897`; artifact `12570465704`; evidence `WP4_9_VERIFICATION.md`',
     '- WP4.9 D045 final closeout: `89fabc51348078101c8e376bba7814af9d42ae80` — run `40116897104`, job `120939922897`; artifact `12570465704`; evidence `WP4_9_VERIFICATION.md`\n- WP4.10 D046 contract: `aaceafc24322611bf00255bf028466d7d88643de` — `WP4_10_PHASE4_EXIT_CONTRACT.md`')
once(ps,
     '2026-10-24 - WP4.9 / #40 COMPLETE. D045 linked the real eight-track tracker lane to PULSE/NOISE/TRIANGLE/WAVETABLE-8, preserved bounded D031-D034 ownership, passed canonical Gate 5 fixtures + Gate 6 inherited/realtime closeout, and built at 72,504 B image / 19,604 B normal RAM / 113,000 B pool. Final run `40116897104`, job `120939922897`; evidence `WP4_9_VERIFICATION.md`. WP4.10 / #41 is now the only active Phase 4 package; freeze its representative benchmark/exit contract before any production change.',
     '2026-10-24 - WP4.10 / #41 active with D046 FROZEN in `WP4_10_PHASE4_EXIT_CONTRACT.md`. WP4.9 production remains the immutable baseline unless stress exposes a real defect. Exact next action: implement the frozen representative full-pool/long-run/queue/realtime/target verification gates, then produce `WP4_10_VERIFICATION.md` and close Phase 4 only if every gate is green.')

# WORK_PACKAGES.
once('docs/WORK_PACKAGES.md',
     '- **WP4.10 representative eight-track benchmark + Phase 4 closeout** — IN PROGRESS — issue #41 — freeze benchmark/exit contract before any production change',
     '- **WP4.10 representative eight-track benchmark + Phase 4 closeout** — IN PROGRESS — issue #41 — D046 FROZEN in `WP4_10_PHASE4_EXIT_CONTRACT.md`; verification-only full-pool/long-run/queue/realtime/pinned-target exit implementation next')

# ROADMAP.
once('docs/ROADMAP.md',
     'Active package: WP4.10 / #41 — WP4.9 closed on run `40116897104`, job `120939922897`; evidence `WP4_9_VERIFICATION.md`. Freeze the representative eight-track benchmark/Phase 4 exit contract before any new production change, then close Phase 4 without claiming physical execution margin until hardware exists. Branch: `phase4/8bit-engine`.',
     'Active package: WP4.10 / #41 — D046 FROZEN in `WP4_10_PHASE4_EXIT_CONTRACT.md`. Implement the production-frozen representative 2,048-lock/eight-track benchmark, long-run + D034 pressure + structural realtime audit, inherited regressions and exact pinned target identity; then close Phase 4 without claiming physical execution margin until hardware exists. Branch: `phase4/8bit-engine`.')
once('docs/ROADMAP.md',
     '- measured eight-track hardware benchmark.',
     '- representative eight-track host/device-build benchmark with physical execution-margin measurement explicitly deferred until hardware exists.')

# TEST_MATRIX: replace the single placeholder with the frozen D046 exit set.
once('docs/TEST_MATRIX.md',
     '| Representative eight-track timing | benchmark | required | required later | ⬜ WP4.10 |',
     '| D046 full 2,048-record linked workload across all 8 tracks / 64 rows | required | required | n/a | ⬜ WP4.10 implementation |\n| D046 all four engines + D031/D032/D033/D044 representative matrix deterministic | required | required | optional later | ⬜ WP4.10 implementation |\n| D046 4096-row / 64-wrap linked long-run counts + state digest deterministic | required | required | optional later | ⬜ WP4.10 implementation |\n| D046 D043 release-tail contention never steals owned active cross-track voice | required | required | optional later | ⬜ WP4.10 implementation |\n| D046 concurrent D034 32-event/full/overflow/recovery pressure with tracker active | required | required | optional later | ⬜ WP4.10 implementation |\n| D046 structural realtime work-bound audit; host wall time non-normative | inspect/test | required | required later | ⬜ WP4.10 implementation |\n| D046 production tree remains D045 baseline unless focused defect proof exists | git compare | required | n/a | ⬜ WP4.10 implementation |\n| D046 pinned target image/RAM/pool + BIN/FWSC identity frozen | n/a | required | n/a | ⬜ WP4.10 implementation |\n| Representative eight-track physical execution margin | n/a | n/a | required later | ⬜ no physical FM-1 available |')
once('docs/TEST_MATRIX.md',
     'WP4.9 final linked-engine evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_VERIFICATION.md`.',
     'WP4.9 final linked-engine evidence: `WP4_9_LINKED_ENGINE_CONTRACT.md`, `WP4_9_VERIFICATION.md`.\nWP4.10 normative exit contract: `WP4_10_PHASE4_EXIT_CONTRACT.md` (D046).')

# START_HERE.
once('START_HERE.md',
     'For the current active package, also read the most recently completed verification and the active package contract named by `docs/PROJECT_STATE.md`. As of the current 2026-10-24 handoff, WP4.9 / D045 is verified in `docs/WP4_9_VERIFICATION.md`, and WP4.10 / #41 is the only active Phase 4 package; freeze its representative benchmark/exit contract before any new production change.',
     'For the current active package, also read the most recently completed verification and the active package contract named by `docs/PROJECT_STATE.md`. As of the current 2026-10-24 handoff, WP4.9 / D045 is verified in `docs/WP4_9_VERIFICATION.md`; WP4.10 / #41 is active with D046 FROZEN in `docs/WP4_10_PHASE4_EXIT_CONTRACT.md`, and verification-only benchmark/Phase 4 exit implementation is next.')

print('WP4_10_D046_FREEZE_OK=1')
