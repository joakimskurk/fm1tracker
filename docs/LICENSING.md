# FM1Tracker Licensing and Provenance Policy

Status: **Phase 0 policy**

This document is a project policy, not legal advice.

The goal is to make every imported source file, table, emulation core and asset traceable and redistributable with the FM1Tracker firmware.

## 1. Intended firmware license

FM1Tracker intends to use the Dinghy/Felucca/SLOOP platform as its firmware base.

Dinghy is GPL-3.0-only. Therefore, once FM1Tracker imports/derives from that platform, the combined firmware source should be distributed under **GPL-3.0-only**, while preserving upstream notices and any compatible third-party notices.

Before the first platform import, add the full GPL-3.0 license text at repository root and preserve upstream `LICENSING.md` / copyright headers from the copied baseline.

## 2. Furnace

Furnace is a design and interoperability reference.

The Furnace repository states that most of Furnace is GPL-2.0-or-later and that certain components, including emulation cores/libraries/tools, have their own licenses.

Project rule:

> Never treat “present in the Furnace source tree” as sufficient license clearance for FM1Tracker.

Any Furnace source file or third-party core considered for reuse must be audited at the file/tree level before copying code.

Because GPL-2.0-or-later code may generally be distributed under a later GPL version when its own terms permit, compatible Furnace-origin code may be technically possible to combine with a GPL-3.0-only firmware. However, the exact file and every relevant dependency still require review.

The default implementation strategy is therefore **clean original implementation of the FM1Tracker tracker/engines**, using Furnace for behavior/workflow reference unless there is a strong reason to import code.

## 3. No full Furnace port assumption

FM1Tracker does not plan to copy:

- the full Furnace desktop engine;
- the Furnace UI;
- bundled emulation cores as a group;
- Furnace artwork/assets/branding;
- `.fur` parsing code merely for feature parity.

Interoperability should preferably be implemented from documented formats/specifications or through a deliberately scoped converter/subset.

## 4. Upstream FM-1 platform credits

When the Dinghy baseline is imported, preserve at minimum the notices required by its source and licensing documentation for:

- Dinghy / zvenson;
- Felucca / Leo Kuroshita / Hügelton Instruments;
- SLOOP / isod89;
- any other third-party code already identified by Dinghy/Felucca/SLOOP.

Do not remove copyright headers from copied source files.

## 5. Import checklist

Before adding external code, record:

```text
Component:
Upstream repository:
Exact commit/tag:
Files imported:
Upstream license:
License file/header inspected:
GPL-3.0-only compatibility conclusion:
Required copyright/notice text:
Modifications made:
CPU/RAM relevance:
Date audited:
```

A future `THIRD_PARTY.md` should contain this ledger once the first external component beyond the Dinghy baseline is added.

## 6. Emulation-core checklist

A chip/emulation core requires a stricter review:

- license is explicit;
- license is compatible with distribution inside GPL-3.0-only firmware;
- no non-commercial-only restriction;
- no “source available” terms mistaken for an open-source license;
- no firmware/ROM/data blobs with unclear redistribution rights;
- no copied proprietary lookup tables/assets without permission;
- attribution requirements are understood;
- dependencies have also been audited;
- implementation can run within FM-1 hardware constraints.

If any point is unclear, do not import the core.

## 7. Names and compatibility claims

Generic original engines should use generic names such as:

- PULSE
- TRIANGLE
- WAVETABLE
- NOISE
- FM4

Do not label an engine “NES”, “Game Boy”, “SNES”, “YM2612”, etc. merely because it sounds reminiscent of that hardware. Specific names should only be used when the implementation intentionally targets that system/chip and the project can substantiate the compatibility/emulation claim.

Trademark references, when needed for factual compatibility/documentation, should not imply endorsement.

## 8. Project assets

Create original FM1Tracker branding, icons and screen graphics.

Do not copy Furnace logos, screenshots or artwork into the distributable firmware/repository unless the asset license explicitly permits the intended reuse and the required attribution is included.

## 9. Source availability

For firmware releases based on GPL-3.0-only platform code:

- publish the corresponding source for the released firmware;
- keep build instructions sufficient to reproduce the firmware as far as the upstream toolchain permits;
- preserve source notices;
- publish local modifications rather than distributing only a `.fwsc` binary.

## 10. References checked during Phase 0

- Dinghy repository: https://github.com/zvenson/dinghy
- Dinghy licensing statement: GPL-3.0-only, with credits/provenance documented upstream
- Furnace repository: https://github.com/tildearrow/furnace
- Furnace `LICENSE`: most of Furnace GPL-2.0-or-later; some components under separate licenses

This policy must be updated when the first Dinghy source import and any later third-party engine/core import occur.
