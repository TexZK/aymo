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
#ifndef _include_aymo_ymf262_x86_avx2_h
#define _include_aymo_ymf262_x86_avx2_h

#include "aymo_cpu.h"
#include "aymo_ymf262_common.h"

#include <stddef.h>

#ifdef AYMO_CPU_SUPPORT_X86_AVX2

AYMO_CXX_EXTERN_C_BEGIN


#undef AYMO_
#undef aymo_
#define AYMO_(_token_)  AYMO_YMF262_X86_AVX2_##_token_
#define aymo_(_token_)  aymo_ymf262_x86_avx2_##_token_


#define AYMO_YMF262_X86_AVX2_SLOT_NUM_MAX           64
#define AYMO_YMF262_X86_AVX2_CHANNEL_NUM_MAX        32
#define AYMO_YMF262_X86_AVX2_SLOT_GROUP_NUM         4
#define AYMO_YMF262_X86_AVX2_SLOT_GROUP_LENGTH      16


AYMO_PRAGMA_SCALAR_STORAGE_ORDER_LITTLE_ENDIAN

// Wave descriptor for single slot
struct aymo_(wave) {
    int16_t wg_phase_mullo;
    int16_t wg_phase_zero;
    int16_t wg_phase_neg;
    int16_t wg_phase_flip;
    int16_t wg_phase_mask;
    int16_t wg_sine_gate;
};

// Waveform enumerator
enum aymo_(wf) {
    aymo_(wf_sin) = 0,
    aymo_(wf_sinup),
    aymo_(wf_sinabs),
    aymo_(wf_sinabsqrt),
    aymo_(wf_sinfast),
    aymo_(wf_sinabsfast),
    aymo_(wf_square),
    aymo_(wf_log)
};


// Connection descriptor for a single slot
struct aymo_(conn) {
    int16_t wg_fbmod_gate;
    int16_t wg_prmod_gate;
    int16_t og_out_gate;
};


// TODO: move reg queue outside YMF262
#ifndef AYMO_YMF262_X86_AVX2_REG_QUEUE_LENGTH
#define AYMO_YMF262_X86_AVX2_REG_QUEUE_LENGTH       256
#endif
#ifndef AYMO_YMF262_X86_AVX2_REG_QUEUE_LATENCY
#define AYMO_YMF262_X86_AVX2_REG_QUEUE_LATENCY      2
#endif

struct aymo_(reg_queue_item) {
    uint16_t address;
    uint8_t value;
};


#define AYMO_YMF262_X86_AVX2_EG_GEN_ATTACK          0
#define AYMO_YMF262_X86_AVX2_EG_GEN_DECAY           1
#define AYMO_YMF262_X86_AVX2_EG_GEN_SUSTAIN         2
#define AYMO_YMF262_X86_AVX2_EG_GEN_RELEASE         3

#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_ATTACK    (1 <<  0)
#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_DECAY     (1 <<  4)
#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_SUSTAIN   (1 <<  8)
#define AYMO_YMF262_X86_AVX2_EG_GEN_MULLO_RELEASE   (1 << 12)
#define AYMO_YMF262_X86_AVX2_EG_GEN_SRLHI           10

#define AYMO_YMF262_X86_AVX2_EG_KEY_NORMAL          (1 << 0)
#define AYMO_YMF262_X86_AVX2_EG_KEY_DRUM            (1 << 8)

// Packed ADSR register values
AYMO_PRAGMA_PACK_PUSH_1
struct aymo_(eg_adsr) {
    uint16_t rr : 4;
    uint16_t sr : 4;
    uint16_t dr : 4;
    uint16_t ar : 4;
};
AYMO_PRAGMA_PACK_POP


