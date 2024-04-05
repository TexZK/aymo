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
#ifndef _include_aymo_ym7128_common_h
#define _include_aymo_ym7128_common_h

#include "aymo_cc.h"

#include <stddef.h>
#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


// Object-oriented API

struct aymo_ym7128_chip;  // forward
typedef uint32_t (*aymo_ym7128_get_sizeof_f)(void);
typedef void (*aymo_ym7128_ctor_f)(struct aymo_ym7128_chip* chip);
typedef void (*aymo_ym7128_dtor_f)(struct aymo_ym7128_chip* chip);
typedef uint8_t (*aymo_ym7128_read_f)(struct aymo_ym7128_chip* chip, uint16_t address);
typedef void (*aymo_ym7128_write_f)(struct aymo_ym7128_chip* chip, uint16_t address, uint8_t value);
typedef void (*aymo_ym7128_process_i16_f)(struct aymo_ym7128_chip* chip, uint32_t count, const int16_t x[], int16_t y[]);

struct aymo_ym7128_vt {
    const char* class_name;
    aymo_ym7128_get_sizeof_f get_sizeof;
    aymo_ym7128_ctor_f ctor;
    aymo_ym7128_dtor_f dtor;
    aymo_ym7128_read_f read;
    aymo_ym7128_write_f write;
    aymo_ym7128_process_i16_f process_i16;
};

struct aymo_ym7128_chip {
    const struct aymo_ym7128_vt* vt;
};


#define AYMO_YM7128_REG_COUNT       31
#define AYMO_YM7128_GAIN_BITS       6
#define AYMO_YM7128_GAIN_COUNT      64
#define AYMO_YM7128_TAP_BITS        5
#define AYMO_YM7128_TAP_COUNT       32
#define AYMO_YM7128_COEFF_BITS      6
#define AYMO_YM7128_KERNEL_LENGTH   19
#define AYMO_YM7128_DELAY_LENGTH    2356
#define AYMO_YM7128_GAIN_UNIT       0x7FFF
#define AYMO_YM7128_GAIN_MASK       0xFFF0
#define AYMO_YM7128_SIGNAL_BITS     14
#define AYMO_YM7128_SIGNAL_MASK     0xFFFC
#define AYMO_YM7128_INPUT_RATE      23550
#define AYMO_YM7128_OUTPUT_RATE     47100


enum aymo_ym7128_reg {
    aymo_ym7128_reg_gl1 = 0,
    aymo_ym7128_reg_gl2,
    aymo_ym7128_reg_gl3,
    aymo_ym7128_reg_gl4,
    aymo_ym7128_reg_gl5,
    aymo_ym7128_reg_gl6,
    aymo_ym7128_reg_gl7,
    aymo_ym7128_reg_gl8,

    aymo_ym7128_reg_gr1,
    aymo_ym7128_reg_gr2,
    aymo_ym7128_reg_gr3,
    aymo_ym7128_reg_gr4,
    aymo_ym7128_reg_gr5,
    aymo_ym7128_reg_gr6,
    aymo_ym7128_reg_gr7,
    aymo_ym7128_reg_gr8,

    aymo_ym7128_reg_vm,
    aymo_ym7128_reg_vc,

    aymo_ym7128_reg_vl,
    aymo_ym7128_reg_vr,

    aymo_ym7128_reg_c0,
    aymo_ym7128_reg_c1,

    aymo_ym7128_reg_t0,
    aymo_ym7128_reg_t1,
    aymo_ym7128_reg_t2,
    aymo_ym7128_reg_t3,
    aymo_ym7128_reg_t4,
    aymo_ym7128_reg_t5,
    aymo_ym7128_reg_t6,
    aymo_ym7128_reg_t7,
    aymo_ym7128_reg_t8
};


AYMO_PUBLIC const int16_t aymo_ym7128_gain[AYMO_YM7128_GAIN_COUNT];
AYMO_PUBLIC const int16_t aymo_ym7128_tap[AYMO_YM7128_TAP_COUNT];
AYMO_PUBLIC const int16_t aymo_ym7128_kernel_linear[AYMO_YM7128_KERNEL_LENGTH];
AYMO_PUBLIC const int16_t aymo_ym7128_kernel_minèhase[AYMO_YM7128_KERNEL_LENGTH];


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_ym7128_common_h
