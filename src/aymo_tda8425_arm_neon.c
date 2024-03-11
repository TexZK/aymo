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

#include "aymo_cpu.h"
#ifdef AYMO_CPU_SUPPORT_ARM_NEON

#define AYMO_KEEP_SHORTHANDS
#include "aymo_tda8425_arm_neon.h"
#include "aymo_tda8425.h"

#include <assert.h>

AYMO_CXX_EXTERN_C_BEGIN

#undef cos
#undef fabs
#undef log10
#undef pow
#undef sqrt
#undef tan

#define cos     (aymo_tda8425_math->cos)
#define fabs    (aymo_tda8425_math->fabs)
#define log10   (aymo_tda8425_math->log10)
#define pow     (aymo_tda8425_math->pow)
#define sqrt    (aymo_tda8425_math->sqrt)
#define tan     (aymo_tda8425_math->tan)


const struct aymo_tda8425_vt aymo_(vt) =
{
    AYMO_STRINGIFY2(aymo_(vt)),
    (aymo_tda8425_get_sizeof_f)&(aymo_(get_sizeof)),
    (aymo_tda8425_ctor_f)&(aymo_(ctor)),
    (aymo_tda8425_dtor_f)&(aymo_(dtor)),
    (aymo_tda8425_read_f)&(aymo_(read)),
    (aymo_tda8425_write_f)&(aymo_(write)),
    (aymo_tda8425_process_f32_f)&(aymo_(process_f32))
};


const struct aymo_tda8425_vt* aymo_(get_vt)(void)
{
    return &aymo_(vt);
}


uint32_t aymo_(get_sizeof)(void)
{
    return sizeof(struct aymo_(chip));
}


void aymo_(ctor)(struct aymo_(chip)* chip, float sample_rate)
{
    assert(chip);
    assert(sample_rate > 0.f);

    // Wipe everything
    aymo_memset(chip, 0, sizeof(struct aymo_(chip)));

    // Setup default parameters
    chip->sample_rate = sample_rate;
    chip->pseudo_c1 = aymo_tda8425_pseudo_preset_c1[0];
    chip->pseudo_c2 = aymo_tda8425_pseudo_preset_c2[0];

    // Setup default registers
    aymo_(write)(chip, 0x00u, 0xFCu);  // VL: 0 dB
    aymo_(write)(chip, 0x01u, 0xFCu);  // VR: 0 dB
    aymo_(write)(chip, 0x02u, 0xF6u);  // BA: 0 dB
    aymo_(write)(chip, 0x03u, 0xF6u);  // TR: 0 dB
    aymo_(write)(chip, 0x07u, 0xFCu);  // PP: light pseudo
    aymo_(write)(chip, 0x08u, 0xCEu);  // SF: linear stereo, channel 1, unmuted
}


void aymo_(dtor)(struct aymo_(chip)* chip)
{
    AYMO_UNUSED_VAR(chip);
    assert(chip);
}


static void aymo_(apply_vl)(struct aymo_(chip)* chip)
{
    double db = (double)aymo_tda8425_reg_v_to_db[chip->reg_vl & 0x3Fu];

    if (chip->reg_sf & 0x20u) {  // mute
        db = -90.;
    }

    double g = pow(10., (db * .05));
    chip->kv = vset_lane_f32((float)g, chip->kv, 0);
}


static void aymo_(apply_vr)(struct aymo_(chip)* chip)
{
    double db = (double)aymo_tda8425_reg_v_to_db[chip->reg_vr & 0x3Fu];

    if (chip->reg_sf & 0x20u) {  // mute
        db = -90.;
    }

    double g = pow(10., (db * .05));
    chip->kv = vset_lane_f32((float)g, chip->kv, 1);
}


