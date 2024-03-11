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
#ifndef _include_aymo_tda8425_common_h
#define _include_aymo_tda8425_common_h

#include "aymo_cc.h"

#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


// Object-oriented API

struct aymo_tda8425_chip;  // forward
typedef uint32_t (*aymo_tda8425_get_sizeof_f)(void);
typedef void (*aymo_tda8425_ctor_f)(struct aymo_tda8425_chip* chip, float sample_rate);
typedef void (*aymo_tda8425_dtor_f)(struct aymo_tda8425_chip* chip);
typedef uint8_t (*aymo_tda8425_read_f)(struct aymo_tda8425_chip* chip, uint16_t address);
typedef void (*aymo_tda8425_write_f)(struct aymo_tda8425_chip* chip, uint16_t address, uint8_t value);
typedef void (*aymo_tda8425_process_f32_f)(struct aymo_tda8425_chip* chip, uint32_t count, const float x[], float y[]);

struct aymo_tda8425_vt {
    const char* class_name;
    aymo_tda8425_get_sizeof_f get_sizeof;
    aymo_tda8425_ctor_f ctor;
    aymo_tda8425_dtor_f dtor;
    aymo_tda8425_read_f read;
    aymo_tda8425_write_f write;
    aymo_tda8425_process_f32_f process_f32;
};

struct aymo_tda8425_chip {
    const struct aymo_tda8425_vt* vt;
};


// Math API

typedef double (*aymo_tda8425_math1_f)(double a);
typedef double (*aymo_tda8425_math2_f)(double a, double b);

struct aymo_tda8425_math {
    aymo_tda8425_math1_f cos;
    aymo_tda8425_math1_f fabs;
    aymo_tda8425_math1_f log10;
    aymo_tda8425_math2_f pow;
    aymo_tda8425_math1_f sqrt;
    aymo_tda8425_math1_f tan;
};

// Defines the default math functions, after #include <math.h>
#define AYMO_TDA8425_DEFINE_MATH_DEFAULT(name__)  \
    const struct aymo_tda8425_math name__ = { (cos), (fabs), (log10), (pow), (sqrt), (tan) }


AYMO_PUBLIC const int8_t aymo_tda8425_reg_v_to_db[64];
AYMO_PUBLIC const int8_t aymo_tda8425_reg_ba_to_db[16];
AYMO_PUBLIC const int8_t aymo_tda8425_reg_tr_to_db[16];

AYMO_PUBLIC const float aymo_tda8425_pseudo_preset_c1[3];
AYMO_PUBLIC const float aymo_tda8425_pseudo_preset_c2[3];


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_tda8425_common_h
