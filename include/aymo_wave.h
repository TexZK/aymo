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
#ifndef _include_aymo_wave_h
#define _include_aymo_wave_h

#include "aymo_cc.h"

#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN

AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN
AYMO_PRAGMA_PACK_PUSH_1

#define AYMO_WAVE_FMT_TYPE_PCM      1u
#define AYMO_WAVE_FMT_TYPE_FLOAT    3u

/* Basic WAVE heading part. */
struct aymo_wave_heading {
    char riff_fourcc[4];
    uint32_t riff_size;

    char wave_fourcc[4];

    char wave_fmt_fourcc[4];
    uint32_t wave_fmt_size;
    uint16_t wave_fmt_type;
    uint16_t wave_fmt_channel_count;
    uint32_t wave_fmt_sample_rate;
    uint32_t wave_fmt_byte_rate;
    uint16_t wave_fmt_block_align;
    uint16_t wave_fmt_sample_bits;

    char wave_data_fourcc[4];
    uint32_t wave_data_size;

//  sample_t wave_data_samples[...];
};

AYMO_PRAGMA_PACK_POP
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


/* Basic setup of a WAVE heading part.
 *
 * Made for the common audio formats used with AYMO:
 * - around 50 kHz sample rate
 * - little-endian
 * - 8/16/32-bit signed integers
 * - 1/2/4 channel_count
 * - up to a few minutes
 *
 * NOTE: Function arguments are not checked in depth!
 *       Please make sure they are valid!
 */
AYMO_PUBLIC void aymo_wave_heading_setup(
    struct aymo_wave_heading* heading,
    uint16_t wave_fmt_type,
    uint16_t channel_count,
    uint16_t sample_bits,
    uint32_t sample_rate,
    uint32_t sample_count
);


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_wave_h
