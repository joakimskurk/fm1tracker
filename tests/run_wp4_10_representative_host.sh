#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host build/tests build/wp4_10

PRODUCTION=c93c39ad344760b4da0dfbc76829ca5fef886174

# D046 is verification-only unless this benchmark proves a real defect.
git diff --exit-code "$PRODUCTION" -- \
    firmware/src firmware/hal firmware/loader firmware/app.ld build.sh tools
printf '%s\n' 'WP4_10_PRODUCTION_FREEZE_OK=1 baseline=c93c39ad344760b4da0dfbc76829ca5fef886174'

FIXTURE=build/wp4_10/representative.txt
python3 - <<'PY' > "$FIXTURE"
# Exact D046 8-track / 64-row / full-lock-pool workload.
NOTE_TIE = 0xFD
NOTE_OFF = 0xFE
INHERIT = 0xFF
P_VOICE = 0x01
P_FINE = 0x02
P_ATTACK = 0x20
P_DECAY = 0x21
P_SUSTAIN = 0x22
P_RELEASE = 0x23
P_WIDTH = 0x24
P_TABLE = 0x25
P_TRACK = 0xA0
slot0 = (P_WIDTH, P_ATTACK, P_ATTACK, P_TABLE)
values = (0, 1, 2, 3, 4, 5, 6, 255)

