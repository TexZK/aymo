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
#ifndef _include_aymo_tda8425_x86_avx2_h
#define _include_aymo_tda8425_x86_avx2_h

#include "aymo_cpu.h"
#include "aymo_tda8425_common.h"

#include <stddef.h>

#ifdef AYMO_CPU_SUPPORT_X86_AVX2

AYMO_CXX_EXTERN_C_BEGIN


#undef AYMO_
#undef aymo_
#define AYMO_(_token_)  AYMO_TDA8425_X86_AVX2_##_token_
#define aymo_(_token_)  aymo_tda8425_x86_avx2_##_token_


// Chip SIMD and scalar status data
// Processing order (kinda), size/alignment order
AYMO_ALIGN_V256
struct aymo_(chip) {
    struct aymo_tda8425_chip parent;
    uint8_t align_[sizeof(vf32x8_t) - sizeof(struct aymo_tda8425_chip)];

    // Vector data
    vf32x8_t hb1;
    vf32x8_t hb2;
    vf32x8_t kb2;

    vf32x8_t ha1;
    vf32x8_t ha2;
    vf32x8_t ka2;

    vf32x8_t hb0;
    vf32x8_t kb1;

    vf32x8_t ha0;
    vf32x8_t ka1;

    vf32x8_t krl;
    vf32x8_t klr;

    vf32x8_t kb0;

    vf32x8_t kv;

    // 32-bit data
    float sample_rate;  // [Hz]
    float pseudo_c1;  // [F]
    float pseudo_c2;  // [F]

    // 8-bit data
    uint8_t reg_vl;
    uint8_t reg_vr;
    uint8_t reg_ba;
    uint8_t reg_tr;
    uint8_t reg_pp;
    uint8_t reg_sf;
    uint8_t pad32_[2];
};


AYMO_PUBLIC const struct aymo_tda8425_vt* aymo_(get_vt)(void);
AYMO_PUBLIC uint32_t aymo_(get_sizeof)(void);
AYMO_PUBLIC void aymo_(ctor)(struct aymo_(chip)* chip, float sample_rate);
AYMO_PUBLIC void aymo_(dtor)(struct aymo_(chip)* chip);
AYMO_PUBLIC uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address);
AYMO_PUBLIC void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC void aymo_(process_f32)(struct aymo_(chip)* chip, uint32_t count, const float x[], float y[]);


#ifndef AYMO_KEEP_SHORTHANDS
    #undef AYMO_KEEP_SHORTHANDS
    #undef AYMO_
    #undef aymo_
#endif  // AYMO_KEEP_SHORTHANDS

AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_AVX2

#endif  // _include_aymo_tda8425_x86_avx2_h
