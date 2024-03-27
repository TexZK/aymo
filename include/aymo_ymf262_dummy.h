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
#ifndef _include_aymo_ymf262_dummy_h
#define _include_aymo_ymf262_dummy_h

#include "aymo_cpu.h"
#include "aymo_ymf262_common.h"

#include "opl3.h"

#include <stddef.h>
#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


#undef AYMO_
#undef aymo_
#define AYMO_(_token_)  AYMO_YMF262_DUMMY_##_token_
#define aymo_(_token_)  aymo_ymf262_dummy_##_token_


struct aymo_(chip) {
    struct aymo_ymf262_chip parent;
};

AYMO_PUBLIC const struct aymo_ymf262_vt aymo_(vt);


AYMO_PUBLIC const struct aymo_ymf262_vt* aymo_(get_vt)(void);
AYMO_PUBLIC uint32_t aymo_(get_sizeof)(void);
AYMO_PUBLIC void aymo_(ctor)(struct aymo_(chip)* chip);
AYMO_PUBLIC void aymo_(dtor)(struct aymo_(chip)* chip);
AYMO_PUBLIC uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address);
AYMO_PUBLIC void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC int aymo_(enqueue_write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC int aymo_(enqueue_delay)(struct aymo_(chip)* chip, uint32_t count);
AYMO_PUBLIC int16_t aymo_(get_output)(struct aymo_(chip)* chip, uint8_t channel);
AYMO_PUBLIC void aymo_(tick)(struct aymo_(chip)* chip, uint32_t count);
AYMO_PUBLIC void aymo_(generate_i16x2)(struct aymo_(chip)* chip, uint32_t count, int16_t y[]);
AYMO_PUBLIC void aymo_(generate_i16x4)(struct aymo_(chip)* chip, uint32_t count, int16_t y[]);
AYMO_PUBLIC void aymo_(generate_f32x2)(struct aymo_(chip)* chip, uint32_t count, float y[]);
AYMO_PUBLIC void aymo_(generate_f32x4)(struct aymo_(chip)* chip, uint32_t count, float y[]);


#ifndef AYMO_KEEP_SHORTHANDS
    #undef AYMO_KEEP_SHORTHANDS
    #undef AYMO_
    #undef aymo_
#endif  // AYMO_KEEP_SHORTHANDS

AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_ymf262_dummy_h
