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

#include "aymo_ymf262.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_ymf262_dummy.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_ymf262_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_ymf262_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_ymf262_ctor_f)&(aymo_(ctor)),
    (aymo_ymf262_dtor_f)&(aymo_(dtor)),
    (aymo_ymf262_read_f)&(aymo_(read)),
    (aymo_ymf262_write_f)&(aymo_(write)),
    (aymo_ymf262_enqueue_write_f)&(aymo_(enqueue_write)),
    (aymo_ymf262_enqueue_delay_f)&(aymo_(enqueue_delay)),
    (aymo_ymf262_get_output_f)&(aymo_(get_output)),
    (aymo_ymf262_tick_f)&(aymo_(tick)),
    (aymo_ymf262_generate_i16x2_f)&(aymo_(generate_i16x2)),
    (aymo_ymf262_generate_i16x4_f)&(aymo_(generate_i16x4)),
    (aymo_ymf262_generate_f32x2_f)&(aymo_(generate_f32x2)),
    (aymo_ymf262_generate_f32x4_f)&(aymo_(generate_f32x4))
};


const struct aymo_ymf262_vt* aymo_(get_vt)(void)
{
    return &(aymo_(vt));
}


uint32_t aymo_(get_sizeof)(void)
{
    return sizeof(struct aymo_(chip));
}


void aymo_(ctor)(struct aymo_(chip)* chip)
{
    AYMO_UNUSED_VAR(chip);
    assert(chip);
}


void aymo_(dtor)(struct aymo_(chip)* chip)
{
    AYMO_UNUSED_VAR(chip);
    assert(chip);
}


uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address)
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(address);
    assert(chip);

    // not supported
    return 0u;
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(address);
    AYMO_UNUSED_VAR(value);
    assert(chip);

    // not supported
}


int aymo_(enqueue_write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(address);
    AYMO_UNUSED_VAR(value);
    assert(chip);

    // not supported
    return 1;
}


int aymo_(enqueue_delay)(struct aymo_(chip)* chip, uint32_t count)
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    assert(chip);

    // not supported
    (void)chip;
    (void)count;
    return 1;
}


int16_t aymo_(get_output)(struct aymo_(chip)* chip, uint8_t channel)
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(channel);
    assert(chip);

    // not supported
    return 0;
}


void aymo_(tick)(struct aymo_(chip)* chip, uint32_t count)
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    assert(chip);

    // not supported
}


void aymo_(generate_i16x2)(struct aymo_(chip)* chip, uint32_t count, int16_t y[])
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    AYMO_UNUSED_VAR(y);
    assert(chip);

    // not supported
}


void aymo_(generate_i16x4)(struct aymo_(chip)* chip, uint32_t count, int16_t y[])
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    AYMO_UNUSED_VAR(y);
    assert(chip);

    // not supported
}


void aymo_(generate_f32x2)(struct aymo_(chip)* chip, uint32_t count, float y[])
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    AYMO_UNUSED_VAR(y);
    assert(chip);

    // not supported
}


void aymo_(generate_f32x4)(struct aymo_(chip)* chip, uint32_t count, float y[])
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    AYMO_UNUSED_VAR(y);
    assert(chip);

    // not supported
}


AYMO_CXX_EXTERN_C_END
