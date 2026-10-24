/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "fmt_tracker_effect.h"
#include "fmt_transport.h"

int main(void)
{
    static const uint32_t expected_frames[5] = {919u, 1838u, 2757u, 3675u, 4594u};
    FmtTransport transport;
    FmtTrackerEffectState effect;
    uint32_t frame;
    uint8_t seen = 0u;

    assert(FMT_TRANSPORT_PHASE_PER_STEP == 661500u);
    assert(FMT_TRACKER_EFFECT_TICK_PHASE == 110250u);
    assert(FMT_TRACKER_EFFECT_TICK_PHASE * FMT_TRACKER_EFFECT_TICKS_PER_ROW ==
           FMT_TRANSPORT_PHASE_PER_STEP);
    assert(FMT_TRANSPORT_BPM_DEFAULT == 120u);

    fmt_transport_reset(&transport);
    fmt_tracker_effect_state_reset(&effect);
    fmt_tracker_effect_begin_row(&effect,
                                 FMT_PLAYBACK_EVENT_NOTE,
                                 60u,
                                 fmt_tracker_effect_pack(FMT_TRACKER_EFFECT_ARPEGGIO,
                                                         0x47u,
                                                         0u));
    fmt_transport_start(&transport);

    /* D021/D032 ordering: render frame first, advance D020 at frame end, then
       advance at most one effect tick if no row boundary occurred. The first
       frame that hears the new effect tick is therefore frame+1 below. */
    for (frame = 0u; frame < 5513u; frame++) {
        uint8_t boundary = fmt_transport_tick_frame(&transport);
        if (boundary) {
            assert(frame == 5512u);
            assert(transport.step == 1u);
            assert(effect.tick == 5u); /* row boundary wins; there is no tick 6 */
            continue;
        }
        if (fmt_tracker_effect_tick_due(&effect, transport.phase)) {
            assert(seen < 5u);
            assert(fmt_tracker_effect_advance_tick(&effect));
            assert(frame + 1u == expected_frames[seen]);
            assert(effect.tick == (uint8_t)(seen + 1u));
            seen++;
        }
    }

    assert(seen == 5u);
    assert(transport.step == 1u);
    assert(transport.phase == 60u); /* frame 5512 crossed row boundary by 60 phase units */

    printf("WP3_6_TICK_GRID_TESTS_OK=1\n");
    printf("bpm=120 tick_frames=919,1838,2757,3675,4594 next_row_frame=5513 row_boundary_wins=1 max_one_tick_per_frame=1\n");
    return 0;
}
