// CPU-specific header file for x86 SSE4.1.
// DO NOT include this file; #include "aymo_cpu.h" instead.
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
#ifndef _include_aymo_cpu_x86_sse41_h
#define _include_aymo_cpu_x86_sse41_h

#include <smmintrin.h>

AYMO_CXX_EXTERN_C_BEGIN


typedef __m128i vi16x8_t;
typedef __m128i vu16x8_t;

typedef __m128i vi32x4_t;
typedef __m128i vu32x4_t;

typedef __m128 vf32x4_t;


#ifndef AYMO_ALIGN_V128
    #define AYMO_ALIGN_V128     AYMO_ALIGN(16)
#endif


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_cpu_x86_sse41_h
