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
#ifndef _include_aymo_convert_none_h
#define _include_aymo_convert_none_h

#include "aymo_cc.h"

#include <stddef.h>
#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


#undef AYMO_
#undef aymo_
#define AYMO_(_token_)  AYMO_CONVERT_NONE_##_token_
#define aymo_(_token_)  aymo_convert_none_##_token_


AYMO_PUBLIC void aymo_(i16_f32)(size_t n, const int16_t i16v[], float f32v[]);
AYMO_PUBLIC void aymo_(f32_i16)(size_t n, const float f32v[], int16_t i16v[]);

AYMO_PUBLIC void aymo_(i16_f32_1)(size_t n, const int16_t i16v[], float f32v[]);
AYMO_PUBLIC void aymo_(f32_i16_1)(size_t n, const float f32v[], int16_t i16v[]);

AYMO_PUBLIC void aymo_(i16_f32_k)(size_t n, const int16_t i16v[], float f32v[], float scale);
AYMO_PUBLIC void aymo_(f32_i16_k)(size_t n, const float f32v[], int16_t i16v[], float scale);

AYMO_PUBLIC void aymo_(u16_f32)(size_t n, const uint16_t i16v[], float f32v[]);
AYMO_PUBLIC void aymo_(f32_u16)(size_t n, const float f32v[], uint16_t i16v[]);

AYMO_PUBLIC void aymo_(u16_f32_1)(size_t n, const uint16_t i16v[], float f32v[]);
AYMO_PUBLIC void aymo_(f32_u16_1)(size_t n, const float f32v[], uint16_t i16v[]);

AYMO_PUBLIC void aymo_(u16_f32_k)(size_t n, const uint16_t i16v[], float f32v[], float scale);
AYMO_PUBLIC void aymo_(f32_u16_k)(size_t n, const float f32v[], uint16_t i16v[], float scale);


#ifndef AYMO_KEEP_SHORTHANDS
    #undef AYMO_KEEP_SHORTHANDS
    #undef AYMO_
    #undef aymo_
#endif  // AYMO_KEEP_SHORTHANDS

AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_convert_none_h
