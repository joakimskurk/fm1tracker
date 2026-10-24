#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build

PHASE4_START=d14955f23a11044e8a348884feca24f8978fd700
PHASE3_PRODUCTION=354bf7d356774ad828f1f660bd456dbab7f0490d

# WP4.1 is architecture-only. No production firmware/platform/build source may
# differ from the clean Phase 4 branch start.
git diff --exit-code "$PHASE4_START" -- \
    firmware/src firmware/hal firmware/loader firmware/app.ld build.sh tools
echo "WP4_1_PRODUCTION_IDENTICAL_OK=1 baseline=$PHASE4_START"

# D037 must be frozen in the durable repository sources before any oscillator
# implementation starts.
grep -F '## D037 — Phase 4 uses one bounded eight-voice 8-BIT engine interface' docs/DECISIONS.md
grep -F 'Status: **FROZEN BY D037 BEFORE PRODUCTION ENGINE SOURCE**' docs/WP4_1_ENGINE_CONTRACT.md
grep -F 'engine 1  PULSE' docs/WP4_1_ENGINE_CONTRACT.md
grep -F 'engine 2  NOISE' docs/WP4_1_ENGINE_CONTRACT.md
grep -F 'engine 3  TRIANGLE' docs/WP4_1_ENGINE_CONTRACT.md
grep -F 'engine 4  WAVETABLE-8' docs/WP4_1_ENGINE_CONTRACT.md
grep -F 'fixed maximum of **eight engine voice slots**' docs/WP4_1_ENGINE_CONTRACT.md
grep -F '0..16383' docs/WP4_1_ENGINE_CONTRACT.md
grep -F '8 tracks * 64 rows * 4 bytes = 2048 bytes ordinary RAM' docs/WP4_1_ENGINE_CONTRACT.md
grep -F '| app image | +65,536 B | 132,496 B |' docs/WP4_1_ENGINE_CONTRACT.md
grep -F '| normal RAM | +8,192 B | 21,488 B |' docs/WP4_1_ENGINE_CONTRACT.md
grep -F '| pool RAM | +16,384 B | 129,384 B |' docs/WP4_1_ENGINE_CONTRACT.md
grep -F 'D037 FROZEN; WP4.1 verification/closeout next' docs/PROJECT_STATE.md
grep -F 'D037 FROZEN; verification/closeout next; no production engine DSP' docs/WORK_PACKAGES.md
grep -F 'WP4.1 D037 FROZEN; verification/closeout next; no production engine DSP' docs/ROADMAP.md
echo 'WP4_1_D037_REPOSITORY_FREEZE_OK=1 voices=8 pitch=q7 params_cache_max=2048'

# A docs-only package must not have introduced an 8-BIT production translation
# unit or hidden oscillator implementation. Production identity above is the
# authoritative proof; these names make accidental early source especially clear.
for f in firmware/src/fmt_8bit_engine.c firmware/src/fmt_8bit_engine.h \
         firmware/src/fmt_pulse.c firmware/src/fmt_noise.c \
         firmware/src/fmt_triangle.c firmware/src/fmt_wavetable8.c; do
    test ! -e "$f"
done
echo 'WP4_1_NO_ENGINE_SOURCE_OK=1'

# Re-run the complete frozen Phase 3 exit gate. Because WP4.1 production is
# byte-identical to both the Phase 4 start and the final Phase 3 production tree,
# this re-proves all inherited deterministic behavior plus the pinned target build.
sh tests/run_wp3_10_closeout_ci.sh > build/wp4_1_inherited_phase3.log
cat build/wp4_1_inherited_phase3.log
grep -F 'WP3_10_PHASE3_EXIT_OK=1 wp3_1_through_wp3_10=complete host_device_build=1 physical_hardware_pending=1' build/wp4_1_inherited_phase3.log
grep -F 'WP3_10_CLOSEOUT_OK=1' build/wp4_1_inherited_phase3.log

# The target artifacts must remain the frozen Phase 3 production artifacts.
BIN_SHA="$(sha256sum build/fm1tracker.bin | awk '{print $1}')"
FWSC_SHA="$(sha256sum build/fm1tracker-0.1.fwsc | awk '{print $1}')"
test "$BIN_SHA" = 'b2fc3af11c772233134a0473d6046877f00009d3d1b1f7907f7284a60d7bfe27'
test "$FWSC_SHA" = '7870d11b046433a46052865a724186219fecf34ebb566c50f8f173b599b5257c'

git diff --exit-code "$PHASE3_PRODUCTION" -- \
    firmware/src firmware/hal firmware/loader firmware/app.ld build.sh tools

printf '%s\n' \
    'WP4_1_FINAL_RESOURCES image=66960 normal_ram=13296 pool_ram=113000 production_delta=0' \
    "WP4_1_FINAL_BIN_SHA256=$BIN_SHA" \
    "WP4_1_FINAL_FWSC_SHA256=$FWSC_SHA" \
    'WP4_1_D037_EXIT_OK=1 architecture_frozen=1 production_engine_source=0 inherited_phase3=1 physical_hardware_pending=1' \
    'WP4_1_CLOSEOUT_OK=1' \
    | tee build/wp4_1_final_summary.txt
