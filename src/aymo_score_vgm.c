/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023 Andrea Zoppi.

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

#include "aymo_score_vgm.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_score_vt aymo_score_vgm_vt = {
    "aymo_score_vgm",
    (aymo_score_ctor_f)aymo_score_vgm_ctor,
    (aymo_score_dtor_f)aymo_score_vgm_dtor,
    (aymo_score_load_f)aymo_score_vgm_load,
    (aymo_score_unload_f)aymo_score_vgm_unload,
    (aymo_score_get_status_f)aymo_score_vgm_get_status,
    (aymo_score_restart_f)aymo_score_vgm_restart,
    (aymo_score_tick_f)aymo_score_vgm_tick
};


static inline uint16_t decode_u16le(const uint8_t* ptr)
{
    uint16_t value = (
        ((uint16_t)ptr[0] <<  0u) |
        ((uint16_t)ptr[1] <<  8u)
    );
    return value;
}


static inline uint32_t decode_u32le(const uint8_t* ptr)
{
    uint32_t value = (
        ((uint32_t)ptr[0] <<  0u) |
        ((uint32_t)ptr[1] <<  8u) |
        ((uint32_t)ptr[2] << 16u) |
        ((uint32_t)ptr[3] << 24u)
    );
    return value;
}


int aymo_score_vgm_ctor(
    struct aymo_score_vgm_instance* score
)
{
    assert(score);

    uint32_t opl_rate = AYMO_SCORE_OPL_RATE_DEFAULT;
    uint32_t division = (opl_rate / 1000u);  // TODO: improve resolution via fixed point 24.8
    division += (uint32_t)(division == 0u);

    score->vt = &aymo_score_vgm_vt;

    score->events = NULL;

    score->opl_rate = opl_rate;
    score->division = division;
    score->eof_offset = 0u;
    score->total_samples = 0u;
    score->loop_samples = 0u;
    score->loop_offset = 0u;

    aymo_score_vgm_restart(score);
    return 0;
}


void aymo_score_vgm_dtor(
    struct aymo_score_vgm_instance* score
)
{
    AYMO_UNUSED_VAR(score);
    assert(score);
}


int aymo_score_vgm_load(
    struct aymo_score_vgm_instance* score,
    const void* data,
    uint32_t size
)
{
    assert(score);
    assert(data);
    assert(size);

    if (size < 0x40u) {
        return 1;
    }
    const uint8_t* ptr = data;
    uint32_t ident = decode_u32le(&ptr[aymo_score_vgm_offset_vgm_ident]);
    if (ident != 0x206D6756uL) {  // "Vgm "
        return 1;
    }
    uint32_t version = decode_u32le(&ptr[aymo_score_vgm_offset_version]);
    if (version < 0x151uL) {
        return 1;
    }
    uint32_t eof_offset = decode_u32le(&ptr[aymo_score_vgm_offset_eof_offset]);
    eof_offset += (uint32_t)aymo_score_vgm_offset_eof_offset;
    if (size < eof_offset) {
        return 1;
    }
    uint32_t data_offset = decode_u32le(&ptr[aymo_score_vgm_offset_vgm_data_offset]);
    data_offset += (uint32_t)aymo_score_vgm_offset_vgm_data_offset;
    if (size < data_offset) {
        return 1;
    }
    uint32_t total_samples = decode_u32le(&ptr[aymo_score_vgm_offset_total_samples]);
    uint32_t loop_samples = decode_u32le(&ptr[aymo_score_vgm_offset_loop_samples]);
    uint32_t loop_offset = decode_u32le(&ptr[aymo_score_vgm_offset_loop_offset]);
    loop_offset += (uint32_t)aymo_score_vgm_offset_loop_offset;

    if (loop_samples > total_samples) {
        loop_samples = 0u;
    }
    if (loop_offset > eof_offset) {
        loop_offset = 0u;
    }
    if (!loop_offset || !loop_samples) {
        loop_samples = 0u;
        loop_offset = 0u;
    }

    score->events = &ptr[data_offset];

    score->eof_offset = (eof_offset - data_offset);
    score->total_samples = total_samples;
    score->loop_samples = loop_samples;
    score->loop_offset = (loop_offset - data_offset);

    aymo_score_vgm_restart(score);
    return 0;
}


