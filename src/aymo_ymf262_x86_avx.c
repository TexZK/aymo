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

#include <assert.h>
#include "aymo_cpu_x86_sse41_inline.h"  // actually using SSE4.1
#include "aymo_ymf262.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_ymf262_x86_avx.h"

#ifdef AYMO_CPU_SUPPORT_X86_AVX

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_ymf262_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_ymf262_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_ymf262_ctor_f)&(aymo_(ctor)),
    (aymo_ymf262_dtor_f)&(aymo_(dtor)),
    (aymo_ymf262_read_f)&(aymo_(read)),
    (aymo_ymf262_write_f)&(aymo_(write)),
    (aymo_ymf262_enqueue_write_f)&(aymo_(enqueue_write)),
    (aymo_ymf262_enqueue_delay_f)&(aymo_(enqueue_delay)),
    (aymo_ymf262_get_output_f)&(aymo_(get_output)),
    (aymo_ymf262_tick_f)&(aymo_(tick)),
    (aymo_ymf262_generate_i16x2_f)&(aymo_(generate_i16x2)),
    (aymo_ymf262_generate_i16x4_f)&(aymo_(generate_i16x4)),
    (aymo_ymf262_generate_f32x2_f)&(aymo_(generate_f32x2)),
    (aymo_ymf262_generate_f32x4_f)&(aymo_(generate_f32x4))
};


// 32-bit Slot Group side (lo/hi)
const int8_t aymo_(sgo_side)[8] =
{
    0, 0, 0, 0,  1, 1, 1, 1
};

// 32-bit Slot Group cell
const int8_t aymo_(sgo_cell)[8] =
{
    0, 1, 2, 3,  0, 1, 2, 3
};


const uint16_t aymo_(eg_incstep_table)[4] =
{
    ((1 << 15) | (1 << 14) | (1 << 13)),
    ((0 << 15) | (0 << 14) | (1 << 13)),
    ((0 << 15) | (1 << 14) | (1 << 13)),
    ((0 << 15) | (0 << 14) | (0 << 13))
};


// Wave descriptors
const struct aymo_(wave) aymo_(wave_table)[8] =  // TODO: share bits; select vit shifts
{
    { 1,  0x0000,  0x0200,  0x0100,  0x00FF,  -1 },
    { 1,  0x0200,  0x0000,  0x0100,  0x00FF,  -1 },
    { 1,  0x0000,  0x0000,  0x0100,  0x00FF,  -1 },
    { 1,  0x0100,  0x0000,  0x0100,  0x00FF,  -1 },
    { 2,  0x0400,  0x0200,  0x0100,  0x00FF,  -1 },
    { 2,  0x0400,  0x0000,  0x0100,  0x00FF,  -1 },
    { 1,  0x0000,  0x0200,  0x0200,  0x0001,   0 },
    { 8,  0x0000,  0x1000,  0x1000,  0x1FFF,   0 }
};


// 2-channel connection descriptors
const struct aymo_(conn) aymo_(conn_ch2x_table)[2/* cnt */][2/* slot */] =
{
    {
        { -1,   0,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,  -1 },
        {  0,   0,  -1 }
    },
};

// 4-channel connection descriptors
const struct aymo_(conn) aymo_(conn_ch4x_table)[4/* cnt */][4/* slot */] =
{
    {
        { -1,   0,   0 },
        {  0,  -1,   0 },
        {  0,  -1,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,   0 },
        {  0,  -1,  -1 },
        {  0,   0,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,  -1 },
        {  0,   0,   0 },
        {  0,  -1,   0 },
        {  0,  -1,  -1 }
    },
    {
        { -1,   0,  -1 },
        {  0,   0,   0 },
        {  0,  -1,  -1 },
        {  0,   0,  -1 }
    },
};

// Rhythm connection descriptors
const struct aymo_(conn) aymo_(conn_ryt_table)[4][2/* slot */] =
{
    // Channel 6: BD, FM
    {
        { -1,   0,   0 },
        {  0,  -1,  -1 }
    },
    // Channel 6: BD, AM
    {
        { -1,   0,   0 },
        {  0,   0,  -1 }
    },
    // Channel 7: HH + SD
    {
        {  0,   0,  -1 },
        {  0,   0,  -1 }
    },
    // Channel 8: TT + TC
    {
        {  0,   0,  -1 },
        {  0,   0,  -1 }
    }
};


// Slot mask output delay for outputs A and C
const uint8_t aymo_(og_prout_ac)[AYMO_(SLOT_GROUP_NUM)] =  // TODO: TBV: use a shared mask; use bit 7 as mask flag; <<=1 for the next flag
{
    0xF8,
    0xF8,
    0xF8,
    0xFF,
    0xF8,
    0xFF,
    0xF8,
    0xFF
};


// Slot mask output delay for outputs B and D
const uint8_t aymo_(og_prout_bd)[AYMO_(SLOT_GROUP_NUM)] =  // TODO: TBV: use a shared mask; use bit 7 as mask flag; <<=1 for the next flag
{
    0x88,
    0xF8,
    0x88,
    0xF8,
    0x88,
    0xFF,
    0x88,
    0xFF
};


// Updates phase generator
static inline
void aymo_(pg_update_deltafreq)(
    struct aymo_(chip)* chip,
    struct aymo_(ch2x_group)* cg,
    struct aymo_(slot_group)* sg
)
{
    // Update phase
    vi16_t fnum = cg->pg_fnum;
    vi16_t range = vand(fnum, vset1(7 << 7));
    range = vmulihi(range, vand(sg->pg_vib, chip->pg_vib_mulhi));
    range = vsub(vxor(range, chip->pg_vib_neg), chip->pg_vib_neg);  // flip sign
    fnum = vadd(fnum, range);

    vi32_t zero = vsetz();
    vi32_t fnum_lo = vunpacklo(fnum, zero);
    vi32_t fnum_hi = vunpackhi(fnum, zero);
    vi32_t block_sll_lo = vunpacklo(cg->pg_block, zero);
    vi32_t block_sll_hi = vunpackhi(cg->pg_block, zero);
    vi32_t basefreq_lo = vvsrli(vvsllv(fnum_lo, block_sll_lo), 1);
    vi32_t basefreq_hi = vvsrli(vvsllv(fnum_hi, block_sll_hi), 1);
    vi32_t pg_mult_x2_lo = vunpacklo(sg->pg_mult_x2, zero);
    vi32_t pg_mult_x2_hi = vunpackhi(sg->pg_mult_x2, zero);
    vi32_t deltafreq_lo = vvsrli(vvmullo(basefreq_lo, pg_mult_x2_lo), 1);
    vi32_t deltafreq_hi = vvsrli(vvmullo(basefreq_hi, pg_mult_x2_hi), 1);
    sg->pg_deltafreq_lo = deltafreq_lo;
    sg->pg_deltafreq_hi = deltafreq_hi;
}


// Updates noise generator
static inline
void aymo_(ng_update)(struct aymo_(chip)* chip, unsigned times)
{
    // Update noise
    uint32_t noise = chip->ng_noise;
    while (times--) {
        uint32_t n_bit = (((noise >> 14) ^ noise) & 1);
        noise = ((noise >> 1) | (n_bit << 22));
    }
    chip->ng_noise = noise;
}


// Updates rhythm manager, slot group 1
static inline
void aymo_(rm_update_sg1)(struct aymo_(chip)* chip)
{
    struct aymo_(slot_group)* sg = &chip->sg[1];

    if AYMO_UNLIKELY(chip->chip_regs.reg_BDh.ryt) {
        // Double rhythm outputs
        vi16_t ryt_slot_mask = vsetr(-1, -1, -1, 0, 0, 0, 0, 0);
        vi16_t wave_out = vand(sg->wg_out, ryt_slot_mask);
        chip->og_acc_a = vadd(chip->og_acc_a, vand(wave_out, sg->og_out_ch_gate_a));
        chip->og_acc_b = vadd(chip->og_acc_b, vand(wave_out, sg->og_out_ch_gate_b));
        chip->og_acc_c = vadd(chip->og_acc_c, vand(wave_out, sg->og_out_ch_gate_c));
        chip->og_acc_d = vadd(chip->og_acc_d, vand(wave_out, sg->og_out_ch_gate_d));
    }

    vi16_t phase = sg->pg_phase_out;
    uint16_t phase13 = (uint16_t)vextract(phase, 1);

    // Update noise bits
    chip->rm_hh_bit2 = ((phase13 >> 2) & 1);
    chip->rm_hh_bit3 = ((phase13 >> 3) & 1);
    chip->rm_hh_bit7 = ((phase13 >> 7) & 1);
    chip->rm_hh_bit8 = ((phase13 >> 8) & 1);

    if AYMO_UNLIKELY(chip->chip_regs.reg_BDh.ryt) {
        // Calculate noise bit
        uint16_t rm_xor = (
            (chip->rm_hh_bit2 ^ chip->rm_hh_bit7) |
            (chip->rm_hh_bit3 ^ chip->rm_tc_bit5) |
            (chip->rm_tc_bit3 ^ chip->rm_tc_bit5)
        );

        // Update HH
        uint16_t noise = (uint16_t)chip->ng_noise;
        phase13 = (rm_xor << 9);
        if (rm_xor ^ (noise & 1)) {
            phase13 |= 0xD0;
        } else {
            phase13 |= 0x34;
        }
        phase = vinsert(phase, (int16_t)phase13, 1);

        sg->pg_phase_out = phase;
    }
}


