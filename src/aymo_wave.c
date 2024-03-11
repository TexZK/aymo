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

#include "aymo_wave.h"

#include <assert.h>
#include <stddef.h>

AYMO_CXX_EXTERN_C_BEGIN


AYMO_PUBLIC void aymo_wave_heading_setup(
    struct aymo_wave_heading* heading,
    uint16_t wave_fmt_type,
    uint16_t channel_count,
    uint16_t sample_bits,
    uint32_t sample_rate,
    uint32_t sample_count
)
{
    assert(heading);
    assert(channel_count > 0u);
    assert(sample_bits > 0u);
    assert(sample_rate > 0u);

    uint16_t sample_byte_size = (sample_bits / 8u);
    uint32_t sample_data_size = (sample_count * channel_count * sample_byte_size);
    assert(sample_data_size < (UINT32_MAX - 32u));

    heading->riff_fourcc[0]         = 'R';
    heading->riff_fourcc[1]         = 'I';
    heading->riff_fourcc[2]         = 'F';
    heading->riff_fourcc[3]         = 'F';
    heading->riff_size              = (32u + sample_data_size);

    heading->wave_fourcc[0]         = 'W';
    heading->wave_fourcc[1]         = 'A';
    heading->wave_fourcc[2]         = 'V';
    heading->wave_fourcc[3]         = 'E';

    heading->wave_fmt_fourcc[0]     = 'f';
    heading->wave_fmt_fourcc[1]     = 'm';
    heading->wave_fmt_fourcc[2]     = 't';
    heading->wave_fmt_fourcc[3]     = ' ';
    heading->wave_fmt_size          = 16u;

    heading->wave_fmt_type          = wave_fmt_type;
    heading->wave_fmt_channel_count = channel_count;
    heading->wave_fmt_sample_rate   = sample_rate;
    heading->wave_fmt_byte_rate     = (sample_byte_size * sample_rate);
    heading->wave_fmt_block_align   = (sample_byte_size * channel_count);
    heading->wave_fmt_sample_bits   = sample_bits;

    heading->wave_data_fourcc[0]    = 'd';
    heading->wave_data_fourcc[1]    = 'a';
    heading->wave_data_fourcc[2]    = 't';
    heading->wave_data_fourcc[3]    = 'a';
    heading->wave_data_size         = sample_data_size;
}


AYMO_CXX_EXTERN_C_END
