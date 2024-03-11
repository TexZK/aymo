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

#include "aymo_cpu.h"
#if (defined(AYMO_CPU_FAMILY_X86) || defined(AYMO_CPU_FAMILY_X86_64))

#ifdef AYMO_CPU_HAVE_CPUINFO
    #if defined(AYMO_CPU_HAVE_CPUINFO_CPUID_H_CPUID)
        #include <cpuid.h>
    #elif defined(AYMO_CPU_HAVE_CPUINFO_INTRIN_H_CPUID)
        #include <intrin.h>
    #endif
#endif  // AYMO_CPU_HAVE_CPUINFO

AYMO_CXX_EXTERN_C_BEGIN

#define AYMO_CPU_X86_CPUID_SSE      (1uL << 25u)  // edx[25] @ leaf 1
#define AYMO_CPU_X86_CPUID_SSE2     (1uL << 26u)  // edx[26] @ leaf 1
#define AYMO_CPU_X86_CPUID_SSE3     (1uL <<  0u)  // ecx[ 0] @ leaf 1
#define AYMO_CPU_X86_CPUID_SSSE3    (1uL <<  9u)  // ecx[ 9] @ leaf 1
#define AYMO_CPU_X86_CPUID_SSE41    (1uL << 19u)  // ecx[19] @ leaf 1
#define AYMO_CPU_X86_CPUID_SSE42    (1uL << 20u)  // ecx[20] @ leaf 1
#define AYMO_CPU_X86_CPUID_AVX      (1uL << 28u)  // ecx[28] @ leaf 1
#define AYMO_CPU_X86_CPUID_AVX2     (1uL <<  5u)  // ebx[ 5] @ leaf 7.0
#define AYMO_CPU_X86_CPUID_FMA      (1uL << 12u)  // ecx[12] @ leaf 1


static unsigned aymo_cpu_x86_extensions;


void aymo_cpu_x86_boot(void)
{
    unsigned mask = 0u;

#ifdef AYMO_CPU_PRESUME_X86_SSE
    mask |= AYMO_CPU_X86_EXT_SSE;
#endif
#ifdef AYMO_CPU_PRESUME_X86_SSE2
    mask |= AYMO_CPU_X86_EXT_SSE2;
#endif
#ifdef AYMO_CPU_PRESUME_X86_SSE3
    mask |= AYMO_CPU_X86_EXT_SSE3;
#endif
#ifdef AYMO_CPU_PRESUME_X86_SSSE3
    mask |= AYMO_CPU_X86_EXT_SSSE3;
#endif
#ifdef AYMO_CPU_PRESUME_X86_SSE41
    mask |= AYMO_CPU_X86_EXT_SSE41;
#endif
#ifdef AYMO_CPU_PRESUME_X86_SSE42
    mask |= AYMO_CPU_X86_EXT_SSE42;
#endif
#ifdef AYMO_CPU_PRESUME_X86_AVX
    mask |= AYMO_CPU_X86_EXT_AVX;
#endif
#ifdef AYMO_CPU_PRESUME_X86_AVX2
    mask |= AYMO_CPU_X86_EXT_AVX2;
#endif
#ifdef AYMO_CPU_PRESUME_X86_FMA3
    mask |= AYMO_CPU_X86_EXT_FMA3;
#endif

#ifdef AYMO_CPU_HAVE_CPUINFO
    unsigned e1[4] = { 0u, 0u, 0u, 0u };
    unsigned e7[4] = { 0u, 0u, 0u, 0u };

    #if defined(AYMO_CPU_HAVE_CPUINFO_CPUID_H_CPUID)
        __cpuid(1u, e1[0], e1[1], e1[2], e1[3]);
    #elif defined(AYMO_CPU_HAVE_CPUINFO_INTRIN_H_CPUID)
        __cpuid((int*)e1, 1);
    #endif

    #if defined(AYMO_CPU_HAVE_CPUINFO_CPUID_H_CPUID_COUNT)
        __cpuid_count(7u, 0u, e7[0], e7[1], e7[2], e7[3]);
    #elif defined(AYMO_CPU_HAVE_CPUINFO_INTRIN_H_CPUIDEX)
        __cpuidex((int*)e7, 7, 0);
    #endif

    if (e1[3] & AYMO_CPU_X86_CPUID_SSE  ) { mask |= AYMO_CPU_X86_EXT_SSE; }
    if (e1[3] & AYMO_CPU_X86_CPUID_SSE2 ) { mask |= AYMO_CPU_X86_EXT_SSE2; }
    if (e1[2] & AYMO_CPU_X86_CPUID_SSE3 ) { mask |= AYMO_CPU_X86_EXT_SSE3; }
    if (e1[2] & AYMO_CPU_X86_CPUID_SSSE3) { mask |= AYMO_CPU_X86_EXT_SSSE3; }
    if (e1[2] & AYMO_CPU_X86_CPUID_SSE41) { mask |= AYMO_CPU_X86_EXT_SSE41; }
    if (e1[2] & AYMO_CPU_X86_CPUID_SSE42) { mask |= AYMO_CPU_X86_EXT_SSE42; }
    if (e1[2] & AYMO_CPU_X86_CPUID_AVX  ) { mask |= AYMO_CPU_X86_EXT_AVX; }
    if (e7[1] & AYMO_CPU_X86_CPUID_AVX2 ) { mask |= AYMO_CPU_X86_EXT_AVX2; }
    if (e1[2] & AYMO_CPU_X86_CPUID_FMA  ) { mask |= AYMO_CPU_X86_EXT_FMA3; }
#endif  // AYMO_CPU_HAVE_CPUINFO

    aymo_cpu_x86_extensions = mask;
}


unsigned aymo_cpu_x86_get_extensions(void)
{
    return aymo_cpu_x86_extensions;
}


AYMO_CXX_EXTERN_C_END

#endif  // (defined(AYMO_CPU_FAMILY_X86) || defined(AYMO_CPU_FAMILY_X86_64))