// Updates rhythm manager, slot group 3
static inline
void aymo_(rm_update_sg3)(struct aymo_(chip)* chip)
{
    struct aymo_(slot_group)* sg = &chip->sg[3];

    if AYMO_UNLIKELY(chip->chip_regs.reg_BDh.ryt) {
        // Double rhythm outputs
        vi16_t ryt_slot_mask = vsetr(-1, -1, -1, 0, 0, 0, 0, 0);
        vi16_t wave_out = vand(sg->wg_out, ryt_slot_mask);
        chip->og_acc_a = vadd(chip->og_acc_a, vand(wave_out, sg->og_out_ch_gate_a));
        chip->og_acc_b = vadd(chip->og_acc_b, vand(wave_out, sg->og_out_ch_gate_b));
        chip->og_acc_c = vadd(chip->og_acc_c, vand(wave_out, sg->og_out_ch_gate_c));
        chip->og_acc_d = vadd(chip->og_acc_d, vand(wave_out, sg->og_out_ch_gate_d));

        // Calculate noise bit
        uint16_t rm_xor = (
            (chip->rm_hh_bit2 ^ chip->rm_hh_bit7) |
            (chip->rm_hh_bit3 ^ chip->rm_tc_bit5) |
            (chip->rm_tc_bit3 ^ chip->rm_tc_bit5)
        );
        vi16_t phase = sg->pg_phase_out;

        // Update SD
        uint16_t noise = (uint16_t)chip->ng_noise;
        uint16_t phase16 = (
            ((uint16_t)chip->rm_hh_bit8 << 9) |
            ((uint16_t)(chip->rm_hh_bit8 ^ (noise & 1)) << 8)
        );
        phase = vinsert(phase, (int16_t)phase16, 1);

        // Update TC
        uint32_t phase17 = vextract(phase, 2);
        chip->rm_tc_bit3 = ((phase17 >> 3) & 1);
        chip->rm_tc_bit5 = ((phase17 >> 5) & 1);
        phase17 = ((rm_xor << 9) | 0x80);
        phase = vinsert(phase, (int16_t)phase17, 2);

        sg->pg_phase_out = phase;
    }
}


// Updates slot generators
static
void aymo_(sg_update)(
    struct aymo_(chip)* chip,
    struct aymo_(slot_group)* sg
)
{
    // EG: Compute envelope output
    vi16_t sg_eg_rout = sg->eg_rout;
    sg->eg_out = vadd(vadd(sg_eg_rout, sg->eg_tremolo_am), sg->eg_ksl_sh_tl_x4);

    // PG: Compute phase output
    vi32_t phase_out_mask = vvset1(0xFFFF);
    vi32_t phase_out_lo = vvand(vvsrli(sg->pg_phase_lo, 9), phase_out_mask);
    vi32_t phase_out_hi = vvand(vvsrli(sg->pg_phase_hi, 9), phase_out_mask);
    vi16_t phase_out = vvpackus(phase_out_lo, phase_out_hi);
    sg->pg_phase_out = phase_out;

    // EG: Compute rate
    vi16_t eg_prgen = sg->eg_gen;
    vi16_t eg_gen_rel = vcmpeq(eg_prgen, vset1(AYMO_(EG_GEN_RELEASE)));
    vi16_t notreset = vcmpz(vand(sg->eg_key, eg_gen_rel));
    vi16_t eg_gen_mullo = vblendv(vset1(AYMO_(EG_GEN_MULLO_ATTACK)), sg->eg_gen_mullo, notreset);
    vi16_t reg_rate = vu2i(vmululo(vi2u(sg->eg_adsr), vi2u(eg_gen_mullo)));  // move to top nibble
    vi16_t rate_temp = vand(reg_rate, vset1((int16_t)0xF000));  // keep top nibble
    rate_temp = vsrli(rate_temp, AYMO_(EG_GEN_SRLHI));
    vi16_t rate = vadd(sg->eg_ks, rate_temp);
    vi16_t rate_lo = vand(rate, vset1(3));
    vi16_t rate_hi = vsrli(rate, 2);
    rate_hi = vmini(rate_hi, vset1(15));

    // PG: Update phase
    vi32_t notreset_lo = vunpacklo(notreset, notreset);
    vi32_t notreset_hi = vunpackhi(notreset, notreset);
    vi32_t pg_phase_lo = vvand(notreset_lo, sg->pg_phase_lo);
    vi32_t pg_phase_hi = vvand(notreset_hi, sg->pg_phase_hi);
    sg->pg_phase_lo = vvadd(pg_phase_lo, sg->pg_deltafreq_lo);
    sg->pg_phase_hi = vvadd(pg_phase_hi, sg->pg_deltafreq_hi);

    // EG: Compute shift (< 12)
    vi16_t eg_shift = vadd(rate_hi, chip->eg_add);
    vi16_t rate_pre_lt12 = vor(vslli(rate_lo, 1), vset1(8));
    vi16_t shift_lt12 = vsrlv(rate_pre_lt12, vsubsu(vset1(15), eg_shift));
    vi16_t eg_state = vset1((int16_t)chip->eg_state);
    shift_lt12 = vand(shift_lt12, eg_state);

    // WG: Compute feedback and modulation inputs
    vi16_t fbsum = vslli(vadd(sg->wg_out, sg->wg_prout), 1);
    vi16_t fbsum_sh = vmulihi(fbsum, sg->wg_fb_mulhi);
    vi16_t prmod = vand(chip->wg_mod, sg->wg_prmod_gate);
    vi16_t fbmod = vand(fbsum_sh, sg->wg_fbmod_gate);
    sg->wg_prout = sg->wg_out;

    // WG: Compute operator phase input
    vi16_t modsum = vadd(fbmod, prmod);
    vi16_t phase = vadd(phase_out, modsum);

    // EG: Compute shift (>= 12)
    vu16_t rate_lo_muluhi = vi2u(vslli(vpow2m1lt4(rate_lo), 1));
    vi16_t incstep_ge12 = vand(vu2i(vmuluhi(chip->eg_incstep, rate_lo_muluhi)), vset1(1));
    vi16_t shift_ge12 = vadd(vand(rate_hi, vset1(3)), incstep_ge12);
    shift_ge12 = vmini(shift_ge12, vset1(3));
    shift_ge12 = vblendv(shift_ge12, eg_state, vcmpz(shift_ge12));

    vi16_t shift = vblendv(shift_lt12, shift_ge12, vcmpgt(rate_hi, vset1(11)));
    shift = vandnot(vcmpz(rate_temp), shift);

    // EG: Instant attack
    vi16_t eg_rout = sg_eg_rout;
    eg_rout = vandnot(vandnot(notreset, vcmpeq(rate_hi, vset1(15))), eg_rout);

    // WG: Process phase
    vi16_t phase_sped = vu2i(vmululo(vi2u(phase), sg->wg_phase_mullo));
    vi16_t phase_gate = vcmpz(vand(phase_sped, sg->wg_phase_zero));
    vi16_t phase_flip = vcmpp(vand(phase_sped, sg->wg_phase_flip));
    vi16_t phase_mask = sg->wg_phase_mask;
    vi16_t phase_xor = vand(phase_flip, phase_mask);
    vi16_t phase_idx = vxor(phase_sped, phase_xor);
    phase_out = vand(vand(phase_gate, phase_mask), phase_idx);

    // EG: Envelope off
    vi16_t eg_off = vcmpgt(sg_eg_rout, vset1(0x01F7));
    vi16_t eg_gen_natk_and_nrst = vand(vcmpp(eg_prgen), notreset);
    eg_rout = vblendv(eg_rout, vset1(0x01FF), vand(eg_gen_natk_and_nrst, eg_off));

    // WG: Compute logsin variant
    vi16_t phase_lo = phase_out;  // vgather() masks to low byte
    vi16_t logsin_val = vgather(aymo_ymf262_logsin_table, phase_lo);
    logsin_val = vblendv(vset1(0x1000), logsin_val, phase_gate);

    // EG: Compute common increment not in attack state
    vi16_t eg_inc_natk_cond = vand(vand(notreset, vcmpz(eg_off)), vcmpp(shift));
    vi16_t eg_inc_natk = vand(eg_inc_natk_cond, vpow2m1lt4(shift));
    vi16_t eg_gen = eg_prgen;

    // WG: Compute exponential output
    vi16_t exp_in = vblendv(phase_out, logsin_val, sg->wg_sine_gate);
    vi16_t exp_level = vadd(exp_in, vslli(sg->eg_out, 3));
    exp_level = vmini(exp_level, vset1(0x1FFF));
    vi16_t exp_level_lo = exp_level;  // vgather() masks to low byte
    vi16_t exp_level_hi = vsrli(exp_level, 8);
    vi16_t exp_value = vgather(aymo_ymf262_exp_x2_table, exp_level_lo);
    vi16_t exp_out = vsrlv(exp_value, exp_level_hi);

    // EG: Move attack to decay state
    vi16_t eg_inc_atk_cond = vand(vand(vcmpp(sg->eg_key), vcmpp(shift)),
                                  vand(vcmpz(eg_prgen), vcmpgt(vset1(15), rate_hi)));
    vi16_t eg_inc_atk_ninc = vsrlv(sg_eg_rout, vsub(vset1(4), shift));
    vi16_t eg_inc = vandnot(eg_inc_atk_ninc, eg_inc_atk_cond);
    vi16_t eg_gen_atk_to_dec = vcmpz(vor(eg_prgen, sg_eg_rout));
    eg_gen = vsub(eg_gen, eg_gen_atk_to_dec);  // 0 --> 1
    eg_inc = vblendv(eg_inc_natk, eg_inc, vcmpz(eg_prgen));
    eg_inc = vandnot(eg_gen_atk_to_dec, eg_inc);

    // WG: Compute operator wave output
    vi16_t wave_pos = vcmpz(vand(phase_sped, sg->wg_phase_neg));
    vi16_t wave_neg = vandnot(wave_pos, phase_gate);
    vi16_t wave_out = vxor(exp_out, wave_neg);
    sg->wg_out = wave_out;
    chip->wg_mod = wave_out;

    // EG: Move decay to sustain state
    vi16_t eg_gen_dec = vcmpeq(eg_prgen, vset1(AYMO_(EG_GEN_DECAY)));
    vi16_t sl_hit = vcmpeq(vsrli(sg_eg_rout, 4), sg->eg_sl);
    vi16_t eg_gen_dec_to_sus = vand(eg_gen_dec, sl_hit);
    eg_gen = vsub(eg_gen, eg_gen_dec_to_sus);  // 1 --> 2
    eg_inc = vandnot(eg_gen_dec_to_sus, eg_inc);

    // WG: Update chip output accumulators, with quirky slot output delay
    vi16_t og_out_ac = vblendv(wave_out, sg->og_prout, sg->og_prout_ac);
    vi16_t og_out_bd = vblendv(wave_out, sg->og_prout, sg->og_prout_bd);
    sg->og_prout = wave_out;
    chip->og_acc_a = vadd(chip->og_acc_a, vand(og_out_ac, sg->og_out_ch_gate_a));
    chip->og_acc_c = vadd(chip->og_acc_c, vand(og_out_ac, sg->og_out_ch_gate_c));
    chip->og_acc_b = vadd(chip->og_acc_b, vand(og_out_bd, sg->og_out_ch_gate_b));
    chip->og_acc_d = vadd(chip->og_acc_d, vand(og_out_bd, sg->og_out_ch_gate_d));

    // EG: Move back to attack state
    eg_gen = vand(notreset, eg_gen);  // * --> 0

    // EG: Move to release state
    eg_gen = vor(eg_gen, vsrli(vcmpz(sg->eg_key), 14));  // * --> 3

    // EG: Update envelope generator
    eg_rout = vadd(eg_rout, eg_inc);
    eg_rout = vand(eg_rout, vset1(0x01FF));
    sg->eg_rout = eg_rout;
    sg->eg_gen = eg_gen;
    sg->eg_gen_mullo = vsllv(vset1(1), vslli(eg_gen, 2));

#ifdef AYMO_DEBUG
    sg->eg_rate = rate;
    sg->eg_inc = eg_inc;
    sg->wg_fbmod = fbsum_sh;
    sg->wg_mod = modsum;
#endif
}


