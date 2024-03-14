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

#include "aymo_score.h"
#include "aymo_score_avd.h"
#include "aymo_score_dro.h"
#include "aymo_score_imf.h"
#include "aymo_score_raw.h"
#include "aymo_score_vgm.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


int aymo_score_ctor(
    struct aymo_score_instance* score
)
{
    assert(score);
    assert(score->vt);
    return score->vt->ctor(score);
}


void aymo_score_dtor(
    struct aymo_score_instance* score
)
{
    assert(score);
    assert(score->vt);
    score->vt->dtor(score);
}


int aymo_score_load(
    struct aymo_score_instance* score,
    const void* data,
    uint32_t size
)
{
    assert(score);
    assert(score->vt);
    return score->vt->load(score, data, size);
}


void aymo_score_unload(
    struct aymo_score_instance* score
)
{
    assert(score);
    assert(score->vt);
    score->vt->unload(score);
}


struct aymo_score_status* aymo_score_get_status(
    struct aymo_score_instance* score
)
{
    assert(score);
    assert(score->vt);
    return score->vt->get_status(score);
}


void aymo_score_restart(
    struct aymo_score_instance* score
)
{
    assert(score);
    assert(score->vt);
    score->vt->restart(score);
}


uint32_t aymo_score_tick(
    struct aymo_score_instance* score,
    uint32_t count
)
{
    assert(score);
    assert(score->vt);
    return score->vt->tick(score, count);
}


enum aymo_score_type aymo_score_ext_to_type(
    const char *tag
)
{
    if (tag != NULL) {
        if (((tag[0] == 'A') || (tag[0] == 'a')) &&
            ((tag[1] == 'V') || (tag[1] == 'v')) &&
            ((tag[2] == 'D') || (tag[2] == 'd')) &&
            (tag[3] == '\0')) {
            return aymo_score_type_avd;
        }
        if (((tag[0] == 'D') || (tag[0] == 'd')) &&
            ((tag[1] == 'R') || (tag[1] == 'r')) &&
            ((tag[2] == 'O') || (tag[2] == 'o')) &&
            (tag[3] == '\0')) {
            return aymo_score_type_dro;
        }
        if (((tag[0] == 'I') || (tag[0] == 'i')) &&
            ((tag[1] == 'M') || (tag[1] == 'm')) &&
            ((tag[2] == 'F') || (tag[2] == 'f')) &&
            (tag[3] == '\0')) {
            return aymo_score_type_imf;
        }
        if (((tag[0] == 'R') || (tag[0] == 'r')) &&
            ((tag[1] == 'A') || (tag[1] == 'a')) &&
            ((tag[2] == 'W') || (tag[2] == 'w')) &&
            (tag[3] == '\0')) {
            return aymo_score_type_raw;
        }
        if (((tag[0] == 'V') || (tag[0] == 'v')) &&
            ((tag[1] == 'G') || (tag[1] == 'g')) &&
            ((tag[2] == 'M') || (tag[2] == 'm')) &&
            (tag[3] == '\0')) {
            return aymo_score_type_vgm;
        }
    }
    return aymo_score_type_unknown;
}


const struct aymo_score_vt* aymo_score_type_to_vt(
    enum aymo_score_type score_type
)
{
    switch (score_type) {
        case aymo_score_type_avd: return &aymo_score_avd_vt;
        case aymo_score_type_dro: return &aymo_score_dro_vt;
        case aymo_score_type_imf: return &aymo_score_imf_vt;
        case aymo_score_type_raw: return &aymo_score_raw_vt;
        case aymo_score_type_vgm: return &aymo_score_vgm_vt;
        default: return NULL;
    }
}


AYMO_CXX_EXTERN_C_END
