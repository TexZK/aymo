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
#ifndef _include_aymo_ymf262_common_h
#define _include_aymo_ymf262_common_h

#include "aymo_cc.h"

#include <stdint.h>

AYMO_CXX_EXTERN_C_BEGIN


// Object-oriented API

struct aymo_ymf262_chip;  // forward
typedef uint32_t (*aymo_ymf262_get_sizeof_f)(void);
typedef void (*aymo_ymf262_ctor_f)(struct aymo_ymf262_chip* chip);
typedef void (*aymo_ymf262_dtor_f)(struct aymo_ymf262_chip* chip);
typedef uint8_t (*aymo_ymf262_read_f)(struct aymo_ymf262_chip* chip, uint16_t address);
typedef void (*aymo_ymf262_write_f)(struct aymo_ymf262_chip* chip, uint16_t address, uint8_t value);
typedef int (*aymo_ymf262_enqueue_write_f)(struct aymo_ymf262_chip* chip, uint16_t address, uint8_t value);
typedef int (*aymo_ymf262_enqueue_delay_f)(struct aymo_ymf262_chip* chip, uint32_t count);
typedef int16_t (*aymo_ymf262_get_output_f)(struct aymo_ymf262_chip* chip, uint8_t channel);
typedef void (*aymo_ymf262_tick_f)(struct aymo_ymf262_chip* chip, uint32_t count);
typedef void (*aymo_ymf262_generate_i16x2_f)(struct aymo_ymf262_chip* chip, uint32_t count, int16_t y[]);
typedef void (*aymo_ymf262_generate_i16x4_f)(struct aymo_ymf262_chip* chip, uint32_t count, int16_t y[]);
typedef void (*aymo_ymf262_generate_f32x2_f)(struct aymo_ymf262_chip* chip, uint32_t count, float y[]);
typedef void (*aymo_ymf262_generate_f32x4_f)(struct aymo_ymf262_chip* chip, uint32_t count, float y[]);

struct aymo_ymf262_vt {
    const char* class_name;
    aymo_ymf262_get_sizeof_f get_sizeof;
    aymo_ymf262_ctor_f ctor;
    aymo_ymf262_dtor_f dtor;
    aymo_ymf262_read_f read;
    aymo_ymf262_write_f write;
    aymo_ymf262_enqueue_write_f enqueue_write;
    aymo_ymf262_enqueue_delay_f enqueue_delay;
    aymo_ymf262_get_output_f get_output;
    aymo_ymf262_tick_f tick;
    aymo_ymf262_generate_i16x2_f generate_i16x2;
    aymo_ymf262_generate_i16x4_f generate_i16x4;
    aymo_ymf262_generate_f32x2_f generate_f32x2;
    aymo_ymf262_generate_f32x4_f generate_f32x4;
};

struct aymo_ymf262_chip {
    const struct aymo_ymf262_vt* vt;
};


// Limits
#define AYMO_YMF262_SLOT_NUM            36
#define AYMO_YMF262_CHANNEL_NUM         18

#define AYMO_YMF262_SLOT_NUM_MAX        64
#define AYMO_YMF262_CHANNEL_NUM_MAX     32

#ifndef AYMO_YMF262_REG_SAMPLE_LATENCY
#define AYMO_YMF262_REG_SAMPLE_LATENCY  2
#endif


// Registers; little-endian bitfields
AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN

AYMO_PRAGMA_PACK_PUSH_1


struct aymo_ymf262_reg_01h {
    uint8_t lsitest_lo : 8;
};
struct aymo_ymf262_reg_101h {
    uint8_t lsitest_hi : 6;
    uint8_t _7_6 : 2;
};
struct aymo_ymf262_reg_02h {
    uint8_t timer1 : 8;
};
struct aymo_ymf262_reg_03h {
    uint8_t timer2 : 8;
};
struct aymo_ymf262_reg_04h {
    uint8_t st1 : 1;
    uint8_t st2 : 1;
    uint8_t _4_2 : 3;
    uint8_t mt2 : 1;
    uint8_t mt1 : 1;
    uint8_t rst : 1;
};
struct aymo_ymf262_reg_104h {
    uint8_t conn : 6;
    uint8_t _7_6 : 2;
};
struct aymo_ymf262_reg_105h {
    uint8_t newm : 1;
    uint8_t stereo : 1;
    uint8_t simd : 1;
    uint8_t _7_3 : 5;
};
struct aymo_ymf262_reg_08h {
    uint8_t _5_0 : 6;
    uint8_t nts : 1;
    uint8_t csm : 1;
};
struct aymo_ymf262_reg_20h {
    uint8_t mult : 4;
    uint8_t ksr : 1;
    uint8_t egt : 1;
    uint8_t vib : 1;
    uint8_t am : 1;
};
struct aymo_ymf262_reg_40h {
    uint8_t tl : 6;
    uint8_t ksl : 2;
};
struct aymo_ymf262_reg_60h {
    uint8_t dr : 4;
    uint8_t ar : 4;
};
struct aymo_ymf262_reg_80h {
    uint8_t rr : 4;
    uint8_t sl : 4;
};
struct aymo_ymf262_reg_A0h {
    uint8_t fnum_lo : 8;
};
struct aymo_ymf262_reg_B0h {
    uint8_t fnum_hi : 2;
    uint8_t block : 3;
    uint8_t kon : 1;
    uint8_t _7_6 : 2;
};
struct aymo_ymf262_reg_BDh {
    uint8_t hh : 1;
    uint8_t tc : 1;
    uint8_t tom : 1;
    uint8_t sd : 1;
    uint8_t bd : 1;
    uint8_t ryt : 1;
    uint8_t dvb : 1;
    uint8_t dam : 1;
};
struct aymo_ymf262_reg_C0h {
    uint8_t cnt : 1;
    uint8_t fb : 3;
    uint8_t cha : 1;
    uint8_t chb : 1;
    uint8_t chc : 1;
    uint8_t chd : 1;
};
struct aymo_ymf262_reg_E0h {
    uint8_t ws : 3;
    uint8_t _7_3 : 5;
};

