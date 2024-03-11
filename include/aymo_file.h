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
#ifndef _include_aymo_utils_h
#define _include_aymo_utils_h

#include "aymo_cc.h"

#include <stddef.h>

AYMO_CXX_EXTERN_C_BEGIN


#ifndef AYMO_FILE_CHUNK_SIZE
#define AYMO_FILE_CHUNK_SIZE    (1000000uL)  // 1 MB
#endif


AYMO_PUBLIC int aymo_file_save(const char* pathp, const void* datap, size_t size);
AYMO_PUBLIC int aymo_file_load(const char* pathp, void** datapp, size_t* sizep);
AYMO_PUBLIC void aymo_file_unload(void* datap);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_utils_h
