#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/wp5_0_exit

# Historical contract-first evidence must remain durable in the repository.
grep -F 'Status: **PASS - production migration is now allowed under D048**' docs/WP5_0_PREMIGRATION_EVIDENCE.md
grep -F 'run: `40126003645`' docs/WP5_0_PREMIGRATION_EVIDENCE.md
grep -F 'WP5_0_PREMIGRATION_GATE_REPRODUCED=1 valid_8bit=1 song_mode_only_change=blocked instrument_gate=present' docs/WP5_0_PREMIGRATION_EVIDENCE.md
grep -F 'WP5_0_PREMIGRATION_OK=1 production=frozen gate=reproduced router_model=frozen shared_cache=2048 fm_dsp=absent' docs/WP5_0_PREMIGRATION_EVIDENCE.md
printf '%s\n' 'WP5_0_EXIT_PREMIGRATION_EVIDENCE_OK=1 contract_first=proven run=40126003645'

# Pure common router/publication state and test-only mixed-family model.
sh tests/run_wp5_0_router_host.sh | tee build/wp5_0_exit/router_core.log
grep -F 'WP5_0_ROUTER_SIZE_OK=1 bytes=8' build/wp5_0_exit/router_core.log
grep -F 'WP5_0_IDENTITY_PUBLICATION_OK=1 instruments=16 bytes=64' build/wp5_0_exit/router_core.log
grep -F 'WP5_0_ROUTER_CORE_OK=1 tracks=8 families=2 unknown_family=safe' build/wp5_0_exit/router_core.log
grep -F 'WP5_0_SONG_MODE_NONGATING_PUBLICATION_OK=1 mode0_mode1_identity_words=identical' build/wp5_0_exit/router_core.log

# Production-path family semantics, including immediate/delayed cross-family
# replacement, malformed family/engine fail-to-silence and non-applicable lock.
sh tests/run_wp5_0_family_switch_host.sh | tee build/wp5_0_exit/family_switch.log
grep -F 'WP5_0_FAMILY_SWITCH_OK=1 cross_family_order=hard_silence_clear_attempt_commit_on_success delayed_note=same_router malformed=safe nonapplicable_plock=ignored fm_dsp=absent active_cross_track_steals=0' build/wp5_0_exit/family_switch.log

# Exact D046 worst-case workload carried through the D048 production source.
sh tests/run_wp5_0_representative_host.sh | tee build/wp5_0_exit/representative.log
grep -F 'WP5_0_REPRESENTATIVE_WAV_IDENTITY_OK=1 sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9' build/wp5_0_exit/representative.log
grep -F 'WP5_0_REPRESENTATIVE_OK=1 tracks=8 rows=64 locks=2048 long_rows=4096 repeats=identical active_cross_track_steals=0 d046_wav=byte_identical' build/wp5_0_exit/representative.log

# Structural callback/ownership audit for the new router plus inherited bounds.
sh tests/run_wp5_0_realtime_audit.sh | tee build/wp5_0_exit/realtime.log
grep -F 'WP5_0_REALTIME_AUDIT_OK=1 hardware_margin_claim=none' build/wp5_0_exit/realtime.log

# All applicable Phase 1-4 component/context/UI/stress/timing regressions.
sh tests/run_wp5_0_inherited_regressions.sh | tee build/wp5_0_exit/inherited.log
grep -F 'WP5_0_INHERITED_REGRESSIONS_OK=1 phase1_4=applicable contracts=pass obsolete_proof_sine=excluded' build/wp5_0_exit/inherited.log

# Final committed production build: exact D045 audio regressions, actual-app
# song-meta independence and pinned JieLi/AC79 target resource/artifact evidence.
sh tests/run_wp5_0_postmigration.sh | tee build/wp5_0_exit/postmigration.log
grep -F 'WP5_0_POSTMIGRATION_OK=1 production=committed d046_wav=byte_identical song_meta=nongating canonical_8bit=pass target=pinned fm_dsp=absent hardware_margin_claim=none' build/wp5_0_exit/postmigration.log
grep -F 'image_bytes=72792' build/wp5_0_postmigration/metrics.txt
grep -F 'normal_ram_bytes=19676' build/wp5_0_postmigration/metrics.txt
grep -F 'pool_ram_bytes=113000' build/wp5_0_postmigration/metrics.txt
grep -F 'image_delta_vs_d046=288' build/wp5_0_postmigration/metrics.txt
grep -F 'normal_ram_delta_vs_d046=72' build/wp5_0_postmigration/metrics.txt
grep -F 'pool_delta_vs_d046=0' build/wp5_0_postmigration/metrics.txt

BIN_SHA="$(sed -n 's/^fm1tracker_bin_sha256=//p' build/wp5_0_postmigration/metrics.txt | tail -n 1)"
FWSC_SHA="$(sed -n 's/^fm1tracker_fwsc_sha256=//p' build/wp5_0_postmigration/metrics.txt | tail -n 1)"
test -n "$BIN_SHA"
test -n "$FWSC_SHA"

# Final source guards: WP5.0 is routing only and retains one page cache.
if grep -R -E 'fmt_fm|fm_operator|FM4 operator|phase modulation' firmware/src; then
    echo 'FM DSP entered WP5.0 unexpectedly' >&2
    exit 1
fi
CACHE_DEFS="$(grep -R -h '^volatile Fmt8BitRowParamCache fmt_8bit_row_param_cache' firmware/src | wc -l | tr -d ' ')"
test "$CACHE_DEFS" -eq 1
if grep -R -F 'song->meta.mode != FMT_MODE_8BIT' firmware/src/dinghy.c firmware/src/fmt_8bit_linked_params.c; then
    echo 'obsolete project-wide playback gate survived WP5.0' >&2
    exit 1
fi

printf 'WP5_0_EXIT_OK=1 representative_wav_sha256=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9 default_wav_sha256=17fa5ce1759cb728e152d1d3d417a51a13d358908bdcc2dc2e505c27c86a1076 image=72792 ram=19676 pool=113000 bin_sha256=%s fwsc_sha256=%s inherited=pass realtime=pass family_switch=pass song_meta=nongating fm_dsp=absent hardware_margin_claim=none\n' "$BIN_SHA" "$FWSC_SHA" \
    | tee build/wp5_0_exit/summary.txt
