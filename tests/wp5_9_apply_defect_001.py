#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
from pathlib import Path

path = Path("firmware/src/dinghy.c")
text = path.read_text()

old = '''    tracker_fm_row_patch_valid[track] =
        (uint8_t)tracker_fm_snapshot_row(track, event->step, instrument);
'''
new = '''    {
        uint8_t fm_snapshot_valid =
            (uint8_t)tracker_fm_snapshot_row(track, event->step, instrument);

        /* WP5.9 Defect 001: NOTE_DELAY keeps the old route alive until its
         * scheduled tick. If that route is FM and the future instrument cannot
         * publish an FM snapshot, keep the already-valid current FM patch until
         * the delayed NOTE goes through the normal replacement path. */
        if (fm_snapshot_valid ||
            !row_event->pending_valid ||
            fmt_engine_router_family(&fmt_engine_router_runtime,
                                     (uint8_t)track) != FMT_ENGINE_FAMILY_FM)
            tracker_fm_row_patch_valid[track] = fm_snapshot_valid;
    }
'''

if text.count(old) != 1:
    raise SystemExit("expected exactly one pre-fix D056 snapshot assignment")
if "WP5.9 Defect 001" in text:
    raise SystemExit("defect fix already appears to be applied")

updated = text.replace(old, new, 1)
path.write_text(updated)
print("WP5_9_DEFECT_001_PATCH_APPLIED=1 source=firmware/src/dinghy.c state_growth=0")
