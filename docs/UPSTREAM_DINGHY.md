# Pinned Dinghy Upstream Baseline

FM1Tracker Phase 1 is based on a deliberately pinned Dinghy snapshot rather than an unqualified `main` checkout.

## Upstream

- Repository: https://github.com/zvenson/dinghy
- Commit: `d44df7db09b98ccad2f1233b8f83f5f09c04829e`
- Commit date: 2026-10-12
- Upstream status in README: Dinghy 0.3
- License: GPL-3.0-only

The commit was chosen because it is the current Dinghy `main` revision at the start of FM1Tracker Phase 1 on 2026-10-23.

## Why pin it

Pinning gives FM1Tracker a reproducible platform baseline. Later Dinghy changes can be reviewed and deliberately rebased/cherry-picked instead of silently changing the hardware abstraction, updater, USB or memory assumptions underneath the tracker.

## Provenance boundaries from upstream

According to Dinghy's `LICENSING.md` at the pinned commit:

- the FM-1 platform code (`firmware/hal/`, `firmware/loader/`, `crt0.S`, `app.ld`, core `firmware/src/` platform files and `tools/`) is credited to Leo Kuroshita / Hügelton Instruments / Felucca with SLOOP changes by isod89;
- `firmware/src/dinghy.c`, `platform.c`, host mode/tests and Dinghy documentation are credited to zvenson;
- the generated Terminus 8x16 font data is derived from Terminus under SIL OFL 1.1;
- required JieLi SDK build inputs are not shipped in Dinghy and are obtained from the AC79 SDK at build time;
- Dinghy logo assets are not GPL and must not be copied as FM1Tracker branding.

FM1Tracker must preserve the applicable upstream source headers/notices when the baseline source is copied.

## Import policy

For the first baseline import:

1. copy platform/build/test source from this exact commit;
2. do **not** copy Dinghy logo assets as FM1Tracker branding;
3. preserve the Terminus font license if the generated font data is retained;
4. keep upstream headers and licensing text;
5. make only identity/name/build-output changes required to turn the clean baseline into `FM1TRACKER`;
6. run the host build before adding any tracker/sequencer code;
7. record clean baseline memory/code usage before feature work.

## Future upstream updates

Any future Dinghy update should record:

```text
old Dinghy commit
new Dinghy commit
upstream commits reviewed
FM1Tracker files affected
reason for update
host build result
firmware build result
resource delta
```

Do not track Dinghy `main` automatically.