// Clear output accumulators
static inline
void aymo_(og_clear)(struct aymo_(chip)* chip)
{
    chip->og_acc_a = vsetz();
    chip->og_acc_b = vsetz();
    chip->og_acc_c = vsetz();
    chip->og_acc_d = vsetz();
}


// Updates output mixdown
static inline
void aymo_(og_update)(struct aymo_(chip)* chip)
{
    vi16x8_t one = _mm_set1_epi16(1);
    vi32x4_t tot_a = _mm_madd_epi16(chip->og_acc_a, one);
    vi32x4_t tot_b = _mm_madd_epi16(chip->og_acc_b, one);
    vi32x4_t tot_c = _mm_madd_epi16(chip->og_acc_c, one);
    vi32x4_t tot_d = _mm_madd_epi16(chip->og_acc_d, one);

    tot_a = _mm_add_epi32(tot_a, _mm_shuffle_epi32(tot_a, _MM_SHUFFLE(2, 3, 0, 1)));
    tot_b = _mm_add_epi32(tot_b, _mm_shuffle_epi32(tot_b, _MM_SHUFFLE(2, 3, 0, 1)));
    tot_c = _mm_add_epi32(tot_c, _mm_shuffle_epi32(tot_c, _MM_SHUFFLE(2, 3, 0, 1)));
    tot_d = _mm_add_epi32(tot_d, _mm_shuffle_epi32(tot_d, _MM_SHUFFLE(2, 3, 0, 1)));

    tot_a = _mm_add_epi32(tot_a, _mm_shuffle_epi32(tot_a, _MM_SHUFFLE(1, 0, 3, 2)));
    tot_b = _mm_add_epi32(tot_b, _mm_shuffle_epi32(tot_b, _MM_SHUFFLE(1, 0, 3, 2)));
    tot_c = _mm_add_epi32(tot_c, _mm_shuffle_epi32(tot_c, _MM_SHUFFLE(1, 0, 3, 2)));
    tot_d = _mm_add_epi32(tot_d, _mm_shuffle_epi32(tot_d, _MM_SHUFFLE(1, 0, 3, 2)));

    vi32x4_t tot_ab = _mm_blend_epi16(tot_a, tot_b, 0xCC);
    vi32x4_t tot_cd = _mm_blend_epi16(tot_c, tot_d, 0x33);
    vi32x4_t tot_abcd = _mm_blend_epi16(tot_ab, tot_cd, 0xF0);
    vi16x8_t sat_abcd = _mm_packs_epi32(tot_abcd, tot_abcd);

    vi16x8_t old_abcd = _mm_shuffle_epi32(chip->og_out, _MM_SHUFFLE(1, 0, 3, 2));
    vi16x8_t out_abcd = _mm_blend_epi16(old_abcd, sat_abcd, 0xF5);

    chip->og_out = out_abcd;
}


