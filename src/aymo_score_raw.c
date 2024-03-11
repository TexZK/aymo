/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023-2024 Andrea Zoppi.

This file is part of AYMO.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2.1 of the License, or (at your option)
any later version.

AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with AYMO. If not, see <https://www.gnu.org/licenses/>.
*/

#include "aymo_score_raw.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_score_vt aymo_score_raw_vt = {
    "aymo_score_raw",
    (aymo_score_ctor_f)aymo_score_raw_ctor,
    (aymo_score_dtor_f)aymo_score_raw_dtor,
    (aymo_score_load_f)aymo_score_raw_load,
    (aymo_score_unload_f)aymo_score_raw_unload,
    (aymo_score_get_status_f)aymo_score_raw_get_status,
    (aymo_score_restart_f)aymo_score_raw_restart,
    (aymo_score_tick_f)aymo_score_raw_tick
};


static void aymo_score_raw_update_clock(
    struct aymo_score_raw_instance* score
)
{
    score->clock += (uint16_t)(score->clock == 0u);
    score->raw_rate = (AYMO_SCORE_RAW_REFCLK / score->clock);
    score->division = (AYMO_SCORE_OPL_RATE_DEFAULT / score->raw_rate);  // TODO: improve resolution via fixed point 24.8
    score->division += (uint32_t)(score->division == 0u);
}


int aymo_score_raw_ctor(
    struct aymo_score_raw_instance* score
)
{
    assert(score);

    score->vt = &aymo_score_raw_vt;

    score->events = NULL;
    score->raw_rate = AYMO_SCORE_RAW_REFCLK;
    score->division = 1u;
    score->length = 0u;
    score->address_hi = 0u;

    aymo_score_raw_restart(score);
    return 0;
}


void aymo_score_raw_dtor(
    struct aymo_score_raw_instance* score
)
{
    AYMO_UNUSED_VAR(score);
    assert(score);
}


int aymo_score_raw_load(
    struct aymo_score_raw_instance* score,
    const void* data,
    uint32_t size
)
{
    assert(score);
    assert(data);
    assert(size);

    if (size < sizeof(struct aymo_score_raw_header)) {
        return 1;
    }
    const uint8_t* ptr = (const uint8_t*)data;

    if (((ptr[0] != 'R') ||
         (ptr[1] != 'A') ||
         (ptr[2] != 'W') ||
         (ptr[3] != 'A') ||
         (ptr[4] != 'D') ||
         (ptr[5] != 'A') ||
         (ptr[6] != 'T') ||
         (ptr[7] != 'A'))) {
        return 1;
    }
    score->clock_initial = *(const uint16_t*)(const void*)&ptr[8];
    score->events = (const struct aymo_score_raw_event*)(const void*)&ptr[10];

    uint32_t length_by_size = (uint32_t)(size - sizeof(struct aymo_score_raw_header));
    length_by_size /= sizeof(struct aymo_score_raw_event);
    if (score->length > length_by_size) {
        score->length = length_by_size;
    }

    aymo_score_raw_restart(score);
    return 0;
}


void aymo_score_raw_unload(
    struct aymo_score_raw_instance* score
)
{
    aymo_score_raw_restart(score);
}


struct aymo_score_status* aymo_score_raw_get_status(
    struct aymo_score_raw_instance* score
)
{
    assert(score);
    return &score->status;
}


void aymo_score_raw_restart(
    struct aymo_score_raw_instance* score
)
{
    assert(score);

    score->index = 0u;
    score->address_hi = 0u;
    score->clock = score->clock_initial;
    aymo_score_raw_update_clock(score);

    score->status.delay = 0u;
    score->status.address = 0u;
    score->status.value = 0u;
    score->status.flags = 0u;

    if (score->index >= score->length) {
        score->status.flags |= AYMO_SCORE_FLAG_EOF;
    }
}


uint32_t aymo_score_raw_tick(
    struct aymo_score_raw_instance* score,
    uint32_t count
)
{
    assert(score);
    assert(!score->length || score->events);

    uint32_t pending = count;

    do {
        if (pending >= score->status.delay) {
            pending -= score->status.delay;
            score->status.delay = 0u;
        }
        else {
            score->status.delay -= pending;
            pending = 0u;
        }

        score->status.address = 0u;
        score->status.value = 0u;
        score->status.flags = 0u;

        if (score->status.delay) {
            score->status.flags = AYMO_SCORE_FLAG_DELAY;
        }
        else if (score->index < score->length) {
            const struct aymo_score_raw_event* event = &score->events[score->index++];

            if (event->ctrl == 0x00u) {
                uint8_t delay = event->data;
                if (delay) {
                    score->status.delay = (delay * score->division);
                    score->status.flags = AYMO_SCORE_FLAG_DELAY;
                }
            }
            else if (event->ctrl == 0x02u) {
                if (event->ctrl == 0x00u) {
                    if ((score->index + 1u) < score->length) {
                        score->index++;
                        score->clock = *(const uint16_t*)(void*)++event;
                        aymo_score_raw_update_clock(score);
                    }
                    else {
                        score->status.flags = AYMO_SCORE_FLAG_EOF;
                        break;
                    }
                }
                else if (event->ctrl == 0x01u) {
                    score->address_hi = 0u;
                }
                else if (event->ctrl == 0x02u) {
                    score->address_hi = 1u;
                }
            }
            else {
                score->status.address = ((uint16_t)(score->address_hi << 8u) | event->ctrl);
                score->status.value = event->data;
                score->status.flags = AYMO_SCORE_FLAG_EVENT;
                count -= pending;  // FIXME: what if another event follows immediately? --> count -= CONSUMED
                break;
            }
        }
        else {
            score->status.flags = AYMO_SCORE_FLAG_EOF;
            break;
        }
    } while (pending);

    return count;
}


AYMO_CXX_EXTERN_C_END
