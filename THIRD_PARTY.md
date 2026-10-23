# Third-Party and Upstream Components

This file records code/assets that FM1Tracker did not originate.

## Dinghy / Felucca / SLOOP platform

- Upstream repository: https://github.com/zvenson/dinghy
- Pinned commit: `d44df7db09b98ccad2f1233b8f83f5f09c04829e`
- License: GPL-3.0-only
- Import date: 2026-10-23
- Status: **baseline imported, pristine verified, renamed as FM1Tracker and device-build verified; protected app/platform/HAL/loader/build-driver implementation remains unchanged through Phase 1**

Upstream provenance at this commit:

- FM-1 platform/HAL/loader/build tooling: Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments / Felucca, with SLOOP changes by isod89.
- Dinghy app/platform wiring/host mode/docs: zvenson.

FM1Tracker modifications are tracked through Git history. Phase 1 applies app/package/artifact identity around the verified upstream-derived baseline instead of rewriting the proven platform implementation.

See:
- `docs/UPSTREAM_DINGHY.md` for the baseline policy;
- `docs/DINGHY_IMPORT_MANIFEST.md` for the exact pinned files, upstream blob IDs, exclusions and SDK build inputs;
- `docs/WP1_3_VERIFICATION.md` for pristine verification;
- `docs/WP1_4_VERIFICATION.md` for identity-only verification;
- `docs/WP1_5_VERIFICATION.md` for the final renamed device-build/resource baseline.

## Terminus font

- Material: Terminus 8x16 font data used by the Dinghy-generated `firmware/gen/felucca_font.h`
- License: SIL Open Font License 1.1
- Upstream license file: `assets/fonts/Terminus-LICENSE.txt`
- Status: imported byte-for-byte with the generated font header in WP1.2.

## JieLi AC79 SDK build inputs

The pinned Dinghy documentation identifies the official JieLi SDK source as:

`https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git`

Exact release/tag used by the verified FM1Tracker Phase 1 device build:

`AC79NN_SDK_V1.2.1_2023-12-13`

The annotated tag resolves to commit:

`d179b4484759423312073f5fbb232501aa491047`

License: upstream Dinghy identifies this JieLi SDK release as Apache-2.0 for the build inputs it consumes. The SDK checkout is an **external build dependency** and is not vendored into FM1Tracker.

Pinned Dinghy/FM1Tracker verifies these package inputs under `$AC79_SDK/cpu/wl82/tools/`:

- `uboot.boot` — SHA-256 `4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4`
- `cfg_tool.bin` — SHA-256 `276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b`
- `cfg/eq_cfg_hw.bin` — SHA-256 `41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d`

WP1.5 GitHub Actions successfully shallow-cloned the exact official tag outside the repository, verified commit and hashes, and completed the FM1Tracker firmware/package build. See `docs/WP1_5_VERIFICATION.md`.

Local setup may use:

```sh
git clone --depth 1 --branch AC79NN_SDK_V1.2.1_2023-12-13 \
  https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK.git ~/fw-AC79_AIoT_SDK
```

Do not commit the SDK checkout or copy its files into this repository merely for convenience.

## Furnace

- Repository: https://github.com/tildearrow/furnace
- Role in FM1Tracker today: design/interoperability reference only
- Code imported: **none**

Most Furnace code is GPL-2.0-or-later, while individual bundled components may use different licenses. Any future Furnace-origin source/core requires a separate entry here with exact files/commit/license audit before import.

## Dinghy branding

Dinghy's `assets/logo.svg` / `logo.png` are explicitly not part of Dinghy's GPL grant according to its upstream licensing file. FM1Tracker did not import or reuse those assets.

The WP1.4 screen identity uses project text (`FM1TRACKER`) rendered through the retained Terminus-derived font data; it does not reuse Dinghy logo artwork.
