# WP5.8 D056 Preimplementation Evidence

Status: **ACCEPTED - IMPLEMENTATION AUTHORIZED**
Date: 2026-10-25
Package: WP5.8 / issue #51
Branch: `phase5/fm4-parameters`
Contract checkpoint: `9436e385a96fa89b74c4c016c03f7857c2373552`
Normative contract: `WP5_8_FM_PARAMETER_PAGE_CONTRACT.md`
Audit: `WP5_8_PRECONTRACT_AUDIT.md`

## Canonical source-absent run

```text
run      40178938471
job      121125607384
head     9436e385a96fa89b74c4c016c03f7857c2373552
status   SUCCESS
artifact wp5-8-contract-evidence
ID       12592494488
SHA-256  9c8ca18fe027b7286c3fe4e0b2086e133780b89df8721a01f4a91eafc672d51c
```

## Accepted markers

```text
WP5_8_SOURCE_ABSENT_OK=1 firmware_delta=0 new_fm_param_sources=0
WP5_8_REFERENCE_OK=1 ids=27 pages=7 base_bytes=27 bank_bytes=432 legacy_cache=2048 generalized_cache=8704 net_cache_growth=6656 known_ram_growth=7088
FMT_PARAM_REGISTRY_TESTS_OK=1
WP5_7_HOST_GATE_OK=1 d055=integrated engine=788 slots=8 d043=exact d048=integrated d054=exact all8bit=wp5_0_identical mixed=deterministic hardware_margin_claim=none
WP5_8_CONTRACT_DOCS_OK=1 decision=D056 ids=27 pages=7 source_boundary=frozen
WP5_8_CONTRACT_FREEZE_OK=1 decision=D056 production_authorized_after_green=1 hardware_margin_claim=none
```

## Source boundary proven before implementation

At the accepted checkpoint:

- `firmware/src` and `firmware/gen` were byte-for-byte unchanged from the accepted WP5.7 closeout `80587f12786122dcec819260ddad5dd5db4b1005`;
- `firmware/src/fmt_fm_params.h` did not exist;
- `firmware/src/fmt_fm_params.c` did not exist;
- `firmware/src/fmt_fm_linked_params.h` did not exist;
- `firmware/src/fmt_fm_linked_params.c` did not exist;
- the inherited semantic parameter registry gate remained exact;
- the accepted final D055 integrated host gate remained exact;
- all-8BIT output remained WP5.0-identical and mixed-family behavior remained deterministic.

## Frozen D056 resource and ABI contract

```text
FM semantic IDs          27
namespace                0x60..0x7B, except 0x63
logical pages             7
FmtFmParamSet             27 B target
FM base bank             432 B target
max p-locks per step       4 unchanged
generalized row cache   8704 B target
legacy 8BIT view        2048 B exact
net cache growth        6656 B
known normal-RAM growth 7088 B
image ceiling          93432 B
normal RAM ceiling     28656 B
pool                  113000 B
```

The existing 8BIT cache ABI, symbol and first 2048-byte layout remain frozen. FM row overrides must be compact, bounded to the existing four p-locks per step, validated in their semantic domains before publication, and published at row boundaries without traversing the lock store in the audio inner loop.

## Implementation authorization

D056 production implementation is now authorized inside the bounded source family defined by the normative contract. The first production slice must extend the existing semantic parameter architecture rather than introduce a parallel FM parameter system.

The implementation must preserve:

- the frozen 27 semantic FM IDs and seven-page mapping;
- exact D055 default normalized controls and default bridge patch behavior;
- the legacy `fmt_8bit_row_param_cache` API/layout and its exact 2048-byte footprint;
- the existing maximum of four p-locks per step;
- domain validation of stored 16-bit p-lock values before publication;
- row-boundary publication with no lock-store traversal in the sample/audio inner loop;
- D049 through D055 synthesis/voice behavior unless the D056 contract explicitly authorizes an adapter seam.

WP5.9 remains out of scope.

No physical FM-1 is available. `hardware_margin_claim=none`.