static void aymo_(apply_ba)(struct aymo_(chip)* chip)
{
    double dbb = (double)aymo_tda8425_reg_ba_to_db[chip->reg_ba & 0x0Fu];
    double gb = pow(10., (dbb * (.05 * .5)));
    double fs = (double)chip->sample_rate;
    double pi = 3.14159265358979323846264338327950288;
    double fcb = 300.;  // [Hz]
    double wb = ((2. * pi) * fcb);
    double kb = (tan(wb * (.5 / fs)) / wb);

    double a0 = ((kb * wb) + gb);
    double a1 = ((kb * wb) - gb);
    double a2 = 0.;

    double b0 = (((kb * wb) * (gb * gb)) + gb);
    double b1 = (((kb * wb) * (gb * gb)) - gb);
    double b2 = 0.;

    double ra0 = (1. / a0);
    chip->kb0 = vsetq_lane_f32((float)(b0 * ra0), chip->kb0, 2);
    chip->kb1 = vsetq_lane_f32((float)(b1 * ra0), chip->kb1, 2);
    chip->kb2 = vsetq_lane_f32((float)(b2 * ra0), chip->kb2, 2);
    ra0 = -ra0;
    chip->ka1 = vsetq_lane_f32((float)(a1 * ra0), chip->ka1, 2);
    chip->ka2 = vsetq_lane_f32((float)(a2 * ra0), chip->ka2, 2);
}


static void aymo_(apply_tr)(struct aymo_(chip)* chip)
{
    double db = (double)aymo_tda8425_reg_tr_to_db[chip->reg_tr & 0x0Fu];
    double gt = pow(10., (db * (.05 * .5)));
    double fs = (double)chip->sample_rate;
    double pi = 3.14159265358979323846264338327950288;
    double fcd = 10.;  // [Hz]
    double wd = ((2. * pi) * fcd);
    double kd = ((chip->reg_sf & 0x40u) ? 0. : (tan(wd * (.5 / fs)) / wd));
    double fct = 4500.;  // [Hz]
    double wt = ((2. * pi) * fct);
    double kt = (tan(wt * (.5 / fs)) / wt);

    double a0 = (((gt * kt * wt) * (kd * wd)) + ((gt * kt * wt) + (kd * wd)) + 1.);
    double a1 = (((gt * kt * wt) * (kd * wd) * 2.) - 2.);
    double a2 = (((gt * kt * wt) * (kd * wd)) - ((gt * kt * wt) + (kd * wd)) + 1.);

    double b0 = ((gt * gt) + (gt * kt * wt));
    double b1 = ((gt * gt) * -2.);
    double b2 = ((gt * gt) - (gt * kt * wt));

    double ra0 = (1. / a0);
    chip->kb0 = vsetq_lane_f32((float)(b0 * ra0), chip->kb0, 1);
    chip->kb1 = vsetq_lane_f32((float)(b1 * ra0), chip->kb1, 1);
    chip->kb2 = vsetq_lane_f32((float)(b2 * ra0), chip->kb2, 1);
    ra0 = -ra0;
    chip->ka1 = vsetq_lane_f32((float)(a1 * ra0), chip->ka1, 1);
    chip->ka2 = vsetq_lane_f32((float)(a2 * ra0), chip->ka2, 1);
}


