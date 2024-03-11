// CPU-specific header file for x86 AVX2.
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
#ifndef _include_aymo_cpu_x86_avx2_h
#define _include_aymo_cpu_x86_avx2_h

#include <immintrin.h>

AYMO_CXX_EXTERN_C_BEGIN


typedef __m256i vi16x16_t;
typedef __m256i vu16x16_t;

typedef __m256i vi32x8_t;
typedef __m256i vu32x8_t;

typedef __m256 vf32x8_t;


#ifndef AYMO_ALIGN_V256
    #define AYMO_ALIGN_V256     AYMO_ALIGN(32)
#endif


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_cpu_x86_avx2_h
