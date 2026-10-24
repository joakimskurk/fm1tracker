from pathlib import Path


def once(path, old, new):
    p = Path(path)
    s = p.read_text()
    count = s.count(old)
    assert count == 1, (path, old[:120], count)
    p.write_text(s.replace(old, new, 1))

# TEST_MATRIX: convert the two WP3.7 open checks into frozen D033 evidence,
# retain WP3.8 queue as open, and add the durable verification document.
once(
    'docs/TEST_MATRIX.md',
    "| Queue overflow policy deterministic | required | required | stress test | ⬜ WP3.8 |\n| Note delay/cut timing bounded | required | required | required | ⬜ WP3.7 |\n| Retrig timing/count bounded | required | required | required | ⬜ WP3.7 |",
    "| D033 command domain exactly RETRIG `0x06` / NOTE DELAY `0x07` / NOTE CUT `0x08` | required | compile | observable | ✅ WP3.7 |\n"
    "| D033 row-event state exactly 4 B and no general queue | required | required | n/a | ✅ WP3.7 |\n"
    "| NOTE DELAY 0..5 timing bounded on exact D032 six-tick grid | required | required | required | ✅ frame 2757 for delay 3 at 120 BPM |\n"
    "| NOTE CUT 0..5 timing bounded including CUT0-before-first-sample | required | required | required | ✅ WP3.7 full-app audio proof |\n"
    "| RETRIG interval/count bounded to nonzero ticks 1..5 | required | required | required | ✅ frame 1838 first divergence for retrig 2 |\n"
    "| D033 values >5 are scheduling no-ops without modulo wrap | required | compile | observable | ✅ delay6/cut6/retrig6 byte-identical to control |\n"
    "| Delayed NOTE consumes exact D031 row snapshot captured at dispatch | required | compile | observable | ✅ WP3.7 |\n"
    "| D033 preserves frozen D032 audio hashes and historical default/D031 WAVs | required | required | n/a | ✅ run `40077809743` |\n"
    "| WP3.7 final resource delta measured/frozen | n/a | required | n/a | ✅ +412 B image / +4 B RAM / +0 B pool |\n"
    "| WP3.7 preserves all Phase 2 + WP3.1-WP3.6 regressions | required | required | later | ✅ run `40077809743` |\n"
    "| Queue overflow policy deterministic | required | required | stress test | ⬜ WP3.8 / #27 |"
)
once(
    'docs/TEST_MATRIX.md',
    "- `WP3_6_VERIFICATION.md`\n",
    "- `WP3_6_VERIFICATION.md`\n- `WP3_7_VERIFICATION.md`\n"
)

# ROADMAP: advance the authoritative progress summary without starting WP3.8 source.
once(
    'docs/ROADMAP.md',
    "Status: **IN PROGRESS — WP3.1–WP3.6 complete; WP3.7 next**",
    "Status: **IN PROGRESS — WP3.1–WP3.7 complete; WP3.8 next**"
)
once(
    'docs/ROADMAP.md',
    "7. ⬜ **WP3.7 — retrig / note delay / note cut**, issue #26, READY TO START.\n8. ⬜ WP3.8 — bounded event queue and overflow policy.",
    "7. ✅ **WP3.7 — retrig / note delay / note cut**. D033 adds commands `0x06..0x08`, exact D032 six-tick scheduling and a 4 B fixed row-event state without a general queue. Final run `40077809743`; evidence `WP3_7_EVENT_CONTRACT.md`, `WP3_7_VERIFICATION.md`.\n8. ⬜ **WP3.8 — bounded event queue and overflow policy**, issue #27, READY FOR D034 ARCHITECTURE FREEZE."
)
once(
    'docs/ROADMAP.md',
    "Current verified Phase 3 footprint after WP3.6: 65,528 B image, 13,004 B normal RAM and 113,000 B pool. The historical no-effect WAV and D031 locked WAV remain byte-identical.\n\nImmediate next package: **WP3.7 / #26**. D033 must be frozen before source. WP3.8 remains a separate queue/overflow package and must not be pulled forward by convenience.",
    "Current verified Phase 3 footprint after WP3.7: 65,940 B image, 13,008 B normal RAM and 113,000 B pool. The historical default WAV, D031 locked WAV and frozen WP3.6 D032 audio fixtures remain byte-identical.\n\nImmediate next package: **WP3.8 / #27**. Freeze D034 before source. D034 must prove the producer/consumer need, exact event record/capacity/RAM budget, ownership, timestamp domain, overflow policy and bounded audio work before any queue implementation begins."
)

# CHANGELOG: append one milestone section once.
p = Path('CHANGELOG.md')
s = p.read_text()
marker = '## 2026-10-24 - WP3.7 retrig / note delay / note cut'
if marker not in s:
    s += """

## 2026-10-24 - WP3.7 retrig / note delay / note cut

- Frozen D033 before production audio integration in `docs/WP3_7_EVENT_CONTRACT.md`.
- Extended the existing tracker command domain with RETRIG `0x06`, NOTE DELAY `0x07` and NOTE CUT `0x08`; values above the valid scheduling range safely no-op.
- Reused the exact D032 six-tick D020-derived row grid rather than introducing a second timing domain.
- Added exactly 4 B of fixed `FmtTrackerRowEventState`; WP3.7 deliberately does not pull the WP3.8 general event queue forward.
- Integrated delayed NOTE against the exact D031 row snapshot captured at dispatch, CUT0 before the first rendered sample, and bounded nonzero-tick delay/cut/retrig actions.
- Added deterministic scheduler/unit and full-app WAV proofs. At 120 BPM, DELAY 3 and CUT 3 act at frame 2757 and RETRIG 2 first diverges at frame 1838.
- Verified delay/cut/retrig value 6 as byte-identical scheduling no-ops and preserved all frozen WP3.6 effect WAVs plus historical default and D031 locked WAVs.
- Final resources: 65,940 B image / 13,008 B normal RAM / 113,000 B pool; delta vs WP3.6 +412 / +4 / +0 B.
- Final artifacts: `fm1tracker.bin` `f71bc5770ce2b56bcbc3fb4034b882f1a80f0b1997cdd4534cc7b8dd45100e2b`; `fm1tracker-0.1.fwsc` `cd4b3138ecb6e4e853b67cbe4b8a049e43290798dad26accf606279dc69e383d`.
- Final clean-tree verification: checkpoint `d2a97798149816957745de818e4568c518a9a7e7`, run `40077809743`, job `120824870575`; evidence `docs/WP3_7_VERIFICATION.md`.
- Next: WP3.8 / #27 bounded event queue and overflow policy. D034 architecture freeze is required before source.
"""
    p.write_text(s)

print('WP3_7_DOCS_FINALIZED=1')