print('1 clear')
for track in range(8):
    for row in range(64):
        phase = row & 7
        engine = (track + row // 16) & 3
        if phase in (1, 5):
            note = NOTE_TIE
            instrument = INHERIT
        elif phase == 3:
            note = NOTE_OFF
            instrument = INHERIT
        else:
            note = 48 + ((track * 5 + (row // 8) * 2 + phase) % 30)
            instrument = engine if row in (0, 16, 32, 48) else INHERIT
        command = (row + track) % 9
        value = values[track]
        print(f'1 step {track} {row} {note} {instrument} {command} {value}')

# Exactly 512 owners * 4 records = the complete 2,048-record D028 pool.
for track in range(8):
    for row in range(64):
        engine = (track + row // 16) & 3
        s0 = slot0[engine]
        if row & 1:
            ids = (s0, P_DECAY, P_SUSTAIN, P_RELEASE)
        else:
            ids = (P_VOICE, P_FINE, P_TRACK, s0)
        for ordinal, param in enumerate(ids):
            value = (track * 37 + row * 13 + ordinal * 71) & 0xFF
            # Force deterministic boundary values throughout the matrix.
            marker = (track + row + ordinal) % 19
            if marker == 0:
                value = 0
            elif marker == 1:
                value = 128
            elif marker == 2:
                value = 255
            # OFF rows must retain detached tails into the following NOTE row.
            if (row & 7) == 3 and param == P_RELEASE:
                value = 255
            print(f'1 lock {track} {row} {param} {value}')

print('1 probe')

# Concurrent D034 pressure while the linked tracker is running. Each epoch sends
# 40 NOTE_ON events into the 32-entry queue in one main-loop poll: 32 accepted,
# 8 reject-newest overflows. A later NOTE_ON/OFF pair proves recovery.
for t in (1000, 4000, 10000, 100000, 300000):
    for i in range(40):
        print(f'{t} midi 0x90 {40 + (i % 27)} {1 + (i % 127)}')
    print(f'{t + 50} midi 0x90 72 100')
    print(f'{t + 50} midi 0x80 72 0')
    print(f'{t + 100} probe')

# Safety end only; benchmark row target exits first in canonical modes.
print('600000 end')
PY

events="$(grep -vc '^[[:space:]]*$' "$FIXTURE")"
test "$events" -lt 4096
locks="$(grep -c ' lock ' "$FIXTURE")"
steps="$(grep -c ' step ' "$FIXTURE")"
test "$locks" -eq 2048
test "$steps" -eq 512
printf 'WP4_10_FIXTURE_SHAPE_OK=1 events=%s steps=%s locks=%s owners=512\n' "$events" "$steps" "$locks"

# D046 requires exact D033 action counts, allocator outcome counts and an explicit
# zero-active-cross-track-steal proof. Keep production byte-identical by building
# a byte-for-byte dinghy.c copy plus generated observation wrappers around the
# real D033 evaluator and D043 allocator acquire function. The raw function
# bodies are copied verbatim from the frozen production sources.
python3 - <<'PY'
from pathlib import Path

out = Path('build/tests')
out.mkdir(parents=True, exist_ok=True)

dinghy = Path('firmware/src/dinghy.c').read_text()
(out / 'dinghy_wp4_10.c').write_text(dinghy)

alloc = Path('firmware/src/fmt_voice_allocator.c').read_text()
needle = 'FmtVoiceAllocation fmt_voice_allocator_acquire('
assert alloc.count(needle) == 1, 'allocator acquire definition shape changed'
alloc = alloc.replace(needle, 'static FmtVoiceAllocation wp4_10_fmt_voice_allocator_acquire_raw(', 1)
alloc += r'''

FmtVoiceAllocation fmt_voice_allocator_acquire(FmtVoiceAllocator *allocator,
                                               uint8_t track,
                                               const uint8_t stage[FMT_VOICE_ALLOCATOR_SLOTS])
{
    FmtVoiceAllocation result;
    uint8_t owner_before[FMT_VOICE_ALLOCATOR_SLOTS];
    uint32_t i;

    for (i = 0u; i < FMT_VOICE_ALLOCATOR_SLOTS; i++)
        owner_before[i] = allocator ? allocator->voice_owner[i] : FMT_TRACK_NONE;
    result = wp4_10_fmt_voice_allocator_acquire_raw(allocator, track, stage);
    return wp4_10_record_allocation(track, owner_before, stage, result);
}
'''
(out / 'fmt_voice_allocator.c').write_text(alloc)

event = Path('firmware/src/fmt_tracker_event.c').read_text()
needle = 'uint8_t fmt_tracker_row_event_action('
assert event.count(needle) == 1, 'tracker action definition shape changed'
event = event.replace(needle, 'static uint8_t wp4_10_fmt_tracker_row_event_action_raw(', 1)
event += r'''

uint8_t fmt_tracker_row_event_action(FmtTrackerRowEventState *state,
                                     uint8_t tick,
                                     uint8_t logical_active,
                                     uint8_t *note_out)
{
    uint8_t action = wp4_10_fmt_tracker_row_event_action_raw(
        state, tick, logical_active, note_out);
    return wp4_10_record_action(action);
}
'''
(out / 'fmt_tracker_event.c').write_text(event)

assert (out / 'dinghy_wp4_10.c').read_text() == Path('firmware/src/dinghy.c').read_text()
print('WP4_10_INSTRUMENTED_LINKED_SOURCE_OK=1 dinghy_copy=exact allocator_wrapper=1 tracker_event_wrapper=1 production_mutation=0')
PY

# Build the real full app through the ordinary DINGHY_HOST path, with only the
# generated observation wrappers above interposed. The wrapper header records
# results but never mutates tracker/engine/event/allocator state.
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST \
    -DAPP_NAME='"FM1TRACKER"' -DAPP_VER='"wp4.10"' \
    -include tests/wp4_10_instrument.h \
    -Ibuild/tests -Ifirmware/src -Ifirmware/gen \
    -o build/host/fm1tracker_wp4_10 build/tests/dinghy_wp4_10.c

# One complete 64-row pattern with full WAV evidence.
FM1TRACKER_HOST_BENCH_ROWS=64 \
    build/host/fm1tracker_wp4_10 build/host/wp4_10_representative "$FIXTURE" \
    > build/wp4_10/representative.log
cat build/wp4_10/representative.log
REP="$(grep 'D046_HOST_BENCH rows=64 ' build/wp4_10/representative.log | tail -n 1)"
REP_COUNTS="$(grep '^WP4_10_D046_COUNTS ' build/wp4_10/representative.log | tail -n 1)"
test -n "$REP"
test -n "$REP_COUNTS"
printf '%s\n' "$REP" | grep -F 'ticks=2560 '
printf '%s\n' "$REP" | grep -F 'lock_used=2048 lock_free=0 queue_depth=0 '
printf '%s\n' "$REP_COUNTS" | grep -F 'active_cross_track_steals=0 alloc_invariant_fail=0'
REP_OVERFLOW="$(printf '%s\n' "$REP" | sed -n 's/.* overflow=\([0-9][0-9]*\) seen=\([0-9][0-9]*\).*/\1 \2/p')"
test "$REP_OVERFLOW" = '16 16'
test -s build/host/wp4_10_representative.wav
REP_WAV="$(sha256sum build/host/wp4_10_representative.wav | awk '{print $1}')"
REP_FRAMES="$(python3 - <<'PY'
import wave
with wave.open('build/host/wp4_10_representative.wav', 'rb') as f:
    print(f.getnframes())
PY
)"
printf 'WP4_10_REPRESENTATIVE_WAV_SHA256=%s frames=%s\n' "$REP_WAV" "$REP_FRAMES"

# Repeat the complete canonical render and require byte-identical WAV, state
# digest and exact D033/D043 observation counts.
FM1TRACKER_HOST_BENCH_ROWS=64 \
    build/host/fm1tracker_wp4_10 build/host/wp4_10_representative_repeat "$FIXTURE" \
    > build/wp4_10/representative_repeat.log
REP_REPEAT="$(grep 'D046_HOST_BENCH rows=64 ' build/wp4_10/representative_repeat.log | tail -n 1)"
REP_COUNTS_REPEAT="$(grep '^WP4_10_D046_COUNTS ' build/wp4_10/representative_repeat.log | tail -n 1)"
test "$REP_REPEAT" = "$REP"
test "$REP_COUNTS_REPEAT" = "$REP_COUNTS"
test -s build/host/wp4_10_representative_repeat.wav
REP_WAV_REPEAT="$(sha256sum build/host/wp4_10_representative_repeat.wav | awk '{print $1}')"
test "$REP_WAV_REPEAT" = "$REP_WAV"
rm -f build/host/wp4_10_representative_repeat.wav
printf '%s\n' 'WP4_10_REPRESENTATIVE_REPEAT_OK=1 wav=byte_identical state_digest=identical action_alloc_counts=identical'

# 4,096 row boundaries / 64 complete wraps, through the same real adapter.
# WAV storage is suppressed to avoid a ~90 MB artifact; every rendered sample is
# still folded into the deterministic FNV stream by tests/host.c.
FM1TRACKER_HOST_BENCH_ROWS=4096 FM1TRACKER_HOST_NO_WAV=1 \
    build/host/fm1tracker_wp4_10 build/host/wp4_10_long "$FIXTURE" \
    > build/wp4_10/long_run.log
cat build/wp4_10/long_run.log
LONG="$(grep 'D046_HOST_BENCH rows=4096 ' build/wp4_10/long_run.log | tail -n 1)"
LONG_COUNTS="$(grep '^WP4_10_D046_COUNTS ' build/wp4_10/long_run.log | tail -n 1)"
test -n "$LONG"
test -n "$LONG_COUNTS"
printf '%s\n' "$LONG" | grep -F 'ticks=163840 '
printf '%s\n' "$LONG" | grep -F 'lock_used=2048 lock_free=0 queue_depth=0 '
printf '%s\n' "$LONG_COUNTS" | grep -F 'active_cross_track_steals=0 alloc_invariant_fail=0'
LONG_OVERFLOW="$(printf '%s\n' "$LONG" | sed -n 's/.* overflow=\([0-9][0-9]*\) seen=\([0-9][0-9]*\).*/\1 \2/p')"
test "$LONG_OVERFLOW" = '40 40'
printf '%s\n' "$LONG" | grep -F 'step=0 phase=0'

# Repeat the entire 4,096-row linked run. Both the final state/sample digest and
# every D033/D043 count must match exactly.
FM1TRACKER_HOST_BENCH_ROWS=4096 FM1TRACKER_HOST_NO_WAV=1 \
    build/host/fm1tracker_wp4_10 build/host/wp4_10_long_repeat "$FIXTURE" \
    > build/wp4_10/long_run_repeat.log
LONG_REPEAT="$(grep 'D046_HOST_BENCH rows=4096 ' build/wp4_10/long_run_repeat.log | tail -n 1)"
LONG_COUNTS_REPEAT="$(grep '^WP4_10_D046_COUNTS ' build/wp4_10/long_run_repeat.log | tail -n 1)"
test "$LONG_REPEAT" = "$LONG"
test "$LONG_COUNTS_REPEAT" = "$LONG_COUNTS"
printf '%s\n' 'WP4_10_LONG_RUN_REPEAT_OK=1 sample_digest=identical state_digest=identical action_alloc_counts=identical'

# D036's independent full-pool and queue-wrap tests remain authoritative and
# complement the real-adapter D046 workload above.
sh tests/run_wp3_10_worst_case.sh > build/wp4_10/inherited_worst_case.log
cat build/wp4_10/inherited_worst_case.log
grep -F 'WP3_10_LOCK_POOL_STRESS_OK=1 records=2048 owners=512' build/wp4_10/inherited_worst_case.log
grep -F 'WP3_10_QUEUE_STRESS_OK=1 cycles=2048 rejected=6144' build/wp4_10/inherited_worst_case.log
grep -F 'WP3_10_FOCUSED_WORST_CASE_OK=1' build/wp4_10/inherited_worst_case.log

printf '%s\n' "$REP_COUNTS"
printf '%s\n' "$LONG_COUNTS"
printf '%s\n' 'WP4_10_D033_ALLOC_COUNTS_OK=1 representative_repeat=identical long_repeat=identical active_cross_track_steals=0 allocation_invariants=pass'
printf '%s\n' 'WP4_10_FULL_POOL_LINKED_OK=1 tracks=8 rows=64 owners=512 locks=2048 engines=4'
printf '%s\n' 'WP4_10_QUEUE_PRESSURE_OK=1 short_overflow=16 long_overflow=40 recovery=1 tracker_continues=1'
printf 'WP4_10_LONG_RUN_OK=1 rows=4096 wraps=64 ticks=163840 representative_wav=%s\n' "$REP_WAV"