// Updates timer management
static inline
void aymo_(tm_update)(struct aymo_(chip)* chip)
{
    // Update tremolo
    if AYMO_UNLIKELY((chip->tm_timer & 0x3F) == 0x3F) {
        chip->eg_tremolopos = ((chip->eg_tremolopos + 1) % 210);

        uint16_t eg_tremolopos = chip->eg_tremolopos;
        if (eg_tremolopos >= 105) {
            eg_tremolopos = (210 - eg_tremolopos);
        }
        vi16_t eg_tremolo = vset1((int16_t)(eg_tremolopos >> chip->eg_tremoloshift));

        for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
            struct aymo_(slot_group)* sg = &chip->sg[sgi];
            sg->eg_tremolo_am = vand(eg_tremolo, sg->eg_am);
        }
    }

    // Update vibrato
    if AYMO_UNLIKELY((chip->tm_timer & 0x3FF) == 0x3FF) {
        chip->pg_vibpos = ((chip->pg_vibpos + 1) & 7);
        uint8_t vibpos = chip->pg_vibpos;
        int16_t pg_vib_mulhi = (0x10000 >> 7);
        int16_t pg_vib_neg = 0;

        if (!(vibpos & 3)) {
            pg_vib_mulhi = 0;
        }
        else if (vibpos & 1) {
            pg_vib_mulhi >>= 1;
        }
        pg_vib_mulhi >>= chip->eg_vibshift;
        pg_vib_mulhi &= 0x7F80;

        if (vibpos & 4) {
            pg_vib_neg = -1;
        }
        chip->pg_vib_mulhi = vset1(pg_vib_mulhi);
        chip->pg_vib_neg = vset1(pg_vib_neg);

        for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
            int cgi = aymo_(sgi_to_cgi)(sgi);
            struct aymo_(ch2x_group)* cg = &chip->cg[cgi];
            struct aymo_(slot_group)* sg = &chip->sg[sgi];
            aymo_(pg_update_deltafreq)(chip, cg, sg);
        }
    }

    chip->tm_timer++;
    uint16_t eg_incstep = aymo_(eg_incstep_table)[chip->tm_timer & 3];
    chip->eg_incstep = vi2u(vset1((int16_t)eg_incstep));

    // Update timed envelope patterns
    int16_t eg_shift = (int16_t)uffsll(chip->eg_timer);
    int16_t eg_add = ((eg_shift > 13) ? 0 : eg_shift);
    chip->eg_add = vset1(eg_add);

    // Update envelope timer and flip state
    if (chip->eg_state | chip->eg_timerrem) {
        if (chip->eg_timer < ((1ULL << AYMO_YMF262_SLOT_NUM) - 1ULL)) {
            chip->eg_timer++;
            chip->eg_timerrem = 0;
        }
        else {
            chip->eg_timer = 0;
            chip->eg_timerrem = 1;
        }
    }
    chip->eg_state ^= 1;
}


// Updates the register queue
static inline
void aymo_(rq_update)(struct aymo_(chip)* chip)
{
    if (chip->rq_delay) {
        if (--chip->rq_delay) {
            return;
        }
    }
    if (chip->rq_head != chip->rq_tail) {
        struct aymo_(reg_queue_item)* item = &chip->rq_buffer[chip->rq_head];

        if (item->address & 0x8000u) {
            chip->rq_delay = AYMO_(REG_QUEUE_LATENCY);
            chip->rq_delay += (((uint32_t)(item->address & 0x7FFFu) << 16) | item->value);
        }
        else {
            aymo_(write)(chip, item->address, item->value);
        }

        if (++chip->rq_head >= AYMO_(REG_QUEUE_LENGTH)) {
            chip->rq_head = 0;
        }
    }
}


static
void aymo_(tick_once)(struct aymo_(chip)* chip)
{
    int sgi;

    // Clear output accumulators
    aymo_(og_clear)(chip);

    // Process slot group 0
    sgi = 0;
    aymo_(sg_update)(chip, &chip->sg[sgi]);

    // Process slot group 2
    sgi = 2;
    aymo_(sg_update)(chip, &chip->sg[sgi]);

    // Process slot group 4
    sgi = 4;
    aymo_(sg_update)(chip, &chip->sg[sgi]);

    // Process slot group 6
    sgi = 6;
    aymo_(sg_update)(chip, &chip->sg[sgi]);

    // Process slot group 1
    sgi = 1;
    aymo_(sg_update)(chip, &chip->sg[sgi]);
    aymo_(ng_update)(chip, (36 - 3));  // slot 16 --> slot 13
    aymo_(rm_update_sg1)(chip);

    // Process slot group 3
    sgi = 3;
    aymo_(sg_update)(chip, &chip->sg[sgi]);
    aymo_(ng_update)(chip, 3);  // slot 13 --> slot 16
    aymo_(rm_update_sg3)(chip);

    if AYMO_UNLIKELY(chip->process_all_slots) {
        // Process slot group 5
        sgi = 5;
        aymo_(sg_update)(chip, &chip->sg[sgi]);

        // Process slot group 7
        sgi = 7;
        aymo_(sg_update)(chip, &chip->sg[sgi]);
    }

    // Update outputs
    aymo_(og_update)(chip);

    // Update timers
    aymo_(tm_update)(chip);

    // Dequeue registers
    aymo_(rq_update)(chip);
}


static
void aymo_(eg_update_ksl)(struct aymo_(chip)* chip, int word)
{
    int slot = aymo_ymf262_word_to_slot[word];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi);
    struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_ymf262_reg_40h* reg_40h = &(chip->slot_regs[slot].reg_40h);

    int16_t pg_fnum = vextractv(cg->pg_fnum, sgo);
    int16_t pg_fnum_hn = ((pg_fnum >> 6) & 15);

    int ch2x = aymo_ymf262_word_to_ch2x[aymo_ymf262_slot_to_word[slot]];
    int16_t eg_block = (int16_t)(chip->ch2x_regs[ch2x].reg_B0h.block);
    int16_t eg_ksl = aymo_ymf262_eg_ksl_table[pg_fnum_hn];
    eg_ksl = ((eg_ksl << 2) - ((8 - eg_block) << 5));
    if (eg_ksl < 0) {
        eg_ksl = 0;
    }
    int16_t eg_kslsh = aymo_ymf262_eg_kslsh_table[reg_40h->ksl];
    int16_t eg_ksl_sh = (eg_ksl >> eg_kslsh);

    int16_t eg_tl_x4 = ((int16_t)reg_40h->tl << 2);

    int16_t eg_ksl_sh_tl_x4 = (eg_ksl_sh + eg_tl_x4);
    vinsertv(sg->eg_ksl_sh_tl_x4, eg_ksl_sh_tl_x4, sgo);

#ifdef AYMO_DEBUG
    vinsertv(sg->eg_ksl, eg_ksl, sgo);
#endif
}


static
void aymo_(chip_pg_update_nts)(struct aymo_(chip)* chip)
{
    for (int slot = 0; slot < AYMO_(SLOT_NUM_MAX); ++slot) {
        int word = aymo_ymf262_slot_to_word[slot];
        int ch2x = aymo_ymf262_word_to_ch2x[word];
        struct aymo_ymf262_reg_A0h* reg_A0h = &(chip->ch2x_regs[ch2x].reg_A0h);
        struct aymo_ymf262_reg_B0h* reg_B0h = &(chip->ch2x_regs[ch2x].reg_B0h);
        struct aymo_ymf262_reg_08h* reg_08h = &(chip->chip_regs.reg_08h);
        int16_t pg_fnum = (int16_t)(reg_A0h->fnum_lo | ((uint16_t)reg_B0h->fnum_hi << 8));
        int16_t eg_ksv = ((reg_B0h->block << 1) | ((pg_fnum >> (9 - reg_08h->nts)) & 1));

        int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
        int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
        int cgi = aymo_(sgi_to_cgi)(sgi);
        struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
        struct aymo_(slot_group)* sg = &(chip->sg[sgi]);

        struct aymo_ymf262_reg_20h* reg_20h = &(chip->slot_regs[slot].reg_20h);
        int16_t ks = (eg_ksv >> ((reg_20h->ksr ^ 1) << 1));

        vinsertv(cg->eg_ksv, eg_ksv, sgo);
        vinsertv(sg->eg_ks,  ks,     sgo);
    }
}


static
void aymo_(pg_update_fnum)(
    struct aymo_(chip)* chip, int ch2x,
    int16_t pg_fnum, int16_t eg_ksv, int16_t pg_block
)
{
    int word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
    int sgi0 = (word0 / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word0 % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi0);
    struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);

    vinsertv(cg->pg_block, pg_block, sgo);
    vinsertv(cg->pg_fnum, pg_fnum, sgo);
    vinsertv(cg->eg_ksv, eg_ksv, sgo);

    struct aymo_(slot_group)* sg0 = &(chip->sg[sgi0]);
    int slot0 = aymo_ymf262_word_to_slot[word0];
    struct aymo_ymf262_reg_20h* reg_20h0 = &(chip->slot_regs[slot0].reg_20h);
    int16_t ks0 = (eg_ksv >> ((reg_20h0->ksr ^ 1) << 1));
    vinsertv(sg0->eg_ks, ks0, sgo);
    aymo_(eg_update_ksl)(chip, word0);
    aymo_(pg_update_deltafreq)(chip, cg, sg0);

    int word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
    int sgi1 = (word1 / AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg1 = &(chip->sg[sgi1]);
    int slot1 = aymo_ymf262_word_to_slot[word1];
    struct aymo_ymf262_reg_20h* reg_20h1 = &(chip->slot_regs[slot1].reg_20h);
    int16_t ks1 = (eg_ksv >> ((reg_20h1->ksr ^ 1) << 1));
    vinsertv(sg1->eg_ks, ks1, sgo);
    aymo_(eg_update_ksl)(chip, word1);
    aymo_(pg_update_deltafreq)(chip, cg, sg1);
}


