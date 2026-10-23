/* SPDX-License-Identifier: GPL-3.0-only */
#include "fmt_store.h"

/* D019: one V0.1 editable song lives in the large NOLOAD pool, not ordinary
 * .bss. Keep the symbol externally visible so CI can prove its device address
 * and size from the linked ELF without exposing it in the public header. */
FmtSong fmt_store_storage __attribute__((section(".pool"), aligned(4)));

FMT_STATIC_ASSERT(store_is_one_song, sizeof(fmt_store_storage) == sizeof(FmtSong));

void fmt_store_reset(void)
{
    uint32_t i, pattern, track, step;
    uint8_t *bytes = (uint8_t *)&fmt_store_storage;

    /* Reset every non-step field deterministically first. This intentionally
     * does not rely on linker/startup zeroing so the same operation is safe to
     * call again after edits and behaves identically in host tests. */
    for (i = 0; i < (uint32_t)sizeof(fmt_store_storage); i++)
        bytes[i] = 0u;

    /* V0.1 currently fixes patterns at 64 steps. Other song metadata remains
     * zero until later work packages give those fields runtime behavior. */
    fmt_store_storage.meta.default_pattern_length = (uint8_t)FMT_STEPS_PER_PATTERN;

    /* Zero bytes are not valid empty tracker rows: EMPTY/inherit/no-lock are
     * 0xFF/0xFFFF sentinels. Initialize all 16 * 8 * 64 = 8192 rows explicitly. */
    for (pattern = 0; pattern < FMT_PATTERN_SLOTS; pattern++)
        for (track = 0; track < FMT_TRACKS; track++)
            for (step = 0; step < FMT_STEPS_PER_PATTERN; step++)
                fmt_step_init(&fmt_store_storage.patterns[pattern].steps[track][step]);
}

FmtSong *fmt_store_song(void)
{
    return &fmt_store_storage;
}

const FmtSong *fmt_store_song_const(void)
{
    return &fmt_store_storage;
}

FmtPattern *fmt_store_pattern(uint32_t pattern)
{
    return fmt_song_pattern(&fmt_store_storage, pattern);
}

const FmtPattern *fmt_store_pattern_const(uint32_t pattern)
{
    return fmt_song_pattern_const(&fmt_store_storage, pattern);
}

FmtStep *fmt_store_step(uint32_t pattern, uint32_t track, uint32_t step)
{
    return fmt_song_step(&fmt_store_storage, pattern, track, step);
}

const FmtStep *fmt_store_step_const(uint32_t pattern, uint32_t track, uint32_t step)
{
    return fmt_song_step_const(&fmt_store_storage, pattern, track, step);
}
