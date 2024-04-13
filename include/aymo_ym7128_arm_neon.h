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
#ifndef _include_aymo_ym7128_arm_neon_h
#define _include_aymo_ym7128_arm_neon_h

#include "aymo_cpu.h"
#include "aymo_ym7128.h"

#ifdef AYMO_CPU_SUPPORT_ARM_NEON

AYMO_CXX_EXTERN_C_BEGIN


#undef AYMO_
#undef aymo_
#define AYMO_(_token_)  AYMO_YM7128_ARM_NEON_##_token_
#define aymo_(_token_)  aymo_ym7128_arm_neon_##_token_


// Chip SIMD and scalar status data
// Processing order (kinda), size/alignment order
AYMO_ALIGN_V128
struct aymo_(chip) {
    struct aymo_ym7128_chip parent;
    uint8_t align_[sizeof(vi16x8_t) - sizeof(struct aymo_ym7128_chip)];

    // 128-bit data
    vi16x8_t xxv;
    vi16x8_t kk1;
    vi16x8_t kk2;
    vi16x8_t kkm;
    vi16x8_t ti;
    vi16x8_t kgl;
    vi16x8_t kgr;
    vi16x8_t kv;

    vi16x8_t zc;
    vi16x8_t zb;
    vi16x8_t kf;
    vi16x8_t ke;
    vi16x8_t za;
    vi16x8_t kd;
    vi16x8_t kc;
    vi16x8_t kb;
    vi16x8_t ka;

    // 16-bit data
    int16_t uh[AYMO_YM7128_DELAY_LENGTH];

    // 8-bit data
    uint8_t regs[AYMO_YM7128_REG_COUNT];

    uint8_t pad32_[3];
};


AYMO_PUBLIC const struct aymo_ym7128_vt* aymo_(get_vt)(void);
AYMO_PUBLIC uint32_t aymo_(get_sizeof)(void);
AYMO_PUBLIC void aymo_(ctor)(struct aymo_(chip)* chip);
AYMO_PUBLIC void aymo_(dtor)(struct aymo_(chip)* chip);
AYMO_PUBLIC uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address);
AYMO_PUBLIC void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC void aymo_(process_i16)(struct aymo_(chip)* chip, uint32_t count, const int16_t x[], int16_t y[]);


#ifndef AYMO_KEEP_SHORTHANDS
    #undef AYMO_KEEP_SHORTHANDS
    #undef AYMO_
    #undef aymo_
#endif  // AYMO_KEEP_SHORTHANDS

AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_ARM_NEON

#endif  // _include_aymo_ym7128_arm_neon_h
