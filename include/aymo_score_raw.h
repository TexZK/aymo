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
#ifndef _include_aymo_score_raw_h
#define _include_aymo_score_raw_h

#include "aymo_score.h"

AYMO_CXX_EXTERN_C_BEGIN


AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN
AYMO_PRAGMA_PACK_PUSH_1

#define AYMO_SCORE_RAW_RAWADATA "RAWADATA"
#define AYMO_SCORE_RAW_REFCLK   1193180L

struct aymo_score_raw_header {
    uint8_t rawadata[8];
    uint16_t clock;
};

struct aymo_score_raw_event {
    uint8_t data;
    uint8_t ctrl;
};

AYMO_PRAGMA_PACK_POP
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


struct aymo_score_raw_instance {
    const struct aymo_score_vt* vt;
    struct aymo_score_status status;
    const struct aymo_score_raw_event* events;
    uint32_t raw_rate;
    uint32_t division;
    uint32_t length;
    uint32_t index;
    uint16_t clock;
    uint16_t clock_initial;
    uint8_t address_hi;
};


AYMO_PUBLIC const struct aymo_score_vt aymo_score_raw_vt;


AYMO_PUBLIC int aymo_score_raw_ctor(
    struct aymo_score_raw_instance* score
);

AYMO_PUBLIC void aymo_score_raw_dtor(
    struct aymo_score_raw_instance* score
);

AYMO_PUBLIC int aymo_score_raw_load(
    struct aymo_score_raw_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_raw_unload(
    struct aymo_score_raw_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_raw_get_status(
    struct aymo_score_raw_instance* score
);

AYMO_PUBLIC void aymo_score_raw_restart(
    struct aymo_score_raw_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_raw_tick(
    struct aymo_score_raw_instance* score,
    uint32_t count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_raw_h
