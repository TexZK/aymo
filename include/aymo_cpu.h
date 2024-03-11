// Main CPU header file.
// Always include this one, not the CPU-specific ones.
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
#ifndef _include_aymo_cpu_h
#define _include_aymo_cpu_h

#include "aymo_cc.h"

#if (defined(AYMO_CPU_FAMILY_X86) || defined(AYMO_CPU_FAMILY_X86_64))
    #include "aymo_cpu_x86.h"

    #if defined(AYMO_CPU_SUPPORT_X86_AVX2)
        #include "aymo_cpu_x86_avx2.h"
    #endif

    #if defined(AYMO_CPU_SUPPORT_X86_SSE41)
        #include "aymo_cpu_x86_sse41.h"
    #endif
#endif

#if (defined(AYMO_CPU_FAMILY_ARM) || defined(AYMO_CPU_FAMILY_AARCH64))
    #include "aymo_cpu_arm.h"

    #if defined(AYMO_CPU_SUPPORT_ARM_NEON)
        #include "aymo_cpu_arm_neon.h"
    #endif
#endif

AYMO_CXX_EXTERN_C_BEGIN


AYMO_PUBLIC void aymo_cpu_boot(void);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_cpu_h
