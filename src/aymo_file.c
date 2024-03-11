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

#include "aymo_file.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AYMO_CXX_EXTERN_C_BEGIN


static unsigned char aymo_file_chunk[AYMO_FILE_CHUNK_SIZE];


int aymo_file_save(const char* pathp, const void* datap, size_t size)
{
    const char* chunkp = (const char*)datap;
    FILE* filep = (FILE*)NULL;
    size_t total = 0u;
    size_t subsize = 0u;

    assert(pathp != NULL);
    assert(*pathp != '\0');
    if (datap == NULL) {
        size = 0u;
    }

    filep = fopen(pathp, "wb");
    if (filep == NULL) {
        perror("fopen()");
        goto error_;
    }

    while (total < size) {
        subsize = fwrite(chunkp, 1u, (total - size), filep);
        if (subsize == 0u) {
            perror("fwrite()");
            goto error_;
        }

        total += subsize;
        chunkp += subsize;
    }
    return 0;

error_:
    return 1;
}


int aymo_file_load(const char* pathp, void** datapp, size_t* sizep)
{
    FILE* filep = (FILE*)NULL;
    void* datap = NULL;
    size_t total = 0u;
    size_t subsize = 0u;

    assert(pathp != NULL);
    assert(*pathp != '\0');
    assert(datapp != NULL);
    assert(sizep != NULL);

    *datapp = NULL;
    *sizep = 0U;

    filep = fopen(pathp, "rb");
    if (filep == NULL) {
        perror("fopen()");
        goto error_;
    }

    datap = malloc(1u);
    if (datap == NULL) {
        perror("malloc()");
        goto error_;
    }

    while (!feof(filep)) {
        subsize = fread(&aymo_file_chunk[0], 1u, AYMO_FILE_CHUNK_SIZE, filep);
        if (subsize == 0u) {
            perror("fread()");
            goto error_;
        }

        datap = realloc(datap, (total + subsize));
        if (datap == NULL) {
            perror("realloc()");
            goto error_;
        }

        (void)memcpy((unsigned char*)datap + total, &aymo_file_chunk[0], subsize);

        total += subsize;
    }
    *datapp = datap;
    *sizep = total;
    return 0;

error_:
    aymo_file_unload(datap);
    return 1;
}


void aymo_file_unload(void* datap)
{
    if (datap) {
        free(datap);
    }
}


AYMO_CXX_EXTERN_C_END
