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
#include "aymo_score_dro.h"
#include "aymo_score_avd.h"
#include "aymo_score_imf.h"
#include "aymo_testing.h"
#include "aymo_ymf262.h"

#include "opl3.h"

#include <stdio.h>
#include <string.h>

AYMO_CXX_EXTERN_C_BEGIN


struct app_args {
    int argc;
    char** argv;

    // Score parameters
    const char* score_path_cstr;  // NULL or "-" for stdin
    const char* score_type_cstr;  // NULL uses score file extension
    enum aymo_score_type score_type;
};


// copied from opl3.c
static const uint8_t mt[16] = {
    1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30
};


static int app_return;

static struct app_args app_args;

static void* score_data;
static size_t score_size;
static union app_scores {
    struct aymo_score_instance base;
    struct aymo_score_avd_instance avd;
    struct aymo_score_dro_instance dro;
    struct aymo_score_imf_instance imf;
} score;

static struct aymo_(chip) aymo_chip;
static opl3_chip nuked_chip;
static int16_t nuked_out[4];


#undef assert
#define assert(x)  {  \
    if (!(x)) {  \
        fprintf(stderr, "@ %d: FAILED assert(%s)\n",  \
        __LINE__, (#x)); goto catch_;  \
    }  \
}//
