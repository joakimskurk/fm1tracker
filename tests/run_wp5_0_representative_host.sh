#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."

OUT=build/wp5_0_representative
mkdir -p "$OUT" build/tests
EXPECTED_REP_WAV=33e819237fb0be16b67371043ba49b69327f31de4dd1b0c44d4914e6a2ef56f9

# Reuse the exact frozen D046 workload/instrumentation. Only remove its historical
# production-freeze assertion, because D048 intentionally changes production.
# The generated copy lives below build/, so replace the original script-relative
# cd with an explicit repository-root cd. No benchmark body is otherwise changed.
python3 - <<'PY'
from pathlib import Path
src = Path('tests/run_wp4_10_representative_host.sh').read_text()
cd_token = 'cd "$(dirname "$0")/.."\n'
assert src.count(cd_token) == 1
src = src.replace(cd_token, 'cd "$(git rev-parse --show-toplevel)"\n', 1)
start_token = 'PRODUCTION=c93c39ad344760b4da0dfbc76829ca5fef886174\n'
end_token = "printf '%s\\n' 'WP4_10_PRODUCTION_FREEZE_OK=1 baseline=c93c39ad344760b4da0dfbc76829ca5fef886174'\n"
assert src.count(start_token) == 1 and src.count(end_token) == 1
start = src.index(start_token)
end = src.index(end_token, start) + len(end_token)
replacement = "printf '%s\\n' 'WP5_0_REPRESENTATIVE_PRODUCTION_MIGRATED_OK=1 baseline=d046 workload=unchanged'\n"
body = src[:start] + replacement + src[end:]
Path('build/tests/run_wp5_0_d046_representative_body.sh').write_text(body)
PY
chmod +x build/tests/run_wp5_0_d046_representative_body.sh

sh build/tests/run_wp5_0_d046_representative_body.sh | tee "$OUT/d046_workload.log"

grep -F 'WP5_0_REPRESENTATIVE_PRODUCTION_MIGRATED_OK=1 baseline=d046 workload=unchanged' "$OUT/d046_workload.log"
grep -F 'WP4_10_FIXTURE_SHAPE_OK=1' "$OUT/d046_workload.log"
grep -F 'WP4_10_REPRESENTATIVE_REPEAT_OK=1 wav=byte_identical state_digest=identical action_alloc_counts=identical' "$OUT/d046_workload.log"
grep -F 'WP4_10_LONG_RUN_REPEAT_OK=1 sample_digest=identical state_digest=identical action_alloc_counts=identical' "$OUT/d046_workload.log"
grep -F 'WP4_10_D033_ALLOC_COUNTS_OK=1 representative_repeat=identical long_repeat=identical active_cross_track_steals=0 allocation_invariants=pass' "$OUT/d046_workload.log"
grep -F 'WP4_10_FULL_POOL_LINKED_OK=1 tracks=8 rows=64 owners=512 locks=2048 engines=4' "$OUT/d046_workload.log"
grep -F 'WP4_10_QUEUE_PRESSURE_OK=1 short_overflow=16 long_overflow=40 recovery=1 tracker_continues=1' "$OUT/d046_workload.log"
grep -F 'WP4_10_LONG_RUN_OK=1 rows=4096 wraps=64 ticks=163840' "$OUT/d046_workload.log"
grep -F 'WP3_10_FOCUSED_WORST_CASE_OK=1' "$OUT/d046_workload.log"

REP_LINE="$(grep '^WP4_10_REPRESENTATIVE_WAV_SHA256=' "$OUT/d046_workload.log" | tail -n 1)"
REP_SHA="$(printf '%s\n' "$REP_LINE" | sed -n 's/^WP4_10_REPRESENTATIVE_WAV_SHA256=\([0-9a-f][0-9a-f]*\) .*/\1/p')"
test "$REP_SHA" = "$EXPECTED_REP_WAV"
printf 'WP5_0_REPRESENTATIVE_WAV_IDENTITY_OK=1 sha256=%s\n' "$REP_SHA" | tee "$OUT/wav_identity.txt"
printf '%s\n' 'WP5_0_REPRESENTATIVE_OK=1 tracks=8 rows=64 locks=2048 long_rows=4096 repeats=identical active_cross_track_steals=0 d046_wav=byte_identical' | tee "$OUT/summary.txt"
