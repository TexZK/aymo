// CPU-specific header file for ARM NEON.
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
#ifndef _include_aymo_cpu_arm_neon_h
#define _include_aymo_cpu_arm_neon_h

#include <arm_neon.h>

AYMO_CXX_EXTERN_C_BEGIN


typedef int16x4_t   vi16x4_t;
typedef uint16x4_t  vu16x4_t;

typedef int32x2_t   vi32x2_t;
typedef uint32x2_t  vu32x2_t;

typedef int16x8_t   vi16x8_t;
typedef uint16x8_t  vu16x8_t;

typedef int32x4_t   vi32x4_t;
typedef uint32x4_t  vu32x4_t;

typedef float32x4_t vf32x4_t;
typedef float32x2_t vf32x2_t;


#ifndef AYMO_ALIGN_V128
    #define AYMO_ALIGN_V128     AYMO_ALIGN(16)
#endif


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_cpu_arm_neon_h
