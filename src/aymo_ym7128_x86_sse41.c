/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023-2024 Andrea Zoppi.

This file is part of AYMO.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published yb the Free
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
#ifdef AYMO_CPU_SUPPORT_X86_SSE41

#include "aymo_cpu_x86_sse41_inline.h"
#include "aymo_ym7128.h"
#define AYMO_KEEP_SHORTHANDS
#include "aymo_ym7128_x86_sse41.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN


const struct aymo_ym7128_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_ym7128_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_ym7128_ctor_f)&(aymo_(ctor)),
    (aymo_ym7128_dtor_f)&(aymo_(dtor)),
    (aymo_ym7128_read_f)&(aymo_(read)),
    (aymo_ym7128_write_f)&(aymo_(write)),
    (aymo_ym7128_process_i16_f)&(aymo_(process_i16))
};


const struct aymo_ym7128_vt* aymo_(get_vt)(void)
{
    return &aymo_(vt);
}


uint32_t aymo_(get_sizeof)(void)
{
    return sizeof(struct aymo_(chip));
}


void aymo_(ctor)(struct aymo_(chip)* chip)
{
    assert(chip);

    // Wipe everything, except VT
    aymo_memset((&chip->parent.vt + 1u), 0, (sizeof(*chip) - sizeof(chip->parent.vt)));

    // Initialize input stage coefficients (-1 as a placeholder for computed values)
    chip->xxv[2] = 1;
    chip->xxv[3] = 1;

    chip->kk1 = vseta(0, -1, -1, -1, -0x8000, -0x8000, -0x8000, -0x8000);
    chip->kk2 = vseta(0, -1, -0x8000, 0, 0, 0, 0x8000, -0x8000);
    chip->kkm = vseta(0, 0x7FFF, 0x7FFF, 0, 0, 0, AYMO_YM7128_DELAY_LENGTH, AYMO_YM7128_DELAY_LENGTH);

    // Initialize oversampler coefficients
    const int16_t* k = aymo_ym7128_kernel_linear;
    chip->ka = vseta(k[ 6], k[ 6], k[ 4], k[ 4], k[ 2], k[ 2], k[ 0], k[ 0]);
    chip->kb = vseta(k[ 7], k[ 7], k[ 5], k[ 5], k[ 3], k[ 3], k[ 1], k[ 1]);
    chip->kc = vseta(k[14], k[14], k[12], k[12], k[10], k[10], k[ 8], k[ 8]);
    chip->kd = vseta(k[15], k[15], k[13], k[13], k[11], k[11], k[ 9], k[ 9]);
    chip->ke = vseta(    0,     0,     0,     0, k[18], k[18], k[16], k[16]);
    chip->kf = vseta(    0,     0,     0,     0,     0,     0, k[17], k[17]);

    // Initialize as pass-through
    aymo_(write)(chip, (uint16_t)aymo_ym7128_reg_gl1, 0x3Fu);
    aymo_(write)(chip, (uint16_t)aymo_ym7128_reg_gr1, 0x3Fu);
    aymo_(write)(chip, (uint16_t)aymo_ym7128_reg_vm, 0x3Fu);
    aymo_(write)(chip, (uint16_t)aymo_ym7128_reg_vl, 0x3Fu);
    aymo_(write)(chip, (uint16_t)aymo_ym7128_reg_vr, 0x3Fu);
}


void aymo_(dtor)(struct aymo_(chip)* chip)
{
    AYMO_UNUSED_VAR(chip);
    assert(chip);
}


uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address)
{
    assert(chip);

    if (address < (uint16_t)AYMO_YM7128_REG_COUNT) {
        return chip->regs[address];
    }
    return 0x00u;
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    assert(chip);

    if (address <= (uint16_t)aymo_ym7128_reg_gl8) {
        value &= 0x3Fu;
        int16_t gl = aymo_ym7128_gain[value];
        int i = (int)(address - (uint16_t)aymo_ym7128_reg_gl1);
        chip->kgl = vinsertn(chip->kgl, gl, i);
    }
    else if (address <= (uint16_t)aymo_ym7128_reg_gr8) {
        value &= 0x3Fu;
        int16_t gr = aymo_ym7128_gain[value];
        int i = (int)(address - (uint16_t)aymo_ym7128_reg_gr1);
        chip->kgr = vinsertn(chip->kgr, gr, i);
    }
    else if (address <= (uint16_t)aymo_ym7128_reg_vr) {
        value &= 0x3Fu;
        int16_t v = aymo_ym7128_gain[value];
        if (address == (uint16_t)aymo_ym7128_reg_vm) {
            chip->kk1 = vinsert(chip->kk1, -v, 5);
        }
        else if (address == (uint16_t)aymo_ym7128_reg_vc) {
            chip->kk2 = vinsert(chip->kk2, v, 6);
        }
        else if (address == (uint16_t)aymo_ym7128_reg_vl) {
            chip->kv = vinsert(chip->kv, v, 6);
        }
        else {
            chip->kv = vinsert(chip->kv, v, 7);
        }
    }
    else if (address <= (uint16_t)aymo_ym7128_reg_c1) {
        value &= 0x3Fu;
        int16_t v = ((int16_t)value << (16 - AYMO_YM7128_COEFF_BITS));
        if (address == (uint16_t)aymo_ym7128_reg_c0) {
            chip->kk1 = vinsert(chip->kk1, v, 4);
        }
        else {
            chip->kk1 = vinsert(chip->kk1, v, 6);
        }
    }
    else if (address <= (uint16_t)aymo_ym7128_reg_t8) {
        value &= 0x1Fu;
        int16_t t = aymo_ym7128_tap[value];
        int16_t hi = chip->xxv[1];  // hi
        t = (hi - t);
        if (t < 0) {
            t += AYMO_YM7128_DELAY_LENGTH;
        }
        if (address == (uint16_t)aymo_ym7128_reg_t0) {
            chip->xxv[0] = t;  // ti0
        }
        else {
            uint16_t i = (address - (uint16_t)aymo_ym7128_reg_t1);
            chip->tiv[i] = t;
        }
    }

    if (address < (uint16_t)AYMO_YM7128_REG_COUNT) {
        chip->regs[address] = value;
    }
}


