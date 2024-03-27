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

#include "aymo_score_avd.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_score_vt aymo_score_avd_vt = {
    "aymo_score_avd",
    (aymo_score_ctor_f)aymo_score_avd_ctor,
    (aymo_score_dtor_f)aymo_score_avd_dtor,
    (aymo_score_load_f)aymo_score_avd_load,
    (aymo_score_unload_f)aymo_score_avd_unload,
    (aymo_score_get_status_f)aymo_score_avd_get_status,
    (aymo_score_restart_f)aymo_score_avd_restart,
    (aymo_score_tick_f)aymo_score_avd_tick
};


int aymo_score_avd_ctor(
    struct aymo_score_avd_instance* score
)
{
    assert(score);

    score->vt = &aymo_score_avd_vt;

    score->events = NULL;
    score->length = 0u;
    aymo_score_avd_restart(score);
    return 0;
}


void aymo_score_avd_dtor(
    struct aymo_score_avd_instance* score
)
{
    AYMO_UNUSED_VAR(score);
    assert(score);
}


int aymo_score_avd_load(
    struct aymo_score_avd_instance* score,
    const void* data,
    uint32_t size
)
{
    assert(score);

    uint32_t length = (size / sizeof(struct aymo_score_avd_event));
    assert(!length || data);

    score->events = (const struct aymo_score_avd_event*)data;
    score->length = length;
    aymo_score_avd_restart(score);
    return 0;
}


void aymo_score_avd_unload(
    struct aymo_score_avd_instance* score
)
{
    aymo_score_avd_ctor(score);
}


struct aymo_score_status* aymo_score_avd_get_status(
    struct aymo_score_avd_instance* score
)
{
    assert(score);
    return &score->status;
}


void aymo_score_avd_restart(
    struct aymo_score_avd_instance* score
)
{
    assert(score);

    score->index = 0u;

    score->status.delay = 0u;
    score->status.address = 0u;
    score->status.value = 0u;
    score->status.flags = 0u;

    if (score->index >= score->length) {
        score->status.flags |= AYMO_SCORE_FLAG_EOF;
    }
}


uint32_t aymo_score_avd_tick(
    struct aymo_score_avd_instance* score,
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
            const struct aymo_score_avd_event* event = &score->events[score->index++];

            if (event->address_hi & 0x80u) {  // delay tag
                uint32_t delay = (((uint32_t)(event->address_hi & 0x7Fu) << 16u) |
                                    ((uint32_t)event->address_lo << 8u) | event->value);
                if (delay) {
                    score->status.delay = delay;
                    score->status.flags = AYMO_SCORE_FLAG_DELAY;
                }
            }
            else {
                score->status.address = (((uint16_t)event->address_hi << 8u) | event->address_lo);
                score->status.value = event->value;
                score->status.flags = AYMO_SCORE_FLAG_EVENT;
                count -= pending;
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
