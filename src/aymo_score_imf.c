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

#include "aymo_score_imf.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_score_vt aymo_score_imf_vt = {
    "aymo_score_imf",
    (aymo_score_ctor_f)aymo_score_imf_ctor,
    (aymo_score_dtor_f)aymo_score_imf_dtor,
    (aymo_score_load_f)aymo_score_imf_load,
    (aymo_score_unload_f)aymo_score_imf_unload,
    (aymo_score_get_status_f)aymo_score_imf_get_status,
    (aymo_score_restart_f)aymo_score_imf_restart,
    (aymo_score_tick_f)aymo_score_imf_tick
};


// See: https://moddingwiki.shikadi.net/wiki/IMF_Format
uint8_t aymo_score_imf_guess_type(
    const void* data,
    uint32_t size
)
{
    assert(data);

    if (size < 2u) {
        return 0u;
    }

    const uint8_t* ptr = (const uint8_t *)data;
    uint16_t word = (ptr[0] | ((uint16_t)ptr[1] << 8u));
    ptr += 2u;
    if (!word || (word & 3u)) {
        return 0u;
    }

    uint32_t sum1 = 0u;
    uint32_t sum2 = 0u;
    uint16_t i = 42u;

    while ((size >= 4u) && i--)
    {
        word = (ptr[0] | ((uint16_t)ptr[1] << 8u));
        ptr += 2u;
        sum1 += word;

        word = (ptr[0] | ((uint16_t)ptr[1] << 8u));
        ptr += 2u;
        sum2 += word;
    }
    return (uint8_t)(sum1 > sum2);
}


int aymo_score_imf_ctor_specific(
    struct aymo_score_imf_instance* score,
    uint32_t imf_rate,
    uint32_t opl_rate
)
{
    assert(score);
    assert(opl_rate);
    assert(imf_rate);

    aymo_memset((&score->parent.vt + 1u), 0, (sizeof(*score) - sizeof(score->parent.vt)));

    uint32_t division = (opl_rate / imf_rate);  // TODO: improve resolution via fixed point 24.8
    division += (uint32_t)(division == 0u);

    score->events = NULL;
    score->imf_rate = imf_rate;
    score->opl_rate = opl_rate;
    score->division = division;
    score->length = 0u;
    score->type = 0u;
    score->address_hi = 0u;

    aymo_score_imf_restart(score);
    return 0;
}


int aymo_score_imf_ctor(
    struct aymo_score_imf_instance* score
)
{
    return aymo_score_imf_ctor_specific(score, aymo_score_imf_rate_default, AYMO_SCORE_OPL_RATE_DEFAULT);
}


void aymo_score_imf_dtor(
    struct aymo_score_imf_instance* score
)
{
    AYMO_UNUSED_VAR(score);
    assert(score);
}


int aymo_score_imf_load_specific(
    struct aymo_score_imf_instance* score,
    const void* data,
    uint32_t size,
    uint8_t type
)
{
    assert(score);
    assert(data);
    assert(size);

    score->type = type;

    if (type) {
        const uint8_t* ptr = data;
        uint32_t length_by_header = (ptr[0] | ((uint16_t)ptr[1] << 8u));
        length_by_header /= sizeof(struct aymo_score_imf_event);
        score->length = length_by_header;
        score->events = (const struct aymo_score_imf_event*)(const void*)&ptr[2];

        uint32_t length_by_size = (uint32_t)(size - 2);
        length_by_size /= sizeof(struct aymo_score_imf_event);
        if (score->length > length_by_size) {
            score->length = length_by_size;
        }
    }
    else {
        uint32_t length_by_size = (uint32_t)size;
        length_by_size /= sizeof(struct aymo_score_imf_event);
        score->length = length_by_size;
        score->events = (const struct aymo_score_imf_event*)data;
    }

    aymo_score_imf_restart(score);
    return 0;
}


int aymo_score_imf_load(
    struct aymo_score_imf_instance* score,
    const void* data,
    uint32_t size
)
{
    uint8_t type = aymo_score_imf_guess_type(data, size);
    return aymo_score_imf_load_specific(score, data, size, type);
}


void aymo_score_imf_unload(
    struct aymo_score_imf_instance* score
)
{
    aymo_score_imf_restart(score);
}


struct aymo_score_status* aymo_score_imf_get_status(
    struct aymo_score_imf_instance* score
)
{
    assert(score);
    return &score->parent.status;
}


void aymo_score_imf_restart(
    struct aymo_score_imf_instance* score
)
{
    assert(score);

    score->index = 0u;
    score->address_hi = 0u;

    score->parent.status.delay = 0u;
    score->parent.status.address = 0u;
    score->parent.status.value = 0u;
    score->parent.status.flags = 0u;

    if (score->index >= score->length) {
        score->parent.status.flags |= AYMO_SCORE_FLAG_EOF;
    }
}


uint32_t aymo_score_imf_tick(
    struct aymo_score_imf_instance* score,
    uint32_t count
)
{
    assert(score);
    assert(!score->length || score->events);

    uint32_t pending = count;

    do {
        if (pending >= score->parent.status.delay) {
            pending -= score->parent.status.delay;
            score->parent.status.delay = 0u;
        }
        else {
            score->parent.status.delay -= pending;
            pending = 0u;
        }

        score->parent.status.address = 0u;
        score->parent.status.value = 0u;
        score->parent.status.flags = 0u;

        if (score->parent.status.delay) {
            score->parent.status.flags = AYMO_SCORE_FLAG_DELAY;
        }
        else if (score->index < score->length) {
            const struct aymo_score_imf_event* event = &score->events[score->index++];

            uint16_t delay = (((uint16_t)event->delay_hi << 8u) | event->delay_lo);
            if (delay) {
                score->parent.status.delay = (delay * score->division);
                score->parent.status.flags = AYMO_SCORE_FLAG_DELAY;
            }

            // Override virtual register 0x05 to extend the address range for OPL3
            if AYMO_UNLIKELY(event->address_lo == 0x05u) {
                score->address_hi = (event->value & 0x01u);
            }
            else {
                score->parent.status.address = ((uint16_t)(score->address_hi << 8u) | event->address_lo);
                score->parent.status.value = event->value;
                score->parent.status.flags = AYMO_SCORE_FLAG_EVENT;
                count -= pending;
                break;
            }
        }
        else {
            score->parent.status.flags = AYMO_SCORE_FLAG_EOF;
            break;
        }
    } while (pending);

    return count;
}


AYMO_CXX_EXTERN_C_END
