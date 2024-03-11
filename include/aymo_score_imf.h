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
#ifndef _include_aymo_score_imf_h
#define _include_aymo_score_imf_h

#include "aymo_score.h"

AYMO_CXX_EXTERN_C_BEGIN


// Common IMF score event rates
#define aymo_score_imf_rate_280Hz                       280u
#define aymo_score_imf_rate_duke_nukem_ii               aymo_score_imf_rate_280Hz

#define aymo_score_imf_rate_560Hz                       560u
#define aymo_score_imf_rate_bio_menace                  aymo_score_imf_rate_560Hz
#define aymo_score_imf_rate_commander_keen              aymo_score_imf_rate_560Hz
#define aymo_score_imf_rate_cosmos_cosmic_adventures    aymo_score_imf_rate_560Hz
#define aymo_score_imf_rate_monster_bash                aymo_score_imf_rate_560Hz
#define aymo_score_imf_rate_major_stryker               aymo_score_imf_rate_560Hz

#define aymo_score_imf_rate_700Hz                       700u
#define aymo_score_imf_rate_blake_stone                 aymo_score_imf_rate_700Hz
#define aymo_score_imf_rate_operation_body_count        aymo_score_imf_rate_700Hz
#define aymo_score_imf_rate_wolfenstein_3d              aymo_score_imf_rate_700Hz
#define aymo_score_imf_rate_corridor_7                  aymo_score_imf_rate_700Hz

#define aymo_score_imf_rate_default                     aymo_score_imf_rate_560Hz


AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN
AYMO_PRAGMA_PACK_PUSH_1

struct aymo_score_imf_event {
    uint8_t address_lo;
    uint8_t value;
    uint8_t delay_lo;
    uint8_t delay_hi;
};

AYMO_PRAGMA_PACK_POP
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


struct aymo_score_imf_instance {
    const struct aymo_score_vt* vt;
    struct aymo_score_status status;
    const struct aymo_score_imf_event* events;
    uint32_t imf_rate;
    uint32_t opl_rate;
    uint32_t division;
    uint32_t length;
    uint32_t index;
    uint8_t type;
    uint8_t address_hi;
};


AYMO_PUBLIC const struct aymo_score_vt aymo_score_imf_vt;


AYMO_PUBLIC uint8_t aymo_score_imf_guess_type(
    const void* data,
    uint32_t size
);

AYMO_PUBLIC int aymo_score_imf_ctor_specific(
    struct aymo_score_imf_instance* score,
    uint32_t imf_rate,
    uint32_t opl_rate
);

AYMO_PUBLIC int aymo_score_imf_ctor(
    struct aymo_score_imf_instance* score
);

AYMO_PUBLIC void aymo_score_imf_dtor(
    struct aymo_score_imf_instance* score
);

AYMO_PUBLIC int aymo_score_imf_load_specific(
    struct aymo_score_imf_instance* score,
    const void* data,
    uint32_t size,
    uint8_t type
);

AYMO_PUBLIC int aymo_score_imf_load(
    struct aymo_score_imf_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_imf_unload(
    struct aymo_score_imf_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_imf_get_status(
    struct aymo_score_imf_instance* score
);

AYMO_PUBLIC void aymo_score_imf_restart(
    struct aymo_score_imf_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_imf_tick(
    struct aymo_score_imf_instance* score,
    uint32_t count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_imf_h
