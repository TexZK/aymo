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

#include "aymo_convert.h"
#include "aymo_convert_arm_neon.h"
#include "aymo_convert_none.h"
#include "aymo_convert_x86_avx2.h"
#include "aymo_convert_x86_sse41.h"
#include "aymo_cpu.h"

AYMO_CXX_EXTERN_C_BEGIN


// Dispatcher function types
typedef void (*aymo_convert_i16_f32_f)(size_t n, const int16_t i16v[], float f32v[]);
typedef void (*aymo_convert_f32_i16_f)(size_t n, const float f32v[], int16_t i16v[]);
typedef void (*aymo_convert_i16_f32_1_f)(size_t n, const int16_t i16v[], float f32v[]);
typedef void (*aymo_convert_f32_i16_1_f)(size_t n, const float f32v[], int16_t i16v[]);
typedef void (*aymo_convert_i16_f32_k_f)(size_t n, const int16_t i16v[], float f32v[], float scale);
typedef void (*aymo_convert_f32_i16_k_f)(size_t n, const float f32v[], int16_t i16v[], float scale);
typedef void (*aymo_convert_u16_f32_f)(size_t n, const uint16_t u16v[], float f32v[]);
typedef void (*aymo_convert_f32_u16_f)(size_t n, const float f32v[], uint16_t u16v[]);
typedef void (*aymo_convert_u16_f32_1_f)(size_t n, const uint16_t u16v[], float f32v[]);
typedef void (*aymo_convert_f32_u16_1_f)(size_t n, const float f32v[], uint16_t u16v[]);
typedef void (*aymo_convert_u16_f32_k_f)(size_t n, const uint16_t u16v[], float f32v[], float scale);
typedef void (*aymo_convert_f32_u16_k_f)(size_t n, const float f32v[], uint16_t u16v[], float scale);

// Dispatcher function pointers
static aymo_convert_i16_f32_f aymo_convert_i16_f32_p;
static aymo_convert_f32_i16_f aymo_convert_f32_i16_p;
static aymo_convert_i16_f32_1_f aymo_convert_i16_f32_1_p;
static aymo_convert_f32_i16_1_f aymo_convert_f32_i16_1_p;
static aymo_convert_i16_f32_k_f aymo_convert_i16_f32_k_p;
static aymo_convert_f32_i16_k_f aymo_convert_f32_i16_k_p;
static aymo_convert_u16_f32_f aymo_convert_u16_f32_p;
static aymo_convert_f32_u16_f aymo_convert_f32_u16_p;
static aymo_convert_u16_f32_1_f aymo_convert_u16_f32_1_p;
static aymo_convert_f32_u16_1_f aymo_convert_f32_u16_1_p;
static aymo_convert_u16_f32_k_f aymo_convert_u16_f32_k_p;
static aymo_convert_f32_u16_k_f aymo_convert_f32_u16_k_p;


void aymo_convert_boot(void)
{
#ifdef AYMO_CPU_SUPPORT_X86_AVX2
    if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_AVX2) {
        aymo_convert_i16_f32_p = aymo_convert_x86_avx2_i16_f32;
        aymo_convert_f32_i16_p = aymo_convert_x86_avx2_f32_i16;
        aymo_convert_i16_f32_1_p = aymo_convert_x86_avx2_i16_f32_1;
        aymo_convert_f32_i16_1_p = aymo_convert_x86_avx2_f32_i16_1;
        aymo_convert_i16_f32_k_p = aymo_convert_x86_avx2_i16_f32_k;
        aymo_convert_f32_i16_k_p = aymo_convert_x86_avx2_f32_i16_k;
        aymo_convert_u16_f32_p = aymo_convert_x86_avx2_u16_f32;
        aymo_convert_f32_u16_p = aymo_convert_x86_avx2_f32_u16;
        aymo_convert_u16_f32_1_p = aymo_convert_x86_avx2_u16_f32_1;
        aymo_convert_f32_u16_1_p = aymo_convert_x86_avx2_f32_u16_1;
        aymo_convert_u16_f32_k_p = aymo_convert_x86_avx2_u16_f32_k;
        aymo_convert_f32_u16_k_p = aymo_convert_x86_avx2_f32_u16_k;
        return;
    }
#endif  // AYMO_CPU_SUPPORT_X86_AVX2

#ifdef AYMO_CPU_SUPPORT_X86_SSE41
    if (aymo_cpu_x86_get_extensions() & AYMO_CPU_X86_EXT_SSE41) {
        aymo_convert_i16_f32_p = aymo_convert_x86_sse41_i16_f32;
        aymo_convert_f32_i16_p = aymo_convert_x86_sse41_f32_i16;
        aymo_convert_i16_f32_1_p = aymo_convert_x86_sse41_i16_f32_1;
        aymo_convert_f32_i16_1_p = aymo_convert_x86_sse41_f32_i16_1;
        aymo_convert_i16_f32_k_p = aymo_convert_x86_sse41_i16_f32_k;
        aymo_convert_f32_i16_k_p = aymo_convert_x86_sse41_f32_i16_k;
        aymo_convert_u16_f32_p = aymo_convert_x86_sse41_u16_f32;
        aymo_convert_f32_u16_p = aymo_convert_x86_sse41_f32_u16;
        aymo_convert_u16_f32_1_p = aymo_convert_x86_sse41_u16_f32_1;
        aymo_convert_f32_u16_1_p = aymo_convert_x86_sse41_f32_u16_1;
        aymo_convert_u16_f32_k_p = aymo_convert_x86_sse41_u16_f32_k;
        aymo_convert_f32_u16_k_p = aymo_convert_x86_sse41_f32_u16_k;
        return;
    }
