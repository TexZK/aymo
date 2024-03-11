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

#include "aymo_tda8425_common.h"

AYMO_CXX_EXTERN_C_BEGIN


const int8_t aymo_tda8425_reg_v_to_db[64] =
{
    -90,  //  0
    -90,  //  1
    -90,  //  2
    -90,  //  3
    -90,  //  4
    -90,  //  5
    -90,  //  6
    -90,  //  7
    -90,  //  8
    -90,  //  9
    -90,  // 10
    -90,  // 11
    -90,  // 12
    -90,  // 13
    -90,  // 14
    -90,  // 15
    -90,  // 16
    -90,  // 17
    -90,  // 18
    -90,  // 19
    -90,  // 20
    -90,  // 21
    -90,  // 22
    -90,  // 23
    -90,  // 24
    -90,  // 25
    -90,  // 26
    -90,  // 27
    -64,  // 28
    -62,  // 29
    -60,  // 30
    -58,  // 31
    -56,  // 32
    -54,  // 33
    -52,  // 34
    -50,  // 35
    -48,  // 36
    -46,  // 37
    -44,  // 38
    -42,  // 39
    -40,  // 40
    -38,  // 41
    -36,  // 42
    -34,  // 43
    -32,  // 44
    -30,  // 45
    -28,  // 46
    -26,  // 47
    -24,  // 48
    -22,  // 49
    -20,  // 50
    -18,  // 51
    -16,  // 52
    -14,  // 53
    -12,  // 54
    -10,  // 55
    - 8,  // 56
    - 6,  // 57
    - 4,  // 58
    - 2,  // 59
    + 0,  // 60
    + 2,  // 61
    + 4,  // 62
    + 6   // 63
};

const int8_t aymo_tda8425_reg_ba_to_db[16] =
{
    -12,  //  0
    -12,  //  1
    -12,  //  2
    - 9,  //  3
    - 6,  //  4
    - 3,  //  5
    + 0,  //  6
    + 3,  //  7
    + 6,  //  8
    + 9,  //  9
    +12,  // 10
    +15,  // 11
    +15,  // 12
    +15,  // 13
    +15,  // 14
    +15   // 15
};

const int8_t aymo_tda8425_reg_tr_to_db[16] =
{
    -12,  //  0
    -12,  //  1
    -12,  //  2
    - 9,  //  3
    - 6,  //  4
    - 3,  //  5
    + 0,  //  6
    + 3,  //  7
    + 6,  //  8
    + 9,  //  9
    +12,  // 10
    +12,  // 11
    +12,  // 12
    +12,  // 13
    +12,  // 14
    +12   // 15
};


const float aymo_tda8425_pseudo_preset_c1[3] =
{
    15.e-9f,
    5.6e-9f,
    5.6e-9f
};

const float aymo_tda8425_pseudo_preset_c2[3] =
{
    15.e-9f,
    47.e-9f,
    68.e-9f
};


AYMO_CXX_EXTERN_C_END
