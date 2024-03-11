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
#ifndef _include_aymo_score_avd_h
#define _include_aymo_score_avd_h

#include "aymo_score.h"

#include <stddef.h>

AYMO_CXX_EXTERN_C_BEGIN


AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN
AYMO_PRAGMA_PACK_PUSH_1

struct aymo_score_avd_event {
    uint8_t address_hi;
    uint8_t address_lo;
    uint8_t value;
};

AYMO_PRAGMA_PACK_POP
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


struct aymo_score_avd_instance {
    const struct aymo_score_vt* vt;
    struct aymo_score_status status;
    const struct aymo_score_avd_event* events;
    uint32_t length;
    uint32_t index;
};


AYMO_PUBLIC const struct aymo_score_vt aymo_score_avd_vt;


AYMO_PUBLIC int aymo_score_avd_ctor(
    struct aymo_score_avd_instance* score
);

AYMO_PUBLIC void aymo_score_avd_dtor(
    struct aymo_score_avd_instance* score
);

AYMO_PUBLIC int aymo_score_avd_load(
    struct aymo_score_avd_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_avd_unload(
    struct aymo_score_avd_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_avd_get_status(
    struct aymo_score_avd_instance* score
);

AYMO_PUBLIC void aymo_score_avd_restart(
    struct aymo_score_avd_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_avd_tick(
    struct aymo_score_avd_instance* score,
    uint32_t count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_avd_h