#endif  // AYMO_CPU_SUPPORT_X86_SSE41

#if 0//def AYMO_CPU_SUPPORT_ARM_NEON   //FIXME: TODO:
    if (aymo_cpu_arm_get_extensions() & AYMO_CPU_ARM_EXT_NEON) {
        aymo_convert_i16_f32_p = aymo_convert_arm_neon_i16_f32;
        aymo_convert_f32_i16_p = aymo_convert_arm_neon_f32_i16;
        aymo_convert_i16_f32_1_p = aymo_convert_arm_neon_i16_f32_1;
        aymo_convert_f32_i16_1_p = aymo_convert_arm_neon_f32_i16_1;
        aymo_convert_i16_f32_k_p = aymo_convert_arm_neon_i16_f32_k;
        aymo_convert_f32_i16_k_p = aymo_convert_arm_neon_f32_i16_k;
        aymo_convert_u16_f32_p = aymo_convert_arm_neon_u16_f32;
        aymo_convert_f32_u16_p = aymo_convert_arm_neon_f32_u16;
        aymo_convert_u16_f32_1_p = aymo_convert_arm_neon_u16_f32_1;
        aymo_convert_f32_u16_1_p = aymo_convert_arm_neon_f32_u16_1;
        aymo_convert_u16_f32_k_p = aymo_convert_arm_neon_u16_f32_k;
        aymo_convert_f32_u16_k_p = aymo_convert_arm_neon_f32_u16_k;
        return;
    }
#endif  // AYMO_CPU_SUPPORT_ARM_NEON

    // Default dispatcher functions
    aymo_convert_i16_f32_p = aymo_convert_none_i16_f32;
    aymo_convert_f32_i16_p = aymo_convert_none_f32_i16;
    aymo_convert_i16_f32_1_p = aymo_convert_none_i16_f32_1;
    aymo_convert_f32_i16_1_p = aymo_convert_none_f32_i16_1;
    aymo_convert_i16_f32_k_p = aymo_convert_none_i16_f32_k;
    aymo_convert_f32_i16_k_p = aymo_convert_none_f32_i16_k;
    aymo_convert_u16_f32_p = aymo_convert_none_u16_f32;
    aymo_convert_f32_u16_p = aymo_convert_none_f32_u16;
    aymo_convert_u16_f32_1_p = aymo_convert_none_u16_f32_1;
    aymo_convert_f32_u16_1_p = aymo_convert_none_f32_u16_1;
    aymo_convert_u16_f32_k_p = aymo_convert_none_u16_f32_k;
    aymo_convert_f32_u16_k_p = aymo_convert_none_f32_u16_k;
}


void aymo_convert_i16_f32(size_t n, const int16_t i16v[], float f32v[])
{
    aymo_convert_i16_f32_p(n, i16v, f32v);
}


void aymo_convert_f32_i16(size_t n, const float f32v[], int16_t i16v[])
{
    aymo_convert_f32_i16_p(n, f32v, i16v);
}


void aymo_convert_i16_f32_1(size_t n, const int16_t i16v[], float f32v[])
{
    aymo_convert_i16_f32_1_p(n, i16v, f32v);
}


void aymo_convert_f32_i16_1(size_t n, const float f32v[], int16_t i16v[])
{
    aymo_convert_f32_i16_1_p(n, f32v, i16v);
}


void aymo_convert_i16_f32_k(size_t n, const int16_t i16v[], float f32v[], float scale)
{
    aymo_convert_i16_f32_k_p(n, i16v, f32v, scale);
}


void aymo_convert_f32_i16_k(size_t n, const float f32v[], int16_t i16v[], float scale)
{
    aymo_convert_f32_i16_k_p(n, f32v, i16v, scale);
}


void aymo_convert_u16_f32(size_t n, const uint16_t u16v[], float f32v[])
{
    aymo_convert_u16_f32_p(n, u16v, f32v);
}


void aymo_convert_f32_u16(size_t n, const float f32v[], uint16_t u16v[])
{
    aymo_convert_f32_u16_p(n, f32v, u16v);
}


void aymo_convert_u16_f32_1(size_t n, const uint16_t u16v[], float f32v[])
{
    aymo_convert_u16_f32_1_p(n, u16v, f32v);
}


void aymo_convert_f32_u16_1(size_t n, const float f32v[], uint16_t u16v[])
{
    aymo_convert_f32_u16_1_p(n, f32v, u16v);
}


void aymo_convert_u16_f32_k(size_t n, const uint16_t u16v[], float f32v[], float scale)
{
    aymo_convert_u16_f32_k_p(n, u16v, f32v, scale);
}


void aymo_convert_f32_u16_k(size_t n, const float f32v[], uint16_t u16v[], float scale)
{
    aymo_convert_f32_u16_k_p(n, f32v, u16v, scale);
}


AYMO_CXX_EXTERN_C_END
