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

#include "aymo_score_dro.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_score_vt aymo_score_dro_vt = {
    "aymo_score_dro",
    (aymo_score_ctor_f)aymo_score_dro_ctor,
    (aymo_score_dtor_f)aymo_score_dro_dtor,
    (aymo_score_load_f)aymo_score_dro_load,
    (aymo_score_unload_f)aymo_score_dro_unload,
    (aymo_score_get_status_f)aymo_score_dro_get_status,
    (aymo_score_restart_f)aymo_score_dro_restart,
    (aymo_score_tick_f)aymo_score_dro_tick
};


static inline uint16_t make_u16le(uint8_t lo, uint8_t hi)
{
    return (uint16_t)((unsigned)lo | ((unsigned)hi << 8u));
}


int aymo_score_dro_ctor_specific(
    struct aymo_score_dro_instance* score,
    uint32_t opl_rate
)
{
    assert(score);
    assert(opl_rate);

    uint32_t division = (opl_rate / 1000u);  // TODO: improve resolution via fixed point 24.8
    division += (uint32_t)(division == 0u);

    score->vt = &aymo_score_dro_vt;

    score->header = NULL;
    score->v1_header = NULL;
    score->v2_header = NULL;
    score->codemap = NULL;
    score->events = NULL;

    score->opl_rate = opl_rate;
    score->division = division;
    score->length = 0u;
    score->offset = 0u;
    score->address_hi = 0u;

    aymo_score_dro_restart(score);
    return 0;
}


int aymo_score_dro_ctor(
    struct aymo_score_dro_instance* score
)
{
    return aymo_score_dro_ctor_specific(score, AYMO_SCORE_OPL_RATE_DEFAULT);
}


void aymo_score_dro_dtor(
    struct aymo_score_dro_instance* score
)
{
    AYMO_UNUSED_VAR(score);
    assert(score);
}


int aymo_score_dro_load(
    struct aymo_score_dro_instance* score,
    const void* data,
    uint32_t size
)
{
    assert(score);
    assert(data);
    assert(size);

    score->header = NULL;
    score->v1_header = NULL;
    score->v2_header = NULL;
    score->codemap = NULL;
    score->events = NULL;
    score->length = 0u;

    aymo_score_dro_restart(score);

    if (size < sizeof(struct aymo_score_dro_header)) {
        return 1;
    }
    const uint8_t* ptr = (const uint8_t*)data;
    const struct aymo_score_dro_header* header = NULL;
    header = (const struct aymo_score_dro_header*)(const void*)ptr;
    ptr += sizeof(struct aymo_score_dro_header);
    size -= sizeof(struct aymo_score_dro_header);
    const struct aymo_score_dro_v1_header* v1_header = NULL;
    const struct aymo_score_dro_v2_header* v2_header = NULL;
    const uint8_t* codemap = NULL;
    const uint8_t* events = NULL;
    uint32_t length = 0u;

    for (unsigned i = 0u; i < 8u; ++i) {
        if (header->signature[i] != AYMO_DRO_SIGNATURE[i]) {
            return 1;
        }
    }

    if ((((header->version_major == 0u) && (header->version_minor == 1u)) ||
         ((header->version_major == 1u) && (header->version_minor == 0u)))) {
        if (size < sizeof(struct aymo_score_dro_v1_header)) {
            return 1;
        }
        v1_header = (const struct aymo_score_dro_v1_header*)(const void*)ptr;
        ptr += sizeof(struct aymo_score_dro_v1_header);
        size -= sizeof(struct aymo_score_dro_v1_header);
        if ((v1_header->hardware_extra[0] ||
             v1_header->hardware_extra[1] ||
             v1_header->hardware_extra[2])) {
            ptr -= 3u;
        }
        events = ptr;
        length = v1_header->length_bytes;
    }
    else if ((header->version_major == 2u) && (header->version_minor == 0u)) {
        if (size < sizeof(struct aymo_score_dro_v1_header)) {
            return 1;
        }
        v2_header = (const struct aymo_score_dro_v2_header*)(const void*)ptr;
        ptr += sizeof(struct aymo_score_dro_v2_header);
        size -= sizeof(struct aymo_score_dro_v2_header);
        if (v2_header->format != (uint8_t)aymo_score_dro_v2_format_interleaved) {
            return 1;
        }
        if (v2_header->codemap_length > 128u) {
            return 1;
        }
        if (size < v2_header->codemap_length) {
            return 1;
        }
        codemap = ptr;
        ptr += v2_header->codemap_length;
        size -= v2_header->codemap_length;
        events = ptr;
        length = (v2_header->length_pairs * sizeof(struct aymo_score_dro_pair));
    }
    else {
        return 1;
    }

    score->header = header;
    score->v1_header = v1_header;
    score->v2_header = v2_header;
    score->codemap = codemap;
    score->events = events;
    score->length = length;

    aymo_score_dro_restart(score);
    return 0;
}