static
void aymo_(ch2x_update_fnum)(struct aymo_(chip)* chip, int ch2x, int8_t ch2p)
{
    struct aymo_ymf262_reg_A0h* reg_A0h = &(chip->ch2x_regs[ch2x].reg_A0h);
    struct aymo_ymf262_reg_B0h* reg_B0h = &(chip->ch2x_regs[ch2x].reg_B0h);
    struct aymo_ymf262_reg_08h* reg_08h = &(chip->chip_regs.reg_08h);
    int16_t pg_fnum = (int16_t)(reg_A0h->fnum_lo | ((uint16_t)reg_B0h->fnum_hi << 8));
    int16_t pg_block = (int16_t)reg_B0h->block;
    int16_t eg_ksv = ((pg_block << 1) | ((pg_fnum >> (9 - reg_08h->nts)) & 1));

    aymo_(pg_update_fnum)(chip, ch2x, pg_fnum, eg_ksv, pg_block);

    if (ch2p >= 0) {
        aymo_(pg_update_fnum)(chip, ch2p, pg_fnum, eg_ksv, pg_block);
    }
}


static inline
void aymo_(eg_key_on)(struct aymo_(chip)* chip, int word, int16_t mode)
{
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &chip->sg[sgi];
    int16_t eg_key = vextractv(sg->eg_key, sgo);
    eg_key |= mode;
    vinsertv(sg->eg_key, eg_key, sgo);
}


static inline
void aymo_(eg_key_off)(struct aymo_(chip)* chip, int word, int16_t mode)
{
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &chip->sg[sgi];
    int16_t eg_key = vextractv(sg->eg_key, sgo);
    eg_key &= (int16_t)~mode;
    vinsertv(sg->eg_key, eg_key, sgo);
}


static
void aymo_(ch2x_key_on)(struct aymo_(chip)* chip, int ch2x)
{
    if (chip->chip_regs.reg_105h.newm) {
        unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
        unsigned ch2x_is_drum    = (chip->og_ch2x_drum    & (1UL << ch2x));
        int ch2p = aymo_ymf262_ch2x_paired[ch2x];
        int ch2x_is_secondary = (ch2p < ch2x);

        if (ch2x_is_pairing && !ch2x_is_secondary) {
            int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
            int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
            int ch2p_word0 = aymo_ymf262_ch2x_to_word[ch2p][0];
            int ch2p_word1 = aymo_ymf262_ch2x_to_word[ch2p][1];
            aymo_(eg_key_on)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2p_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2p_word1, AYMO_(EG_KEY_NORMAL));
        }
        else if (!ch2x_is_pairing || ch2x_is_drum) {
            int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
            int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
            aymo_(eg_key_on)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_on)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
        }
    }
    else {
        int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
        int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
        aymo_(eg_key_on)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
        aymo_(eg_key_on)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
    }
}


static
void aymo_(ch2x_key_off)(struct aymo_(chip)* chip, int ch2x)
{
    if (chip->chip_regs.reg_105h.newm) {
        unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
        unsigned ch2x_is_drum    = (chip->og_ch2x_drum    & (1UL << ch2x));
        int ch2p = aymo_ymf262_ch2x_paired[ch2x];
        int ch2x_is_secondary = (ch2p < ch2x);

        if (ch2x_is_pairing && !ch2x_is_secondary) {
            int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
            int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
            int ch2p_word0 = aymo_ymf262_ch2x_to_word[ch2p][0];
            int ch2p_word1 = aymo_ymf262_ch2x_to_word[ch2p][1];
            aymo_(eg_key_off)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2p_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2p_word1, AYMO_(EG_KEY_NORMAL));
        }
        else if (!ch2x_is_pairing || ch2x_is_drum) {
            int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
            int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
            aymo_(eg_key_off)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
            aymo_(eg_key_off)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
        }
    }
    else {
        int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
        int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
        aymo_(eg_key_off)(chip, ch2x_word0, AYMO_(EG_KEY_NORMAL));
        aymo_(eg_key_off)(chip, ch2x_word1, AYMO_(EG_KEY_NORMAL));
    }
}


static
void aymo_(cm_rewire_slot)(struct aymo_(chip)* chip, int word, const struct aymo_(conn)* conn)
{
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &chip->sg[sgi];
    vinsertv(sg->wg_fbmod_gate, conn->wg_fbmod_gate, sgo);
    vinsertv(sg->wg_prmod_gate, conn->wg_prmod_gate, sgo);
    int16_t og_out_gate = conn->og_out_gate;
    vinsertv(sg->og_out_gate, og_out_gate, sgo);

    int cgi = aymo_(sgi_to_cgi)(sgi);
    struct aymo_(ch2x_group)* cg = &chip->cg[cgi];
    vinsertv(sg->og_out_ch_gate_a, (vextractv(cg->og_ch_gate_a, sgo) & og_out_gate), sgo);
    vinsertv(sg->og_out_ch_gate_b, (vextractv(cg->og_ch_gate_b, sgo) & og_out_gate), sgo);
    vinsertv(sg->og_out_ch_gate_c, (vextractv(cg->og_ch_gate_c, sgo) & og_out_gate), sgo);
    vinsertv(sg->og_out_ch_gate_d, (vextractv(cg->og_ch_gate_d, sgo) & og_out_gate), sgo);
}


static
void aymo_(cm_rewire_ch2x)(struct aymo_(chip)* chip, int ch2x)
{
    if (chip->chip_regs.reg_105h.newm && (chip->og_ch2x_pairing & (1UL << ch2x))) {
        int ch2p = aymo_ymf262_ch2x_paired[ch2x];
        int ch2x_is_secondary = (ch2p < ch2x);
        if (ch2x_is_secondary) {
            int t = ch2x;
            ch2x = ch2p;
            ch2p = t;
        }
        unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
        unsigned ch2p_cnt = chip->ch2x_regs[ch2p].reg_C0h.cnt;
        unsigned ch4x_cnt = ((ch2x_cnt << 1) | ch2p_cnt);
        const struct aymo_(conn)* ch4x_conn = aymo_(conn_ch4x_table)[ch4x_cnt];
        aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][0], &ch4x_conn[0]);
        aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][1], &ch4x_conn[1]);
        aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2p][0], &ch4x_conn[2]);
        aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2p][1], &ch4x_conn[3]);
    }
    else {
        unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
        const struct aymo_(conn)* ch2x_conn = aymo_(conn_ch2x_table)[ch2x_cnt];
        aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][0], &ch2x_conn[0]);
        aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][1], &ch2x_conn[1]);
    }
}


static
void aymo_(cm_rewire_conn)(
    struct aymo_(chip)* chip,
    const struct aymo_ymf262_reg_104h* reg_104h_prev
)
{
    struct aymo_ymf262_reg_104h* reg_104h = &chip->chip_regs.reg_104h;
    unsigned diff = (reg_104h_prev ? (reg_104h_prev->conn ^ reg_104h->conn) : 0xFF);

    for (int ch4x = 0; ch4x < (AYMO_(CHANNEL_NUM_MAX) / 2); ++ch4x) {
        if (diff & (1 << ch4x)) {
            int ch2x = aymo_ymf262_ch4x_to_pair[ch4x][0];
            int ch2p = aymo_ymf262_ch4x_to_pair[ch4x][1];

            if (reg_104h->conn & (1 << ch4x)) {
                chip->og_ch2x_pairing |= ((1UL << ch2x) | (1UL << ch2p));

                unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
                unsigned ch2p_cnt = chip->ch2x_regs[ch2p].reg_C0h.cnt;
                unsigned ch4x_cnt = ((ch2x_cnt << 1) | ch2p_cnt);
                const struct aymo_(conn)* ch4x_conn = aymo_(conn_ch4x_table)[ch4x_cnt];
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][0], &ch4x_conn[0]);
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][1], &ch4x_conn[1]);
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2p][0], &ch4x_conn[2]);
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2p][1], &ch4x_conn[3]);
            }
            else {
                chip->og_ch2x_pairing &= ~((1UL << ch2x) | (1UL << ch2p));

                unsigned ch2x_cnt = chip->ch2x_regs[ch2x].reg_C0h.cnt;
                const struct aymo_(conn)* ch2x_conn = aymo_(conn_ch2x_table)[ch2x_cnt];
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][0], &ch2x_conn[0]);
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2x][1], &ch2x_conn[1]);

                unsigned ch2p_cnt = chip->ch2x_regs[ch2p].reg_C0h.cnt;
                const struct aymo_(conn)* ch2p_conn = aymo_(conn_ch2x_table)[ch2p_cnt];
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2p][0], &ch2p_conn[0]);
                aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[ch2p][1], &ch2p_conn[1]);
            }
        }
    }
}