struct aymo_ymf262_chip_regs {
    struct aymo_ymf262_reg_01h reg_01h;
    struct aymo_ymf262_reg_02h reg_02h;
    struct aymo_ymf262_reg_03h reg_03h;
    struct aymo_ymf262_reg_04h reg_04h;
    struct aymo_ymf262_reg_08h reg_08h;
    struct aymo_ymf262_reg_BDh reg_BDh;
    struct aymo_ymf262_reg_101h reg_101h;
    struct aymo_ymf262_reg_104h reg_104h;
    struct aymo_ymf262_reg_105h reg_105h;
    uint8_t _pad32[3];
};

struct aymo_ymf262_slot_regs {
    struct aymo_ymf262_reg_20h reg_20h;
    struct aymo_ymf262_reg_40h reg_40h;
    struct aymo_ymf262_reg_60h reg_60h;
    struct aymo_ymf262_reg_80h reg_80h;
    struct aymo_ymf262_reg_E0h reg_E0h;
    uint8_t _pad32[3];
};

struct aymo_ymf262_chan_regs {
    struct aymo_ymf262_reg_A0h reg_A0h;
    struct aymo_ymf262_reg_B0h reg_B0h;
    struct aymo_ymf262_reg_C0h reg_C0h;
    struct aymo_ymf262_reg_C0h reg_D0h;
};


// Packed ADSR register values
struct aymo_ymf262_adsr {
    uint16_t rr : 4;
    uint16_t sr : 4;
    uint16_t dr : 4;
    uint16_t ar : 4;
};


AYMO_PRAGMA_PACK_POP

AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


AYMO_PUBLIC const int16_t aymo_ymf262_exp_x2_table[256 + 4];
AYMO_PUBLIC const int16_t aymo_ymf262_logsin_table[256 + 4];

AYMO_PUBLIC const int8_t aymo_ymf262_word_to_slot[AYMO_YMF262_SLOT_NUM_MAX];
AYMO_PUBLIC const int8_t aymo_ymf262_slot_to_word[AYMO_YMF262_SLOT_NUM_MAX];

AYMO_PUBLIC const int8_t aymo_ymf262_word_to_ch2x[AYMO_YMF262_SLOT_NUM_MAX];
AYMO_PUBLIC const int8_t aymo_ymf262_ch2x_to_word[AYMO_YMF262_SLOT_NUM_MAX / 2][2/* slot */];

AYMO_PUBLIC const int8_t aymo_ymf262_word_to_ch4x[AYMO_YMF262_SLOT_NUM_MAX];
AYMO_PUBLIC const int8_t aymo_ymf262_ch4x_to_word[AYMO_YMF262_SLOT_NUM_MAX / 4][4/* slot */];
AYMO_PUBLIC const int8_t aymo_ymf262_ch4x_to_pair[AYMO_YMF262_CHANNEL_NUM_MAX / 2][2/* slot */];
AYMO_PUBLIC const int8_t aymo_ymf262_ch2x_paired[AYMO_YMF262_CHANNEL_NUM_MAX];

AYMO_PUBLIC const int8_t aymo_ymf262_subaddr_to_slot[AYMO_YMF262_SLOT_NUM_MAX];
AYMO_PUBLIC const int8_t aymo_ymf262_slot_to_subaddr[AYMO_YMF262_SLOT_NUM_MAX];

AYMO_PUBLIC const int8_t aymo_ymf262_subaddr_to_ch2x[AYMO_YMF262_CHANNEL_NUM_MAX];
AYMO_PUBLIC const int8_t aymo_ymf262_ch2x_to_subaddr[AYMO_YMF262_CHANNEL_NUM_MAX];

AYMO_PUBLIC const int8_t aymo_ymf262_pg_mult_x2_table[16];

AYMO_PUBLIC const int8_t aymo_ymf262_eg_ksl_table[16];
AYMO_PUBLIC const int8_t aymo_ymf262_eg_kslsh_table[4];


AYMO_CXX_EXTERN_C_END

#endif  // _include_aymo_ymf262_common_h
