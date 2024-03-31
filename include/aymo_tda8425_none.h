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
#ifndef _include_aymo_tda8425_none_h
#define _include_aymo_tda8425_none_h

#include "aymo_cpu.h"
#include "aymo_tda8425_common.h"

#include "TDA8425_emu.h"

AYMO_CXX_EXTERN_C_BEGIN


#undef AYMO_
#undef aymo_
#define AYMO_(_token_)  AYMO_TDA8425_NONE_##_token_
#define aymo_(_token_)  aymo_tda8425_none_##_token_

#define AYMO_TDA8425_NONE_DELAY 4


struct aymo_(chip) {
    TDA8425_Chip emu;
    float yh[AYMO_TDA8425_NONE_DELAY][2];
};


AYMO_PUBLIC const struct aymo_tda8425_vt* aymo_(get_vt)(void);
AYMO_PUBLIC uint32_t aymo_(get_sizeof)(void);
AYMO_PUBLIC void aymo_(ctor)(struct aymo_(chip)* chip, float sample_rate);
AYMO_PUBLIC void aymo_(dtor)(struct aymo_(chip)* chip);
AYMO_PUBLIC uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address);
AYMO_PUBLIC void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC void aymo_(process_f32)(struct aymo_(chip)* chip, uint32_t count, const float x[], float y[]);


#ifndef AYMO_KEEP_SHORTHANDS
    #undef AYMO_KEEP_SHORTHANDS
    #undef AYMO_
    #undef aymo_
#endif  // AYMO_KEEP_SHORTHANDS

AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_tda8425_none_h
