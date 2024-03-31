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
#include "aymo_ym7128_none.h"

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
    assert(chip);

    YM7128B_ChipFixed* emu = &chip->emu;
    YM7128B_ChipFixed_Ctor(emu);
    YM7128B_ChipFixed_Reset(emu);

    // Initialize as pass-through
    YM7128B_ChipFixed_Write(emu, (YM7128B_Address)YM7128B_Reg_GL1, 0x3Fu);
    YM7128B_ChipFixed_Write(emu, (YM7128B_Address)YM7128B_Reg_GR1, 0x3Fu);
    YM7128B_ChipFixed_Write(emu, (YM7128B_Address)YM7128B_Reg_VM, 0x3Fu);
    YM7128B_ChipFixed_Write(emu, (YM7128B_Address)YM7128B_Reg_VL, 0x3Fu);
    YM7128B_ChipFixed_Write(emu, (YM7128B_Address)YM7128B_Reg_VR, 0x3Fu);

    YM7128B_ChipFixed_Start(emu);
}


void aymo_(dtor)(struct aymo_(chip)* chip)
{
    assert(chip);

    YM7128B_ChipFixed* emu = &chip->emu;
    YM7128B_ChipFixed_Stop(emu);
    YM7128B_ChipFixed_Dtor(emu);
}


uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address)
{
    assert(chip);

    if (address <= (uint16_t)YM7128B_Address_Max) {
        return YM7128B_ChipFixed_Read(&chip->emu, (YM7128B_Address)address);
    }
    return 0x00u;
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    assert(chip);

    if (address <= (uint16_t)YM7128B_Address_Max) {
        YM7128B_ChipFixed_Write(&chip->emu, (YM7128B_Address)address, value);
    }
}


void aymo_(process_i16)(struct aymo_(chip)* chip, uint32_t count, const int16_t x[], int16_t y[])
{
    assert(chip);
    assert(x);
    assert(y);
    if AYMO_UNLIKELY(!count) return;

    YM7128B_ChipFixed* emu = &chip->emu;
    YM7128B_ChipFixed_Process_Data data;

    const int16_t* xe = &x[count];

    while AYMO_LIKELY(x != xe) {
        data.inputs[YM7128B_InputChannel_Mono] = *x++;

        YM7128B_ChipFixed_Process(emu, &data);

        for (int k = 0; k < YM7128B_Oversampler_Factor; ++k) {
            for (int c = 0; c < YM7128B_OutputChannel_Count; ++c) {
                *y++ = data.outputs[c][k];
            }
        }
    }
}


AYMO_CXX_EXTERN_C_END