// Slot SIMD group status
// Processing order (kinda)
AYMO_ALIGN_V256
struct aymo_(slot_group) {
    // Updated each sample cycle
    vi16x16_t eg_rout;
    vi16x16_t eg_tremolo_am;
    vi16x16_t eg_ksl_sh_tl_x4;
    vi32x8_t pg_phase_lo;
    vi32x8_t pg_phase_hi;
    vi16x16_t pg_phase_out;
    vi16x16_t eg_gen;
    vi16x16_t eg_key;           // bit 8 = drum, bit 0 = normal
    vi16x16_t eg_gen_mullo;     // depends on reg_type for reg_sr
    vi16x16_t eg_adsr;          // struct aymo_(eg_adsr)
    vi16x16_t eg_ks;
    vi32x8_t pg_deltafreq_lo;
    vi32x8_t pg_deltafreq_hi;
    vi16x16_t wg_out;
    vi16x16_t wg_prout;
    vi16x16_t wg_fb_mulhi;
    vi16x16_t wg_prmod_gate;
    vi16x16_t wg_fbmod_gate;
    vi16x16_t wg_phase_mullo;
    vi16x16_t wg_phase_zero;
    vi16x16_t wg_phase_flip;
    vi16x16_t wg_phase_mask;
    vi16x16_t wg_sine_gate;
    vi16x16_t eg_out;
    vi16x16_t wg_phase_neg;
    vi16x16_t eg_sl;
    vi16x16_t og_prout;
    vi16x16_t og_prout_ac;
    vi16x16_t og_prout_bd;
    vi16x16_t og_out_ch_gate_a;
    vi16x16_t og_out_ch_gate_c;
    vi16x16_t og_out_ch_gate_b;
    vi16x16_t og_out_ch_gate_d;

    // Updated infrequently
    vi16x16_t pg_vib;
    vi16x16_t pg_mult_x2;

    // Updated only by writing registers
    vi16x16_t eg_am;
    vi16x16_t og_out_gate;

#ifdef AYMO_DEBUG
    // Variables for debug
    vi16x16_t eg_ksl;
    vi16x16_t eg_rate;
    vi16x16_t eg_inc;
    vi16x16_t wg_fbmod;
    vi16x16_t wg_mod;
#endif  // AYMO_DEBUG
};

// Channel_2xOP SIMD group status
// Processing order (kinda)
AYMO_ALIGN_V256
struct aymo_(ch2x_group) {
    // Updated infrequently
    vi16x16_t pg_fnum;
    vi16x16_t pg_block;

    // Updated only by writing registers
    vi16x16_t eg_ksv;
    vi16x16_t og_ch_gate_a;
    vi16x16_t og_ch_gate_b;
    vi16x16_t og_ch_gate_c;
    vi16x16_t og_ch_gate_d;

#ifdef AYMO_DEBUG
    // Variables for debug
#endif  // AYMO_DEBUG
};

// Chip SIMD and scalar status data
// Processing order (kinda), size/alignment order
AYMO_ALIGN_V256
struct aymo_(chip) {
    struct aymo_ymf262_chip parent;
    uint8_t align32_[sizeof(vi16x16_t) - sizeof(struct aymo_ymf262_chip)];

    // 256-bit data
    struct aymo_(slot_group) sg[AYMO_(SLOT_GROUP_NUM)];
    struct aymo_(ch2x_group) cg[AYMO_(SLOT_GROUP_NUM) / 2];

    vi16x16_t eg_add;
    vi16x16_t wg_mod;
    vu16x16_t eg_incstep;
    vi16x16_t og_acc_a;
    vi16x16_t og_acc_c;
    vi16x16_t og_acc_b;
    vi16x16_t og_acc_d;

    vi16x16_t pg_vib_mulhi;
    vi16x16_t pg_vib_neg;

    // 128-bit data
    vi16x8_t og_out;

    // 64-bit data
    uint64_t eg_timer;
    uint64_t tm_timer;

    // 32-bit data
    uint32_t rq_delay;
    uint32_t og_ch2x_pairing;
    uint32_t og_ch2x_drum;  // FIXME: use this instead of BDh.RYT
    uint32_t ng_noise;

    // 16-bit data
    uint16_t rq_head;
    uint16_t rq_tail;

    // 8-bit data
    uint8_t eg_state;
    uint8_t eg_timerrem;
    uint8_t rm_hh_bit2;
    uint8_t rm_hh_bit3;
    uint8_t rm_hh_bit7;
    uint8_t rm_hh_bit8;
    uint8_t rm_tc_bit3;
    uint8_t rm_tc_bit5;
    uint8_t eg_tremolopos;
    uint8_t eg_tremoloshift;
    uint8_t eg_vibshift;
    uint8_t pg_vibpos;
    uint8_t pad32_[2];