void aymo_score_dro_unload(
    struct aymo_score_dro_instance* score
)
{
    aymo_score_dro_restart(score);
}


void aymo_score_dro_restart(
    struct aymo_score_dro_instance* score
)
{
    assert(score);

    score->offset = 0u;
    score->address_hi = 0u;

    score->status.delay = 0u;
    score->status.address = 0u;
    score->status.value = 0u;
    score->status.flags = 0u;

    if (score->offset >= score->length) {
        score->status.flags |= AYMO_SCORE_FLAG_EOF;
    }
}


static void aymo_score_dro_decode_v1(
    struct aymo_score_dro_instance* score
)
{
    const uint8_t* ptr = &(score->events[score->offset]);

    switch ((enum aymo_score_dro_v1_code)ptr[0]) {
        case aymo_score_dro_v1_code_delay_byte: {
            if ((score->offset + 1u) <= score->length) {
                score->status.delay = ((ptr[1] + 1uL) * score->division);
                score->status.flags = AYMO_SCORE_FLAG_DELAY;
                score->offset += 2u;
            }
            else {
                score->status.flags = AYMO_SCORE_FLAG_DELAY;
                score->offset = score->length;
            }
            break;
        }
        case aymo_score_dro_v1_code_delay_word: {
            if ((score->offset + 2u) <= score->length) {
                score->status.delay = ((make_u16le(ptr[1], ptr[2]) + 1uL) * score->division);
                score->status.flags = AYMO_SCORE_FLAG_DELAY;
                score->offset += 3u;
            }
            else {
                score->status.flags = AYMO_SCORE_FLAG_EOF;
                score->offset = score->length;
            }
            break;
        }
        case aymo_score_dro_v1_code_switch_low: {
            score->address_hi = 0u;
            score->offset += 1u;
            break;
        }
        case aymo_score_dro_v1_code_switch_high: {
            score->address_hi = 1u;
            score->offset += 1u;
            break;
        }
        case aymo_score_dro_v1_code_escape: {
            if ((score->offset + 2u) <= score->length) {
                score->status.address = make_u16le(ptr[1], score->address_hi);
                score->status.value = ptr[2];
                score->status.flags = AYMO_SCORE_FLAG_EVENT;
                score->offset += 3u;
            }
            else {
                score->status.flags = AYMO_SCORE_FLAG_EOF;
                score->offset = score->length;
            }
            break;
        }
        case aymo_score_dro_v1_code_invalid:
        default: {
            if ((score->offset + 2u) <= score->length) {
                score->status.address = make_u16le(ptr[0], score->address_hi);
                score->status.value = ptr[1];
                score->status.flags = AYMO_SCORE_FLAG_EVENT;
                score->offset += 2u;
            }
            else {
                score->status.flags = AYMO_SCORE_FLAG_EOF;
                score->offset = score->length;
            }
            break;
        }
    }
}


static void aymo_score_dro_decode_v2(
    struct aymo_score_dro_instance* score
)
{
    const struct aymo_score_dro_v2_header *v2_header = score->v2_header;
    const uint8_t* ptr = &(score->events[score->offset]);

    if (ptr[0] == v2_header->short_delay_code) {
        score->status.delay = (ptr[1] + 1uL);
        score->status.flags = AYMO_SCORE_FLAG_DELAY;
    }
    else if (ptr[0] == v2_header->long_delay_code) {
        score->status.delay = ((ptr[1] + 1uL) * 256u);
        score->status.flags = AYMO_SCORE_FLAG_DELAY;
    }
    else if ((ptr[0] & 0xFFu) < v2_header->codemap_length) {
        score->address_hi = ((ptr[0] & 0x80u) >> 7u);
        uint8_t address_lo = score->codemap[ptr[0] & 0xFFu];
        score->status.address = make_u16le(address_lo, score->address_hi);
        score->status.value = ptr[1];
        score->status.flags = AYMO_SCORE_FLAG_EVENT;
    }
    score->offset += 2u;
}


struct aymo_score_status* aymo_score_dro_get_status(
    struct aymo_score_dro_instance* score
)
{
    assert(score);
    return &score->status;
}


uint32_t aymo_score_dro_tick(
    struct aymo_score_dro_instance* score,
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
        else if (score->offset < score->length) {
            if (score->v2_header) {
                aymo_score_dro_decode_v2(score);
            }
            else if (score->v1_header) {
                aymo_score_dro_decode_v1(score);
            }
            else {
                score->status.flags = AYMO_SCORE_FLAG_EOF;
                score->offset = score->length;
                break;
            }

            if (score->status.flags & AYMO_SCORE_FLAG_EVENT) {
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
