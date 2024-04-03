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

#include "aymo_score_ref.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


/*
This is a VERY CRUDE score player for AdPlug reference score sheets.
See: <ADPLUG_SRC>/test/testref/<*>.ref
*/

const struct aymo_score_vt aymo_score_ref_vt = {
    "aymo_score_ref",
    (aymo_score_ctor_f)aymo_score_ref_ctor,
    (aymo_score_dtor_f)aymo_score_ref_dtor,
    (aymo_score_load_f)aymo_score_ref_load,
    (aymo_score_unload_f)aymo_score_ref_unload,
    (aymo_score_get_status_f)aymo_score_ref_get_status,
    (aymo_score_restart_f)aymo_score_ref_restart,
    (aymo_score_tick_f)aymo_score_ref_tick
};


int aymo_score_ref_ctor(
    struct aymo_score_ref_instance* score
)
{
    assert(score);

    aymo_memset((&score->parent.vt + 1u), 0, (sizeof(*score) - sizeof(score->parent.vt)));

    score->text = NULL;
    score->size = 0u;
    aymo_score_ref_restart(score);
    return 0;
}


void aymo_score_ref_dtor(
    struct aymo_score_ref_instance* score
)
{
    AYMO_UNUSED_VAR(score);
    assert(score);
}


int aymo_score_ref_load(
    struct aymo_score_ref_instance* score,
    const void* data,
    uint32_t size
)
{
    assert(score);
    assert(!size || data);

    score->text = (const char*)data;
    score->size = size;
    aymo_score_ref_restart(score);
    return 0;
}


void aymo_score_ref_unload(
    struct aymo_score_ref_instance* score
)
{
    aymo_score_ref_ctor(score);
}


struct aymo_score_status* aymo_score_ref_get_status(
    struct aymo_score_ref_instance* score
)
{
    assert(score);
    return &score->parent.status;
}


static void aymo_score_ref_skip_whitespace(
    struct aymo_score_ref_instance* score
)
{
    const char* text = score->text;
    uint32_t offset = score->offset;
    uint32_t size = score->size;

    for (; offset < size; ++offset) {
        char c = text[offset];
        if (c == '\n') {
            score->line++;
        }
        if ((c < '\t') || ((c > '\r') && (c != ' '))) {
            break;
        }
    }
    score->offset = offset;
}


static void aymo_score_ref_skip_line(
    struct aymo_score_ref_instance* score
)
{
    const char* text = score->text;
    uint32_t offset = score->offset;
    uint32_t size = score->size;

    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c == '\n') || (c == '\r')) {
            break;
        }
    }
    score->offset = offset;

    aymo_score_ref_skip_whitespace(score);
}


void aymo_score_ref_restart(
    struct aymo_score_ref_instance* score
)
{
    assert(score);

    score->parent.status.delay = 0u;
    score->parent.status.address = 0u;
    score->parent.status.value = 0u;
    score->parent.status.flags = 0u;

    score->offset = 0u;
    score->line = 1u;

    aymo_score_ref_skip_whitespace(score);
    if (score->offset < score->size) {
        if (score->text[score->offset] == 'i') {  //  "init\n"
            aymo_score_ref_skip_line(score);
        }
    }

    if (score->offset >= score->size) {
        score->parent.status.flags |= AYMO_SCORE_FLAG_EOF;
    }
}


static void aymo_score_ref_decode_setchip(
    struct aymo_score_ref_instance* score
)
{
    const char* text = score->text;
    uint32_t offset = score->offset;
    uint32_t size = score->size;

    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            break;
        }
    }

    uint8_t addrhi = 0u;
    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            addrhi *= 10u;
            addrhi += (uint8_t)(c - '0');
        }
        else {
            break;
        }
    }

    score->offset = offset;
    aymo_score_ref_skip_line(score);

    score->addrhi = addrhi;
}


static void aymo_score_ref_decode_rate(
    struct aymo_score_ref_instance* score
)
{
    const char* text = score->text;
    uint32_t offset = score->offset;
    uint32_t size = score->size;

    ++offset;  // 'r'

    uint32_t numer = 0u;
    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            numer *= 10u;
            numer += (uint8_t)(c - '0');
        }
        else {
            break;
        }
    }

    ++offset;  // '.'

    uint32_t denom = 0u;
    uint32_t decimals = offset;
    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            denom *= 10u;
            denom += (uint8_t)(c - '0');
        }
        else {
            break;
        }
    }
    decimals = (offset - decimals);

    score->offset = offset;
    aymo_score_ref_skip_line(score);

    uint32_t scale = 1u;
    while (decimals--) {
        scale *= 10u;
    }
    uint32_t scaled = ((numer * scale) + denom);
    uint32_t delay = (((AYMO_SCORE_OPL_RATE_DEFAULT * scale) + (scaled / 2u)) / scaled);

    if (delay) {
        score->parent.status.delay = delay;
        score->parent.status.flags = AYMO_SCORE_FLAG_DELAY;
    }
}


static void aymo_score_ref_decode_write(
    struct aymo_score_ref_instance* score
)
{
    const char* text = score->text;
    uint32_t offset = score->offset;
    uint32_t size = score->size;

    uint8_t addrlo = 0u;
    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            addrlo <<= 4u;
            addrlo |= (uint8_t)(c - '0');
        }
        else if ((c >= 'a') && (c <= 'f')) {
            addrlo <<= 4u;
            addrlo |= (uint8_t)(c - ('a' - 10));
        }
        else {
            break;
        }
    }

    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            break;
        }
        if ((c >= 'a') && (c <= 'f')) {
            break;
        }
    }

    uint8_t value = 0u;
    for (; offset < size; ++offset) {
        char c = text[offset];
        if ((c >= '0') && (c <= '9')) {
            value <<= 4u;
            value |= (uint8_t)(c - '0');
        }
        else if ((c >= 'a') && (c <= 'f')) {
            value <<= 4u;
            value |= (uint8_t)(c - ('a' - 10));
        }
        else {
            break;
        }
    }

    score->offset = offset;
    aymo_score_ref_skip_line(score);

    uint16_t address = (((uint16_t)score->addrhi << 8u) | addrlo);
    score->parent.status.address = address;
    score->parent.status.value = value;
    score->parent.status.flags = AYMO_SCORE_FLAG_EVENT;
}


static void aymo_score_ref_decode_line(
    struct aymo_score_ref_instance* score
)
{
    const char* text = score->text;
    uint32_t offset = score->offset;
    char c = text[offset];

    if (c == 'i') {  // "init\n"
        aymo_score_ref_skip_line(score);
    }
    else if (c == 's') {  // "setchip(%d)\n"
        aymo_score_ref_decode_setchip(score);
    }
    else if (c == 'r') {  // "r%.2f\n"
        aymo_score_ref_decode_rate(score);
    }
    else {  // "%x <- %x\n"
        aymo_score_ref_decode_write(score);
    }
}


uint32_t aymo_score_ref_tick(
    struct aymo_score_ref_instance* score,
    uint32_t count
)
{
    assert(score);
    assert(!score->size || score->text);

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
        else if (score->offset < score->size) {
            aymo_score_ref_decode_line(score);

            if (score->parent.status.flags & AYMO_SCORE_FLAG_EVENT) {
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