void aymo_score_vgm_unload(
    struct aymo_score_vgm_instance* score
)
{
    aymo_score_vgm_restart(score);
}


struct aymo_score_status* aymo_score_vgm_get_status(
    struct aymo_score_vgm_instance* score
)
{
    assert(score);
    return &score->status;
}


void aymo_score_vgm_restart(
    struct aymo_score_vgm_instance* score
)
{
    assert(score);

    score->offset = 0u;
    score->index = 0u;

    score->status.delay = 0u;
    score->status.address = 0u;
    score->status.value = 0u;
    score->status.flags = 0u;

    if ((score->offset >= score->eof_offset) ||
        (score->index >= score->total_samples)) {

        score->status.flags |= AYMO_SCORE_FLAG_EOF;
    }
}


static void aymo_score_vgm_decode(
    struct aymo_score_vgm_instance* score
)
{
    const uint8_t* ptr = &(score->events[score->offset]);
    uint8_t opcode = ptr[0];
    uint32_t skip = 1u;
    uint32_t wait = 0u;

    if ((opcode >= 0x5Au) && (opcode <= 0x5Fu)) {
        score->status.address = ptr[1];
        score->status.value = ptr[2];
        score->status.flags = AYMO_SCORE_FLAG_EVENT;
        skip = 3u;

        if (opcode == 0x5Fu) {
            score->status.address |= 0x100u;
        }
    }
    else if (opcode == 0x61u) {
        wait = decode_u16le(&ptr[1]);
        skip = 3u;
    }
    else if (opcode == 0x62u) {
        wait = 735u;
    }
    else if (opcode == 0x63u) {
        wait = 882u;
    }
    else if (opcode == 0x66u) {
        score->status.flags |= AYMO_SCORE_FLAG_EOF;
    }
    else if ((opcode >= 0x70u) && (opcode <= 0x7Fu)) {
        wait = (opcode - 0x70u + 1u);
    }
    else if ((opcode >= 0x80u) && (opcode <= 0x8Fu)) {
        wait = (opcode - 0x80u);
    }
    else if ((opcode >= 0x30u) && (opcode <= 0x3Fu)) {
        skip = 2u;
    }
    else if ((opcode >= 0x40u) && (opcode <= 0x4Eu)) {
        skip = 3u;
    }
    else if ((opcode >= 0x4Fu) && (opcode <= 0x50u)) {
        skip = 2u;
    }
    else if ((opcode >= 0x51u) && (opcode <= 0x61u)) {
        skip = 3u;
    }
    else if ((opcode >= 0xA0u) && (opcode <= 0xBFu)) {
        skip = 3u;
    }
    else if ((opcode >= 0xC0u) && (opcode <= 0xDFu)) {
        skip = 4u;
    }
    else if (opcode >= 0xE0u) {
        skip = 5u;
    }
    score->offset += skip;

    if (wait) {
        uint32_t delay = ((wait * AYMO_SCORE_OPL_RATE_DEFAULT) / 44100u);
        score->status.delay = delay;
        score->status.flags |= AYMO_SCORE_FLAG_DELAY;
    }
}


uint32_t aymo_score_vgm_tick(
    struct aymo_score_vgm_instance* score,
    uint32_t count
)
{
    assert(score);
    assert(!score->eof_offset || score->events);

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
        else if ((score->offset < score->eof_offset) &&
                 (score->index < score->total_samples)) {

            aymo_score_vgm_decode(score);

            if (score->status.flags & AYMO_SCORE_FLAG_EVENT) {
                count -= pending;  // FIXME: what if another event follows immediately? --> count -= CONSUMED
                break;
            }
        }
        else {  // TODO: support for loops
            score->status.flags = AYMO_SCORE_FLAG_EOF;
            break;
        }
    } while (pending);

    return count;
}


AYMO_CXX_EXTERN_C_END
