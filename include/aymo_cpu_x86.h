// CPU-specific header file for x86.
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
#ifndef _include_aymo_cpu_x86_h
#define _include_aymo_cpu_x86_h
#if (defined(AYMO_CPU_FAMILY_X86) || defined(AYMO_CPU_FAMILY_X86_64))

AYMO_CXX_EXTERN_C_BEGIN


#define AYMO_CPU_X86_EXT_SSE        (1u << 0u)
#define AYMO_CPU_X86_EXT_SSE2       (1u << 1u)
#define AYMO_CPU_X86_EXT_SSE3       (1u << 2u)
#define AYMO_CPU_X86_EXT_SSSE3      (1u << 3u)
#define AYMO_CPU_X86_EXT_SSE41      (1u << 4u)
#define AYMO_CPU_X86_EXT_SSE42      (1u << 5u)
#define AYMO_CPU_X86_EXT_AVX        (1u << 6u)
#define AYMO_CPU_X86_EXT_AVX2       (1u << 7u)
#define AYMO_CPU_X86_EXT_FMA3       (1u << 8u)


AYMO_PUBLIC void aymo_cpu_x86_boot(void);
AYMO_PUBLIC unsigned aymo_cpu_x86_get_extensions(void);


AYMO_CXX_EXTERN_C_END

#endif  // (defined(AYMO_CPU_FAMILY_X86) || defined(AYMO_CPU_FAMILY_X86))
#endif  // _include_aymo_cpu_x86_h