void aymo_(process_i16)(struct aymo_(chip)* chip, uint32_t count, const int16_t x[], int16_t y[])
{
    assert(chip);
    assert(x);
    assert(y);
    if AYMO_UNLIKELY(!count) return;

    int16_t AYMO_ALIGN_V128 vv[8] = {0};

    int16_t ti0 = chip->xxv[0];
    int16_t t0 = chip->uh[ti0];
    chip->xxv[4] = t0;

    const int16_t* xe = &x[count];

    while AYMO_LIKELY(x != xe) {
        chip->xxv[5] = (*x++ & AYMO_YM7128_SIGNAL_MASK);

        vsfence();
        vi16x8_t xx = vload((void*)chip->xxv);
        chip->xxv[6] = t0;  // t0d = t0
        xx = vmulhrs(xx, chip->kk1);
        xx = vaddsi(xx, vvshuffle(xx, KSHUFFLE(2, 3, 0, 1)));  // "2301"
        xx = vmulhrs(xx, chip->kk2);
        xx = vand(xx, vcmpgt(chip->kkm, xx));
        xx = vaddsi(xx, valignr(xx, xx, 2));
        vstore((void*)vv, xx);
        vi16x8_t ti = vload((void*)chip->tiv);
        vi16x8_t tj = vsub(ti, vset1(-1));
        vi16x8_t tm = vcmpgt(vset1(AYMO_YM7128_DELAY_LENGTH - 1), ti);  // tj < DL
        vstore((void*)chip->tiv, vand(tj, tm));
        vsfence();

        chip->xxv[0] = vv[7];  // ti0'
        int16_t hj = vv[1];
        chip->xxv[1] = hj;  // hi'
        int16_t u = vv[5];
        chip->uh[hj] = u;
        int16_t AYMO_ALIGN_V128 tuv[8];
        for (unsigned i = 0u; i < 8u; ++i) {
            tuv[i] = chip->uh[chip->tiv[i]];
        }
        vsfence();
        vi16x8_t tu = vload((void*)tuv);

        vi16x8_t gl = vmulhrs(tu, chip->kgl);
        vi16x8_t gr = vmulhrs(tu, chip->kgr);
        vi32x4_t ggl = vmadd(gl, vset1(1));
        vi32x4_t ggr = vmadd(gr, vset1(1));
        ggl = vvadd(ggl, vvshuffle(ggl, KSHUFFLE(1, 0, 3, 2)));  // "1032"
        ggr = vvadd(ggr, vvshuffle(ggr, KSHUFFLE(1, 0, 3, 2)));  // "1032"
        ggl = vvadd(ggl, vvshuffle(ggl, KSHUFFLE(2, 3, 0, 1)));  // "2301"
        ggr = vvadd(ggr, vvshuffle(ggr, KSHUFFLE(2, 3, 0, 1)));  // "2301"
        vi16x8_t ggrl = vvpacks(ggr, ggl);
        vi16x8_t gglr = valignr(ggrl, ggrl, 2);
        vi16x8_t vlr = vmulhrs(gglr, chip->kv);

        vi16x8_t zc = chip->zc;
        vi16x8_t zb = chip->zb;
        zc = valignr(zc, zb, 12);  // '543210..'
        chip->zc = zc;

        vi16x8_t y1 = vmulhrs(zc, chip->kf);
        vi16x8_t y0 = vmulhrs(zc, chip->ke);

        vi16x8_t za = chip->za;
        zb = valignr(zb, za, 12);  // '543210..'
        chip->zb = zb;

        y1 = vaddsi(y1, vmulhrs(zb, chip->kd));
        y0 = vaddsi(y0, vmulhrs(zb, chip->kc));

        za = valignr(za, vlr, 12);  // '543210..'
        chip->za = za;

        y1 = vaddsi(y1, vmulhrs(za, chip->kb));
        y0 = vaddsi(y0, vmulhrs(za, chip->ka));

        y0 = vaddsi(y0, vvshuffle(y0, KSHUFFLE(1, 0, 3, 2)));  // "1032"
        y1 = vaddsi(y1, vvshuffle(y1, KSHUFFLE(1, 0, 3, 2)));  // "1032"
        y0 = vaddsi(y0, vvshuffle(y0, KSHUFFLE(2, 3, 0, 1)));  // "2301"
        y1 = vaddsi(y1, vvshuffle(y1, KSHUFFLE(2, 3, 0, 1)));  // "2301"

        vi16x8_t yy = vblendi(y0, y1, 0xCC);        // '1100''1100'
        yy = vand(yy, vset1((int16_t)AYMO_YM7128_SIGNAL_MASK));
        vstorelo((void*)y, yy); y += 4u;

        ti0 = chip->xxv[0];
        t0 = chip->uh[ti0];
        chip->xxv[4] = t0;
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_X86_SSE41
