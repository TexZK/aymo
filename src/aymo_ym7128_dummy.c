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

#define AYMO_KEEP_SHORTHANDS
#include "aymo_ym7128_dummy.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_ym7128_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_ym7128_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_ym7128_ctor_f)&(aymo_(ctor)),
    (aymo_ym7128_dtor_f)&(aymo_(dtor)),
    (aymo_ym7128_read_f)&(aymo_(read)),
    (aymo_ym7128_write_f)&(aymo_(write)),
    (aymo_ym7128_process_i16_f)&(aymo_(process_i16))
};


const struct aymo_ym7128_vt* aymo_(get_vt)(void)
{
    return &aymo_(vt);
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


void aymo_(process_i16)(struct aymo_(chip)* chip, uint32_t count, const int16_t x[], int16_t y[])
{
    AYMO_UNUSED_VAR(chip);
    AYMO_UNUSED_VAR(count);
    AYMO_UNUSED_VAR(x);
    AYMO_UNUSED_VAR(y);
    assert(chip);
    assert(x);
    assert(y);

    // not supported
}


AYMO_CXX_EXTERN_C_END
