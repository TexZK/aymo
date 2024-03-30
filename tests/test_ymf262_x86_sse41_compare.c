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

#include "aymo.h"
#ifdef AYMO_CPU_SUPPORT_X86_SSE41

#include "aymo_cpu_x86_sse41_inline.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_ymf262_x86_sse41.h"

#include "test_ymf262_compare_prologue_inline.h"


static int compare_slots(int slot_)
{
    if (slot_ >= AYMO_YMF262_SLOT_NUM) {
        return 0;  // ignore
    }

    int word = aymo_ymf262_slot_to_word[slot_];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi);
    const struct aymo_(slot_group)* sg = &aymo_chip.sg[sgi];
    const struct aymo_(ch2x_group)* cg = &aymo_chip.cg[cgi];
    const opl3_slot* slot = &nuked_chip.slot[slot_];
    (void)cg;

    // TODO: Commented stuff
    assert((int16_t)vextractn(sg->wg_out, sgo) == slot->out);
    int16_t channel_fb = (int16_t)(slot->channel->fb ? (0x40 << slot->channel->fb) : 0);
    assert((int16_t)vextractn(sg->wg_fb_mulhi, sgo) == channel_fb);
#ifdef AYMO_DEBUG
    assert(vextractn(sg->wg_fbmod, sgo) == slot->fbmod);
    assert(vextractn(sg->wg_mod, sgo) == *slot->mod);
#endif
    assert((int16_t)vextractn(sg->wg_prout, sgo) == slot->prout);
    assert((uint16_t)vextractn(sg->eg_rout, sgo) == slot->eg_rout);
    assert((uint16_t)vextractn(sg->eg_out, sgo) == slot->eg_out);
#ifdef AYMO_DEBUG
    assert(vextractn(sg->eg_inc, sgo) == slot->eg_inc);
#endif
    assert((uint16_t)vextractn(sg->eg_gen, sgo) == slot->eg_gen);
#ifdef AYMO_DEBUG
    assert(vextractn(sg->eg_rate, sgo) == slot->eg_rate);
    assert(vextractn(sg->eg_ksl, sgo) == slot->eg_ksl);
    assert((uint16_t)vextractn(sg->eg_tl_x4, sgo) == (slot->reg_tl * 4u));
#endif
    assert((int16_t)vextractn(sg->eg_tremolo_am, sgo) == *slot->trem);
    assert((uint16_t)-vextractn(sg->pg_vib, sgo) == slot->reg_vib);
    //assert(vextractn(sg->eg_egt, sgo) == slot->reg_type);
    //assert(vextractn(sg->eg_ksr, sgo) == slot->reg_ksr);
    assert((uint16_t)vextractn(sg->pg_mult_x2, sgo) == mt[slot->reg_mult]);
    assert((((uint16_t)vextractn(sg->eg_adsr, sgo) >> 12) & 15) == slot->reg_ar);
    assert((((uint16_t)vextractn(sg->eg_adsr, sgo) >>  8) & 15) == slot->reg_dr);
    assert((uint16_t)vextractn(sg->eg_sl, sgo) == slot->reg_sl);
    assert((((uint16_t)vextractn(sg->eg_adsr, sgo) >>  0) & 15) == slot->reg_rr);
    //assert(vextractn(sg->wg_wf, sgo) == slot->reg_wf);
    uint16_t eg_key = (uint16_t)vextractn(sg->eg_key, sgo);
    eg_key = ((eg_key >> 7) | (eg_key & 1));
    assert(eg_key == slot->key);
    vi32_t pg_phase_vv = (aymo_(sgo_side)[sgo] ? sg->pg_phase_hi : sg->pg_phase_lo);
    uint32_t pg_phase = vvextractn(pg_phase_vv, aymo_(sgo_cell)[sgo]);
    assert(pg_phase == slot->pg_phase);
    assert((uint16_t)vextractn(sg->pg_phase_out, sgo) == slot->pg_phase_out);

    return 0;
catch_:
    return 1;
}