static
void aymo_(cm_rewire_rhythm)(
    struct aymo_(chip)* chip,
    const struct aymo_ymf262_reg_BDh* reg_BDh_prev
)
{
    const struct aymo_ymf262_reg_BDh reg_BDh_zero = { 0, 0, 0, 0, 0, 0, 0, 0 };
    const struct aymo_ymf262_reg_BDh* reg_BDh = &chip->chip_regs.reg_BDh;
    int force_update = 0;

    if (reg_BDh->ryt) {
        if (!reg_BDh_prev->ryt) {
            // Apply special connection for rhythm mode
            unsigned ch6_cnt = chip->ch2x_regs[6].reg_C0h.cnt;
            const struct aymo_(conn)* ch6_conn = aymo_(conn_ryt_table)[ch6_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[6][0], &ch6_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[6][1], &ch6_conn[1]);

            const struct aymo_(conn)* ch7_conn = aymo_(conn_ryt_table)[2];
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[7][0], &ch7_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[7][1], &ch7_conn[1]);

            const struct aymo_(conn)* ch8_conn = aymo_(conn_ryt_table)[3];
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[8][0], &ch8_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[8][1], &ch8_conn[1]);

            force_update = 1;
        }
    }
    else {
        if (reg_BDh_prev->ryt) {
            // Apply standard Channel_2xOP connection
            unsigned ch6_cnt = chip->ch2x_regs[6].reg_C0h.cnt;
            const struct aymo_(conn)* ch6_conn = aymo_(conn_ch2x_table)[ch6_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[6][0], &ch6_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[6][1], &ch6_conn[1]);

            unsigned ch7_cnt = chip->ch2x_regs[7].reg_C0h.cnt;
            const struct aymo_(conn)* ch7_conn = aymo_(conn_ch2x_table)[ch7_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[7][0], &ch7_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[7][1], &ch7_conn[1]);

            unsigned ch8_cnt = chip->ch2x_regs[8].reg_C0h.cnt;
            const struct aymo_(conn)* ch8_conn = aymo_(conn_ch2x_table)[ch8_cnt];
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[8][0], &ch8_conn[0]);
            aymo_(cm_rewire_slot)(chip, aymo_ymf262_ch2x_to_word[8][1], &ch8_conn[1]);

            reg_BDh = &reg_BDh_zero;  // force all keys off
            force_update = 1;
        }
    }

    if ((reg_BDh->hh != reg_BDh_prev->hh) || force_update) {
        int word_hh = aymo_ymf262_ch2x_to_word[7][0];
        if (reg_BDh->hh) {
            aymo_(eg_key_on)(chip, word_hh, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_hh, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->tc != reg_BDh_prev->tc) || force_update) {
        int word_tc = aymo_ymf262_ch2x_to_word[8][1];
        if (reg_BDh->tc) {
            aymo_(eg_key_on)(chip, word_tc, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_tc, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->tom != reg_BDh_prev->tom) || force_update) {
        int word_tom = aymo_ymf262_ch2x_to_word[8][0];
        if (reg_BDh->tom) {
            aymo_(eg_key_on)(chip, word_tom, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_tom, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->sd != reg_BDh_prev->sd) || force_update) {
        int word_sd = aymo_ymf262_ch2x_to_word[7][1];
        if (reg_BDh->sd) {
            aymo_(eg_key_on)(chip, word_sd, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_sd, AYMO_(EG_KEY_DRUM));
        }
    }

    if ((reg_BDh->bd != reg_BDh_prev->bd) || force_update) {
        int word_bd0 = aymo_ymf262_ch2x_to_word[6][0];
        int word_bd1 = aymo_ymf262_ch2x_to_word[6][1];
        if (reg_BDh->bd) {
            aymo_(eg_key_on)(chip, word_bd0, AYMO_(EG_KEY_DRUM));
            aymo_(eg_key_on)(chip, word_bd1, AYMO_(EG_KEY_DRUM));
        } else {
            aymo_(eg_key_off)(chip, word_bd0, AYMO_(EG_KEY_DRUM));
            aymo_(eg_key_off)(chip, word_bd1, AYMO_(EG_KEY_DRUM));
        }
    }
}


static
void aymo_(write_00h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    switch (address) {
    case 0x01: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_01h) = value;
        break;
    }
    case 0x02: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_02h) = value;
        break;
    }
    case 0x03: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_03h) = value;
        break;
    }
    case 0x04: {
        *(uint8_t*)(void*)&(chip->chip_regs.reg_04h) = value;
        break;
    }
    case 0x104: {
        struct aymo_ymf262_reg_104h reg_104h_prev = chip->chip_regs.reg_104h;
        *(uint8_t*)(void*)&(chip->chip_regs.reg_104h) = value;
        aymo_(cm_rewire_conn)(chip, &reg_104h_prev);
        break;
    }
    case 0x105: {
        struct aymo_ymf262_reg_105h reg_105h_prev = chip->chip_regs.reg_105h;
        *(uint8_t*)(void*)&(chip->chip_regs.reg_105h) = value;
        if (chip->chip_regs.reg_105h.newm != reg_105h_prev.newm) {
            ;
        }
        break;
    }
    case 0x08: {
        struct aymo_ymf262_reg_08h reg_08h_prev = chip->chip_regs.reg_08h;
        *(uint8_t*)(void*)&(chip->chip_regs.reg_08h) = value;
        if (chip->chip_regs.reg_08h.nts != reg_08h_prev.nts) {
            aymo_(chip_pg_update_nts)(chip);
        }
        break;
    }
    }
}


