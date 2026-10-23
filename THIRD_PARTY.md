# Third-Party and Upstream Components

This file records code/assets that FM1Tracker did not originate.

## Dinghy / Felucca / SLOOP platform

- Upstream repository: https://github.com/zvenson/dinghy
- Pinned commit: `d44df7db09b98ccad2f1233b8f83f5f09c04829e`
- License: GPL-3.0-only
- Import date: 2026-10-23
- Status: **baseline import in progress**

Upstream provenance at this commit:

- FM-1 platform/HAL/loader/build tooling: Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments / Felucca, with SLOOP changes by isod89.
- Dinghy app/platform wiring/host mode/docs: zvenson.

FM1Tracker modifications will be marked through Git history and project-specific source headers where appropriate.

See `docs/UPSTREAM_DINGHY.md` for the baseline policy.

## Terminus font

- Material: Terminus 8x16 font data used by the Dinghy-generated `firmware/gen/felucca_font.h`
- License: SIL Open Font License 1.1
- Upstream license file: `assets/fonts/Terminus-LICENSE.txt`
- Status: include the upstream license if the generated font header is imported.

## JieLi AC79 SDK build inputs

Dinghy expects several build inputs from a separately obtained JieLi AC79 SDK checkout. The SDK files are **not** to be vendored into this repository merely to simplify setup.

Upstream Dinghy identifies the required SDK as:

`AC79NN_SDK_V1.2.1_2023-12-13`

and identifies its license as Apache-2.0.

## Furnace

- Repository: https://github.com/tildearrow/furnace
- Role in FM1Tracker today: design/interoperability reference only
- Code imported: **none**

Most Furnace code is GPL-2.0-or-later, while individual bundled components may use different licenses. Any future Furnace-origin source/core requires a separate entry here with exact files/commit/license audit before import.

## Dinghy branding

Dinghy's `assets/logo.svg` / `assets/logo.png` are explicitly not part of Dinghy's GPL grant according to its upstream licensing file. FM1Tracker does not intend to reuse those assets.