static void aymo_(apply_source_mode)(struct aymo_(chip)* chip)
{
    // Default mute
    vf32x2_t klr = vdup_n_f32(0.f);
    vf32x2_t krl = vdup_n_f32(0.f);

    uint8_t source = (chip->reg_sf & 0x07u);
    uint8_t mode = ((chip->reg_sf >> 3u) & 0x03u);

    // Forced mono
    if (mode == 0x00u) {  // process
        switch (source) {
            // Channel 1
            case 0x02u:
            case 0x04u:
            case 0x06u: {
                klr = vdup_n_f32(1.f);
                krl = vdup_n_f32(1.f);
                break;
            }
        }
    }
    else {  // not forced mono
        switch (source) {
            // Channel 1
            case 0x02u: {  // mono left
                klr = vset_lane_f32(1.f, klr, 0);
                krl = vset_lane_f32(1.f, krl, 1);
                break;
            }
            case 0x04u: {  // mono right
                klr = vset_lane_f32(1.f, klr, 1);
                krl = vset_lane_f32(1.f, krl, 0);
                break;
            }
            case 0x06u: {  // stereo
                klr = vdup_n_f32(1.f);
                krl = vdup_n_f32(0.f);
                break;
            }
            default: {
                if (mode == 0x03u) {  // spatial stereo
                    mode = 0x02u;  // force linear stereo (mute)
                }
                break;
            }
        }

        // Spatial stereo
        if (mode == 0x03u) {  // process
            const float xt = .52f;  // cross-talk
            vf32x2_t kx = vdup_n_f32(xt);
            klr = vadd_f32(klr, kx);
            krl = vsub_f32(krl, kx);
        }
    }  // not forced mono

    chip->klr = klr;
    chip->krl = krl;
}


static void aymo_(apply_pseudo)(struct aymo_(chip)* chip)
{
    uint8_t mode = ((chip->reg_sf >> 3u) & 0x03u);

    // Pseudo stereo
    if (mode == 0x02u) {  // enabled
        double c1 = (double)chip->pseudo_c1;
        double c2 = (double)chip->pseudo_c2;
        double r1 = 15000.;  // [ohm]
        double r2 = 15000.;  // [ohm]
        double t1 = (c1 * r1);
        double t2 = (c2 * r2);

        double fs = (double)chip->sample_rate;
        double k = (.5 / fs);
        double kk = (k * k);
        double t1_t2 = (t1 * t2);
        double t1_t2_k = ((t1 + t2) * k);

        double a0 = (kk + t1_t2 + t1_t2_k);
        double a1 = ((kk - t1_t2) * 2.);
        double a2 = (kk + t1_t2 - t1_t2_k);

        double b0 = a2;
        double b1 = a1;
        double b2 = a0;

        double ra0 = (1. / a0);
        chip->kb0 = vsetq_lane_f32((float)(b0 * ra0), chip->kb0, 0);
        chip->kb1 = vsetq_lane_f32((float)(b1 * ra0), chip->kb1, 0);
        chip->kb2 = vsetq_lane_f32((float)(b2 * ra0), chip->kb2, 0);
        ra0 = -ra0;
        chip->ka1 = vsetq_lane_f32((float)(a1 * ra0), chip->ka1, 0);
        chip->ka2 = vsetq_lane_f32((float)(a2 * ra0), chip->ka2, 0);
    }
    else {  // pass-through
        chip->kb0 = vsetq_lane_f32(1.f, chip->kb0, 0);
        chip->kb1 = vsetq_lane_f32(.0f, chip->kb1, 0);
        chip->kb2 = vsetq_lane_f32(.0f, chip->kb2, 0);

        chip->ka1 = vsetq_lane_f32(.0f, chip->ka1, 0);
        chip->ka2 = vsetq_lane_f32(.0f, chip->ka2, 0);
    }
}


