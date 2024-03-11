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
#if (defined(AYMO_CPU_FAMILY_ARM) || defined(AYMO_CPU_FAMILY_AARCH64))

AYMO_CXX_EXTERN_C_BEGIN


static unsigned aymo_cpu_arm_extensions;


void aymo_cpu_arm_boot(void)
{
    unsigned mask = 0u;

#ifdef AYMO_CPU_PRESUME_ARM_NEON
    mask |= AYMO_CPU_ARM_EXT_NEON;
#endif
#ifdef AYMO_CPU_PRESUME_ARM_NEON64
    mask |= AYMO_CPU_ARM_EXT_NEON64;
#endif

    // FIXME: TODO: feature detection
#ifdef AYMO_CPU_SUPPORT_ARM_NEON
    mask |= AYMO_CPU_ARM_EXT_NEON;
#endif
#ifdef AYMO_CPU_SUPPORT_ARM_NEON64
    mask |= AYMO_CPU_ARM_EXT_NEON64;
#endif

    aymo_cpu_arm_extensions = mask;
}


unsigned aymo_cpu_arm_get_extensions(void)
{
    return aymo_cpu_arm_extensions;
}


AYMO_CXX_EXTERN_C_END

#endif  // (defined(AYMO_CPU_FAMILY_ARM) || defined(AYMO_CPU_FAMILY_AARCH64))
