# WP5.7 / D055 Verification

Date: 2026-10-25

Package: **WP5.7 - FM4 voice allocation and D048 router integration**  
Decision: **D055**  
Branch: `phase5/fm4-voice-integration`

## Result

**TECHNICALLY COMPLETE.** The D049-D054 FM4 primitives are integrated into a bounded eight-slot family-local FM engine and routed through D048 without changing the accepted all-8BIT output or permitting active cross-track primary stealing.

Canonical implementation head:

```text
75ff8136881799ad652df380a53002c5efa70516
```

Canonical implementation verification:

```text
run     40173076046  SUCCESS
host    121107852886 SUCCESS
target  121107852943 SUCCESS
```

Artifacts:

```text
host    12590982004  sha256:156762cdc26b3400d0060530b05be42f660302d35f2322456a22a3e3f38f8f53
target  12590199901  sha256:765ab8937782dd21854eb0609f2c897e2542fd5f0595c374dfbf7e6ebe13c667
```

## D055 architecture verified

```text
FM slots                     8
FmtFmVoice                  92 B
FmtFmEngine                788 B
allocator policy            exact D043
active cross-track steals     0
release tails                detached and reusable
mutable FM parameter bank      0 B in WP5.7
```

The allocator semantics remain: reuse own primary, otherwise lowest idle, otherwise oldest detached RELEASE tail, never another track's active primary.

NOTE performs hard FM retrigger. RETRIG hard-resets the active FM voice. OFF/CUT detach ownership and enter deterministic release. Malformed state fails to safe silence in the focused engine fixture.

## D050 compatibility seam

The single backward-compatible externally-rendered-OP4 composition seam is verified across all four algorithms for 4,096 frames each. OP4 still advances exactly once per rendered sample. Existing D050 behavior remains equivalent.

## D048 family integration

The production `dinghy.c` path now dispatches FM4 through the existing D048 family router. The test fixture performs a real same-track family sequence:

```text
FM4 -> 8-BIT -> FM4
```

Two complete renders are byte-identical:

```text
mixed WAV sha256  e7bef408b3ce8c40e5b51cc6489dc39068ade82a86ccad0297070a3d2f2df034
frames            286464
FM window A        71999 nonzero frames
8-BIT window       72000 nonzero frames
FM window B        75997 nonzero frames
```

## Exact all-8BIT regression

Default production startup remains all-8BIT and is exactly WP5.0:

```text
frames  286464
sha256  17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076
```

Inherited D049-D054 and representative D046/WP5.0 gates remain green.

## Source and realtime boundary

Exactly five production files changed relative to the frozen pre-integration base:

```text
firmware/src/dinghy.c
firmware/src/fmt_fm_algorithm.c
firmware/src/fmt_fm_algorithm.h
firmware/src/fmt_fm_engine.c
firmware/src/fmt_fm_engine.h
```

D043, D049, D051, D052, D053, D054 and generated tables remain unchanged.

Static audit:

```text
fixed FM slots                  8
family render calls/sample      1
heap                             0
float                            0
int64 realtime use               0
runtime division/modulo          0
store/editor cache reads         0
D050 OP4 seam                    1
```

## Pinned PI32v2 / full-target evidence

PI32v2 objects:

```text
fmt_fm_engine text+rodata       1284 B
fmt_fm_algorithm text+rodata     380 B
combined                        1664 B
fmt_fm_engine.o sha256          bde92b1ddd517e4d829c9db3a6301dedecb099e4563eb3fef5500ed09445ae91
fmt_fm_algorithm.o sha256       276058c58da067327e56ddce29b4073b2109e9b8548cd515fe96d7abab7edd85
```

Full application:

```text
image             77048 B   (+4256 B vs WP5.0; ceiling +8192 B)
normal RAM        20464 B    (+788 B vs WP5.0; ceiling +1024 B)
pool RAM         113000 B    (unchanged)
BIN sha256        3f19bc0bbf9a4b6fbc19f2c62dc578083f42ddf3e600b7504b8ad4d82b9308f6
FWSC sha256       be0b399493134e27a6b8f36948496ec1c11a30fa5e06ed383b0f1951c06c8305
```

All frozen D055 resource ceilings pass.

## Exit state

The implementation exit criteria are satisfied. This documentation checkpoint intentionally triggers the same WP5.7 host+target workflow one final time on the closeout head. Issue #50 may close only after that documentation-head run is green.

WP5.8 owns mutable FM parameter pages and p-lock mapping. No WP5.8 production changes belong on this branch.

No physical FM-1 is available. `hardware_margin_claim=none`.