static void aymo_(apply_tfilter)(struct aymo_(chip)* chip)
{
    // T-filter
    if (chip->reg_sf & 0x80u) {  // pass-through
        chip->kb0 = vsetq_lane_f32(1.f, chip->kb0, 3);
        chip->kb1 = vsetq_lane_f32(.0f, chip->kb1, 3);
        chip->kb2 = vsetq_lane_f32(.0f, chip->kb2, 3);

        chip->ka1 = vsetq_lane_f32(.0f, chip->ka1, 3);
        chip->ka2 = vsetq_lane_f32(.0f, chip->ka2, 3);
    }
    else {  // enabled
        double db = (double)aymo_tda8425_reg_ba_to_db[chip->reg_ba & 0x0Fu];
        double g = pow(10., (db * (.05 * .5)));
        double fs = (double)chip->sample_rate;
        double pi = 3.14159265358979323846264338327950288;
        double fc = 180.;  // [Hz]
        double w = ((2. * pi) * fc);
        double k = (tan(w * (.5 / fs)) / w);

        double log10_g = log10(g);
        double ang = (log10_g * .85);
        double abs_sqrt_log10_g = sqrt(fabs(log10_g));
        double abs2_sqrt_log10_g = abs_sqrt_log10_g * abs_sqrt_log10_g;
        double kw = (k * w);
        double m_k2w2 = ((kw * kw) * -.05);
        double sqrt_5 = 2.23606797749978980505147774238139391;
        double ph = (pi * .75);
        double h_sqrt_5_kw_abs_sqrt_log10_g = ((sqrt_5 * .2) * kw * abs_sqrt_log10_g);
        double cosm = cos(ang - ph);
        double cosp = cos(ang + ph);

        double a0 = (((m_k2w2 - abs2_sqrt_log10_g) + (h_sqrt_5_kw_abs_sqrt_log10_g * cosm)));
        double a1 = (((m_k2w2 + abs2_sqrt_log10_g)) * 2.);
        double a2 = (((m_k2w2 - abs2_sqrt_log10_g) - (h_sqrt_5_kw_abs_sqrt_log10_g * cosm)));

        double b0 = (((m_k2w2 - abs2_sqrt_log10_g) + (h_sqrt_5_kw_abs_sqrt_log10_g * cosp)));
        double b1 = a1;
        double b2 = (((m_k2w2 - abs2_sqrt_log10_g) - (h_sqrt_5_kw_abs_sqrt_log10_g * cosp)));

        double ra0 = (1. / a0);
        chip->kb0 = vsetq_lane_f32((float)(b0 * ra0), chip->kb0, 3);
        chip->kb1 = vsetq_lane_f32((float)(b1 * ra0), chip->kb1, 3);
        chip->kb2 = vsetq_lane_f32((float)(b2 * ra0), chip->kb2, 3);
        ra0 = -ra0;
        chip->ka1 = vsetq_lane_f32((float)(a1 * ra0), chip->ka1, 3);
        chip->ka2 = vsetq_lane_f32((float)(a2 * ra0), chip->ka2, 3);
    }
}


static void aymo_(apply_pp)(struct aymo_(chip)* chip)
{
    uint8_t pseudo_preset = (chip->reg_pp & 0x03u);
    if (pseudo_preset >= 3u) {
        pseudo_preset = 0u;
    }
    chip->pseudo_c1 = aymo_tda8425_pseudo_preset_c1[pseudo_preset];
    chip->pseudo_c2 = aymo_tda8425_pseudo_preset_c2[pseudo_preset];
}


uint8_t aymo_(read)(struct aymo_(chip)* chip, uint16_t address)
{
    assert(chip);

    switch (address) {
        case 0x00u: {
            return chip->reg_vl;
        }
        case 0x01u: {
            return chip->reg_vr;
        }
        case 0x02u: {
            return chip->reg_ba;
        }
        case 0x03u: {
            return chip->reg_tr;
        }
        case 0x07u: {
            return chip->reg_pp;
        }
        case 0x08u: {
            return chip->reg_sf;
        }
        default: {
            return 0xFFu;
        }
    }
}


