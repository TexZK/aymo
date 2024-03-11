// CPU-specific inline methods for ARM NEON.
// Only #include after "aymo_cpu.h" to have inline methods.
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

#define AYMO_KEEP_SHORTHANDS
#include "aymo_convert_none.h"

AYMO_CXX_EXTERN_C_BEGIN


static inline float convert_i16_f32(int16_t i)
{
    return (float)i;
}


static inline int16_t convert_f32_i16(float f)
{
    if (f >= (float)INT16_MAX) {
        return INT16_MAX;
    }
    if (f < (float)INT16_MIN) {
        return INT16_MIN;
    }
    return (int16_t)f;
}


static inline float convert_u16_f32(uint16_t u)
{
    return (float)u;
}


static inline uint16_t convert_f32_u16(float f)
{
    if (f >= (float)UINT16_MAX) {
        return UINT16_MAX;
    }
    if (f < 0.f) {
        return 0u;
    }
    return (uint16_t)f;
}


void aymo_(i16_f32)(size_t n, const int16_t i16v[], float f32v[])
{
    const int16_t* i16e = (i16v + n);
    while (i16v != i16e) {
        *f32v++ = convert_i16_f32(*i16v++);
    }
}


void aymo_(f32_i16)(size_t n, const float f32v[], int16_t i16v[])
{
    const float* f32e = (f32v + n);
    while (f32v != f32e) {
        *i16v++ = convert_f32_i16(*f32v++);
    }
}


void aymo_(i16_f32_1)(size_t n, const int16_t i16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    const int16_t* i16e = (i16v + n);
    while (i16v != i16e) {
        *f32v++ = (convert_i16_f32(*i16v++) * scale);
    }
}


void aymo_(f32_i16_1)(size_t n, const float f32v[], int16_t i16v[])
{
    const float scale = (float)(32768.);
    const float* f32e = (f32v + n);
    while (f32v != f32e) {
        *i16v++ = convert_f32_i16(*f32v++ * scale);
    }
}


void aymo_(i16_f32_k)(size_t n, const int16_t i16v[], float f32v[], float scale)
{
    const int16_t* i16e = (i16v + n);
    while (i16v != i16e) {
        *f32v++ = (convert_i16_f32(*i16v++) * scale);
    }
}


void aymo_(f32_i16_k)(size_t n, const float f32v[], int16_t i16v[], float scale)
{
    const float* f32e = (f32v + n);
    while (f32v != f32e) {
        *i16v++ = convert_f32_i16(*f32v++ * scale);
    }
}


void aymo_(u16_f32)(size_t n, const uint16_t u16v[], float f32v[])
{
    const uint16_t* u16e = (u16v + n);
    while (u16v != u16e) {
        *f32v++ = convert_u16_f32(*u16v++);
    }
}


void aymo_(f32_u16)(size_t n, const float f32v[], uint16_t u16v[])
{
    const float* f32e = (f32v + n);
    while (f32v != f32e) {
        *u16v++ = convert_f32_u16(*f32v++);
    }
}


void aymo_(u16_f32_1)(size_t n, const uint16_t u16v[], float f32v[])
{
    const float scale = (float)(1. / 32768.);
    const uint16_t* u16e = (u16v + n);
    while (u16v != u16e) {
        *f32v++ = (convert_u16_f32(*u16v++) * scale);
    }
}


void aymo_(f32_u16_1)(size_t n, const float f32v[], uint16_t u16v[])
{
    const float scale = (float)(32768.);
    const float* f32e = (f32v + n);
    while (f32v != f32e) {
        *u16v++ = convert_f32_u16(*f32v++ * scale);
    }
}


void aymo_(u16_f32_k)(size_t n, const uint16_t u16v[], float f32v[], float scale)
{
    const uint16_t* u16e = (u16v + n);
    while (u16v != u16e) {
        *f32v++ = (convert_u16_f32(*u16v++) * scale);
    }
}


void aymo_(f32_u16_k)(size_t n, const float f32v[], uint16_t u16v[], float scale)
{
    const float* f32e = (f32v + n);
    while (f32v != f32e) {
        *u16v++ = convert_f32_u16(*f32v++ * scale);
    }
}


AYMO_CXX_EXTERN_C_END
