/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef FM1TRACKER_WP4_10_INSTRUMENT_H
#define FM1TRACKER_WP4_10_INSTRUMENT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt_envelope.h"
#include "fmt_tracker_event.h"
#include "fmt_voice_allocator.h"

/*
 * D046 host-only observation hooks.
 *
 * The representative benchmark compiles byte-for-byte copies of the production
 * translation-unit entry point plus generated wrappers around exactly two
 * functions: the D033 action evaluator and D043 allocator acquire. Production
 * sources remain untouched. These counters are evidence only and never feed
 * back into tracker, engine, allocator or event state.
 */
static uint32_t wp4_10_action_count[4];
static uint32_t wp4_10_alloc_count[4];
static uint32_t wp4_10_active_cross_track_steals;
static uint32_t wp4_10_alloc_invariant_fail;
static uint8_t wp4_10_report_registered;

static void wp4_10_instrument_report(void)
{
    printf("WP4_10_D046_COUNTS actions_none=%u actions_delay=%u actions_cut=%u actions_retrig=%u "
           "alloc_none=%u alloc_reuse=%u alloc_fresh_idle=%u alloc_steal_tail=%u "
           "active_cross_track_steals=%u alloc_invariant_fail=%u\n",
           (unsigned)wp4_10_action_count[FMT_TRACKER_ROW_ACTION_NONE],
           (unsigned)wp4_10_action_count[FMT_TRACKER_ROW_ACTION_DELAYED_NOTE],
           (unsigned)wp4_10_action_count[FMT_TRACKER_ROW_ACTION_CUT],
           (unsigned)wp4_10_action_count[FMT_TRACKER_ROW_ACTION_RETRIG],
           (unsigned)wp4_10_alloc_count[FMT_VOICE_ALLOC_NONE],
           (unsigned)wp4_10_alloc_count[FMT_VOICE_ALLOC_REUSE],
           (unsigned)wp4_10_alloc_count[FMT_VOICE_ALLOC_FRESH_IDLE],
           (unsigned)wp4_10_alloc_count[FMT_VOICE_ALLOC_STEAL_TAIL],
           (unsigned)wp4_10_active_cross_track_steals,
           (unsigned)wp4_10_alloc_invariant_fail);
}

static void wp4_10_instrument_register(void)
{
    if (!wp4_10_report_registered) {
        wp4_10_report_registered = 1u;
        if (atexit(wp4_10_instrument_report) != 0) {
            fprintf(stderr, "D046 instrumentation: atexit registration failed\n");
            exit(4);
        }
    }
}

static uint8_t wp4_10_record_action(uint8_t action)
{
    wp4_10_instrument_register();
    if (action <= FMT_TRACKER_ROW_ACTION_RETRIG)
        wp4_10_action_count[action]++;
    else
        wp4_10_alloc_invariant_fail++;
    return action;
}

static FmtVoiceAllocation wp4_10_record_allocation(
    uint8_t track,
    const uint8_t owner_before[FMT_VOICE_ALLOCATOR_SLOTS],
    const uint8_t stage_before[FMT_VOICE_ALLOCATOR_SLOTS],
    FmtVoiceAllocation result)
{
    uint8_t before = FMT_TRACK_NONE;
    uint8_t stage = 0xFFu;

    wp4_10_instrument_register();
    if (result.kind <= FMT_VOICE_ALLOC_STEAL_TAIL)
        wp4_10_alloc_count[result.kind]++;
    else
        wp4_10_alloc_invariant_fail++;

    if (result.voice != FMT_VOICE_NONE && result.voice < FMT_VOICE_ALLOCATOR_SLOTS) {
        before = owner_before[result.voice];
        if (stage_before)
            stage = stage_before[result.voice];
        if (before != FMT_TRACK_NONE && before != track)
            wp4_10_active_cross_track_steals++;
    }

    switch (result.kind) {
    case FMT_VOICE_ALLOC_NONE:
        if (result.voice != FMT_VOICE_NONE)
            wp4_10_alloc_invariant_fail++;
        break;
    case FMT_VOICE_ALLOC_REUSE:
        if (result.voice == FMT_VOICE_NONE || before != track)
            wp4_10_alloc_invariant_fail++;
        break;
    case FMT_VOICE_ALLOC_FRESH_IDLE:
        if (result.voice == FMT_VOICE_NONE || before != FMT_TRACK_NONE ||
            stage != FMT_ENVELOPE_IDLE)
            wp4_10_alloc_invariant_fail++;
        break;
    case FMT_VOICE_ALLOC_STEAL_TAIL:
        if (result.voice == FMT_VOICE_NONE || before != FMT_TRACK_NONE ||
            stage != FMT_ENVELOPE_RELEASE)
            wp4_10_alloc_invariant_fail++;
        break;
    default:
        break;
    }

    return result;
}

#endif /* FM1TRACKER_WP4_10_INSTRUMENT_H */