void aymo_(write)(struct aymo_(chip)* chip, uint16_t address, uint8_t value)
{
    assert(chip);

    switch (address) {
        case 0x00u: {  // VL
            value |= 0xC0u;
            chip->reg_vl = value;
            aymo_(apply_vl)(chip);
            break;
        }
        case 0x01u: {  // VR
            value |= 0xC0u;
            chip->reg_vr = value;
            aymo_(apply_vr)(chip);
            break;
        }
        case 0x02u: {  // BA
            value |= 0xF0u;
            chip->reg_ba = value;
            aymo_(apply_ba)(chip);
            break;
        }
        case 0x03u: {  // TR
            value |= 0xF0u;
            chip->reg_tr = value;
            aymo_(apply_tr)(chip);
            break;
        }
        case 0x07u: {  // PP
            value |= 0xFCu;
            chip->reg_pp = value;
            aymo_(apply_pp)(chip);
            aymo_(apply_pseudo)(chip);
            break;
        }
        case 0x08u: {  // SF
            chip->reg_sf = value;
            aymo_(apply_source_mode)(chip);
            aymo_(apply_pseudo)(chip);
            aymo_(apply_tfilter)(chip);
            aymo_(apply_vl)(chip);
            aymo_(apply_vr)(chip);
            aymo_(apply_tr)(chip);
            break;
        }
    }
}


void aymo_(process_f32)(struct aymo_(chip)* chip, uint32_t count, const float x[], float y[])
{
    assert(chip);
    assert(x);
    assert(y);

    vf32x4_t b2l = chip->hb1l;
    vf32x4_t b2r = chip->hb1r;
    vf32x4_t a2l = chip->ha1l;
    vf32x4_t a2r = chip->ha1r;

    const float* xe = &x[count * 2u];

    while AYMO_LIKELY(x != xe) {
        vf32x4_t y2l = vaddq_f32(vmulq_f32(b2l, chip->kb2), vmulq_f32(a2l, chip->ka2));
        vf32x4_t y2r = vaddq_f32(vmulq_f32(b2r, chip->kb2), vmulq_f32(a2r, chip->ka2));
        chip->hb2l = b2l;
        chip->hb2r = b2r;
        chip->ha2l = a2l;
        chip->ha2r = a2r;

        vf32x4_t b1l = chip->hb0l;
        vf32x4_t b1r = chip->hb0r;
        vf32x4_t a1l = chip->ha0l;
        vf32x4_t a1r = chip->ha0r;
        vf32x4_t y1l = vaddq_f32(vmulq_f32(b1l, chip->kb1), vmulq_f32(a1l, chip->ka1));
        vf32x4_t y1r = vaddq_f32(vmulq_f32(b1r, chip->kb1), vmulq_f32(a1r, chip->ka1));
        chip->hb1l = b1l;
        chip->hb1r = b1r;
        chip->ha1l = a1l;
        chip->ha1r = a1r;

        vf32x4_t yyl = vaddq_f32(y2l, y1l);
        vf32x4_t yyr = vaddq_f32(y2r, y1r);

        vf32x2_t xlr = vld1_f32(x); x += 2u;
        vf32x2_t xrl = vrev64_f32(xlr);
        vf32x2_t wx = vadd_f32(vmul_f32(xlr, chip->klr), vmul_f32(xrl, chip->krl));
        vf32x4_t xx = vcombine_f32(wx, wx);

        vf32x4_t xl = vrev64q_f32(xx);
        vf32x4_t b0l = vextq_f32(xl, a1l, 3);
        vf32x4_t b0r = vextq_f32(xx, a1r, 3);
        yyl = vaddq_f32(yyl, vmulq_f32(b0l, chip->kb0));
        yyr = vaddq_f32(yyr, vmulq_f32(b0r, chip->kb0));
        chip->hb0l = b0l;
        chip->hb0r = b0r;

        chip->ha0l = yyl;
        chip->ha0r = yyr;

        vf32x2_t ylh = vget_high_f32(yyl);
        vf32x2_t yrh = vget_high_f32(yyr);
        vf32x2_t yy = vext_f32(ylh, vrev64_f32(yrh), 1);
        yy = vmul_f32(yy, chip->kv);

        b2l = chip->hb1l;
        b2r = chip->hb1r;
        a2l = chip->ha1l;
        a2r = chip->ha1r;

        vst1_f32(y, yy); y += 2u;
    }
}


AYMO_CXX_EXTERN_C_END

#endif  // AYMO_CPU_SUPPORT_ARM_NEON
