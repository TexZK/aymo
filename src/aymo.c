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

#include "aymo.h"
#include "aymo_convert.h"
#include "aymo_cpu.h"

AYMO_CXX_EXTERN_C_BEGIN


void aymo_boot(void)
{
    aymo_cpu_boot();
    aymo_convert_boot();
}


const char* aymo_get_version(void)
{
    return PACKAGE_VERSION;
}


AYMO_CXX_EXTERN_C_END