    struct aymo_ymf262_chip_regs chip_regs;
    struct aymo_ymf262_slot_regs slot_regs[AYMO_(SLOT_NUM_MAX)];
    struct aymo_ymf262_chan_regs ch2x_regs[AYMO_(CHANNEL_NUM_MAX)];

    struct aymo_(reg_queue_item) rq_buffer[AYMO_(REG_QUEUE_LENGTH)];

#ifdef AYMO_DEBUG
    // Variables for debug
#endif  // AYMO_DEBUG
};

AYMO_PRAGMA_SCALAR_STORAGE_ORDER_DEFAULT


AYMO_PUBLIC const int8_t aymo_(sgo_side)[16];
AYMO_PUBLIC const int8_t aymo_(sgo_cell)[16];

AYMO_PUBLIC const uint16_t aymo_(eg_incstep_table)[4];

AYMO_PUBLIC const struct aymo_(wave) aymo_(wave_table)[8];
AYMO_PUBLIC const struct aymo_(conn) aymo_(conn_ch2x_table)[2/* cnt */][2/* slot */];
AYMO_PUBLIC const struct aymo_(conn) aymo_(conn_ch4x_table)[4/* cnt */][4/* slot */];
AYMO_PUBLIC const struct aymo_(conn) aymo_(conn_ryt_table)[4][2/* slot */];

AYMO_PUBLIC const uint16_t aymo_(og_prout_ac)[AYMO_(SLOT_GROUP_NUM)];
AYMO_PUBLIC const uint16_t aymo_(og_prout_bd)[AYMO_(SLOT_GROUP_NUM)];

AYMO_PUBLIC const struct aymo_ymf262_vt aymo_(vt);


AYMO_PUBLIC const struct aymo_ymf262_vt* aymo_(get_vt)(void);
AYMO_PUBLIC uint32_t aymo_(get_sizeof)(void);
AYMO_PUBLIC void aymo_(ctor)(struct aymo_(chip)* chip);
AYMO_PUBLIC void aymo_(dtor)(struct aymo_(chip)* chip);
AYMO_PUBLIC uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address);
AYMO_PUBLIC void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC int aymo_(enqueue_write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value);
AYMO_PUBLIC int aymo_(enqueue_delay)(struct aymo_(chip)* chip, uint32_t count);
AYMO_PUBLIC int16_t aymo_(get_output)(struct aymo_(chip)* chip, uint8_t channel);
AYMO_PUBLIC void aymo_(tick)(struct aymo_(chip)* chip, uint32_t count);
AYMO_PUBLIC void aymo_(generate_i16x2)(struct aymo_(chip)* chip, uint32_t count, int16_t y[]);
AYMO_PUBLIC void aymo_(generate_i16x4)(struct aymo_(chip)* chip, uint32_t count, int16_t y[]);
AYMO_PUBLIC void aymo_(generate_f32x2)(struct aymo_(chip)* chip, uint32_t count, float y[]);
AYMO_PUBLIC void aymo_(generate_f32x4)(struct aymo_(chip)* chip, uint32_t count, float y[]);


// Slot group index to Channel group index
static inline
int aymo_(sgi_to_cgi)(int sgi)
{
    return (sgi / 2);
}


// Address to Slot index
static inline
int8_t aymo_(addr_to_slot)(uint16_t address)
{
    unsigned subaddr = ((address & 0x1Fu) | ((address >> 3u) & 0x20u));
    int8_t slot = aymo_ymf262_subaddr_to_slot[subaddr];
    return slot;
}


// Address to Channel_2xOP index
static inline
int8_t aymo_(addr_to_ch2x)(uint16_t address)
{
    unsigned subaddr = ((address & 0x0Fu) | ((address >> 4u) & 0x10u));
    int8_t ch2x = aymo_ymf262_subaddr_to_ch2x[subaddr];
    return ch2x;
}


#ifndef AYMO_KEEP_SHORTHANDS
    #undef AYMO_KEEP_SHORTHANDS
    #undef AYMO_
    #undef aymo_
#endif  // AYMO_KEEP_SHORTHANDS

AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_AVX2

#endif  // _include_aymo_ymf262_x86_avx2_h
