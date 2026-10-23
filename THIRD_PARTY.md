# Third-Party and Upstream Components

This file records code/assets that FM1Tracker did not originate.

## Dinghy / Felucca / SLOOP platform

- Upstream repository: https://github.com/zvenson/dinghy
- Pinned commit: `d44df7db09b98ccad2f1233b8f83f5f09c04829e`
- License: GPL-3.0-only
- Import date: 2026-10-23
- Status: **baseline imported and pristine host-verified; WP1.4 FM1Tracker identity layer added without modifying the protected baseline app/platform/HAL/loader/build-driver implementation**

Upstream provenance at this commit:

- FM-1 platform/HAL/loader/build tooling: Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments / Felucca, with SLOOP changes by isod89.
- Dinghy app/platform wiring/host mode/docs: zvenson.

FM1Tracker modifications are tracked through Git history. WP1.4 intentionally applies app/package/artifact identity around the verified upstream-derived baseline instead of rewriting the baseline implementation before renamed verification is complete.

See:
- `docs/UPSTREAM_DINGHY.md` for the baseline policy;
- `docs/DINGHY_IMPORT_MANIFEST.md` for the exact pinned files, upstream blob IDs, exclusions and SDK build inputs;
- `docs/WP1_3_VERIFICATION.md` for pristine verification;
- `docs/WP1_4_VERIFICATION.md` for identity-only verification.

## Terminus font

- Material: Terminus 8x16 font data used by the Dinghy-generated `firmware/gen/felucca_font.h`
- License: SIL Open Font License 1.1
- Upstream license file: `assets/fonts/Terminus-LICENSE.txt`
- Status: imported byte-for-byte with the generated font header in WP1.2.

## JieLi AC79 SDK build inputs

Dinghy expects several build inputs from a separately obtained JieLi AC79 SDK checkout. The SDK files are **not** vendored into this repository merely to simplify setup.

Expected SDK:

`AC79NN_SDK_V1.2.1_2023-12-13`

Pinned Dinghy checks the following external inputs under `$AC79_SDK/cpu/wl82/tools/`:

- `uboot.boot` — SHA-256 `4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4`
- `cfg_tool.bin` — SHA-256 `276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b`
- `cfg/eq_cfg_hw.bin` — SHA-256 `41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d`

These SDK files are external build inputs, not FM1Tracker repository files. The project currently does not have an AC79 SDK checkout available in its CI environment, so full renamed device package verification remains pending WP1.5.

Upstream Dinghy identifies the SDK license as Apache-2.0. Before automating acquisition or using any mirror, WP1.5 must verify the exact source/provenance and must not silently vendor SDK material into this repository.

## Furnace

- Repository: https://github.com/tildearrow/furnace
- Role in FM1Tracker today: design/interoperability reference only
- Code imported: **none**

Most Furnace code is GPL-2.0-or-later, while individual bundled components may use different licenses. Any future Furnace-origin source/core requires a separate entry here with exact files/commit/license audit before import.

## Dinghy branding

Dinghy's `assets/logo.svg` / `logo.png` are explicitly not part of Dinghy's GPL grant according to its upstream licensing file. FM1Tracker did not import or reuse those assets.

The WP1.4 screen identity uses project text (`FM1TRACKER`) rendered through the retained Terminus-derived font data; it does not reuse Dinghy logo artwork.