static
void aymo_(write_20h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int sgi = (aymo_ymf262_slot_to_word[slot] / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (aymo_ymf262_slot_to_word[slot] % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi);
    struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_ymf262_reg_20h* reg_20h = &(chip->slot_regs[slot].reg_20h);
    struct aymo_ymf262_reg_20h reg_20h_prev = *reg_20h;
    *(uint8_t*)(void*)reg_20h = value;
    unsigned update_deltafreq = 0;

    if (reg_20h->mult != reg_20h_prev.mult) {
        int16_t pg_mult_x2 = aymo_ymf262_pg_mult_x2_table[reg_20h->mult];
        vinsertv(sg->pg_mult_x2, pg_mult_x2, sgo);
        update_deltafreq = 1;  // force
    }

    if (reg_20h->ksr != reg_20h_prev.ksr) {
        int16_t eg_ksv = vextractv(cg->eg_ksv, sgo);
        int16_t eg_ks = (eg_ksv >> ((reg_20h->ksr ^ 1) << 1));
        vinsertv(sg->eg_ks, eg_ks, sgo);
    }

    if (reg_20h->egt != reg_20h_prev.egt) {
        int16_t eg_adsr_word = vextractv(sg->eg_adsr, sgo);
        struct aymo_(eg_adsr)* eg_adsr = (struct aymo_(eg_adsr)*)(void*)&eg_adsr_word;
        eg_adsr->sr = (reg_20h->egt ? 0 : chip->slot_regs[slot].reg_80h.rr);
        vinsertv(sg->eg_adsr, eg_adsr_word, sgo);
    }

    if (reg_20h->vib != reg_20h_prev.vib) {
        int16_t pg_vib = -(int16_t)reg_20h->vib;
        vinsertv(sg->pg_vib, pg_vib, sgo);
        update_deltafreq = 1;  // force
    }

    if (reg_20h->am != reg_20h_prev.am) {
        int16_t eg_am = -(int16_t)reg_20h->am;
        vinsertv(sg->eg_am, eg_am, sgo);

        uint16_t eg_tremolopos = chip->eg_tremolopos;
        if (eg_tremolopos >= 105) {
            eg_tremolopos = (210 - eg_tremolopos);
        }
        vi16_t eg_tremolo = vset1((int16_t)(eg_tremolopos >> chip->eg_tremoloshift));
        vsfence();
        sg->eg_tremolo_am = vand(eg_tremolo, sg->eg_am);
    }

    if (update_deltafreq) {
        for (sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
            cgi = aymo_(sgi_to_cgi)(sgi);
            cg = &chip->cg[cgi];
            sg = &chip->sg[sgi];
            aymo_(pg_update_deltafreq)(chip, cg, sg);
        }
    }
}


static
void aymo_(write_40h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_ymf262_slot_to_word[slot];
    struct aymo_ymf262_reg_40h* reg_40h = &(chip->slot_regs[slot].reg_40h);
    struct aymo_ymf262_reg_40h reg_40h_prev = *reg_40h;
    *(uint8_t*)(void*)reg_40h = value;

    if ((reg_40h->tl != reg_40h_prev.tl) || (reg_40h->ksl != reg_40h_prev.ksl)) {
        aymo_(eg_update_ksl)(chip, word);
    }
}


static
void aymo_(write_60h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_ymf262_slot_to_word[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_ymf262_reg_60h* reg_60h = &(chip->slot_regs[slot].reg_60h);
    struct aymo_ymf262_reg_60h reg_60h_prev = *reg_60h;
    *(uint8_t*)(void*)reg_60h = value;

    if ((reg_60h->dr != reg_60h_prev.dr) || (reg_60h->ar != reg_60h_prev.ar)) {
        int16_t eg_adsr_word = vextractv(sg->eg_adsr, sgo);
        struct aymo_(eg_adsr)* eg_adsr = (struct aymo_(eg_adsr)*)(void*)&eg_adsr_word;
        eg_adsr->dr = reg_60h->dr;
        eg_adsr->ar = reg_60h->ar;
        vinsertv(sg->eg_adsr, eg_adsr_word, sgo);
    }
}


static
void aymo_(write_80h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_ymf262_slot_to_word[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_ymf262_reg_80h* reg_80h = &(chip->slot_regs[slot].reg_80h);
    struct aymo_ymf262_reg_80h reg_80h_prev = *reg_80h;
    *(uint8_t*)(void*)reg_80h = value;

    if ((reg_80h->rr != reg_80h_prev.rr) || (reg_80h->sl != reg_80h_prev.sl)) {
        int16_t eg_adsr_word = vextractv(sg->eg_adsr, sgo);
        struct aymo_(eg_adsr)* eg_adsr = (struct aymo_(eg_adsr)*)(void*)&eg_adsr_word;
        eg_adsr->sr = (chip->slot_regs[slot].reg_20h.egt ? 0 : reg_80h->rr);
        eg_adsr->rr = reg_80h->rr;
        vinsertv(sg->eg_adsr, eg_adsr_word, sgo);
        int16_t eg_sl = (int16_t)reg_80h->sl;
        if (eg_sl == 0x0F) {
            eg_sl = 0x1F;
        }
        vinsertv(sg->eg_sl, eg_sl, sgo);
    }
}


static
void aymo_(write_E0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int slot = aymo_(addr_to_slot)(address);
    int word = aymo_ymf262_slot_to_word[slot];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
    struct aymo_ymf262_reg_E0h* reg_E0h = &(chip->slot_regs[slot].reg_E0h);
    struct aymo_ymf262_reg_E0h reg_E0h_prev = *reg_E0h;
    *(uint8_t*)(void*)reg_E0h = value;

    if (!chip->chip_regs.reg_105h.newm) {
        reg_E0h->ws &= 3;
    }

    if (reg_E0h->ws != reg_E0h_prev.ws) {
        const struct aymo_(wave)* wave = &aymo_(wave_table)[reg_E0h->ws];
        vinsertv(sg->wg_phase_mullo, wave->wg_phase_mullo, sgo);
        vinsertv(sg->wg_phase_zero,  wave->wg_phase_zero,  sgo);
        vinsertv(sg->wg_phase_neg,   wave->wg_phase_neg,   sgo);
        vinsertv(sg->wg_phase_flip,  wave->wg_phase_flip,  sgo);
        vinsertv(sg->wg_phase_mask,  wave->wg_phase_mask,  sgo);
        vinsertv(sg->wg_sine_gate,   wave->wg_sine_gate,   sgo);
    }
}


static
void aymo_(write_A0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
    int ch2p = aymo_ymf262_ch2x_paired[ch2x];
    int ch2x_is_secondary = (ch2p < ch2x);
    if (chip->chip_regs.reg_105h.newm && ch2x_is_pairing && ch2x_is_secondary) {
        return;
    }
    if (!ch2x_is_pairing || ch2x_is_secondary) {
        ch2p = -1;
    }

    struct aymo_ymf262_reg_A0h* reg_A0h = &(chip->ch2x_regs[ch2x].reg_A0h);
    struct aymo_ymf262_reg_A0h reg_A0h_prev = *reg_A0h;
    *(uint8_t*)(void*)reg_A0h = value;

    if (reg_A0h->fnum_lo != reg_A0h_prev.fnum_lo) {
        aymo_(ch2x_update_fnum)(chip, ch2x, ch2p);
    }
}


static
void aymo_(write_B0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    unsigned ch2x_is_pairing = (chip->og_ch2x_pairing & (1UL << ch2x));
    int ch2p = aymo_ymf262_ch2x_paired[ch2x];
    int ch2x_is_secondary = (ch2p < ch2x);
    if (chip->chip_regs.reg_105h.newm && ch2x_is_pairing && ch2x_is_secondary) {
        return;
    }
    if (!ch2x_is_pairing || ch2x_is_secondary) {
        ch2p = -1;
    }

    if (address == 0xBD) {
        struct aymo_ymf262_reg_BDh* reg_BDh = &chip->chip_regs.reg_BDh;
        struct aymo_ymf262_reg_BDh reg_BDh_prev = *reg_BDh;
        *(uint8_t*)(void*)reg_BDh = value;

        chip->eg_tremoloshift = (((reg_BDh->dam ^ 1) << 1) + 2);
        chip->eg_vibshift = (reg_BDh->dvb ^ 1);
        aymo_(cm_rewire_rhythm)(chip, &reg_BDh_prev);
    }
    else {
        struct aymo_ymf262_reg_B0h* reg_B0h = &(chip->ch2x_regs[ch2x].reg_B0h);
        struct aymo_ymf262_reg_B0h reg_B0h_prev = *reg_B0h;
        *(uint8_t*)(void*)reg_B0h = value;

        if ((reg_B0h->fnum_hi != reg_B0h_prev.fnum_hi) || (reg_B0h->block != reg_B0h_prev.block)) {
            aymo_(ch2x_update_fnum)(chip, ch2x, ch2p);
        }

        if (reg_B0h->kon != reg_B0h_prev.kon) {
            if (reg_B0h->kon) {
                aymo_(ch2x_key_on)(chip, ch2x);
            } else {
                aymo_(ch2x_key_off)(chip, ch2x);
            }
        }
    }
}


static
void aymo_(write_C0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    struct aymo_ymf262_reg_C0h* reg_C0h = &(chip->ch2x_regs[ch2x].reg_C0h);
    struct aymo_ymf262_reg_C0h reg_C0h_prev = *reg_C0h;
    if (!chip->chip_regs.reg_105h.newm) {
        value = ((value | 0x30) & 0x3F);
    }
    *(uint8_t*)(void*)reg_C0h = value;

    int ch2x_word0 = aymo_ymf262_ch2x_to_word[ch2x][0];
    int ch2x_word1 = aymo_ymf262_ch2x_to_word[ch2x][1];
    int sgo = (ch2x_word0 % AYMO_(SLOT_GROUP_LENGTH));
    int sgi0 = (ch2x_word0 / AYMO_(SLOT_GROUP_LENGTH));
    int sgi1 = (ch2x_word1 / AYMO_(SLOT_GROUP_LENGTH));
    struct aymo_(slot_group)* sg0 = &chip->sg[sgi0];
    struct aymo_(slot_group)* sg1 = &chip->sg[sgi1];
    int cgi = aymo_(sgi_to_cgi)(sgi0);
    struct aymo_(ch2x_group)* cg = &chip->cg[cgi];

    if (reg_C0h->cha != reg_C0h_prev.cha) {
        int16_t og_ch_gate_a = -(int16_t)reg_C0h->cha;
        vinsertv(cg->og_ch_gate_a, og_ch_gate_a, sgo);
        vinsertv(sg0->og_out_ch_gate_a, (vextractv(sg0->og_out_gate, sgo) & og_ch_gate_a), sgo);
        vinsertv(sg1->og_out_ch_gate_a, (vextractv(sg1->og_out_gate, sgo) & og_ch_gate_a), sgo);
    }
    if (reg_C0h->chb != reg_C0h_prev.chb) {
        int16_t og_ch_gate_b = -(int16_t)reg_C0h->chb;
        vinsertv(cg->og_ch_gate_b, og_ch_gate_b, sgo);
        vinsertv(sg0->og_out_ch_gate_b, (vextractv(sg0->og_out_gate, sgo) & og_ch_gate_b), sgo);
        vinsertv(sg1->og_out_ch_gate_b, (vextractv(sg1->og_out_gate, sgo) & og_ch_gate_b), sgo);
    }
    if (reg_C0h->chc != reg_C0h_prev.chc) {
        int16_t og_ch_gate_c = -(int16_t)reg_C0h->chc;
        vinsertv(cg->og_ch_gate_c, og_ch_gate_c, sgo);
        vinsertv(sg0->og_out_ch_gate_c, (vextractv(sg0->og_out_gate, sgo) & og_ch_gate_c), sgo);
        vinsertv(sg1->og_out_ch_gate_c, (vextractv(sg1->og_out_gate, sgo) & og_ch_gate_c), sgo);
    }
    if (reg_C0h->chd != reg_C0h_prev.chd) {
        int16_t og_ch_gate_d = -(int16_t)reg_C0h->chd;
        vinsertv(cg->og_ch_gate_d, og_ch_gate_d, sgo);
        vinsertv(sg0->og_out_ch_gate_d, (vextractv(sg0->og_out_gate, sgo) & og_ch_gate_d), sgo);
        vinsertv(sg1->og_out_ch_gate_d, (vextractv(sg1->og_out_gate, sgo) & og_ch_gate_d), sgo);
    }

    if (reg_C0h->fb != reg_C0h_prev.fb) {
        int16_t fb_mulhi = (reg_C0h->fb ? (0x0040 << reg_C0h->fb) : 0);
        vinsertv(sg0->wg_fb_mulhi, fb_mulhi, sgo);
        vinsertv(sg1->wg_fb_mulhi, fb_mulhi, sgo);
    }

    if (chip->chip_regs.reg_105h.stereo) {
        // TODO:
    }

    if (reg_C0h->cnt != reg_C0h_prev.cnt) {
        aymo_(cm_rewire_ch2x)(chip, ch2x);
    }
}


static
void aymo_(write_D0h)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    int ch2x = aymo_(addr_to_ch2x)(address);
    *(uint8_t*)(void*)&(chip->ch2x_regs[ch2x].reg_C0h) = value;

    if (chip->chip_regs.reg_105h.stereo) {
        // TODO:
    }
}


static
int aymo_(rq_enqueue)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    uint16_t rq_tail = chip->rq_tail;
    uint16_t rq_next = (rq_tail + 1);
    if (rq_next >= AYMO_(REG_QUEUE_LENGTH)) {
        rq_next = 0u;
    }

    if (rq_next != chip->rq_head) {
        chip->rq_buffer[rq_tail].address = address;
        chip->rq_buffer[rq_tail].value = value;
        chip->rq_tail = rq_next;
        return 1;
    }
    return 0;
}


const struct aymo_ymf262_vt* aymo_(get_vt)(void)
{
    return &(aymo_(vt));
}


uint32_t aymo_(get_sizeof)(void)
{
    return sizeof(struct aymo_(chip));
}


void aymo_(ctor)(struct aymo_(chip)* chip)
{
    assert(chip);

    // Wipe everything, except VT
    const struct aymo_ymf262_vt* vt = chip->parent.vt;
    aymo_memset(chip, 0, sizeof(*chip));
    chip->parent.vt = vt;

    // Initialize slots
    for (int sgi = 0; sgi < AYMO_(SLOT_GROUP_NUM); ++sgi) {
        struct aymo_(slot_group)* sg = &(chip->sg[sgi]);
        sg->eg_rout         = vset1(0x01FF);
        sg->eg_out          = vset1(0x01FF);
        sg->eg_gen          = vset1(AYMO_(EG_GEN_RELEASE));
        sg->eg_gen_mullo    = vset1(AYMO_(EG_GEN_MULLO_RELEASE));
        sg->pg_mult_x2      = vset1(aymo_ymf262_pg_mult_x2_table[0]);
        sg->og_prout_ac     = vsetm(aymo_(og_prout_ac)[sgi]);
        sg->og_prout_bd     = vsetm(aymo_(og_prout_bd)[sgi]);

        const struct aymo_(wave)* wave = &aymo_(wave_table)[0];
        sg->wg_phase_mullo  = vset1(wave->wg_phase_mullo);
        sg->wg_phase_zero   = vset1(wave->wg_phase_zero);
        sg->wg_phase_neg    = vset1(wave->wg_phase_neg);
        sg->wg_phase_flip   = vset1(wave->wg_phase_flip);
        sg->wg_phase_mask   = vset1(wave->wg_phase_mask);
        sg->wg_sine_gate    = vset1(wave->wg_sine_gate);
    }

    // Initialize channels
    for (int cgi = 0; cgi < (AYMO_(SLOT_GROUP_NUM) / 2); ++cgi) {
        struct aymo_(ch2x_group)* cg = &(chip->cg[cgi]);
        cg->og_ch_gate_a = vset1(-1);
        cg->og_ch_gate_b = vset1(-1);
    }
    for (int ch2x = 0; ch2x < AYMO_(CHANNEL_NUM_MAX); ++ch2x) {
        aymo_(cm_rewire_ch2x)(chip, ch2x);
    }

    // Initialize chip
    chip->ng_noise = 1;

    chip->eg_tremoloshift = 4;
    chip->eg_vibshift = 1;
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
    assert(chip);

    if (address > 0x1FF) {
        return;
    }

    switch (address & 0xF0) {
    case 0x00: {
        aymo_(write_00h)(chip, address, value);
        break;
    }
    case 0x20:
    case 0x30: {
        aymo_(write_20h)(chip, address, value);
        break;
    }
    case 0x40:
    case 0x50: {
        aymo_(write_40h)(chip, address, value);
        break;
    }
    case 0x60:
    case 0x70: {
        aymo_(write_60h)(chip, address, value);
        break;
    }
    case 0x80:
    case 0x90: {
        aymo_(write_80h)(chip, address, value);
        break;
    }
    case 0xE0:
    case 0xF0: {
        aymo_(write_E0h)(chip, address, value);
        break;
    }
    case 0xA0: {
        aymo_(write_A0h)(chip, address, value);
        break;
    }
    case 0xB0: {
        aymo_(write_B0h)(chip, address, value);
        break;
    }
    case 0xC0: {
        aymo_(write_C0h)(chip, address, value);
        break;
    }
    case 0xD0: {
        aymo_(write_D0h)(chip, address, value);
        break;
    }
    }
    vsfence();
}


int aymo_(enqueue_write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    assert(chip);

    if (address < 0x8000u) {
        return aymo_(rq_enqueue)(chip, address, value);
    }
    return 0;
}


int aymo_(enqueue_delay)(struct aymo_(chip)* chip, uint32_t count)
{
    assert(chip);

    if (count < 0x8000u) {
        uint16_t address = (uint16_t)((count >> 8) | 0x8000u);
        uint8_t value = (uint8_t)(count & 0xFFu);
        return aymo_(rq_enqueue)(chip, address, value);
    }
    return 0;
}


int16_t aymo_(get_output)(struct aymo_(chip)* chip, uint8_t channel)
{
    assert(chip);

    switch (channel) {
        case 0u: return _mm_extract_epi16(chip->og_out, 0);
        case 1u: return _mm_extract_epi16(chip->og_out, 1);
        case 2u: return _mm_extract_epi16(chip->og_out, 2);
        case 3u: return _mm_extract_epi16(chip->og_out, 3);
        default: return 0;
    }
}


void aymo_(tick)(struct aymo_(chip)* chip, uint32_t count)
{
    assert(chip);

    while (count--) {
        aymo_(tick_once)(chip);
    }
}


void aymo_(generate_i16x2)(struct aymo_(chip)* chip, uint32_t count, int16_t y[])
{
    assert(chip);
    assert(((uintptr_t)(void*)y & 3u) == 0u);

    while (count--) {
        aymo_(tick_once)(chip);

        *(int32_t*)y = _mm_cvtsi128_si32(chip->og_out);
        y += 2u;
    }
}


void aymo_(generate_i16x4)(struct aymo_(chip)* chip, uint32_t count, int16_t y[])
{
    assert(chip);
    assert(((uintptr_t)(void*)y & 7u) == 0u);

    while (count--) {
        aymo_(tick_once)(chip);

        _mm_storel_epi64((void*)y, chip->og_out);
        y += 4u;
    }
}


void aymo_(generate_f32x2)(struct aymo_(chip)* chip, uint32_t count, float y[])
{
    assert(chip);
    assert(((uintptr_t)(void*)y & 7u) == 0u);

    while (count--) {
        aymo_(tick_once)(chip);

        vi32x4_t vi32 = _mm_cvtepi16_epi32(chip->og_out);
        vf32x4_t vf32 = _mm_cvtepi32_ps(vi32);
        _mm_storel_pi((void*)y, vf32);
        y += 2u;
    }
}


void aymo_(generate_f32x4)(struct aymo_(chip)* chip, uint32_t count, float y[])
{
    assert(chip);
    assert(((uintptr_t)(void*)y & 15u) == 0u);

    while (count--) {
        aymo_(tick_once)(chip);

        vi32x4_t vi32 = _mm_cvtepi16_epi32(chip->og_out);
        vf32x4_t vf32 = _mm_cvtepi32_ps(vi32);
        _mm_store_ps(y, vf32);
        y += 4u;
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_AVX
