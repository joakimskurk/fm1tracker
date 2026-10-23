/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_pattern_input.h"

/* FM-1 note-key indexes 0..26 span F3..G5. The white keys in physical order
 * are 0,2,4,6,7,9,11,12,14,16,18,19,21,23,24,26. -1 means not a step key. */
static const int8_t fmt_pattern_input_local_for_key[FMT_PATTERN_INPUT_KEY_COUNT] = {
     0, -1,  1, -1,  2, -1,  3,  4, -1,
     5, -1,  6,  7, -1,  8, -1,  9, -1,
    10, 11, -1, 12, -1, 13, 14, -1, 15
};

/* D023: first eight physical black keys select logical tracks 0..7. The final
 * three black keys stay reserved. -1 means not a WP2.6 track selector. */
static const int8_t fmt_pattern_input_track_for_key[FMT_PATTERN_INPUT_KEY_COUNT] = {
    -1,  0, -1,  1, -1,  2, -1, -1,  3,
    -1,  4, -1, -1,  5, -1,  6, -1,  7,
    -1, -1, -1, -1, -1, -1, -1, -1, -1
};

void fmt_pattern_input_reset(FmtPatternInput *input)
{
    if (!input)
        return;
    input->context = FMT_PATTERN_CONTEXT_PERFORMANCE;
    input->page = 0u;
    input->local_step = 0u;
    input->selected_track = 0u;
}

void fmt_pattern_input_enter_pattern(FmtPatternInput *input)
{
    if (input)
        input->context = FMT_PATTERN_CONTEXT_PATTERN;
}

void fmt_pattern_input_enter_performance(FmtPatternInput *input)
{
    if (input)
        input->context = FMT_PATTERN_CONTEXT_PERFORMANCE;
}

int fmt_pattern_input_is_pattern(const FmtPatternInput *input)
{
    return input && input->context == FMT_PATTERN_CONTEXT_PATTERN;
}

int fmt_pattern_input_key_to_local(uint32_t key_index, uint8_t *local_step)
{
    int8_t mapped;

    if (local_step)
        *local_step = 0u;
    if (key_index >= FMT_PATTERN_INPUT_KEY_COUNT)
        return 0;

    mapped = fmt_pattern_input_local_for_key[key_index];
    if (mapped < 0)
        return 0;

    if (local_step)
        *local_step = (uint8_t)mapped;
    return 1;
}

int fmt_pattern_input_key_to_track(uint32_t key_index, uint8_t *track)
{
    int8_t mapped;

    if (track)
        *track = 0u;
    if (key_index >= FMT_PATTERN_INPUT_KEY_COUNT)
        return 0;

    mapped = fmt_pattern_input_track_for_key[key_index];
    if (mapped < 0)
        return 0;

    if (track)
        *track = (uint8_t)mapped;
    return 1;
}

int fmt_pattern_input_select_key(FmtPatternInput *input, uint32_t key_index)
{
    uint8_t local;

    if (!fmt_pattern_input_is_pattern(input))
        return 0;
    if (!fmt_pattern_input_key_to_local(key_index, &local))
        return 0;

    input->local_step = local;
    return 1;
}

int fmt_pattern_input_select_track_key(FmtPatternInput *input, uint32_t key_index)
{
    uint8_t track;

    if (!fmt_pattern_input_is_pattern(input))
        return 0;
    if (!fmt_pattern_input_key_to_track(key_index, &track))
        return 0;

    input->selected_track = track;
    return 1;
}

int fmt_pattern_input_page_prev(FmtPatternInput *input)
{
    if (!fmt_pattern_input_is_pattern(input) || input->page == 0u)
        return 0;
    input->page--;
    return 1;
}

int fmt_pattern_input_page_next(FmtPatternInput *input)
{
    if (!fmt_pattern_input_is_pattern(input) || input->page + 1u >= FMT_PATTERN_INPUT_PAGE_COUNT)
        return 0;
    input->page++;
    return 1;
}

int fmt_pattern_input_advance_step(FmtPatternInput *input)
{
    if (!fmt_pattern_input_is_pattern(input) ||
        input->page >= FMT_PATTERN_INPUT_PAGE_COUNT ||
        input->local_step >= FMT_PATTERN_INPUT_STEPS_PER_PAGE ||
        input->selected_track >= FMT_PATTERN_INPUT_TRACK_COUNT)
        return 0;

    input->local_step++;
    if (input->local_step >= FMT_PATTERN_INPUT_STEPS_PER_PAGE) {
        input->local_step = 0u;
        input->page++;
        if (input->page >= FMT_PATTERN_INPUT_PAGE_COUNT)
            input->page = 0u;
    }
    return 1;
}

uint8_t fmt_pattern_input_global_step(const FmtPatternInput *input)
{
    uint32_t page, local;

    if (!input)
        return 0u;
    page = input->page < FMT_PATTERN_INPUT_PAGE_COUNT ? input->page : 0u;
    local = input->local_step < FMT_PATTERN_INPUT_STEPS_PER_PAGE ? input->local_step : 0u;
    return (uint8_t)(page * FMT_PATTERN_INPUT_STEPS_PER_PAGE + local);
}