static int compare_ch2xs(int ch2x)
{
    if (ch2x >= AYMO_YMF262_CHANNEL_NUM) {
        return 0;  // ignore
    }

    int word = aymo_ymf262_ch2x_to_word[ch2x][0];
    int sgi = (word / AYMO_(SLOT_GROUP_LENGTH));
    int sgo = (word % AYMO_(SLOT_GROUP_LENGTH));
    int cgi = aymo_(sgi_to_cgi)(sgi);
    const struct aymo_(ch2x_group)* cg = &aymo_chip.cg[cgi];
    const opl3_channel* channel = &nuked_chip.channel[ch2x];

    // TODO: Commented stuff
    //int16_t* out[0];
    //int16_t* out[1];
    //int16_t* out[2];
    //int16_t* out[3];
    //int32_t leftpan;
    //int32_t rightpan;
    //uint8_t chtype;
    assert((uint16_t)vextractn(cg->pg_fnum, sgo) == channel->f_num);
    assert((uint16_t)vextractn(cg->pg_block, sgo) == channel->block);
    //uint8_t fb;  // compared at slot group level
    //uint8_t con;
    //uint8_t alg;
    assert((uint16_t)vextractn(cg->eg_ksv, sgo) == channel->ksv);
    assert((uint16_t)vextractn(cg->og_ch_gate_a, sgo) == channel->cha);
    assert((uint16_t)vextractn(cg->og_ch_gate_b, sgo) == channel->chb);
    assert((uint16_t)vextractn(cg->og_ch_gate_c, sgo) == channel->chc);
    assert((uint16_t)vextractn(cg->og_ch_gate_d, sgo) == channel->chd);

    return 0;
catch_:
    return 1;
}


static int compare_chips(void)
{
    vsfence();

    for (int slot = 0; slot < AYMO_YMF262_SLOT_NUM; ++slot) {
        if (compare_slots(slot)) {
            assert(0);
        }
    }

    for (int ch2x = 0; ch2x < AYMO_YMF262_CHANNEL_NUM; ++ch2x) {
        if (compare_ch2xs(ch2x)) {
            assert(0);
        }
    }

    // TODO: Commented stuff
    assert((uint16_t)aymo_chip.tm_timer == (uint16_t)nuked_chip.timer);
    assert(aymo_chip.eg_timer == nuked_chip.eg_timer);
    assert(aymo_chip.eg_timerrem == nuked_chip.eg_timerrem);
    assert(aymo_chip.eg_state == nuked_chip.eg_state);
    assert((uint16_t)vextractn(aymo_chip.eg_add, 0) == nuked_chip.eg_add);
    //uint8_t newm;
    //uint8_t nts;
    //uint8_t rhy;
    assert(aymo_chip.pg_vibpos == nuked_chip.vibpos);
    assert(aymo_chip.eg_vibshift == nuked_chip.vibshift);
    //assert((uint16_t)vextractn(aymo_chip.eg_tremolo, 0) == nuked_chip.tremolo);
    assert(aymo_chip.eg_tremolopos == nuked_chip.tremolopos);
    assert(aymo_chip.eg_tremoloshift == nuked_chip.tremoloshift);
    assert(aymo_chip.ng_noise == nuked_chip.noise);
    assert((int16_t)vextract(aymo_chip.og_out, 0) == nuked_out[0]);
    assert((int16_t)vextract(aymo_chip.og_out, 1) == nuked_out[1]);
    assert((int16_t)vextract(aymo_chip.og_out, 2) == nuked_out[2]);
    assert((int16_t)vextract(aymo_chip.og_out, 3) == nuked_out[3]);
    assert(aymo_chip.rm_hh_bit2 == nuked_chip.rm_hh_bit2);
    assert(aymo_chip.rm_hh_bit3 == nuked_chip.rm_hh_bit3);
    assert(aymo_chip.rm_hh_bit7 == nuked_chip.rm_hh_bit7);
    assert(aymo_chip.rm_hh_bit8 == nuked_chip.rm_hh_bit8);
    assert(aymo_chip.rm_tc_bit3 == nuked_chip.rm_tc_bit3);
    assert(aymo_chip.rm_tc_bit5 == nuked_chip.rm_tc_bit5);

    return 0;
catch_:
    return 1;
}


#include "test_ymf262_compare_epilogue_inline.h"


#endif  // AYMO_CPU_SUPPORT_X86_SSE41
