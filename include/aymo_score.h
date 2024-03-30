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
#ifndef _include_aymo_score_h
#define _include_aymo_score_h

#include "aymo_cc.h"

#include <stddef.h>
#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


enum aymo_score_type {
    aymo_score_type_dro,
    aymo_score_type_imf,
    aymo_score_type_raw,
    aymo_score_type_ref,
    aymo_score_type_vgm,
    aymo_score_type_unknown
};


#define AYMO_SCORE_OPL_RATE_DEFAULT     49716u

#define AYMO_SCORE_FLAG_EVENT   1u
#define AYMO_SCORE_FLAG_DELAY   2u
#define AYMO_SCORE_FLAG_EOF     4u

struct aymo_score_status {
    uint32_t delay;  // after
    uint16_t address;
    uint8_t value;
    uint8_t flags;
};

struct aymo_score_instance;  // forward

typedef int (*aymo_score_ctor_f)(
    struct aymo_score_instance* score
);

typedef void (*aymo_score_dtor_f)(
    struct aymo_score_instance* score
);

typedef int (*aymo_score_load_f)(
    struct aymo_score_instance* score,
    const void* data,
    uint32_t size
);

typedef void (*aymo_score_unload_f)(
    struct aymo_score_instance* score
);

typedef struct aymo_score_status* (*aymo_score_get_status_f)(
    struct aymo_score_instance* score
);

typedef void (*aymo_score_restart_f)(
    struct aymo_score_instance* score
);

typedef uint32_t (*aymo_score_tick_f)(
    struct aymo_score_instance* score,
    uint32_t count
);

struct aymo_score_vt {
    const char* class_name;
    aymo_score_ctor_f ctor;
    aymo_score_dtor_f dtor;
    aymo_score_load_f load;
    aymo_score_unload_f unload;
    aymo_score_get_status_f get_status;
    aymo_score_restart_f restart;
    aymo_score_tick_f tick;
};

struct aymo_score_instance {
    const struct aymo_score_vt* vt;
};


AYMO_PUBLIC int aymo_score_ctor(
    struct aymo_score_instance* score
);

AYMO_PUBLIC void aymo_score_dtor(
    struct aymo_score_instance* score
);

AYMO_PUBLIC int aymo_score_load(
    struct aymo_score_instance* score,
    const void* data,
    uint32_t size
);

AYMO_PUBLIC void aymo_score_unload(
    struct aymo_score_instance* score
);

AYMO_PUBLIC struct aymo_score_status* aymo_score_get_status(
    struct aymo_score_instance* score
);

AYMO_PUBLIC void aymo_score_restart(
    struct aymo_score_instance* score
);

AYMO_PUBLIC uint32_t aymo_score_tick(
    struct aymo_score_instance* score,
    uint32_t count
);


AYMO_PUBLIC enum aymo_score_type aymo_score_ext_to_type(
    const char *tag
);

AYMO_PUBLIC const struct aymo_score_vt* aymo_score_type_to_vt(
    enum aymo_score_type score_type
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_score_h
