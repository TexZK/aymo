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

#include "aymo_tda8425_common.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_tda8425_none.h"

#include <assert.h>
#include <math.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_tda8425_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_tda8425_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_tda8425_ctor_f)&(aymo_(ctor)),
    (aymo_tda8425_dtor_f)&(aymo_(dtor)),
    (aymo_tda8425_read_f)&(aymo_(read)),
    (aymo_tda8425_write_f)&(aymo_(write)),
    (aymo_tda8425_process_f32_f)&(aymo_(process_f32))
};


const struct aymo_tda8425_vt* aymo_(get_vt)(void)
{
    return &aymo_(vt);
}


uint32_t aymo_(get_sizeof)(void)
{
    return sizeof(struct aymo_(chip));
}


void aymo_(ctor)(struct aymo_(chip)* chip, float sample_rate)
{
    assert(chip);
    assert(sample_rate > .0f);

    for (int i = 0; i < AYMO_(DELAY); ++i) {
        chip->yh[i][0] = .0f;
        chip->yh[i][1] = .0f;
    }

    TDA8425_Chip* emu = &chip->emu;
    TDA8425_Chip_Ctor(emu);

    TDA8425_Chip_Setup(
        emu,
        (TDA8425_Float)sample_rate,
        (TDA8425_Float)TDA8425_Pseudo_C1_Table[TDA8425_Pseudo_Preset_1],
        (TDA8425_Float)TDA8425_Pseudo_C2_Table[TDA8425_Pseudo_Preset_1],
        TDA8425_Tfilter_Mode_Disabled
    );

    TDA8425_Chip_Reset(emu);

    TDA8425_Chip_Write(emu, (TDA8425_Address)TDA8425_Reg_VL, 0xFCu);
    TDA8425_Chip_Write(emu, (TDA8425_Address)TDA8425_Reg_VR, 0xFCu);
    TDA8425_Chip_Write(emu, (TDA8425_Address)TDA8425_Reg_BA, 0xF6u);
    TDA8425_Chip_Write(emu, (TDA8425_Address)TDA8425_Reg_TR, 0xF6u);
    TDA8425_Chip_Write(emu, (TDA8425_Address)TDA8425_Reg_SF, 0xCEu);

    TDA8425_Chip_Start(emu);
}


void aymo_(dtor)(struct aymo_(chip)* chip)
{
    AYMO_UNUSED_VAR(chip);
    assert(chip);
}


uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address)
{
    assert(chip);

    if (address <= (uint16_t)TDA8425_Reg_SF) {
        return TDA8425_Chip_Read(&chip->emu, (TDA8425_Address)address);
    }
    return 0xFFu;
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    assert(chip);

    if (address <= (uint16_t)TDA8425_Reg_SF) {
        TDA8425_Chip_Write(&chip->emu, (TDA8425_Address)address, value);
    }
}


void aymo_(process_f32)(struct aymo_(chip)* chip, uint32_t count, const float x[], float y[])
{
    assert(chip);
    assert(x);
    assert(y);

    TDA8425_Chip* emu = &chip->emu;
    TDA8425_Chip_Process_Data data;
    data.inputs[TDA8425_Source_2][TDA8425_Stereo_L] = (TDA8425_Float)0.f;
    data.inputs[TDA8425_Source_2][TDA8425_Stereo_R] = (TDA8425_Float)0.f;

    while (count--) {
        data.inputs[TDA8425_Source_1][TDA8425_Stereo_L] = (TDA8425_Float)*x++;
        data.inputs[TDA8425_Source_1][TDA8425_Stereo_R] = (TDA8425_Float)*x++;

        TDA8425_Chip_Process(emu, &data);

        for (int i = (AYMO_(DELAY) - 1); i > 0; --i) {
            chip->yh[i][0] = chip->yh[i-1][0];
            chip->yh[i][1] = chip->yh[i-1][1];
        }
        chip->yh[0][0] = (float)data.outputs[TDA8425_Stereo_L];
        chip->yh[0][1] = (float)data.outputs[TDA8425_Stereo_R];

        *y++ = chip->yh[AYMO_(DELAY)-1][0];
        *y++ = chip->yh[AYMO_(DELAY)-1][1];
    }
}


AYMO_CXX_EXTERN_C_END
