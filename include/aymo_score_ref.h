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
#ifndef _include_aymo_score_ref_h
#define _include_aymo_score_ref_h

#include "aymo_score.h"

#include <stddef.h>

AYMO_CXX_EXTERN_C_BEGIN


AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN
AYMO_PRAGMA_PACK_PUSH_1

AYMO_PRAGMA_PACK_POP
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


struct aymo_score_ref_instance {
    struct aymo_score_instance parent;
    const char* text;
    uint32_t size;
    uint32_t offset;
    uint32_t line;
    uint8_t addrhi;
};


AYMO_PUBLIC const struct aymo_score_vt aymo_score_ref_vt;


AYMO_PUBLIC int aymo_score_ref_ctor(
    struct aymo_score_ref_instance* score
);

AYMO_PUBLIC void aymo_score_ref_dtor(
    struct aymo_score_ref_instance* score
);

AYMO_PUBLIC int aymo_score_ref_load(
    struct aymo_score_ref_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_ref_unload(
    struct aymo_score_ref_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_ref_get_status(
    struct aymo_score_ref_instance* score
);

AYMO_PUBLIC void aymo_score_ref_restart(
    struct aymo_score_ref_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_ref_tick(
    struct aymo_score_ref_instance* score,
    uint32_t count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_ref_h
